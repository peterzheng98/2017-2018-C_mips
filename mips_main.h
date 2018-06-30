//
// Created by Peter Zheng on 2018/06/30.
//

#ifndef MIPS_MIPS_MAIN_H
#define MIPS_MIPS_MAIN_H

#include "stdc++.h"
#include "Base.h"
#include "utilities.h"

#define DADDR unsigned char
using namespace std;

class mips{
private:
    int stackHead, dataHead, textHead;
    int stackBottom, dataBottom, textBottom;

    int mainEntryPoint; //the Main Entry Point

    UINT32 regNum[regNUMMAX];
    DADDR mem[memoryMAX];


public:
    mips(){
        for(int i = 0; i < regNUMMAX; i++) regNum[i] = 0;
        for(int i = 0; i < memoryMAX; i++) mem[i] = 0;
    }




};

mips Kernel = mips();

#endif //MIPS_MIPS_MAIN_H
