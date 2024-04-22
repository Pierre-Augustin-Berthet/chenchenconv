#include "utils.h"
#include "gadgets.h"

#include <time.h>

#define MODULO 65536

int main(int *argc, char **argv){
    srand(time(NULL));
    uint64_t a1[MASKSIZE], a2[MASKSIZE], a3[MASKSIZE];
    uint64_t b1[MASKSIZE], b2[MASKSIZE], b3[MASKSIZE];

    uint64_t B1,B2,resB;
    uint64_t A1,A2,resA;

    fprintf(OUTPUT,"==================================================\nTests of the masked gadgets\n==================================================\n");
    fprintf(OUTPUT,"---------------Boolean masking---------------\n");
    //MaskB, UnmaskB, SecAnd, SecOr
    B1 = rand64();
    MaskB(b1,B1);
    UnmaskB(&resB,b1);

    if(B1^resB){
        fprintf(OUTPUT,"Boolean Masking-Demasking failed!\n");
        print_binary_form(B1);
        fprintf(OUTPUT,"\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"Boolean Masking-Demasking Succeeded!\n");

    B2 = rand64();
    MaskB(b2,B2);
    SecAnd(b3,b1,b2);
    UnmaskB(&resB,b3);

    if((B1&B2)^resB){
        fprintf(OUTPUT,"SecAnd failed!\n");
        print_binary_form(B1);
        fprintf(OUTPUT,"\n&\n");
        print_binary_form(B2);
        fprintf(OUTPUT,"\n=\n");
        print_binary_form(B1&B2);
        fprintf(OUTPUT,"\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecAnd Succeeded!\n");

    SecOr(b3,b1,b2);
    UnmaskB(&resB,b3);

    if((B1|B2)^resB){
        fprintf(OUTPUT,"SecOr failed!\n");
        print_binary_form(B1);
        fprintf(OUTPUT,"\n|\n");
        print_binary_form(B2);
        fprintf(OUTPUT,"\n=\n");
        print_binary_form(B1&B2);
        fprintf(OUTPUT,"\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecOr Succeeded!\n");

    RefreshMasks(b2,MASKORDER);
    UnmaskB(&resB,b2);
    
    if(B2^resB){
        fprintf(OUTPUT,"RefreshMasks failed!\n");
        print_binary_form(B2);
        fprintf(OUTPUT,"\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"RefreshMasks Succeeded!\n");

    B2 = randmod(MODULO);
    MaskB(b2,B2);
    RefreshXOR(b3,b2,64,MASKSIZE);
    UnmaskB(&resB,b3);
    if(B2^resB){
        fprintf(OUTPUT,"RefreshXOR failed!\n");
        print_binary_form(B2);
        fprintf(OUTPUT,"\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"RefreshXOR Succeeded!\n");

    uint64_t tempB1 = randmod(MODULO);
    uint64_t tempB2 = randmod(MODULO);
    MaskB(b1,tempB1);
    MaskB(b2,tempB2);
    SecAdd(b3,b1,b2,MODULO,4);
    UnmaskB(&resB,b3);
    resB %= MODULO;

    if(subq(resB,addq(tempB1,tempB2,MODULO),MODULO)){
        fprintf(OUTPUT,"SecAdd failed!\n");
        fprintf(OUTPUT,"%ld",tempB1);
        fprintf(OUTPUT,"\n+\n");
        fprintf(OUTPUT,"%ld",tempB2);
        fprintf(OUTPUT,"\n=\n");
        fprintf(OUTPUT,"%ld",addq(tempB1,tempB2,MODULO));
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%ld",resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecAdd Succeeded!\n");

    fprintf(OUTPUT,"\n---------------Arithmetic masking---------------\n");
    //MaskA, UnmaskA, SecMult
    A1 = randmod(MODULO);
    MaskA(a1,A1,MODULO);
    UnmaskA(&resA,a1,MODULO);

    if(subq(A1,resA,MODULO)){
        fprintf(OUTPUT,"Arithmetic Masking-Demasking failed!\n");
        fprintf(OUTPUT,"%ld",A1);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%ld",resA);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"Arithmetic Masking-Demasking Succeeded!\n");

    A2 = randmod(MODULO);
    MaskA(a2,A2,MODULO);
    SecMult(a3,a1,a2,MODULO);
    UnmaskA(&resA,a3,MODULO);

    if(subq(resA,mulq(A1,A2,MODULO),MODULO)){
        fprintf(OUTPUT,"SecMul failed!\n");
        fprintf(OUTPUT,"%ld",A1);
        fprintf(OUTPUT,"\n*\n");
        fprintf(OUTPUT,"%ld",A2);
        fprintf(OUTPUT,"\n=\n");
        fprintf(OUTPUT,"%ld",mulq(A1,A2,MODULO));
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%ld",resA);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecMul Succeeded!\n");

    fprintf(OUTPUT,"\n---------------Conversions---------------\n");
    B2 = randmod(MODULO);
    MaskB(b2,B2);
    B2A(a2,b2,MODULO,MASKSIZE);
    UnmaskA(&resA,a2,MODULO);

    if(subq(resA,B2,MODULO)){
        fprintf(OUTPUT,"B2A Conversion failed!\n");
        fprintf(OUTPUT,"%ld",B2);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%ld",resA);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"B2A Conversion Succeeded!\n");
    A2 = randmod(MODULO);
    MaskA(a2,A2,MODULO);
    A2B(b2,a2,MODULO,MASKSIZE);
    UnmaskB(&resB,b2);

    /*if(subq(resB,A2,MODULO)){
        fprintf(OUTPUT,"A2B Conversion failed!\n");
        fprintf(OUTPUT,"%ld",A2);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%ld",resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"A2B Conversion Succeeded!\n");*/
  
    fprintf(OUTPUT,"\n-----------------B2A_bit-----------------\n");
    MaskedB y;
  
    uint64_t e = 1099;
    uint64_t res, mod;
    mod = 5;
    MaskedA A;
    
    for (uint64_t i = 0; i< (1<<3); i++){
        mod = rand64()%3000;
        printf("%lu = ", i);
        y[0] = i&1;
        y[1] = (i&0b10)>>1;
        y[2] = (i&0b100)>>2;
        printf("%lu %lu %lu = %lu ", y[2], y[1], y[0], y[2]^y[1]^y[0]);
        B2A_bit(A, y, mod);

        UnmaskA(&res,A, mod);

        printf("------------> RES B2A_bit = %lu\n", res);
    }

    fprintf(OUTPUT,"\n-----------------Mult128-----------------\n");

    uint64_t a = (uint64_t)1<<63;

    for (int i = 1 ; i<64; i++){
        uint64_t b = (uint64_t)1<<i;

        uint64_t intab[2] = {a, b};

        uint64_t out[2] = {0,0};

        Mult128(out, intab);

        printf("out[0] = %lu \n", out[0] );
        printf("out[1] = %lu \n", out[1] );

    }
    uint64_t b = (uint64_t)1<<1;

    printf("\n\n\n");

    a = 100654132587456132;
    b = 156321574565181561;

    uint64_t intab[2] = {a, b};

    uint64_t out[2] = {0,0};

    Mult128(out, intab);

    printf("out[0] = %lu \n", out[0] );
    printf("out[1] = %lu \n", out[1] );

    fprintf(OUTPUT,"\n-----------------t=t&c-----------------\n");

  

    return 0;
}