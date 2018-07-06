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
    int runningStage = 0;

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

    virtual void ID() { Imm.s = _WORD(Rdest).s; }

    virtual void EX() {}

    virtual void MEM() {}

    virtual void WB() { regNum[RSrc].s = Imm.s; }
};

class Compare : public executionInstructionNewPipeLine {
public:
    _WORD _Rsrc1, _src2;
    bool result;

    virtual void ID() {
        _Rsrc1.s = regNum[Rdest].s;
        _src2.s = regNum[Src].s;
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
        regNum[RSrc].s = result;
    }
};

class IfAndJump : public executionInstructionNewPipeLine {
public:
    int nextExeLine;
    _WORD _Rsrc1, _src2;
    bool result;
    _WORD Reg31;

    virtual void ID() {
        lockFlag = true;
        switch (type) {
            case BEQ:
            case BNE:
            case BGE:
            case BLE:
            case BGT:
            case BLT:
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
                _Rsrc1.s = regNum[RSrc].s;
                nextExeLine = ARdest;
                break;
            case J:
            case B:
            case JAL:
                nextExeLine = ARSrc;
                break;
            case JR:
            case JALR:
                _Rsrc1.s = regNum[Rdest].s;
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
        switch (type) {
            case JAL:
            case JALR:
                regNum[31].s = Reg31.s;
        }
        lockFlag = false;
    }
};


class StoreData : public executionInstructionNewPipeLine {
public:
    int source;
    _WORD tmp, _Rsrc;

    virtual void ID() {
        if (BLRdest) tmp.s = LRdest; else tmp.s = regNum[Rdest].s;
        _Rsrc.s = regNum[RSrc].s;
    }

    virtual void EX() {
        source = tmp.us + offset;
    }

    virtual void MEM() {
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
    }

    virtual void WB() {}
};


class LoadData : public executionInstructionNewPipeLine {
public:
    int labelAddress, source;
    int _Rdest;
    _WORD _RSrc;
    virtual void ID() {
        switch (type){
            case LA:
                _Rdest = regNum[Rdest].s;
                break;
            case LB:
            case LH:
            case LW:
                _Rdest = regNum[Rdest].s;
                break;
        }
    }

    virtual void EX() {
        switch (type){
            case LA:
                labelAddress = (BLRdest ? LRdest : _Rdest);
                break;
            case LB:
            case LH:
            case LW:
                source = (BLRdest ? LRdest : _Rdest) + offset;
                break;
        }

    }

    virtual void MEM() {
        switch (type){
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
    }

    virtual void WB() {
        switch (type){
            case LB:
            case LH:
            case LW:
                regNum[RSrc] = _RSrc;
                break;
            case LA:
                regNum[RSrc] = labelAddress;
        }
    }
};

class Special : public executionInstructionNewPipeLine {
public:
    int opt;
    int a0, a1, v0;
    int i;
    int t;
    string str;
    virtual void ID() {
        opt = regNum[2].s;
        if(type == SYSCALL){
            switch (opt){
                case 8:
                    a1 = regNum[5].us;
                case 1:
                case 4:
                case 9:
                    a0 = regNum[4].s;
                    break;
            }
        }
    }

    virtual void EX() {

    }

    virtual void MEM() {
        if(type == SYSCALL){
            switch(opt){
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
                    for (i = 0; i < str.length() && i < a1 - 1; ++i){
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
    }

    virtual void WB() {
        if(type == SYSCALL){
            switch(opt){

                case 5:
                    regNum[2].s = t;
                    break;
                case 9:
                    regNum[2].s = v0;
                    break;
                case 10:
                    exit(0);
                case 17:
                    exit(a0);
            }
        }
    }
};

class MoveData : public executionInstructionNewPipeLine {
public:
    int tmp;

    virtual void ID() {
        switch (type) {
            case MOVE:
                tmp = regNum[Rdest].s;
                break;
            case MFHI:
                tmp = regNum[HIREGISTER].s;
                break;
            case MFLO:
                tmp = regNum[LOREGISTER].s;
                break;
        }
    }

    virtual void EX() {}

    virtual void MEM() {}

    virtual void WB() {
        regNum[RSrc].s = tmp;
    }
};


#endif //MIPS_EXECUTEDINSTRUCTION_H
