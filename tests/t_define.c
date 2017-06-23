#include "test.h"
#include "../evaluator.h"

int
main ()
{
  evaluator_init ();

  evaluator_push_atom (make_symbol_atom (DEFINE));
  TEST_EQ ((void *)evaluator_state_definition_name,
	   evaluator_state (),
	   "%p",
	   "evaluator state is waiting for a name");
  evaluator_push_atom (make_symbol_atom ("X"));
  TEST_EQ ((void *)evaluator_state_definition_body,
	   evaluator_state (),
	   "%p",
	   "evaluator state is in the body");
  evaluator_push_atom (make_int_atom (1));
  TEST_EQ (1,
	   atom_int_value (atom_from_list_item (list_head (op_get_definition (evaluator_get_current_definition ())))),
			   "%d",
			   "top value of the definition stack");
  evaluator_push_atom (make_symbol_atom (ENDEF));

  TEST_EQ (0,
	   list_size (evaluator_list ()),
	   "%d",
	   "list size after complete definition");

  evaluator_push_atom (make_symbol_atom ("X"));
  TEST_EQ (1,
	   list_size (evaluator_list ()),
	   "%d",
	   "list size after using the definition");
  TEST_EQ (1,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "top stack value after definition");

  evaluator_push_atom (make_symbol_atom (DEFINE));
  evaluator_push_atom (make_symbol_atom ("Y"));
  evaluator_push_atom (make_symbol_atom ("+"));
  evaluator_push_atom (make_symbol_atom (ENDEF));

  evaluator_push_atom (make_int_atom (3));
  evaluator_push_atom (make_symbol_atom ("Y"));

  TEST_EQ (4,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "top stack value after using Y definition");

  evaluator_push_atom (make_symbol_atom ("POP"));
  
  evaluator_push_atom (make_symbol_atom (DEFINE));
  evaluator_push_atom (make_symbol_atom ("Z"));
  evaluator_push_atom (make_symbol_atom ("X"));
  evaluator_push_atom (make_symbol_atom ("X"));
  evaluator_push_atom (make_symbol_atom ("X"));
  evaluator_push_atom (make_symbol_atom ("Y"));
  evaluator_push_atom (make_symbol_atom ("Y"));
  evaluator_push_atom (make_symbol_atom (ENDEF));

  TEST_EQ (0, list_size (evaluator_list ()),
	   "%d",
	   "list size is correct after a definition");

  evaluator_push_atom (make_symbol_atom ("Z"));
  TEST_EQ (1,
	   list_size (evaluator_list ()),
	   "%d",
	   "list size after pushing definition");
  TEST_EQ (3,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "top stack value after compound definition");
}
