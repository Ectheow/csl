#include "evaluator.h"
#include "ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

struct evaluator evaluator;
struct dictionary *dict = NULL; /* global dictionary */
struct stack *stack_head = NULL;
void pop_atom (void);

static void __evaluate_definition (struct stack *s);
void free_op (struct op *);

void
evaluate (void)
{
  evaluator.eval_func ();
}

void
evaluator_die (char *msg)
{
  fprintf (stderr, "Evaluation error %s\n", msg);
  exit (EXIT_FAILURE);
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
evaluate_define (void)
{
  struct op *o = (struct op *)evaluator.state_info;
  struct stack *s = NULL, *p = NULL, *l = NULL;
  
  /* If we're at the end of the definition, then pop these values off
     onto the stack for their op. */
  /*
    stack picture
    ENDEF -> ATOM1 -> ... -> ATOMN -> NAME -> DEFINE -> (REST OF STACK)
  */

  /* printf ("eval define\n"); */
  if (!o)
    evaluator_die ("No allocated operation for define");

  if (stack_head->atom->type == ATOM_SYMBOL
      && !strcmp (stack_head->atom->value_sym, ENDEF))
    {
      pop_atom ();		/* get rid of the ENDEF */
      o->type = OP_STACK;
      o->definition = stack_head;
      for (s = stack_head;
	   s && (s->atom->type != ATOM_SYMBOL
		 || strcmp (s->atom->value_sym, DEFINE));
	   l=p,p=s,s=s->next)
	;
      
      if (!s)
	evaluator_die ("Empty definition stack");
      if (p->atom->type != ATOM_SYMBOL)
	evaluator_die ("No name for defined value");

      
      l->next = NULL;
      strcpy (o->op_name, p->atom->value_sym);
      stack_head = s->next;
      free_stack_elem (p);
      free_stack_elem (s);
      evaluator.eval_func = evaluate_stack;
      insert_op (o);
      evaluator.in_def = 0;
    }
}

void
evaluate_if (void)
{
  struct stack *s = NULL, *p = NULL, *if_stack = NULL, *else_stack = NULL;
  int free_p;


  if (stack_head->atom->type == ATOM_SYMBOL
      && !strcmp (stack_head->atom->value_sym, ENDIF))
    {
      for (s = stack_head,free_p = 0;
	   s && (s->atom->type != ATOM_SYMBOL
		 || strcmp (s->atom->value_sym, IF));
	   p = s,s = s->next)
	{
	  if (s->atom->type == ATOM_SYMBOL
	      && !strcmp (s->atom->value_sym, ELSE))
	    {
	      else_stack = stack_head->next;
	      p->next = NULL;
	      if_stack = s->next;
	      free_p = 1;
	    }
	  else if (free_p)
	    {
	      free_stack_elem (p); /* free p, which was an else. */
	      free_p = 0;
	    }
	}
      if (!s)
	evaluator_die ("Empty if stack");
      if (!if_stack)
	if_stack = stack_head->next;
      if (s->next->atom->type == ATOM_INT
	     && s->next->atom->value_i == 0)
	{
	  /* printf (">>> else\n"); */
	  free_stack_elem (stack_head);
	  stack_head = s->next;
	  s->next = NULL;
	  free_stack (if_stack);
	  pop_atom ();		/* pop of the conditional which was
				   false. */
	  evaluator.in_if = 0;
	  evaluator.eval_func = evaluate_stack;
	  if (else_stack)
	    {
	      __evaluate_definition (else_stack);
	      free_stack (else_stack);
	    }
	}
      else
	{
	  if (else_stack)
	    free_stack (else_stack);
	  p->next = NULL;
	  free_stack_elem (stack_head);
	  stack_head = s->next;
	  free_stack_elem (s);
	  pop_atom ();  
	  evaluator.in_if = 0;
	  evaluator.eval_func = evaluate_stack; 
	  __evaluate_definition (if_stack);
	  free_stack (if_stack);

	}
      evaluator.in_if = 0;
      evaluator.eval_func = evaluate_stack;
    }
}

void
define (void)
{
  struct op *o = make_stack_op ();
  evaluator.state_info = o;
  evaluator.eval_func = evaluate_define; /* don't pop though */
  evaluator.in_def = 1;
}

void
if_symbol (void)
{
  
  evaluator.eval_func = evaluate_if;
  evaluator.state_info = NULL;
  evaluator.in_if = 1;
}

void
evaluate_stack (void)
{

  while (stack_head
	 && stack_head->atom->type == ATOM_SYMBOL
	 && !evaluator.in_def
	 && !evaluator.in_if)
    evaluate_symbol ();
  

}



void
evaluate_symbol (void)
{
  struct op *o = NULL;

  /* printf ("symbol: %s\n", */
  /* 	  stack_head->atom->value_sym);/*  *\/ */
  /* __print_stack (); */
  if (! (o = lookup_op_sym (stack_head->atom)))
      evaluator_die ("evaluate_symbol: can't find this atom");
  
  evaluate_op (o);
}

void
evaluator_init (void)
{
  evaluator.eval_func = evaluate_stack;
  evaluator.state_info = NULL;
  evaluator.in_def = 0;
  
  insert_op (make_atomic_op ("PRINT", print_stack));
  insert_op (make_atomic_op ("+", add_stack));
  insert_op (make_atomic_op ("-", sub_stack));
  insert_op (make_atomic_op ("DEFINE", define));
  insert_op (make_atomic_op ("POP", pop_stack));
  insert_op (make_atomic_op ("IF", if_symbol));
  insert_op (make_atomic_op ("DUP", dup));
  insert_op (make_atomic_op ("SWAP", swap));
  insert_op (make_atomic_op ("ROT", rot));
  insert_op (make_atomic_op ("TWOSWAP", twoswap));
  insert_op (make_atomic_op ("TWODUP", twodup));
  insert_op (make_atomic_op ("NIP", nip));
  
  
}

void
evaluator_cleanup ()
{
  free_stack (stack_head);
  free_dictionary (dict);  
}

void
evaluate_op (struct op *o)
{
  if (!o)
    evaluator_die ("ERROR: null operator given to evaluate_op");
  if (o->type == OP_STACK)
    evaluate_definition (o->definition);
  else if (o->type == OP_FUNCTION)
    o->carry_out_op ();
  else
    evaluator_die ("EVALUTOR ERROR: Was given a bad operation");
    
}


static void
__evaluate_definition (struct stack *s)
{
  if (!s)
    return;

  __evaluate_definition (s->next);
  push_atom (atom_copy (s->atom));
  evaluator.eval_func ();
}

void
evaluate_definition (struct stack *s)
{
  pop_atom ();
  __evaluate_definition (s);
}
