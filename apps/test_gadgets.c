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
    UnmaskB(&resB,b1,MASKSIZE);

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
    SecAnd(b3,b1,b2,MASKSIZE);
    UnmaskB(&resB,b3,MASKSIZE);

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
    UnmaskB(&resB,b3,MASKSIZE);

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
    UnmaskB(&resB,b2,MASKSIZE);
    
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
    UnmaskB(&resB,b3,MASKSIZE);
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
    SecAdd(b3,b1,b2,MODULO,4,MASKSIZE);
    UnmaskB(&resB,b3,MASKSIZE);
    resB %= MODULO;

    if(subq(resB,addq(tempB1,tempB2,MODULO),MODULO)){
        fprintf(OUTPUT,"SecAdd failed!\n");
        fprintf(OUTPUT,"%lu",tempB1);
        fprintf(OUTPUT,"\n+\n");
        fprintf(OUTPUT,"%lu",tempB2);
        fprintf(OUTPUT,"\n=\n");
        fprintf(OUTPUT,"%lu",addq(tempB1,tempB2,MODULO));
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%lu",resB);
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
        fprintf(OUTPUT,"%lu",A1);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%lu",resA);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"Arithmetic Masking-Demasking Succeeded!\n");

    A2 = randmod(MODULO);
    MaskA(a2,A2,MODULO);
    SecMult(a3,a1,a2,MODULO);
    UnmaskA(&resA,a3,MODULO);

    if(subq(resA,mulq(A1,A2,MODULO),MODULO)){
        fprintf(OUTPUT,"SecMult failed!\n");
        fprintf(OUTPUT,"%lu",A1);
        fprintf(OUTPUT,"\n*\n");
        fprintf(OUTPUT,"%lu",A2);
        fprintf(OUTPUT,"\n=\n");
        fprintf(OUTPUT,"%lu",mulq(A1,A2,MODULO));
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%lu",resA);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecMult Succeeded!\n");

    fprintf(OUTPUT,"\n---------------Conversions---------------\n");
    B2 = randmod(MODULO);
    MaskB(b2,B2);
    B2A(a2,b2,MODULO,MASKSIZE);
    UnmaskA(&resA,a2,MODULO);

    if(subq(resA,B2,MODULO)){
        fprintf(OUTPUT,"B2A Conversion failed!\n");
        fprintf(OUTPUT,"%lu",B2);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%lu",resA);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"B2A Conversion Succeeded!\n");
    A2 = randmod(MODULO);
    MaskA(a2,A2,MODULO);
    //a2[0] = 0; a2[1] = 0; a2[2] = 0;
    A2B(b2,a2,MODULO);
    UnmaskB(&resB,b2,MASKSIZE);
    resB%=MODULO;

    if(subq(resB,A2,MODULO)){
        fprintf(OUTPUT,"A2B Conversion failed!\n");
        fprintf(OUTPUT,"%lu",A2);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%lu",resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"A2B Conversion Succeeded!\n");
  
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

    fprintf(OUTPUT,"\n---------------128bits---------------\n");
    B1 = rand64();
    B2 = rand64();
    MaskedB b31,b11,b21;
    uint64_t resB1,comp1,comp;
    tempB1 = rand64();
    tempB2 = rand64();
    MaskB(b11,tempB1);
    MaskB(b1,tempB2);
    MaskB(b21,B1);
    MaskB(b2,B2);
    SecAdd128(b31,b3,b11,b1,b21,b2,MASKSIZE);
    UnmaskB(&resB1,b31,MASKSIZE);
    UnmaskB(&resB,b3,MASKSIZE);

    Add128(&comp1,&comp,B1,B2,tempB1,tempB2);

    if(comp1^resB1|comp^resB){
        fprintf(OUTPUT,"SecAdd128 failed!\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",tempB1,tempB2);
        fprintf(OUTPUT,"\n+\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",B1,B2);
        fprintf(OUTPUT,"\n=\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",comp1,comp);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",resB1,resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecAdd128 Succeeded!\n");


    //A1 = 5629499534213120;
    //A2 = 0;
    A1 = rand64();
    A2 = rand64();
    MaskA128(a1,a2,A1,A2);
    UnmaskA128(&resB,&resA,a1,a2);

    if((A1^resB)|(A2^resA)){
        fprintf(OUTPUT,"MaskA128-DemaskA128 failed!\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",A2,A1);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",resB,resA);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"MaskA128-DemaskA128 Succeeded!\n");

    A1 = rand64();
    A2 = rand64();
    MaskedA a3up,a1up,a2up,a3down;
    uint64_t resAup,resAdown;
    MaskA128(a1up,a1,0,A1);
    MaskA128(a2up,a2,0,A2);
    SecMult128(a3up,a3down,a1up,a1,a2up,a2);
    UnmaskA128(&resAup,&resAdown,a3up,a3down);

    Mult128(&comp,&comp1,A1,A2);

    if(comp1^resAdown|comp^resAup){
        fprintf(OUTPUT,"SecMult128 failed!\n");
        fprintf(OUTPUT,"%lu",A2);
        fprintf(OUTPUT,"\n*\n");
        fprintf(OUTPUT,"%lu",A1);
        fprintf(OUTPUT,"\n=\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",comp,comp1);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",resAup,resAdown);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecMult128 Succeeded!\n");

    A1 = rand64();
    A2 = rand64();
    MaskedB b3up,b3down;
    MaskedA a1down;
    MaskA128(a1up,a1down,A2,A1);
    A2B128(b3up,b3down,a1up,a1down,MASKSIZE);
    UnmaskB(&resB,b3up,MASKSIZE);
    UnmaskB(&resA,b3down,MASKSIZE);

    if(A2^resB|A1^resA){
        fprintf(OUTPUT,"A2B128 failed!\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",A2,A1);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",resB,resA);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"A2B128 Succeeded!\n");

     
    B1 = rand64();
    MaskB(b1,B1);
    MaskB(b2,2);
    B2A128(a3up,a3down,b2,b1,MASKSIZE);
    UnmaskA128(&resB1,&resB,a3up,a3down);
    printf("test chelou : %lu\n",B1^resB|(resB1^2));
    if(B1^resB|(resB1^2)){
        fprintf(OUTPUT,"B2A128 failed!\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",(uint64_t)0,B1);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%lu * 2^64 + %lu",resB1,resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"B2A128 Succeeded!\n");
    return 0;
}