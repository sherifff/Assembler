#ifndef LITOB_H
#define LITOB_H
class LitOb
{
    public:
        LitOb(int add);
        int address;
        int getAddress();
        void setAddress(int newAddress);
        virtual ~LitOb();
    protected:
    private:
};

#endif // SYMOB_H

