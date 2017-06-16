#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>

void scan_start (void);
int scanner_eof (void);
FILE *scanner_fp = NULL;

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
  if (scanner_eof ())
    {
      free (a);
      return NULL;
    }
  
  while (scanner_getch ())
    ;

   if (scanner.type == ATOM_INT)
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
void
scanner_scan_file (char *fname)
{
  if ( !(scanner_fp = fopen (fname, "r")))
    scanner_die ("can't open file");
}

void
scanner_init (void)
{
  if (!scanner_fp)
    scanner_fp = stdin;
}

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
  else if ( (scanner.c = getc (scanner_fp)) == '\n')
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
