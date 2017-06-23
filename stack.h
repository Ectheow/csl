#ifndef _STACK_H_
#define _STACK_H_
#include "scanner.h"
#include "evaluator.h"
#include "error.h"


void free_stack_elem (struct stack *s);
void free_stack (struct stack *s);

static inline struct stack *
stack_make (void)
{
  return NULL;
}


struct stack *stack_push_atom (struct stack *, struct atom *);
struct stack *stack_push (struct stack *, struct stack *);
struct stack* stack_pop (struct stack *);
struct stack *__stack_pop (struct stack *, struct stack **);
struct stack *__stack_snip (struct stack *, struct stack **);
struct stack *stack_snip (struct stack *);
struct stack *stack_splice (struct stack *, struct stack *);
struct stack *stack_copy (struct stack *);
struct stack *make_stack_elem (struct atom *);
struct stack *stack_splice_out (struct stack *, struct stack *);
struct stack *stack_head (struct stack *);
struct stack *stack_tail (struct stack *);
#define stack_die(msg, ...) error (1, msg, ##__VA_ARGS__)
#endif
