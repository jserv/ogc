#include "gc_internal.h"

gc_t __gc_object = (gc_t){.ref_count = 0};

void gc_init(void *ptr, size_t limit)
{
    if (__gc_object.ref_count) {
        __gc_object.ref_count++;
        return;
    }
    __gc_object = (gc_t){.stack_start = ptr,
                         .ptr_map = {NULL},
                         .ptr_num = 0,
                         .ref_count = 1,
                         .limit = limit,
                         .min = UINTPTR_MAX,
                         .max = 0,
                         .globals = NULL};
}

static inline void swap_ptr(uint8_t **a, uint8_t **b)
{
    uint8_t *tmp = *a;
    *a = *b;
    *b = tmp;
}

void gc_mark(uint8_t *start, uint8_t *end)
{
    if (start > end)
        swap_ptr(&start, &end);
    while (start < end) {
        gc_list_t *idx = gc_ptr_index((uintptr_t)(*((void **) start)));
        if (idx && idx->data.marked != true) {
            idx->data.marked = true;
            gc_mark((uint8_t *) (idx->data.start),
                    (uint8_t *) (idx->data.start + idx->data.size));
        }
        start++;
    }
}

void gc_sweep(void)
{
    for (int i = 0; ++i < PTR_MAP_SIZE;) {
        gc_list_t *e = __gc_object.ptr_map[i];
        int k = 0;
        while (e) {
            if (!e->data.marked) {
                gc_mfree(e);
                e = e->next;
                gc_list_del(&__gc_object.ptr_map[i], k);
            } else {
                e->data.marked = false;
                e = e->next;
            }
            k++;
        }
    }
}

void gc_run(void)
{
    gc_mark_stack();
    gc_sweep();
}

void gc_destroy(void)
{
    __gc_object.ref_count--;
    if (__gc_object.ref_count <= 0) {
        __gc_object.ref_count = 0;
        for (int i = -1; ++i < PTR_MAP_SIZE;) {
            gc_list_t *m = __gc_object.ptr_map[i];
            while (m) {
                gc_list_t *tmp = m;
                free((void *) (m->data.start));
                m = m->next;
                free(tmp);
            }
            __gc_object.ptr_map[i] = 0;
        }
    }
}
