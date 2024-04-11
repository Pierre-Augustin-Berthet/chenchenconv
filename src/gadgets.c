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
    for(size_t i=0; i<MASKORDER; i++){
        out[i] = rand64();
        r ^= out[i];
    }
    out[MASKORDER] = in^r;
}

/*------------------------------------------------
UnmaskB   :   Boolean demasking at order MASKORDER
input     :   Boolean masking of input (MaskedB)
output    :   sensitive data in (uint64_t)
------------------------------------------------*/
void UnmaskB(uint64_t *out, MaskedB in){
    uint64_t r=0;
    for(size_t i=0; i<MASKORDER; i++){
        r ^= in[i];
    }
    *out = in[MASKORDER]^r;
}

/*------------------------------------------------
UnmaskB   :   Boolean demasking at order MASKORDER
input     :   Boolean masking of input (MaskedB)
output    :   sensitive data in (uint64_t)
------------------------------------------------*/
void SecAnd(MaskedB out, MaskedB ina, MaskedB inb){
    uint64_t r[MASKSIZE][MASKSIZE];
    for(size_t i = 0; i<MASKSIZE; i++) out[i] = ina[i]&inb[i];
    for(size_t i = 0; i <MASKSIZE-1; i++){
        for(size_t j = i+1; j<MASKSIZE;j++){
            r[i][j] = rand64();
            r[j][i] = (r[i][j]^(ina[i]&inb[j]));
            r[j][i] ^= (ina[j]&inb[i]);
            out[i] ^= r[i][j];
            out[j] ^= r[j][i];
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