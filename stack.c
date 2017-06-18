#include "stack.h"
#include "scanner.h"
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
  s->prev = NULL;
  s->atom = NULL;
  if (!s)
    stack_die ("stack overflow: can't allocate a stack element");
  else
    s->atom = a;
  return s;
}

struct atom *
atom_copy (struct atom *in)
{
  struct atom *a = malloc (sizeof *a);

  if (!a)
    stack_die ("cannot make a new atom, no memory");
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
    evaluator_die ("invalid type %d for atom", in->type);
  
  return a;
}

struct stack *
stack_push_atom (struct stack *s, struct atom *a)
{
  return stack_push (s, make_stack_elem (a));
}

struct stack *
stack_push (struct stack *s, struct stack *pushon)
{

  if (s)
    {
      if (s->prev)
	stack_die ("cannot push into the middle of a stack");
      s->prev = pushon;
    }

  pushon->next = s;
  pushon->prev = NULL;
  return pushon;
}

struct stack *
stack_splice_out (struct stack *s_start,
		  struct stack *s_end)
{
  if (s_start->next)
    s_start->next->prev = NULL;
  if (s_end->prev)
    s_end->prev->next = NULL;
  
  s_start->next = s_end;
  s_end->prev = s_start;
  
  return s_start;
}

		  
struct stack *
stack_splice (struct stack *s, struct stack *splice)
{
  struct stack *splicee_head = splice;

  if (!s)
    return splice;

  if (!splice)
    stack_die ("null splice");
  
  while (splicee_head->prev)
    splicee_head = splicee_head->prev;
 
  splicee_head->prev = s->prev;
  if (s->prev)
    s->prev->next = splicee_head;

  s->prev = splice;
  splice->next = s;
  return s;
}

struct stack *
stack_snip (struct stack *s)
{
  struct stack *out = NULL, *ret = NULL;

  ret = __stack_snip (s, &out);
  free_stack_elem (out);
  return ret;
}

struct stack *
__stack_snip (struct stack *s, struct stack **out)
{
  struct stack *s_next = s->next;

  if (!s->prev)
    return __stack_pop (s, out);
  else
    {
      if (s->next)
	s->next->prev = s->prev;
      s->prev->next = s->next;
      s->next = NULL;
      s->prev = NULL;
      *out = s;
      return s_next;
    }
}

struct stack *
stack_pop (struct stack *s)
{
  struct stack *old = NULL, *new = NULL;

  new =  __stack_pop (s, &old);
  free_stack_elem (old);
  
  return new;
}

struct stack *
__stack_pop (struct stack *s, struct stack **out)
{
  struct stack *news = NULL;

  if (s->prev)
    stack_die ("invalid operation: pop on a non-top element");
  
  if (s->next)
      s->next->prev = NULL;
  news = s->next;
  s->next = NULL;
  *out = s;
  return news;
}

struct stack *
stack_elem_deepcopy (struct stack *in)
{
  struct stack *s = malloc (sizeof *s);

  if (!s)
    stack_die ("unable to allocate memory");
  
  s->next = NULL;
  s->prev = NULL;
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
  if (out->next)
    out->next->prev = out;
  
  return out;
}

struct stack *
stack_head (struct stack *in)
{
  while (in && in->prev) in = in->prev;
  return in;
}

struct stack *
stack_tail (struct stack *in)
{
  while (in && in->next) in = in->next;
  return in;
}

void
free_stack_elem (struct stack *s)
{
  free_atom (s->atom);
  memset (s, 0xab, sizeof *s);
  free (s);
}
