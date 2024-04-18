#ifndef SECFPR_H
#define SECFPR_H

#include "gadgets.h"

void SecNonZeroA(MaskedB out, MaskedA in, int32_t mod);
void SecNonZeroB(MaskedB out, MaskedB in);

void SecFprUrsh(MaskedB out, MaskedB x, MaskedA c);
void SecFprUrsh2(MaskedB out, MaskedB in, MaskedA c);

void SecFprNorm64(MaskedB x, MaskedA e, uint64_t);

void SecFPR(MaskedB x[2], MaskedB s, MaskedA e, MaskedB m);

void SecFprAdd(MaskedB out, MaskedB in1, MaskedB in2, uint64_t mod);

#endif
