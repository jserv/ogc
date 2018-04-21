#include <stdio.h>
#include "gc_internal.h"

static inline gc_list_t *list_search(uintptr_t ptr, gc_list_t *e)
{
    for (; e; e = e->next) {
        if (ptr >= e->data.start && e->data.start + e->data.size >= ptr)
            return e;
    }
    return NULL;
}

gc_list_t *gc_ptr_index(uintptr_t ptr)
{
    if (ptr > __gc_object.max || ptr < __gc_object.min)
        return NULL;

    int h = HASH(ptr) % PTR_MAP_SIZE;
    gc_list_t *e;
    if ((e = list_search(ptr, __gc_object.ptr_map[h])))
        return e;

    for (int i = 0; ++i + h < PTR_MAP_SIZE || (h > i);) {
        if ((h > i) && (e = list_search(ptr, __gc_object.ptr_map[h - i])))
            return e;
        if ((h + i < PTR_MAP_SIZE) &&
            (e = list_search(ptr, __gc_object.ptr_map[h + i])))
            return e;
    }
    return NULL;
}

void gc_mark_stack(void)
{
    uint8_t tmp;
    gc_mark(__gc_object.stack_start, &tmp);
    for (gc_list_t *e = __gc_object.globals; e; e = e->next) {
        gc_mark((uint8_t *) (e->data.start),
                (uint8_t *) (e->data.start + e->data.size));
    }
}

void gc_dump_internals(void)
{
    puts("## GC internals ##");
    for (int i = -1; ++i < PTR_MAP_SIZE;) {
        gc_list_t *m = __gc_object.ptr_map[i];
        if (!m)
            continue;
        printf(" - GC.ptr_map[%d]:\n", i);
        for (; m; m = m->next) {
            printf("   { ptr: %p, size: %lu, marked: %d }\n",
                   (void *) m->data.start, m->data.size, m->data.marked);
        }
    }
}
