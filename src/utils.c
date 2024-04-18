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