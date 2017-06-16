#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>


/*
 * the stack language will look like forth.
 *
 * > 1 1 + POP
 *
 * will print
 *
 * > 2
 *
 * the stack will be empty
 *
 * > 1 PRINT 
 * 
 * will print 1. The stack will still have 1 on it and whatever its
 * previous contents were.
 * 
 * Initially these operations will be defined
 * / + - * POP PRINT SWAP DUP 2SWAP 2DUP
 * 
 * Next I'll define a way to make a dictionary entry
 * DEFINE
 *
 * All operations will be in the dictionary. An operation is either
 * 1. a function that carries out the op
 * 2. a list of smaller operations that carry out the op.
 * 3. nothing
 * 
 * The stack will (initially) hold atoms, which will be either numbers
 * or symbols. Symbols are always operations and will be interpreted
 * as such.
 */

enum atom_type
  {

    ATOM_INT,
    ATOM_FLOAT,
    ATOM_SYMBOL,
    ATOM_INVALID,
  };

struct atom
{
  enum atom_type type;
  union
  {
    double value_f;
    int value_i;
    char *value_sym;
  };
};

void free_atom (struct atom *);

struct stack
{
  struct atom *atom;
  struct stack *next;
};

struct stack *stack_head = NULL;

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
struct dictionary *dict = NULL; /* global dictionary */

struct evaluator
{
  void (*eval_func) (void);
  void *state_info;
  int in_def;
  int in_if;
} evaluator;

#define DEFINE "DEFINE"
#define ENDEF "ENDEF"
#define ELSE "ELSE"
#define PRINT "PRINT"
#define IF "IF"
#define ENDIF "ENDIF"

void
free_stack_elem (struct stack *s);
  
struct op *
lookup_op_sym (struct atom *a);

void
evaluate_op (struct op *o);

void
insert_op (struct op *o);

void
delete_op (struct op *o);

struct atom *
get_atom (void);

void
evaluate_stack (void);

void
plus (void);

void
minus (void);

void
divide (void);

void
multiply (void);

void
free_stack (struct stack *);

void
print_stack (void);

struct op *
make_atomic_op (char *, void (*)());

void
add_stack (void);

void
sub_stack (void);

void
push_atom (struct atom *a);

void
pop_atom (void);

void pop_stack (void);		/* for the actual POP op. */
struct stack *
make_stack_elem (struct atom *a);

void
define (void);

void
evaluator_init (void);

void
if_symbol (void);

void
dup (void);

void
free_dictionary (struct dictionary *);

void
free_op (struct op *);
void
rot (void);

void
swap (void);

void
nip (void);

void
twodup (void);

void
twoswap (void);
struct atom * atom_copy(struct atom *);

int
main (void)
{
  struct atom *a = NULL;
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
  evaluator_init ();
  
  while ( (a = get_atom ()))
    {
      push_atom (a);
      evaluator.eval_func ();
    }
  free_stack (stack_head);
  free_dictionary (dict);
}

void
free_dictionary (struct dictionary *d)
{
  if (!d)
    return;
  free_dictionary (d->next);
  free_op (d->op);
}

struct stack *
__pop_atom (void);

void
evaluator_die (char *msg);

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




struct atom *
make_int_atom (int i)
{
  struct atom *a = malloc (sizeof *a);
  
  a->type = ATOM_INT;
  a->value_i = i;
  
  return a;
}
struct atom *
make_float_atom (double f)
{
  struct atom *a = malloc (sizeof *a);

  a->type = ATOM_FLOAT;
  a->value_f = f;

  return a;
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
free_stack (struct stack *stack_head)
{
  if (!stack_head)
    return;
  
  free_stack (stack_head->next);  
  free_stack_elem (stack_head);
  stack_head = NULL;
}

void
evaluator_die (char *msg)
{
  fprintf (stderr, "Evaluation error %s\n", msg);
  exit (EXIT_FAILURE);
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
insert_op (struct op *o)
{
  struct dictionary *dent = malloc (sizeof *dent);

  if (!dent)
    evaluator_die ("EVALUATOR ERROR: can't allocate dent");
  dent->op = o;
  dent->next = dict;
  dict = dent;
}

void
evaluator_init (void)
{
  evaluator.eval_func = evaluate_stack;
  evaluator.state_info = NULL;
  evaluator.in_def = 0;
  
}
void
free_stack_elem (struct stack *s)
{
  free_atom (s->atom);
  free (s);
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
pop_atom (void)
{
  struct stack *s = __pop_atom ();
  free_stack_elem (s);
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

struct op *
make_stack_op (void)
{
  struct op *o = malloc (sizeof *o);

  o->type = OP_INVALID;
  memset (o->op_name, 0, sizeof o->op_name);
  o->definition = NULL;
  return o;
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
	  free_stack_elem (s);
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


void scan_start (void);
int scanner_eof (void);
struct scanner
{
  int c;
  int is_eof;
  int lineno;
  int charno;
  int bufpos;
  char buf[BUFSIZ];
  char output_value[BUFSIZ];
  int output_value_pos;
  enum atom_type type;
  void (*state)(void);
} scanner = {
  0,
  0,
  0,
  0,
  0,
  {0},
  {0},
  0,
  ATOM_INVALID,
  scan_start,
};

void
scanner_die (char *msg)
{
  fprintf (stderr,
	   "SCAN ERROR: %s. Scanner state:"
	   "\tchar = %c (%d)"
	   "\tchar# = %d"
	   "\tline = %d\n",
	   msg, scanner.c, scanner.c, scanner.charno, scanner.lineno);
  exit (EXIT_FAILURE);
}

void
scanner_reinit (void)
{
  scanner.c = scanner.bufpos = scanner.output_value_pos = 0;
  memset (scanner.buf, 0, sizeof scanner.buf);
  memset (scanner.output_value, 0, sizeof scanner.output_value);
  scanner.type = ATOM_INVALID;
  scanner.state = scan_start;
  /* don't reinit is_eof */
}

int
scanner_getch (void);

void
free_atom (struct atom *atom)
{
  if (!atom)
    return;
  else if (atom->type == ATOM_SYMBOL)
    free (atom->value_sym);

  free (atom);
}

struct atom *
get_atom (void)
{

  struct atom *a = malloc (sizeof *a);

  scanner_reinit ();
  a->type = ATOM_INVALID;
  a->value_sym = NULL;
  
  while (scanner_getch ())
    ;
  if (scanner_eof ())
    {
      free (a);
      return NULL;
    }
  else if (scanner.type == ATOM_INT)
    {
      a->value_i = atoi (scanner.output_value);
      a->type = ATOM_INT;
    }
  else if (scanner.type == ATOM_FLOAT)
    {
      a->value_f = atof (scanner.output_value);
      a->type = ATOM_FLOAT;
    }
  else if (scanner.type == ATOM_SYMBOL)
    {
      a->value_sym = strdup (scanner.output_value);
      a->type = ATOM_SYMBOL;
    }
  else
    scanner_die ("get_atom: bad atom type");
  
  return a;
}

void (*scan_end)(void) = NULL;
void scan_num (void);
void scan_sym (void);

int
scanner_isspace (int c)
{
  return isspace (c);
}

int
scanner_issym_char (int c)
{
  return ((c >= 'a' && c <= 'z') ||
	  (c >= 'A' && c <= 'Z') ||
	  c == ':' ||
	  c == '-' ||
	  c == '_' ||
	  c == '&' ||
	  c == '^' ||
	  c == '+' ||
	  c == '*' ||
	  c == '/' ||
	  c == '%');
}

int scanner_eof (void)
{
  return scanner.is_eof;
}
int
scanner_isnum_char (int c)
{
  return (c == '.' || (c >= '0' && c <= '9'));
}

void
scanner_ungetch (int c)
{
  scanner.buf[scanner.bufpos++] = c;
}

int
scanner_getch (void)
{
  if (scanner.state == scan_end || scanner.is_eof)
    return 0;


  if (scanner.bufpos > 0)
    scanner.c = scanner.buf[--scanner.bufpos];
  else if ( (scanner.c = getchar ()) == '\n')
    {
      scanner.c = ' ';
      scanner.charno++;
      scanner.lineno++;
    }
  else if (scanner.c == EOF)
    {
      scanner.c = ' '; 		/* the rest of the scanner may have a symbol */
      scanner.is_eof = 1;
    }
  else
    scanner.charno++;

  scanner.state ();
  return 1;
}


void
scan_start (void)
{
  if (scanner_isspace (scanner.c))
    {
      scanner.state = scan_start;
    }
  else if (scanner_isnum_char (scanner.c))
    {
      scanner_ungetch (scanner.c);
      scanner.state = scan_num;
    }
  else if (scanner_issym_char (scanner.c))
    {
      scanner_ungetch (scanner.c);
      scanner.state = scan_sym;
    }
  else
    scanner_die ("unknown/invalid char");
}

void
scan_num (void)
{
  if (scanner.c == '.')
    scanner.type = ATOM_FLOAT;
  else
    scanner.type = ATOM_INT;
  
  if (scanner_isspace (scanner.c))
    {
      scanner.output_value[scanner.output_value_pos++] = '\0';
      scanner.state = scan_end;
    }
  else if (scanner_isnum_char (scanner.c))
    scanner.output_value[scanner.output_value_pos++] = scanner.c;
  else
    scanner_die ("scan_num: invalid char while scanning number");

}

void
scan_sym (void)
{
  scanner.type = ATOM_SYMBOL;
  if (scanner_isspace (scanner.c))
    {
      scanner.output_value[scanner.output_value_pos++] = '\0';
      scanner.state = scan_end;
    }
  else if (scanner_issym_char (scanner.c))
    scanner.output_value[scanner.output_value_pos++] = scanner.c;
  else
    scanner_die ("scan_sym: invalid char while scanning a symbol");
}
