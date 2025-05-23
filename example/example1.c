// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2011-2017 by Guangbo Cui <2407018371@qq.com>
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <execinfo.h>
#include <list.h>
#include <min_max.h>
#include <memset_track.h>
#include <array_size.h>
#include <instruction_pointer.h>

static LIST_HEAD(memset_track_list);

void memset_track_register(const char *name, uintptr_t addr, unsigned long size)
{
        struct track_mem *track = malloc(sizeof(struct track_mem));
        if (!track)
                return;

        track->name = name;
        track->addr = addr;
        track->size = size;
        list_add(&track->list_head, &memset_track_list);
}

void memset_track_unregister(uintptr_t addr)
{
        struct track_mem *pos, *tmp;

        list_for_each_entry_safe(pos, tmp, &memset_track_list, list_head) {
                if (pos->addr != addr)
                        continue;

                list_del(&pos->list_head);
                free(pos);
        }
}

static void print_stack_trace()
{
    void *buffer[30] = { NULL };
    int size = backtrace(buffer, ARRAY_SIZE(buffer));
    
    backtrace_symbols_fd(buffer, size, STDOUT_FILENO);
}

static inline bool is_overlapping(uintptr_t start1, size_t size1, uintptr_t start2, size_t size2) {
    uintptr_t end1 = start1 + size1;
    uintptr_t end2 = start2 + size2;

    return !(end1 <= start2 || end2 <= start1);
}

void *memset_s(void *dest, int size, int c, unsigned long count)
{
        return memset(dest, c, count);
}

void *memset_track_s(void *dest, int size, int c, unsigned long count)
{
        struct track_mem *pos = NULL, *tmp = NULL;
        uintptr_t addr = (uintptr_t)dest;

        list_for_each_entry_safe(pos, tmp, &memset_track_list, list_head) {
                if (is_overlapping(addr, min(size, count), pos->addr, pos->size)) {
                        fprintf(stderr, "ERROR: memset_s overlaps the track memory [%s] at IP = 0x%lx\n",
                                pos->name, _RET_IP_);
                        print_stack_trace();
                }
        }

        return memset_s(dest, size, c, count);
}

int main()
{
        uint8_t buf[16];
        uint8_t tracked_mem[16];

        memset_track_register("tracked_mem", (uintptr_t)&tracked_mem, sizeof(tracked_mem));

        memset_s(buf, sizeof(buf) + 2, 0x00, sizeof(buf) + 2);

        printf("buf:         ");

        for (int i = 0; i < ARRAY_SIZE(buf); i++) {
                printf("%02x ", buf[i]);
        }

        printf("\n");

        printf("tracked_mem: ");

        for (int i = 0; i < ARRAY_SIZE(tracked_mem); i++) {
                printf("%02x ", tracked_mem[i]);
        }

        printf("\n");

        return 0;
}