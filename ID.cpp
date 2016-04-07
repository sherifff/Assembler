#include "ID.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
ID::ID(int oc, int nb)
{
    //ctor
    opcode = oc;
    numBytes = nb;
}

int ID::getOC(){
    return opcode;
}

int ID::getNumBytes(){
    return numBytes;
}


ID::~ID()
{
    //dtor
}
