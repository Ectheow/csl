#include "test.h"
#include "../atoms.h"


int
main ()
{
  atom_t atom_i, atom_s, atom_f;

  atom_s = make_symbol_atom ("+");
  atom_i = make_int_atom (1);
  atom_f = make_float_atom (1.0);
  

  TEST_OK (atom_is_symbol (atom_s), "atom is symbol");
  TEST_OK (atom_is_int (atom_i), "atom is integer");
  TEST_OK (atom_is_float (atom_f), "atom is float");
  TEST_OK (atom_compare_to_string (atom_s, "+"), "plus atom comparison");
  TEST_EQ (0, atom_compare_to_string  (atom_i, "+"), "%d", "int atom comparison fails");
  TEST_EQ (1, atom_compare_to_int (atom_i, 1), "%d", "int comparison ok");
}
