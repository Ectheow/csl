#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_
#include "stack.h"
#include "scanner.h"
#include "error.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define DEFINE "DEFINE"
#define ENDEF "ENDEF"
#define ELSE "ELSE"
#define PRINT "PRINT"
#define IF "IF"
#define ENDIF "ENDIF"

typedef void (*evaluator_state_t)(atom_t);

enum op_type
  {
    OP_FUNCTION,
    OP_DEFINITION, 
    OP_INVALID,
  };


struct op
{
  enum op_type type;
  char op_name[BUFSIZ];
  union
  {
    list_t definition;
    void (*carry_out_op)(void);
  };
};
typedef struct op *op_t;

struct evaluator
{
  int if_was_true;
  evaluator_state_t evaluator_state;
  op_t current_definition;
  list_t list;
  list_t dictionary;
  void *state_info;
  int in_def;
  int in_if;
};

typedef struct evaluator *evaluator_t;
extern struct evaluator evaluator;

void evaluator_state_normal_push (atom_t);
void evaluator_state_definition_push (atom_t);
void evaluator_state_definition_name (atom_t);
void evaluator_state_definition_body (atom_t);


static inline evaluator_state_t
evaluator_state (void)
{
  return evaluator.evaluator_state;
}

static void
evaluator_set_current_definition (op_t o)
{
  evaluator.current_definition = o;
}

static op_t
evaluator_get_current_definition (void)
{
  return evaluator.current_definition;
}

static void
evaluator_set_definition_state (void)
{
  assert (evaluator.evaluator_state == evaluator_state_normal_push);
  evaluator.evaluator_state = evaluator_state_definition_name;
}

static void
evaluator_set_definition_state_body (void)
{
  assert (evaluator.evaluator_state == evaluator_state_definition_name);
  evaluator.evaluator_state = evaluator_state_definition_body;
}

static void
evaluator_set_normal_state (void)
{
  assert (evaluator.current_definition == NULL);
  evaluator.evaluator_state = evaluator_state_normal_push;
}

static inline op_t
op_make (char *name)
{
  op_t o = malloc (sizeof *o);

  assert (o);
  memset (o, 0, sizeof *o);
  strcpy (o->op_name, name);
  return o;
}

static void
op_set_type (op_t o, enum op_type type)
{
  assert (type < OP_INVALID);
  o->type = type;
}

static inline char *
op_name (op_t o)
{
  return o->op_name;
}

void op_free (op_t o);
static inline int
op_is_function (op_t o)
{
  return o->type == OP_FUNCTION;
}

static inline int
op_is_definition (op_t o)
{
  return o->type == OP_DEFINITION;
}

static inline list_t
op_get_definition (op_t o)
{
  assert (op_is_definition (o));
  return o->definition;
}

static void
op_set_definition (op_t o, list_t definition)
{
  o->definition = definition;
}

static inline void (*op_function (op_t o))(void)
{
  return o->carry_out_op;
}

void evaluator_init (void);
void evaluator_cleanup (void);
void evaluate (void);


struct evaluator *make_evaluator (list_t list);

static inline list_t
evaluator_dictionary (void)
{
  return evaluator.dictionary;
}

static inline list_t
evaluator_list (void)
{
  return evaluator.list;
}

static atom_t
evaluator_peek (void)
{
  return atom_from_list_item (list_item_at (evaluator_list (), 0));
}
void evaluator_state_if_body (atom_t);
static void
evaluator_set_if_body_state (void)
{
  assert (evaluator.evaluator_state == evaluator_state_normal_push);
  assert (evaluator.if_was_true == 0
	  || evaluator.if_was_true == 1);
  evaluator.evaluator_state = evaluator_state_if_body;
}
void evaluator_state_else_body (atom_t);

static void
evaluator_set_else_body_state (void)
{
  assert (evaluator.evaluator_state == evaluator_state_if_body);
  evaluator.evaluator_state = evaluator_state_else_body;
}

static void
evaluator_set_if_was_true (int v)
{
  if (v == 0 || v == 1)
    assert (evaluator.if_was_true == -1);
  else
    assert (evaluator.if_was_true == 0
	    || evaluator.if_was_true == 1);

  evaluator.if_was_true = v;
}

static int
evaluator_if_was_true (void)
{
  return evaluator.if_was_true == 1;
}
void evaluator_push_atom (atom_t);
void evaluator_evaluate (void);




#define evaluator_die(msg, ...) error (1, msg, ##__VA_ARGS__)
#endif	/* _EVALUATOR_H_ */
