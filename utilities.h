//
// Created by Peter Zheng on 2018/06/30.
//

#ifndef MIPS_UTILITIES_H
#define MIPS_UTILITIES_H

#include "Base.h"
#include "stdc++.h"
using namespace std;

const int regNUMMAX = 35;
const int memoryMAX = 4 * 1024 * 1024;
unsigned short sizeT(Base &t) {
    return t.getSize();
}

template <class T>
T atoi(const string &rhs){
    T ret;
    stringstream ss;
    ss << rhs;
    ss >> ret;
    return ret;
}


#endif //MIPS_UTILITIES_H
