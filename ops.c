#include "ops.h"
#include "evaluator.h"
#include <stdio.h>
#include <stdlib.h>

struct stack *
__pop_atom (void);

void
pop_stack (void)
{
  pop_atom ();			/* pop the pop symbol */
  pop_atom ();
}

void
swap (void)
{
  struct stack *s1 = NULL, *s2 = NULL;

  pop_atom ();
  s1 = __pop_atom ();
  s2 = __pop_atom ();

  s2->next = s1;
  s1->next = stack_head;
  stack_head = s2;
}

void
rot (void)
{
  struct stack *s1 = NULL, *s2 = NULL, *s3 = NULL;

  pop_atom ();
  s3 = __pop_atom ();
  s2 = __pop_atom ();
  s1 = __pop_atom ();

  s1->next = s3;
  s3->next = s2;
  s2->next = stack_head;

  stack_head = s1;
}

void
twodup (void)
{
  struct stack *s1 = NULL, *s2 = NULL;
  struct atom *a1 = NULL, *a2 = NULL;

  pop_atom ();
  if (!(s1 = stack_head))
    evaluator_die ("stack underflow");
  if (!(s2 = s1->next))
    evaluator_die ("stack underflow");

  a1 = atom_copy (s1->atom);
  a2 = atom_copy (s2->atom);

  push_atom (a2);
  push_atom (a1);
}

void
nip (void)
{
  struct stack *s1 = NULL;

  pop_atom ();
  s1 = __pop_atom ();
  pop_atom ();
  s1->next = stack_head;
  stack_head = s1;
}

void
twoswap (void)
{
  struct stack *s1 = NULL,
    *s2 = NULL,
    *s3 = NULL,
    *s4 = NULL;

  pop_atom ();
  s1 = __pop_atom ();
  s2 = __pop_atom ();
  s3 = __pop_atom ();
  s4 = __pop_atom ();

  s2->next = stack_head;
  s4->next = s1;
  stack_head = s3;
}

struct stack *
__pop_atom (void)
{
  struct stack *s = stack_head;
  if (!stack_head)
    evaluator_die ("stack underflow");

  stack_head = s->next;
  return s;
}

void
dup (void)
{

  
  pop_atom ();
  if (!stack_head)
    evaluator_die ("stack underflow");
  push_atom (atom_copy (stack_head->atom));
}

void
__print_stack (struct stack *s)
{

  if (!s)
    return;
  else
    {
      __print_stack (s->next);
      if (s->atom->type == ATOM_FLOAT)
	printf ("%f ", s->atom->value_f);
      else if (s->atom->type == ATOM_SYMBOL)
	printf ("%s ", s->atom->value_sym);
      else if (s->atom->type == ATOM_INT)
	printf ("%d ", s->atom->value_i);
    }
}

void
print_stack (void)
{

  pop_atom ();			/* pop the PRINT operation */
  __print_stack (stack_head);
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
  pop_atom ();
  s1 = __pop_atom ();
  s2 = __pop_atom ();
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
    evaluator_die ("EVALUATOR ERROR: can't add two non-numbers");

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
  push_atom (a);
}

void
add_stack (void)
{
  do_arith (ADD);
}

void
sub_stack (void)
{
  do_arith (SUB);
}
  
void
push_atom (struct atom *a)
{
  struct stack *s = make_stack_elem (a);
  s->next = stack_head;
  stack_head = s;
}




void
pop_atom (void)
{
  struct stack *s = __pop_atom ();
  free_stack_elem (s);
}
