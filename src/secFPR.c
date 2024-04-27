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
    int64_t bitsize = 8*sizeof(in[0]);
    uint64_t mask = ~((uint64_t)(0));
    int64_t len=bitsize/2;
    while(len>=1){
        mask >>= len;
        for(size_t i=0; i<MASKSIZE;i++){
            t2[i] = ((t[i]>>len)&mask);
        }
        RefreshXOR(l,t2,((uint64_t)1<<len),MASKSIZE);
        for(size_t i=0; i<MASKSIZE;i++){
            r[i] = (t[i])&mask;
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

void SecFprMul(MaskedB out, MaskedB x, MaskedB y){
    MaskedB s,sx,sy,p1,p2,b,z,z2,w,bx,by,d,ebx,eby;
    MaskedA e,ex,ey,wa,p3,p4,mx,my;
    for(size_t i =0; i < MASKSIZE;i++){
        sx[i] = (x[i]>>63);
        sy[i] = (y[i]>>63);
        ex[i] = ((x[i]<<1)>>53);
        ey[i] = ((y[i]<<1)>>53);
        mx[i] = x[i] & 0xfffffffffffff;
        my[i] = y[i] & 0xfffffffffffff;
    }

    for(size_t i= 0; i <MASKSIZE; i++) s[i] = sx[i]^sy[i];
    e[0] = ex[0] + ey[0] -2100;
    for(size_t i =1;i < MASKSIZE;i++) e[i] = ex[i] + ey[i];
    SecMult128(p3,p4,mx,my);
    A2B128(p1,p2,p3,p4,MASKSIZE);
    for(size_t i =0; i < MASKSIZE;i++) w[i] = p2[i] & 0x7ffffffffffff;
    SecNonZeroB(b,w);
    for(size_t i =0; i < MASKSIZE;i++) z[i] = (p2[i]>>50)^((p1[i]&0xffffffffff)<<13);
    for(size_t i =0; i < MASKSIZE;i++) z2[i] = (p2[i]>>51)^((p1[i]&0x1ffffffffff)<<12)^z[i];
    for(size_t i =0; i < MASKSIZE;i++) w[i] = (p2[i]>>42)&1;
    for(size_t i =0; i < MASKSIZE;i++) d[i] = -w[i];
    RefreshXOR(d,d,0,MASKSIZE);
    SecAnd(z2,z2,d,MASKSIZE);
    for(size_t i =0; i < MASKSIZE;i++) z[i] ^= z2[i];
    SecOr(z,z,b);
    B2A_bit(w,w,(1<<16));
    A2B(ebx,ex,(1<<16),MASKSIZE);
    A2B(eby,ey,(1<<16),MASKSIZE);
    SecNonZeroB(bx,ebx);
    SecNonZeroB(by,eby);
    SecAnd(d,bx,by,MASKSIZE);
    for(size_t i =0; i < MASKSIZE;i++) bx[i] = -(d[i]&1);
    SecAnd(z,z,bx,MASKSIZE);
    SecFPR(out,s,e,z);
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
/* Check this part
*//*
    UnmaskB(&res, in1m, MASKSIZE);
    printf("in1m = %lu\n", res);
    UnmaskB(&res, in1);
    printf("in1 = %lu\n", res);

    UnmaskB(&res, in2m, MASKSIZE);
    printf("in2m = %lu ---->", res);
    print_binary_form(res);
    UnmaskB(&res, in2, MASKSIZE);
    printf("\nin2 = %lu\n", res);

*/

 SecAdd(d, in1m, in2m, ((uint64_t)1<<63), 64, MASKSIZE);      //d=xm-ym-1
/* Check this part
Just one question here : which parameters must I choose for this function.
*////*
    UnmaskB(&res, d, MASKSIZE);

    printf("d = %lu ---->\n", res);
    //print_binary_form(res);
//*/
    for (int i = 1; i<MASKSIZE; i++){
        dp[i] = d[i];
    }
    dp[0] = ~d[0];
    SecNonZeroB(b, dp);
/* Check this part
Here the result must be 0 or 1 : 0 if in1 = in2, 1 if not.
*///
/*
    UnmaskB(&res, dp, MASKSIZE);
    printf("dp = %lu ------> ", res);
    print_binary_form(res);
    printf("\n");
    printf("\n\n");
    UnmaskB(&res, b, MASKSIZE);
    printf("b = %lu\n", res);
*/

    dp[0] = ~(d[0] ^ ((uint64_t)1<<63));

    SecNonZeroB(bp,dp);
/* Check this part
NOT YET : TO DO -- TEST
*/
/*
    UnmaskB(&res, dp, MASKSIZE);

    printf("dp = %lu ------> ", res);
    print_binary_form(res);
    printf("\n\n");
    printf("\n");
    UnmaskB(&res, bp, MASKSIZE);
    printf("bp = %lu\n", res);
*/

    for (int i = 1; i<MASKSIZE; i++){
        dp[i] = b[i];
    }
    dp[0] = ~b[0];
    for (int i = 0; i<MASKSIZE; i++){
        x_63[i] = in1m[i]>>63;
    }

    SecAnd(cs, b, x_63,MASKSIZE);
    //TEST A FAIRE ICI

    for (int i = 0; i<MASKSIZE; i++){
        x_63[i] = (d[i]>>63) ^ b[i] ^ bp[i];
    }
    SecOr(cs, cs, x_63);
    for (int i = 0; i<MASKSIZE; i++){
        x_63[i] = in1[i] ^ in2[i];
        cs[i] = -cs[i];
    }
    SecAnd(m, x_63, cs,MASKSIZE);
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
    SecAnd(my, my, tempb,MASKSIZE);
    for (int i = 0; i<MASKSIZE; i++) temp[i] = c[i]&0b11111;
    SecFprUrsh(my, my, temp);
    for (int i = 1; i<MASKSIZE; i++) {
        myp[i] = my[i];
        un[i] = 0;
    }
    un[0] = 1;
    myp[0] = -my[0] - 1;
    SecAdd(tempb, myp, un,((uint64_t)1<<63), 64,MASKSIZE);
    for (int i = 0; i<MASKSIZE; i++) s[i] = (-(sx[i] ^ sy[i]))&1; // voir pour la taille ici.
    //Refresh(my); //A MODIFIER 
    for (int i = 0; i<MASKSIZE; i++) tempb[i] = my[i] ^ myp[i];
    SecAnd(myp, tempb, s,MASKSIZE);
    for (int i = 0; i<MASKSIZE; i++) my[i] = my[i] ^ myp[i];
    SecAdd(z, mx, my,((uint64_t)1<<63), 64,MASKSIZE);
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