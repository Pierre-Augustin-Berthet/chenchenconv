#include "floorFPR.h"


void FourierEA(MaskedB out, MaskedB in){
    MaskedB invpi,depi,demi,tier;
    MaskedB sin,cos,t0,t1,t2;
    MaskedB tempx,tempy,add;

    //masquer depi
    SecFprMul(tempx,depi,in);
    Taylor(sin,cos,tempx);
    SerialTerm(t0,t1,t2,sin,cos);
    //masquer invpi
    SecFprMul(tempx,invpi,t0);
    SecFprAdd(add,in,tempx,0);
    RefreshXOR(invpi,invpi,0,MASKSIZE);
    //masquer demi
    SecFprMul(tempy,invpi,demi);
    SecFprMul(tempx,tempy,t1);
    SecFprAdd(add,add,tempx,0);
    RefreshXOR(invpi,invpi,0,MASKSIZE);
    //masquer tiers
    SecFprMul(tempy,invpi,tier);
    SecFprMul(tempx,tempy,t2);
    SecFprAdd(out,add,tempx,0);
}

void SecArFloorStep(MaskedB out, MaskedB in, int iter){
    for(size_t i = 0; i < iter ; i++){
        FourierEA(out,in);
        for(size_t j = 0; j < MASKSIZE; j++){
            in[i] = out[i];
        }
    }
}

void SecArFloor(MaskedB out, MaskedB in){
    MaskedB dix,bias,tempx,tempy,dixprec;
    SecArFloorStep(tempx,in,24);
    //masquer 10
    //masquer bias = -0.45
    SecFprAdd(tempy,tempx,bias,0);
    SecFprMul(tempx,tempy,dix);
    for(int i =1 ; i<16;i++){
        SecArFloorStep(tempy,tempx,3);
        RefreshXOR(dix,dix,0,MASKSIZE);
        RefreshXOR(bias,bias,0,MASKSIZE);
        SecFprAdd(tempy,tempy,bias,0);
        SecFprMul(tempx,tempy,dix);
    }
    //masquer 10^-16
    SecFprMul(out,tempx,dixprec);
}