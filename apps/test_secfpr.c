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

    uint64_t a, b, i_out;
    a = 55;
    b = 19;

    MaskedB outb, inb;
    MaskedA ina;

    MaskA(ina, b, ((uint64_t)1<<6));
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
    
    inb[0] = 5;
    inb[1] = 0;
    inb[2] = 0;


    SecFprNorm64(inb, ina, ((uint64_t)1<<16) );

    UnmaskA(&i_out, ina, ((uint64_t)1<<16));

    printf("in_a = %lu\n", i_out);
    print_binary_form(i_out);
    printf("\n");

    for (int i = 0; i<MASKSIZE; i++){
        uint64_t temp = inb[i]>>63;
        printf("temp = %lu\n", temp);
        printf("e = %lu\n", ina[i]);
    }

    UnmaskB(&i_out, inb, MASKSIZE);

    printf("i_out = %lu\n", i_out);
    print_binary_form(i_out);
    printf("\n");

    fprintf(OUTPUT,"\n-----------------FprAdd------------------\n");


    inb[0] = 5;
    inb[1] = 0;
    inb[2] = 0;

    MaskedB inb2;
    int mod = 2;

    inb2[0] = 5 -(uint64_t)((uint64_t)1<<63);
    inb2[1] = 0;
    inb2[2] = 0;

    SecFprAdd(outb, inb, inb2, mod);

    fprintf(OUTPUT,"---------------SecFprMult---------------\n");
    uint64_t sx,sy,ex,ey,mx,my;
    MaskedB sbx,sby,mbx,mby;
    MaskedA eax,eay;
    sx=0;
    ex=0;
    mx=0;
    sy=randmod(2);
    ey=randmod((1<<11));
    my=randmod((((uint64_t)1)<<52));
    MaskB(sbx,sx);
    MaskA(eax,ex,(1<<16));
    MaskB(mbx,mx);
    MaskB(sby,sy);
    MaskA(eay,ey,(1<<16));
    MaskB(mby,my);
    for(size_t i =0; i<MASKSIZE;i++){
        sbx[i]&=1;
        sby[i]&=1;
        mbx[i]=((mbx[i]<<8)>>8);
        mby[i]=((mby[i]<<8)>>8);
    }
    SecFPR(b1,sbx,eax,mbx);
    SecFPR(b2,sby,eay,mby);
    
    SecFprMul(b3,b1,b2);
    UnmaskB(&resB,b3,MASKSIZE);
    if(resB){
        fprintf(OUTPUT,"SecFprMult anything by 0 failed!\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecFprMult anything by 0 Succeeded!\n");

    sx=randmod(2);
    ex=1076;
    mx=randmod((((uint64_t)1)<<52));
    sy=randmod(2);
    ey=1076;
    my=randmod((((uint64_t)1)<<52));
    MaskB(sbx,sx);
    MaskA(eax,ex,(1<<16));
    MaskB(mbx,mx);
    MaskB(sby,sy);
    MaskA(eay,ey,(1<<16));
    MaskB(mby,my);
    for(size_t i =0; i<MASKSIZE;i++){
        sbx[i]&=1;
        sby[i]&=1;
        mbx[i]=((mbx[i]<<8)>>8);
        mby[i]=((mby[i]<<8)>>8);
    }
    SecFPR(b1,sbx,eax,mbx);
    SecFPR(b2,sby,eay,mby);
    
    SecFprMul(b3,b1,b1);
    UnmaskB(&resB,b3,MASKSIZE);
    uint64_t resb1,resb2;
    UnmaskB(&resb1,b1,MASKSIZE);
    UnmaskB(&resb2,b2,MASKSIZE);
    if(resB==0){
        fprintf(OUTPUT,"SecFprMult integer by integer failed!\n");
        print_binary_form(resb1);        
        fprintf(OUTPUT,"\n*\n");
        print_binary_form(resb2);
        fprintf(OUTPUT,"\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecFprMult integer by integer Succeeded!\n");
/*
    B1 = rand64();
    MaskB(b1,B1);
    SecNonZeroB(b2,b1);
    UnmaskB(&resB,b2,MASKSIZE);
    if(((B1!=0) & (resB == 0))||((B1==0) & (resB!=0))){
        fprintf(OUTPUT,"SecFprMult negative by anything failed!\n");
        print_binary_form(B1);
        fprintf(OUTPUT,"\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecFprMult negative by anything Succeeded!\n");

    B1 = rand64();
    MaskB(b1,B1);
    SecNonZeroB(b2,b1);
    UnmaskB(&resB,b2,MASKSIZE);
    if(((B1!=0) & (resB == 0))||((B1==0) & (resB!=0))){
        fprintf(OUTPUT,"SecFprMult anything by anything failed!\n");
        print_binary_form(B1);
        fprintf(OUTPUT,"\n!=\n");
        print_binary_form(resB);
        fprintf(OUTPUT,"\n");
        exit(1);
    }
    fprintf(OUTPUT,"SecFprMult anything by anything Succeeded!\n");*/

    return 0;
}
//00000 00000 00000 00000 00000 00000 000
//00000 00000 00000 00000 00000 00000 00
//00000 00000 00000 00000 00000 00000 01 .... 
//La fonction SecNonZero regarde seulement les 32 premiers bits.

