#include "secFPR.h"

int main(int *argc, char **argv){

    srand(time(NULL));
    uint64_t a1[MASKSIZE], a2[MASKSIZE], a3[MASKSIZE];
    uint64_t b1[MASKSIZE], b2[MASKSIZE], b3[MASKSIZE];

    uint64_t B1,B2,resB;
    uint64_t A1,A2,resA;

    fprintf(OUTPUT,"==================================================\nTests of the masked FPR\n==================================================\n");
    fprintf(OUTPUT,"---------------SecNonZero---------------\n");
    
    MaskB(b1,0);
    SecNonZeroB(b2,b1);
    UnmaskB(&resB,b2,MASKSIZE);
    if(resB){
        fprintf(OUTPUT,"SecNonZeroB with 0 failed!\n");
        fprintf(OUTPUT,"0\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecNonZeroB with 0 Succeeded!\n");

    MaskB(b1,((uint64_t)1<<63)^((uint64_t)1<<50));
    SecNonZeroB(b2,b1);
    UnmaskB(&resB,b2,MASKSIZE);
    if(resB==0){
        fprintf(OUTPUT,"SecNonZeroB with (1<<63)^(1<<50) failed!\n");
        fprintf(OUTPUT,"0\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecNonZeroB with (1<<63)^(1<<50) Succeeded!\n");

    B1 = rand64();
    MaskB(b1,B1);
    SecNonZeroB(b2,b1);
    UnmaskB(&resB,b2,MASKSIZE);
    if(((B1!=0) & (resB == 0))||((B1==0) & (resB!=0))){
        fprintf(OUTPUT,"SecNonZeroB with random input failed!\n");
        print_binary_form(B1);
        fprintf(OUTPUT,"\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecNonZeroB with random input Succeeded!\n");

    fprintf(OUTPUT,"\n-----------------FprUrsh-----------------\n");

    uint64_t a, b, i_out, res;
    a = ((uint64_t)1)<<50;
    b = 5;

    MaskedB outb, inb;
    MaskedA ina;

    MaskA(ina, b, ((uint64_t)1<<16));
    MaskB(inb, a);
    

    SecFprUrsh(outb, inb, ina);

    UnmaskB(&i_out, outb, MASKSIZE);

    printf("i_out = %lu\n", i_out);
    print_binary_form(i_out);
    printf("\n");
    print_binary_form(a);
    printf("\n");

    fprintf(OUTPUT,"\n-----------------FprNorm-----------------\n");


    MaskA(ina, b, ((uint64_t)1<<16));
    
    inb[0] = (uint64_t)1<<52;
    inb[1] = 0;
    inb[2] = 0;

    UnmaskA(&i_out, ina, ((uint64_t)1<<16));

    printf("in_a = %lu\n", i_out);
    print_binary_form(i_out);
    printf("\n");


    SecFprNorm64(inb, ina, ((uint64_t)1<<16) );

    UnmaskA(&i_out, ina, ((uint64_t)1<<16));

    printf("in_a = %lu\n", i_out);
    print_binary_form(i_out);
    printf("\n");

    UnmaskB(&i_out, inb, MASKSIZE);
    printf("i_out = %lu\n", i_out);
    print_binary_form(i_out);
    printf("\n");

    fprintf(OUTPUT,"\n-----------------FprAdd------------------\n");


    inb[0] = 0x422E2F5DC41C0000;//((uint64_t)2<<50) + ((uint64_t)0<<63) + ((uint64_t)1025<<52);
    print_binary_form(((uint64_t)1026<<52));
    printf("\ninb[0] = %lu\n", inb[0]);
    print_binary_form(inb[0]);
    printf("\n");
    inb[1] = 0;
    inb[2] = 0;

    MaskedB inb2;
    int mod = (1<<16);

    inb2[0] = 0x4014000000000000;//((uint64_t)0<<50) + ((uint64_t)0<<63) + ((uint64_t)1024<<52);
    inb2[1] = 0;
    inb2[2] = 0;

    SecFprAdd(outb, inb, inb2, mod); 

    fprintf(OUTPUT,"\n-----------------FprTrunc----------------\n");

    printf("TEST SecFprTrunc(5,5)\n");
    inb[0] = 0x3FD999999999999A;//((uint64_t)0<<51) + ((uint64_t)0 <<63) + ((uint64_t)1022<<52);
    printf("inb[0] = %lu\n", inb[0]);
    print_binary_form(inb[0]);
    printf("\n");
    inb[1] = 0;
    inb[2] = 0;

    SecFprTrunc(inb2, inb);
    UnmaskB(&res, inb2, MASKSIZE);
    printf("trunc = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    SecFprFloor(inb2, inb);
    UnmaskB(&res, inb2, MASKSIZE);
    printf("floor = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    SecFprRound(inb2, inb);
    UnmaskB(&res, inb2, MASKSIZE);
    printf("round = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    exit(0);
}
//00000 00000 00000 00000 00000 00000 000
//00000 00000 00000 00000 00000 00000 00
//00000 00000 00000 00000 00000 00000 01 .... 
//00011 10100 01100 01110 11000
//La fonction SecNonZero regarde seulement les 32 premiers bits.

//1 000010111010111010000000000000000000000000000000000000000000000
