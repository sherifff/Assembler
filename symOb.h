#ifndef SYMOB_H
#define SYMOB_H


class symOb
{
    public:
        symOb(int add, bool re);
        int address;
        bool relocate;
        int getAddress();
        void setAddress(int newAddress);
        bool getRelocatable();
        virtual ~symOb();
    protected:
    private:
};

#endif // SYMOB_H
