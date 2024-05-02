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
    A2B(t,in, mod);
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
    uint64_t len = 32;
    while(len>=1){
        uint64_t mask =0;
        for (int i = 0; i<len; i++) mask += (uint64_t)1<<i;
        for(size_t i=0; i<MASKSIZE;i++){
            t2[i] = (t[i]>>(len)) & mask;
        } 
        RefreshXOR(l,t2,((uint64_t)1<<len),MASKSIZE);
        for(size_t i=0; i<MASKSIZE;i++){
            r[i] = t[i] & mask;
        }
        SecOr(t,l,r);

        len = len >> 1;
    }
    
    for(size_t i = 0; i <MASKSIZE; i++) out[i] = t[i]&1; // (t_i^{(1)})

}

void SecFpr(MaskedB x, MaskedB s, MaskedA e, MaskedB z){

    MaskedB eb,b,za,ea,xb, f;
    uint64_t res;

    e[0] += 1076;
    A2B(eb, e, 1<<16);

    for (int i = 0; i<MASKSIZE; i++) b[i] =  -((eb[i]>>15)&1);
    b[0] = ~b[0];

    SecAnd(z, z, b, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) b[i] =  -((z[i]>>54)&1);

    SecAnd(eb,eb,b, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) b[i] =  ((z[i]>>54)&1);

    SecAdd(eb, eb, b,0, 8, MASKSIZE);


    RefreshXOR(eb,eb,0,MASKSIZE);
    RefreshXOR(s,s,0,MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) x[i] = (s[i]<<63) ^ ((eb[i])<<52) ^ ((z[i]>>2)& 0xfffffffffffff);

    for (int i = 0; i<MASKSIZE; i++) b[i] =  ((z[i])&1);

    for (int i = 0; i<MASKSIZE; i++) za[i] =  ((z[i]>>2)&1);

    RefreshXOR(b,b,0,MASKSIZE);
    SecOr(f,b, za);

    for (int i = 0; i<MASKSIZE; i++) za[i] =  ((z[i]>>1)&1);
    SecAnd(f, f, za, MASKSIZE);
   
    SecAdd(x,f,x,0, 8,MASKSIZE);

}

/*------------------------------------------------
SecFprUrsh  :   Secure right-shift
input       :   Boolean masking in (MaskedB)
                6-bits Integer vector c
output      :   Boolean masking out (MaskedB)
------------------------------------------------*/

void SecFprUrsh(MaskedB out, MaskedB in1, MaskedA c){
    MaskedB m;
    MaskedB in;
    for (int i = 0; i<MASKSIZE; i++) in[i] = in1[i];
    for (int i = 0; i<MASKSIZE; i++) m[i] = 0;
    m[0] = ((uint64_t)1)<<63;
    for (int j =0; j< MASKSIZE; j++){
        vecRightRotate(in, c[j]);
        RefreshMasks(in, MASKSIZE);
        vecRightRotate(m, c[j]);
        RefreshMasks(m, MASKSIZE);
    }
    uint64_t len = 1;
    while(len<=32){
        for (int i = 0; i < MASKSIZE; i++){
            m[i] = m[i] ^ (m[i]>>len);
        }
        len = len << 1;
    }
    MaskedB temp;
    SecAnd(temp, in, m,MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++){
        out[i] = temp[i] ^ in[i];
        out[i]^= temp[i] & 1;
    }
    MaskedB b;
    SecNonZeroB(b, out);
    for (int i = 0; i<MASKSIZE; i++){
        out[i] = (temp[i] - (temp[i] & (uint64_t)1)); //| b[i];
    }
}

void SecFprUrsh2(MaskedB out, MaskedB in, MaskedA c){
    MaskedB m, inp;

    for (int i = 0; i<MASKSIZE; i++) {
        m[i] = 0;
        inp[i] = in[i];
    }
    m[0] = ((uint64_t)1)<<63;
    for (int j =0; j< MASKSIZE; j++){
        vecRightRotate(inp, c[j]);
        RefreshMasks(inp, MASKSIZE);
        vecRightRotate(m, c[j]);
        RefreshMasks(m, MASKSIZE);
    }
    uint64_t len = 1;
    while(len<=32){
        for (int i = 0; i < MASKSIZE; i++){
            m[i] = m[i] ^ (m[i]>>len);
        }
        len = len << 1;
    }
    SecAnd(out, inp, m,MASKSIZE);
}

void SecFprUrsh3(MaskedB out, MaskedB out2, MaskedB in, MaskedA c){
    MaskedB m, inp;

    for (int i = 0; i<MASKSIZE; i++) {
        m[i] = 0;
        inp[i] = in[i];
    }
    m[0] = ((uint64_t)1)<<63;
    for (int j =0; j< MASKSIZE; j++){
        vecRightRotate(inp, c[j]);
        RefreshMasks(inp, MASKSIZE);
        vecRightRotate(m, c[j]);
        RefreshMasks(m, MASKSIZE);
    }
    uint64_t len = 1;
    while(len<=32){
        for (int i = 0; i < MASKSIZE; i++){
            m[i] = m[i] ^ (m[i]>>len);
        }
        len = len << 1;
    }
    SecAnd(out, inp, m,MASKSIZE);
    for (int i = 0; i < MASKSIZE; i++){
            m[i] = ~m[i];
    }
    SecAnd(out2, inp, m, MASKSIZE);
}

void 
SecFprNorm64(MaskedB out, MaskedA e, uint64_t mod){
    MaskedB t, n, b, bp, tp;
    MaskedA ba;
    e[0]= e[0]-63;
    for (int j = 5 ; j>=0; j--){
        for (int i = 0; i< MASKSIZE; i++){
            t[i] = out[i] ^ (out[i]<<(1<<j));
            n[i] = out[i] >> (64 - (1<<j));
        }
        SecNonZeroB(b, n); 
        for (int i = 0; i<MASKSIZE; i++){
            bp[i] = -b[i];
        }

        bp[0] = ~bp[0];
        SecAnd(t, t, bp, MASKSIZE);

        for (int i = 0; i<MASKSIZE; i++){
            out[i] = out[i] ^ t[i];
        }
        B2A_bit(ba, b, mod );
        for (int i = 0; i<MASKSIZE; i++){
            e[i] = e[i] + (ba[i]<<j) %mod;
        }
    }
}



void SecFprMul(MaskedB out, MaskedB x, MaskedB y){
    MaskedB s,sbx,sby,p1,p2,b,z,z2,w,w2,bx,by,d,ebx,eby,mbxu,mbxd,mbyu,mbyd, b1,b2;
    MaskedA e,eax,eay,wa,p3,p4,mxu,mxd,myu,myd,sx,sy;
    uint64_t test, test2;
    //EXTRACTION
    for(size_t i =0; i < MASKSIZE;i++){
        sbx[i] = (x[i]>>63);
        sby[i] = (y[i]>>63);
        ebx[i] = ((x[i]<<1)>>53);
        eby[i] = ((y[i]<<1)>>53);
        mbxd[i] = x[i] & 0xfffffffffffff;
        mbxu[i]=0;
        mbyd[i] = y[i] & 0xfffffffffffff;
        mbyu[i]=0;
    }

    SecNonZeroB(b1, x);
    SecNonZeroB(b2, y);

    for(size_t i =0; i<MASKSIZE; i++){
        mbxd[i] = mbxd[i] ^ (b1[i]<<52);
        mbyd[i] = mbyd[i] ^ (b2[i]<<52);
    }

    B2A(eax,ebx,(1<<16),MASKSIZE);
    B2A128(mxu,mxd,mbxu,mbxd,MASKSIZE);
    B2A(eay,eby,(1<<16),MASKSIZE);
    B2A128(myu,myd,mbyu,mbyd,MASKSIZE);


    for(size_t i= 0; i <MASKSIZE; i++) s[i] = sbx[i]^sby[i];

    //printf("\n\n==============Recherche erreur multiplication=============\n");

    e[0] = eax[0] + eay[0] -2100;
    for(size_t i =1;i < MASKSIZE;i++) e[i] = eax[i] + eay[i];

    SecMult128(p3,p4,mxu,mxd,myu,myd);

    A2B128(p1,p2,p3,p4,MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++){
        b[i] = (p2[i]<<(13))>>13;
        z[i] = p2[i]>>50;
        z2[i] = p2[i]>>51;
    }
    for (int i = 0; i<MASKSIZE; i++){
        z[i]^= (p1[i]<<23)>>9;
        z2[i]^= (p1[i]<<22)>>9;
    }

    SecNonZeroB(b,b);
    for (int i = 0; i<MASKSIZE; i++){
        z2[i] = z2[i]^z[i];
        //w[i] = (p1[i]<<23)>>63;
        w[i] = (p1[i]>>41)&1;
        w2[i] = -w[i];
    }

    RefreshXOR(w2, w2, 0,  MASKSIZE);
    SecAnd(z2, z2, w2, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++){
        z[i] = z2[i]^z[i];
    }

    SecOr(z, z, b );

    B2A_bit(wa, w, 1<<16);

    for(int i =0; i<MASKSIZE; i++) e[i] = e[i] + wa[i];

    SecNonZeroB(bx, ebx);
    SecNonZeroB(by, eby);

    SecAnd(d,bx,by, MASKSIZE);

    for (int i =0; i<MASKSIZE; i++) d[i] = -(d[i]&1);
    SecAnd(z, z, d, MASKSIZE);

    SecFpr(out, s, e, z);
}




void SecFprAdd(MaskedB out, MaskedB in1, MaskedB in2, uint64_t mod){
    //PART 1 : BEFORE EXTRACTING (S,E,M)
    MaskedB in1m, in2m, d, b, bp, cs, m, x_63, dp;

    for (int i = 0; i<MASKSIZE; i++){
        in1m[i] = (in1[i]<<1)>>1;
        in2m[i] = (in2[i]<<1)>>1;
    }
    in2m[0] = ~in2m[0];

    SecAdd(d, in1m, in2m, 0, 8, MASKSIZE);      //d=xm-ym-1

    for (int i = 1; i<MASKSIZE; i++){
        dp[i] = d[i];
    }
    dp[0] = ~d[0];
    
    SecNonZeroB(b, dp);

    dp[0] = ~(d[0] ^ ((uint64_t)1<<63));

    SecNonZeroB(bp,dp);

    for (int i = 1; i<MASKSIZE; i++){
        dp[i] = b[i];
    }

    dp[0] = ~b[0];
    for (int i = 0; i<MASKSIZE; i++){
        x_63[i] = in1[i]>>63;
    }

    SecAnd(cs, dp, x_63,MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++){
        x_63[i] = (d[i]>>63) ^ b[i] ^ bp[i];
    }

    SecOr(cs, cs, x_63);

    for (int i = 0; i<MASKSIZE; i++){
        x_63[i] = in1[i] ^ in2[i];
    }
    // OPERATION : -cs. when cs = 0 ----> cps = 0
    //                  when cs = 1 ----> cps = 0xff..ff
    MaskedB csp;
    for (int j = 0; j<MASKSIZE; j++){
        csp[j] = 0;
        for (int i = 0; i<64; i++){
            csp[j] ^= ((cs[j])<<i);
        }
    }
    RefreshMasks(csp,MASKSIZE);

    RefreshMasks(csp,MASKSIZE);

    SecAnd(m, x_63, csp,MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++){
        in1[i] = in1[i] ^ m[i];
        in2[i] = in2[i] ^ m[i];
    }

    //PART 2 : EXTRACTING (S,E,M)
    MaskedB mx, my, sx, sy, exp, eyp;
    MaskedA ex, ey;
    for (int i = 0; i<MASKSIZE; i++){
        //52 low bits. 
        mx[i] = (in1[i]<<12)>>12; 
        my[i] = (in2[i]<<12)>>12;
        //11 bits just after the 52 first bits.
        exp[i] = (uint32_t)(in1[i]>>52);
        exp[i] = exp[i] - (exp[i]&0b100000000000); 
        eyp[i] = (uint32_t)(in2[i]>>52);
        eyp[i] = eyp[i] - (eyp[i]&0b100000000000);
        //1 top bit.
        sx[i] = in1[i]>>63;
        sy[i] = in2[i]>>63;
    }

    //PART 3 : OPERATION ON (S,E,M)
    MaskedA c, temp, s;
    MaskedB cp, myp, tempb, un, z;

    mx[0] += (uint64_t)1 << 52;
    RefreshMasks(mx, MASKSIZE);
    my[0] += (uint64_t)1 << 52;
    RefreshMasks(my, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++){
        mx[i]<<=3;
        my[i]<<=3;
    }

    B2A(ex, exp, (1<<16), MASKSIZE);
    B2A(ey, eyp, (1<<16), MASKSIZE);
    ex[0] -=1078;
    ey[0] -=1078;

    for (int i = 0; i < MASKSIZE; i++){
        c[i] = ex[i] - ey[i];
        temp[i] = c[i];
    }
    temp[0] -= 60;

    A2B(cp, temp, (1<<16));

    for (int i = 0; i<MASKSIZE; i++){
        tempb[i] = -((cp[i]>>15)&1); 
        //si        cp[i](16)    = 0 
        //---> not(cp[i](16))    = 0b111...1111
        //---> not(cp[i](16)) +1 = 0b000...0000 (+ 1b000...0000)
        // and ---> my = 0
        //si        cp[i](16)    = 1 
        //---> not(cp[i](16))    = 0b111...1110
        //---> not(cp[i](16)) +1 = 0b111...1111
        // and ---> my = my
    }

    SecAnd(my, my, tempb,MASKSIZE);

    MaskedA tempA;
    for (int i =0; i<MASKSIZE; i++) tempA[i] = c[i]& 0b111111;
    SecFprUrsh(my, my, tempA);

    for (int i = 1; i<MASKSIZE; i++) {
        myp[i] = my[i];
        un[i] = 0;
    }
    un[0] = 1;
    myp[0] = ~my[0];
    
    SecAdd(myp, myp, un,0, 8,MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) s[i] = (-(sx[i] ^ sy[i]));
    RefreshMasks(my, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) tempb[i] = my[i] ^ myp[i];

    SecAnd(myp, tempb, s,MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) my[i] = my[i] ^ myp[i];

    SecAdd(z, mx, my,0, 8,MASKSIZE);

    SecFprNorm64(z,ex, mod);

    for (int i = 0; i<MASKSIZE; i++) b[i] = z[i]&0b1111111111;
    SecNonZeroB(b,b);
    for (int i = 0; i<MASKSIZE; i++) {
        z[i] = z[i]>>11;
        z[i] = z[i] - (z[i]&1) + b[i];
    }
    ex[0]+= 10;

    //PART 6 : SEC FPR

    //SecFpr(out, sx, ex, z );

    ex[0] += 1076;

    MaskedB e, bi, zi;

    A2B(e,ex,1<<16);

    for (int i = 0; i<MASKSIZE; i++) bi[i] = -((e[i]>>15)&1);
    bi[0] = ~bi[0];

    SecAnd(z, z, bi, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) zi[i] = -((z[i]>>52)&1);

    SecAnd(e, e, zi, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) out[i] = (((z[i]- (z[i]&0b11))<<12)>>12) + ((e[i]&0b11111111111)<<52) + (sx[i]<<63);
}


void 
SecFprTrunc(MaskedB out, MaskedB in){

//PART 1 : Extract mx, ex, sx

    MaskedB mx, sx, exp, c, c0, cp, cd;
    MaskedA ex, cx;
    for (int i = 0; i<MASKSIZE; i++){
        //52 low bits. 
        mx[i] = (in[i]<<12)>>12; 
        //11 bits just after the 52 first bits.
        exp[i] = (uint32_t)(in[i]>>52);
        exp[i] = exp[i] - (exp[i]&0b100000000000); 
        //1 top bit.
        sx[i] = in[i]>>63;
    }

    //Ajout du bit implicite
    mx[0] += (uint64_t)1 << 52;
    RefreshMasks(mx, MASKSIZE);

    B2A(ex, exp, (1<<16), MASKSIZE);

//PART 2 : Check if e-1023 is positive or not
    
    for(int i = 0; i<MASKSIZE; i++) cx[i] = ex[i];
    cx[0] = ex[0] - 1023;

    A2B(c, cx, 1<<16);

    for(int i = 0; i<MASKSIZE; i++) c[i] = (c[i]>>15)&1;
    //SecNonZeroB(c,c);

    for (int j = 0; j<MASKSIZE; j++){
        c0[j] = 0;
        for (int i = 0; i<64; i++){
            c0[j] ^= ((c[j])<<i);
        }
        c0[j] = ~c0[j];
    }
    RefreshMasks(c0,MASKSIZE);

    MaskedB mout, sout; 
    MaskedA eout; 

    SecAnd(mout, c0, mx, MASKSIZE); //if c0 = 00000 ---> at the end we change eout
    for (int i = 0; i<MASKSIZE; i++) eout[i] = ex[i];
    for (int i = 0; i<MASKSIZE; i++) sout[i] = sx[i];

//PART 3 : Check if e-1023 is superior than 52
    cx[0] = cx[0] - 52;
    A2B(c, cx, 1<<16);
    for(int i = 0; i<MASKSIZE; i++) c[i] = (c[i]>>15)&1;
    //SecNonZeroB(c,c);

    for (int j = 0; j<MASKSIZE; j++){
        cp[j] = 0;
        for (int i = 0; i<64; i++){
            cp[j] ^= ((c[j])<<i);
        }
    }
    RefreshMasks(cp,MASKSIZE);

    cx[0] = cx[0] + 52;

    A2B(c, cx, 1<<16);

    SecAnd(c, c, cp, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) c[i] = (c[i]);

    B2A(cx, c, 1<<16, MASKSIZE);

//PART 4 : SecFprUrsh

    cx[0] = cx[0] - 52 ;

    for (int i = 0; i<MASKSIZE; i++) cd[i] = (-(cx[i]));

    A2B(c, cd, 1<<16);

    SecAnd(c, c, cp, MASKSIZE);

    B2A(cd, c, 1<<16, MASKSIZE);

    SecFprUrsh2(mout, mout, cd);

    for (int i = 0; i<MASKSIZE; i++) eout[i] += cd[i];

//PART 5 : Normalization

    SecFprNorm64(mout, eout, 1<<16);

    for (int i = 0; i<MASKSIZE; i++){
        mout[i] = mout[i]>>11;
        //mout[i] = mout[i] - ((uint64_t)1<<52);
    }
    
    eout[0] = eout[0] +11;

    MaskedB eoutp; 
    A2B(eoutp, eout, 1<<16);

    SecAnd(eoutp,eoutp, c0, MASKSIZE);
    SecAnd(sout, sout, c0, MASKSIZE);


    for (int i = 0; i<MASKSIZE; i++) out[i] = ((mout[i]<<12)>>12) + (eoutp[i]<<52) + (sout[i]<<63);


}



void SecFprScalPtwo(MaskedB out, MaskedB in1, uint16_t ptwo){
   
    //PART 2 : EXTRACTING (S,E,M)
    MaskedB mx, my, sx, sy, exp, eyp, b;
    MaskedA ex, ey;
    for (int i = 0; i<MASKSIZE; i++){
        //52 low bits. 
        mx[i] = (in1[i]<<12)>>12; 
        //11 bits just after the 52 first bits.
        exp[i] = (uint32_t)(in1[i]>>52);
        exp[i] = exp[i] - (exp[i]&0b100000000000); 
        //1 top bit.
        sx[i] = in1[i]>>63;
    }

    SecNonZeroB(b, exp); // test si exp = 0
    //si exp = 0; Addition doesn't count
    for (int i =0; i<MASKSIZE; i++) b[i] = -b[i];

    B2A(ex, exp, 1<<16, MASKSIZE);
    ex[0] += ptwo;

    A2B(exp, ex, 1<<16);

    SecAnd(exp, exp, b, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) out[i] = (mx[i]) ^ ((exp[i])<<52) ^ (sx[i]<<63);
}

void SecFprDivPtwo(MaskedB out, MaskedB in1, uint16_t ptwo){
   
    //PART 2 : EXTRACTING (S,E,M)
    MaskedB mx, my, sx, sy, exp, eyp, b;
    MaskedA ex, ey;
    for (int i = 0; i<MASKSIZE; i++){
        //52 low bits. 
        mx[i] = (in1[i]<<12)>>12; 
        //11 bits just after the 52 first bits.
        exp[i] = (uint32_t)(in1[i]>>52);
        exp[i] = exp[i] - (exp[i]&0b100000000000); 
        //1 top bit.
        sx[i] = in1[i]>>63;
    }

    SecNonZeroB(b, exp); // test si exp = 0
    //si exp = 0; Addition doesn't count
    for (int i =0; i<MASKSIZE; i++) b[i] = -b[i];

    B2A(ex, exp, 1<<16, MASKSIZE);
    ex[0] -= ptwo;

    A2B(exp, ex, 1<<16);

    SecAnd(exp, exp, b, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) out[i] = (mx[i]) ^ ((exp[i])<<52) ^ (sx[i]<<63);
}



void 
SecFprFloor(MaskedB out, MaskedB in){
uint64_t res;

//PART 1 : Extract mx, ex, sx

    MaskedB mx, sx, exp, c, c0, cp, cd;
    MaskedA ex, cx;
    for (int i = 0; i<MASKSIZE; i++){
        //52 low bits. 
        mx[i] = (in[i]<<12)>>12; 
        //11 bits just after the 52 first bits.
        exp[i] = (uint32_t)(in[i]>>52);
        exp[i] = exp[i] - (exp[i]&0b100000000000); 
        //1 top bit.
        sx[i] = in[i]>>63;
    }
    //Ajout du bit implicite
    mx[0] += (uint64_t)1 << 52;
    RefreshMasks(mx, MASKSIZE);

    B2A(ex, exp, (1<<16), MASKSIZE);

//PART 2 : Check if e-1023 is positive or not
    
    for(int i = 0; i<MASKSIZE; i++) cx[i] = ex[i];
    cx[0] = ex[0] - 1023;

    A2B(c, cx, 1<<16);

    for(int i = 0; i<MASKSIZE; i++) c[i] = (c[i]>>15)&1;
    SecNonZeroB(c,c);

    for (int j = 0; j<MASKSIZE; j++){
        c0[j] = 0;
        for (int i = 0; i<64; i++){
            c0[j] ^= ((c[j])<<i);
        }
        c0[j] = ~c0[j];
    }
    RefreshMasks(c0,MASKSIZE);

    MaskedB mout, sout; 
    MaskedA eout; 

    SecAnd(mout, c0, mx, MASKSIZE); //if c0 = 00000 ---> at the end we change eout
    for (int i = 0; i<MASKSIZE; i++) eout[i] = ex[i];
    for (int i = 0; i<MASKSIZE; i++) sout[i] = sx[i];

//PART 3 : Check if e-1023 is superior than 52
    cx[0] = cx[0] - 52;
    A2B(c, cx, 1<<16);
    for(int i = 0; i<MASKSIZE; i++) c[i] = (c[i]>>15)&1;
    SecNonZeroB(c,c);

    MaskedB cs;

    for (int j = 0; j<MASKSIZE; j++){
        cp[j] = -c[j];
        cs[j] = c[j]; 
    }
    RefreshMasks(cp,MASKSIZE);
    A2B(c, cx, 1<<16);
    SecAnd(c, c, cp, MASKSIZE);
    B2A(cx, c, 1<<16, MASKSIZE);
//PART 4 : SecFprUrsh
    for (int i = 0; i<MASKSIZE; i++) cd[i] = (-(cx[i]));

    MaskedB Int_test;

    SecFprUrsh3(mout,Int_test, mout, cd);
    MaskedB b;
    SecNonZeroB(b, Int_test);

    //SecAnd(cs, cs, sout, MASKSIZE);
    SecAnd(cs, b, sout, MASKSIZE);

    SecAdd(mout, mout, cs,0,8,  MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) eout[i] += cd[i];

//PART 5 : Normalization

    SecFprNorm64(mout, eout, 1<<16);

    for (int i = 0; i<MASKSIZE; i++){
        mout[i] = mout[i]>>11;
        //mout[i] = mout[i] - ((uint64_t)1<<52);
    }
    
    eout[0] = eout[0] +11;

    MaskedB eoutp; 
    A2B(eoutp, eout, 1<<16);


    MaskedB bd;
    for (int i = 0; i<MASKSIZE; i++) bd[i] = -sout[i];

    SecOr(bd, bd, c0);

    SecAnd(eoutp,eoutp, bd, MASKSIZE);
    SecAnd(sout, sout, bd, MASKSIZE);


    for (int i = 0; i<MASKSIZE; i++) out[i] = ((mout[i]<<12)>>12) + (eoutp[i]<<52) + (sout[i]<<63);

}

void 
SecFprRound(MaskedB out, MaskedB in){

//PART 1 : Extract mx, ex, sx

    MaskedB mx, sx, exp, c, c0, cp, cd;
    MaskedA ex, cx;
    for (int i = 0; i<MASKSIZE; i++){
        //52 low bits. 
        mx[i] = (in[i]<<12)>>12; 
        //11 bits just after the 52 first bits.
        exp[i] = (uint32_t)(in[i]>>52);
        exp[i] = exp[i] - (exp[i]&0b100000000000); 
        //1 top bit.
        sx[i] = in[i]>>63;
    }

    //Ajout du bit implicite
    mx[0] += (uint64_t)1 << 52;
    RefreshMasks(mx, MASKSIZE);

    B2A(ex, exp, (1<<16), MASKSIZE);

//PART 2 : Check if e-1023 is positive or not
    
    for(int i = 0; i<MASKSIZE; i++) cx[i] = ex[i];
    cx[0] = ex[0] - 1022;

    A2B(c, cx, 1<<16);

    for(int i = 0; i<MASKSIZE; i++) c[i] = (c[i]>>15)&1;
    SecNonZeroB(c,c);

    for (int j = 0; j<MASKSIZE; j++){
        c0[j] = 0;
        for (int i = 0; i<64; i++){
            c0[j] ^= ((c[j])<<i);
        }
        c0[j] = ~c0[j];
    }
    RefreshMasks(c0,MASKSIZE);

    MaskedB mout, sout; 
    MaskedA eout; 

    SecAnd(mout, c0, mx, MASKSIZE); //if c0 = 00000 ---> at the end we change eout
    for (int i = 0; i<MASKSIZE; i++) eout[i] = ex[i];
    for (int i = 0; i<MASKSIZE; i++) sout[i] = sx[i];

//PART 3 : Check if e-1023 is superior than 52
    cx[0] = cx[0] - 53;
    A2B(c, cx, 1<<16);
    for(int i = 0; i<MASKSIZE; i++) c[i] = (c[i]>>15)&1;
    SecNonZeroB(c,c);

    MaskedB rshORnot;

    for (int i = 0; i<MASKSIZE; i++) rshORnot[i] = -c[i];

    uint64_t res;

    for (int j = 0; j<MASKSIZE; j++){
        cp[j] = 0;
        for (int i = 0; i<64; i++){
            cp[j] ^= ((c[j])<<i);
        }
    }
    RefreshMasks(cp,MASKSIZE);

    cx[0] = cx[0] + 52;
    
    cx[0] = cx[0] - 51 ;

    A2B(c, cx, 1<<16);

    SecAnd(c, c, cp, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) c[i] = (c[i]);

    B2A(cx, c, 1<<16, MASKSIZE);

//PART 4 : SecFprUrsh

    for (int i = 0; i<MASKSIZE; i++) cd[i] = (-(cx[i]));

    SecFprUrsh2(mout, mout, cd);
    
// ICI POUR AVOIR LA PARTIE ENTIERE. 
    MaskedB b;
    for (int i = 0; i<MASKSIZE; i++) b[i] = mout[i]&1;
    SecNonZeroB(b, b);

    SecAnd(b, b, rshORnot, MASKSIZE);

    MaskedB moutp1, moutp2;
    MaskedB e1, e2;
    MaskedA etemp;

    for (int i = 0; i<MASKSIZE; i++){
        eout[i] += cd[i];
        e1[i] = 0;
        e2[i] = 0;
    } 
    e1[0] = 1;

    for (int i = 0; i<MASKSIZE; i++) {
        moutp1[i] = mout[i]>>1;
        moutp2[i] = mout[i];
    }
    SecAnd(moutp1, moutp1, rshORnot, MASKSIZE);
    SecAnd(e1, e1, rshORnot, MASKSIZE);
    for (int i = 0; i<MASKSIZE; i++) rshORnot[i] = ~rshORnot[i];
    SecAnd(moutp2, moutp2, rshORnot, MASKSIZE);
    SecAnd(e2, e2, rshORnot, MASKSIZE);

    SecOr(mout, moutp1, moutp2);
    SecOr(e1, e1, e2);

    SecAdd(mout, mout, b ,64,6,  MASKSIZE);

    B2A(etemp, e1, 1<<16, MASKSIZE);
    //SecAdd(mout, mout, sout ,64,6,  MASKSIZE);

//FIN PARTIE ENTIERE

    for (int i = 0; i<MASKSIZE; i++){
        eout[i] += etemp[i];
    }
    

//PART 5 : Normalization

    SecFprNorm64(mout, eout, 1<<16);

    for (int i = 0; i<MASKSIZE; i++){
        mout[i] = mout[i]>>11;
        //mout[i] = mout[i] - ((uint64_t)1<<52);
    }
    
    eout[0] = eout[0] +11;

    MaskedB eoutp; 
    A2B(eoutp, eout, 1<<16);

    MaskedB bp;
    for (int i =0; i<MASKSIZE; i++) bp[i] = b[i];

    SecOr(bp, c0, bp);

    SecAnd(eoutp,eoutp, bp, MASKSIZE);

    SecAnd(sout, sout, bp, MASKSIZE);


    for (int i = 0; i<MASKSIZE; i++) out[i] = ((mout[i]<<12)>>12) + (eoutp[i]<<52) + (sout[i]<<63);
}




void
Secfpr_expm_p63(MaskedB out, MaskedB x, MaskedB ccs)
{

    static const uint64_t Cp[] = {
		0x4211D0460E8C0000u,
		0x424B2A467E030000u,
		0x42827EE5F8A05000u,
		0x42B71D939DE04500u,
		0x42EA019EB1EDF080u,
		0x431A01A073DE5B8Cu,
		0x4346C16C182D87F5u,
		0x4371111110E066FDu,
		0x4395555555541C3Cu,
		0x43B55555555581FFu,
		0x43D00000000000ADu,
		0x43DFFFFFFFFFFFD2u,
		0x43E0000000000000u
	};

    uint64_t y, ptwo_63;
    MaskedB z, yz, ymask, cmask;

    for (int i = 0; i<MASKSIZE; i++) ymask[i] = 0;
    ymask[0] = Cp[0];

    SecFprScalPtwo(z, x, 63);
    SecFprFloor(z, z);

    uint64_t res;
    for (int u = 1; u < 13; u ++) {
        SecFprMul(yz, z, ymask);
        SecFprDivPtwo(yz, yz, 63);

        for (int i = 0; i<MASKSIZE; i++) cmask[i] = 0;
        cmask[0] = Cp[u];
        yz[0] = yz[0] ^ ((uint64_t)1<<63);

        SecFprAdd(ymask,yz,cmask, 1<<16);
	}

    SecFprScalPtwo(z, ccs, 63);
    SecFprFloor(z, z);
    SecFprMul(ymask, ymask, z);
    SecFprDivPtwo(out, ymask, 63);
}

uint64_t
SecFprBerExp(MaskedB out, MaskedB x, MaskedB ccs){
    uint64_t fpr_ln2_inv = 0x3FF71547652B82FE;
    uint64_t fpr_ln2 = 0x3FE62E42FEFA39EF;

    MaskedB inv_ln2, ln2, s, r, b, es, esa, z;

    MaskB(inv_ln2, fpr_ln2_inv);
    MaskB(ln2, fpr_ln2);

    SecFprMul(s, inv_ln2, x);

    SecFprFloor(s,s);

    uint64_t res;
    UnmaskB(&res, s, MASKSIZE);
    printf("s       = %lu\n", res);
    print_binary_form(res);

    //SecFprScalPtwo(s, s, 63);
    //SecFprScalPtwo(ln2, ln2, 63);

    SecFprMul(r,ln2,s); //problème ici sur l'exposant, à regarder.

    //SecFprDivPtwo(s, s, 63);
    //SecFprDivPtwo(r, r, 126);

    UnmaskB(&res, r, MASKSIZE);
    printf("r       = %lu\n", res);
    print_binary_form(res);


    SecNonZeroB(b, r);
    for (int i =0; i<MASKSIZE; i++) r[i] = r[i]^(b[i]<<63);

    SecFprAdd(r,x,r, 1<<16);


    //SecFprScalPtwo(s, s, 63); // test when s > 64

    //COMPARAISON 
    for (int i = 0; i<MASKSIZE; i++) es[i] = ((s[i]<<1)>>53);
    B2A(esa, es, 1<<16, MASKSIZE);
    esa[0] -= 1029;
    A2B(es,esa, 1<<16);

    for (int i = 0; i<MASKSIZE; i++) b[i] = -((es[i]>>15)&1);
    SecAnd(s, b, s, MASKSIZE);

    UnmaskB(&res, s, MASKSIZE);
    printf("s       = %lu\n", res);
    print_binary_form(res);

    //Create new s : 
    MaskedB news, un;
    MaskB(news, 0x404F800000000000);
    b[0] = ~b[0];

    SecAnd(news, news, b, MASKSIZE);

    SecAdd(s, s, news, 0, 8, MASKSIZE);

    Secfpr_expm_p63(z, r, ccs);

    UnmaskB(&res, z, MASKSIZE);
    printf("z       = %lu\n", res);
    print_binary_form(res);


    MaskB(un, 0xBFF0000000000000); //équivalent -1

    SecFprScalPtwo(z, z, 1);
    SecFprAdd(z,z,un, 1<<16);

    MaskedA cs;
    esa[0] -= 46;
    for (int i = 0; i<MASKSIZE; i++) cs[i] = -(esa[i]);
    for (int i = 0; i<MASKSIZE; i++) s[i] = (s[i]&((uint64_t)(0xfffffffffffff))) + ((uint64_t)1<<52);

    SecFprUrsh2(s, s, cs);
    B2A(cs, s, 1<<16, MASKSIZE);

    MaskedB mz;
    for(int i = 0; i<MASKSIZE; i++){
        mz[i] = (z[i]<<12)>>12;
    }
    mz[0] += (uint64_t)1 << 52;
 
    for (int i = 0; i<MASKSIZE; i++) {
        mz[i] <<= 11;
    }

    SecFprUrsh2(mz, mz, cs);

    //---> DANS mZ, on a les bits à comparer
    UnmaskB(&res, mz, MASKSIZE);
    printf("mz = %lu\n", res);
    print_binary_form(res);
    

   // A2B(ebz, ez, 1<<16);

    //for (int i = 0; i<MASKSIZE; i++) z[i] = (ebz[i]<<52) ^ (mz[i]&0xfffffffffffff);

    uint64_t counter = 64;
    uint64_t w = 0;

    do{
        counter -= 8;

    }while((w==0&(counter>0)));

    /*
    Pour la comparaison.
    b = SecNonZero(w), si b = 0 hors de la boucle.
    b2 = SecNonZero(w^(64)) ----> retourner b2;
    */

    return (w<0);




    UnmaskB(&res, inv_ln2, MASKSIZE);
    printf("inv_ln2 = %lu\n", res);
    print_binary_form(res);
    
    SecFprUrsh2(mz,mz,cs);

    UnmaskB(&res, inv_ln2, MASKSIZE);
    printf("inv_ln2 = %lu\n", res);
    print_binary_form(res);

    
    UnmaskB(&res, inv_ln2, MASKSIZE);
    printf("inv_ln2 = %lu\n", res);
    print_binary_form(res);

    printf("\n");

    UnmaskB(&res, s, MASKSIZE);
    printf("s       = %lu\n", res);
    print_binary_form(res);

    printf("\n");

    UnmaskB(&res, x, MASKSIZE);
    printf("x       = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, r, MASKSIZE);
    printf("r       = %lu\n", res);
    print_binary_form(res);

    UnmaskB(&res, z, MASKSIZE);
    printf("z       = %lu\n", res);
    print_binary_form(res);

    UnmaskB(&res, mz, MASKSIZE);
    printf("mz = %lu\n", res);
    print_binary_form(res);


    return 0;
}

