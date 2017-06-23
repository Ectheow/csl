#include "../list.h"
#include "test.h"
#include <stdlib.h>
#include <assert.h>

int
main ()
{
  list_t l = make_list ();

  list_push (l, int_list_item (1));
  list_insert (l, list_item_at (l, 0), int_list_item (0));

  TEST_OK (0 == int_list_item_get (list_item_at (l, 0)),
	   "item get 0");
  TEST_OK (1 == int_list_item_get (list_item_at (l, 1)),
	   "item get 1");

  list_del (l, list_item_at (l, 0));
  TEST_OK (1 == int_list_item_get (list_item_at (l, 0)),
	   "item get 0 after delete");

  list_push (l, int_list_item (2));
  list_push (l, int_list_item (3));
  
  TEST_OK (3 == list_size (l),
	   "list size");
  printf ("OK size\n");
  
  TEST_OK (l->head == list_item_at (l, 0),
	   "HEAD is in the right place");
  TEST_OK (l->tail == list_item_at (l, 2),
	   "TAIL is in the right place");
  
  TEST_OK (2 == int_list_item_get (list_item_at (l, 1)),
	   "list item get 2");
  TEST_OK (1 == int_list_item_get (list_item_at (l, 2)),
	   "list item get 1");
  TEST_OK (3 == int_list_item_get (list_item_at (l, 0)),
	   "list item get 0");

  list_push (l, int_list_item (4));
  list_push (l, int_list_item (5));
  list_push (l, int_list_item (6));

  int item_is_equal (void *, long);
  
  TEST_EQ (list_item_at (l, 5),
	   list_find (l, (void *)1UL,
		      (int (*)(void *, void *))item_is_equal),
	   "%p",
	   "Search worked");

  TEST_EQ (list_item_at (l, 0),
	   list_find (l, (void *)6UL,
		      (int (*)(void *, void *))item_is_equal),
	   "%p",
	   "search 2 worked");
  list_free (l);

  l = make_list ();

  list_push_back (l, int_list_item (1));
  list_push_back (l, int_list_item (2));
  TEST_EQ (2,
	   int_list_item_get (list_item_at (l, 1)),
	   "%d",
	   "list item 1 after push_back");
  list_push_back (l, int_list_item (3));
  TEST_EQ (3,
	   int_list_item_get (list_item_at (l, 2)),
	   "%d",
	   "list item 2 after push_back");
  list_push_back (l, int_list_item (4));
  TEST_EQ (4,
	   int_list_item_get (list_tail (l)),
	   "%d",
	   "tail list item is 4 after push_back");
  
  TEST_EQ (4,
	   list_size (l),
	   "%d",
	   "list size pre-cut");

  list_t cut;
  cut = list_cut (l, list_item_at (l, 1),
		  list_item_at (l, 2));

  TEST_EQ (2,
	   list_size (cut),
	   "%d",
	   "cut list size");

  TEST_EQ (2,
	   int_list_item_get (list_item_at (cut, 0)),
	   "%d",
	   "first list item is 2 in the cut list");
  TEST_EQ (3,
	   int_list_item_get (list_item_at (cut, 1)),
	   "%d",
	   "second list item is 3 in the cut list");
  list_free (cut);
  TEST_EQ (2,
	   list_size (l),
	   "%d",
	   "list size after a cut");

  TEST_EQ (4,
	   int_list_item_get (list_item_at (l, 1)),
	   "%d",
	   "2nd list item dereferenced, after cut");
  TEST_EQ (1,
	   int_list_item_get (list_item_at (l, 0)),
	   "%d",
	   "1st list item dereferenced after cut");
}

int
item_is_equal (void *looking_for,
	       long x)
{
  return (long)looking_for == x;
}
