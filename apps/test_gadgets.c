#include "utils.h"
#include "gadgets.h"

#define MODULO 65536

int main(int *argc, char **argv){

    MaskedA a1, a2, a3;
    MaskedB b1, b2, b3;

    uint64_t B1,B2,resB;
    int32_t A1,A2,resA;

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

    fprintf(OUTPUT,"\n---------------Arithmetic masking---------------\n");
    //MaskA, UnmaskA, SecMult, SecAdd
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
/*
    SecAdd(a3,a1,a2);
    UnmaskA(&resA,a3,MODULO);

    if(subq(resA,addq(A1,A2,MODULO),MODULO)){
        fprintf(OUTPUT,"SecAdd failed!\n");
        fprintf(OUTPUT,"%d",A1);
        fprintf(OUTPUT,"\n+\n");
        fprintf(OUTPUT,"%d",A2);
        fprintf(OUTPUT,"\n=\n");
        fprintf(OUTPUT,"%d",addq(A1,A2,MODULO));
        fprintf(OUTPUT,"\n!=\n");
        fprintf(OUTPUT,"%d",resA);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecAdd Succeeded!\n");*/
    return 0;
}