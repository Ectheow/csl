#ifndef _LIST_H_
#define _LIST_H_
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct list
{
  struct list_item *head;
  struct list_item *tail;
  void (*free_item)(void *);
};
typedef struct list *list_t;

struct list_item
{
  void *elem;
  struct list_item *next;
  struct list_item *prev;
};
typedef struct list_item *list_item_t;

struct list_item *list_item_at (struct list *, int);
static int
list_empty (list_t l)
{
  return (l->head == l->tail
	  && l->tail == NULL);
}

void list_push (struct list *, struct list_item *);
void list_push_back (list_t, list_item_t);
void list_pop (struct list *, struct list_item **);
void list_insert (struct list *, struct list_item *, struct list_item *);
list_t list_cut (list_t, struct list_item *, struct list_item *);
void list_del (struct list *, struct list_item *);
struct list_item *list_find (struct list *, void *, int (*)(void *, void *));

static inline list_item_t
list_head (list_t l)
{
  return l->head;
}

static inline list_item_t
list_tail (list_t l)
{
  return l->tail;
}

static inline list_item_t
list_prev (list_item_t l)
{
  return l->prev;
}

static inline list_item_t
list_next (list_item_t l)
{
  return l->next;
}

size_t list_size (struct list *);
void list_free (list_t l);

void list_item_free (list_t l, list_item_t li);

static inline void
list_set_item_free_func (list_t l, void (*free_item)(void *))
{
  l->free_item = free_item;
}

static inline list_item_t
list_item_next (list_item_t li)
{
  assert (li);
  return li->next;
}

static inline list_item_t
list_item_prev (list_item_t li)
{
  assert (li);
  return li->prev;
}

static list_t
make_list (void)
{
  list_t l = malloc (sizeof *l);
  l->free_item = NULL;
  l->head = l->tail = NULL;
  return l;
}

static struct list_item *
make_list_item (void)
{
  struct list_item *it = malloc (sizeof *it);
  
  assert (it);
  it->next = it->prev = NULL;
  it->elem = NULL;

  return it;
}

static list_item_t
make_list_item_from (void *value)
{
  list_item_t it = make_list_item ();
  it->elem = value;
  return it;
}

void *list_item_get_value (list_item_t);
int int_list_item_get (struct list_item *it);
list_item_t int_list_item (int elem);

#endif 
