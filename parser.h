//
// Created by Peter Zheng on 2018/06/30.
//

#ifndef MIPS_PARSER_H
#define MIPS_PARSER_H

#include "stdc++.h"
#define HIREGISTER 32
#define LOREGISTER 33
#define PCREGISTER 34


using namespace std;
enum Instruction {
    ADD = 0, ADDU, ADDIU, SUB, SUBU, MUL, MULU, DIV, DIVU, XOR, XORU, NEG, NEGU, REM, REMU,
    LI,
    SEQ, SGE, SGT, SLE, SLT, SNE,
    B, BEQ, BNE, BGE, BLE, BGT, BLT, BEQZ, BNEZ, BLEZ, BGEZ, BGTZ, BLTZ, J, JR, JAL, JALR,
    LA, LB, LH, LW,
    SB, SH, SW,
    MOVE, MFHI, MFLO,
    NOP, SYSCALL,
    COMMENT,
    LABEL,
    DATA, TEXT, ALIGN, ASCII, ASCIIZ, BYTE, HALF, WORD, SPACE
};

class parser {
    friend class mips;

private:
    map<string, Instruction> instrFind;
    map<string, int> registerMap, labelMap;
public:
    parser() {
        registerMap["$zero"] = registerMap["$0"] = 0;
        registerMap["$at"] = registerMap["$1"] = 1;
        registerMap["$v0"] = registerMap["$2"] = 2;
        registerMap["$v1"] = registerMap["$3"] = 3;
        registerMap["$a0"] = registerMap["$4"] = 4;
        registerMap["$a1"] = registerMap["$5"] = 5;
        registerMap["$a2"] = registerMap["$6"] = 6;
        registerMap["$a3"] = registerMap["$7"] = 7;
        registerMap["$t0"] = registerMap["$8"] = 8;
        registerMap["$t1"] = registerMap["$9"] = 9;
        registerMap["$t2"] = registerMap["$10"] = 10;
        registerMap["$t3"] = registerMap["$11"] = 11;
        registerMap["$t4"] = registerMap["$12"] = 12;
        registerMap["$t5"] = registerMap["$13"] = 13;
        registerMap["$t6"] = registerMap["$14"] = 14;
        registerMap["$t7"] = registerMap["$15"] = 15;
        registerMap["$s0"] = registerMap["$16"] = 16;
        registerMap["$s1"] = registerMap["$17"] = 17;
        registerMap["$s2"] = registerMap["$18"] = 18;
        registerMap["$s3"] = registerMap["$19"] = 19;
        registerMap["$s4"] = registerMap["$20"] = 20;
        registerMap["$s5"] = registerMap["$21"] = 21;
        registerMap["$s6"] = registerMap["$22"] = 22;
        registerMap["$s7"] = registerMap["$23"] = 23;
        registerMap["$t8"] = registerMap["$24"] = 24;
        registerMap["$t9"] = registerMap["$25"] = 25;
        registerMap["$k0"] = registerMap["$26"] = 26;
        registerMap["$k1"] = registerMap["$27"] = 27;
        registerMap["$gp"] = registerMap["$28"] = 28;
        registerMap["$sp"] = registerMap["$29"] = 29;
        registerMap["$fp"] = registerMap["$30"] = 30;
        registerMap["$ra"] = registerMap["$31"] = 31;
        registerMap["$hi"] = HIREGISTER;
        registerMap["$lo"] = LOREGISTER;
        registerMap["$pc"] = PCREGISTER;

        instrFind[".data"] = DATA;
        instrFind[".text"] = TEXT;
        instrFind[".align"] = ALIGN;
        instrFind[".ascii"] = ASCII;
        instrFind[".asciiz"] = ASCIIZ;
        instrFind[".byte"] = BYTE;
        instrFind[".half"] = HALF;
        instrFind[".word"] = WORD;
        instrFind[".space"] = SPACE;

        instrFind["add"] = ADD;
        instrFind["addu"] = ADDU;
        instrFind["addiu"] = ADDIU;
        instrFind["sub"] = SUB;
        instrFind["subu"] = SUBU;
        instrFind["mul"] = MUL;
        instrFind["mulu"] = MULU;
        instrFind["div"] = DIV;
        instrFind["divu"] = DIVU;
        instrFind["xor"] = XOR;
        instrFind["xoru"] = XORU;
        instrFind["neg"] = NEG;
        instrFind["negu"] = NEGU;
        instrFind["rem"] = REM;
        instrFind["remu"] = REMU;

        instrFind["li"] = LI;

        instrFind["seq"] = SEQ;
        instrFind["sge"] = SGE;
        instrFind["sgt"] = SGT;
        instrFind["sle"] = SLE;
        instrFind["slt"] = SLT;
        instrFind["sne"] = SNE;

        instrFind["b"] = B;
        instrFind["beq"] = BEQ;
        instrFind["bne"] = BNE;
        instrFind["bge"] = BGE;
        instrFind["ble"] = BLE;
        instrFind["bgt"] = BGT;
        instrFind["blt"] = BLT;
        instrFind["beqz"] = BEQZ;
        instrFind["bnez"] = BNEZ;
        instrFind["bgez"] = BGEZ;
        instrFind["blez"] = BLEZ;
        instrFind["bgtz"] = BGTZ;
        instrFind["bltz"] = BLTZ;
        instrFind["j"] = J;
        instrFind["jr"] = JR;
        instrFind["jal"] = JAL;
        instrFind["jalr"] = JALR;

        instrFind["la"] = LA;
        instrFind["lb"] = LB;
        instrFind["lh"] = LH;
        instrFind["lw"] = LW;

        instrFind["sb"] = SB;
        instrFind["sh"] = SH;
        instrFind["sw"] = SW;

        instrFind["move"] = MOVE;
        instrFind["mfhi"] = MFHI;
        instrFind["mflo"] = MFLO;

        instrFind["nop"] = NOP;
        instrFind["syscall"] = SYSCALL;

        instrFind["comment"] = COMMENT;
    }

    inline Instruction Type(const string &rhs) {
        if (instrFind.find(rhs) == instrFind.end()) return LABEL;
        return instrFind[rhs];
    }

    inline int RegisterAddr(const string &rhs) {
        if (registerMap.find(rhs) == registerMap.end()) return -1;
        return registerMap[rhs];
    }

    inline int LabelAddr(const string& rhs){
        if(labelMap.find(rhs) == labelMap.end()) return -1;
        return labelMap[rhs];
    }



};

#endif //MIPS_PARSER_H
