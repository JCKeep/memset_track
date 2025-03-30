/* SPDX-License-Identifier: GPL-2.0 */
#ifndef MIN_MAX_H
#define MIN_MAX_H

#define min(x, y)               \
({                              \
        typeof(x) __x = x;      \
        typeof(y) __y = y;      \
        __x < __y ? __x : __y;  \
})

#define max(x, y)               \
({                              \
        typeof(x) __x = x;      \
        typeof(y) __y = y;      \
        __x > __y ? __x : __y;  \
})

#endif /* LIST_TYPES_H */
