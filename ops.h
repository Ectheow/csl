#ifndef _OPS_H_
#define _OPS_H_
void
rot (void);

void
swap (void);

void
nip (void);

void
twodup (void);

void
twoswap (void);
struct atom * atom_copy(struct atom *);

void
define (void);

void
evaluator_init (void);

void
if_symbol (void);

void
dup (void);

void
add_stack (void);

void
sub_stack (void);

void
push_atom (struct atom *a);

void
pop_atom (void);

void print_stack (void);
void pop_stack (void);


void
plus (void);

void
minus (void);

void
divide (void);

void
multiply (void);

#endif
