#ifndef GC_INTERNAL_H
#define GC_INTERNAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "gc.h"

#define PTR_MAP_SIZE 64
#define HASH(ptr) ((uintptr_t) ptr >> 3)

struct __gc_ptr {
    uintptr_t start;
    size_t size;
    bool marked;
};
typedef struct __gc_ptr gc_ptr_t;

struct __gc_list {
    struct __gc_list *next;
    gc_ptr_t data;
};
typedef struct __gc_list gc_list_t;

struct __gc {
    void *stack_start;
    gc_list_t *ptr_map[PTR_MAP_SIZE];
    size_t ptr_num;
    size_t limit;
    size_t ref_count;
    uintptr_t min, max;
    gc_list_t *globals;
};
typedef struct __gc gc_t;

/* global GC object */
extern gc_t __gc_object;

void gc_mark(uint8_t *s, uint8_t *e);
void gc_sweep();
void gc_mark_stack(void);

gc_list_t *gc_ptr_index(uintptr_t ptr);
void gc_list_add(gc_list_t **begin_list, gc_ptr_t data);
void gc_list_del(gc_list_t **begin_list, size_t idx);
bool gc_list_exist(gc_list_t *begin_list, uintptr_t ptr);

static inline void gc_mfree(gc_list_t *e)
{
    free((void *) e->data.start);
    __gc_object.ptr_num--;
}

#endif
