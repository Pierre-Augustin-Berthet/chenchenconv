#include "utils.h"
#include "gadgets.h"

#include <time.h>

#define MODULO 65536

int main(int *argc, char **argv){
    srand(time(NULL));
    uint32_t a1[MASKSIZE], a2[MASKSIZE], a3[MASKSIZE];
    uint32_t b1[MASKSIZE], b2[MASKSIZE], b3[MASKSIZE];

    uint32_t B1,B2,resB;
    uint32_t A1,A2,resA;

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

    RefreshXOR(b3,b2,64);
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
/*
    uint32_t tempB1 = randmod(MODULO);
    uint32_t tempB2 = randmod(MODULO);
    MaskB(b1,tempB1);
    MaskB(b2,tempB2);
    SecAdd(b3,b1,b2,16,4);
    UnmaskB(&resB,b3);
    resB %= MODULO;

    if(subq(resB,addq(tempB1,tempB2,MODULO),MODULO)){
        fprintf(OUTPUT,"SecAdd failed!\n");
        fprintf(OUTPUT,"%d",tempB1);
        fprintf(OUTPUT,"\n+\n");
        fprintf(OUTPUT,"%d",tempB2);
        fprintf(OUTPUT,"\n=\n");
        fprintf(OUTPUT,"%d",addq(tempB1,tempB2,MODULO));
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%d",resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecAdd Succeeded!\n");*/

    fprintf(OUTPUT,"\n---------------Arithmetic masking---------------\n");
    //MaskA, UnmaskA, SecMult
    A1 = randmod(MODULO);
    MaskA(a1,A1,MODULO);
    UnmaskA(&resA,a1,MODULO);

    if(subq(A1,resA,MODULO)){
        fprintf(OUTPUT,"Arithmetic Masking-Demasking failed!\n");
        fprintf(OUTPUT,"%d",A1);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%d",resA);
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
        fprintf(OUTPUT,"%d",A1);
        fprintf(OUTPUT,"\n*\n");
        fprintf(OUTPUT,"%d",A2);
        fprintf(OUTPUT,"\n=\n");
        fprintf(OUTPUT,"%d",mulq(A1,A2,MODULO));
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%d",resA);
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
        fprintf(OUTPUT,"%d",B2);
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%d",resA);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"B2A Conversion Succeeded!\n");


    return 0;
}