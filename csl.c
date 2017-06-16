#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include "scanner.h"
#include "evaluator.h"

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

int
main (int argc, char **argv)
{
  struct atom *a = NULL;

  if (argc == 2)
      scanner_scan_file (argv[1]);

  scanner_init ();
  evaluator_init ();
  
  while ( (a = get_atom ()))
    {
      push_atom (a);
      evaluate ();
    }
  evaluator_cleanup ();
}

