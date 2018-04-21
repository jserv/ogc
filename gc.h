#ifndef GC_H
#define GC_H

#include <stddef.h>

void gc_init(void *ptr, size_t limit);
void *gc_alloc(size_t size);
void gc_free(void *ptr);
void gc_run();
void gc_destroy(void);

void gc_dump_internals(void);

#endif
