#include "utils.h"

#include <time.h>

#define MODULO 65536

int main(int *argc, char **argv){
    srand(time(NULL));
    uint64_t B1,B2,resB;
    uint64_t A1,A2,resA;

    fprintf(OUTPUT,"==================================================\nTests of the masked gadgets\n==================================================\n");
    fprintf(OUTPUT,"\n-----------------Add128-----------------\n");

    A1 = (uint64_t)1<<63;
    A2 = (uint64_t)1<<63;
    B1 = 0;
    B2 = 0;

    Add128(&resB,&resA, B1,A1,B2,A2);

        if(resA!=0 | resB !=1){
            fprintf(OUTPUT,"Add128 failed!\n");
            print_binary_form(resB);
            fprintf(OUTPUT,"\n");
            print_binary_form(resA);
            fprintf(OUTPUT,"\n!=\n");
            print_binary_form(B1);
            fprintf(OUTPUT,"\n");
            print_binary_form(A1);
            fprintf(OUTPUT,"\n+\n");
            print_binary_form(B2);
            fprintf(OUTPUT,"\n");
            print_binary_form(A2);
            fprintf(OUTPUT,"\n");
            exit(1);
        }
    
    fprintf(OUTPUT,"Add128 succeeded!\n");
    A1 = (uint64_t)1<<63;

    for (int i = 1 ; i<64; i++){
        A2 = (uint64_t)1<<i;

        Mult128(&resA,&resB, A1,A2);

        if(resA!=0 | resB^(A2>>1)){
            fprintf(OUTPUT,"Mult128 failed!\n");
            print_binary_form(resB);
            fprintf(OUTPUT,"\n");
            print_binary_form(resA);
            fprintf(OUTPUT,"\n!=\n");
            print_binary_form(A1);
            fprintf(OUTPUT,"\n*\n");
            print_binary_form(A2);
            fprintf(OUTPUT,"\n");
            exit(1);
        }
    }
    fprintf(OUTPUT,"Mult128 succeeded!\n");/*
    uint64_t b = (uint64_t)1<<1;

    printf("\n\n\n");

    a = 100654132587456132;
    b = 156321574565181561;

    uint64_t intab[2] = {a, b};

    uint64_t out[2] = {0,0};

    Mult128(out, intab);

    printf("out[0] = %lu \n", out[0] );
    printf("out[1] = %lu \n", out[1] );

    fprintf(OUTPUT,"\n-----------------t=t&c-----------------\n");*/
    return 0;
}