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

#endif //MIPS_EXECUTEDINSTRUCTION_H
