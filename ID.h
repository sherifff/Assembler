#ifndef ID_H
#define ID_H
#include <iostream>
#include <stdlib.h>

using namespace std;
class ID
{
    public:
        ID(int oc, int nb);
        virtual ~ID();
        int opcode;
        int numBytes;
        int getNumBytes();
        int getOC();
    protected:
    private:
};

#endif // ID_H
