#include "gadgets.h"
/*------------------------------------------------
MaskB   :   Boolean Masking at order MASKORDER
input   :   sensitive data in (uint64_t)
output  :   Boolean masking out (MaskedB)
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
UnmaskB   :   Boolean unmasking at order MASKORDER
input     :   Boolean masking in (MaskedB)
output    :   sensitive data out (uint64_t)
------------------------------------------------*/
void UnmaskB(uint64_t *out, MaskedB in){
    uint64_t r=0;
    for(size_t i=0; i<MASKORDER; i++){
        r ^= in[i];
    }
    *out = in[MASKORDER]^r;
}

/*------------------------------------------------
SecAnd    :   Secure AND at order MASKORDER
input     :   Boolean maskings ina, inb (MaskedB)
output    :   Boolean masking out (MaskedB)
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

/*------------------------------------------------
SecOr     :   Secure OR at order MASKORDER
input     :   Boolean maskings ina,inb (MaskedB)
output    :   Boolean masking out (MaskedB)
------------------------------------------------*/
void SecOr(MaskedB out, MaskedB ina, MaskedB inb){
    MaskedB t,s;
    t[0] = ~ina[0];
    s[0] = ~inb[0];
    for(size_t i=1; i< MASKSIZE;i++){
        t[i] = ina[i];
        s[i] = inb[i];
    }
    SecAnd(out,t,s);
    out[0] = ~out[0];
}

/*------------------------------------------------
MaskA   :   Arithmetic Masking at order MASKORDER
input   :   sensitive data in (int32_t), Modulo mod (int32_t)
output  :   Arithmetic masking out (MaskedA)
------------------------------------------------*/
void MaskA(MaskedA out, int32_t in, int32_t mod){
    int32_t r=0;
    for(size_t i=0; i<MASKORDER; i++){
        out[i] = randmod(mod);
        r = addq(r,out[i],mod);
    }
    out[MASKORDER] = subq(in,r,mod);
}

/*------------------------------------------------
UnmaskA   :   Arithmetic unmasking at order MASKORDER
input     :   Arithmetic masking in (MaskedA), Modulo mod (int32_t)
output    :   sensitive data out (int32_t)
------------------------------------------------*/
void UnmaskA(int32_t *out, MaskedA in, int32_t mod){
    int32_t r=0;
    for(size_t i=0; i<MASKORDER; i++){
        r = addq(r,in[i],mod);
    }
    *out = addq(in[MASKORDER],r,mod);
}

/*------------------------------------------------
SecMult   :   Secure multiplication mod a power of 2 at order MASKORDER
input     :   Arithmetic maskings ina,inb (MaskedA), Modulo mod (int32_t)
output    :   Arithmetic masking out (MaskedA)
------------------------------------------------*/
void SecMult(MaskedA out, MaskedA ina, MaskedA inb, int32_t mod){
    int32_t r[MASKSIZE][MASKSIZE];
    for(size_t i = 0; i<MASKSIZE; i++) out[i] = mulq(ina[i],inb[i],mod);
    for(size_t i = 0; i <MASKSIZE-1; i++){
        for(size_t j = i+1; j<MASKSIZE;j++){
            r[i][j] = randmod(mod);
            r[j][i] = addq(r[i][j],mulq(ina[i],inb[j],mod),mod);
            r[j][i] = addq(r[j][i],mulq(ina[j],inb[i],mod),mod);
            out[i] = subq(out[i],r[i][j],mod);
            out[j] = addq(out[j],r[j][i],mod);
        }
    }
}

/*------------------------------------------------
RefreshXOR:   XOR with Refresh for SecAdd at order MASKORDER
input     :   Boolean masking in (MaskedB), 
              Power of 2 modulo k2 (int32_t)
output    :   Boolean masking out (MaskedB)
------------------------------------------------*/
void RefreshXOR(MaskedB out, MaskedB in, int32_t k2){
    uint64_t r;
    for(size_t i = 0; i<MASKSIZE; i++) out[i] = in[i];
    for(size_t i = 0; i<MASKORDER; i++){
        for(size_t j = i+1; j<MASKSIZE; j++){
            r = (uint64_t) randmod(k2);
            out[i] ^= r;
            out[j] ^= r;
        }
    }
}

/*------------------------------------------------
SecAdd    :   Secure addition at order MASKORDER
input     :   Boolean maskings ina,inb (MaskedB),
              Power of 2 k (int32_t),
              Log2 of k minus 1 log2km1 (int32_t)
output    :   Arithmetic masking out (MaskedB)
------------------------------------------------*/
void SecAdd(MaskedB out, MaskedB ina, MaskedB inb, int32_t k, int32_t log2km1){
    MaskedB p,g,a;
    int pow=1;
    for(size_t i = 0; i<MASKSIZE; i++) 
        p[i] = ina[i] ^ inb[i];
    SecAnd(g,ina,inb);
    for(size_t j = 0; j<log2km1-1; j++){
        for(size_t i = 0; i<MASKSIZE; i++) 
            a[i] = g[i] << pow;
        SecAnd(a,a,p);
        for(size_t i =0; i<MASKSIZE; i++) {
            g[i] ^= a[i];
            a[i] = p[i] << pow;
        }
        RefreshXOR(a,a,k);
        SecAnd(p,p,a);
        pow *= 2;
    }
    for(size_t i = 0; i<MASKSIZE; i++) a[i] = g[i] << pow;
    SecAnd(a,a,p);
    for(size_t i = 0; i<MASKSIZE; i++){
        g[i] ^= a[i];
        out[i] = ina[i]^inb[i]^(g[i]<<1);
    }
}


void    RefreshMasks        ();
void    Refresh             (); //ATTENTION ON DOIT POUVOIR CHOISIR QUELLES SHARES ON REFRESH

void A2B(MaskedB out, MaskedA in, int32_t mod, int size){
    if(size=1)
        out[0] = in[0];
    
    MaskedA up,down;
    MaskedB y,z;

    for(size_t i = 0; i < size/2; i++){
        down[i] = in[i];
    }
    for(size_t i = 0; i < size - size/2; i++){
        up[i] = in[size/2 + i];
    }

    A2B(y,down,mod,size/2);
    A2B(z,up,mod,size-size/2);

    for(size_t i = size/2; i<size;i++){
        y[i] = 0;
    }
    for(size_t i = size-size/2;i<size;i++){
        z[i] = 0;
    }
    RefreshXOR(y,y,6);
    RefreshXOR(z,z,6);
    SecAdd(out,z,y,16,4);
}


void    B2A                 (MaskedA out, MaskedB in, int32_t mod);
void    B2A_bit             ();