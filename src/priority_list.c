#include "priority_list.h"

struct priority_list_item_t {
    void *n;
    struct priority_list_item_t *prev;
    struct priority_list_item_t *next;
    int priority;
};

struct priority_list_t {
    priority_list_item_t *first;
    priority_list_item_t *last;
    size_t size;
};

void priority_list_init(priority_list_t **list)
{
  *list = (priority_list_t *)calloc(1, sizeof(priority_list_t));
}

void priority_list_deinit(priority_list_t *list)
{
  priority_list_clear(list);
  free(list);
}

void priority_list_clear(priority_list_t *list)
{
  priority_list_item_t *tmp; 
  priority_list_item_t *current = list->first;
  while(current) {
    tmp = current;
    current = current->next;
    free(tmp);
  }
  list->first = NULL;
  list->last = NULL;
  list->size = 0;
}

void priority_list_copy(priority_list_t *dest, const priority_list_t *src)
{
  priority_list_item_t *current = src->first;
  priority_list_item_t *prev = NULL;
  priority_list_item_t *new = NULL;
  
  if (!current)
      return;

  priority_list_clear(dest);
  new = (priority_list_item_t *)calloc(1, sizeof(priority_list_item_t));
  dest->first = new;
  do {
      dest->last = new;
      new->n = current->n;
      new->priority = current->priority;
      new->prev = prev;
      new->next = NULL;
      if (prev)
          prev->next = new;

      current = current->next;
      prev = new;
      dest->size++;
  } while (current && (new = (priority_list_item_t *)calloc(1, sizeof(priority_list_item_t))));
}

void* priority_list_erase(priority_list_iterator_t *it)
{
  priority_list_item_t *prev, *next;
  void *ret = NULL;
  prev = it->item->prev;
  next = it->item->next;
  if(prev) {
    prev->next = next;
  } else {
    it->list->first = next;
  }
  if(next) {
    next->prev = prev;
  } else {
    it->list->last = prev;
  }
  it->list->size--;
  free(it->item);
  it->item = next;
  if(it->item) {
    ret = it->item->n;
  }
  return ret;
}

void* priority_list_first(priority_list_t *list, priority_list_iterator_t *it)
{
  void *ret = NULL;
  it->list = list;
  it->item = list->first;
  if(it->item) {
    ret = it->item->n;
  }
  return ret;
}

void* priority_list_last(priority_list_t *list, priority_list_iterator_t *it)
{
  void *ret = NULL;
  it->list = list;
  it->item = list->last;
  if(it->item) {
    ret = it->item->n;
  }
  return ret;
}

void* priority_list_next(priority_list_iterator_t *it)
{
  void *ret = NULL;
  if(it->item) {
    it->item = it->item->next;
  }
  if(it->item) {
    ret = it->item->n;
  }
  return ret;
}

void* priority_list_prev(priority_list_iterator_t *it)
{
  void *ret = NULL;
  if(it->item) {
    it->item = it->item->prev;
  }
  if(it->item) {
    ret = it->item->n;
  }
  return ret;
}

void* priority_list_get(priority_list_iterator_t *it)
{
    if (it->item)
        return (it->item->n);
    else
        return NULL;
}

size_t priority_list_size(priority_list_t *list)
{
    return list->size;
}

bool priority_list_find(priority_list_t *list, const void *obj, priority_list_iterator_t *it)
{
  void *current = priority_list_first(list, it);
  while(current && current != obj) {
    current = priority_list_next(it);
  }
  return current != NULL;
}

int priority_list_get_priority(priority_list_iterator_t *it)
{
    return it->item->priority;
}
/*INSERT:
 * * Try to find obj
 * * If it exists, update priority and re-sort
 * * If it doesnt exist, create item and insert at end of same priority lvl
 * This is O(N)
 * */
void priority_list_insert(priority_list_t *list, void *obj, const int priority, priority_list_iterator_t *it)
{
    priority_list_item_t *prev, *next, *new;

    if (priority_list_find(list, obj, it))
    {
        /*Object already exists: Update priority and re-insert*/
        new = it->item;
        new->priority = priority;
        prev = it->item->prev;
        next = it->item->next;
        /*Remove at old position*/
        if (prev)
            prev->next = it->item->next;
        else
            list->first = it->item->next;
        if (next)
            next->prev = it->item->prev;
        else
            list->last = it->item->prev;
    } else {
        /*Object doesnt exit: Create and insert*/
        new = (priority_list_item_t *)calloc(1, sizeof(priority_list_item_t));
        new->n = obj;
        new->priority = priority;
        list->size++;
    }

    prev = NULL;
    next = list->first;
    /*Find place to insert*/
    while (next && next->priority <= priority) {
        prev = next;
        next = next->next;
    }
    if (next)
    {
        /*Insert*/
        new->prev = prev;
        new->next = next;
        if (prev)
            prev->next = new;
        else
            list->first = new;
        next->prev = new;
    } else {
        if (prev)
        {
            /*Insert at back*/
            new->prev = prev;
            new->next = NULL;
            prev->next = new;
            list->last = new;
        } else {
            /*Insert in empty list*/
            new->prev = NULL;
            new->next = NULL;
            list->last = new;
            list->first = new;
        }
    }

    /*Update iterator*/
    it->item = new;
    it->list = list;
}
