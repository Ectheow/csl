#include "test.h"
#include "../evaluator.h"

int
main ()
{
  evaluator_init ();

  evaluator_push_atom (make_int_atom (2));
  evaluator_push_atom (make_int_atom (1));

  // 2 1 
  evaluator_push_atom (make_symbol_atom ("SWAP"));
  // 1 2 
  TEST_EQ (2,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "basic swap works");


  evaluator_push_atom (make_symbol_atom ("DUP"));
  // 1 2 2
  
  TEST_EQ (3,
	   list_size (evaluator_list ()),
	   "%d",
	   "dup list size is correct");
  evaluator_push_atom (make_symbol_atom ("ROT"));
  // 2 2 1
  TEST_EQ (1,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "rot works");
  evaluator_push_atom (make_symbol_atom ("ROT"));
  
  TEST_EQ (2,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "rot works (2)");
  
  evaluator_push_atom (make_symbol_atom ("ROT"));
  TEST_EQ (2,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "rot works (3)");
  evaluator_push_atom (make_symbol_atom ("ROT"));
  
  TEST_EQ (1,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "rot works (4)");

  evaluator_push_atom (make_symbol_atom ("TWODUP"));
  // 2 2 1 2 1

  TEST_EQ (5,
	   list_size (evaluator_list ()),
	   "%d",
	   "list size after twodup");
  TEST_EQ (1,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "first value on stack after twodup");
  evaluator_push_atom (make_symbol_atom ("POP"));
  // 2 2 1 2
  TEST_EQ (2,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "first value on stack after a pop");
  evaluator_push_atom (make_symbol_atom ("POP"));
  // 2 2 1 
  TEST_EQ (1,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "first value on stack after a pop");

  evaluator_push_atom (make_symbol_atom ("NIP"));
  // 2 1
  TEST_EQ (1,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "nip works");
  
  evaluator_push_atom (make_symbol_atom ("POP"));
  
  TEST_EQ (2,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "nip works (2)");
  TEST_EQ (1,
	   list_size (evaluator_list ()),
	   "%d",
	   "size verification for nip");
  evaluator_push_atom (make_symbol_atom ("POP"));

  evaluator_push_atom (make_int_atom (1));
  evaluator_push_atom (make_int_atom (2));
  evaluator_push_atom (make_int_atom (3));
  evaluator_push_atom (make_int_atom (4));
  
  TEST_EQ (4,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "top value of stack for twoswap -- before");
  evaluator_push_atom (make_symbol_atom ("TWOSWAP"));

  TEST_EQ (2,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "top value of stack for twoswap -- after");
  evaluator_push_atom (make_symbol_atom ("POP"));
  TEST_EQ (1,
	   atom_int_value (evaluator_peek ()),
	   "%d",
	   "top value of stack for twoswap -- after (2)");  
  
}
