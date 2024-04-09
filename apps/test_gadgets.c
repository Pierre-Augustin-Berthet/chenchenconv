#include "gadgets.h"
#include <stdio.h>

#define OUTPUT stdout

int main(int *argc, char **argv){

    MaskedA a1, a2, a3;
    MaskedB b1, b2, b3;

    uint64_t B,resB;
    uint32_t A,resA;

    fprintf(OUTPUT,"=========================\nTests of the masked gadgets\n=========================\n\n");
    fprintf(OUTPUT,"Boolean masking\n");

    //MaskB, UnmaskB, SecAnd

    B = rand64();
    MaskB(b1,B);
    UnmaskB(&resB,b1);

    if(B^resB){
        fprintf(OUTPUT,"Boolean masking-demasking failed!");
        exit(1);
    }

    resB = rand64();
    MaskB(b2,resB);
    SecAnd(b3,b1,b2);
    B &= resB;
    UnmaskB(&resB,b3);

    if(B^resB){
        fprintf(OUTPUT,"SecAnd failed!");
        exit(1);
    }

    return 0;
}