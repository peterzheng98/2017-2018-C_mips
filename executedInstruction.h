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

class Execution {
public:
    string labelName;
    int index;
    vector<executionInstruction> prog;

    void clear() {
        labelName = "";
        prog.clear();
    }
};

class executionInstructionNew {
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
    bool f[3] = {false};

    executionInstructionNew() : index(0), argc(0), RSrc(0x3f3f3f3f), Rdest(0x3f3f3f3f), Src(0x3f3f3f3f), offset(0),
                                ARSrc(0x3f3f3f3f), ARdest(0x3f3f3f3f), ASrc(0x3f3f3f3f), LRSrc(0x3f3f3f3f),
                                LRdest(0x3f3f3f3f), LSrc(0x3f3f3f3f) {
        argv.reserve(5);
    }
};

class executionInstructionNewPipeLine2_0 {
public:
    Instruction type;
    vector<string> argv;
    int index, argc = 0;

    struct parametre {
        ParaType type;
        int arg;
        parametre() : type(REGISTER), arg(0){}

        parametre(ParaType ty, int arr) : type(ty), arg(arr) {}

        void setData(ParaType ty, int arr) {
            type = ty;
            arg = arr;
        }
    };
    int offset;
    parametre para[5];
    string typeName = "";
    executionInstructionNewPipeLine2_0() : index(0), argc(0), offset(0){ }

    void printP(){
        printf("Idx:[%d] Command:[%s] argv:[%s] [%s] [%s]\n", index, typeName.c_str(), (argc >= 1 ? argv[0].c_str() : ""),
               (argc >= 2 ? argv[1].c_str() : ""), (argc >= 3 ? argv[2].c_str() : ""));
    }

    virtual bool ID() = 0;

    virtual bool EX() = 0;

    virtual bool MEM() = 0;

    virtual bool WB() = 0;

    ~executionInstructionNewPipeLine2_0(){}
};

class LogicalAndCalculate : public executionInstructionNewPipeLine2_0 {
public:
    int _Rdest, _RSrc, _Src;
    bool ImmData = false;
    _WORD Rdest, RSrc, Src;
    _WORD ansWord, ansWord2;
    _DWORD ansDword;
    LogicalAndCalculate() = default;

    virtual bool ID() override {
        if (argc == 3) {
            _Rdest = para[0].arg;

            if (para[1].type == IMM) _Src = para[1].arg, ImmData = true;
            else _RSrc = para[1].arg;

            if (para[2].type == IMM) _Src = para[2].arg, ImmData = true;
            else _Src = para[2].arg;

            if (ImmData) Src.s = _Src;
        }
        if (argc == 2) {
            if (type == MULU || type == MUL || type == DIV || type == DIVU) {
                _Rdest = para[0].arg;

                if (para[1].type == IMM) _Src = para[1].arg, ImmData = true;
                else _Src = para[1].arg;

                if (ImmData) Src.s = _Src;
            } else {
                _Rdest = para[0].arg;
                _RSrc = para[1].arg;
            }
        }

        switch (type) {
            case ADDU:
            case ADDIU:
            case SUBU:
            case XORU:
            case REMU:
            case ADD:
            case SUB:
            case XOR:
            case REM:
                if (registerLock[_RSrc]) { return false; }
                if (!ImmData && registerLock[_Src]) { return false; }
                RSrc = regNum[_RSrc];
                if (!ImmData) Src = regNum[_Src];
                registerLock[_Rdest] = true;
                break;
            case MUL:
            case DIV:
            case DIVU:
            case MULU:
                if (argc == 2) {
                    if (!ImmData && registerLock[_Src]) { return false; }
                    if (registerLock[_Rdest]) { return false; }
                    if (!ImmData) Src = regNum[_Src];
                    Rdest = regNum[_Rdest];
                    registerLock[HIREGISTER] = registerLock[LOREGISTER] = true;
                } else {
                    if (registerLock[_RSrc]) { return false; }
                    if (!ImmData && registerLock[_Src]) { return false; }
                    RSrc = regNum[_RSrc];
                    if (!ImmData) Src = regNum[_Src];
                    registerLock[_Rdest] = true;
                    break;
                }
            case NEG:
            case NEGU: {
                if (registerLock[_RSrc]) return false;
                RSrc = regNum[_RSrc];
                registerLock[_Rdest] = true;
                break;
            }
        }
        return true;
    }

    virtual bool EX() override {
        if (argc == 2 && (type == MULU || type == MUL || type == DIV || type == DIVU))
            registerLock[HIREGISTER] = registerLock[LOREGISTER] = true;
        else registerLock[_Rdest] = true;

        switch (type) {
            case ADDU:
            case ADDIU:
                ansWord = _WORD(RSrc.us + Src.us);
                break;
            case SUBU:
                ansWord = _WORD(RSrc.us - Src.us);
                break;
            case XORU:
                ansWord = _WORD(RSrc.us ^ Src.us);
                break;
            case NEGU:
                ansWord = _WORD(~RSrc.us);
                break;
            case REMU:
                ansWord = _WORD(RSrc.us % Src.us);
                break;
            case ADD:
                ansWord = _WORD(RSrc.s + Src.s);
                break;
            case SUB:
                ansWord = _WORD(RSrc.s - Src.s);
                break;
            case XOR:
                ansWord = _WORD(RSrc.s ^ Src.s);
                break;
            case NEG:
                ansWord = _WORD(-RSrc.s);
                break;
            case REM:
                ansWord = _WORD(RSrc.s % Src.s);
                break;
            case MUL:
                if (argv.size() == 2) {
                    ansDword = _DWORD((long long) Rdest.s * Src.s);
                    ansWord = _WORD(ansDword.core.u1, ansDword.core.u2, ansDword.core.u3, ansDword.core.u4);
                    ansWord2 = _WORD(ansDword.core.u5, ansDword.core.u6, ansDword.core.u7, ansDword.core.u8);
                } else ansWord = _WORD(RSrc.s * Src.s);
                break;
            case DIV:
                if (argv.size() == 2) {
                    ansWord = _WORD(Rdest.s % Src.s);
                    ansWord2 = _WORD(Rdest.s / Src.s);
                } else ansWord = _WORD(RSrc.s / Src.s);
                break;
            case MULU:
                if (argv.size() == 2) {
                    ansDword = _DWORD((unsigned long long) Rdest.us * Src.us);
                    ansWord = _WORD(ansDword.core.u1, ansDword.core.u2, ansDword.core.u3, ansDword.core.u4);
                    ansWord2 = _WORD(ansDword.core.u5, ansDword.core.u6, ansDword.core.u7, ansDword.core.u8);
                } else ansWord = _WORD(RSrc.us * Src.us);
                break;
            case DIVU:
                if (argv.size() == 2) {
                    ansWord = _WORD(Rdest.us % Src.us);
                    ansWord2 = _WORD(Rdest.us / Src.us);
                } else ansWord = _WORD(RSrc.us / Src.us);
                break;
        }
        return true;
    }

    virtual bool MEM() override {
        if (argc == 2 && (type == MULU || type == MUL || type == DIV || type == DIVU))
            registerLock[HIREGISTER] = registerLock[LOREGISTER] = true;
        else registerLock[_Rdest] = true;
        return true;
    }

    virtual bool WB() override {
        if (argv.size() == 2 && (type == MULU || type == MUL || type == DIV || type == DIVU))
            registerLock[HIREGISTER] = registerLock[LOREGISTER] = true;
        else registerLock[_Rdest] = true;

        switch (type) {
            case ADDU:
            case ADDIU:
            case SUBU:
            case XORU:
            case REMU:
            case ADD:
            case SUB:
            case XOR:
            case NEG:
            case REM:
                regNum[_Rdest] = ansWord, registerLock[_Rdest] = false;
                break;
            case MUL:
            case DIV:
            case MULU:
            case DIVU:
                if (argv.size() == 2) {
                    regNum[HIREGISTER] = ansWord;
                    regNum[LOREGISTER] = ansWord2;
                    registerLock[HIREGISTER] = registerLock[LOREGISTER] = false;
                } else regNum[_Rdest] = ansWord, registerLock[_Rdest] = false;
                break;
        }
        return true;
    }

    ~LogicalAndCalculate(){}
};

class LiOpt : public executionInstructionNewPipeLine2_0 {
public:
    _WORD Imm;
    LiOpt() = default;
    virtual bool ID() override {
        Imm.s = para[1].arg;
        registerLock[para[0].arg] = true;
        return true;
    }

    virtual bool EX() override {
        registerLock[para[0].arg] = true;
        return true;
    }

    virtual bool MEM() override {
        registerLock[para[0].arg] = true;
        return true;
    }

    virtual bool WB() override {
        regNum[para[0].arg] = Imm;
        registerLock[para[0].arg] = false;
        return true;
    }
};

class CompareOpt : public executionInstructionNewPipeLine2_0 {
public:
    bool result, ImmData = false;
    _WORD Rsrc, Src;
    int _Rdest, _RSrc, _Src;
    CompareOpt() = default;
    virtual bool ID() override {
        if (para[2].type == IMM) {
            ImmData = true;
            Src.s = para[2].arg;
        } else {
            _Src = para[2].arg;
            if (registerLock[_Src]) return false;
            Src = regNum[_Src];
        }
        _Rdest = para[0].arg;
        _RSrc = para[1].arg;
        if (registerLock[_RSrc]) return false;
        Rsrc = regNum[_RSrc];
        registerLock[_Rdest] = true;
        return true;
    }

    virtual bool EX() override {
        registerLock[_Rdest] = true;
        switch (type) {
            case SEQ:
                result = Rsrc.s == Src.s;
                break;
            case SGE:
                result = Rsrc.s >= Src.s;
                break;
            case SGT:
                result = Rsrc.s > Src.s;
                break;
            case SLE:
                result = Rsrc.s <= Src.s;
                break;
            case SLT:
                result = Rsrc.s < Src.s;
                break;
            case SNE:
                result = Rsrc.s != Src.s;
                break;
        }
        return true;
    }

    virtual bool MEM() override {
        registerLock[_Rdest] = true;
        return true;
    }

    virtual bool WB() override {
        registerLock[_Rdest] = true;
        regNum[_Rdest] = result;
        registerLock[_Rdest] = false;
        return true;
    }
};

class NopOpt : public executionInstructionNewPipeLine2_0 {
public:
    NopOpt() = default;
    virtual bool ID() override {
        return true;
    }

    virtual bool EX() override {
        return true;
    }

    virtual bool MEM() override {
        return true;
    }

    virtual bool WB() override {
        return true;
    }
};

class IfAndJumpOpt : public executionInstructionNewPipeLine2_0 {
public:
    int nextExeLine;
    _WORD RSrc, Src;
    int _RSrc, _Src;
    bool result;
    int functionAddress;
    int Reg31;
    IfAndJumpOpt() = default;
    virtual bool ID() override {
        switch (type) {
            case BEQ:
            case BNE:
            case BGE:
            case BLE:
            case BGT:
            case BLT: {
                if (para[1].type == IMM) Src.s = para[1].arg;
                else _Src = para[1].arg;
                _RSrc = para[0].arg;

                if (registerLock[_RSrc]) return false;
                if (para[1].type == REGISTER && registerLock[_Src]) return false;

                RSrc.s = regNum[_RSrc].s;
                if (para[1].type != IMM) Src.s = regNum[_Src].s;

                nextExeLine = para[2].arg;
                break;
            }
            case BEQZ:
            case BNEZ:
            case BLEZ:
            case BGEZ:
            case BGTZ:
            case BLTZ: {
                _RSrc = para[0].arg;

                if (registerLock[_RSrc]) return false;

                RSrc.s = regNum[_RSrc].s;

                nextExeLine = para[1].arg;
                break;
            }
            case B:
            case J: {
                nextExeLine = para[0].arg;
                break;
            }
            case JAL: {
                registerLock[31] = true;
                nextExeLine = para[0].arg;
                break;
            }
            case JR:
            case JALR: {
                _RSrc = para[0].arg;
                if (registerLock[_RSrc]) return false;

                nextExeLine = regNum[_RSrc].s;

                if (type == JALR) registerLock[31] = true;
                RSrc.s = regNum[_RSrc].s;
                break;
            }
        }
        lockFlag = true;
        return true;
    }

    virtual bool EX() override {
        if (type == JAL || type == JALR) registerLock[31] = true;
        switch (type) {
            case BEQ:
                if (RSrc.s == Src.s) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BNE:
                if (RSrc.s != Src.s) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BGE:
                if (RSrc.s >= Src.s) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BLE:
                if (RSrc.s <= Src.s) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BGT:
                if (RSrc.s > Src.s) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BLT:
                if (RSrc.s < Src.s) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BEQZ:
                if (RSrc.s == 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BNEZ:
                if (RSrc.s != 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BLEZ:
                if (RSrc.s <= 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BGEZ:
                if (RSrc.s >= 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BGTZ:
                if (RSrc.s > 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BLTZ:
                if (RSrc.s < 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case J:
            case B:
                nextLine = nextExeLine;
                haveJump = true;
                break;
            case JR:
                nextLine = RSrc.s;
                haveJump = true;
                break;
            case JAL:
                Reg31 = currentLine + 1;
                nextLine = nextExeLine;
                haveJump = true;
                break;
            case JALR:
                Reg31 = currentLine + 1;
                nextLine = RSrc.s;
                haveJump = true;
                break;
        }
        return true;
    }

    virtual bool MEM() override {
        if (type == JAL || type == JALR) registerLock[31] = true;
        return true;
    }

    virtual bool WB() override {
        if (type == JAL || type == JALR) regNum[31].s = Reg31, registerLock[31] = false;
        lockFlag = false;
        return true;
    }
};

class StoreDataOpt : public executionInstructionNewPipeLine2_0 {
public:
    _WORD RSrc;
    int memoryAddress;
    int _RSrc;
    StoreDataOpt() = default;
    virtual bool ID() override {
        _RSrc = para[0].arg;

        if (registerLock[_RSrc]) return false;
        if (para[1].type == REGISTER && registerLock[para[1].arg]) return false;

        RSrc.s = regNum[_RSrc].s;
        memoryAddress = (para[1].type == ADDRESS_LABEL ? para[1].arg : regNum[para[1].arg].s);
        return true;
    }

    virtual bool EX() override {
        memoryAddress += offset;
        return true;
    }

    virtual bool MEM() override {
        switch (type) {
            case SB:
                mem[memoryAddress] = RSrc.core.u1;
                break;
            case SH:
                mem[memoryAddress] = RSrc.core.u1;
                mem[memoryAddress + 1] = RSrc.core.u2;
                break;
            case SW:
                mem[memoryAddress] = RSrc.core.u1;
                mem[memoryAddress + 1] = RSrc.core.u2;
                mem[memoryAddress + 2] = RSrc.core.u3;
                mem[memoryAddress + 3] = RSrc.core.u4;
                break;
        }
        return true;
    }

    virtual bool WB() override {
        return true;
    }
};

class LoadDataOpt : public executionInstructionNewPipeLine2_0 {
public:
    int memoryAddress;
    int _Rdest;
    _WORD Rdest;
    LoadDataOpt() = default;
    virtual bool ID() override {
        _Rdest = para[0].arg;
        switch (type) {
            case LA:
            case LB:
            case LH:
            case LW: {
                if (para[1].type == REGISTER && registerLock[para[1].arg]) return false;
                memoryAddress = (para[1].type == ADDRESS_LABEL ? para[1].arg : regNum[para[1].arg].s);
                registerLock[_Rdest] = true;
                break;
            }
        }
        return true;
    }

    virtual bool EX() override {
        registerLock[_Rdest] = true;
        switch (type) {
            case LA:
            case LB:
            case LH:
            case LW:
                memoryAddress += offset;
                break;
        }
        return true;
    }

    virtual bool MEM() override {
        registerLock[_Rdest] = true;
        switch (type) {
            case LB:
                Rdest = _WORD(int(mem[memoryAddress]));
                break;
            case LH:
                Rdest = _WORD((int) (_HALF(mem[memoryAddress], mem[memoryAddress + 1]).s));
                break;
            case LW:
                Rdest = _WORD(mem[memoryAddress], mem[memoryAddress + 1], mem[memoryAddress + 2],
                              mem[memoryAddress + 3]);
                break;
        }
        return true;
    }

    virtual bool WB() override {
        registerLock[_Rdest] = true;
        switch (type) {
            case LB:
            case LH:
            case LW:
                regNum[_Rdest].s = 0;
                regNum[_Rdest].s = Rdest.s;
                break;
            case LA:
                regNum[_Rdest].s = memoryAddress;
        }
        registerLock[_Rdest] = false;
        return true;
    }
};

class SpecialOpt : public executionInstructionNewPipeLine2_0 {
public:
    int optIdx;
    _WORD a0, a1, v0;
    string str;
    int i;
    int t;
    SpecialOpt() = default;
    virtual bool ID() override {
        if (registerLock[2]) return false;
        optIdx = regNum[2].s;
        switch (optIdx) {
            case 8: {
                if (registerLock[5]) return false;
                a1.us = regNum[5].us;
                a0.us = regNum[4].us;
                break;
            }
            case 1:
            case 4:
            case 9: {
                if (registerLock[4]) return false;
                a0.us = regNum[4].us;
                registerLock[2] = true;
                break;
            }
        }
        if (optIdx == 5) registerLock[2] = true;
        return true;
    }


    virtual bool EX() override {
        if (optIdx == 5 || optIdx == 9) registerLock[2] = true;
        return true;
    }

    virtual bool MEM() override {
        if (optIdx == 5 || optIdx == 9) registerLock[2] = true;
        switch (optIdx) {
            case 1:
                cout << a0.s;
                break;
            case 4:
                for (i = a0.s;; ++i) {
                    if (mem[i] == 0) break;
                    cout << (char) mem[i];
                }
                cout << flush;
                break;
            case 5:
                cin >> t;
                break;
            case 8:
                cin >> str;
                for (i = 0; i < str.length() && i < a1.s - 1; ++i) {
                    mem[a0.s + i] = (unsigned char) str[i];
                }
                mem[a0.s + str.length()] = 0;
                break;
            case 9:
                v0 = memHead + 1;
                memHead += a0.s;
                break;
            case 17:
                a0 = regNum[4].s;
                break;
        }
        return true;
    }

    virtual bool WB() override {
        switch(optIdx){
            case 5:
                regNum[2].s = t;
                registerLock[2] = false;
                break;
            case 9:
                regNum[2].s = v0.s;
                registerLock[2] = false;
                break;
            case 10:
                exit(0);
            case 17:
                exit(a0.s);
        }
        registerLock[2] = registerLock[4] = false;
        return true;
    }
};

class MoveDataOpt : public executionInstructionNewPipeLine2_0 {
public:
    _WORD Rsrc;
    int tmp, _RSrc, _Rdest;
    MoveDataOpt() = default;
    virtual bool ID() override {
        _Rdest = para[0].arg;
        switch (type) {
            case MOVE:{
                _RSrc = para[1].arg;
                if (registerLock[_RSrc]) return false;
                tmp = regNum[_RSrc].s;
                break;
            }

            case MFHI:{
                if (registerLock[HIREGISTER]) return false;
                tmp = regNum[HIREGISTER].s;
                break;
            }
            case MFLO:{
                if (registerLock[LOREGISTER]) return false;
                tmp = regNum[LOREGISTER].s;
                break;
            }
        }
        registerLock[_Rdest] = true;
        return true;
    }

    virtual bool EX() override {
        registerLock[_Rdest] = true;
        return true;
    }

    virtual bool MEM() override {
        registerLock[_Rdest] = true;
        return true;
    }

    virtual bool WB() override {
        regNum[_Rdest].s = tmp;
        registerLock[_Rdest] = false;
        return true;
    }
};


class executionInstructionNewPipeLine {
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
    bool f[3] = {false};
    int runningStage = 0;

    executionInstructionNewPipeLine() : index(0), argc(0), RSrc(0x3f3f3f3f), Rdest(0x3f3f3f3f), Src(0x3f3f3f3f),
                                        offset(0),
                                        ARSrc(0x3f3f3f3f), ARdest(0x3f3f3f3f), ASrc(0x3f3f3f3f), LRSrc(0x3f3f3f3f),
                                        LRdest(0x3f3f3f3f), LSrc(0x3f3f3f3f) {
        argv.reserve(5);
    }

    virtual bool ID() = 0;

    virtual bool EX() = 0;

    virtual bool MEM() = 0;

    virtual bool WB() = 0;

    void printDebug() {
        printf("Execution Stage: InsideIdx[%d] InsideStage[%d] [%s] argv:[%s] [%s] [%s]\n", index, runningStage,
               typeName.c_str(), (argc >= 1 ? argv[0].c_str() : ""),
               (argc >= 2 ? argv[1].c_str() : ""), (argc >= 3 ? argv[2].c_str() : ""));
    }

    void printP() {
        printf("Debug Stage: InIdx[%d]  [%s] argv:[%s] [%s] [%s] RSrc[%d] Rdest[%d] Src[%d] offset[%d] ARSrc[%d] "
               "ARdest[%d] ASrc[%d] LRSrc[%d] LRdest[%d] LSrc[%d] BLRSrc[%d] BLRdest[%d] BLSrc[%d]\n",
               index, typeName.c_str(), (argc >= 1 ? argv[0].c_str() : ""),
               (argc >= 2 ? argv[1].c_str() : ""), (argc >= 3 ? argv[2].c_str() : ""),
               RSrc, Rdest, Src, offset, ARSrc, ARdest, ASrc, LRSrc, LRdest, LSrc, BLRSrc, BLRdest, LSrc);
    }
};

class LogAndCal : public executionInstructionNewPipeLine {
public:
    _WORD _RSrc, _Rdest, _Src;
    _DWORD ansDword;
    _WORD ansWord, ansWord2;

    virtual bool ID() {
        /*if (lockFlag) return;*/
        switch (type) {
            case ADDU:
            case ADDIU:
            case SUBU:
            case XORU:
            case REMU:
                if (registerLock[RSrc]) {
                    stageLock = 1;
                    return false;
                }
                if (registerLock[Rdest]) {
                    stageLock = 1;
                    return false;
                }
                if (f[2] && registerLock[Src]) {
                    stageLock = 1;
                    return false;
                }
                _RSrc = regNum[RSrc].us;
                _Rdest = regNum[Rdest].us;
                _Src = f[2] ? regNum[Src].us : (unsigned int) Src;
                break;
            case ADD:
            case SUB:
            case XOR:
            case NEG:
            case REM:
                if (registerLock[RSrc]) {
                    stageLock = 1;
                    return false;
                }
                if (registerLock[Rdest]) {
                    stageLock = 1;
                    return false;
                }
                if (f[2] && registerLock[Src]) {
                    stageLock = 1;
                    return false;
                }
                _RSrc = regNum[RSrc].s;
                _Rdest = regNum[Rdest].s;
                _Src = f[2] ? regNum[Src].s : (int) Src;
                break;
            case MUL:
            case DIV:
                if (argv.size() == 2) {
                    if (registerLock[RSrc]) {
                        stageLock = 1;
                        return false;
                    }
                    if (f[1] && registerLock[Rdest]) {
                        stageLock = 1;
                        return false;
                    }
                    _Rdest = regNum[RSrc].s;
                    _Src = f[1] ? regNum[Rdest].s : Rdest;
                } else {
                    if (registerLock[RSrc]) {
                        stageLock = 1;
                        return false;
                    }
                    if (registerLock[Rdest]) {
                        stageLock = 1;
                        return false;
                    }
                    if (f[2] && registerLock[Src]) {
                        stageLock = 1;
                        return false;
                    }
                    _RSrc = regNum[RSrc].s;
                    _Rdest = regNum[Rdest].s;
                    _Src = f[2] ? regNum[Src].s : (int) Src;
                }
                break;
            case MULU:
            case DIVU:
                if (argv.size() == 2) {
                    if (registerLock[RSrc]) {
                        stageLock = 1;
                        return false;
                    }
                    if (f[1] && registerLock[Rdest]) {
                        stageLock = 1;
                        return false;
                    }
                    _Rdest = regNum[RSrc].us;
                    _Src = f[1] ? regNum[Rdest].us : Rdest;
                } else {
                    if (registerLock[RSrc]) {
                        stageLock = 1;
                        return false;
                    }
                    if (registerLock[Rdest]) {
                        stageLock = 1;
                        return false;
                    }
                    if (f[2] && registerLock[Src]) {
                        stageLock = 1;
                        return false;
                    }
                    _RSrc = regNum[RSrc].us;
                    _Rdest = regNum[Rdest].us;
                    _Src = f[2] ? regNum[Src].us : (int) Src;
                }
        }

        if (argv.size() == 2 && (type == MULU || type == MUL || type == DIV || type == DIVU))
            registerLock[HIREGISTER] = registerLock[LOREGISTER] = true;
        else registerLock[Rdest] = true;
        failToDo = false;
        runningStage++;
        return true;
    }

    virtual bool EX() {
        stageLock = 2;
        if (argv.size() == 2 && (type == MULU || type == MUL || type == DIV || type == DIVU))
            registerLock[HIREGISTER] = registerLock[LOREGISTER] = true;
        else registerLock[Rdest] = true;
        /*if (lockFlag) return;*/
        switch (type) {
            case ADDU:
            case ADDIU:
                ansWord = _WORD(_RSrc.us + _Src.us);
                break;
            case SUBU:
                ansWord = _WORD(_RSrc.us - _Src.us);
                break;
            case XORU:
                ansWord = _WORD(_RSrc.us ^ _Src.us);
                break;
            case NEGU:
                ansWord = _WORD(~_RSrc.us);
                break;
            case REMU:
                ansWord = _WORD(_RSrc.us % _Src.us);
                break;
            case ADD:
                ansWord = _WORD(_RSrc.s + _Src.s);
                break;
            case SUB:
                ansWord = _WORD(_RSrc.s - _Src.s);
                break;
            case XOR:
                ansWord = _WORD(_RSrc.s ^ _Src.s);
                break;
            case NEG:
                ansWord = _WORD(-_RSrc.s);
                break;
            case REM:
                ansWord = _WORD(_RSrc.s % _Src.s);
                break;
            case MUL:
                if (argv.size() == 2) {
                    ansDword = _DWORD((long long) _Rdest.s * _Src.s);
                    ansWord = _WORD(ansDword.core.u1, ansDword.core.u2, ansDword.core.u3, ansDword.core.u4);
                    ansWord2 = _WORD(ansDword.core.u5, ansDword.core.u6, ansDword.core.u7, ansDword.core.u8);
                } else ansWord = _WORD((int) _RSrc.s * _Src.s);
                break;
            case DIV:
                if (argv.size() == 2) {
                    ansWord = _WORD(_RSrc.s % _Src.s);
                    ansWord2 = _WORD(_RSrc.s / _Src.s);
                } else ansWord = _WORD(_RSrc.s * _Src.s);
                break;
            case MULU:
                if (argv.size() == 2) {
                    ansDword = _DWORD((unsigned long long) _Rdest.us * _Src.us);
                    ansWord = _WORD(ansDword.core.u1, ansDword.core.u2, ansDword.core.u3, ansDword.core.u4);
                    ansWord2 = _WORD(ansDword.core.u5, ansDword.core.u6, ansDword.core.u7, ansDword.core.u8);
                } else ansWord = _WORD(_RSrc.us * _Src.us);
                break;
            case DIVU:
                if (argv.size() == 2) {
                    ansWord = _WORD(_RSrc.us % _Src.us);
                    ansWord2 = _WORD(_RSrc.us / _Src.us);
                } else ansWord = _WORD(_RSrc.us * _Src.us);
                break;
        }

        runningStage++;
        return true;
    }

    virtual bool MEM() {
        if (argv.size() == 2 && (type == MULU || type == MUL || type == DIV || type == DIVU))
            registerLock[HIREGISTER] = registerLock[LOREGISTER] = true;
        else registerLock[Rdest] = true;
        /*if (lockFlag) return;*/
        runningStage++;
        return true;
    }

    virtual bool WB() {
        if (argv.size() == 2 && (type == MULU || type == MUL || type == DIV || type == DIVU))
            registerLock[HIREGISTER] = registerLock[LOREGISTER] = true;
        else registerLock[Rdest] = true;
        /*if (lockFlag) return;*/
        stageLock = -1;
        switch (type) {
            case ADDU:
            case ADDIU:
            case SUBU:
            case XORU:
            case REMU:
            case ADD:
            case SUB:
            case XOR:
            case NEG:
            case REM:
                regNum[Rdest] = ansWord, registerLock[Rdest] = false;
                break;
            case MUL:
            case DIV:
            case MULU:
            case DIVU:
                if (argv.size() == 2) {
                    regNum[HIREGISTER] = ansWord;
                    regNum[LOREGISTER] = ansWord2;
                    registerLock[HIREGISTER] = registerLock[LOREGISTER] = false;
                } else regNum[Rdest] = ansWord, registerLock[Rdest] = false;
                break;
        }
        runningStage++;
        return true;
    }
};

class Li : public executionInstructionNewPipeLine {
public:
    _WORD Imm;

    virtual bool ID() {
        /*if (lockFlag) return;*/
        failToDo = false;
        Imm.s = _WORD(Rdest).s;
        registerLock[RSrc] = true;
        runningStage++;
        return true;
    }

    virtual bool EX() {
        registerLock[RSrc] = true;
        stageLock = 2;
        /*if (lockFlag) return;*/
        runningStage++;
        return true;
    }

    virtual bool MEM() {
        registerLock[RSrc] = true;
        /*if (lockFlag) return;*/
        runningStage++;
        return true;
    }

    virtual bool WB() {
        registerLock[RSrc] = true;
        stageLock = -1;
        /*if (lockFlag) return;*/
        regNum[RSrc].s = Imm.s;
        registerLock[RSrc] = false;
        runningStage++;
        return true;
    }
};

class Compare : public executionInstructionNewPipeLine {
public:
    _WORD _Rsrc1, _src2;
    bool result;

    virtual bool ID() {
        /*if (lockFlag) return;*/
        if ((Rdest != 0x3f3f3f3f && registerLock[Rdest]) ||
            (RSrc != 0x3f3f3f3f && registerLock[RSrc])) {
            stageLock = 1;
            return false;
        }
        _Rsrc1.s = regNum[Rdest].s;
        _src2.s = (Src != 0x3f3f3f3f ? regNum[Src].s : Src);
        registerLock[RSrc] = true;
        failToDo = false;

        runningStage++;
        return true;
    }

    virtual bool EX() {
        registerLock[RSrc] = true;
        /*if (lockFlag) return;*/
        switch (type) {
            case SEQ:
                result = _Rsrc1.s == _src2.s;
                break;
            case SGE:
                result = _Rsrc1.s >= _src2.s;
                break;
            case SGT:
                result = _Rsrc1.s > _src2.s;
                break;
            case SLE:
                result = _Rsrc1.s <= _src2.s;
                break;
            case SLT:
                result = _Rsrc1.s < _src2.s;
                break;
            case SNE:
                result = _Rsrc1.s != _src2.s;
                break;
        }
        runningStage++;
        return true;
    }

    virtual bool MEM() {
        registerLock[RSrc] = true;
        /*if (lockFlag) return;*/
        runningStage++;
        return true;
    }

    virtual bool WB() {
        registerLock[RSrc] = true;
        stageLock = -1;
        /*if (lockFlag) return;*/
        regNum[RSrc].s = result;
        registerLock[RSrc] = false;
        runningStage++;
        return true;
    }
};

class IfAndJump : public executionInstructionNewPipeLine {
public:
    int nextExeLine;
    _WORD _Rsrc1, _src2;
    bool result;
    _WORD Reg31;

    virtual bool ID() {
        //if (lockFlag) return false;
        lockFlag = true;
        switch (type) {
            case BEQ:
            case BNE:
            case BGE:
            case BLE:
            case BGT:
            case BLT:
                if ((Rdest != 0x3f3f3f3f && registerLock[Rdest]) ||
                    (RSrc != 0x3f3f3f3f && registerLock[RSrc])) {
                    stageLock = 1;
                    return false;
                }
                _Rsrc1.s = regNum[RSrc].s;
                _src2.s = regNum[Rdest].s;
                nextExeLine = ASrc;
                break;
            case BEQZ:
            case BNEZ:
            case BLEZ:
            case BGEZ:
            case BGTZ:
            case BLTZ:
                if (RSrc != 0x3f3f3f3f && registerLock[RSrc]) {
                    stageLock = 1;
                    return false;
                }
                _Rsrc1.s = regNum[RSrc].s;
                nextExeLine = ARdest;
                break;
            case B:
            case J:
            case JAL:
                nextExeLine = ARSrc;
                break;
            case JR:
            case JALR:
                if (registerLock[RSrc]) {
                    stageLock = 1;
                    return false;
                }
                _Rsrc1.s = regNum[RSrc].s;
                break;
        }
        if (type == JAL || type == JALR) registerLock[31] = true;
        failToDo = false;

        runningStage++;
        lockFlag = true;
        nextClockIF += 4;
        return true;
    }

    virtual bool EX() {
        /*if (lockFlag) return;*/
        switch (type) {
            case BEQ:
                if (_Rsrc1.us == _src2.us) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BNE:
                if (_Rsrc1.us != _src2.us) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BGE:
                if (_Rsrc1.us >= _src2.us) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BLE:
                if (_Rsrc1.us <= _src2.us) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BGT:
                if (_Rsrc1.us > _src2.us) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BLT:
                if (_Rsrc1.us < _src2.us) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BEQZ:
                if (_Rsrc1.us == 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BNEZ:
                if (_Rsrc1.us != 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BLEZ:
                if (_Rsrc1.us <= 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BGEZ:
                if (_Rsrc1.us >= 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BGTZ:
                if (_Rsrc1.us > 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case BLTZ:
                if (_Rsrc1.us < 0) {
                    nextLine = nextExeLine;
                    haveJump = true;
                }
                break;
            case J:
            case B:
                nextLine = nextExeLine;
                haveJump = true;
                break;
            case JR:
                nextLine = _Rsrc1.s;
                haveJump = true;
                break;
            case JAL:
                Reg31 = currentLine + 1;
                nextLine = nextExeLine;
                haveJump = true;
                break;
            case JALR:
                Reg31 = currentLine + 1;
                nextLine = _Rsrc1.s;
                haveJump = true;
                break;
        }
        runningStage++;
        return true;
    }

    virtual bool MEM() {
        /*if (lockFlag) return;*/
        runningStage++;
        return true;
    }

    virtual bool WB() {
        stageLock = -1;
        /*if (lockFlag) return;*/
        switch (type) {
            case JAL:
            case JALR:
                regNum[31].s = Reg31.s;
                registerLock[31] = false;
        }
        lockFlag = false;
        runningStage++;
        return true;
    }
};

class StoreData : public executionInstructionNewPipeLine {
public:
    int source;
    _WORD tmp, _Rsrc;

    virtual bool ID() {
        /*if (lockFlag) return;*/
        if ((Rdest != 0x3f3f3f3f && registerLock[Rdest]) ||
            (RSrc != 0x3f3f3f3f && registerLock[RSrc])) {
            stageLock = 1;
            return false;
        }
        failToDo = false;
        if (LRdest != 0x3f3f3f3f) tmp.us = LRdest; else tmp.us = regNum[Rdest].s;
        _Rsrc.s = regNum[RSrc].s;
        runningStage++;
        return true;
    }

    virtual bool EX() {
        /*if (lockFlag) return;*/
        if (index == 509) {
            cout << "Here!";
        }
        source = tmp.us + offset;
        runningStage++;
        return true;
    }

    virtual bool MEM() {
        /*if (lockFlag) return;*/
        switch (type) {
            case SB:
                mem[source] = _Rsrc.core.u1;
                break;
            case SH:
                mem[source] = _Rsrc.core.u1;
                mem[source + 1] = _Rsrc.core.u2;
                break;
            case SW:
                mem[source] = _Rsrc.core.u1;
                mem[source + 1] = _Rsrc.core.u2;
                mem[source + 2] = _Rsrc.core.u3;
                mem[source + 3] = _Rsrc.core.u4;
                break;
        }
        runningStage++;
        return true;
    }

    virtual bool WB() {
        stageLock = -1;
        /*if (lockFlag) return;*/
        runningStage++;
        return true;
    }
};


class LoadData : public executionInstructionNewPipeLine {
public:
    int labelAddress, source;
    int _Rdest;
    _WORD _RSrc;

    virtual bool ID() {
        /*if (lockFlag) return;*/
        switch (type) {
            case LA:
                if (!BLRdest && registerLock[Rdest]) {
                    stageLock = 1;
                    return false;
                }
                _Rdest = BLRdest ? LRdest : regNum[Rdest].s;
                break;
            case LB:
            case LH:
            case LW:
                if (!BLRdest && registerLock[Rdest]) {
                    stageLock = 1;
                    return false;
                }
                _Rdest = BLRdest ? LRdest : regNum[Rdest].s;
                break;
        }
        failToDo = false;

        runningStage++;
        registerLock[RSrc] = true;
        return true;
    }

    virtual bool EX() {
        registerLock[RSrc] = true;
        /*if (lockFlag) return;*/
        switch (type) {
            case LA:
                labelAddress = (BLRdest ? LRdest : _Rdest);
                break;
            case LB:
            case LH:
            case LW:
                source = (BLRdest ? LRdest : _Rdest) + offset;
                break;
        }
        runningStage++;
        return true;
    }

    virtual bool MEM() {
        registerLock[RSrc] = true;
        /*if (lockFlag) return;*/
        switch (type) {
            case LB:
                _RSrc = _WORD(int(mem[source]));
                break;
            case LH:
                _RSrc = _WORD((int) (_HALF(mem[source], mem[source + 1]).s));
                break;
            case LW:
                _RSrc = _WORD(mem[source], mem[source + 1], mem[source + 2], mem[source + 3]);
                break;
        }
        runningStage++;
        return true;
    }

    virtual bool WB() {
        registerLock[RSrc] = true;
        stageLock = -1;
        /*if (lockFlag) return;*/
        switch (type) {
            case LB:
            case LH:
            case LW:
                regNum[RSrc] = _RSrc;
                break;
            case LA:
                regNum[RSrc] = labelAddress;
        }
        registerLock[RSrc] = false;
        runningStage++;
        return true;
    }
};

class Special : public executionInstructionNewPipeLine {
public:
    int opt;
    int a0, a1, v0;
    int i;
    int t;
    string str;

    virtual bool ID() {
        /*if (lockFlag) return;*/
        if (registerLock[2]) {
            stageLock = 1;
            return false;
        }
        opt = regNum[2].s;
        if (type == SYSCALL) {
            switch (opt) {
                case 8:
                    if (registerLock[5]) {
                        stageLock = 1;
                        return false;
                    }
                    a1 = regNum[5].us;
                case 1:
                case 4:
                case 9:
                    if (registerLock[4]) {
                        stageLock = 1;
                        return false;
                    }
                    a0 = regNum[4].s;
                    registerLock[2] = true;
                    break;
            }
            if (opt == 5) registerLock[2] = true;
        }
        failToDo = false;

        runningStage++;
        return true;
    }

    virtual bool EX() {
        if (opt == 5 || opt == 9) registerLock[2] = true;
        /*if (lockFlag) return;*/
        runningStage++;
        return true;
    }

    virtual bool MEM() {
        if (opt == 5 || opt == 9) registerLock[2] = true;
        /*if (lockFlag) return;*/
        if (type == SYSCALL) {
            switch (opt) {
                case 1:
                    cout << a0;
                    break;
                case 4:
                    for (i = a0;; ++i) {
                        if (mem[i] == 0) break;
                        cout << (char) mem[i];
                    }
                    //cout << "\0]\n";
                    cout << flush;
                    break;
                case 5:
                    cin >> t;
                    break;
                case 8:
                    cin >> str;
                    for (i = 0; i < str.length() && i < a1 - 1; ++i) {
                        mem[a0 + i] = (unsigned char) str[i];
                    }
                    mem[a0 + str.length()] = 0;
                    break;
                case 9:
                    v0 = memHead + 1;
                    memHead += a0;
                    break;
                case 17:
                    a0 = regNum[4].s;
                    break;
            }
        }
        runningStage++;
        return true;
    }

    virtual bool WB() {
        if (opt == 5 || opt == 9) registerLock[2] = true;
        stageLock = -1;
        /*if (lockFlag) return;*/
        if (type == SYSCALL) {
            switch (opt) {

                case 5:
                    regNum[2].s = t;
                    registerLock[2] = false;
                    break;
                case 9:
                    regNum[2].s = v0;
                    registerLock[2] = false;
                    break;
                case 10:
                    exit(0);
                case 17:
                    exit(a0);
            }
        }
        runningStage++;
        return true;
    }
};

class MoveData : public executionInstructionNewPipeLine {
public:
    int tmp;

    virtual bool ID() {
        /*if (lockFlag) return;*/
        switch (type) {
            case MOVE:
                if (registerLock[Rdest]) {
                    stageLock = 1;
                    return false;
                }
                tmp = regNum[Rdest].s;
                break;
            case MFHI:
                if (registerLock[HIREGISTER]) {
                    stageLock = 1;
                    return false;
                }
                tmp = regNum[HIREGISTER].s;
                break;
            case MFLO:
                if (registerLock[LOREGISTER]) {
                    stageLock = 1;
                    return false;
                }
                tmp = regNum[LOREGISTER].s;
                break;
        }
        registerLock[RSrc] = true;
        failToDo = false;
        runningStage++;
        return true;
    }

    virtual bool EX() {
        /*if (lockFlag) return;*/
        registerLock[RSrc] = true;
        runningStage++;
        return true;
    }

    virtual bool MEM() {
        /*if (lockFlag) return;*/
        registerLock[RSrc] = true;
        runningStage++;
        return true;
    }

    virtual bool WB() {
        registerLock[RSrc] = true;
        stageLock = -1;
        /*if (lockFlag) return;*/
        regNum[RSrc].s = tmp;
        registerLock[RSrc] = false;
        runningStage++;
        return true;
    }
};


#endif //MIPS_EXECUTEDINSTRUCTION_H
