#include "gadgets.h"

void    MaskA               (MaskedA out, uint32_t in);
void    UnmaskA             (uint32_t *out, MaskedA in);


/*------------------------------------------------
MaskB   :   Boolean Masking at order MASKORDER
input   :   sensitive data in (uint64_t)
output  :   Boolean masking of input (MaskedB)
------------------------------------------------*/
void MaskB(MaskedB out, uint64_t in){
    uint64_t r=0;
    for(size_t i=MASKORDER-1; i>1; i--){
        out[i] = rand64();
        r ^= out[i];
    }
    out[0] = in^r;
}

/*------------------------------------------------
UnmaskB   :   Boolean demasking at order MASKORDER
input     :   Boolean masking of input (MaskedB)
output    :   sensitive data in (uint64_t)
------------------------------------------------*/
void UnmaskB(uint64_t *out, MaskedB in){
    uint64_t r=0;
    for(size_t i=MASKORDER-1; i>1; i--){
        r ^= in[i];
    }
    out = in[0]^r;
}

/*------------------------------------------------
UnmaskB   :   Boolean demasking at order MASKORDER
input     :   Boolean masking of input (MaskedB)
output    :   sensitive data in (uint64_t)
------------------------------------------------*/
void SecAnd(MaskedB out, MaskedB ina, MaskedB inb){
    uint64_t r,t;
    for(size_t i = 0; i <MASKORDER; i++) out[i] = ina[i] & inb[i];
    for(size_t i = 0; i <MASKORDER; i++){
        for(size_t j = i+1; j <MASKORDER;j++){
            r = rand64();
            out[i] = out[i] ^r;
            t = ina[i] & inb[j];
            r ^= t;
            t = ina[j] & inb[i];
            r ^= t;
            out[j] ^= r;
        }
    }
}

void    SecOr               (MaskedB out, MaskedB ina, MaskedB inb);

void    RefreshMasks        ();
void    Refresh             ();

void    SecAdd              ();
void    SecMult             ();

void    A2B                 (MaskedB out, MaskedA in);
void    B2A                 (MaskedA out, MaskedB in);
void    B2A_bit             ();