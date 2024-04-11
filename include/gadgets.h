#ifndef GADGETS_H
#define GADGETS_H

#include <stdint.h>
#include <stddef.h>

#include "utils.h"

#define MASKORDER 2
#define MASKSIZE MASKORDER+1

typedef int32_t MaskedA[MASKSIZE];
typedef uint64_t MaskedB[MASKSIZE];

void    MaskB               (MaskedB out, uint64_t in);
void    UnmaskB             (uint64_t *out, MaskedB in);

void    SecAnd              (MaskedB out, MaskedB ina, MaskedB inb);
void    SecOr               (MaskedB out, MaskedB ina, MaskedB inb);


void    MaskA               (MaskedA out, int32_t in, int32_t mod);
void    UnmaskA             (int32_t *out, MaskedA in, int32_t mod);

void    SecMult             (MaskedA out, MaskedA ina, MaskedA inb, int32_t mod);

void    RefreshMasks        ();
void    Refresh             ();

void    SecAdd              ();


void    A2B                 (MaskedB out, MaskedA in);
void    B2A                 (MaskedA out, MaskedB in);
void    B2A_bit             ();


#endif