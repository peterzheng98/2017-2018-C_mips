//
// Created by Peter Zheng on 2018/06/30.
//

#ifndef MIPS_MIPS_MAIN_H
#define MIPS_MIPS_MAIN_H

#include "stdc++.h"
#include "Base.h"
#include "utilities.h"
#include "parser.h"
#include "executedInstruction.h"

#define DADDR unsigned char
using namespace std;

static int arga1 = 0;
static int arga2 = 0;

class mips {
private:
    int memHead, stackTop = memoryMAX - 1;

    int mainEntryPoint; //the Main Entry Point

    _WORD regNum[regNUMMAX];
    DADDR mem[memoryMAX];

    string coreData;

    vector<executionInstructionNew> programSentenceNew;

    void debugDataPrint() {
        cout << "=================================================================\n";
        cout << "Debug Data\n";
        cout << "=================================================================\n";
        cout << "Numeric memHead:" << memHead << endl;
        cout << "Data:\n";
        for (int i = 0; i < memHead + 10; i++)
            cout << "Dat[" << i << "]:[" << (int) mem[i] << "]" << endl;
        cout << "Variable Pointer:" << endl;
        for (map<string, int>::iterator ite = Parser.labelMap.begin(); ite != Parser.labelMap.end(); ite++) {
            cout << "Name[" << ite->first << "]  Address:" << ite->second;
            cout << endl;
        }
        cout << "=================================================================\n";
        cout << "Debug End\n";
        cout << "=================================================================\n\n\n";
        cout << flush;
    }

    map<string, int> executionMap;

    inline int checkLabel(const string &rhs) {
        if (executionMap.find(rhs) == executionMap.end()) return -1;
        return executionMap[rhs];
    }

public:
    mips() {
        for (int i = 0; i < regNUMMAX; i++) regNum[i] = 0;
        for (int i = 0; i < memoryMAX; i++) mem[i] = 0;
        memHead = 0;
        programSentenceNew.reserve(10000);
    }

    void instructionFetch() {
        if (controlDebug) debugMess("Instruction Fetch(IF) Started.", "General - IF");

    }

    void instructionDecode() {

    }

    void execution() {

    }

    void memoryAccess() {

    }

    void writeBack() {

    }

    void setInstructionNew(const string &rhs) {
        coreData = rhs;
        stringstream ss;
        ss << rhs;
        string token;
        bool dataField = false;
        bool textField = false;
        string lastLabel = "";
        string NextToken = "";
        bool haveLabel = false;
        int currentSentence = 0;
        while (haveLabel || ss >> token) {
            if (haveLabel) { token = NextToken; }
            if (token == ".data") {
                dataField = true;
                textField = false;
                if (controlDebug) debugMess("In Section: Data Started.", "Parser");
                haveLabel = false;
                continue;
            }
            if (token == ".text") {
                dataField = false;
                textField = true;
                if (controlDebug) debugMess("In Section: Text Started.", "Parser");
                haveLabel = false;
                continue;
            }

            if (dataField) {
                Instruction result = Parser.Type(token);
                if (controlDebug) {
                    string str("Found Token [");
                    str += (int) result;
                    str += "]";
                    debugMess(str, "Parser - Data Field");
                }
                string next;
                string mess;
                if (result == ALIGN) {
                    int n;
                    ss >> n;
                    // Debug Sect
                    int dbg = memHead;
                    if ((memHead + 1) % n != 0) memHead = (memHead / (1 << n) + 1) * (1 << n) - 1;
                    if (controlDebug) {
                        debugMess("", "In Align Section");
                        cout << flush;
                        cout << "memHead Old:[" << dbg << "] New:[" << memHead << "]" << endl;
                    }
                } else if (result == ASCIIZ) {
                    char ch;
                    ch = ss.get();
                    ch = ss.get();
                    while (ch != '\n') {
                        next += ch;
                        ch = ss.get();
                    }
//                    ss >> next;
                    deleteCertainChar(next, '\"');
                    next = decodeSpecial(next);
                    if (controlDebug) debugMess("In ASCIIZ", "Parser - Data Field");
                    for (int i = 0; i < next.length(); i++) {
                        mem[memHead] = (unsigned char) (next[i]);
                        memHead++;
                    }
                    mem[memHead++] = (unsigned char) 0;
                    if (controlDebug) debugDataPrint();
                } else if (result == ASCII) {
                    ss >> next;
                    deleteCertainChar(next, '\"');
                    next = decodeSpecial(next);
                    if (controlDebug) debugMess("In ASCII", "Parser - Data Field");
                    for (int i = 0; i < next.length(); i++) {
                        mem[memHead] = (unsigned char) (next[i]);
                        memHead++;
                    }
                } else if (result == BYTE) {
                    if (controlDebug) debugMess("In BYTE", "Parser - Data Field");
                    ss >> next;
                    while (next[0] <= '9' && next[0] >= '0') {
                        unsigned char final = (unsigned char) string2int(next);
                        mem[memHead++] = final;
                        ss >> next;
                    }
//                    ss << next;
                    haveLabel = true;
                    NextToken = next;
                    if (controlDebug) debugDataPrint();
                } else if (result == HALF) {
                    if (controlDebug) debugMess("In HALF", "Parser - Data Field");
                    ss >> next;
                    while (next[0] <= '9' && next[0] >= '0') {
                        _HALF u = _HALF((short) string2int(next));
                        mem[memHead++] = u.core.u1;
                        mem[memHead++] = u.core.u2;
                        ss >> next;
                    }
//                    ss << next;
                    haveLabel = true;
                    NextToken = next;
                    if (controlDebug) debugDataPrint();
                } else if (result == WORD) {
                    if (controlDebug) debugMess("In WORD", "Parser - Data Field");
                    ss >> next;
                    while (next[0] <= '9' && next[0] >= '0') {
                        _WORD u = _WORD(string2int(next));
                        mem[memHead++] = u.core.u1;
                        mem[memHead++] = u.core.u2;
                        mem[memHead++] = u.core.u3;
                        mem[memHead++] = u.core.u4;
                        ss >> next;
                    }
                    //if(controlDebug) cout << "Debug:[" << ss.str() << "]" << endl;
//                    ss << next;
                    haveLabel = true;
                    NextToken = next;
                } else if (result == SPACE) {
                    if (controlDebug) debugMess("In SPACE", "Parser - Data Field");
                    int n1;
                    ss >> n1;
                    memHead += n1;
                    if (controlDebug) debugDataPrint();
                } else if (result == LABEL) {
                    if (controlDebug) debugMess("In LABEL", "Parser - Data Field");
                    if (haveLabel) {
                        next = NextToken;
                        haveLabel = false;
                    }// else ss >> next;
                    else next = token;
                    deleteCertainChar(next, ':');
                    if (controlDebug) cout << "Variable:" << next << "|\n" << endl;
                    Parser.labelMap[next] = memHead;
                    haveLabel = false;
                } else if (controlDebug) debugMess("In OTHER", "Parser - Data Field");
            }
            if (textField) {
                Instruction result = Parser.Type(token);
                if (controlDebug) {
                    string str("Found Token [");
                    str += (int) result;
                    str += "]";
                    debugMess(str, "Parser - Text Field");
                }
                string next;
                string mess;
                if (result == LABEL) {
                    deleteCertainChar(token, ':');
                    string labelName = token;
                    executionMap[labelName] = programSentenceNew.size();
                    if (labelName == "main") mainEntryPoint = programSentenceNew.size();
                } else {
                    executionInstructionNew s1;
                    s1.type = result;
                    if (result == ADD || result == ADDU || result == ADDIU || result == SUB ||
                        result == SUBU || result == XOR || result == XORU || result == REM ||
                        result == REMU || result == SEQ || result == SGE || result == SGT ||
                        result == SLE || result == SLT || result == SNE) {
                        string arg1, arg2, arg3;
                        ss >> arg1 >> arg2 >> arg3;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        deleteCertainChar(arg3, ',');
                        s1.argc = 3;

                        s1.argv.push_back(arg1);
                        s1.argv.push_back(arg2);
                        s1.argv.push_back(arg3);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1.RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1.Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1.offset = offset;
                        }
                        if (arg3[0] == '$') {
                            s1.Src = Parser.registerMap[arg3];
                        } else if (allNumber(arg3)) {
                            s1.Src = string2int(arg3);
                        }

                        programSentenceNew.push_back(s1);
                    }
                    if (result == BEQ ||
                        result == BNE || result == BGE || result == BLE || result == BGT ||
                        result == BLT) {
                        string arg1, arg2, arg3;
                        ss >> arg1 >> arg2 >> arg3;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        deleteCertainChar(arg3, ',');
                        s1.argc = 3;

                        s1.argv.push_back(arg1);
                        s1.argv.push_back(arg2);
                        s1.argv.push_back(arg3);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1.RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1.Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1.offset = offset;
                        } else if (allNumber(arg2)) {
                            s1.Rdest = string2int(arg2);
                        }
                        if (arg3[0] == '$') {
                            s1.Src = Parser.registerMap[arg3];
                        }

                        programSentenceNew.push_back(s1);

                    }

                    if (result == LI || result == BEQZ || result == BNEZ || result == BLEZ || result == BGEZ ||
                        result == BGTZ || result == BLTZ || result == LA || result == LB || result == LH ||
                        result == LW || result == SB || result == SH || result == SW || result == MOVE ||
                        result == NEG || result == NEGU) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        s1.argc = 2;

                        s1.argv.push_back(arg1);
                        s1.argv.push_back(arg2);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1.RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1.Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1.offset = offset;
                            s1.Rdest = Parser.registerMap[right];
                        } else if (allNumber(arg2)) {
                            s1.Rdest = string2int(arg2);
                        }

                        programSentenceNew.push_back(s1);
                    }
                    if (result == B || result == J || result == JR || result == JAL || result == JALR ||
                        result == MFHI || result == MFLO) {
                        string arg1;
                        ss >> arg1;
                        deleteCertainChar(arg1, ',');

                        s1.argc = 1;

                        s1.argv.push_back(arg1);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1.RSrc = Parser.registerMap[arg1];
                        }
                        programSentenceNew.push_back(s1);
                    }
                    if (result == NOP) {
                        programSentenceNew.push_back(s1);
                    }
                    if (result == SYSCALL) {
                        programSentenceNew.push_back(s1);
                    }
                    if (result == MUL || result == MULU || result == DIV || result == DIVU) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');

                        int argc = 2;

                        s1.argv.push_back(arg1);
                        s1.argv.push_back(arg2);
                        streampos save = ss.tellg();
                        string ss3, arg3;
                        ss >> ss3;
                        short code = -1;
                        for (int i = 0; i < ss3.length(); ++i) {
                            if (ss3[i] == '\n') {
                                code = 10;
                                break;
                            }
                            if (ss3[i] <= '9' && ss3[i] >= '0') {
                                code = 52;
                                break;
                            }
                        }
                        ss.seekg(save);
                        if (code == 52) {
                            string s3;
                            ss >> s3;
                            deleteCertainChar(s3, ',');
                            s1.argv.push_back(s3);
                            arg3 = s3;
                            argc = 3;
                        }

                        if (argc == 2) {
                            //解析寄存器对象
                            if (arg1[0] == '$') {
                                s1.RSrc = Parser.registerMap[arg1];
                            }
                            if (arg2[0] == '$') {
                                s1.Rdest = Parser.registerMap[arg2];
                            } else if (haveBrackets(arg2)) {
                                string right = arg2;
                                string left = splitWithCertainChar(right, '(');
                                deleteCertainChar(right, ')');
                                int offset = string2int(left);
                                s1.offset = offset;
                                s1.Rdest = Parser.registerMap[right];
                            }
                        } else {
                            if (arg1[0] == '$') {
                                s1.RSrc = Parser.registerMap[arg1];
                            }
                            if (arg2[0] == '$') {
                                s1.Rdest = Parser.registerMap[arg2];
                            } else if (haveBrackets(arg2)) {
                                string right = arg2;
                                string left = splitWithCertainChar(right, '(');
                                deleteCertainChar(right, ')');
                                int offset = string2int(left);
                                s1.offset = offset;
                                s1.Rdest = Parser.registerMap[right];
                            }
                            if (arg3[0] == '$') {
                                s1.Src = Parser.registerMap[arg3];
                            } else if (allNumber(arg3)) {
                                s1.Src = string2int(arg3);
                            }
                        }
                        s1.argc = argc;
                        programSentenceNew.push_back(s1);
                    }
                }

            }
        }
        for (int i = 0; i < programSentenceNew.size(); ++i) {
            if (programSentenceNew[i].argc == 0) continue;
            if (programSentenceNew[i].argv[0] != "") {
                int Addr = checkLabel(programSentenceNew[i].argv[0]);
//                if(Addr == -1) throw 1; // Error: Not found the label;
                programSentenceNew[i].ARSrc = (Addr == -1 ? 0x3f3f3f3f : Addr);
            }
            if (programSentenceNew[i].argv[0] != "") {
                int Addr = Parser.LabelAddr(programSentenceNew[i].argv[0]);
                programSentenceNew[i].LRSrc = (Addr == -1 ? 0x3f3f3f3f : Addr);
                programSentenceNew[i].BLRSrc = (Addr == -1 ? false : true);
            }

            if (programSentenceNew[i].argc == 1) continue;
            if (programSentenceNew[i].argv[1] != "") {
                int Addr = checkLabel(programSentenceNew[i].argv[1]);
//                if(Addr == -1) throw 1; // Error: Not found the label;
                programSentenceNew[i].ARdest = (Addr == -1 ? 0x3f3f3f3f : Addr);
            }
            if (programSentenceNew[i].argv[1] != "") {
                int Addr = Parser.LabelAddr(programSentenceNew[i].argv[1]);
                programSentenceNew[i].LRdest = (Addr == -1 ? 0x3f3f3f3f : Addr);
                programSentenceNew[i].BLRdest = (Addr == -1 ? false : true);
            }

            if (programSentenceNew[i].argc == 2) continue;
            if (programSentenceNew[i].argv[2] != "") {
                int Addr = checkLabel(programSentenceNew[i].argv[2]);
//                if(Addr == -1) throw 1; // Error: Not found the label;
                programSentenceNew[i].ASrc = (Addr == -1 ? 0x3f3f3f3f : Addr);
            }
            if (programSentenceNew[i].argv[2] != "") {
                int Addr = Parser.LabelAddr(programSentenceNew[i].argv[2]);
                programSentenceNew[i].LSrc = (Addr == -1 ? 0x3f3f3f3f : Addr);
                programSentenceNew[i].BLSrc = (Addr == -1 ? false : true);
            }
        }

        //cout << "Parser \n";
    }

    void directlyRun_DR_New() {
        if (controlDebug) {
            map<string, int>::iterator ite;
            cout << "Label Table" << endl;
            for (ite = executionMap.begin(); ite != executionMap.end(); ite++) {
                cout << "Label:" << ite->first << "  Line:" << ite->second << endl;
            }
        }

        int current = mainEntryPoint;
        regNum[29] = stackTop;
        while (current < programSentenceNew.size()) {
            executionInstructionNew &tmp = programSentenceNew[current];
            if (controlDebug) cout << current << "Running:";
            //设置跳转表示符号
            bool jumpFlag = false;
            bool f[3] = {false};
            string s[3] = {""};
            int a[3] = {0};
            for (int i = 0; i < tmp.argc; ++i) {
                if (tmp.argv[i][0] == '$') f[i] = true; else f[i] = false;
                s[i] = tmp.argv[i];
                if (i == 0) a[i] = tmp.RSrc;
                if (i == 1) a[i] = tmp.Rdest;
                if (i == 2) a[i] = tmp.Src;
            }
            //申明Switch语句中可能使用到的变量
            _DWORD tmp2;
            register int nextExe;
            register int labelAddress;
            register bool haveBracket;
            register string left, right;
            register int offset = 0;
            register int source;
            //根据语句类型执行语句
            switch (tmp.type) {
                case ADD:
                    regNum[tmp.RSrc].s = (tmp.argv[2][0] == '$' ? regNum[tmp.Src].s : tmp.Src) + regNum[tmp.Rdest].s;
                    if (controlDebug) cout << __LINE__ << ": Stage: ADD" << endl;
                    break;
                case ADDU:
                    regNum[tmp.RSrc].us = (f[2] ? regNum[tmp.Src].us : (unsigned int) tmp.Src) + regNum[tmp.Rdest].us;
                    if (controlDebug) cout << __LINE__ << ": Stage: ADDU" << endl;
                    break;
                case ADDIU:
                    regNum[a[0]].us = (f[2] ? regNum[tmp.Src].us : (unsigned int) tmp.Src) + regNum[tmp.Rdest].us;
                    if (controlDebug) cout << __LINE__ << ": Stage: ADDIU" << endl;
                    break;
                case SUB:
                    regNum[a[0]].s = regNum[tmp.Rdest].s - (f[2] ? regNum[tmp.Src].s : tmp.Src);
                    if (controlDebug) cout << __LINE__ << ": Stage: SUB" << endl;
                    break;
                case SUBU:
                    regNum[a[0]].us = regNum[tmp.Rdest].us - (f[2] ? regNum[tmp.Src].us : (unsigned int) tmp.Src);
                    if (controlDebug) cout << __LINE__ << ": Stage: SUBU" << endl;

                    break;
                case MUL:
                    if (tmp.argv.size() == 2) {
                        tmp2 = (long long) (f[1] ? regNum[tmp.Rdest].s : a[1]) * (long long) regNum[a[0]].s;
                        regNum[HIREGISTER] = _WORD(tmp2.core.u1, tmp2.core.u2, tmp2.core.u3, tmp2.core.u4);
                        regNum[LOREGISTER] = _WORD(tmp2.core.u5, tmp2.core.u6, tmp2.core.u7, tmp2.core.u8);
                    } else regNum[a[0]].s = (tmp.argv[2][0] == '$' ? regNum[tmp.Src].s : tmp.Src) * regNum[tmp.Rdest].s;
                    if (controlDebug) cout << __LINE__ << ": Stage: MUL" << endl;

                    break;
                case MULU:
                    if (tmp.argv.size() == 2) {
                        tmp2 = (unsigned long long) (f[1] ? regNum[tmp.Rdest].us : a[1]) *
                               (unsigned long long) regNum[a[0]].us;
                        regNum[HIREGISTER] = _WORD(tmp2.core.u1, tmp2.core.u2, tmp2.core.u3, tmp2.core.u4);
                        regNum[LOREGISTER] = _WORD(tmp2.core.u5, tmp2.core.u6, tmp2.core.u7, tmp2.core.u8);
                    } else regNum[a[0]].us = (f[2] ? regNum[tmp.Src].us : a[2]) * regNum[tmp.Rdest].us;
                    if (controlDebug) cout << __LINE__ << ": Stage: MULU" << endl;

                    break;
                case DIVU:
                    if (tmp.argv.size() == 2) {
                        regNum[HIREGISTER] = _WORD(regNum[a[0]].us % (f[1] ? regNum[tmp.Rdest].us : a[1]));
                        regNum[LOREGISTER] = _WORD(regNum[a[0]].us / (f[1] ? regNum[tmp.Rdest].us : a[1]));
                    } else regNum[a[0]].us = regNum[tmp.Rdest].us / (f[2] ? regNum[tmp.Src].us : a[2]);
                    if (controlDebug) cout << __LINE__ << ": Stage: DIVU" << endl;

                    break;
                case DIV:
                    if (tmp.argv.size() == 2) {
                        regNum[HIREGISTER] = _WORD(regNum[a[0]].s % (f[1] ? regNum[tmp.Rdest].s : a[1]));
                        regNum[LOREGISTER] = _WORD(regNum[a[0]].s / (f[1] ? regNum[tmp.Rdest].s : a[1]));
                    } else regNum[a[0]].us = regNum[tmp.Rdest].us / (f[2] ? regNum[tmp.Src].us : a[2]);
                    if (controlDebug) cout << __LINE__ << ": Stage: DIV" << endl;

                    break;
                case XOR:
                    regNum[a[0]].s = regNum[tmp.Rdest].s ^ (f[2] ? regNum[tmp.Src].s : a[2]);
                    if (controlDebug) cout << __LINE__ << ": Stage: XOR" << endl;

                    break;
                case XORU:
                    regNum[a[0]].us = regNum[tmp.Rdest].us ^ (f[2] ? regNum[tmp.Src].us : a[2]);
                    if (controlDebug) cout << __LINE__ << ": Stage: XORU" << endl;

                    break;
                case NEG:
                    regNum[a[0]].s = -regNum[tmp.Rdest].s;
                    if (controlDebug) cout << __LINE__ << ": Stage: NEG" << endl;

                    break;
                case NEGU:
                    regNum[a[0]].us = ~regNum[tmp.Rdest].us;
                    if (controlDebug) cout << __LINE__ << ": Stage: NEGU" << endl;

                    break;
                case REM:
                    regNum[a[0]].s = regNum[tmp.Rdest].s % (f[2] ? regNum[tmp.Src].s : a[2]);
                    if (controlDebug) cout << __LINE__ << ": Stage: REM" << endl;

                    break;
                case REMU:
                    regNum[a[0]].us = regNum[tmp.Rdest].us % (f[2] ? regNum[tmp.Src].us : a[2]);
                    if (controlDebug) cout << __LINE__ << ": Stage: REMU" << endl;

                    break;
                case LI:
                    regNum[a[0]] = _WORD(a[1]);
                    if (controlDebug) cout << __LINE__ << ": Stage: LI" << endl;

                    break;
                case SEQ:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s == (f[2] ? regNum[tmp.Src].s : a[2]));
                    if (controlDebug) cout << __LINE__ << ": Stage: SEQ" << endl;

                    break;
                case SGE:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s >= (f[2] ? regNum[tmp.Src].s : a[2]));
                    if (controlDebug) cout << __LINE__ << ": Stage: SGE" << endl;

                    break;
                case SGT:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s > (f[2] ? regNum[tmp.Src].s : a[2]));
                    if (controlDebug) cout << __LINE__ << ": Stage: SGT" << endl;

                    break;
                case SLE:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s <= (f[2] ? regNum[tmp.Src].s : a[2]));
                    if (controlDebug) cout << __LINE__ << ": Stage: SLE" << endl;

                    break;
                case SLT:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s < (f[2] ? regNum[tmp.Src].s : a[2]));
                    if (controlDebug) cout << __LINE__ << ": Stage: SLT" << endl;

                    break;
                case SNE:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s != (f[2] ? regNum[tmp.Src].s : a[2]));
                    if (controlDebug) cout << __LINE__ << ": Stage: SNE" << endl;

                    break;
                case J:
                case B:
                    nextExe = tmp.ARSrc;
                    current = nextExe;
                    jumpFlag = true;
                    if (controlDebug) cout << __LINE__ << ": Stage: J/B" << endl;
                    break;
                case BEQ:
                    if (regNum[a[0]].s == (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BEQ" << endl;

                    break;
                case BNE:
                    if (regNum[a[0]].s != (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BNE" << endl;

                    break;
                case BGE:
                    if (regNum[a[0]].s >= (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BGE" << endl;

                    break;
                case BLE:
                    if (regNum[a[0]].s <= (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BLE" << endl;

                    break;
                case BGT:
                    if (regNum[a[0]].s > (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BGT" << endl;

                    break;
                case BLT:
                    if (regNum[a[0]].s < (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BLT" << endl;

                    break;
                case BEQZ:
                    if (regNum[a[0]].s == 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BEQZ" << endl;

                    break;
                case BNEZ:
                    if (regNum[a[0]].s != 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BNEZ" << endl;

                    break;
                case BLEZ:
                    if (regNum[a[0]].s <= 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BLEZ" << endl;

                    break;
                case BGEZ:
                    if (regNum[a[0]].s >= 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BGEZ" << endl;

                    break;
                case BGTZ:
                    if (regNum[a[0]].s > 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BGTZ" << endl;

                    break;
                case BLTZ:
                    if (regNum[a[0]].s < 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: BLTZ" << endl;

                    break;
                case JR:
                    current = regNum[a[0]].s;
                    jumpFlag = true;
                    if (controlDebug) cout << __LINE__ << ": Stage: JR" << endl;

                    break;
                case JAL:
                    nextExe = tmp.ARSrc;

                    jumpFlag = true;
                    regNum[31] = current + 1;
                    current = nextExe;
                    if (controlDebug) cout << __LINE__ << ": Stage: JAL" << endl;

                    break;
                case JALR:

                    jumpFlag = true;
                    regNum[31] = current + 1;
                    current = regNum[a[0]].s;
                    if (controlDebug) cout << __LINE__ << ": Stage: JALR" << endl;

                    break;
                case LA:
                    labelAddress = Parser.labelMap[s[1]];
                    regNum[a[0]] = labelAddress;
                    if (controlDebug) cout << __LINE__ << ": Stage: LA" << endl;

                    break;
                case LB:
                    source = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s) + tmp.offset;
                    regNum[a[0]] = _WORD(int(mem[source]));
                    if (controlDebug) cout << __LINE__ << ": Stage: LB" << endl;

                    break;
                case LH:
                    source = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s) + tmp.offset;
                    regNum[a[0]] = _WORD((int) (_HALF(mem[source], mem[source] + 1).s));
                    if (controlDebug) cout << __LINE__ << ": Stage: LH" << endl;

                    break;
                case LW:
//                    cout << "Debug a[1] " << a[1] << endl;
                    //搜索有没有括号
                    if (a[1] == 0x3f3f3f3f && !tmp.BLRdest) {
                        arga2++;
                        haveBracket = haveBrackets(s[1]);
                        if (haveBracket) {//如果含有括号
                            right = s[1];
                            left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            offset = string2int(left);
//                        offset /= 4;
                        }
                        if (haveBracket && right[0] == '$') a[1] = regNum[Parser.registerMap[right]].s;
//                    cout << "a[1] " << a[1] << " " << Parser.registerMap[right] << endl;
                        source = (haveAlpha(s[1]) ? Parser.labelMap[s[1]] : a[1]) + offset;
                    } else {
                        source = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s) + tmp.offset;
                    }
                    regNum[a[0]] = _WORD(mem[source], mem[source + 1], mem[source + 2], mem[source + 3]);
                    if (controlDebug) cout << __LINE__ << ": Stage: LW" << endl;

                    break;
                case SB:
                    /*if (a[1] == 0x3f3f3f3f) {
                        haveBracket = haveBrackets(s[1]);
                        if (haveBracket) {//如果含有括号
                            right = s[1];
                            left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            offset = string2int(left);
//                        offset /= 4;
                        }
                        if (haveBracket && right[0] == '$') a[1] = regNum[Parser.registerMap[right]].s;
//                    cout << "a[1] " << a[1] << " " << Parser.registerMap[right] << endl;
                        source = (haveAlpha(s[1]) ? Parser.labelMap[s[1]] : a[1]) + offset;
                    } else {*/
                        source = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s) + tmp.offset;
                    //}
                    mem[source] = regNum[a[0]].core.u1;
                    if (controlDebug)
                        cout << endl << "Stage SB: " << source << " |In Memory: " << (int) mem[source] << endl;
                    if (controlDebug) cout << __LINE__ << ": Stage: SB" << endl;

                    break;
                case SH:
                    /*if (a[1] == 0x3f3f3f3f) {
                        haveBracket = haveBrackets(s[1]);
                        if (haveBracket) {//如果含有括号
                            right = s[1];
                            left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            offset = string2int(left);
//                        offset /= 4;
                        }
                        if (haveBracket && right[0] == '$') a[1] = regNum[Parser.registerMap[right]].s;
//                    cout << "a[1] " << a[1] << " " << Parser.registerMap[right] << endl;
                        source = (haveAlpha(s[1]) ? Parser.labelMap[s[1]] : a[1]) + offset;
                    } else {*/
                        source = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s) + tmp.offset;
                    //}
                    mem[source] = regNum[a[0]].core.u1;
                    mem[source + 1] = regNum[a[0]].core.u2;
                    if (controlDebug)
                        cout << endl << "Stage SH: " << source << " |In Memory: " << (int) mem[source] << endl;
                    if (controlDebug) cout << __LINE__ << ": Stage: SH" << endl;

                    break;
                case SW:
//                    if(tmp.argv[0] == "$v0" && tmp.argv[1] == "-64($fp)") cout << "Debug: sw v0, -64 fp HeapSort" << endl;
//                    if(tmp.argv[0] == "$v0" && tmp.argv[1] == "-60($fp)") cout << "Debug: sw v0, -60 fp makeHeap" << endl;
//                    if(tmp.argv[0] == "$v0" && tmp.argv[1] == "-92($fp)") cout << "Debug: sw v0, -92 fp adjustHeap" << endl;
                    if (!tmp.BLRdest && a[1] == 0x3f3f3f3f) {
                        arga1++;
                        haveBracket = haveBrackets(s[1]);
                        if (haveBracket) {//如果含有括号
                            right = s[1];
                            left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            offset = string2int(left);
//                        offset /= 4;
                        }
                        if (haveBracket && right[0] == '$') a[1] = regNum[Parser.registerMap[right]].s;
//                    cout << "a[1] " << a[1] << " " << Parser.registerMap[right] << endl;
                        source = (haveAlpha(s[1]) ? Parser.labelMap[s[1]] : a[1]) + offset;
                    } else {
                        source = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s) + tmp.offset;
                    }
                    mem[source] = regNum[a[0]].core.u1;
                    mem[source + 1] = regNum[a[0]].core.u2;
                    mem[source + 2] = regNum[a[0]].core.u3;
                    mem[source + 3] = regNum[a[0]].core.u4;
                    if (controlDebug)
                        cout << endl << "Stage SW: " << source << " |In Memory: " << (int) mem[source] << endl;
                    if (controlDebug) cout << __LINE__ << ": Stage: SW" << endl;

                    break;
                case MOVE:

                    if (controlDebug)
                        cout << __LINE__ << ": Stage: MOVE[From" << a[0] << "] [To" << a[1] << "]" << endl;
                    regNum[a[0]].s = regNum[tmp.Rdest].s;
                    break;
                case MFHI:
                    regNum[a[0]].s = regNum[HIREGISTER].s;
                    if (controlDebug) cout << __LINE__ << ": Stage: MHFI" << endl;

                    break;
                case MFLO:
                    regNum[a[0]].s = regNum[LOREGISTER].s;
                    if (controlDebug) cout << __LINE__ << ": Stage: MFLO" << endl;

                    break;
                case NOP:
                    if (controlDebug) cout << __LINE__ << ": Stage: NOP" << endl;

                    break;
                case SYSCALL:
                    int optionCode = regNum[2].s;
                    int i;
                    int start;
                    int a;
                    int t;
                    string str;
                    switch (optionCode) {
                        case 1:
                            cout << regNum[4].s;
                            break;
                        case 4:
                            start = regNum[4].s;
                            for (i = start;; ++i) {
                                if (mem[i] == 0) break;
                                cout << (char) mem[i];
                            }
                            cout << "\0";
                            cout << flush;
                            break;
                        case 5:
                            cin >> t;
//                            t = 9987;
                            regNum[2].s = t;
                            break;
                        case 8:
                            cin >> str;
                            for (i = 0; i < str.length(); ++i) {
                                mem[regNum[4].us + i] = (unsigned char) str[i];
                            }
                            mem[regNum[4].us + str.length()] = 0;
                            regNum[5].us = str.length() + 1;
                            break;
                        case 9:
                            a = regNum[4].s;
                            regNum[2].s = memHead + 1;
                            memHead += a;
                            break;
                        case 10:
                            exit(0);
//                            return;
                        case 17:
                            exit(regNum[4].s);
//                            return;
                    }
                    if (controlDebug) cout << __LINE__ << ": Stage: SYSCALL" << endl;
                    break;
            }
            if (!jumpFlag) current++;
        }
    }


    void run() {
//        instructionFetch();
//        instructionDecode();
//        execution();
//        memoryAccess();
//        writeBack();
//        directlyRun_DR();
        directlyRun_DR_New();
    }


};

mips Kernel = mips();

#endif //MIPS_MIPS_MAIN_H
