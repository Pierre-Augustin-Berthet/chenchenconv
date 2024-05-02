#ifndef SECFPR_H
#define SECFPR_H

#include "gadgets.h"

void SecNonZeroA(MaskedB out, MaskedA in, int32_t mod);
void SecNonZeroB(MaskedB out, MaskedB in);

void SecFprUrsh(MaskedB out, MaskedB x, MaskedA c);
void SecFprUrsh2(MaskedB out, MaskedB in, MaskedA c);
void SecFprUrsh3(MaskedB out, MaskedB out2, MaskedB in, MaskedA c);

void SecFprNorm64(MaskedB x, MaskedA e, uint64_t);

void SecFpr(MaskedB x, MaskedB s, MaskedA e, MaskedB m);
void SecFprMul(MaskedB out, MaskedB x, MaskedB y);
void SecFprMul2(MaskedB out, MaskedB x, MaskedB y);

void SecFprAdd(MaskedB out, MaskedB in1, MaskedB in2, uint64_t mod);
void SecFprAdd2(MaskedB out, MaskedB in1, MaskedB in2, uint64_t mod);

void SecFprTrunc(MaskedB out, MaskedB in);
void SecFprFloor(MaskedB out, MaskedB in);
void SecFprRound(MaskedB out, MaskedB in);


void Secfpr_expm_p63(MaskedB out, MaskedB x, MaskedB ccs);
void SecFprScalPtwo(MaskedB out, MaskedB in1, uint16_t ptwo);
void SecFprDivPtwo(MaskedB out, MaskedB in1, uint16_t ptwo);

uint64_t SecFprBerExp(MaskedB out, MaskedB x, MaskedB ccs);

#endif
