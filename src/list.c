#include "gc_internal.h"

bool gc_list_exist(gc_list_t *begin_list, uintptr_t ptr)
{
    while (begin_list) {
        if (begin_list->data.start == ptr)
            return true;
        begin_list = begin_list->next;
    }
    return false;
}

void gc_list_add(gc_list_t **begin_list, gc_ptr_t data)
{
    if (!begin_list)
        return;

    gc_list_t *elem = malloc(sizeof(gc_list_t));
    *elem = (gc_list_t){.data = data, .next = *begin_list};
    *begin_list = elem;
}

void gc_list_del(gc_list_t **begin_list, size_t idx)
{
    gc_list_t *node, *prev = NULL;
    if (!(node = *begin_list))
        return;

    for (size_t i = 0; node; i++) {
        if (i == idx) {
            if (prev)
                prev->next = node->next;
            else
                *begin_list = node->next;
            free(node);
            return;
        }
        prev = node;
        node = node->next;
    }
}
