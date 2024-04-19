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
    uint64_t t[MASKSIZE],l[MASKSIZE],r[MASKSIZE],t2[MASKSIZE];
    for(size_t i= 0; i < MASKSIZE; i++) t[i] = in[i];
    int32_t bitsize = 8*sizeof(in[0]);
    int32_t len=bitsize/2;
    while(len>=1){
        for(size_t i=0; i<MASKSIZE;i++){
            t2[i] = (t[i]<<(63-2*len))>>(63-2*len+len);
        }
        RefreshXOR(l,t2,(1<<len),MASKSIZE);
        for(size_t i=0; i<MASKSIZE;i++){
            r[i] = (t[i]<<(63-len))>>(63-len);
        }
        SecOr(t,l,r);
        len = len >> 1;
    }
    for(size_t i = 0; i <MASKSIZE; i++) out[i] = t[i]&1; // (t_i^{(1)})
}

void SecFPR(MaskedB x, MaskedB s, MaskedA e, MaskedB z){
    MaskedB eb,b,za,ea,xb;
    e[0] += 1076;
    A2B(eb,e,(1<<16),MASKSIZE);
    for(size_t i = 0; i <MASKSIZE; i++) b[i] = -((e[i]&(1<<15))>>15);
    b[0] = ~b[0];
    SecAnd(za,z,b);
    for(size_t i = 0; i < MASKSIZE; i++) b[i] = -((za[i]&(1<<54))>>54);
    SecAnd(ea,eb,b);
    for(size_t i = 0; i <MASKSIZE; i++) b[i] = (za[i]&(1<<54)>>54);
    SecAdd(eb,ea,b,(1<<16),4);
    RefreshXOR(eb,eb,(1<<16),MASKSIZE);
    RefreshXOR(s,s,1,MASKSIZE);
    for(size_t i = 0; i < MASKSIZE ;i++) xb[i] = (s[i]&1 << 63) ^ ((eb[i]&(0x7ff))<<52)^(za[i]&0x3ffffffffffff8);
    for(size_t i = 0; i < MASKSIZE; i++) {eb[i] = za[i]&1; b[i] = (za[i]&(1<<2))>>2;}
    RefreshXOR(eb,eb,1,MASKSIZE);
    SecOr(ea,eb,b);
    for(size_t i = 0; i<MASKSIZE;i++) b[i] = (za[i]&0x2)>>1;
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
SecFprNorm64(MaskedB out, MaskedA e, uint64_t mod){
    MaskedB t, n, b, bp;
    for (int j = 5 ; j>=0; j--){
        for (int i = 0; i< MASKSIZE; i++){
            t[i] = out[i] ^ (out[i]<<(1<<j));
            n[i] = out[i] >> (64 - (1<<j));
        }
        SecNonZeroB(b, n); // Les éléments de b sont des bits.
        for (int i = 1; i<MASKSIZE; i++){
            bp[i] = -b[i];
        }
        bp[0] = b[0];
        //Il faut écrire not(bp[0]);
        SecAnd(t, t, bp);
        for (int i = 0; i<MASKSIZE; i++){
            out[i] = out[i] ^ t[i];
        }
        B2A_bit(b, e, mod );
        for (int i = 0; i<MASKSIZE; i++){
            e[i] = e[i] + (b[i]<<j);
        }
    }
}



void SecFprAdd(MaskedB out, MaskedB in1, MaskedB in2, uint64_t mod){
    //PART 1 : BEFORE EXTRACTING (S,E,M)
    MaskedB in1m, in2m, d, b, bp, cs, m, x_63, dp;
    for (int i = 0; i<MASKSIZE; i++){
        in1m[i] = (in1[i]<<2)>>2;
        in2m[i] = (in2[i]<<2)>>2;
    }
    in2m[0] = -in2m[0] -1;
    SecAdd(d, in1m, in2m, ((uint64_t)1<<63), 64);      //d=xm-ym-1
    for (int i = 1; i<MASKSIZE; i++){
        dp[i] = d[i];
    }
    dp[0] = -d[0]-1;
    SecNonZeroB(b, dp);
    dp[0] = -(d[0] ^ ((uint64_t)1<<63)) -1;
    SecNonZeroB(bp,d);
    for (int i = 1; i<MASKSIZE; i++){
        dp[i] = b[i];
    }
    dp[0] = -b[0]-1;
    for (int i = 0; i<MASKSIZE; i++){
        x_63[i] = in1m[i]>>63; //Le 63 à valider
    }
    SecAnd(cs, b, x_63);
    for (int i = 0; i<MASKSIZE; i++){
        x_63[i] = (d[i]>>63) ^ b[i] ^ bp[i];
    }
    SecOr(cs, cs, x_63);
    for (int i = 0; i<MASKSIZE; i++){
        x_63[i] = in1[i] ^ in2[i];
        cs[i] = -cs[i];
    }
    SecAnd(m, x_63, cs);
    for (int i = 0; i<MASKSIZE; i++){
        in1[i] = in1[i] ^ m[i];
        in2[i] = in2[i] ^ m[i];
    }
    //PART 2 : EXTRACTING (S,E,M)
    MaskedB mx, my, sx, sy;
    MaskedA ex, ey;
    for (int i = 0; i<MASKSIZE; i++){
        //52 low bits. 
        mx[i] = (in1[i]<<12)>>12; 
        my[i] = (in2[i]<<12)>>12;
        //11 bits just after the 52 first bits.
        ex[i] = (uint32_t)(in1[i]>>52);
        ex[i] = ex[i] - (ex[i]&0b100000000000); 
        ey[i] = (uint32_t)(in2[i]>>52);
        ey[i] = ey[i] - (ey[i]&0b100000000000);
        //1 top bit.
        sx[i] = in1[i]>>63;
        sy[i] = in2[i]>>63;
    }
    //PART 3 : OPERATION ON (S,E,M)
    MaskedA c, temp, s;
    MaskedB cp, myp, tempb, un, z;
    for (int i = 0; i<MASKSIZE; i++){
        mx[i]<<=3;
        my[i]<<=3;
        ex[i] -=1078;
        ey[i] -=1078;
        c[i] = ex[i] - ey[i];
        temp[i] = c[i];
    }
    temp[0] -= 60;
    A2B(cp, temp, mod, MASKSIZE);
    for (int i = 0; i<MASKSIZE; i++){
        tempb[i] = -((cp[i]>>16)&1); 
        //si        cp[i](16)    = 0 
        //---> not(cp[i](16))    = 0b111...1111
        //---> not(cp[i](16)) +1 = 0b000...0000 (+ 1b000...0000)
        // and ---> my = 0
        //si        cp[i](16)    = 1 
        //---> not(cp[i](16))    = 0b111...1110
        //---> not(cp[i](16)) +1 = 0b111...1111
        // and ---> my = my
    }
    SecAnd(my, my, tempb);
    for (int i = 0; i<MASKSIZE; i++) temp[i] = c[i]&0b11111;
    SecFprUrsh(my, my, temp);
    for (int i = 1; i<MASKSIZE; i++) {
        myp[i] = my[i];
        un[i] = 0;
    }
    un[0] = 1;
    myp[0] = -my[0] - 1;
    SecAdd(tempb, myp, un,((uint64_t)1<<63), 64);
    for (int i = 0; i<MASKSIZE; i++) s[i] = (-(sx[i] ^ sy[i]))&1; // voir pour la taille ici.
    //Refresh(my); //A MODIFIER 
    for (int i = 0; i<MASKSIZE; i++) tempb[i] = my[i] ^ myp[i];
    SecAnd(myp, tempb, s);
    for (int i = 0; i<MASKSIZE; i++) my[i] = my[i] ^ myp[i];
    SecAdd(z, mx, my,((uint64_t)1<<63), 64);
    SecFprNorm64(z,ex, mod);
    for (int i = 0; i<MASKSIZE; i++) b[i] = z[i]&0b111111111;
    SecNonZeroB(b,b);
    for (int i = 0; i<MASKSIZE; i++) {
        z[i] = z[i]>>9;
        z[i] -= z[i]&1;
        z[i] += b[i];
        ex[i]+= 9;
    }
}