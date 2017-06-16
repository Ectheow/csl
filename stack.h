#ifndef _STACK_H_
#define _STACK_H_
#include "scanner.h"

struct stack
{
  struct atom *atom;
  struct stack *next;
};

void free_stack_elem (struct stack *s);
void free_stack (struct stack *s);
#endif
