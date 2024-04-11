#ifndef GADGETS_H
#define GADGETS_H

#include <stdint.h>
#include <stddef.h>

#include "utils.h"

#define MASKORDER 2
#define MASKSIZE MASKORDER+1

typedef int32_t MaskedA[MASKSIZE];
typedef uint64_t MaskedB[MASKSIZE];

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