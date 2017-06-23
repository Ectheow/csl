#include "list.h"
#include <assert.h>

void
list_insert_after (list_t l,
		   list_item_t after,
		   list_item_t it)
{
  assert (it != after);
  assert (it);

  if (after)
    {
      if (after->next)
	after->next->prev = it;
      it->next = after->next;
      after->next = it;
      it->prev = after;
    }
  else
    {
      assert (!l->head && !l->tail);
      it->next = it->prev = NULL;
    }

  if (after == l->tail)
    l->tail = it;
  if (!l->head)
    {
      assert (!after);
      l->head = it;
    }
}

void
list_insert (struct list *l,
	     struct list_item *before,
	     struct list_item *it)
{
  assert (it != before);

  if (before)
    {
      if (before->prev)
	before->prev->next = it;
      it->prev = before->prev;
      before->prev = it;
      it->next = before;
    }
  else
    {
      assert (!l->head && !l->tail);
      it->next = it->prev = NULL;
    }



  if (before == l->head)
    l->head = it;
  if (!l->tail)
    {
      assert (!before);
      l->tail = it;
    }
}

struct list_item *
list_item_at (struct list *l,
	      int index)
{
  struct list_item *it = NULL;
  int i;
  
  for (i=0,it=l->head; i < index && it; it=it->next,i++)
    ;
  assert (it);

  return it;
}

struct list_item *
list_find (struct list *l,
	   void *cmp,
	   int (*is_right)(void *, void *))
{
  struct list_item *it = NULL;
  for (it=l->head; it; it=it->next)
    if (is_right (cmp, it->elem))
      return it;
  return NULL;
}

size_t
list_size (struct list *l)
{
  size_t i;
  struct list_item *it=NULL;
  
  for (i=0,it=l->head; it; it = it->next,i++)
    ;
  return i;
}

void
list_del (struct list *l, struct list_item *it)
{
  if (it->prev)
    it->prev->next = it->next;
  if (it->next)
    it->next->prev = it->prev;

  if (it == l->head)
    {
      assert (!it->prev);
      l->head = it->next;
    }
  if (it == l->tail)
    {
      assert (!it->next);
      l->tail = it->prev;
    }
}

void
list_pop (struct list *l, struct list_item **it)
{
  *it = list_item_at (l, 0);
  list_del (l, *it);
}

void
list_push (struct list *l, struct list_item *it)
{
  list_insert (l, l->head, it);
}

void
list_push_back (list_t l, list_item_t it)
{
  list_insert_after (l, l->tail, it);
}

static list_t
__list_from_range (list_item_t start,
		   list_item_t end)
{
  list_t l = make_list ();

  l->head = start;
  l->tail = end;
  return l;
}

list_t
list_cut (struct list *l,
	  struct list_item *start,
	  struct list_item *end)
{
  assert (start);
  assert (end);
  list_item_t startprev = start->prev;
  list_item_t endnext = end->next;

  start->prev = NULL;
  end->next = NULL;
  if (endnext)
    endnext->prev = startprev;
  if (startprev)
    startprev->next = endnext;

  return __list_from_range (start, end);
}

void
list_item_free (list_t l, list_item_t li)
{
  if (l->free_item)
    l->free_item (li->elem);
  free (li);
}

void
list_free (list_t l)
{
  list_item_t it;
  for (it = l->head; it; it = it->next)
    list_item_free (l, it);
}

void *
list_item_get_value (list_item_t it)
{
  return it->elem;
}

int
int_list_item_get (struct list_item *it)
{

  return (int)(long)(it->elem);
}

struct list_item *
int_list_item (int elem)
{
  struct list_item *i = make_list_item ();
  i->elem = (void *)(long)elem;
  return i;
}
