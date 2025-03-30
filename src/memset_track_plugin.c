// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2011-2017 by Guangbo Cui <2407018371@qq.com>
 *
 * Reference: [stackleak_plugin](https://elixir.bootlin.com/linux/v6.14-rc6/source/scripts/gcc-plugins/stackleak_plugin.c)
 */

#include <stdio.h>
#include <string.h>
#include <gcc-common.h>

#define MEMSET_TRACK_PLUGIN "memset_track_plugin"

#define T(t) t ## _type_node
#define MEMSET_S_TYPE build_function_type_list(T(ptr), \
        T(ptr), T(integer), T(integer), T(integer), NULL)

__visible int plugin_is_GPL_compatible = 1;
static const char memset_s_function[] = "memset_s";
static const char memset_track_s_function[] = "memset_track_s";
static bool verbose = false;

#define current_function DECL_NAME_POINTER(current_function_decl)

static bool is_memset_s(gimple stmt)
{
        tree callee = NULL;
        tree addr_expr_operand = NULL;
        const char *callee_name = NULL;

        callee = gimple_call_fn(stmt);
        if (!callee)
                return false;

        if (TREE_CODE(callee) == FUNCTION_DECL) {
                callee_name = DECL_NAME_POINTER(callee);
                if (callee_name && !strcmp(callee_name, memset_s_function)) {
                        return true;
                }
        }

        if (TREE_CODE(callee) == ADDR_EXPR) {
                addr_expr_operand = TREE_OPERAND(callee, 0);

                if (!addr_expr_operand)
                        return false;

                if (TREE_CODE(addr_expr_operand) == FUNCTION_DECL) {
                        callee_name = DECL_NAME_POINTER(addr_expr_operand);
                        if (callee_name && !strcmp(callee_name, memset_s_function)) {
                                return true;
                        }
                }
        }

        return false;
}

/*
 * Work with the GIMPLE representation of the code. Modify the
 * memset_s() call to memset_track_s() call.
 */
static unsigned int memset_track_execute(void)
{
        basic_block bb = NULL;
        gimple_stmt_iterator gsi = { 0 };

        static tree memset_track_decl = build_fn_decl(memset_track_s_function, MEMSET_S_TYPE);

        /*
         * Loop through the GIMPLE statements in each of cfun basic blocks.
         * cfun is a global variable which represents the function that is
         * currently processed.
         */
        FOR_EACH_BB_FN(bb, cfun) {
                for (gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
                        gimple stmt = gsi_stmt(gsi);

                        if (!is_gimple_call(stmt))
                                continue;

                        if (!is_memset_s(stmt))
                                continue;

                        if (!strcmp(current_function, memset_track_s_function))
                                continue;

                        if (verbose) {
                                fprintf(stderr, "===> [MEMSET_TRACKING] start, "
                                        "rewriting the AST node...\n"
                                        "Before: \n");
                                debug_gimple_stmt(stmt);
                        }

                        gimple_call_set_fndecl(stmt, memset_track_decl);

                        if (verbose) {
                                fprintf(stderr, "\nAfter: \n");
                                debug_gimple_stmt(stmt); 
                                fprintf(stderr, "<=== [MEMSET_TRACKING] finish.\n"); 
                        }

                        fprintf(stderr, "redirect memset_s to memset_track_s in %s()\n", current_function);
                }
        }

        return 0;
}

/*
 * Pass gate function is a predicate function that gets executed before the
 * corresponding pass. If the return value is 'true' the pass gets executed,
 * otherwise, it is skipped.
 */
static bool memset_track_gate(void)
{
        return true;
}

#define PASS_NAME memset_track
#define PROPERTIES_REQUIRED PROP_gimple_leh | PROP_cfg
#define TODO_FLAGS_START  TODO_verify_ssa | TODO_verify_flow  | TODO_verify_stmts
#define TODO_FLAGS_FINISH TODO_verify_ssa | TODO_verify_stmts | TODO_dump_func \
                        | TODO_update_ssa | TODO_rebuild_cgraph_edges
#include <gcc-generate-gimple-pass.h>

/*
 * Every gcc plugin exports a plugin_init() function that is called right
 * after the plugin is loaded. This function is responsible for registering
 * the plugin callbacks and doing other required initialization.
 */
__visible int plugin_init(struct plugin_name_args *plugin_info,
                          struct plugin_gcc_version *version)
{
        const int argc = plugin_info->argc;
        const struct plugin_argument * const argv = plugin_info->argv;
        int i = 0;

        /*
         * The memset_track pass should be executed before the
         * "optimized" pass, which is the control flow graph cleanup that is
         * performed just before expanding gcc trees to the RTL. In former
         * versions of the plugin this new pass was inserted before the
         * "tree_profile" pass, which is currently called "profile".
         */
        PASS_INFO(memset_track, "optimized", 1, PASS_POS_INSERT_BEFORE);

        for (i = 0; i < argc; i++) {
                if (!strcmp(argv[i].key, "verbose"))
                        verbose = true;
        }

        /*
         * Hook into the Pass Manager to register new gcc passes.
         *
         * The stack frame size info is available only at the last RTL pass,
         * when it's too late to insert complex code like a function call.
         * So we register two gcc passes to instrument every function at first
         * and remove the unneeded instrumentation later.
         */
        register_callback(MEMSET_TRACK_PLUGIN, PLUGIN_PASS_MANAGER_SETUP, NULL,
                    &memset_track_pass_info);

    return 0;
}
