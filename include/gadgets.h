#ifndef GADGETS_H
#define GADGETS_H

#include <stdint.h>
#include <stddef.h>

#include "utils.h"

#define MASKORDER 2
#define MASKSIZE MASKORDER+1

typedef uint32_t* MaskedA;
typedef uint32_t* MaskedB;

void    MaskB               (MaskedB out, uint32_t in);
void    UnmaskB             (uint32_t *out, MaskedB in);

void    SecAnd              (MaskedB out, MaskedB ina, MaskedB inb);
void    SecOr               (MaskedB out, MaskedB ina, MaskedB inb);
void    RefreshXOR          (MaskedB out, MaskedB in, uint32_t k2);
void    SecAdd              (MaskedB out, MaskedB ina, MaskedB inb, uint32_t k, uint32_t log2km1);

void    MaskA               (MaskedA out, uint32_t in, uint32_t mod);
void    UnmaskA             (uint32_t *out, MaskedA in, uint32_t mod);

void    SecMult             (MaskedA out, MaskedA ina, MaskedA inb, uint32_t mod);

void    RefreshMasks        (MaskedB out, int size);
void    Refresh             ();

void    A2B                 (MaskedB out, MaskedA in, uint32_t mod, int size);
void    B2A                 (MaskedA out, MaskedB in, uint32_t mod, int size);
void    B2A_bit             ();


#endif