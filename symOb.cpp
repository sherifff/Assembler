#include "symOb.h"

symOb::symOb(int add, bool re)
{
    //ctor
    address = add;
    relocate = re;
}
bool symOb::getRelocatable(){
    return relocate;
}
int symOb::getAddress(){
    return address;
}
void symOb::setAddress(int newAddress){
    address = newAddress;
}
symOb::~symOb()
{
    //dtor
}
