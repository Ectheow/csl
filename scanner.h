#ifndef _SCANNER_H_
#define _SCANNER_H_
#include <stdio.h>
#include <string.h>
#include <strings.h>


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

void scan_start(void);

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
  struct stack *read_pos;
  void (*state)(void);
};
extern struct scanner scanner;


struct atom *get_atom (void);
struct atom *make_float_atom (double);
struct atom *make_int_atom (int);
void free_atom (struct atom *);
void scanner_scan_file (char *);
void scanner_init (void);
int scan_one (void);
char *atom_to_str (struct atom *);

#define scanner_die(msg, ...) error (1, "SCAN ERROR: "msg" Scanner state:" \
				     "\tchar = '%c' (%d)"		\
				     "\tchar# = %d"			\
				     "\tline = %d",			\
				     ##__VA_ARGS__,			\
				     scanner.c,				\
				     scanner.c,				\
				     scanner.charno,			\
				     scanner.lineno)

#endif
