#ifndef FLOORFPR_H
#define FLOORFPR_H

#include "secFPR.h"


void SecArFloor(MaskedB, MaskedB);
void FourierEA(MaskedB out, MaskedB in);
void Taylor(MaskedB sin,MaskedB cos,MaskedB tempx);
void SerialTerm(MaskedB t0,MaskedB t1,MaskedB t2,MaskedB sin, MaskedB cos);

#endif