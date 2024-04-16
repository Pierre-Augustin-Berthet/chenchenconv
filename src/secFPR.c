#include "secFPR.h"

/*------------------------------------------------
SecNonZeroA :   Secure check that remaining bits are
                non zero (input arithmetic) at 
                order MASKORDER
input       :   Arithmetic masking in (MaskedA),
                Modulo mod (int32_t)
output      :   Boolean masking out (MaskedB)
------------------------------------------------*/
void SecNonZeroA(MaskedB out, MaskedA in, int32_t mod){
    MaskedB t,l,r;
    for(size_t i= MASKSIZE/2; i < MASKSIZE; i++)  in[i] = -in[i];
    A2B(t,in, mod, MASKSIZE);
    int32_t bitsize;
    int32_t len=bitsize/2;
    while(len>=1){
        //Refresh(l,t,len,2*len); // ATTENTION IL FAUT UN REFRESH OU ON NE REFRESH QUE CERTAINES SHARES
        for(size_t i=0; i<len-1;i++){
            r[i] = t[i];
        }
        SecOr(t,l,r);
        len = len >> 1;
    }
    out[0] = t[0];
}

/*------------------------------------------------
SecNonZeroB :   Secure check that remaining bits are
                non zero (input boolean) at 
                order MASKORDER
input       :   Boolean masking in (MaskedB)
output      :   Boolean masking out (MaskedB)
------------------------------------------------*/
void SecNonZeroB(MaskedB out, MaskedB in){
    MaskedB t,l,r;
    for(size_t i= 0; i < MASKSIZE; i++) t[i] = in[i];
    int32_t bitsize;
    int32_t len=bitsize/2;
    while(len>=1){
        //Refresh(l,t,len,2*len); // ATTENTION IL FAUT UN REFRESH OU ON NE REFRESH QUE CERTAINES SHARES
        for(size_t i=0; i<len-1;i++){
            r[i] = t[i];
        }
        SecOr(t,l,r);
        len = len >> 1;
    }
    for(size_t i = 0; i <MASKSIZE; i++) out[i] = t[i]&1; // (t_i^{(1)})
}


/*------------------------------------------------
SecFprUrsh  :   Secure right-shift
input       :   Boolean masking in (MaskedB)
                6-bits Integer vector c
output      :   Boolean masking out (MaskedB)
------------------------------------------------*/

void SecFprUrsh(MaskedB out, MaskedB in, MaskedA c){
    MaskedB m;
    m[0] = ((uint64_t)1)<<63;
    for (int j =0; j< MASKSIZE; j++){
        for (int i = 0; i < MASKSIZE; i++){
            RightRotate(&in[i], c[j]); 
// Just need to verify here if we want a rotation or a shifted version of it
        }
        RefreshMasks(in, MASKSIZE);
        for (int i = 0; i < MASKSIZE; i++){
            RightRotate(&m[i], c[j]);
        }
        RefreshMasks(m, MASKSIZE);
    }
    uint8_t len = 1;
    while(len<=32){
        for (int i = 0; i < MASKSIZE; i++){
            m[i] = m[i] ^ (m[i]>>len);
        }
        len <<= 1;
    }
    MaskedB temp;
    SecAnd(temp, in, m);
    for (int i = 0; i<MASKSIZE; i++){
        out[i] = temp[i] ^ in[i];
        out[i]^= temp[i] & 1;
    }
    MaskedB b;
    SecNonZeroB(b, out);
    for (int i = 0; i<MASKSIZE; i++){
        out[i] = (temp - temp & 1) | b[i];
    }
}

void SecFprUrsh2(MaskedB out, MaskedB in, MaskedA c){
    MaskedB m;
    m[0] = ((uint64_t)1)<<63;
    for (int j =0; j< MASKSIZE; j++){
        for (int i = 0; i < MASKSIZE; i++){
            RightRotate2(&in[i], c[j]); 
// Just need to verify here if we want a rotation or a shifted version of it
        }
        RefreshMasks(in, MASKSIZE);
        for (int i = 0; i < MASKSIZE; i++){
            RightRotate2(&m[i], c[j]);
        }
        RefreshMasks(m, MASKSIZE);
    }
    uint8_t len = 1;
    while(len<=32){
        for (int i = 0; i < MASKSIZE; i++){
            m[i] = m[i] ^ (m[i]>>len);
        }
        len <<= 1;
    }
    MaskedB temp;
    SecAnd(temp, in, m);
    for (int i = 0; i<MASKSIZE; i++){
        out[i] = temp[i] ^ in[i];
        out[i]^= temp[i] & 1;
    }
    MaskedB b;
    SecNonZeroB(b, out);
    for (int i = 0; i<MASKSIZE; i++){
        out[i] = (temp - temp & 1) | b[i];
    }
}

void 
SecFprNorm64(MaskedB out, MaskedA e){
    MaskedB t, n, b, bp;
    for (int j = 5 ; j>=0; j--){
        for (int i = 0; i< MASKSIZE; i++){
            t[i] = out[i] ^ (out[i]<<(1<<j));
            n[i] = out[i] >> (64 - (1<<j));
        }
        SecNonZeroB(b, n);
        for (int i = 0; i<MASKSIZE; i++){
            bp[i] = -b[i];
        }
        //Il faut écrire not(bp[0]);
        SecAnd(t, t, bp);
        for (int i = 0; i<MASKSIZE; i++){
            out[i] = out[i] ^ t[i];
        }
        //B2A_bit();
        for (int i = 0; i<MASKSIZE; i++){
            e[i] = e[i] + (b[i]<<j);
        }
    }
}
