#include "utils.h"

uint64_t rand64(void){
   static uint64_t oneway, rotor0, rotor1, feedback, seed;
   if(!oneway){
     seed = rand();
     rotor0 =seed;
     rotor1 =seed*0xF108E4CD87654321UL;
     oneway =1UL;
   }

   feedback =(rotor1<<1)^(rotor0>>63)^(rotor0);
   rotor1 =rotor0;
   rotor0 =feedback;
 
   return((rotor0*0x1525374657E5F50DUL)+0x8B459B95879A07F3UL);
 }

 uint64_t randmod(uint64_t mod){
  if (mod == 0) return rand64();
  return rand()%mod;
 }

 void print_binary_form(uint64_t in){
  fprintf(OUTPUT,"0b");
  for(int i = 0; i<64;i++){
    fprintf(OUTPUT,"%ld",(in>>i)&1);
  }
 }
  
uint64_t addq(uint64_t ina, uint64_t inb, uint64_t mod){
  return (ina + inb) % mod;
}
 

uint64_t subq(uint64_t ina, uint64_t inb, uint64_t mod){
  uint64_t res = (ina - inb) % mod;
  return (res<0) ? res + mod : res;
}

uint64_t mulq(uint64_t ina, uint64_t inb, uint64_t mod){
  return (ina*inb) % mod;
}


void Mult128(uint64_t * out, uint64_t* in){
    uint64_t a1, a2, b1, b2;
    a1 = in[0]>>32;
    a2 = in[0]& 0xffffffff;
    b1 = in[1]>>32;
    b2 = in[1]& 0xffffffff;

    uint64_t mult11, mult12, mult21, mult22;

    mult11 = a1 * b1;
    mult12 = a1 * b2;
    mult21 = a2 * b1;
    mult22 = a2 * b2;

    uint64_t add1, carry;

    out[0] = mult22 & 0xffffffff;
    out[1] = mult11 + (mult12>>32) + (mult21>>32);

    add1  = (mult22>>32) + (mult12 & 0xffffffff) + (mult21 & 0xffffffff);
    carry = (add1 >>32);

    out[1] += carry;

    out[0] += add1 <<32;

}