#ifndef GADGETS_H
#define GADGETS_H

#include <stdint.h>
#include <stddef.h>

#include "utils.h"

#define MASKORDER 2
#define MASKSIZE MASKORDER+1

typedef uint64_t MaskedA[MASKSIZE];
typedef uint64_t MaskedB[MASKSIZE];

void    MaskB               (MaskedB out, uint64_t in);
void    UnmaskB             (uint64_t *out, uint64_t *in, int size);

void    SecAnd              (uint64_t *out, uint64_t *ina, uint64_t *inb, int size);
void    SecOr               (MaskedB out, MaskedB ina, MaskedB inb);
void    RefreshXOR          (MaskedB out, MaskedB in, uint64_t mod, int size);
void    SecAdd              (uint64_t *out, uint64_t *ina, uint64_t *inb, uint64_t k, uint64_t log2km1, int size);

void    MaskA               (MaskedA out, uint64_t in, uint64_t mod);
void    UnmaskA             (uint64_t *out, MaskedA in, uint64_t mod);

void    SecMult             (MaskedA out, MaskedA ina, MaskedA inb, uint64_t mod);

void    RefreshMasks        (MaskedB out, int size);
void    Refresh             ();

void    A2B                 (MaskedB out, MaskedA in, uint64_t mod, int size);
void    B2A                 (MaskedA out, MaskedB in, uint64_t mod, int size);
void    B2A_bit             (MaskedA A, MaskedB x, uint64_t mod);
void    B2A_bit_j           (MaskedA C, MaskedA A, uint64_t xn, uint64_t mod, int n);

void 	RightRotate	        (uint64_t * x, uint32_t c);
void vecRightRotate(MaskedB in, uint32_t c);
void 	RightRotate2	    (uint64_t * x, uint32_t c);
void    NotB                (uint64_t * x);


#endif
