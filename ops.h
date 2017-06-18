#ifndef _OPS_H_
#define _OPS_H_
void
csl_rot (void);

void
csl_swap (void);

void
csl_nip (void);

void
csl_twodup (void);

void
csl_twoswap (void);
struct atom * atom_copy(struct atom *);

void
csl_define (void);

void
csl_if (void);

void
csl_dup (void);

void
csl_plus (void);

void
csl_sub (void);

void
csl_push (struct atom *a);

void
csl_pop (void);

void csl_print (void);
void csl_pop (void);


void
csl_plus (void);

void
csl_minus (void);

void
csl_divide (void);

void
csl_multiply (void);

#endif
