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
        if (lockFlag) return;
        switch (type) {
            case ADDU:
            case ADDIU:
            case SUBU:
            case XORU:
            case REMU:
                if (registerLock[RSrc]) return;
                if (registerLock[Rdest]) return;
                if (f[2] && registerLock[Src]) return;
                _RSrc = regNum[RSrc].us;
                _Rdest = regNum[Rdest].us;
                _Src = f[2] ? regNum[Src].us : (unsigned int) Src;
                break;
            case ADD:
            case SUB:
            case XOR:
            case NEG:
            case REM:
                if (registerLock[RSrc]) return;
                if (registerLock[Rdest]) return;
                if (f[2] && registerLock[Src]) return;
                _RSrc = regNum[RSrc].s;
                _Rdest = regNum[Rdest].s;
                _Src = f[2] ? regNum[Src].s : (int) Src;
                break;
            case MUL:
            case DIV:
                if (argv.size() == 2) {
                    if (registerLock[RSrc]) return;
                    if (f[1] && registerLock[Rdest]) return;
                    _Rdest = regNum[RSrc].s;
                    _Src = f[1] ? regNum[Rdest].s : Rdest;
                } else {
                    if (registerLock[RSrc]) return;
                    if (registerLock[Rdest]) return;
                    if (f[2] && registerLock[Src]) return;
                    _RSrc = regNum[RSrc].s;
                    _Rdest = regNum[Rdest].s;
                    _Src = f[2] ? regNum[Src].s : (int) Src;
                }
                break;
            case MULU:
            case DIVU:
                if (argv.size() == 2) {
                    if (registerLock[RSrc]) return;
                    if (f[1] && registerLock[Rdest]) return;
                    _Rdest = regNum[RSrc].us;
                    _Src = f[1] ? regNum[Rdest].us : Rdest;
                } else {
                    if (registerLock[RSrc]) return;
                    if (registerLock[Rdest]) return;
                    if (f[2] && registerLock[Src]) return;
                    _RSrc = regNum[RSrc].us;
                    _Rdest = regNum[Rdest].us;
                    _Src = f[2] ? regNum[Src].us : (int) Src;
                }
        }

        if (argv.size() == 2) registerLock[HIREGISTER] = registerLock[LOREGISTER] = true;
        else registerLock[Rdest] = true;
        runningStage++;
    }

    virtual void EX() {
        if (lockFlag) return;
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
        runningStage++;
    }

    virtual void MEM() {
        if (lockFlag) return;
        runningStage++;
    }

    virtual void WB() {
        if (lockFlag) return;
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
                    registerLock[HIREGISTER] = registerLock[LOREGISTER] = false;
                } else regNum[Rdest] = ansWord, registerLock[Rdest] = false;
                break;
        }
        runningStage++;
    }
};

class Li : public executionInstructionNewPipeLine {
public:
    _WORD Imm;

    virtual void ID() {
        if (lockFlag) return;
        Imm.s = _WORD(Rdest).s;
        registerLock[RSrc] = true;
        runningStage++;
    }

    virtual void EX() {
        if (lockFlag) return;
        runningStage++;
    }

    virtual void MEM() {
        if (lockFlag) return;
        runningStage++;
    }

    virtual void WB() {
        if (lockFlag) return;
        regNum[RSrc].s = Imm.s;
        registerLock[RSrc] = false;
        runningStage++;
    }
};

class Compare : public executionInstructionNewPipeLine {
public:
    _WORD _Rsrc1, _src2;
    bool result;

    virtual void ID() {
        if (lockFlag) return;
        if (registerLock[Rdest] || registerLock[Src])
            return;
        _Rsrc1.s = regNum[Rdest].s;
        _src2.s = regNum[Src].s;
        registerLock[RSrc] = true;
        runningStage++;
    }

    virtual void EX() {
        if (lockFlag) return;
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
    }

    virtual void MEM() {
        if (lockFlag) return;
        runningStage++;
    }

    virtual void WB() {
        if (lockFlag) return;
        regNum[RSrc].s = result;
        registerLock[RSrc] = false;
        runningStage++;
    }
};

class IfAndJump : public executionInstructionNewPipeLine {
public:
    int nextExeLine;
    _WORD _Rsrc1, _src2;
    bool result;
    _WORD Reg31;

    virtual void ID() {
        if (lockFlag) return;
        lockFlag = true;
        switch (type) {
            case BEQ:
            case BNE:
            case BGE:
            case BLE:
            case BGT:
            case BLT:
                if(registerLock[RSrc] || registerLock[Rdest])
                    return;
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
                if(registerLock[RSrc])
                    return;
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
                if(registerLock[Rdest]) return;
                _Rsrc1.s = regNum[Rdest].s;
                break;
        }
        if(type == JAL ||type == JALR) registerLock[31] = true;
        runningStage++;
    }

    virtual void EX() {
        if (lockFlag) return;
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
        runningStage++;
    }

    virtual void MEM() {
        if (lockFlag) return;
        runningStage++;
    }

    virtual void WB() {
        if (lockFlag) return;
        switch (type) {
            case JAL:
            case JALR:
                regNum[31].s = Reg31.s;
                registerLock[31] = false;
        }
        lockFlag = false;
        runningStage++;
    }
};


class StoreData : public executionInstructionNewPipeLine {
public:
    int source;
    _WORD tmp, _Rsrc;

    virtual void ID() {
        if (lockFlag) return;
        if(registerLock[Rdest] || registerLock[RSrc]) return;
        if (BLRdest) tmp.s = LRdest; else tmp.s = regNum[Rdest].s;
        _Rsrc.s = regNum[RSrc].s;
        runningStage++;
    }

    virtual void EX() {
        if (lockFlag) return;
        source = tmp.us + offset;
        runningStage++;
    }

    virtual void MEM() {
        if (lockFlag) return;
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
    }

    virtual void WB() {
        if (lockFlag) return;
        runningStage++;
    }
};


class LoadData : public executionInstructionNewPipeLine {
public:
    int labelAddress, source;
    int _Rdest;
    _WORD _RSrc;

    virtual void ID() {
        if (lockFlag) return;
        switch (type) {
            case LA:
                if(!BLRdest && registerLock[Rdest])
                    return;
                _Rdest = BLRdest ? LRdest : regNum[Rdest].s;
                break;
            case LB:
            case LH:
            case LW:
                if(!BLRdest && registerLock[Rdest])
                    return;
                _Rdest = BLRdest ? LRdest : regNum[Rdest].s;
                break;
        }
        runningStage++;
        registerLock[RSrc] = true;
    }

    virtual void EX() {
        if (lockFlag) return;
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
    }

    virtual void MEM() {
        if (lockFlag) return;

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
    }

    virtual void WB() {
        if (lockFlag) return;
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
        if (lockFlag) return;
        if(registerLock[2]) return;
        opt = regNum[2].s;
        if (type == SYSCALL) {
            switch (opt) {
                case 8:
                    if(registerLock[5]) return;
                    a1 = regNum[5].us;
                case 1:
                case 4:
                case 9:
                    if(registerLock[4]) return;
                    a0 = regNum[4].s;
                    registerLock[2] = true;
                    break;
            }
            if(opt == 5) registerLock[2] = true;
        }

        runningStage++;
    }

    virtual void EX() {
        if (lockFlag) return;
        runningStage++;
    }

    virtual void MEM() {
        if (lockFlag) return;
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
    }

    virtual void WB() {
        if (lockFlag) return;
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
    }
};

class MoveData : public executionInstructionNewPipeLine {
public:
    int tmp;

    virtual void ID() {
        if (lockFlag) return;
        switch (type) {
            case MOVE:
                if(registerLock[Rdest]) return;
                tmp = regNum[Rdest].s;
                break;
            case MFHI:
                if(registerLock[HIREGISTER]) return;
                tmp = regNum[HIREGISTER].s;
                break;
            case MFLO:
                if(registerLock[LOREGISTER]) return;
                tmp = regNum[LOREGISTER].s;
                break;
        }
        registerLock[RSrc] = true;
        runningStage++;
    }

    virtual void EX() {
        if (lockFlag) return;
        runningStage++;
    }

    virtual void MEM() {
        if (lockFlag) return;
        runningStage++;
    }

    virtual void WB() {
        if (lockFlag) return;
        regNum[RSrc].s = tmp;
        registerLock[RSrc] = false;
        runningStage++;
    }
};


#endif //MIPS_EXECUTEDINSTRUCTION_H
