//
// Created by Peter Zheng on 2018/07/02.
//

#ifndef MIPS_EXECUTEDINSTRUCTION_H
#define MIPS_EXECUTEDINSTRUCTION_H

#include "Base.h"
#include "stdc++.h"
#include "utilities.h"
#include "mips_main.h"

using namespace std;
class executionInstruction {
public:
    Instruction type;
    vector<string> argv;
    int argc = argv.size();
    int index;
};

class RBase{
    virtual void IF() = 0;
};

class JR : public RBase{
private:
    executionInstruction e;

};
class Execution{
public:
    string labelName;
    int index;
    vector<executionInstruction> prog;
    void clear(){
        labelName = "";
        prog.clear();
    }
};

class executionInstructionNew{
public:
    Instruction type;
    vector<string> argv;
    int index, argc;
    int RSrc, Rdest, Src;
    int offset;
    string typeName;
    int ARSrc, ARdest, ASrc;
    int LRSrc, LRdest, LSrc;
    bool BLRSrc = false, BLRdest = false, BLSrc = false;
    executionInstructionNew() : index(0), argc(0), RSrc(0x3f3f3f3f), Rdest(0x3f3f3f3f), Src(0x3f3f3f3f), offset(0),
    ARSrc(0x3f3f3f3f), ARdest(0x3f3f3f3f), ASrc(0x3f3f3f3f), LRSrc(0x3f3f3f3f), LRdest(0x3f3f3f3f), LSrc(0x3f3f3f3f){ }
};

#endif //MIPS_EXECUTEDINSTRUCTION_H
