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
              Number of shares size (int)
output    :   sensitive data out (uint64_t)
------------------------------------------------*/
void UnmaskB(uint64_t *out, uint64_t *in,int size){
    uint64_t r=0;
    for(size_t i=0; i<size-1; i++){
        r ^= in[i];
    }
    *out = in[size-1]^r;
}

/*------------------------------------------------
SecAnd    :   Secure AND at order MASKORDER
input     :   Boolean maskings ina, inb (MaskedB)
              Number of shares size (int)
output    :   Boolean masking out (MaskedB)
------------------------------------------------*/
void SecAnd(uint64_t *out, uint64_t *ina, uint64_t *inb, int size){
    MaskedB tempout;
    for (int i = 0; i<MASKSIZE; i++) tempout[i] = 0;
    uint64_t r[size][size];
    for(size_t i = 0; i<size; i++) tempout[i] = ina[i]&inb[i];
    for(size_t i = 0; i <size-1; i++){
        for(size_t j = i+1; j<size;j++){
            r[i][j] = rand64();
            r[j][i] = (r[i][j]^(ina[i]&inb[j]));
            r[j][i] ^= (ina[j]&inb[i]);
            tempout[i] ^= r[i][j];
            tempout[j] ^= r[j][i];
        }
    }
    for(size_t i = 0; i<size; i++) out[i] = tempout[i];

}

/*------------------------------------------------
SecOr     :   Secure OR at order MASKORDER
input     :   Boolean maskings ina,inb (MaskedB)
output    :   Boolean masking out (MaskedB)
------------------------------------------------*/
void SecOr(MaskedB out, MaskedB ina, MaskedB inb){
    uint64_t t[MASKSIZE],s[MASKSIZE];
    t[0] = ~ina[0];
    s[0] = ~inb[0];
    for(size_t i=1; i< MASKSIZE;i++){
        t[i] = ina[i];
        s[i] = inb[i];
    }
    SecAnd(out,t,s,MASKSIZE);
    out[0] = ~out[0];
}

/*------------------------------------------------
MaskA   :   Arithmetic Masking at order MASKORDER
input   :   sensitive data in (uint64_t), Modulo mod (uint64_t)
output  :   Arithmetic masking out (MaskedA)
------------------------------------------------*/
void MaskA(MaskedA out, uint64_t in, uint64_t mod){
    uint64_t r=0;
    for(size_t i=0; i<MASKORDER; i++){
        out[i] = randmod(mod);
        r = addq(r,out[i],mod);
    }
    out[MASKORDER] = subq(in,r,mod);
}

/*------------------------------------------------
UnmaskA   :   Arithmetic unmasking at order MASKORDER
input     :   Arithmetic masking in (MaskedA), Modulo mod (uint64_t)
output    :   sensitive data out (uint64_t)
------------------------------------------------*/
void UnmaskA(uint64_t *out, MaskedA in, uint64_t mod){
    uint64_t r=0;
    for(size_t i=0; i<MASKORDER; i++){
        r = addq(r,in[i],mod);
    }
    *out = addq(in[MASKORDER],r,mod)%mod;
}

/*------------------------------------------------
SecMult   :   Secure multiplication mod a power of 2 at order MASKORDER
input     :   Arithmetic maskings ina,inb (MaskedA), Modulo mod (uint64_t)
output    :   Arithmetic masking out (MaskedA)
------------------------------------------------*/
void SecMult(MaskedA out, MaskedA ina, MaskedA inb, uint64_t mod){
    uint64_t r[MASKSIZE][MASKSIZE];
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
              Modulo k2 (uint64_t),
              Size size(int)
output    :   Boolean masking out (MaskedB)
------------------------------------------------*/
void RefreshXOR(MaskedB out, MaskedB in, uint64_t k2, int size){
    uint64_t r;
    for(size_t i = 0; i<size; i++) out[i] = in[i];
    for(size_t i = 0; i<size-1; i++){
        for(size_t j = i+1; j<size; j++){
            r = randmod(k2);
            out[i] ^= r;
            out[j] ^= r;
        }
    }
}

/*------------------------------------------------
SecAdd    :   Secure addition at order MASKORDER
input     :   Boolean maskings ina,inb (MaskedB),
              Power of 2 k (uint64_t),
              Log2 of k minus 1 log2km1 (uint64_t)
              Number of shares size (int)
output    :   Arithmetic masking out (MaskedB)
------------------------------------------------*/
void SecAdd(uint64_t *out, uint64_t *ina, uint64_t* inb, uint64_t k, uint64_t log2km1,int size){
    uint64_t p[size],g[size],a[size],a2[size];
    int pow=1;
    for(size_t i = 0; i<size; i++) 
        p[i] = ina[i] ^ inb[i];
    SecAnd(g,ina,inb,size);
    for(size_t j = 0; j<log2km1-1; j++){
        for(size_t i = 0; i<size; i++) 
            a[i] = g[i] << pow;
        SecAnd(a2,a,p,size);
        for(size_t i =0; i<size; i++) {
            g[i] ^= a2[i];
            a2[i] = p[i] << pow;
        }
        RefreshXOR(a2,a2,k,size);
        SecAnd(a,p,a2,size);
        for(size_t i = 0; i < size; i++) p[i] = a[i];
        pow *= 2;
    }
    for(size_t i = 0; i<size; i++) a[i] = g[i] << pow;
    SecAnd(a2,a,p,size);
    for(size_t i = 0; i<size; i++){
        g[i] ^= a2[i];
        out[i] = ina[i]^inb[i]^(g[i]<<1);
    }
}

/*------------------------------------------------
RefreshMasks :   NI-Refresh at order MASKORDER
input        :   Boolean masking out (MaskedB),
                 Amount of shares refreshed size (int)
output       :   Boolean masking out (MaskedB)
------------------------------------------------*/
void RefreshMasks(MaskedB out, int size){
    uint64_t r;
    for(size_t i = 1; i < size; i++){
        r = rand64();
        out[0] ^= r;
        out[i] ^= r;
    }
}
void    Refresh             (); //ATTENTION ON DOIT POUVOIR CHOISIR QUELLES SHARES ON REFRESH

void A2B(uint64_t *out, uint64_t *in, uint64_t mod, int size){
    if(size==1){
        out[0] = in[0];
        return;
    }
    else{
    uint64_t up[size-size/2],down[size/2];
    uint64_t y[size],z[size];

    for(size_t i = 0; i < size/2; i++){down[i] = in[i];}
    for(size_t i = 0; i < size-size/2; i++){ up[i] = in[i+size/2];}

    A2B(y,down,mod,size/2);
    A2B(z,up,mod,size-size/2);

    for(size_t i = size/2; i<size;i++){y[i] = 0;}
    for(size_t i = size-size/2;i<size;i++){z[i] = 0;}

    RefreshXOR(y,y,mod,size);
    RefreshXOR(z,z,mod,size);
    SecAdd(out,z,y,mod,4,size);
    }
}

//For B2A
uint64_t Psi(uint64_t x,uint64_t y)
{
  return (x ^ y)-y;
}

//For B2A
uint64_t Psi0(uint64_t x, uint64_t y,int n)
{
  return Psi(x,y) ^ ((~n & 1) * x);
}

static void B2Aext(MaskedA out, MaskedB extended, uint64_t mod, int size);
void B2A(MaskedA out, MaskedB in, uint64_t mod, int size){
    uint64_t extended[size+1];
    extended[size]= 0;
    for(size_t i = 0; i<size; i++) extended[i] = in[i];
    B2Aext(out,extended,mod,size);
}

void B2Aext(MaskedA out, MaskedB x, uint64_t mod, int size){
    if(size==2){
        uint64_t r1 = rand64();
        uint64_t r2 = rand64();

        uint64_t y0 = (x[0]^r1)^r2;
        uint64_t y1 = x[1]^r1;
        uint64_t y2 = x[2]^r2;

        uint64_t z0 = y0^Psi(y0,y1);
        uint64_t z1 = Psi(y0,y2);

        out[0] = y1^y2;
        out[1] = z0^z1;
        return;
    }

    uint64_t y[size+1];
    for(size_t i = 0; i < size +1;i++) y[i] = x[i];
 
  RefreshMasks(y,size+1);
 
  uint64_t z[size];
 
  z[0]=Psi0(y[0],y[1],size);
  for(int i=1;i<size;i++) z[i]=Psi(y[0],y[i+1]);
 
  uint64_t A[size-1],B[size-1];
  B2Aext(A,y+1,mod,size-1);
  B2Aext(B,z,mod,size-1);
  
  for(int i=0;i<size-2;i++)
    out[i]=addq(A[i],B[i],mod);
 
  out[size-2]=A[size-2];
  out[size-1]=B[size-2];
}



//First one : a right rotation
void RightRotate(uint64_t * x, uint32_t c){
    uint64_t temp = (1<<c)-1;
    temp = *x & temp;
    *x = ((*x) >> c) + temp << (64 - c);  
}

void vecRightRotate(MaskedB in, uint32_t c){
    for (int i = 0; i<MASKSIZE; i++){
       
        uint64_t temp = ((uint64_t)1<<c)-1;
        temp = in[i] & temp;
        uint64_t temp2 = ((in[i]) >> c);
        in[i] = (temp2) ^ (temp << (64 - c));  
    } 
}

//Second one : a right shift
void RightRotate2(uint64_t * x, uint32_t c){
    *x = ((*x) >> c);  
}

void B2A_bit_j(MaskedA C, MaskedA A, uint64_t xn, uint64_t mod, int n){
    uint64_t Aa =0;
    for (int i = 0; i<n-1; i++){
        Aa = (Aa + A[i]);
    } 
    MaskedA B;
    for (int i = 0; i<MASKSIZE; i++) B[i] = 0;
    for (int i = 0; i<MASKSIZE; i++) C[i] = 0;
    B[n-1] = rand64()% mod;
    
    // b0 = a0 - bn-1 mod q;
    uint64_t temp = (mod - B[n-1]) % mod;
    B[0] = (A[0] + temp )%mod;

    for (int j = 1; j<n-1; j++){
        uint64_t R = rand64() % mod;

    //bj = aj - r mod q;
        temp = (mod - R) % mod;
        B[j] = (A[j] + temp) % mod;
        B[n-1] = (B[n-1] + R) % mod;
    }

    for (int j = 0; j < n; j++){
    //cj = bj - 2*bj*xn mod q; 
        temp = (mod - ((2 * (B[j] * xn))%mod)) % mod;
        C[j] = (B[j] + temp) % mod;
    }
    
    int b = ((C[0] + C[1] + C[2])%mod)%2;
    C[0] = (C[0] + xn) % mod; 
}

void B2A_bit(MaskedA A, MaskedB b, uint64_t mod){
    //Compute arithmetic value which unmask is equal to x1 xor x2 xor ... 
    A[0] = b[0];
    for (int j = 1; j<MASKSIZE; j++){
        MaskedA C;
        B2A_bit_j(C,A,b[j], mod, j+1);
        for (int i = 0; i<MASKSIZE; i++) A[i] = C[i];
        uint64_t res;
        UnmaskA(&res,C, mod);
    } 
}   
