#include "test.h"
#include "../evaluator.h"

int
main ()
{
  evaluator_init ();

  evaluator_push_atom (make_int_atom (1));
  evaluator_push_atom (make_symbol_atom (IF));
  evaluator_push_atom (make_int_atom (2));
  evaluator_push_atom (make_symbol_atom (ENDIF));

  TEST_EQ (1,
	   list_size (evaluator_list ()),
	   "%d",
	   "list size is correct after first if");
  TEST_EQ (2,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "top value of stack is 2 after if");

  evaluator_push_atom (make_symbol_atom ("POP"));

  evaluator_push_atom (make_int_atom (0));
  evaluator_push_atom (make_symbol_atom (IF));
  evaluator_push_atom (make_int_atom (3));
  TEST_EQ (0,
	   evaluator_if_was_true (),
	   "%d",
	   "evaluator internal state about if is right");
  evaluator_push_atom (make_symbol_atom (ENDIF));

  TEST_EQ (0,
	   list_size (evaluator_list ()),
	   "%d",
	   "list size is 0 after a fail if, with no else");

  evaluator_push_atom (make_int_atom (0));
  evaluator_push_atom (make_symbol_atom (IF));
  evaluator_push_atom (make_int_atom (3));
  evaluator_push_atom (make_symbol_atom (ELSE));
  evaluator_push_atom (make_int_atom (4));
  evaluator_push_atom (make_symbol_atom (ENDIF));

  TEST_EQ (4,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "top value of stack is 4 after else");
}
