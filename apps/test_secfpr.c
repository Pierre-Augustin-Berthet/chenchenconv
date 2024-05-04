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


    inb[0] = 0x4014000000000000;//((uint64_t)2<<50) + ((uint64_t)0<<63) + ((uint64_t)1025<<52);
    inb[1] = 0;
    inb[2] = 0;

    MaskedB inb2;
    int mod = (1<<16);

    inb2[0] = 0x43A0A741A4627800;//0xC014000000000000;//((uint64_t)0) + ((uint64_t)0<<63) + ((uint64_t)1075<<52);
    inb2[1] = 0;
    inb2[2] = 0;

    //-5--> 0xC014000000000000
    //5--> 0x4014000000000000
    //6--> 0x4018000000000000

    SecFprAdd(outb, inb, inb2, mod); 

    UnmaskB(&res, outb, MASKSIZE);
    printf("outb = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    //for(int i = 1; i<1023; i++){
    //for(uint64_t j = 1; j< ((uint64_t)1<<52); j<<=1){

    fprintf(OUTPUT,"\n-----------------FprTrunc----------------\n");

    printf("TEST SecFprTrunc(5,5)\n");
    inb[0] = ((uint64_t)1) + ((uint64_t)1<<63) + ((uint64_t)1000<<52);//0x3FD999999999999A;//((uint64_t)0<<51) + ((uint64_t)0 <<63) + ((uint64_t)1022<<52);
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
    uint64_t e = (res<<1)>>53;
    printf("\ne = %lu\n", e);
    printf("\n\n\n");
    

    SecFprRound(inb2, inb);     
    UnmaskB(&res, inb2, MASKSIZE);
    printf("round = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    //}}

    fprintf(OUTPUT,"\n-----------------FprMul----------------\n");


    inb[0]  = 0x4014000000000000;//0x404c800000000000;//0x4014000000000000;//((uint64_t)2<<50) + ((uint64_t)0<<63) + ((uint64_t)1025<<52);
    inb[1] = 0;
    inb[2] = 0;

    inb2[0] = 0x41F7F91566100000;//0x443f38135d49f424;//0x4014000000000000;//0x41F7F91566100000;//0x43A0A741A4627800;//0x4059000000000000;//0x4014000000000000;//0x43A0A741A4627800;//0xC014000000000000;//((uint64_t)0) + ((uint64_t)0<<63) + ((uint64_t)1075<<52);
    inb2[1] = 0;
    inb2[2] = 0;

    //-5--> 0xC014000000000000
    //5--> 0x4014000000000000
    //6--> 0x4018000000000000

    SecFprMul(outb, inb, inb2); 

    UnmaskB(&res, outb, MASKSIZE);
    printf("outb = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");

    SecFprAdd(outb, inb, inb2, mod); 

    UnmaskB(&res, outb, MASKSIZE);
    printf("outb = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");



    fprintf(OUTPUT,"\n-----------------APPROX EXP----------------\n");

    inb[0]  = 0x3FE3333333333333;//0x3FE0000000000000;
    inb[1] = 0;
    inb[2] = 0;

    inb2[0]  = 0x3FE0000000000000;
    inb2[1] = 0;
    inb2[2] = 0;

    Secfpr_expm_p63(outb,inb, inb2);

    UnmaskB(&res, outb, MASKSIZE);
    printf("approx = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");


    fprintf(OUTPUT,"\n-----------------FprAdd------------------\n");


    inb[0] = 0b0100001001001011001010100100011001111110000000110000000000000000;//((uint64_t)2<<50) + ((uint64_t)0<<63) + ((uint64_t)1025<<52);
    inb[1] = 0;
    inb[2] = 0;

    MaskB(inb, inb[0]);
    for (int i = 0; i<MASKSIZE; i++) printf("inb = %lu\n", inb[i]);


    inb2[0] = 0b1100001000000001110100000100011000001110100011000000000000000000;//0x43A0A741A4627800;//0xC014000000000000;//((uint64_t)0) + ((uint64_t)0<<63) + ((uint64_t)1075<<52);
    inb2[1] = 0;
    inb2[2] = 0;

    MaskB(inb2, inb2[0]);
    for (int i = 0; i<MASKSIZE; i++) printf("inb = %lu\n", inb2[i]);

    //-5--> 0xC014000000000000
    //5--> 0x4014000000000000
    //6--> 0x4018000000000000

    SecFprAdd(outb, inb2, inb, mod); 

    UnmaskB(&res, outb, MASKSIZE);
    printf("outb = %lu\n", res);
    print_binary_form(res);

    printf("Expected : \n");
    print_binary_form(0x424A0D421D1A4000);
    print_binary_form(4776644932387094528);

    


    inb[0] = 0x4014000000000000;//((uint64_t)2<<50) + ((uint64_t)0<<63) + ((uint64_t)1025<<52);
    inb[1] = 0;
    inb[2] = 0;

    inb2[0] = 0x4014000000000000;
    inb2[1] = 0;
    inb2[2] = 0;

    //-5--> 0xC014000000000000
    //5--> 0x4014000000000000
    //6--> 0x4018000000000000

    SecFprAdd(outb, inb, inb2, mod); 

    UnmaskB(&res, outb, MASKSIZE);
    printf("outb = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");


    fprintf(OUTPUT,"\n-----------------BerExp----------------\n");

    inb[0]  = 0x4014000000000000;//0x3FE0000000000000;//0x3FE0000000000000;
    inb[1] = 0;
    inb[2] = 0;

    inb2[0]  = 0x3FE0000000000000;
    inb2[1] = 0;
    inb2[2] = 0;

    SecFprBerExp(outb,inb, inb2);

    UnmaskB(&res, outb, MASKSIZE);
    printf("approx = %lu\n", res);
    print_binary_form(res);
    printf("\n\n\n");


    /*uint64_t fpr_ln2_inv = 0x3FF71547652B82FE;
    uint64_t fpr_ln2 = 0x3FE62E42FEFA39EF;

    MaskedB inv_ln2, ln2, s, r;

    MaskB(inv_ln2, fpr_ln2_inv);
    MaskB(ln2, fpr_ln2);

    SecFprMul2(s, inv_ln2, inb);

    //SecFprFloor(s,s);*/

    inb[0]=0x3FF0000000000000;
    inb[1]=0x0;
    inb[2]=0x0;

    inb2[0]=0b1000011011101110011011011000001101001110010101010110101010011110;
    inb2[1]=0b1000000110010110000001000000010111001111001110110110000101100011;
    inb2[2]=0b1010011100101110100101000101001011001010010110111100100001001101;

    SecFprAdd(outb,inb,inb2,(1<<16));
    UnmaskB(&res,outb,MASKSIZE);
    print_binary_form(res);
    

    exit(0);
}
//00000 00000 00000 00000 00000 00000 000
//00000 00000 00000 00000 00000 00000 00
//00000 00000 00000 00000 00000 00000 01 .... 
//00011 10100 01100 01110 11000
//La fonction SecNonZero regarde seulement les 32 premiers bits.

//1 000010111010111010000000000000000000000000000000000000000000000
