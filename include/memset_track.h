/* SPDX-License-Identifier: GPL-2.0 */
#ifndef MEMSET_TRACK_H
#define MEMSET_TRACK_H

#include <stdint.h>
#include <list.h>

struct track_mem {
        struct list_head list_head;
        const char *name;
        uintptr_t addr;
        unsigned long size;
};

void memset_track_register(const char *name, uintptr_t addr, unsigned long size);
void memset_track_unregister(uintptr_t addr);

#endif /* LIST_TYPES_H */
