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

class mips {
private:
    int memHead;

    int mainEntryPoint; //the Main Entry Point

    _WORD regNum[regNUMMAX];
    DADDR mem[memoryMAX];

    string coreData;

    vector<Execution> program;

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
public:
    mips() {
        for (int i = 0; i < regNUMMAX; i++) regNum[i] = 0;
        for (int i = 0; i < memoryMAX; i++) mem[i] = 0;
        memHead = 0;
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

    void setInstruction(const string &rhs) {
        coreData = rhs;
        stringstream ss;
        ss << rhs;
        string token;
        bool dataField = false;
        bool textField = false;
        string lastLabel = "";
        string NextToken = "";
        Execution lastExe;
        bool haveLabel = false;
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
                    ss >> next;
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
                    program.push_back(lastExe);
                    executionMap[lastExe.labelName] = program.size() - 1;
                    if (lastExe.labelName == "main") mainEntryPoint = program.size() - 1;
                    lastExe.clear();
                    deleteCertainChar(token, ':');
                    lastExe.labelName = token;
                } else {
                    executionInstruction s1;
                    s1.type = result;
                    if (result == ADD || result == ADDU || result == ADDIU || result == SUB ||
                        result == SUBU || result == XOR || result == XORU || result == REM ||
                        result == REMU || result == SEQ || result == SGE || result == SGT ||
                        result == SLE || result == SLT || result == SNE || result == BEQ ||
                        result == BNE || result == BGE || result == BLE || result == BGT ||
                        result == BLT) {
                        string arg1, arg2, arg3;
                        ss >> arg1 >> arg2 >> arg3;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');
                        deleteCertainChar(arg3, ',');

                        s1.argv.push_back(arg1);
                        s1.argv.push_back(arg2);
                        s1.argv.push_back(arg3);
                        lastExe.prog.push_back(s1);
                    }
                    if (result == LI || result == BEQZ || result == BNEZ || result == BLEZ || result == BGEZ ||
                        result == BGTZ || result == BLTZ || result == LA || result == LB || result == LH ||
                        result == LW || result == SB || result == SH || result == SW || result == MOVE ||
                        result == NEG || result == NEGU) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');

                        s1.argv.push_back(arg1);
                        s1.argv.push_back(arg2);
                        lastExe.prog.push_back(s1);
                    }
                    if (result == B || result == J || result == JR || result == JAL || result == JALR ||
                        result == MFHI || result == MFLO) {
                        string arg1;
                        ss >> arg1;
                        deleteCertainChar(arg1, ',');

                        s1.argv.push_back(arg1);
                        lastExe.prog.push_back(s1);
                    }
                    if (result == NOP) {
                        lastExe.prog.push_back(s1);
                    }
                    if (result == SYSCALL) {
                        lastExe.prog.push_back(s1);
                    }
                    if (result == MUL || result == MULU || result == DIV || result == DIVU) {
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');

                        s1.argv.push_back(arg1);
                        s1.argv.push_back(arg2);
                        streampos save = ss.tellg();
                        string ss3;
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
                        }
                        lastExe.prog.push_back(s1);
                    }
                }

            }
        }
    }

    void directlyRun_DR() {
        int current = mainEntryPoint;
        recursionRun_RR(program[mainEntryPoint]);
    }

    void recursionRun_RR(const Execution &rhs) {
        for (int i = 0; i < rhs.prog.size(); ++i) {
            executionInstruction tmp = rhs.prog[i];
            int a[3], dist;
            string s[3];
            bool f[3];
            f[0] = f[1] = f[2] = false;
            int count = tmp.argv.size();
            switch (tmp.argv.size()) {
                case 1:
                    if (tmp.argv[0][0] == '$') f[0] = true;
                    if (f[0]) a[0] = Parser.RegisterAddr(tmp.argv[0]); else a[0] = string2int(tmp.argv[0]);
                    s[0] = tmp.argv[0];
                    break;
                case 2:
                    if (tmp.argv[0][0] == '$') f[0] = true;
                    if (f[0]) a[0] = Parser.RegisterAddr(tmp.argv[0]); else a[0] = string2int(tmp.argv[0]);
                    if (tmp.argv[1][0] == '$') f[1] = true;
                    if (f[1]) a[1] = Parser.RegisterAddr(tmp.argv[1]); else a[1] = string2int(tmp.argv[1]);
                    s[0] = tmp.argv[0];
                    s[1] = tmp.argv[1];
                    break;
                case 3:
                    if (tmp.argv[0][0] == '$') f[0] = true;
                    if (f[0]) a[0] = Parser.RegisterAddr(tmp.argv[0]); else a[0] = string2int(tmp.argv[0]);
                    if (tmp.argv[1][0] == '$') f[1] = true;
                    if (f[1]) a[1] = Parser.RegisterAddr(tmp.argv[1]); else a[1] = string2int(tmp.argv[1]);
                    if (tmp.argv[2][0] == '$') f[2] = true;
                    if (f[2]) a[2] = Parser.RegisterAddr(tmp.argv[2]); else a[2] = string2int(tmp.argv[2]);
                    s[0] = tmp.argv[0];
                    s[1] = tmp.argv[1];
                    s[2] = tmp.argv[2];
                    break;
            }
            _DWORD tmp2;
            int nextExe;
            switch (tmp.type) {
                case ADD:
                    regNum[a[0]].s = (f[2] ? regNum[a[2]].s : a[2]) + regNum[a[1]].s;
                    break;
                case ADDU:
                    regNum[a[0]].us = (f[2] ? regNum[a[2]].us : (unsigned int) a[2]) + regNum[a[1]].us;
                    break;
                case ADDIU:
                    regNum[a[0]].us = (f[2] ? regNum[a[2]].us : (unsigned int) a[2]) + regNum[a[1]].us;
                    break;
                case SUB:
                    regNum[a[0]].s = regNum[a[1]].s - (f[2] ? regNum[a[2]].s : a[2]);
                    break;
                case SUBU:
                    regNum[a[0]].us = regNum[a[1]].us - (f[2] ? regNum[a[2]].us : (unsigned int) a[2]);
                    break;
                case MUL:
                    if(tmp.argv.size() == 2){
                        tmp2 = (long long) (f[2] ? regNum[a[2]].s : a[2]) * (long long) regNum[a[1]].s;
                        regNum[HIREGISTER] = _WORD(tmp2.core.u1, tmp2.core.u2, tmp2.core.u3, tmp2.core.u4);
                        regNum[LOREGISTER] = _WORD(tmp2.core.u5, tmp2.core.u6, tmp2.core.u7, tmp2.core.u8);
                    } else regNum[a[0]].s = (f[2] ? regNum[a[2]].s : a[2]) * regNum[a[1]].s;
                    break;
                case MULU:
                    if(tmp.argv.size() == 2){
                        tmp2 = (unsigned long long) (f[2] ? regNum[a[2]].us : a[2]) * (unsigned long long) regNum[a[1]].us;
                        regNum[HIREGISTER] = _WORD(tmp2.core.u1, tmp2.core.u2, tmp2.core.u3, tmp2.core.u4);
                        regNum[LOREGISTER] = _WORD(tmp2.core.u5, tmp2.core.u6, tmp2.core.u7, tmp2.core.u8);
                    } else regNum[a[0]].us = (f[2] ? regNum[a[2]].us : a[2]) * regNum[a[1]].us;
                    break;
                case DIVU:
                    if(tmp.argv.size() == 2){
                        regNum[HIREGISTER] = _WORD(regNum[a[1]].us / (f[2] ? regNum[a[2]].us : a[2]));
                        regNum[LOREGISTER] = _WORD(regNum[a[1]].us % (f[2] ? regNum[a[2]].us : a[2]));
                    } else regNum[a[0]].us = regNum[a[1]].us / (f[2] ? regNum[a[2]].us : a[2]);
                    break;
                case DIV:
                    if(tmp.argv.size() == 2){
                        regNum[HIREGISTER] = _WORD(regNum[a[1]].s / (f[2] ? regNum[a[2]].s : a[2]));
                        regNum[LOREGISTER] = _WORD(regNum[a[1]].s % (f[2] ? regNum[a[2]].s : a[2]));
                    } else regNum[a[0]].us = regNum[a[1]].us / (f[2] ? regNum[a[2]].us : a[2]);
                    break;
                case XOR:
                    regNum[a[0]].s = regNum[a[1]].s ^ (f[2] ? regNum[a[2]].s : a[2]);
                    break;
                case XORU:
                    regNum[a[0]].us = regNum[a[1]].us ^ (f[2] ? regNum[a[2]].us : a[2]);
                    break;
                case NEG:
                    regNum[a[0]].s = ~regNum[a[1]].s;
                    break;
                case NEGU:
                    regNum[a[0]].us = ~regNum[a[1]].us;
                    break;
                case REM:
                    regNum[a[0]].s = regNum[a[1]].s % (f[2] ? regNum[a[2]].s : a[2]);
                    break;
                case REMU:
                    regNum[a[0]].us = regNum[a[1]].us % (f[2] ? regNum[a[2]].us : a[2]);
                    break;
                case LI:
                    regNum[a[0]] = _WORD(a[1]);
                    break;
                case SEQ:
                    regNum[a[0]].s = (regNum[a[1]].s == (f[2] ? regNum[a[2]].s : a[2]));
                    break;
                case SGE:
                    regNum[a[0]].s = (regNum[a[1]].s >= (f[2] ? regNum[a[2]].s : a[2]));
                    break;
                case SGT:
                    regNum[a[0]].s = (regNum[a[1]].s > (f[2] ? regNum[a[2]].s : a[2]));
                    break;
                case SLE:
                    regNum[a[0]].s = (regNum[a[1]].s <= (f[2] ? regNum[a[2]].s : a[2]));
                    break;
                case SLT:
                    regNum[a[0]].s = (regNum[a[1]].s < (f[2] ? regNum[a[2]].s : a[2]));
                    break;
                case SNE:
                    regNum[a[0]].s = (regNum[a[1]].s != (f[2] ? regNum[a[2]].s : a[2]));
                    break;
                case J:
                case B:
                    nextExe = executionMap[s[0]];
                    recursionRun_RR(program[nextExe]);
                    break;
                case BEQ:
                    if(regNum[a[0]].s == (f[1] ? regNum[a[1]].s : a[1])){
                        nextExe = executionMap[s[2]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BNE:
                    if(regNum[a[0]].s != (f[1] ? regNum[a[1]].s : a[1])){
                        nextExe = executionMap[s[2]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BGE:
                    if(regNum[a[0]].s >= (f[1] ? regNum[a[1]].s : a[1])){
                        nextExe = executionMap[s[2]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BLE:
                    if(regNum[a[0]].s <= (f[1] ? regNum[a[1]].s : a[1])){
                        nextExe = executionMap[s[2]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BGT:
                    if(regNum[a[0]].s > (f[1] ? regNum[a[1]].s : a[1])){
                        nextExe = executionMap[s[2]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BLT:
                    if(regNum[a[0]].s < (f[1] ? regNum[a[1]].s : a[1])){
                        nextExe = executionMap[s[2]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BEQZ:
                    if(regNum[a[0]].s == 0){
                        nextExe = executionMap[s[1]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BNEZ:
                    if(regNum[a[0]].s != 0){
                        nextExe = executionMap[s[1]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BLEZ:
                    if(regNum[a[0]].s <= 0){
                        nextExe = executionMap[s[1]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BGEZ:
                    if(regNum[a[0]].s >= 0){
                        nextExe = executionMap[s[1]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BGTZ:
                    if(regNum[a[0]].s > 0){
                        nextExe = executionMap[s[1]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case BLTZ:
                    if(regNum[a[0]].s < 0){
                        nextExe = executionMap[s[1]];
                        recursionRun_RR(program[nextExe]);
                    }
                    break;
                case JR:

                    break;
                case JAL:

                    break;
                case JALR:

                    break;
                case LA:

                    break;
                case LB:

                    break;
                case LH:

                    break;
                case LW:

                    break;
                case SB:

                    break;
                case SH:

                    break;
                case SW:

                    break;
                case MOVE:
                    regNum[a[0]].s = regNum[a[1]].s;
                    break;
                case MFHI:
                    regNum[a[0]].s = regNum[HIREGISTER].s;
                    break;
                case MFLO:
                    regNum[a[0]].s = regNum[LOREGISTER].s;
                    break;
                case NOP:

                    break;
                case SYSCALL:
                    int optionCode = regNum[2].s;
                    int i;
                    int start;
                    int a;
                    int t;
                    string str;
                    switch(optionCode){
                        case 1: cout << regNum[4].s;
                            break;
                        case 4:
                            start = regNum[4].s;
                            for(i = start;;++i){
                                if(mem[i] == 0) break;
                                cout << (char) mem[i];
                            }
                            break;
                        case 5:
                            cin >> t;
                            regNum[4].s = t;
                            break;
                        case 8:
                            cin >> str;
                            break;
                        case 9:
                            a = regNum[4].s;
                            regNum[2].s = memHead;
                            memHead += a;
                            break;
                        case 10:
                            exit(0);
                        case 17:
                            exit(regNum[4].s);
                    }
                    break;
            }
        }
    }

    void run() {
//        instructionFetch();
//        instructionDecode();
//        execution();
//        memoryAccess();
//        writeBack();
//        directlyRun_DR();
    }


};

mips Kernel = mips();

#endif //MIPS_MIPS_MAIN_H
