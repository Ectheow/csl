#include "evaluator.h"
#include "ops.h"
#include "atoms.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
struct evaluator evaluator = {
  NULL,
  NULL,
  NULL,
  0,
  0
};

struct dictionary *dict = NULL; /* global dictionary */


static void __evaluate_definition (struct stack *s);
void evaluate_define (void);
void evaluate_definition (struct stack *s);

void free_op (struct op *);

int
compare_string_to_op (char *str,
		      op_t op)
{
  return !strcmp (str, op_name (op));
}

void
insert_op (struct op *o)
{
  list_push_back (evaluator_dictionary (),
		  make_list_item_from (o));
}

op_t
lookup_op_sym (struct atom *a)
{
 
  list_item_t found = list_find (evaluator_dictionary (),
				 atom_symbol_value (a),
				 (int (*)(void *, void *))compare_string_to_op);
  if (found)
    return (op_t)list_item_get_value (found);
  else
    return NULL;
}

void
evaluator_push_atom (atom_t a)
{
  evaluator_state () (a);
}


void
evaluator_do_op (op_t o)
{
  csl_pop_one ();
  for (list_item_t i = list_head (op_get_definition (o));
       i;
       i = list_item_next (i))
    evaluator_state_normal_push (atom_copy (atom_from_list_item (i)));
}


void
evaluator_state_normal_push (atom_t a)
{
  op_t op = NULL;
  list_push (evaluator_list (),
	     make_list_item_from_atom (a));
  if (atom_is_symbol (a))
    {
      if ( (op = lookup_op_sym (a)))
	{
	  if (op_is_function (op))
	    {
	      op_function (op) ();
	    }
	  else if (op_is_definition (op))
	    {
	      evaluator_do_op (op);
	    }
	}
      else
	evaluator_die ("symbol %s isn't defined",
		       atom_symbol_value (a));

    }
}


void
evaluator_state_definition_name (atom_t a)
{
  if (!atom_is_symbol (a))
    evaluator_die ("I need a symbol");

  op_t o = op_make (atom_symbol_value (a));
  op_set_type (o, OP_DEFINITION);
  op_set_definition (o, make_list ());
  evaluator_set_current_definition (o);
  evaluator_set_definition_state_body ();

}
void
csl_if (void)
{
  csl_pop_one ();
  atom_t a = atom_from_list_item (__csl_pop ());

  if (atom_is_int (a)
      && atom_int_value (a) == 0)
    evaluator_set_if_was_true (0);
  else
    evaluator_set_if_was_true (1);
  evaluator_set_if_body_state ();
  atom_free (a);
}

void
evaluator_state_if_body (atom_t a)
{
  if (! (atom_is_symbol (a)
	 && (atom_compare_to_string (a, ENDIF)
	     || atom_compare_to_string (a, ELSE))))
    {
      if (evaluator_if_was_true ())
	evaluator_state_normal_push (a);
      else
	atom_free (a);
    }
  else if ( (atom_is_symbol (a)
	     && atom_compare_to_string (a, ELSE)))
    {
      atom_free (a);
      evaluator_set_else_body_state ();
    }
  else if ( (atom_is_symbol (a)
	     && atom_compare_to_string (a, ENDIF)))
    {
      atom_free (a);
      evaluator_set_if_was_true (-1);
      evaluator_set_normal_state ();
    }
}

void
evaluator_state_else_body (atom_t a)
{
  if (! (atom_is_symbol (a)
	 && atom_compare_to_string (a, ENDIF)))

    {
      if (!evaluator_if_was_true ())
	evaluator_state_normal_push (a);
    }
  else if ( (atom_is_symbol (a)
	     && atom_compare_to_string (a, ENDIF)))
    {
      atom_free (a);
      evaluator_set_if_was_true (-1);
      evaluator_set_normal_state ();
    }
}

void
evaluator_state_definition_body (atom_t a)
{
  if (! (atom_is_symbol (a)
	 && atom_compare_to_string (a, ENDEF)))
    {
      list_push_back (op_get_definition (evaluator_get_current_definition ()),
		      make_list_item_from_atom (a));
      assert (op_get_definition (evaluator_get_current_definition ())->tail);
    }
  else
    {
      insert_op (evaluator_get_current_definition ());
      atom_free (a);
      evaluator_set_current_definition (NULL);
      evaluator_set_normal_state ();
    }
}

void
csl_define (void)
{
  csl_pop_one ();
  evaluator_set_definition_state ();
}

struct op *
make_stack_op (void)
{
  struct op *o = malloc (sizeof *o);

  o->type = OP_INVALID;
  memset (o->op_name, 0, sizeof o->op_name);
  o->definition = NULL;
  return o;
}

struct op *
make_atomic_op (char *name, void (*op)(void))
{
  struct op *o = malloc (sizeof *o);

  if (!o)
    evaluator_die ("EVALUATOR ERROR: can't allocate an operation");

  strcpy (o->op_name, name);
  o->type = OP_FUNCTION;
  o->carry_out_op = op;
  return o;
}

void
op_free (op_t o)
{
  if (!o)
    return;

  if (op_is_definition (o))
    list_free (op_get_definition (o));
  free (o);
}

void
evaluator_init (void)
{
   /* evaluator.eval_func = evaluate_stack; */
  evaluator.state_info = NULL;
  evaluator.in_def = 0;
  evaluator_set_current_definition (NULL);
  evaluator_set_normal_state ();
  /* evaluator.current = s; */
  evaluator.list = make_list ();
  evaluator.if_was_true = -1;
  evaluator.dictionary = make_list ();
  
  list_set_item_free_func (evaluator_list (),
			   (void (*)(void *))atom_free);
  list_set_item_free_func (evaluator_dictionary (),
			   (void (*)(void *))op_free);
  insert_op (make_atomic_op ("PRINT", csl_print));
  insert_op (make_atomic_op ("+", csl_plus));
  insert_op (make_atomic_op ("-", csl_sub));
  insert_op (make_atomic_op ("DEFINE", csl_define));
  insert_op (make_atomic_op ("POP", csl_pop));
  insert_op (make_atomic_op ("IF", csl_if));
  insert_op (make_atomic_op ("DUP", csl_dup));
  insert_op (make_atomic_op ("SWAP", csl_swap));
  insert_op (make_atomic_op ("ROT", csl_rot));
  insert_op (make_atomic_op ("TWOSWAP", csl_twoswap));
  insert_op (make_atomic_op ("TWODUP", csl_twodup));
  insert_op (make_atomic_op ("NIP", csl_nip));
  
  
}

void
evaluator_cleanup ()
{
  list_free (evaluator_list ());
  list_free (evaluator_dictionary ());
}
