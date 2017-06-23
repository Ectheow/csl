#ifndef _ATOMS_H_
#define _ATOMS_H_
#include <stdio.h>
#include <assert.h>
#include "list.h"

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

typedef struct atom *atom_t;
atom_t get_atom (void);
atom_t make_float_atom (double);
atom_t make_int_atom (int);
atom_t make_symbol_atom (char *);

void atom_free (struct atom *);
char *atom_to_str (struct atom *);
int atom_is_symbol (atom_t);
int atom_compare_to_string (atom_t, char *);
int atom_compare_to_int (atom_t, int);
atom_t atom_from_list_item (list_item_t);
atom_t atom_copy (atom_t);

list_item_t make_list_item_from_atom (struct atom *a);

static inline void
atom_print (atom_t a)
{
  printf (atom_to_str (a));
}

static inline int
atom_int_value (atom_t a)
{
  assert (a->type == ATOM_INT);
  return a->value_i;
}

static inline double
atom_float_value (atom_t a)
{
  assert (a->type == ATOM_FLOAT);
  return a->value_f;
}

static inline char *
atom_symbol_value (atom_t a)
{
  assert (a->type == ATOM_SYMBOL);
  return a->value_sym;
}

static inline int
atom_is_int (atom_t a)
{
  return a->type == ATOM_INT;
}

static inline int
atom_is_float (atom_t a)
{
  return a->type == ATOM_FLOAT;
}

static inline enum atom_type
atom_get_type (atom_t a)
{
  return a->type;
}

#endif
