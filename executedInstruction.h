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

    executionInstructionNewPipeLine() : index(0), argc(0), RSrc(0x3f3f3f3f), Rdest(0x3f3f3f3f), Src(0x3f3f3f3f),
                                        offset(0),
                                        ARSrc(0x3f3f3f3f), ARdest(0x3f3f3f3f), ASrc(0x3f3f3f3f), LRSrc(0x3f3f3f3f),
                                        LRdest(0x3f3f3f3f), LSrc(0x3f3f3f3f) {
        argv.reserve(5);
    }

    virtual void ID() = 0;

    virtual void EX() = 0;

    virtual void MEM() = 0;

    virtual void WB() = 0;
};

class LogAndCal : public executionInstructionNewPipeLine {
public:
    _WORD _RSrc, _Rdest, _Src;
    _DWORD ansDword;
    _WORD ansWord, ansWord2;

    virtual void ID() {
        switch (type) {
            case ADDU:
            case ADDIU:
            case SUBU:
            case XORU:
            case REMU:
                _RSrc = regNum[RSrc].us;
                _Rdest = regNum[Rdest].us;
                _Src = f[2] ? regNum[Src].us : (unsigned int) Src;
                break;
            case ADD:
            case SUB:
            case XOR:
            case NEG:
            case REM:
                _RSrc = regNum[RSrc].s;
                _Rdest = regNum[Rdest].s;
                _Src = f[2] ? regNum[Src].s : (int) Src;
                break;
            case MUL:
            case DIV:
                if (argv.size() == 2) {
                    _Rdest = regNum[RSrc].s;
                    _Src = f[1] ? regNum[Rdest].s : Rdest;
                } else {
                    _RSrc = regNum[RSrc].s;
                    _Rdest = regNum[Rdest].s;
                    _Src = f[2] ? regNum[Src].s : (int) Src;
                }
                break;
            case MULU:
            case DIVU:
                if (argv.size() == 2) {
                    _Rdest = regNum[RSrc].us;
                    _Src = f[1] ? regNum[Rdest].us : Rdest;
                } else {
                    _RSrc = regNum[RSrc].us;
                    _Rdest = regNum[Rdest].us;
                    _Src = f[2] ? regNum[Src].us : (int) Src;
                }
        }
    }

    virtual void EX() {
        switch (type) {
            case ADDU:
            case ADDIU:
            case SUBU:
            case XORU:
            case REMU:
                ansWord = _WORD(_RSrc.us + _Src.us);
                break;
            case ADD:
            case SUB:
            case XOR:
            case NEG:
            case REM:
                ansWord = _WORD(_RSrc.s + _Src.s);
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
    }

    virtual void MEM() {}

    virtual void WB() {
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
                regNum[Rdest] = ansWord;
                break;
            case MUL:
            case DIV:
            case MULU:
            case DIVU:
                if (argv.size() == 2) {
                    regNum[HIREGISTER] = ansWord;
                    regNum[LOREGISTER] = ansWord2;
                } else regNum[Rdest] = ansWord;
                break;
        }
    }
};

class Li : public executionInstructionNewPipeLine {
public:
    _WORD Imm;

    virtual void ID() { Imm = _WORD(Rdest); }

    virtual void EX() {}

    virtual void MEM() {}

    virtual void WB() { regNum[RSrc] = Imm; }
};

class Compare : public executionInstructionNewPipeLine {
public:
    _WORD _Rsrc1, _src2;
    bool result;

    virtual void ID() {
        _Rsrc1 = regNum[Rdest];
        _src2 = regNum[Src];
    }

    virtual void EX() {
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
    }

    virtual void MEM() {}

    virtual void WB() {
        regNum[RSrc] = result;
    }
};

class IfAndJump : public executionInstructionNewPipeLine {
public:
    int nextExeLine;
    _WORD _Rsrc1, _src2;
    bool result;
    _WORD Reg31;

    virtual void ID() {
        switch (type) {
            case BEQ:
            case BNE:
            case BGE:
            case BLE:
            case BGT:
            case BLT:
                _Rsrc1 = regNum[RSrc];
                _src2 = regNum[Rdest];
                nextExeLine = ASrc;
                break;
            case BEQZ:
            case BNEZ:
            case BLEZ:
            case BGEZ:
            case BGTZ:
            case BLTZ:
                _Rsrc1 = regNum[RSrc];
                nextExeLine = ARdest;
                break;
            case J:
            case B:
            case JAL:
                nextExeLine = ARSrc;
                break;
            case JR:
            case JALR:
                _Rsrc1 = regNum[Rdest];
                break;
        }
    }

    virtual void EX() {
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
                Reg31 = nextLine + 1;
                nextLine = nextExeLine;
                haveJump = true;
                break;
            case JALR:
                Reg31 = nextLine + 1;
                nextLine = _Rsrc1.s;
                haveJump = true;
                break;
        }
    }

    virtual void MEM() {

    }

    virtual void WB() {
        switch (type){
            case JAL:
            case JALR:
                regNum[31] = Reg31;
        }
    }
};

class Load : public executionInstructionNewPipeLine {
public:

    virtual void ID() {

    }

    virtual void EX() {

    }

    virtual void MEM() {

    }

    virtual void WB() {

    }
};

class Store : public executionInstructionNewPipeLine {
public:

    virtual void ID() {

    }

    virtual void EX() {

    }

    virtual void MEM() {

    }

    virtual void WB() {

    }

};

class StoreData : public executionInstructionNewPipeLine {
public:

    virtual void ID() {

    }

    virtual void EX() {

    }

    virtual void MEM() {

    }

    virtual void WB() {

    }
};


class MoveData : public executionInstructionNewPipeLine {

public:

    virtual void ID() {

    }

    virtual void EX() {

    }

    virtual void MEM() {

    }

    virtual void WB() {

    }
};

class Special : public executionInstructionNewPipeLine {
public:

    virtual void ID() {

    }

    virtual void EX() {

    }

    virtual void MEM() {

    }

    virtual void WB() {

    }
};


#endif //MIPS_EXECUTEDINSTRUCTION_H
