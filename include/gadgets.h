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
void    SecAdd128           (uint64_t *out1,uint64_t *out2, uint64_t *ina1, uint64_t *ina2, uint64_t *inb1,uint64_t *inb2, int size);

void    MaskA               (MaskedA out, uint64_t in, uint64_t mod);
void    MaskA128            (MaskedA outup, MaskedA outdown, uint64_t inup, uint64_t indown);
void    UnmaskA             (uint64_t *out, MaskedA in, uint64_t mod);
void    UnmaskA128          (uint64_t *outup,uint64_t *outdown, MaskedA inup,MaskedA indown);

void    RefreshAM           (MaskedA out, MaskedA in, uint64_t mod, int size);

void    SecMult             (MaskedA out, MaskedA ina, MaskedA inb, uint64_t mod);
void    SecMult128          (MaskedA outup,MaskedA outdown, MaskedA inaup, MaskedA inadown, MaskedA inbup, MaskedA inbdown);

void    RefreshMasks        (MaskedB out, int size);

void    A2B                 (MaskedB out, MaskedA in, uint64_t mod);
void    A2B128              (MaskedB outup,MaskedB outdown, MaskedA inup, MaskedA indown, int size);
void    A2B_rec             (uint64_t *out, uint64_t *in, uint64_t mod, int size);
void    B2A                 (MaskedA out, MaskedB in, uint64_t mod, int size);
void    B2A128              (MaskedA outup,MaskedA outdown, MaskedB inup,MaskedB indown, int size);
void    B2A_bit             (MaskedA A, MaskedB x, uint64_t mod);
void    B2A_bit_j           (MaskedA C, MaskedA A, uint64_t xn, uint64_t mod, int n);

void 	RightRotate	        (uint64_t * x, uint32_t c);
void vecRightRotate(MaskedB in, uint32_t c);
void 	RightRotate2	    (uint64_t * x, uint32_t c);
void    NotB                (uint64_t * x);


#endif
