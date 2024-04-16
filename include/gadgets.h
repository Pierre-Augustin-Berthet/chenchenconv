#ifndef GADGETS_H
#define GADGETS_H

#include <stdint.h>
#include <stddef.h>

#include "utils.h"

#define MASKORDER 2
#define MASKSIZE MASKORDER+1

typedef uint64_t* MaskedA;
typedef uint64_t* MaskedB;

void    MaskB               (MaskedB out, uint64_t in);
void    UnmaskB             (uint64_t *out, MaskedB in);

void    SecAnd              (MaskedB out, MaskedB ina, MaskedB inb);
void    SecOr               (MaskedB out, MaskedB ina, MaskedB inb);
void    RefreshXOR          (MaskedB out, MaskedB in, uint64_t k2);
void    SecAdd              (MaskedB out, MaskedB ina, MaskedB inb, uint64_t k, uint64_t log2km1);

void    MaskA               (MaskedA out, uint64_t in, uint64_t mod);
void    UnmaskA             (uint64_t *out, MaskedA in, uint64_t mod);

void    SecMult             (MaskedA out, MaskedA ina, MaskedA inb, uint64_t mod);

void    RefreshMasks        (MaskedB out, int size);
void    Refresh             ();

void    A2B                 (MaskedB out, MaskedA in, uint64_t mod, int size);
void    B2A                 (MaskedA out, MaskedB in, uint64_t mod, int size);
void    B2A_bit             ();

void 	RightRotate	     (uint64_t * x, uint32_t c);


#endif
