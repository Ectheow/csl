#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_
#include "stack.h"
#include "scanner.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

#define DEFINE "DEFINE"
#define ENDEF "ENDEF"
#define ELSE "ELSE"
#define PRINT "PRINT"
#define IF "IF"
#define ENDIF "ENDIF"

struct evaluator
{
  void (*eval_func) (void);
  struct stack *current;
  void *state_info;
  int in_def;
  int in_if;
};

extern struct evaluator evaluator; 
enum op_type
  {
    OP_FUNCTION,
    OP_STACK,
    OP_INVALID,
  };

struct op
{
  enum op_type type;
  char op_name[BUFSIZ];
  union
  {
    struct stack *definition;
    void (*carry_out_op)(void);
  };
};

struct dictionary
{
  struct dictionary *next;
  struct op *op;
};

void evaluator_init (struct stack *s);
void evaluator_cleanup (void);
void evaluate (void);
#define evaluator_die(msg, ...) error (1, msg, ##__VA_ARGS__)
#endif	/* _EVALUATOR_H_ */
