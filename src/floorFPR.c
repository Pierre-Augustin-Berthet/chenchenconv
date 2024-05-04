#include "floorFPR.h"
#define DEBUG_TAYLOR
#ifdef DEBUG_ALL
    #ifndef DEBUG_TAYLOR
        #define DEBUG_TAYLOR
    #endif
    #ifndef DEBUG_COMPTRIG
        #define DEBUG_COMPTRIG
    #endif
    #ifndef DEBUG_SERIALTERM
        #define DEBUG_SERIALTERM
    #endif
    #ifndef DEBUG_FOURIEREA
        #define DEBUG_FOURIEREA
    #endif
    #ifndef DEBUG_ARFLOORSTEP
        #define DEBUG_ARFLOORSTE
    #endif
    #ifndef DEBUG_ARFLOOR
        #define DEBUG_ARFLOOR
    #endif
#endif
void Taylor(MaskedB sin, MaskedB cos, MaskedB x){

  MaskedB a,b,c,demi,sub;
  MaskedB temp;
  uint64_t test;
  double rest;

  for(size_t i = 0; i <MASKSIZE ; i++) {
    a[i] = x[i];
    sin[i] = x[i];
  }
  //printf("\n=======SO FAR SO GOOD===============\n");
  //masquer cos par 1.0 A refaire
  cos[0] =  0b0011111111110000000000000000000000000000000000000000000000000000;
  cos[1]=0;
  cos[2]=0;
#ifdef DEBUG_TAYLOR
    printf("Taylor - x : ");
    UnmaskB(&test,x,MASKSIZE);
    memcpy(&rest,&test,8);
            //print_binary_form(test);
            printf("%.20f\n",rest);
    printf("Taylor - a=x : ");
    UnmaskB(&test,a,MASKSIZE);
    memcpy(&rest,&test,8);
            //print_binary_form(test);
            printf("%.20f\n",rest);
#endif
  SecFprMul(b,x,a);
  #ifdef DEBUG_TAYLOR
    printf("Taylor - x*x : ");
    UnmaskB(&test,b,MASKSIZE);
    memcpy(&rest,&test,8);
            //print_binary_form(test);
            printf("%.20f\n",rest);
#endif
  //masquer demi par -0.5 A refaire
  demi[0] = 0b1011111111100000000000000000000000000000000000000000000000000000;
  demi[1] = 0;
  demi[2] = 0;
  SecFprMul(c,b,demi);
  /*
  for(size_t i = 0; i < MASKSIZE; i++) print_binary_form(c[i]);
  printf("\n");
  for(size_t i = 0; i < MASKSIZE; i++) print_binary_form(cos[i]);
  printf("\n\n");*/
  #ifdef DEBUG_TAYLOR
  printf("Taylor - (-0.5)* x*x");
    UnmaskB(&test,c,MASKSIZE);
    memcpy(&rest,&test,8);
            print_binary_form(test);
            printf("%.50f\n",rest);
    printf("Taylor - cos :");
    UnmaskB(&test,cos,MASKSIZE);
    memcpy(&rest,&test,8);
            print_binary_form(test);
            printf("%.50f\n",rest);
#endif

  SecFprAdd(temp,cos,c,(1<<16));
  #ifdef DEBUG_TAYLOR
    printf("Taylor - 1 -0.5*x*x : ");
    UnmaskB(&test,temp,MASKSIZE);
    memcpy(&rest,&test,8);
            //print_binary_form(test);
            printf("%.20f\n",rest);
#endif
  for(size_t i=0; i <MASKSIZE;i++) cos[i]=temp[i];
  //printf("\n=======SO FAR SO GOOD===============\n");
  SecFprMul(a,x,b);
  //masquer demi par -1/6 A refaire
  demi[0] = 0b1011111111000101010101010101010101010101010101010101010101010101;
  demi[1] = 0;
  demi[2] = 0;
  SecFprMul(c,a,demi);
  SecFprAdd(sin,sin,c,(1<<16));
  SecFprMul(b,x,a);
  //masquer demi par 1/24 A refaire
  demi[0] = 0x3FA5555555555555;
  demi[1] = 0;
  demi[2] = 0;
  SecFprMul(c,b,demi);
  SecFprAdd(cos,cos,c,(1<<16));
  SecFprMul(a,x,b);
  //masquer demi par 1/120
  demi[0] = 0x3F81111111111111;
  demi[1] = 0;
  demi[2] = 0;
  SecFprMul(c,a,demi);
  SecFprAdd(sin,sin,c,(1<<16));
}

void CompTrig(int k, MaskedB s, MaskedB c){

  MaskedB a,b,d;
  MaskedB deux,minus;

  for (int i = 0; i < k; i++){

    for(size_t j = 0; j < MASKSIZE; j++){
        a[j]=s[j];
        b[j]=c[j];
    }
    SecFprMul(d,a,b);
    //masquer deux par 2
    deux[0]=0x4000000000000000;
    deux[1]=0;
    deux[2]=0;
    SecFprMul(s,d,deux);
    SecFprMul(d,b,b);
    RefreshXOR(deux,deux,0,MASKSIZE);
    SecFprMul(a,d,deux);
    //masquer minus par -1
    minus[0]=0xBFF0000000000000;
    minus[1]=0;
    minus[2]=0;
    SecFprAdd(c,a,minus,(1<<16));
  }
}

void SerialTerm(MaskedB out0,MaskedB out1, MaskedB out2, MaskedB sin, MaskedB cos, int k){

  MaskedB a,s,c;
  MaskedB deux,minus;

  for(size_t i = 0; i < MASKSIZE; i++){
    s[i]=sin[i];
    c[i]=cos[i];
  }

  CompTrig(k,s,c);
  
  for(size_t i = 0; i < MASKSIZE; i++) out0[i] = s[i];

  SecFprMul(a,s,c);
  //masquer deux par 2
  deux[0]=0x4000000000000000;
  deux[1]=0;
  deux[2]=0;
  SecFprMul(out1,a,deux);

    SecFprMul(s,c,c);
    RefreshXOR(deux,deux,0,MASKSIZE);
    SecFprMul(a,s,deux);
  
  //masquer minus par -1
  minus[0]=0xBFF0000000000000;
  minus[1]=0;
  minus[2]=0;
  SecFprAdd(a,a,minus,(1<<16));

    SecFprMul(s,out0,a);
    SecFprMul(a,out1,c);
  SecFprAdd(out2,a,s,(1<<16));
}

void FourierEA(MaskedB out, MaskedB in,int k){
    MaskedB invpi,depi,demi,tier;
    MaskedB sin,cos,t0,t1,t2;
    MaskedB tempx,tempy,add;
    uint64_t test;
    double rest;
#ifdef DEBUG_FOURIEREA
    printf("FourierEA - x :");
    UnmaskB(&test,in,MASKSIZE);
    print_binary_form(test);
#endif
    //masquer depi
    depi[0] = 0x401921FB54442D18;
    depi[1]=0;
    depi[2]=0;
    SecFprMul(tempx,depi,in);
#ifdef DEBUG_FOURIEREA
    printf("FourierEA - 2pi*x :");
    UnmaskB(&test,tempx,MASKSIZE);
    print_binary_form(test);
#endif
    Taylor(sin,cos,tempx);
#ifdef DEBUG_FOURIEREA
    printf("FourierEA - sin(2*pi*x) :");
    UnmaskB(&test,sin,MASKSIZE);
    memcpy(&rest,&test,8);
    //print_binary_form(test);
    printf("%.20f\n",rest);
#endif
   // printf("\n=======SO FAR SO GOOD===============\n");
    SerialTerm(t0,t1,t2,sin,cos,k);
   // printf("\n=======SO FAR SO GOOD===============\n");
    //masquer invpi
    invpi[0]=0x3FD45F306DC9C883;
    invpi[1]=0;
    invpi[2]=0;
    SecFprMul(tempx,invpi,t0);
    SecFprAdd(add,in,tempx,(1<<16));
    RefreshXOR(invpi,invpi,0,MASKSIZE);
    //masquer demi
    demi[0] = 0b1011111111100000000000000000000000000000000000000000000000000000;
    demi[1] = 0;
    demi[2] = 0;
    SecFprMul(tempy,invpi,demi);
    SecFprMul(tempx,tempy,t1);
    SecFprAdd(add,add,tempx,(1<<16));
    RefreshXOR(invpi,invpi,0,MASKSIZE);
    //masquer tiers
    tier[0]=0x3FD5555555555555;
    tier[1]=0;
    tier[2]=0;
    SecFprMul(tempy,invpi,tier);
    SecFprMul(tempx,tempy,t2);
    SecFprAdd(out,add,tempx,(1<<16));
}

void SecArFloorStep(MaskedB out, MaskedB in, int iter,int k){
    for(size_t i = 0; i < iter ; i++){
        FourierEA(out,in,k);
 //       printf("\n=======SO FAR SO GOOD===============\n");
        for(size_t j = 0; j < MASKSIZE; j++){
            in[i] = out[i];
        }
    }
}

void SecArFloor(MaskedB out, MaskedB in,int k){
    MaskedB dix,bias,tempx,tempy,dixprec;
    uint64_t test;
    double rest;
    SecArFloorStep(tempx,in,24,k);
    int limit = k+4;
    //printf("\n=======SO FAR SO GOOD===============\n");
    //masquer 10
    dix[0]=0x4024000000000000;
    dix[1]=0;
    dix[2]=0;
    //masquer bias = -0.45
    bias[0]=0xBFDCCCCCCCCCCCCD;
    bias[1]=0;
    bias[2]=0;
    SecFprAdd(tempy,tempx,bias,(1<<16));
    SecFprMul(tempx,tempy,dix);
    #ifdef DEBUG_ARFLOOR
            printf("ArFloor step 0:");
            UnmaskB(&test,tempx,MASKSIZE);
            memcpy(&rest,&test,8);
            //print_binary_form(test);
            printf("%.20f\n",rest);
        #endif
    for(int i =1 ; i<16;i++){
        SecArFloorStep(tempy,tempx,3,limit);
        RefreshXOR(dix,dix,0,MASKSIZE);
        RefreshXOR(bias,bias,0,MASKSIZE);
        SecFprAdd(tempy,tempy,bias,(1<<16));
        SecFprMul(tempx,tempy,dix);
        limit+=4;
        #ifdef DEBUG_ARFLOOR
            printf("ArFloor step %d :",i);
            UnmaskB(&test,tempx,MASKSIZE);
            memcpy(&rest,&test,8);
            //print_binary_form(test);
            printf("%.20f\n",rest);
        #endif
    }
    //masquer 10^-16
    dixprec[0]=0x3C9CD2B297D889BC;
    dixprec[1]=0;
    dixprec[2]=0;
    SecFprMul(out,tempx,dixprec);
}