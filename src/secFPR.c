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
        RefreshXOR(l,t2,(1<<len),MASKSIZE);
        for(size_t i=0; i<MASKSIZE;i++){
            r[i] = t[i] & mask;
        }
        SecOr(t,l,r);

        len = len >> 1;
    }
    
    for(size_t i = 0; i <MASKSIZE; i++) out[i] = t[i]&1; // (t_i^{(1)})

}

void SecFPR(MaskedB x, MaskedB s, MaskedA e, MaskedB z){
    MaskedB eb,b,za,ea,xb;
    e[0] += 1076;
    A2B(eb,e,(1<<16));
    for(size_t i = 0; i <MASKSIZE; i++) b[i] = -((e[i]<<(63-15))>>63);
    b[0] = ~b[0];
    SecAnd(za,z,b,MASKSIZE);
    for(size_t i = 0; i < MASKSIZE; i++) b[i] = -((za[i]<<(63-54))>>63);
    SecAnd(ea,eb,b,MASKSIZE);
    for(size_t i = 0; i <MASKSIZE; i++) b[i] = ((za[i]<<(63-54))>>63);
    SecAdd(eb,ea,b,(1<<16),4,MASKSIZE);
    RefreshXOR(eb,eb,(1<<16),MASKSIZE);
    RefreshXOR(s,s,1,MASKSIZE);
    for(size_t i = 0; i < MASKSIZE ;i++) xb[i] = ((uint64_t)(s[i]&1) << 63) ^ ((eb[i]&(0x7ff))<<52)^((za[i]&0x3ffffffffffff8)>>2);
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
        out[i] = (temp[i] - (temp[i] & (uint64_t)1)) | b[i];
    }
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

void SecFprMul(MaskedB out, MaskedB x, MaskedB y, uint64_t mod){
    MaskedB s,sx,sy,e,ex,ey,p,mx,my,b,z,z2,w,bx,by,d;
    //TO DO Extract sx,ex,mx,sy,ey,my
    for(size_t i= 0; i <MASKSIZE; i++) s[i] = sx[i]^sy[i];
    e[0] = ex[0] + ey[0] -2100;
    for(size_t i=1;i<MASKSIZE;i++) e[i] = ex[i] + ey[i];
    SecMult(d,mx,my,mod);//ATTENTION 128bits!!!
    A2B(p,d,mod); //ATTENTION 128bits!!!!
    for(size_t i =0; i<MASKSIZE;i++) w[i] = (p[i] <<(63-51))>>(63-51); //ATTENTION 128bits!!!
    SecNonZeroB(b,w);
}



void SecFprAdd(MaskedB out, MaskedB in1, MaskedB in2, uint64_t mod){
    //PART 1 : BEFORE EXTRACTING (S,E,M)
    MaskedB in1m, in2m, d, b, bp, cs, m, x_63, dp;
    uint64_t res;

    for (int i = 0; i<MASKSIZE; i++){
        in1m[i] = (in1[i]>>1);
        in2m[i] = (in2[i]>>1);
    }
    in2m[0] = ~in2m[0];

    SecAdd(d, in1m, in2m, ((uint64_t)1<<63), 64, MASKSIZE);      //d=xm-ym-1

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

    //Print value
///*
    UnmaskB(&res, mx, MASKSIZE);
    printf("mx = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, my, MASKSIZE);
    printf("my = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, exp, MASKSIZE);
    printf("exp= %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, eyp, MASKSIZE);
    printf("eyp= %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, sx, MASKSIZE);
    printf("sx = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, sy, MASKSIZE);
    printf("sy = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n\n");

//*/
    //PART 3 : OPERATION ON (S,E,M)
    MaskedA c, temp, s;
    MaskedB cp, myp, tempb, un, z;

    mx[0] += (uint64_t)1 << 52;
    RefreshMasks(mx, MASKSIZE);
    my[0] += (uint64_t)1 << 52;
    RefreshMasks(my, MASKSIZE);

    UnmaskB(&res, mx, MASKSIZE);
    printf("mx = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, my, MASKSIZE);
    printf("my = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    for (int i = 0; i<MASKSIZE; i++){
        mx[i]<<=3;
        my[i]<<=3;
    }
/*
    UnmaskB(&res, mx, MASKSIZE);
    printf("mx = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, my, MASKSIZE);
    printf("my = %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/

    B2A(ex, exp, (1<<16), MASKSIZE);
    B2A(ey, eyp, (1<<16), MASKSIZE);

/* Check this part
    
*//*
    UnmaskA(&res, ex , mod);
    printf("ex = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskA(&res, ey , mod);
    printf("ey = %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/
    ex[0] -=1078;
    ey[0] -=1078;

/* Check this part
    Exponent decode
*////*
    UnmaskA(&res, ex , (1<<16));
    printf("ex = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskA(&res, ey , (1<<16));
    printf("ey = %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/

    for (int i = 0; i < MASKSIZE; i++){
        c[i] = ex[i] - ey[i];
        temp[i] = c[i];
    }
    temp[0] -= 60;

 /* Check this part
    
*///*
    UnmaskA(&res, c , (1<<16));
    printf("c  = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskA(&res, temp , (1<<16));
    printf("temp= %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/

    A2B(cp, temp, (1<<16));
     /* Check this part
*//*
    UnmaskB(&res, cp , MASKSIZE);
    printf("cp = %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/
    for (int i = 0; i<MASKSIZE; i++){
        tempb[i] = -(((cp[i]>>15)&1)); 
        //si        cp[i](16)    = 0 
        //---> not(cp[i](16))    = 0b111...1111
        //---> not(cp[i](16)) +1 = 0b000...0000 (+ 1b000...0000)
        // and ---> my = 0
        //si        cp[i](16)    = 1 
        //---> not(cp[i](16))    = 0b111...1110
        //---> not(cp[i](16)) +1 = 0b111...1111
        // and ---> my = my
    }
    /* Check this part
*//*
    UnmaskB(&res, tempb , MASKSIZE);
    printf("tempb= %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/
    SecAnd(my, my, tempb,MASKSIZE);
/* Check this part
*////*
    UnmaskB(&res, my , MASKSIZE);
    printf("my = %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/

//Here we have something strange, but this work and compute the good rotation
    A2B(tempb, c, 1<<16);
    for (int i = 0; i<MASKSIZE; i++) tempb[i] = tempb[i]&0b111111;
    B2A(temp, tempb, 1<<16, MASKSIZE);

    SecFprUrsh(my, my, temp);
    /* Check this part
*////*
    UnmaskA(&res, temp ,(1<<16) );
    printf("temp = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, my , MASKSIZE);
    printf("my = %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/

    for (int i = 1; i<MASKSIZE; i++) {
        myp[i] = my[i];
        un[i] = 0;
    }
    un[0] = 1;
    myp[0] = ~my[0];
    
    SecAdd(tempb, myp, un,((uint64_t)1<<63), 64,MASKSIZE);

    /* Check this part
*////*
    UnmaskB(&res, tempb ,MASKSIZE);
    printf("temp = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, my , MASKSIZE);
    printf("my  = %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/


    for (int i = 0; i<MASKSIZE; i++) s[i] = (-(sx[i] ^ sy[i]));
    RefreshMasks(my, MASKSIZE);

    for (int i = 0; i<MASKSIZE; i++) tempb[i] = my[i] ^ myp[i];

    SecAnd(myp, tempb, s,MASKSIZE);

    /* Check this part
*////*
    UnmaskB(&res, tempb ,MASKSIZE);
    printf("tempb= %lu\n", res);
    print_binary_form(res);
    printf("\nAND\n");
    UnmaskB(&res, s , MASKSIZE);
    printf("s   = %lu\n", res);
    print_binary_form(res);
    printf("\n= \n");
    UnmaskB(&res, myp , MASKSIZE);
    printf("myp = %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/

    for (int i = 0; i<MASKSIZE; i++) my[i] = my[i] ^ myp[i];

    /* Check this part
*////*
    UnmaskB(&res, tempb ,MASKSIZE);
    printf("temp = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, my , MASKSIZE);
    printf("myp = %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/


    SecAdd(z, mx, my,((uint64_t)1<<63), 64,MASKSIZE);

   /* Check this part
*////*
    UnmaskB(&res, mx ,MASKSIZE);
    printf("mx   = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, my , MASKSIZE);
    printf("my   = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, z , MASKSIZE);
    printf("z    = %lu\n", res);
    print_binary_form(res);
    printf("\n");
//*/

    SecFprNorm64(z,ex, mod);

    UnmaskB(&res, z , MASKSIZE);
    printf("z    = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskA(&res, ex , 1<<16);
    printf("ex   = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    for (int i = 0; i<MASKSIZE; i++) b[i] = z[i]&0b111111111;
    SecNonZeroB(b,b);
    uint64_t mask = (~((uint64_t)0))>>12;
    for (int i = 0; i<MASKSIZE; i++) {
        z[i] = z[i]>>11;
        z[i] -= z[i]&1;
        z[i] += b[i]; //Attention ici pour le remplacement de ce bit. 

        z[i] &= mask;
    }
    ex[0]+= 8;
    ex[0] = ex[0] +1078; //1079

    UnmaskB(&res, z , MASKSIZE);
    printf("z    = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskA(&res, ex , 1<<16);
    printf("ex   = %lu\n", res);
    print_binary_form(res);
    printf("\n");
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

    SecFprUrsh(mout, mout, cd);

    for (int i = 0; i<MASKSIZE; i++) eout[i] += cd[i];

//PART 5 : Normalization

    SecFprNorm64(mout, eout, 1<<16);

    for (int i = 0; i<MASKSIZE; i++){
        mout[i] = mout[i]>>11;
        mout[i] = mout[i] - ((uint64_t)1<<52);
    }
    
    eout[0] = eout[0] +11;

    MaskedB eoutp; 
    A2B(eoutp, eout, 1<<16);

    SecAnd(eoutp,eoutp, c0, MASKSIZE);
    SecAnd(sout, sout, c0, MASKSIZE);


    for (int i = 0; i<MASKSIZE; i++) out[i] = ((mout[i]<<12)>>12) + (eoutp[i]<<52) + (sout[i]<<63);


   /*printf("\n\n\n============================== VALUE ==============================\n\n\n");
    uint64_t res;

    UnmaskB(&res, mx, MASKSIZE);
    printf("mx = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, mout, MASKSIZE);
    printf("mo = %lu\n", res);
    print_binary_form(res);
    printf("\n\n");

    UnmaskB(&res, exp, MASKSIZE);
    printf("exp= %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, ex, 1<<16);
    printf("ex = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, eout, 1<<16);
    printf("eo = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, eoutp, MASKSIZE);
    printf("eop= %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    UnmaskB(&res, c, MASKSIZE);
    printf("c  = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cx, 1<<16);
    printf("cx = %lu mod 1<<16\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cx, 1<<6);
    printf("cx = %lu mod 1<<6\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cd, 1<<16);
    printf("cd = %lu mod 1<<16\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cd, 1<<6);
    printf("cd = %lu mod 1<<6\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, cp, MASKSIZE);
    printf("cp = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, c0, MASKSIZE);
    printf("c0 = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    UnmaskB(&res, sx, MASKSIZE);
    printf("sx = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, sout, MASKSIZE);
    printf("so = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    UnmaskB(&res, out, MASKSIZE);
    printf("out= %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    printf("============================ END VALUE ============================\n");

    printf("\n\n\n\n");*/


}


void 
SecFprFloor(MaskedB out, MaskedB in){

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

    for (int j = 0; j<MASKSIZE; j++){
        cp[j] = 0;
        for (int i = 0; i<64; i++){
            cp[j] ^= ((c[j])<<i);
        }
    }
    RefreshMasks(cp,MASKSIZE);
    A2B(c, cx, 1<<16);
    SecAnd(c, c, cp, MASKSIZE);
    B2A(cx, c, 1<<16, MASKSIZE);
//PART 4 : SecFprUrsh
    for (int i = 0; i<MASKSIZE; i++) cd[i] = (-(cx[i]));

    SecFprUrsh(mout, mout, cd);

    
// ICI POUR AVOIR LA PARTIE ENTIERE. 
    MaskedB b;
    SecNonZeroB(b, mout);

    SecAdd(mout, mout, sout,64,6,  MASKSIZE);

//FIN PARTIE ENTIERE

    for (int i = 0; i<MASKSIZE; i++) eout[i] += cd[i];

//PART 5 : Normalization

    SecFprNorm64(mout, eout, 1<<16);

    for (int i = 0; i<MASKSIZE; i++){
        mout[i] = mout[i]>>11;
        mout[i] = mout[i] - ((uint64_t)1<<52);
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


    /*printf("\n\n\n============================== VALUE ==============================\n\n\n");
    uint64_t res;

    UnmaskB(&res, mx, MASKSIZE);
    printf("mx = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, mout, MASKSIZE);
    printf("mo = %lu\n", res);
    print_binary_form(res);
    printf("\n\n");

    UnmaskB(&res, exp, MASKSIZE);
    printf("exp= %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, ex, 1<<16);
    printf("ex = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, eout, 1<<16);
    printf("eo = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    UnmaskB(&res, c, MASKSIZE);
    printf("c  = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cx, 1<<16);
    printf("cx = %lu mod 1<<16\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cx, 1<<6);
    printf("cx = %lu mod 1<<6\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cd, 1<<16);
    printf("cd = %lu mod 1<<16\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cd, 1<<6);
    printf("cd = %lu mod 1<<6\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, cp, MASKSIZE);
    printf("cp = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, c0, MASKSIZE);
    printf("c0 = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    UnmaskB(&res, sx, MASKSIZE);
    printf("sx = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, sout, MASKSIZE);
    printf("so = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    UnmaskB(&res, b, MASKSIZE);
    printf("b  = %lu\n", res);
    print_binary_form(res);
    printf("\n");
    UnmaskB(&res, bd, MASKSIZE);
    printf("bd = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, out, MASKSIZE);
    printf("out= %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    printf("\n\n\n============================ END VALUE ============================\n");

    printf("\n\n\n\n");*/


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

    cx[0] = cx[0] - 50 ;

    for (int i = 0; i<MASKSIZE; i++) cd[i] = (-(cx[i]));

    //A2B(c, cd, 1<<16);

    //SecAnd(c, c, cp, MASKSIZE);

    //B2A(cd, c, 1<<16, MASKSIZE);

    SecFprUrsh(mout, mout, cd);
    
// ICI POUR AVOIR LA PARTIE ENTIERE. 
    MaskedB b;
    for (int i = 0; i<MASKSIZE; i++) b[i] = mout[i]&10;
    SecNonZeroB(b, b);

    for (int i = 0; i<MASKSIZE; i++) mout[i] = mout[i]>>2;

    SecAdd(mout, mout, b ,64,6,  MASKSIZE);

    //SecAdd(mout, mout, sout ,64,6,  MASKSIZE);

//FIN PARTIE ENTIERE

    for (int i = 0; i<MASKSIZE; i++) eout[i] += cd[i];
    eout[0] +=2;

//PART 5 : Normalization

    SecFprNorm64(mout, eout, 1<<16);

    for (int i = 0; i<MASKSIZE; i++){
        mout[i] = mout[i]>>11;
        mout[i] = mout[i] - ((uint64_t)1<<52);
    }
    
    eout[0] = eout[0] +11;

    MaskedB eoutp; 
    A2B(eoutp, eout, 1<<16);

    MaskedB bp;
    for (int i =0; i<MASKSIZE; i++) bp[i] = -b[i];
    SecOr(bp, c0, bp);

    SecAnd(eoutp,eoutp, bp, MASKSIZE);
    SecAnd(sout, sout, bp, MASKSIZE);


    for (int i = 0; i<MASKSIZE; i++) out[i] = ((mout[i]<<12)>>12) + (eoutp[i]<<52) + (sout[i]<<63);



   /* printf("\n\n\n============================== VALUE ==============================\n\n\n");
    uint64_t res;

    UnmaskB(&res, mx, MASKSIZE);
    printf("mx = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, mout, MASKSIZE);
    printf("mo = %lu\n", res);
    print_binary_form(res);
    printf("\n\n");

    UnmaskB(&res, exp, MASKSIZE);
    printf("exp= %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, ex, 1<<16);
    printf("ex = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, eout, 1<<16);
    printf("eo = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    UnmaskB(&res, c, MASKSIZE);
    printf("c  = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cx, 1<<16);
    printf("cx = %lu mod 1<<16\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cx, 1<<6);
    printf("cx = %lu mod 1<<6\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cd, 1<<16);
    printf("cd = %lu mod 1<<16\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskA(&res, cd, 1<<6);
    printf("cd = %lu mod 1<<6\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, cp, MASKSIZE);
    printf("cp = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, c0, MASKSIZE);
    printf("c0 = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    UnmaskB(&res, sx, MASKSIZE);
    printf("sx = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, sout, MASKSIZE);
    printf("so = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    UnmaskB(&res, b, MASKSIZE);
    printf("b  = %lu\n", res);
    print_binary_form(res);
    printf("\n");

    UnmaskB(&res, out, MASKSIZE);
    printf("out= %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    printf("\n\n\n============================ END VALUE ============================\n");

    printf("\n\n\n\n");*/


}