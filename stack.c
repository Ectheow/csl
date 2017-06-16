#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
void
free_stack (struct stack *stack_head)
{
  if (!stack_head)
    return;
  
  free_stack (stack_head->next);  
  free_stack_elem (stack_head);
  stack_head = NULL;
}

struct stack *
make_stack_elem (struct atom *a)
{
  struct stack *s = malloc (sizeof *s);
  
  s->next = NULL;
  s->atom = NULL;
  if (!s)
    evaluator_die ("EVALUATOR ERROR: stack overflow: can't allocate a stack element");
  else
    s->atom = a;
  return s;
}

struct atom *
atom_copy (struct atom *in)
{
  struct atom *a = malloc (sizeof *a);

  if (!a)
    evaluator_die ("atom_copy: cannot make a new atom, no memory");
  if (in->type == ATOM_SYMBOL)
    {
      a->type = ATOM_SYMBOL;
      a->value_sym = strdup (in->value_sym);
    }
  else if (in->type == ATOM_FLOAT)
    {
      a->type = ATOM_FLOAT;
      a->value_f = in->value_f;
    }
  else if (in->type == ATOM_INT)
    {
      a->type = ATOM_INT;
      a->value_i = in->value_i;
    }
  else
    evaluator_die ("atom_copy: invalid type for atom");
  return a;
}

struct stack *
stack_elem_deepcopy (struct stack *in)
{
  struct stack *s = malloc (sizeof *s);

  s->next = NULL;
  s->atom = atom_copy (in->atom);
  return s;
}

struct stack *
stack_copy (struct stack *in)
{
  struct stack *out = NULL, *outp = NULL;

  if (!in)
    return in;
  
  out = stack_elem_deepcopy (in);
  out->next = stack_copy (in->next);
  
  return out;
}





void
free_stack_elem (struct stack *s)
{
  free_atom (s->atom);
  free (s);
}
