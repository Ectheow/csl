#include "ops.h"
#include "evaluator.h"
#include "list.h"
#include "atoms.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


list_item_t
__csl_pop (void);

void
__csl_push (list_item_t s)
{
  list_push (evaluator_list (),
	     s);
}

void
csl_push (atom_t a)
{
  __csl_push (make_list_item_from_atom (a));
}

list_item_t
__csl_pop (void)
{

  list_item_t elem = NULL;
  if (!list_size (evaluator_list ()))
    evaluator_die ("stack underflow");
  list_pop (evaluator_list (), &elem);
  return elem;
}

void
csl_pop_one (void)
{
  list_item_t li = __csl_pop ();
  list_item_free (evaluator_list (), li);
}

void
csl_pop (void)
{
  csl_pop_one ();
  csl_pop_one ();
}


void
csl_swap (void)
{
  list_item_t li1 = NULL, li2 = NULL;

  csl_pop_one ();
  li1 = __csl_pop ();
  li2 = __csl_pop ();

  __csl_push (li1);
  __csl_push (li2);
}

void
csl_rot (void)
{
  list_item_t li1 = NULL, li2 = NULL, li3 = NULL;

  csl_pop_one ();
  li3 = __csl_pop ();
  li2 = __csl_pop ();
  li1 = __csl_pop ();

  __csl_push (li2);
  __csl_push (li3);
  __csl_push (li1);
}

void
csl_twodup (void)
{
  list_item_t li1 = NULL, li2 = NULL;
  
  csl_pop_one ();
  if (! (li1 = list_head (evaluator_list ())))
    evaluator_die ("stack underflow");
  if (! (li2 = list_item_next (li1)))
    evaluator_die ("stack underflow");

  atom_t a1 = atom_copy (atom_from_list_item (li1));
  atom_t a2 = atom_copy (atom_from_list_item (li2));

  csl_push (a2);
  csl_push (a1);
}

void
csl_nip (void)
{
  list_item_t li1 = NULL;

  csl_pop_one ();
  li1 = __csl_pop ();
  csl_pop_one ();
  __csl_push (li1);
}

void
csl_twoswap (void)
{
  list_item_t li1 = NULL,
    li2 = NULL,
    li3 = NULL,
    li4 = NULL;

  csl_pop_one ();
  li1 = __csl_pop ();
  li2 = __csl_pop ();
  li3 = __csl_pop ();
  li4 = __csl_pop ();
  

  
  __csl_push (li2);
  __csl_push (li1);

  __csl_push (li4);  
  __csl_push (li3);
}



void
csl_dup (void)
{
  csl_pop_one ();
  if (list_empty (evaluator_list ()))
    evaluator_die ("stack underflow");
  csl_push (atom_copy (evaluator_peek ()));
}

void
__csl_print (list_t li)
{

  for (list_item_t it = list_tail (li);
       it;
       it = list_item_prev (it))
    atom_print (atom_from_list_item (it));
}

void
csl_print (void)
{

  csl_pop_one ();			/* pop the PRINT operation */
  __csl_print (evaluator_list ());
  printf ("\n");

}


enum arith_op
  {
    ADD,
    SUB,
  };

void
do_arith (enum arith_op op)
{
  list_item_t li1, li2;
  int i1, i2, is_int;
  double d1, d2;

  is_int = 0;
  csl_pop_one ();
  li1 = __csl_pop ();
  li2 = __csl_pop ();
  atom_t a1 = atom_from_list_item (li1);
  atom_t a2 = atom_from_list_item (li2);
  if (atom_is_int (a1)
      && atom_is_int (a2))
    {
      i1 = atom_int_value (a1);
      i2 = atom_int_value (a2);
      is_int = 1;
    }
  else if (atom_is_int (a1)
	   && atom_is_float (a2))
    {
      d1 = atom_float_value (a1);
      d2 = atom_float_value (a2);
    }
  else if (atom_is_int (a1))
    {
      d1 = atom_int_value (a1);
      d2 = atom_float_value (a2);
    }
  else if (atom_is_int (a2))
    {
      d1 = atom_float_value (a1);
      d2 = atom_int_value (a2);
    }
  else
    evaluator_die ("EVALUATOR ERROR: can't add two non-numbers %s and %s",
		   atom_to_str (a1),
		   atom_to_str (a2));

  atom_t a = NULL;
  switch (op)
    {
    case ADD:
      if (is_int)
	a = make_int_atom (i1 + i2);
      else
	a = make_float_atom (d1 + d2);
      break;
    case SUB:
      if (is_int)
	a = make_int_atom (i2 - i1);
      else
	a = make_float_atom (d2 - d1);
      break;
    }
  end:
  list_item_free (evaluator_list (), li1);
  list_item_free (evaluator_list (), li2);
  csl_push (a);
}

void
csl_plus (void)
{
  do_arith (ADD);
}

void
csl_sub (void)
{
  do_arith (SUB);
}
