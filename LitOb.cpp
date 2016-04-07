#include "LitOb.h"
LitOb::LitOb(int add)
{
    //ctor
    address = add;
}

int LitOb:: getAddress(){
    return address;
}

void LitOb:: setAddress(int newAddress){
    address = newAddress;
}

LitOb::~LitOb()
{
    //dtor
}
