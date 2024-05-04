#include "floorFPR.h"

int main(int *argc, char **argv){

    fprintf(OUTPUT,"==================================================\nTests of the masked floor\n==================================================\n");
    MaskedB test,res;
    uint64_t resB;

    test[0]=0x3FC0000000000000;
    printf("\n Test : 0.125\n");
    print_binary_form(test[0]);
    test[1]=0;
    test[2]=0;
    printf("\nExpected : 0\n");
    printf("\n Result :\n");
    SecArFloor(res,test,1);
    UnmaskB(&resB,res,MASKSIZE);
    print_binary_form(resB);
    printf("\n=======================TEST OVER========================\n");
    exit(0);
}