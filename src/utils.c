#include "utils.h"

uint32_t rand64(void){
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
 
   return((uint32_t)((rotor0*0x1525374657E5F50DUL)+0x8B459B95879A07F3UL)&(~((~0)<<32)));
 }

 uint32_t randmod(uint32_t mod){
  return rand()%mod;
 }

 void print_binary_form(uint32_t in){
  fprintf(OUTPUT,"0b");
  for(int i = 0; i<32;i++){
    fprintf(OUTPUT,"%d",(in>>i)&1);
  }
 }
  
uint32_t addq(uint32_t ina, uint32_t inb, uint32_t mod){
  return ina + inb % mod;
}
 

uint32_t subq(uint32_t ina, uint32_t inb, uint32_t mod){
  uint32_t res = ina - inb % mod;
  return (res<0) ? res + mod : res;
}

uint32_t mulq(uint32_t ina, uint32_t inb, uint32_t mod){
  return ina*inb % mod;
}