#ifndef _SCANNER_H_
#define _SCANNER_H_

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

struct atom *
get_atom (void);
#endif
