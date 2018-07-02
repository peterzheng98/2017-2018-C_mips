//
// Created by Peter Zheng on 2018/06/30.
//

#ifndef MIPS_BASE_H
#define MIPS_BASE_H

class Base {
public:
    virtual unsigned short getSize() = 0;

    ~Base() {}
};

class UINT16 : public Base {
private:
    unsigned short sizeT = 2;
    unsigned char *Byte = nullptr;
    unsigned short data;
    bool neg = false;
public:
    UINT16() {
        data = 0;
        neg = false;
    }

    UINT16(unsigned short us) {
        data = us;
        neg = false;
    }

    UINT16(short s) {
        if (s < 0) {
            neg = true;
            data = -s;
        } else {
            neg = false;
            data = s;
        }
    }

    UINT16(unsigned char s1, unsigned char s2) {
        Byte = new unsigned char[2];
        Byte[0] = s1;
        Byte[1] = s2;
    }

    ~UINT16() {
        if (Byte != nullptr) delete Byte;
    }

    unsigned short getSize() {
        return sizeT;
    }
};

class UINT32 : public Base {
private:
    unsigned short sizeT = 4;
    unsigned char *Byte = nullptr;
    unsigned int data;
    bool neg = false;
public:
    UINT32() {
        data = 0;
        neg = false;
    }

    UINT32(unsigned int us) {
        data = us;
        neg = false;
    }

    UINT32(int s) {
        if (s < 0) {
            neg = true;
            data = -s;
        } else {
            neg = false;
            data = s;
        }
    }

    UINT32(unsigned char s1, unsigned char s2, unsigned char s3, unsigned char s4) {
        Byte = new unsigned char[4];
        Byte[0] = s1;
        Byte[1] = s2;
        Byte[2] = s3;
        Byte[3] = s4;
    }

    ~UINT32() {
        if (Byte != nullptr) delete Byte;
    }

    unsigned short getSize() {
        return sizeT;
    }
};

class DWORD : public Base {
private:
    unsigned short sizeT = 8;
    unsigned char *Byte = nullptr;
    unsigned long long data;
    bool neg = false;
public:
    DWORD() {
        data = 0;
        neg = false;
    }

    DWORD(unsigned long long us) {
        data = us;
        neg = false;
    }

    DWORD(long long s) {
        if (s < 0) {
            neg = true;
            data = -s;
        } else {
            neg = false;
            data = s;
        }
    }

    DWORD(unsigned char s1, unsigned char s2, unsigned char s3, unsigned char s4, unsigned char s5, unsigned char s6,
          unsigned char s7, unsigned char s8) {
        Byte = new unsigned char[8];
        Byte[0] = s1;
        Byte[1] = s2;
        Byte[2] = s3;
        Byte[3] = s4;
        Byte[4] = s5;
        Byte[5] = s6;
        Byte[6] = s7;
        Byte[7] = s8;
    }

    ~DWORD() {
        if (Byte != nullptr) delete Byte;
    }

    unsigned short getSize() {
        return sizeT;
    }
};

union _HALF {
    short s;
    unsigned short us;
    struct{
        unsigned char u1, u2;
    }core;

    _HALF() {
        s = 0;
    }

    _HALF(short _s) {
        s = _s;
    }

    _HALF(unsigned short _us) {
        us = _us;
    }

    _HALF(unsigned char _u1, unsigned char _u2) {
        core.u1 = _u1;
        core.u2 = _u2;
    }
};

union _WORD {
    int s;
    unsigned int us;
    struct{
        unsigned char u1, u2, u3, u4;
    }core;

    _WORD() {
        s = 0;
    }

    _WORD(int _s) {
        s = _s;
    }

    _WORD(unsigned int _us) {
        us = _us;
    }

    _WORD(unsigned char _u1, unsigned char _u2, unsigned char _u3, unsigned char _u4) {
        core.u1 = _u1;
        core.u2 = _u2;
        core.u3 = _u3;
        core.u4 = _u4;
    }
};

#endif //MIPS_BASE_H
