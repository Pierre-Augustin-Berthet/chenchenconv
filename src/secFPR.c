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
    A2B(t,in);
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







void SecFprUrsh(MaskedB_64 out, MaskedB_64 in, MaskedA c){
    MaskedB m;
    m[0] = ((uint64_t)1)<<63;
    for (int j =0; j< MASKSIZE; j++){
        for (int i = 0; i < MASKSIZE; i++){
            RightRotate(&in[i], c[j]);
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
    MaskedB_64 b;
    SecNonZeroB(b, out);
    for (int i = 0; i<MASKSIZE; i++){
        out[i] = (temp - temp & 1) | b[i];
    }
}
