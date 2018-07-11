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

using namespace std;

static int arga1 = 0;
static int arga2 = 0;

class mips {
private:
    int stackTop = memoryMAX - 1;

    int mainEntryPoint; //the Main Entry Point

    string coreData;

    vector<executionInstructionNew> programSentenceNew;

    vector<executionInstructionNewPipeLine *> programSentenceNewPR;

    vector<executionInstructionNewPipeLine2_0 *> programSentenceNewPR_2;

    executionInstructionNewPipeLine *running[5] = {nullptr};

    executionInstructionNewPipeLine2_0 *runningNew[4] = {nullptr};
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
        programSentenceNewPR.reserve(10000);
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

            if (programSentenceNew[i].argv[0][0] == '$') programSentenceNew[i].f[0] = true;
            else programSentenceNew[i].f[0] = false;

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
            if (programSentenceNew[i].argv[1][0] == '$') programSentenceNew[i].f[1] = true;
            else programSentenceNew[i].f[1] = false;

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
            if (programSentenceNew[i].argv[2][0] == '$') programSentenceNew[i].f[2] = true;
            else programSentenceNew[i].f[2] = false;
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
        controlDebug = true;
        while (current < programSentenceNew.size()) {
            //if (controlDebug) {
            //    printf("Debug: ClockTime:%lld\n", clockT);
            //}

            executionInstructionNew &tmp = programSentenceNew[current];
            //printf("Register 2[%d], currentLine[%d]\n", regNum[2], current);
            //if (controlDebug) cout << current << "Running:";
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
                    //if (controlDebug) cout << __LINE__ <<": Stage: ADD" << endl;
                    break;
                case ADDU:
                    regNum[tmp.RSrc].us =
                            (tmp.f[2] ? regNum[tmp.Src].us : (unsigned int) tmp.Src) + regNum[tmp.Rdest].us;
                    //if (controlDebug) cout << __LINE__ <<": Stage: ADDU" << endl;
                    break;
                case ADDIU:
                    regNum[a[0]].us = (tmp.f[2] ? regNum[tmp.Src].us : (unsigned int) tmp.Src) + regNum[tmp.Rdest].us;
                    //if (controlDebug) cout << __LINE__ <<": Stage: ADDIU" << endl;
                    break;
                case SUB:
                    regNum[a[0]].s = regNum[tmp.Rdest].s - (f[2] ? regNum[tmp.Src].s : tmp.Src);
                    //if (controlDebug) cout << __LINE__ <<": Stage: SUB" << endl;
                    break;
                case SUBU:
                    regNum[a[0]].us = regNum[tmp.Rdest].us - (f[2] ? regNum[tmp.Src].us : (unsigned int) tmp.Src);
                    //if (controlDebug) cout << __LINE__ <<": Stage: SUBU" << endl;

                    break;
                case MUL:
                    if (tmp.argv.size() == 2) {
                        tmp2 = (long long) (f[1] ? regNum[tmp.Rdest].s : a[1]) * (long long) regNum[a[0]].s;
                        regNum[HIREGISTER] = _WORD(tmp2.core.u1, tmp2.core.u2, tmp2.core.u3, tmp2.core.u4);
                        regNum[LOREGISTER] = _WORD(tmp2.core.u5, tmp2.core.u6, tmp2.core.u7, tmp2.core.u8);
                    } else regNum[a[0]].s = (tmp.argv[2][0] == '$' ? regNum[tmp.Src].s : tmp.Src) * regNum[tmp.Rdest].s;
                    //if (controlDebug) cout << __LINE__ <<": Stage: MUL" << endl;

                    break;
                case MULU:
                    if (tmp.argv.size() == 2) {
                        tmp2 = (unsigned long long) (f[1] ? regNum[tmp.Rdest].us : a[1]) *
                               (unsigned long long) regNum[a[0]].us;
                        regNum[HIREGISTER] = _WORD(tmp2.core.u1, tmp2.core.u2, tmp2.core.u3, tmp2.core.u4);
                        regNum[LOREGISTER] = _WORD(tmp2.core.u5, tmp2.core.u6, tmp2.core.u7, tmp2.core.u8);
                    } else regNum[a[0]].us = (f[2] ? regNum[tmp.Src].us : a[2]) * regNum[tmp.Rdest].us;
                    //if (controlDebug) cout << __LINE__ <<": Stage: MULU" << endl;

                    break;
                case DIVU:
                    if (tmp.argv.size() == 2) {
                        regNum[HIREGISTER] = _WORD(regNum[a[0]].us % (f[1] ? regNum[tmp.Rdest].us : a[1]));
                        regNum[LOREGISTER] = _WORD(regNum[a[0]].us / (f[1] ? regNum[tmp.Rdest].us : a[1]));
                    } else regNum[a[0]].us = regNum[tmp.Rdest].us / (f[2] ? regNum[tmp.Src].us : a[2]);
                    //if (controlDebug) cout << __LINE__ <<": Stage: DIVU" << endl;

                    break;
                case DIV:
                    if (tmp.argv.size() == 2) {
                        regNum[HIREGISTER] = _WORD(regNum[a[0]].s % (f[1] ? regNum[tmp.Rdest].s : a[1]));
                        regNum[LOREGISTER] = _WORD(regNum[a[0]].s / (f[1] ? regNum[tmp.Rdest].s : a[1]));
                    } else regNum[a[0]].us = regNum[tmp.Rdest].us / (f[2] ? regNum[tmp.Src].us : a[2]);
                    //if (controlDebug) cout << __LINE__ <<": Stage: DIV" << endl;

                    break;
                case XOR:
                    regNum[a[0]].s = regNum[tmp.Rdest].s ^ (f[2] ? regNum[tmp.Src].s : a[2]);
                    //if (controlDebug) cout << __LINE__ <<": Stage: XOR" << endl;

                    break;
                case XORU:
                    regNum[a[0]].us = regNum[tmp.Rdest].us ^ (f[2] ? regNum[tmp.Src].us : a[2]);
                    //if (controlDebug) cout << __LINE__ <<": Stage: XORU" << endl;

                    break;
                case NEG:
                    regNum[a[0]].s = -regNum[tmp.Rdest].s;
                    //if (controlDebug) cout << __LINE__ <<": Stage: NEG" << endl;

                    break;
                case NEGU:
                    regNum[a[0]].us = ~regNum[tmp.Rdest].us;
                    //if (controlDebug) cout << __LINE__ <<": Stage: NEGU" << endl;

                    break;
                case REM:
                    regNum[a[0]].s = regNum[tmp.Rdest].s % (f[2] ? regNum[tmp.Src].s : a[2]);
                    //if (controlDebug) cout << __LINE__ <<": Stage: REM" << endl;

                    break;
                case REMU:
                    regNum[a[0]].us = regNum[tmp.Rdest].us % (f[2] ? regNum[tmp.Src].us : a[2]);
                    //if (controlDebug) cout << __LINE__ <<": Stage: REMU" << endl;

                    break;
                case LI:
                    regNum[a[0]] = _WORD(a[1]);
                    //if (controlDebug) cout << __LINE__ <<": Stage: LI" << endl;

                    break;
                case SEQ:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s == (f[2] ? regNum[tmp.Src].s : a[2]));
                    //if (controlDebug) cout << __LINE__ <<": Stage: SEQ" << endl;

                    break;
                case SGE:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s >= (f[2] ? regNum[tmp.Src].s : a[2]));
                    //if (controlDebug) cout << __LINE__ <<": Stage: SGE" << endl;

                    break;
                case SGT:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s > (f[2] ? regNum[tmp.Src].s : a[2]));
                    //if (controlDebug) cout << __LINE__ <<": Stage: SGT" << endl;

                    break;
                case SLE:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s <= (f[2] ? regNum[tmp.Src].s : a[2]));
                    //if (controlDebug) cout << __LINE__ <<": Stage: SLE" << endl;

                    break;
                case SLT:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s < (f[2] ? regNum[tmp.Src].s : a[2]));
                    //if (controlDebug) cout << __LINE__ <<": Stage: SLT" << endl;

                    break;
                case SNE:
                    regNum[a[0]].s = (regNum[tmp.Rdest].s != (f[2] ? regNum[tmp.Src].s : a[2]));
                    //if (controlDebug) cout << __LINE__ <<": Stage: SNE" << endl;

                    break;
                case J:
                case B:
                    nextExe = tmp.ARSrc;
                    current = nextExe;
                    jumpFlag = true;
                    //if (controlDebug) cout << __LINE__ <<": Stage: J/B" << endl;
                    break;
                case BEQ:
                    if (regNum[a[0]].s == (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BEQ" << endl;

                    break;
                case BNE:
                    if (regNum[a[0]].s != (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BNE" << endl;

                    break;
                case BGE:
                    if (regNum[a[0]].s >= (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BGE" << endl;

                    break;
                case BLE:
                    if (regNum[a[0]].s <= (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BLE" << endl;

                    break;
                case BGT:
                    if (regNum[a[0]].s > (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BGT" << endl;

                    break;
                case BLT:
                    if (regNum[a[0]].s < (f[1] ? regNum[tmp.Rdest].s : a[1])) {
                        nextExe = tmp.ASrc;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BLT" << endl;

                    break;
                case BEQZ:
                    if (regNum[a[0]].s == 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BEQZ" << endl;

                    break;
                case BNEZ:
                    if (regNum[a[0]].s != 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BNEZ" << endl;

                    break;
                case BLEZ:
                    if (regNum[a[0]].s <= 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BLEZ" << endl;

                    break;
                case BGEZ:
                    if (regNum[a[0]].s >= 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BGEZ" << endl;

                    break;
                case BGTZ:
                    if (regNum[a[0]].s > 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BGTZ" << endl;

                    break;
                case BLTZ:
                    if (regNum[a[0]].s < 0) {
                        nextExe = tmp.ARdest;
                        current = nextExe;
                        jumpFlag = true;
                    }
                    //if (controlDebug) cout << __LINE__ <<": Stage: BLTZ" << endl;

                    break;
                case JR:
                    current = regNum[a[0]].s;
                    jumpFlag = true;
                    //if (controlDebug) cout << __LINE__ <<": Stage: JR" << endl;

                    break;
                case JAL:
                    nextExe = tmp.ARSrc;

                    jumpFlag = true;
                    regNum[31] = current + 1;
                    current = nextExe;
                    //if (controlDebug) cout << __LINE__ <<": Stage: JAL" << endl;

                    break;
                case JALR:

                    jumpFlag = true;
                    regNum[31] = current + 1;
                    current = regNum[a[0]].s;
                    //if (controlDebug) cout << __LINE__ <<": Stage: JALR" << endl;

                    break;
                case LA:
                    labelAddress = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s);
                    regNum[a[0]] = labelAddress;
                    //if (controlDebug) cout << __LINE__ <<": Stage: LA" << endl;

                    break;
                case LB:
                    source = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s) + tmp.offset;
                    regNum[a[0]] = _WORD(int(mem[source]));
                    //if (controlDebug) cout << __LINE__ <<": Stage: LB" << endl;

                    break;
                case LH:
                    source = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s) + tmp.offset;
                    regNum[a[0]] = _WORD((int) (_HALF(mem[source], mem[source + 1]).s));
                    //if (controlDebug) cout << __LINE__ <<": Stage: LH" << endl;

                    break;
                case LW:
//                    cout << "Debug a[1] " << a[1] << endl;
                    /*//搜索有没有括号
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
                    } else {*/
                    source = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s) + tmp.offset;
                    //}
                    regNum[a[0]] = _WORD(mem[source], mem[source + 1], mem[source + 2], mem[source + 3]);
                    //if (controlDebug) cout << __LINE__ <<": Stage: LW" << endl;

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
                    //if (controlDebug) cout << __LINE__ <<": Stage: SB" << endl;

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
                    //if (controlDebug) cout << __LINE__ <<": Stage: SH" << endl;

                    break;
                case SW:
//                    if(tmp.argv[0] == "$v0" && tmp.argv[1] == "-64($fp)") cout << "Debug: sw v0, -64 fp HeapSort" << endl;
//                    if(tmp.argv[0] == "$v0" && tmp.argv[1] == "-60($fp)") cout << "Debug: sw v0, -60 fp makeHeap" << endl;
//                    if(tmp.argv[0] == "$v0" && tmp.argv[1] == "-92($fp)") cout << "Debug: sw v0, -92 fp adjustHeap" << endl;
                    /*if (!tmp.BLRdest && a[1] == 0x3f3f3f3f) {
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
                    } else {*/
                    source = (tmp.BLRdest ? tmp.LRdest : regNum[a[1]].s) + tmp.offset;
                    //}
                    mem[source] = regNum[a[0]].core.u1;
                    mem[source + 1] = regNum[a[0]].core.u2;
                    mem[source + 2] = regNum[a[0]].core.u3;
                    mem[source + 3] = regNum[a[0]].core.u4;
                    if (controlDebug)
                        cout << endl << "Stage SW: " << source << " |In Memory: " << (int) mem[source] << endl;
                    //if (controlDebug) cout << __LINE__ <<": Stage: SW" << endl;

                    break;
                case MOVE:

                    if (controlDebug)
                        cout << __LINE__ << ": Stage: MOVE[From" << a[0] << "] [To" << a[1] << "]" << endl;
                    regNum[a[0]].s = regNum[tmp.Rdest].s;
                    break;
                case MFHI:
                    regNum[a[0]].s = regNum[HIREGISTER].s;
                    //if (controlDebug) cout << __LINE__ <<": Stage: MHFI" << endl;

                    break;
                case MFLO:
                    regNum[a[0]].s = regNum[LOREGISTER].s;
                    //if (controlDebug) cout << __LINE__ <<": Stage: MFLO" << endl;

                    break;
                case NOP:
                    //if (controlDebug) cout << __LINE__ <<": Stage: NOP" << endl;

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
                            //cout << "\n[Output: ";
                            start = regNum[4].s;
                            for (i = start;; ++i) {
                                if (mem[i] == 0) break;
                                cout << (char) mem[i];
                            }
                            //cout << "\0]\n";
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
                    //if (controlDebug) cout << __LINE__ <<": Stage: SYSCALL" << endl;
                    break;
            }
            if (!jumpFlag) current++;
            clockT++;
            FILE* fp = fopen("resultDR.log", "a+");
            for (int i = 0; i < 35; ++i) {
                fprintf(fp, "register[%d] : %d ", i, regNum[i].s);
                if ((i + 1) % 5 == 0) fprintf(fp, "\n");
            }
            fprintf(fp ,"InlineLine:%d\n\n", current);
            fclose(fp);
        }
    }

    void setInstructionPR(const string &rhs) {
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
                    executionMap[labelName] = programSentenceNewPR.size();
                    if (labelName == "main") mainEntryPoint = programSentenceNewPR.size();
                } else {
                    if (result == ADD || result == ADDU || result == ADDIU || result == SUB ||
                        result == SUBU || result == XOR || result == XORU || result == REM ||
                        result == REMU) {
                        LogAndCal *s1 = new LogAndCal();
                        s1->typeName = type2String(result);
                        s1->type = result;
                        string arg1, arg2, arg3;
                        ss >> arg1 >> arg2 >> arg3;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        deleteCertainChar(arg3, ',');
                        s1->argc = 3;

                        s1->argv.push_back(arg1);
                        s1->argv.push_back(arg2);
                        s1->argv.push_back(arg3);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->Rdest = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                        }
                        if (arg3[0] == '$') {
                            s1->Src = Parser.registerMap[arg3];
                        } else if (allNumber(arg3)) {
                            s1->Src = string2int(arg3);
                        }

                        programSentenceNewPR.push_back(s1);
                    }

                    if (result == SEQ || result == SGE || result == SGT ||
                        result == SLE || result == SLT || result == SNE) {
                        Compare *s1 = new Compare();
                        s1->typeName = type2String(result);

                        s1->type = result;
                        string arg1, arg2, arg3;
                        ss >> arg1 >> arg2 >> arg3;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        deleteCertainChar(arg3, ',');
                        s1->argc = 3;

                        s1->argv.push_back(arg1);
                        s1->argv.push_back(arg2);
                        s1->argv.push_back(arg3);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1->Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                        }
                        if (arg3[0] == '$') {
                            s1->Src = Parser.registerMap[arg3];
                        } else if (allNumber(arg3)) {
                            s1->Src = string2int(arg3);
                        }

                        programSentenceNewPR.push_back(s1);
                    }
                    if (result == BEQ ||
                        result == BNE || result == BGE || result == BLE || result == BGT ||
                        result == BLT) {
                        IfAndJump *s1 = new IfAndJump();
                        s1->type = result;
                        s1->typeName = type2String(result);

                        string arg1, arg2, arg3;
                        ss >> arg1 >> arg2 >> arg3;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        deleteCertainChar(arg3, ',');
                        s1->argc = 3;

                        s1->argv.push_back(arg1);
                        s1->argv.push_back(arg2);
                        s1->argv.push_back(arg3);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1->Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                        } else if (allNumber(arg2)) {
                            s1->Rdest = string2int(arg2);
                        }
                        if (arg3[0] == '$') {
                            s1->Src = Parser.registerMap[arg3];
                        }

                        programSentenceNewPR.push_back(s1);

                    }
                    if (result == LI) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        Li *s1 = new Li();
                        s1->type = result;
                        s1->argc = 2;
                        s1->typeName = type2String(result);

                        s1->argv.push_back(arg1);
                        s1->argv.push_back(arg2);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1->Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                            s1->Rdest = Parser.registerMap[right];
                        } else if (allNumber(arg2)) {
                            s1->Rdest = string2int(arg2);
                        }

                        programSentenceNewPR.push_back(s1);
                    }
                    if (result == BEQZ || result == BNEZ || result == BLEZ || result == BGEZ ||
                        result == BGTZ || result == BLTZ) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        IfAndJump *s1 = new IfAndJump();
                        s1->argc = 2;
                        s1->type = result;
                        s1->typeName = type2String(result);

                        s1->argv.push_back(arg1);
                        s1->argv.push_back(arg2);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1->Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                            s1->Rdest = Parser.registerMap[right];
                        } else if (allNumber(arg2)) {
                            s1->Rdest = string2int(arg2);
                        }

                        programSentenceNewPR.push_back(s1);
                    }
                    if (result == LA || result == LB || result == LH || result == LW) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        LoadData *s1 = new LoadData();
                        s1->argc = 2;
                        s1->type = result;
                        s1->argv.push_back(arg1);
                        s1->typeName = type2String(result);

                        s1->argv.push_back(arg2);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1->Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                            s1->Rdest = Parser.registerMap[right];
                        } else if (allNumber(arg2)) {
                            s1->Rdest = string2int(arg2);
                        }

                        programSentenceNewPR.push_back(s1);
                    }

                    if (result == SB || result == SH || result == SW) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        StoreData *s1 = new StoreData();
                        s1->argc = 2;
                        s1->type = result;
                        s1->typeName = type2String(result);

                        s1->argv.push_back(arg1);
                        s1->argv.push_back(arg2);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1->Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                            s1->Rdest = Parser.registerMap[right];
                        } else if (allNumber(arg2)) {
                            s1->Rdest = string2int(arg2);
                        }

                        programSentenceNewPR.push_back(s1);
                    }
                    if (result == MOVE) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        MoveData *s1 = new MoveData();
                        s1->argc = 2;
                        s1->type = result;
                        s1->typeName = type2String(result);

                        s1->argv.push_back(arg1);
                        s1->argv.push_back(arg2);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1->Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                            s1->Rdest = Parser.registerMap[right];
                        } else if (allNumber(arg2)) {
                            s1->Rdest = string2int(arg2);
                        }

                        programSentenceNewPR.push_back(s1);
                    }


                    if (result == NEG || result == NEGU) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        LogAndCal *s1 = new LogAndCal();
                        s1->argc = 2;
                        s1->type = result;
                        s1->argv.push_back(arg1);
                        s1->typeName = type2String(result);

                        s1->argv.push_back(arg2);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg1];
                        }
                        if (arg2[0] == '$') {
                            s1->Rdest = Parser.registerMap[arg2];
                        } else if (haveBrackets(arg2)) {
                            string right = arg2;
                            string left = splitWithCertainChar(right, '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                            s1->Rdest = Parser.registerMap[right];
                        } else if (allNumber(arg2)) {
                            s1->Rdest = string2int(arg2);
                        }

                        programSentenceNewPR.push_back(s1);
                    }

                    if (result == MFHI || result == MFLO) {
                        string arg1;
                        ss >> arg1;
                        deleteCertainChar(arg1, ',');
                        MoveData *s1 = new MoveData();
                        s1->argc = 1;
                        s1->typeName = type2String(result);
                        s1->type = result;
                        s1->argv.push_back(arg1);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg1];
                        }
                        programSentenceNewPR.push_back(s1);
                    }
                    if (result == B || result == J || result == JR || result == JAL || result == JALR) {
                        string arg1;
                        ss >> arg1;
                        deleteCertainChar(arg1, ',');
                        IfAndJump *s1 = new IfAndJump();
                        s1->argc = 1;
                        s1->typeName = type2String(result);
                        s1->type = result;
                        s1->argv.push_back(arg1);
                        //解析寄存器对象
                        if (arg1[0] == '$') {
                            s1->RSrc = Parser.registerMap[arg1];
                        }
                        programSentenceNewPR.push_back(s1);
                    }
                    if (result == NOP) {
                        Special *s1 = new Special();
                        s1->typeName = type2String(result);

                        s1->type = NOP;
                        programSentenceNewPR.push_back(s1);
                    }
                    if (result == SYSCALL) {
                        Special *s1 = new Special();
                        s1->typeName = type2String(result);

                        s1->type = SYSCALL;
                        programSentenceNewPR.push_back(s1);
                    }
                    if (result == MUL || result == MULU || result == DIV || result == DIVU) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        LogAndCal *s1 = new LogAndCal();
                        int argc = 2;
                        s1->type = result;
                        s1->argv.push_back(arg1);
                        s1->typeName = type2String(result);

                        s1->argv.push_back(arg2);
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
                            s1->argv.push_back(s3);
                            arg3 = s3;
                            argc = 3;
                        }

                        if (argc == 2) {
                            //解析寄存器对象
                            if (arg1[0] == '$') {
                                s1->RSrc = Parser.registerMap[arg1];
                            }
                            if (arg2[0] == '$') {
                                s1->Rdest = Parser.registerMap[arg2];
                            } else if (haveBrackets(arg2)) {
                                string right = arg2;
                                string left = splitWithCertainChar(right, '(');
                                deleteCertainChar(right, ')');
                                int offset = string2int(left);
                                s1->offset = offset;
                                s1->Rdest = Parser.registerMap[right];
                            }
                        } else {
                            if (arg1[0] == '$') {
                                s1->RSrc = Parser.registerMap[arg1];
                            }
                            if (arg2[0] == '$') {
                                s1->Rdest = Parser.registerMap[arg2];
                            } else if (haveBrackets(arg2)) {
                                string right = arg2;
                                string left = splitWithCertainChar(right, '(');
                                deleteCertainChar(right, ')');
                                int offset = string2int(left);
                                s1->offset = offset;
                                s1->Rdest = Parser.registerMap[right];
                            }
                            if (arg3[0] == '$') {
                                s1->Src = Parser.registerMap[arg3];
                            } else if (allNumber(arg3)) {
                                s1->Src = string2int(arg3);
                            }
                        }
                        s1->argc = argc;
                        programSentenceNewPR.push_back(s1);
                    }
                }

            }
        }
        int sizeT = (int) programSentenceNewPR.size();
        for (int i = 0; i < sizeT; ++i) {
            executionInstructionNewPipeLine *newPipeLine = programSentenceNewPR[i];
            newPipeLine->index = i;
            if (newPipeLine->argc == 0) continue;
            if (newPipeLine->argv[0] != "") {
                int Addr = checkLabel(newPipeLine->argv[0]);
//                if(Addr == -1) throw 1; // Error: Not found the label;
                newPipeLine->ARSrc = (Addr == -1 ? 0x3f3f3f3f : Addr);
            }
            if (newPipeLine->argv[0] != "") {
                int Addr = Parser.LabelAddr(newPipeLine->argv[0]);
                newPipeLine->LRSrc = (Addr == -1 ? 0x3f3f3f3f : Addr);
                newPipeLine->BLRSrc = Addr != -1;
            }

            newPipeLine->f[0] = newPipeLine->argv[0][0] == '$';

            if (newPipeLine->argc == 1) continue;
            if (newPipeLine->argv[1] != "") {
                int Addr = checkLabel(newPipeLine->argv[1]);
//                if(Addr == -1) throw 1; // Error: Not found the label;
                newPipeLine->ARdest = (Addr == -1 ? 0x3f3f3f3f : Addr);
            }
            if (newPipeLine->argv[1] != "") {
                int Addr = Parser.LabelAddr(newPipeLine->argv[1]);
                newPipeLine->LRdest = (Addr == -1 ? 0x3f3f3f3f : Addr);
                newPipeLine->BLRdest = Addr != -1;
            }
            newPipeLine->f[1] = newPipeLine->argv[1][0] == '$';

            if (newPipeLine->argc == 2) continue;
            if (newPipeLine->argv[2] != "") {
                int Addr = checkLabel(newPipeLine->argv[2]);
//                if(Addr == -1) throw 1; // Error: Not found the label;
                newPipeLine->ASrc = (Addr == -1 ? 0x3f3f3f3f : Addr);
            }
            if (newPipeLine->argv[2] != "") {
                int Addr = Parser.LabelAddr(newPipeLine->argv[2]);
                newPipeLine->LSrc = (Addr == -1 ? 0x3f3f3f3f : Addr);
                newPipeLine->BLSrc = Addr != -1;
            }
            newPipeLine->f[2] = newPipeLine->argv[2][0] == '$';
        }

        //cout << "Parser \n";
    }

    void setInstructionPR_New(const string &rhs) {
        coreData = rhs;
        stringstream ss(rhs);

        string token;
        bool dataField = false;
        bool textField = false;

        string lastLabel = "";
        string nextToken = "";
        bool haveLabel = false;
        int currentSentence = 0;

        while (haveLabel || ss >> token) {
            if (haveLabel) token = nextToken;
            if (token == ".data") {
                dataField = true;
                textField = false;
                haveLabel = false;
                continue;
            }
            if (token == ".text") {
                dataField = false;
                textField = true;
                haveLabel = false;
                continue;
            }

            // 数据处理
            if (dataField) {
                Instruction result = Parser.Type(token);
                if (controlDebug) {
                    string str("Found Token [");
                    str += (int) result;
                    str += "]";
                    debugMess(str, "Parser - Data Field");
                }

                string next, messageDebug;

                switch (result) {
                    case ALIGN: {
                        int n;
                        ss >> n;
                        if ((memHead + 1) % (1 << n) != 0) memHead = (memHead / (1 << n) + 1) * (1 << n) - 1;
                        break;
                    }
                    case ASCIIZ: {
                        char ch;
                        ch = ss.get();
                        ch = ss.get();
                        while (ch != '\n') {
                            next += ch;
                            ch = ss.get();
                        }
                        deleteCertainChar(next, '\"');
                        next = decodeSpecial(next);
                        for (int i = 0; i < next.length(); ++i) mem[memHead++] = (unsigned char) next[i];
                        mem[memHead++] = 0;
                        break;
                    }
                    case ASCII: {
                        char ch;
                        ch = ss.get();
                        ch = ss.get();
                        while (ch != '\n') {
                            next += ch;
                            ch = ss.get();
                        }
                        deleteCertainChar(next, '\"');
                        next = decodeSpecial(next);
                        for (int i = 0; i < next.length(); ++i) mem[memHead++] = (unsigned char) next[i];
                        break;
                    }
                    case BYTE: {
                        char ch;
                        ch = ss.get();
                        while (ch != '\n') {
                            next += ch;
                            ch = ss.get();
                        }
                        deleteCertainChar(next, '\"');
                        deleteCertainChar(next, ',');
                        stringstream byteNumber(next);
                        string str;
                        while (byteNumber >> str) mem[memHead++] = (unsigned char) string2int(str);
                        break;
                    }
                    case HALF:{
                        char ch;
                        ch = ss.get();
                        while (ch != '\n') {
                            next += ch;
                            ch = ss.get();
                        }
                        deleteCertainChar(next, '\"');
                        deleteCertainChar(next, ',');
                        stringstream byteNumber(next);
                        string str;
                        while (byteNumber >> str) {
                            _HALF u = _HALF((short) string2int(str));
                            mem[memHead++] = u.core.u1;
                            mem[memHead++] = u.core.u2;
                        }
                        break;
                    }
                    case WORD:{
                        char ch;
                        ch = ss.get();
                        while (ch != '\n') {
                            next += ch;
                            ch = ss.get();
                        }
                        deleteCertainChar(next, '\"');
                        deleteCertainChar(next, ',');
                        stringstream byteNumber(next);
                        string str;
                        while (byteNumber >> str) {
                            _WORD u = _WORD((short) string2int(str));
                            mem[memHead++] = u.core.u1;
                            mem[memHead++] = u.core.u2;
                            mem[memHead++] = u.core.u3;
                            mem[memHead++] = u.core.u4;
                        }
                        break;
                    }
                    case SPACE:{
                        int n1;
                        ss >> n1;
                        memHead += n1;
                        break;
                    }
                    case LABEL:{
                        deleteCertainChar(token, ':');
                        Parser.labelMap[token] = memHead;
                        break;
                    }
                }
            // 开始指令集
            } else {
                Instruction result = Parser.Type(token);
                switch (result){
                    case LABEL:{
                        deleteCertainChar(token, ':');
                        string labelName = token;
                        executionMap[labelName] = programSentenceNewPR_2.size();
                        if(labelName == "main") mainEntryPoint = programSentenceNewPR_2.size();
                        break;
                    }
                    case ADD:
                    case ADDU:
                    case ADDIU:
                    case SUB:
                    case SUBU:
                    case XOR:
                    case XORU:
                    case REM:
                    case REMU:{
                        LogicalAndCalculate* s1 = new LogicalAndCalculate();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        string arg[3];
                        for(int i = 0; i < 3; ++i) {
                            ss >> arg[i];
                            deleteCertainChar(arg[i], ',');
                            s1->argv.push_back(arg[i]);
                        }
                        s1->argc = 3;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                        s1->para[1].setData(REGISTER, Parser.registerMap[s1->argv[1]]);
                        if(s1->argv[2][0] == '$') s1->para[2].setData(REGISTER, Parser.registerMap[s1->argv[2]]);
                        else s1->para[2].setData(IMM, string2int(s1->argv[2]));

                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case MUL:
                    case MULU:
                    case DIV:
                    case DIVU:{
                        string tmp = "";
                        char ch;
                        int comma = 0;
                        ch = ss.get();
                        while(ch != '\n'){
                            tmp += ch;
                            ch = ss.get();
                            if(ch == ',') comma++;
                        }

                        LogicalAndCalculate* s1 = new LogicalAndCalculate();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        s1->argc = comma + 1;
#ifdef ALL_DEBUG
                        cout << "Parser : Comma(In Mul, Mulu, Div, Divu Stage):" << comma;
#endif
                        string arg[s1->argc];
                        if(comma == 1){
                            stringstream ss(tmp);
                            for(int i = 0; i < 2; ++i) {
                                ss >> arg[i];
                                deleteCertainChar(arg[i], ',');
                                s1->argv.push_back(arg[i]);
                            }
                            s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                            if(s1->argv[1][0] == '$') s1->para[1].setData(REGISTER, Parser.registerMap[s1->argv[1]]);
                            else s1->para[1].setData(IMM, string2int(s1->argv[1]));
                        } else {
                            stringstream ss(tmp);
                            for(int i = 0; i < 3; ++i) {
                                ss >> arg[i];
                                deleteCertainChar(arg[i], ',');
                                s1->argv.push_back(arg[i]);
                            }
                            s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                            s1->para[1].setData(REGISTER, Parser.registerMap[s1->argv[1]]);
                            if(s1->argv[2][0] == '$') s1->para[2].setData(REGISTER, Parser.registerMap[s1->argv[2]]);
                            else s1->para[2].setData(IMM, string2int(s1->argv[2]));

                        }
                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case NEG:
                    case NEGU:{
                        LogicalAndCalculate* s1 = new LogicalAndCalculate();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        string arg[2];
                        for(int i = 0; i < 2; ++i) {
                            ss >> arg[i];
                            deleteCertainChar(arg[i], ',');
                            s1->argv.push_back(arg[i]);
                        }
                        s1->argc = 2;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                        s1->para[1].setData(REGISTER, Parser.registerMap[s1->argv[1]]);

                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case LI:{
                        LiOpt* s1 = new LiOpt();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        string arg[2];
                        for(int i = 0; i < 2; ++i) {
                            ss >> arg[i];
                            deleteCertainChar(arg[i], ',');
                            s1->argv.push_back(arg[i]);
                        }
                        s1->argc = 2;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                        s1->para[1].setData(IMM, string2int(s1->argv[1]));
                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case SEQ:
                    case SGE:
                    case SGT:
                    case SLE:
                    case SLT:
                    case SNE:{
                        CompareOpt* s1 = new CompareOpt();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        string arg[3];
                        for(int i = 0; i < 3; ++i) {
                            ss >> arg[i];
                            deleteCertainChar(arg[i], ',');
                            s1->argv.push_back(arg[i]);
                        }
                        s1->argc = 3;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                        s1->para[1].setData(REGISTER, Parser.registerMap[s1->argv[1]]);
                        if(s1->argv[2][0] == '$') s1->para[2].setData(REGISTER, Parser.registerMap[s1->argv[2]]);
                        else s1->para[2].setData(IMM, string2int(s1->argv[2]));

                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case JAL:
                    case B:
                    case J:{
                        IfAndJumpOpt* s1 = new IfAndJumpOpt();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        string s;
                        ss >> s;
                        deleteCertainChar(s, ',');
                        s1->argv.push_back(s);
                        s1->argc = 1;

                        s1->para[0].setData(ADDRESS_FUNCTION, -1);

                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case JR:
                    case JALR:{
                        IfAndJumpOpt* s1 = new IfAndJumpOpt();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        string s;
                        ss >> s;
                        deleteCertainChar(s, ',');
                        s1->argv.push_back(s);
                        s1->argc = 1;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s]);

                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case BEQ:
                    case BNE:
                    case BGE:
                    case BLE:
                    case BGT:
                    case BLT:{
                        IfAndJumpOpt* s1 = new IfAndJumpOpt();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        string arg[3];
                        for(int i = 0; i < 3; ++i) {
                            ss >> arg[i];
                            deleteCertainChar(arg[i], ',');
                            s1->argv.push_back(arg[i]);
                        }
                        s1->argc = 3;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                        if(s1->argv[1][0] == '$') s1->para[1].setData(REGISTER, Parser.registerMap[s1->argv[1]]);
                        else s1->para[1].setData(IMM, string2int(s1->argv[1]));
                        s1->para[2].setData(ADDRESS_FUNCTION, -1);

                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case BEQZ:
                    case BNEZ:
                    case BLEZ:
                    case BGEZ:
                    case BGTZ:
                    case BLTZ:{
                        IfAndJumpOpt* s1 = new IfAndJumpOpt();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        string arg[2];
                        for(int i = 0; i < 2; ++i) {
                            ss >> arg[i];
                            deleteCertainChar(arg[i], ',');
                            s1->argv.push_back(arg[i]);
                        }
                        s1->argc = 2;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                        s1->para[1].setData(ADDRESS_FUNCTION, -0x3f3f3f3f);
                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case LA:
                    case LB:
                    case LH:
                    case LW:{
                        LoadDataOpt* s1 = new LoadDataOpt();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        string arg[2];
                        for(int i = 0; i < 2; ++i) {
                            ss >> arg[i];
                            deleteCertainChar(arg[i], ',');
                            s1->argv.push_back(arg[i]);
                        }
                        s1->argc = 2;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                        if(haveBrackets(s1->argv[1])){
                            string right = s1->argv[1];
                            string left = splitWithCertainChar(s1->argv[1], '(');
                            deleteCertainChar(right, ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                            deleteCertainChar(s1->argv[1], ')');
                            s1->para[1].setData(REGISTER, Parser.registerMap[s1->argv[1]]);
                        } else {
                            s1->para[1].setData(ADDRESS_LABEL, -1);
                        }
                        
                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case SB:
                    case SW:
                    case SH:{
                        StoreDataOpt* s1 = new StoreDataOpt();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        string arg[2];
                        for(int i = 0; i < 2; ++i) {
                            ss >> arg[i];
                            deleteCertainChar(arg[i], ',');
                            s1->argv.push_back(arg[i]);
                        }
                        s1->argc = 2;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                        if(haveBrackets(s1->argv[1])){
                            string right = s1->argv[1];
                            string left = splitWithCertainChar(s1->argv[1], '(');
                            deleteCertainChar(s1->argv[1], ')');
                            int offset = string2int(left);
                            s1->offset = offset;
                            s1->para[1].setData(REGISTER, Parser.registerMap[s1->argv[1]]);
                        } else {
                            s1->para[1].setData(ADDRESS_LABEL, -1);
                        }

                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case MOVE:{
                        MoveDataOpt* s1 = new MoveDataOpt();
                        
                        s1->type = result;
                        s1->typeName = type2String(result);

                        string arg[2];
                        for(int i = 0; i < 2; ++i) {
                            ss >> arg[i];
                            deleteCertainChar(arg[i], ',');
                            s1->argv.push_back(arg[i]);
                        }
                        s1->argc = 2;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);
                        s1->para[1].setData(REGISTER, Parser.registerMap[s1->argv[1]]);

                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case MFLO:
                    case MFHI:{
                        MoveDataOpt* s1 = new MoveDataOpt();

                        s1->type = result;
                        s1->typeName = type2String(result);

                        string arg[1];
                        for(int i = 0; i < 1; ++i) {
                            ss >> arg[i];
                            deleteCertainChar(arg[i], ',');
                            s1->argv.push_back(arg[i]);
                        }
                        s1->argc = 1;

                        s1->para[0].setData(REGISTER, Parser.registerMap[s1->argv[0]]);

                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case NOP:{
                        NopOpt* s1 = new NopOpt();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        
                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                    case SYSCALL:{
                        SpecialOpt *s1 = new SpecialOpt();
                        s1->type = result;
                        s1->typeName = type2String(result);
                        
                        programSentenceNewPR_2.push_back(s1);
                        break;
                    }
                }
            }
        }

        int sizeT = programSentenceNewPR_2.size();
        for(int i = 0; i < sizeT; ++i){
            executionInstructionNewPipeLine2_0* tmp = programSentenceNewPR_2[i];
            tmp->index = i;
            if(tmp->argc == 0) continue;
            if(tmp->para[0].type == ADDRESS_LABEL) tmp->para[0].arg = Parser.labelMap[tmp->argv[0]];
            if(tmp->para[0].type == ADDRESS_FUNCTION) tmp->para[0].arg = executionMap[tmp->argv[0]];

            if(tmp->argc == 1) continue;
            if(tmp->para[1].type == ADDRESS_LABEL) tmp->para[1].arg = Parser.labelMap[tmp->argv[1]];
            if(tmp->para[1].type == ADDRESS_FUNCTION) tmp->para[1].arg = executionMap[tmp->argv[1]];

            if(tmp->argc == 2) continue;
            if(tmp->para[2].type == ADDRESS_LABEL) tmp->para[2].arg = Parser.labelMap[tmp->argv[2]];
            if(tmp->para[2].type == ADDRESS_FUNCTION) tmp->para[2].arg = executionMap[tmp->argv[2]];
        }
    }

    inline executionInstructionNewPipeLine *getNextSentence() {
//        if (lockFlag) { return nullptr; }
        if (haveJump) {
            currentLine = nextLine;
            haveJump = false;
        } else currentLine++;
        return programSentenceNewPR[currentLine];
    }

    inline executionInstructionNewPipeLine2_0* getNextSentence2(){
        if (haveJump) {
            currentLine = nextLine;
            haveJump = false;
        } else currentLine++;
        return programSentenceNewPR_2[currentLine];
    }

    void __IF__() {
        if (runningNew[0] != nullptr || lockFlag) return;
        runningNew[0] = getNextSentence2();
    }

    void __ID__() {
        if (runningNew[1] != nullptr || runningNew[0] == nullptr) return;
        bool result = runningNew[0]->ID();
        if (!result) return;
        runningNew[1] = runningNew[0];
        runningNew[0] = nullptr;
    }

    void __EX__() {
        if (runningNew[2] != nullptr || runningNew[1] == nullptr) return;
        bool result = runningNew[1]->EX();
        if (!result) return;
        runningNew[2] = runningNew[1];
        runningNew[1] = nullptr;
    }

    void __MA__() {
        if (runningNew[3] != nullptr || runningNew[2] == nullptr) return;
        bool result = runningNew[2]->MEM();
        if (!result) return;
        runningNew[3] = runningNew[2];
        runningNew[2] = nullptr;

    }

    void __WB__() {
        if (runningNew[3] == nullptr) return;
        bool result = runningNew[3]->WB();
        if (!result) return;
        runningNew[3] = nullptr;
        if (controlDebug) {
            for (int i = 0; i < 35; ++i) {
                printf("register[%d] : %d       ", i, regNum[i].s);
                if ((i + 1) % 5 == 0) printf("\n");
            }
            printf("Mem:276[%d]\n", *((int *) (&mem[276])));
            printf("Mem:276[%d]  277[%d] 278[%d] 279[%d]\n", mem[276], mem[277], mem[278], mem[279]);
            printf("\n\n");

            FILE* fp = fopen("resultPipe.log", "a+");
            for (int i = 0; i < 35; ++i) {
                fprintf(fp, "register[%d] : %d ", i, regNum[i].s);
                if ((i + 1) % 5 == 0) fprintf(fp,"\n");
            }
            fprintf(fp ,"InlineLine:%d\n\n", currentLine);
            /*if(running[3] != nullptr)
            fprintf(fp, "Idx:[%d] Command:[%s] argv:[%s] [%s] [%s]\n", runningNew[3]->index, runningNew[3]->typeName.c_str(), (runningNew[3]->argc >= 1 ? runningNew[3]->argv[0].c_str() : ""),
                    (runningNew[3]->argc >= 2 ? runningNew[3]->argv[1].c_str() : ""), (runningNew[3]->argc >= 3 ? runningNew[3]->argv[2].c_str() : ""));
            else fprintf(fp, "\n");*/
            fclose(fp);
        }
    }

    void pipelineRun_PR_Flow() {
        if (controlDebug) {
            map<string, int>::iterator ite;
            cout << "Label Table" << endl;
            for (ite = executionMap.begin(); ite != executionMap.end(); ite++) {
                cout << "Label:" << ite->first << "  Line:" << ite->second << endl;
            }
        }
        //cout << "Debug:\n" << flush;
        if(controlDebug) {
            for(int i = 0; i < 4; ++i)
                if(runningNew[i] == nullptr) continue;
            else runningNew[i]->printP();
        }
        currentLine = mainEntryPoint - 1;
        regNum[29] = stackTop;
        int limitSize = (int) programSentenceNewPR_2.size();
        while (currentLine < limitSize) {
            if(controlDebug) {cout << "Debug:\n" << flush;
                for(int i = 0; i < 4; ++i)
                    if(runningNew[i] == nullptr) continue;
                    else runningNew[i]->printP();
            }

            __WB__();
            __MA__();
            __EX__();
            __ID__();
            __IF__();
            clockT++;
        }
    }

    void pipelineRun_PR_New() {
        currentLine = mainEntryPoint;
        regNum[29] = stackTop;
        nextLine = currentLine + 1;
        haveJump = false;
        int limitSize = (int) programSentenceNewPR.size();
        int RunningStage = 1;
        executionInstructionNewPipeLine *idx[5] = {nullptr};

        lockFlag = false; //Hazard
        while (currentLine < limitSize) {
            currentClock++;
            //Debug Start
            if (controlDebug) {
                printf("Debug:\n");
                for (int i = 0; i < 5 && idx[i] != nullptr; ++i) {
                    printf("Idx:%d ", i);
                    idx[i]->printDebug();
                }
                printf("\n\n");
            }
            //Debug Ended
            for (int index = 0; index < 5; ++index) {
                if (!lockFlag && nextIFThread == index && currentClock == nextClockIF) {
                    // Do IF
                    idx[index] = getNextSentence();
                    idx[index]->runningStage = 1;
                    nextClockIF++;
                    nextIFThread = (nextIFThread + 1) % 5;
                } else if (idx[index] == nullptr) continue;
                else if (idx[index]->runningStage > stageLock) {
                    switch (idx[index]->runningStage) {
                        case 1:
                            idx[index]->ID();
                            break;
                        case 2:
                            idx[index]->EX();
                            break;
                        case 3:
                            idx[index]->MEM();
                            break;
                        case 4:
                            idx[index]->WB();
                            break;
                        case 5:
                            if (!lockFlag && nextIFThread == index && currentClock == nextClockIF) {
                                idx[index] = getNextSentence();
                                idx[index]->runningStage = 1;
                                nextClockIF++;
                                nextIFThread = (nextIFThread + 1) % 5;
                                break;
                            }
                        default:
                            break;
                    }
                }
            }
            //currentClock++;
        }
    }

    void pipelineRun_PR() {
        if (controlDebug) {
            map<string, int>::iterator ite;
            cout << "Label Table" << endl;
            for (ite = executionMap.begin(); ite != executionMap.end(); ite++) {
                cout << "Label:" << ite->first << "  Line:" << ite->second << endl;
            }
        }

        currentLine = mainEntryPoint;
        regNum[29] = stackTop;
        nextLine = currentLine + 1;
        haveJump = false;
        int limitSize = (int) programSentenceNewPR.size();
        int RunningStage = 1;
        executionInstructionNewPipeLine *idx[5] = {nullptr};
        lockFlag = false;//lock when meeting the jump sentence
        while (currentLine < limitSize) {
            if (controlDebug) {
                printf("Debug:\n");
                for (int i = 0; i < 5 && idx[i] != nullptr; ++i) {
                    printf("Idx:%d ", i);
                    idx[i]->printDebug();
                }
                printf("\n\n");
            }
            if (idx[0] == nullptr) {
                idx[0] = programSentenceNewPR[currentLine];
                nextLine = ++currentLine;
            }
            if (currentStage != idx[0]->runningStage && (idx[0]->runningStage == 0 || idx[0]->runningStage > stageLock))
                switch (idx[0]->runningStage) {
                    case 0:
                        idx[0]->runningStage = 1;
                        break;
                    case 1:
                        idx[0]->ID();
                        break;
                    case 2:
                        idx[0]->EX();
                        break;
                    case 3:
                        idx[0]->MEM();
                        break;
                    case 4:
                        idx[0]->WB();
                        break;
                    case 5:
                        if (!lockFlag) {
                            idx[0] = getNextSentence();
                            idx[0]->runningStage = 1;
                            break;
                        }
                    default:
                        break;
                }
            if (stageLock != -1) currentStage = idx[0]->runningStage - 1;
            if (RunningStage == 1) {
                RunningStage++;
                continue;
            }

            if (idx[1] == nullptr) {
                idx[1] = programSentenceNewPR[currentLine];
                nextLine = ++currentLine;
            }

            if (currentStage != idx[1]->runningStage && (idx[1]->runningStage == 0 || idx[1]->runningStage > stageLock))
                switch (idx[1]->runningStage) {
                    case 0:
                        idx[1]->runningStage = 1;
                        break;
                    case 1:
                        idx[1]->ID();
                        break;
                    case 2:
                        idx[1]->EX();
                        break;
                    case 3:
                        idx[1]->MEM();
                        break;
                    case 4:
                        idx[1]->WB();
                        break;
                    case 5:
                        if (!lockFlag) {
                            idx[1] = getNextSentence();
                            idx[1]->runningStage = 1;
                            break;
                        }
                    default:
                        break;
                }
            if (stageLock != -1) currentStage = idx[1]->runningStage - 1;
            if (RunningStage == 2) {
                RunningStage++;
                continue;
            }

            if (idx[2] == nullptr) {
                idx[2] = programSentenceNewPR[currentLine];
                nextLine = ++currentLine;
            }
            if (currentStage != idx[2]->runningStage && (idx[2]->runningStage == 0 || idx[2]->runningStage > stageLock))
                switch (idx[2]->runningStage) {
                    case 0:
                        idx[2]->runningStage = 1;
                        break;
                    case 1:
                        idx[2]->ID();
                        break;
                    case 2:
                        idx[2]->EX();
                        break;
                    case 3:
                        idx[2]->MEM();
                        break;
                    case 4:
                        idx[2]->WB();
                        break;
                    case 5:
                        if (!lockFlag) {
                            idx[2] = getNextSentence();
                            idx[2]->runningStage = 1;
                            break;
                        }
                    default:
                        break;
                }
            if (stageLock != -1) currentStage = idx[2]->runningStage - 1;
            if (RunningStage == 3) {
                RunningStage++;
                continue;
            }

            if (idx[3] == nullptr) {
                idx[3] = programSentenceNewPR[currentLine];
                nextLine = ++currentLine;
            }
            if (currentStage != idx[3]->runningStage && (idx[3]->runningStage == 0 || idx[3]->runningStage > stageLock))
                switch (idx[3]->runningStage) {
                    case 0:
                        idx[3]->runningStage = 1;
                        break;
                    case 1:
                        idx[3]->ID();
                        break;
                    case 2:
                        idx[3]->EX();
                        break;
                    case 3:
                        idx[3]->MEM();
                        break;
                    case 4:
                        idx[3]->WB();
                        break;
                    case 5:
                        if (!lockFlag) {
                            idx[3] = getNextSentence();
                            idx[3]->runningStage = 1;
                            break;
                        }
                    default:
                        break;
                }

            if (RunningStage == 4) {
                RunningStage++;
                continue;
            }
            if (stageLock != -1) currentStage = idx[3]->runningStage - 1;
            if (idx[4] == nullptr) {
                idx[4] = programSentenceNewPR[currentLine];
                nextLine = ++currentLine;
            }
            if (currentStage != idx[4]->runningStage && (idx[4]->runningStage == 0 || idx[4]->runningStage > stageLock))
                switch (idx[4]->runningStage) {
                    case 0:
                        idx[4]->runningStage = 1;
                        break;
                    case 1:
                        idx[4]->ID();
                        break;
                    case 2:
                        idx[4]->EX();
                        break;
                    case 3:
                        idx[4]->MEM();
                        break;
                    case 4:
                        idx[4]->WB();
                        break;
                    case 5:
                        if (!lockFlag) {
                            idx[4] = getNextSentence();
                            idx[4]->runningStage = 1;
                            break;
                        }
                    default:
                        break;
                }
            if (stageLock != -1) currentStage = idx[4]->runningStage - 1;
        }

    }

    void run() {
        directlyRun_DR_New();
    }

    void runPR() {
//        pipelineRun_PR_New();
        pipelineRun_PR_Flow();
    }


};

mips Kernel = mips();

#endif //MIPS_MIPS_MAIN_H
