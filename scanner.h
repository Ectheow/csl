#ifndef _SCANNER_H_
#define _SCANNER_H_
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "atoms.h"
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



void scanner_scan_file (char *);
void scanner_init (void);
int scan_one (void);


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
