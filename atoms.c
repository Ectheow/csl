#include "atoms.h"
#include "list.h"
#include <strings.h>
#include <string.h>

atom_t
atom_from_list_item (list_item_t li)
{
  return (atom_t)(li->elem);
}

list_item_t
make_list_item_from_atom (struct atom *a)
{
  list_item_t it = make_list_item ();

  it->elem = (void *)a;
  return it;
}

atom_t
atom_copy (atom_t a)
{

  switch (atom_get_type (a))
    {
    case ATOM_SYMBOL:
      return make_symbol_atom (atom_symbol_value (a));
      break;
    case ATOM_INT:
      return make_int_atom (atom_int_value (a));
      break;
    case ATOM_FLOAT:
      return make_float_atom (atom_float_value (a));
      break;
    }
}


atom_t
make_symbol_atom (char *s)
{
  atom_t a = malloc (sizeof *a);

  a->type = ATOM_SYMBOL;
  a->value_sym = strdup (s);
  
  return a;
}

struct atom *
make_int_atom (int i)
{
  struct atom *a = malloc (sizeof *a);
  
  a->type = ATOM_INT;
  a->value_i = i;
  
  return a;
}

int
atoms_equal (struct atom *a1, struct atom *a2)
{
  if (a1->type != a2->type)
    return 0;
  else if (a1->type == ATOM_INT)
    return a1->value_i == a2->value_i;
  else if (a1->type == ATOM_FLOAT) /* whatever */
    return a1->value_f == a2->value_f;
  else if (a1->type == ATOM_SYMBOL)
    return !strcmp (a1->value_sym, a2->value_sym);
  else
    evaluator_die ("invalid atom type %d", a1->type);
}

int
atom_is_symbol (atom_t a)
{
  return a->type == ATOM_SYMBOL;
}
int
atom_compare_to_string (atom_t a1, char *s)
{
  atom_t a2 = make_symbol_atom (s);
  int result = 0;

  result = atoms_equal (a1, a2);
  atom_free (a2);

  return result;
}

int
atom_compare_to_int (atom_t a1, int i)
{
  atom_t a2 = make_int_atom (i);
  int result = 0;

  result = atoms_equal (a1, a2);
  atom_free (a2);

  return result;
}

struct atom *
make_float_atom (double f)
{
  struct atom *a = malloc (sizeof *a);

  a->type = ATOM_FLOAT;
  a->value_f = f;

  return a;
}


void
atom_free (struct atom *atom)
{
  if (!atom)
    return;
  else if (atom->type == ATOM_SYMBOL)
    free (atom->value_sym);

  memset (atom, 0xab, sizeof *atom);
  free (atom);
}

char *
atom_to_str (struct atom *atom)
{
  static char buf[BUFSIZ];
  
  if (atom->type == ATOM_SYMBOL)
    snprintf (buf, BUFSIZ, "'%s'", atom->value_sym);
  else if (atom->type == ATOM_FLOAT)
    snprintf (buf, BUFSIZ, "%f", atom->value_f);
  else if (atom->type == ATOM_INT)
    snprintf (buf, BUFSIZ, "%d", atom->value_i);

  return buf;
}
