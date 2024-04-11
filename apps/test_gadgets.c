#include "utils.h"
#include "gadgets.h"

int main(int *argc, char **argv){

    MaskedA a1, a2, a3;
    MaskedB b1, b2, b3;

    uint64_t B1,B2,resB;
    uint32_t A1,A2,resA;

    fprintf(OUTPUT,"==================================================\nTests of the masked gadgets\n==================================================\n");
    fprintf(OUTPUT,"Boolean masking\n");
    //MaskB, UnmaskB, SecAnd
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

    return 0;
}