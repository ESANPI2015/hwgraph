#ifndef _PRIORITY_LIST_H
#define _PRIORITY_LIST_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct priority_list_t priority_list_t;
typedef struct priority_list_item_t priority_list_item_t;
typedef struct priority_list_iterator_t {
    priority_list_t *list;
    priority_list_item_t *item;
} priority_list_iterator_t;

void priority_list_init(priority_list_t **list);
void priority_list_deinit(priority_list_t *list);
void priority_list_clear(priority_list_t *list);
void priority_list_copy(priority_list_t *dest, const priority_list_t *src);
void* priority_list_erase(priority_list_iterator_t *it);
void* priority_list_first(priority_list_t *list, priority_list_iterator_t *it);
void* priority_list_last(priority_list_t *list, priority_list_iterator_t *it);
void* priority_list_next(priority_list_iterator_t *it);
void* priority_list_prev(priority_list_iterator_t *it);
void* priority_list_get(priority_list_iterator_t *it);
size_t priority_list_size(priority_list_t *list);
bool priority_list_find(priority_list_t *list, const void *obj, priority_list_iterator_t *it);
int priority_list_get_priority(priority_list_iterator_t *it);

/*INSERT:
 * * Try to find obj
 * * If it exists, update priority and re-sort
 * * If it doesnt exist, create item and insert at end of same priority lvl
 * This is O(N)
 * */
void priority_list_insert(priority_list_t *list, void *obj, const int priority, priority_list_iterator_t *it);

#endif
