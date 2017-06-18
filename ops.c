#include "ops.h"
#include "evaluator.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


struct stack *
__csl_pop (void);

void
__csl_push (struct stack *s)
{
  evaluator.current = stack_splice (evaluator.current, s);
  /* evaluator.current = evaluator.current->prev; */
}

void
csl_push (struct atom *a)
{
  __csl_push (make_stack_elem (a));
}

struct stack *
__csl_pop (void)
{

  struct stack *out = NULL;
  if (!evaluator.current)
    evaluator_die ("stack underflow");

  evaluator.current = __stack_snip (evaluator.current, &out);
  return out;
}

void
csl_pop_one (void)
{
  struct stack *s = __csl_pop ();
  free_stack_elem (s);
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
  struct stack *s1 = NULL, *s2 = NULL;

  csl_pop_one ();
  s1 = __csl_pop ();
  s2 = __csl_pop ();

  __csl_push (s1);
  __csl_push (s2);
}

void
csl_rot (void)
{
  struct stack *s1 = NULL, *s2 = NULL, *s3 = NULL;

  csl_pop_one ();
  s3 = __csl_pop ();
  s2 = __csl_pop ();
  s1 = __csl_pop ();

  __csl_push (s2);
  __csl_push (s3);
  __csl_push (s1);
}

void
csl_twodup (void)
{
  struct stack *s1 = NULL, *s2 = NULL;
  struct atom *a1 = NULL, *a2 = NULL;

  csl_pop_one ();
  if (!(s1 = evaluator.current))
    evaluator_die ("stack underflow");
  if (!(s2 = s1->next))
    evaluator_die ("stack underflow");

  a1 = atom_copy (s1->atom);
  a2 = atom_copy (s2->atom);

  csl_push (a2);
  csl_push (a1);
}

void
csl_nip (void)
{
  struct stack *s1 = NULL;

  csl_pop_one ();
  s1 = __csl_pop ();
  csl_pop_one ();
  __csl_push (s1);
}

void
csl_twoswap (void)
{
  struct stack *s1 = NULL,
    *s2 = NULL,
    *s3 = NULL,
    *s4 = NULL;

  csl_pop_one ();
  s1 = __csl_pop ();
  s2 = __csl_pop ();
  s3 = __csl_pop ();
  s4 = __csl_pop ();
  
  __csl_push (s4);  
  __csl_push (s3);
  
  __csl_push (s2);
  __csl_push (s1);
}



void
csl_dup (void)
{
  csl_pop_one ();
  if (!evaluator.current)
    evaluator_die ("stack underflow");
  csl_push (atom_copy (evaluator.current->atom));
}

void
__csl_print (struct stack *s)
{

  if (!s)
    return;
  else
    {
      __csl_print (s->next);
      if (s->atom->type == ATOM_FLOAT)
	printf ("%f ", s->atom->value_f);
      else if (s->atom->type == ATOM_SYMBOL)
	printf ("%s ", s->atom->value_sym);
      else if (s->atom->type == ATOM_INT)
	printf ("%d ", s->atom->value_i);
    }
}

void
csl_print (void)
{

  csl_pop_one ();			/* pop the PRINT operation */
  __csl_print (evaluator.current);
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
  struct stack *s1 = NULL, *s2 = NULL;
  struct atom *a;
  double d1, d2;
  int i1, i2, is_int;

  is_int = 0;
  csl_pop_one ();
  s1 = __csl_pop ();
  s2 = __csl_pop ();
  if (s1->atom->type == ATOM_INT
      && s2->atom->type == ATOM_INT)
    {
      i1 = s1->atom->value_i;
      i2 = s2->atom->value_i;
      is_int = 1;
    }
  else if (s1->atom->type == ATOM_FLOAT
	   && s2->atom->type == ATOM_FLOAT)
    {
      d1 = s1->atom->value_f;
      d2 = s2->atom->value_f;
    }
  else if (s1->atom->type == ATOM_INT)
    {
      d1 = s1->atom->value_i;
      d2 = s2->atom->value_f;
    }
  else if (s2->atom->type == ATOM_INT)
    {
      d1 = s1->atom->value_f;
      d2 = s2->atom->value_i;
    }
  else
    evaluator_die ("EVALUATOR ERROR: can't add two non-numbers %s and %s",
		   atom_to_str (s1->atom),
		   atom_to_str (s2->atom));

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
  free_stack_elem (s1);
  free_stack_elem (s2);
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
