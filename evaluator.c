#include "evaluator.h"
#include "ops.h"
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
void csl_pop (void);

static void __evaluate_definition (struct stack *s);
void evaluate_define (void);
void evaluate_definition (struct stack *s);

void free_op (struct op *);

void
evaluate (void)
{
  while (evaluator.current
	 && evaluator.current->prev)
    evaluator.eval_func ();
  if (evaluator.current)
    {
      printf (">>> Exiting evaluate, %p", evaluator.current->prev);
      __csl_print (evaluator.current);
    }
  else
    printf (">>> exiting evaluato no current\n");
}

struct atom *
eval_current_atom (void)
{
  if (!evaluator.current)
    evaluator_die ("stack underflow");
  
  return evaluator.current->atom;
}

void
free_dictionary (struct dictionary *d)
{
  if (!d)
    return;
  free_dictionary (d->next);
  free_op (d->op);
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
free_op (struct op *o)
{
  if (!o)
    return;
  
  if (o->type == OP_STACK)
    free_stack (o->definition);
  free (o);
}

void
insert_op (struct op *o)
{
  struct dictionary *dent = malloc (sizeof *dent);

  if (!dent)
    evaluator_die ("EVALUATOR ERROR: can't allocate dent");
  dent->op = o;
  dent->next = dict;
  dict = dent;
}


struct op *
lookup_op_sym (struct atom *a)
{
  struct dictionary *d = dict;

  do
    {
      if (!strcmp (d->op->op_name, a->value_sym))
	return d->op;
      d = d->next;
    }
  while (d);
  
  return NULL;
  
}

void
evaluate_op (struct op *o)
{
  if (!o)
    evaluator_die ("null operator given to evaluate_op");
  if (o->type == OP_STACK)
    evaluate_definition (o->definition);
  else if (o->type == OP_FUNCTION)
    o->carry_out_op ();
  else
    evaluator_die ("bad operation type %d", o->type);
    
}

void
evaluate_symbol (void)
{
  struct op *o = NULL;

  printf ("symbol: %s\n",
  	  evaluator.current->atom->value_sym);
  /* __print_stack (); */
  if (! (o = lookup_op_sym (eval_current_atom ())))
    evaluator_die ("evaluate_symbol: can't find this atom '%s'",
		   evaluator.current->atom->value_sym);
  
  evaluate_op (o);
}

void
evaluate_stack (void)
{

  while (evaluator.current)
    {
      if (evaluator.in_if || evaluator.in_def)
	break;
      if (eval_current_atom ()->type == ATOM_SYMBOL)
	evaluate_symbol ();
      else if (evaluator.current->prev)
	evaluator.current = evaluator.current->prev;
      else if (!evaluator.current->prev)
	break;
    }
  printf (">>> outof stack current, prev %p %p \n", evaluator.current, evaluator.current->prev);
  /* printf (">>>\n"); */
  /* __csl_print (evaluator.current); */
  /* printf ("<<<\n"); */
}

void
evaluate_define (void)
{
  struct op *o = (struct op *)evaluator.state_info;
  struct stack *s = NULL;
  
  /* If we're at the end of the definition, then pop these values off
     onto the stack for their op. */
  /*
    stack picture
    ENDEF -> ATOM1 -> ... -> ATOMN -> NAME -> DEFINE -> (REST OF STACK)
  */

  printf ("eval define\n");
  printf (">>> definition...\n");
  if (!o)
    evaluator_die ("no allocated operation for the define");
  while (evaluator.current)
    {
      if (evaluator.current->atom->type == ATOM_SYMBOL
	  && !strcmp (evaluator.current->atom->value_sym, ENDEF))
	{
	  for (s = evaluator.current;
	       s && (s->atom->type != ATOM_SYMBOL
		     || strcmp (s->atom->value_sym, DEFINE));
	       s=s->next)
	    ;
      
	  if (!s)
	    evaluator_die ("un-terminated definition stack, the stack has been corrupted.");
	  if (s->prev->atom->type != ATOM_SYMBOL)
	    evaluator_die ("no name for defined value, definitions must have names");
	  
	  strcpy (o->op_name, s->prev->atom->value_sym);
	  o->definition = s->prev->prev;

	  o->type = OP_STACK;
	  evaluator.current = stack_splice_out (evaluator.current,
						s->prev);
	  
	  csl_pop_one ();
	  csl_pop_one ();
	  csl_pop_one ();
	  /* free_stack_elem (p); */
	  /* free_stack_elem (s); */
	  printf (">>> DEF\n");
	  __csl_print (stack_head (o->definition));
	  printf ("<<< DEF\n");
	  evaluator.eval_func = evaluate_stack;
	  insert_op (o);
	  evaluator.in_def = 0;
	}
      else if (evaluator.current->prev)
	evaluator.current = evaluator.current->prev;
      else if (!evaluator.current->prev)
	break;
    }

}

void
evaluate_if (void)
{
  struct stack *s = NULL,
    *p = NULL,
    *if_stack = NULL,
    *else_sym = NULL,
    *else_stack = NULL,
    *if_sym = NULL;
  int is_false;

  while (evaluator.current)
    {
      printf (">>> if eval\n");
      if (evaluator.current->atom->type == ATOM_SYMBOL
	  && !strcmp (evaluator.current->atom->value_sym, ENDIF))
	{
	  for (s = evaluator.current;
	       s && (s->atom->type != ATOM_SYMBOL
		     || strcmp (s->atom->value_sym, IF));
	       s = s->next)
	      if (s->atom->type == ATOM_SYMBOL
		  && !strcmp (s->atom->value_sym, ELSE))
		  else_sym = s;

	  if_sym = s;
	  if (!s)
	    evaluator_die ("empty 'if' stack");


	  if_stack = if_sym->prev;

	  if (else_sym)
	    {
	      else_stack = else_sym->prev;
	      stack_splice_out (evaluator.current, else_sym);
	      stack_splice_out (else_sym, if_sym);
	      csl_pop_one ();
	    }
	  else
	    {
	      else_stack = NULL;
	      stack_splice_out (evaluator.current, if_sym);
	    }
	  
	  csl_pop_one ();
	  csl_pop_one ();
	      
	  is_false = (evaluator.current->atom->type == ATOM_INT
		      && evaluator.current->atom->value_i == 0);
	  csl_pop_one ();
	  if (is_false)
	    {
	      /* printf (">>> else\n"); */
	      free_stack (if_stack);
	      if (else_stack)
		evaluator.current = stack_splice (evaluator.current, else_stack);
	    }
	  else
	    {
	      evaluator.current = stack_splice (evaluator.current, if_stack);
	      if (else_stack)
		free_stack (else_stack);
	    }
      
	  evaluator.in_if = 0;
	  evaluator.eval_func = evaluate_stack;
	  break;
	}
      else if (evaluator.current->prev)
	evaluator.current = evaluator.current->prev;
      else if (!evaluator.current->prev)
	break;
    }
}

void
csl_define (void)
{
  evaluator.state_info = make_stack_op ();
  evaluator.eval_func = evaluate_define; /* don't pop though */
  evaluator.in_def = 1;
}

void
csl_if (void)
{

  printf (">>> evaluate if symbol\n");
  evaluator.eval_func = evaluate_if;
  evaluator.state_info = NULL;
  evaluator.in_if = 1;
}

static void
__evaluate_definition (struct stack *s)
{
  printf (">>> eval def\n");
  __csl_print (s);
  evaluator.current = stack_splice (evaluator.current, s);
}

void
evaluate_definition (struct stack *s)
{
  csl_pop_one ();
  printf (">> EVAL\n");
  __csl_print (stack_head (s));
  printf ("<<\n");
  evaluator.current = stack_splice (evaluator.current,
				    stack_tail (stack_copy (stack_head (s))));
  printf (">>> TOTAL STACK\n");
  __csl_print (stack_head (evaluator.current));
  printf ("<<<\n");
}

void
evaluator_init (struct stack *s)
{
  evaluator.eval_func = evaluate_stack;
  evaluator.state_info = NULL;
  evaluator.in_def = 0;
  evaluator.current = s;
  
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
  free_stack (evaluator.current);
  free_dictionary (dict);  
}
