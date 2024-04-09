#ifndef GADGETS_H
#define GADGETS_H

#include <stdint.h>

typedef MaskedA {int32_t shares[3];} MaskedA;
typedef MaskedB {uint64_t shares[3];} MaskedB;

void    SecAnd              (MaskedB out, MaskedB ina, MaskedB inb);
void    SecOr               (MaskedB out, MaskedB ina, MaskedB inb);

void    RefreshMasks        ();
void    Refresh             ();

void    SecAdd              ();
void    SecMult             ();

void    A2B                 (MaskedB out, MaskedA in);
void    B2A                 (MaskedA out, MaskedB in);
void    B2A_bit             ();


#endif