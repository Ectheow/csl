#include "test.h"
#include "../evaluator.h"
#include "../atoms.h"

int
main ()
{

  evaluator_init ();
  evaluator_push_atom (make_int_atom (2));
  evaluator_push_atom (make_int_atom (1));
  evaluator_push_atom (make_symbol_atom ("+"));
  
  TEST_EQ (3, 
	      atom_int_value (evaluator_peek ()),
	      "%d",
	     "addition with 1 and 2 works");
  evaluator_push_atom (make_int_atom (1));
  evaluator_push_atom (make_symbol_atom ("-"));
  TEST_EQ (2,
	      atom_int_value (evaluator_peek ()),
	      "%d",
	      "subtraction with 1 works");
  evaluator_push_atom (make_symbol_atom ("POP"));
  TEST_EQ (1, list_empty (evaluator_list ()),
	   "%d",
	   "list is empty after pop");
  int i;
  for (i=1; i <= 100; i++)
    evaluator_push_atom (make_int_atom (i));
  for (i=1; i < 100; i++)
    evaluator_push_atom (make_symbol_atom ("+"));

  TEST_EQ ( ( (1 + 100) * (100/2) ),
	    atom_int_value (evaluator_peek ()),
	    "%d",
	    "addition from 1 to 100");
  TEST_EQ (1, list_size (evaluator_list ()),
	   "%d",
	   "evaluator list size is correct after summation");
  evaluator_push_atom (make_symbol_atom ("POP"));
  TEST_EQ (0, list_size (evaluator_list ()),
	   "%d",
	   "evaluator list size correct after final pop");
	   

  
}

