#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <execinfo.h>
#include <list.h>

#define array_size(arr) (sizeof(arr) / sizeof(arr[0]))
#define _RET_IP_        (unsigned long)__builtin_return_address(0)
#define _THIS_IP_       ({ __label__ __here; __here: (unsigned long)&&__here; })

#define min(x, y) ({              \
        typeof(x) __x = x;        \
        typeof(y) __y = y;        \
        __x > __y ? __x : __y; }) \

static LIST_HEAD(memset_track_list);

struct track_mem {
        struct list_head list_head;
        uintptr_t addr;
        unsigned long size;
};

void memset_track_register(uintptr_t addr, unsigned long size)
{
        struct track_mem *track = malloc(sizeof(struct track_mem));
        if (!track)
                return;

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

void print_stack_trace()
{
    void *buffer[10];
    int size = backtrace(buffer, 10);
    
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
                        fprintf(stderr, "ERROR: memset_s hit the track memory at RET_IP = 0x%lx\n", _RET_IP_);
                        print_stack_trace();
                }
        }

        return memset_s(dest, size, 0xfe, count);
}

int main()
{
        uint8_t buf[16];
        uint8_t tracked_mem[16];

        memset_track_register((uintptr_t)&tracked_mem, sizeof(tracked_mem));

        memset_s(buf, sizeof(buf) + 2, 0x00, sizeof(buf) + 2);

        printf("buf: ");

        for (int i = 0; i < array_size(buf); i++) {
                printf("%02x ", buf[i]);
        }

        printf("\n");

        printf("tracked_mem: ");

        for (int i = 0; i < array_size(tracked_mem); i++) {
                printf("%02x ", tracked_mem[i]);
        }

        printf("\n");

        return 0;
}