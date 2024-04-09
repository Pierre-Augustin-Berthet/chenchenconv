#ifndef GADGETS_H
#define GADGETS_H

#include <stdint.h>

#define MASKORDER 3

typedef MaskedA {int32_t shares[MASKORDER];} MaskedA;
typedef MaskedB {uint64_t shares[MASKORDER];} MaskedB;

void    MaskA               (MaskedA out, uint32_t in);
void    UnmaskA             (uint32_t *out, MaskedA in);

void    MaskB               (MaskedB out, uint64_t in);
void    UnmaskB             (uint64_t *out, MaskedB in);

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