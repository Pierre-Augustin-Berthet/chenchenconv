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
  if(mod == 0) return rand64();
  return rand()%mod;
 }

 void print_binary_form(uint64_t in){
  fprintf(OUTPUT,"0b");
  for(int i = 63; i>=0;i--){
    fprintf(OUTPUT,"%ld",(in>>i)&1);
  }
  fprintf(OUTPUT, "\n");
 }
  
uint64_t addq(uint64_t ina, uint64_t inb, uint64_t mod){
  if(mod==0) return ina + inb;
  return (ina + inb) % mod;
}
 

uint64_t subq(uint64_t ina, uint64_t inb, uint64_t mod){
  if(mod==0) return ina-inb;
  uint64_t res = (ina - inb) % mod;
  return (res<0) ? res + mod : res;
}

uint64_t mulq(uint64_t ina, uint64_t inb, uint64_t mod){
  return (ina*inb) % mod;
}


void Mult128(uint64_t *outup, uint64_t *outdown, uint64_t in1, uint64_t in2){
    uint64_t au, ad, bu, bd;
    au = in1>>32;
    ad = in1& 0xffffffff;
    bu = in2>>32;
    bd = in2& 0xffffffff;

    uint64_t multuu, multud, multdu, multdd;

    multuu = au * bu;
  //printf("\n%lu *%lu = %lu\n",a1,b1,mult11);
    multud = au * bd;
    multdu = ad * bu;
    multdd = ad * bd;
  //printf("\n%lu *%lu = %lu\n",a2,b2,mult22);

  *outup = multuu;
  //printf("\n%lu\n",*out2);
  *outdown = multdd&0xffffffff;
  //printf("\n%lu\n",*out1);
  uint64_t addlow, addup;

  addlow = (multud&0xffffffff) + (multdu&0xffffffff)+(multdd>>32);
  //printf("\n%lu\n",mult22>>32);
  addup = (multud>>32) + (multdu>>32) + ((addlow>>32)&0x3);
  *outdown += addlow<<32;
  *outup += addup;
/*
    uint64_t add1, carry;

    *out1 = mult22 & 0xffffffff;
    *out2 = mult11 + (mult12>>32) + (mult21>>32);

    add1  = (mult22>>32) + (mult12 & 0xffffffff) + (mult21 & 0xffffffff);
    carry = (add1 >>32);

    *out1 += carry;

    *out2 += add1 <<32;
*/
}

void Add128(uint64_t *outup, uint64_t *outdown, uint64_t in1up, uint64_t in1down, uint64_t in2up, uint64_t in2down){
  uint64_t in12_A, in12_B, in22_A, in22_B;

  in12_A = in1down>>32;
  in12_B = in1down&0xffffffff;
  in22_A = in2down>>32;
  in22_B = in2down&0xffffffff;

  in12_B += in22_B;
  in12_A += in22_A + ((in12_B>>32)&1);

  *outdown = (in12_B&0xffffffff)^((in12_A&0xffffffff)<<32);

  *outup = in1up + in2up + ((in12_A>>32)&0x3);

}

void Mult128Bi(uint64_t *outup, uint64_t *outdown, uint64_t in1up, uint64_t in1down, uint64_t in2up, uint64_t in2down){
  /*
  (u1*2^64 + d1)*(u2*2^64 + d2) = u1u2*2^128 + d1d2 + (d1u2+d2u1)*2^64 = d1d2 + (d1u2+d2u1)<<64 mod 2^128
  */
  uint64_t d1d2u,d1d2d,d1u2u,d1u2d,d2u1u,d2u1d;

  Mult128(&d1d2u,&d1d2d,in1down,in2down);
  Mult128(&d1u2u,&d1u2d,in1down,in2up);
  Mult128(&d2u1u,&d2u1d,in1up,in2down);

  *outup = d1d2u;
  *outdown = d1d2d;

  Add128(&d1d2u,&d1d2d,d1u2u,d1u2d,d2u1u,d2u1d); // A = d1u2 +d2u1
  d2u1u = *outup;
  d2u1d = * outdown;
  Add128(outup,outdown,d2u1u,d2u1d,d1d2d,0); //out = d1d2 + A<<64 = {d1d2u + Ad pour up; d1d2d pour down}
}

uint64_t FPR(int s,int e, uint64_t m){
  uint64_t x;
	uint32_t t;
	unsigned f;

	/*
	 * If e >= -1076, then the value is "normal"; otherwise, it
	 * should be a subnormal, which we clamp down to zero.
	 */
	e += 1076;
	t = (uint32_t)e >> 31;
	m &= (uint64_t)t - 1;

	/*
	 * If m = 0 then we want a zero; make e = 0 too, but conserve
	 * the sign.
	 */
	t = (uint32_t)(m >> 54);
	e &= -(int)t;

	/*
	 * The 52 mantissa bits come from m. Value m has its top bit set
	 * (unless it is a zero); we leave it "as is": the top bit will
	 * increment the exponent by 1, except when m = 0, which is
	 * exactly what we want.
	 */
	x = (((uint64_t)s << 63) | (m >> 2)) + ((uint64_t)(uint32_t)e << 52);

	/*
	 * Rounding: if the low three bits of m are 011, 110 or 111,
	 * then the value should be incremented to get the next
	 * representable value. This implements the usual
	 * round-to-nearest rule (with preference to even values in case
	 * of a tie). Note that the increment may make a carry spill
	 * into the exponent field, which is again exactly what we want
	 * in that case.
	 */
	f = (unsigned)m & 7U;
	x += (0xC8U >> f) & 1;
	return x;
}