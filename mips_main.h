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

    UINT32 regNum[regNUMMAX];
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
                continue;
            }
            if (token == ".text") {
                dataField = false;
                textField = true;
                if (controlDebug) debugMess("In Section: Text Started.", "Parser");
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
                    } else ss >> next;
                    deleteCertainChar(next, ':');
                    if (controlDebug) cout << "Variable:" << next << "|\n" << endl;
                    Parser.labelMap[next] = memHead;
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

                    }
                    if (result == SYSCALL) {

                    }
                    if (result == MUL || result == MULU || result == DIV || result == DIVU){
                        string arg1, arg2;
                        ss >> arg1 >> arg2;
                        deleteCertainChar(arg1, ',');
                        deleteCertainChar(arg2, ',');

                        s1.argv.push_back(arg1);
                        s1.argv.push_back(arg2);
                        string ss3 = ss.str();
                        short code = -1;
                        for(int i = 0; i < ss3.length(); ++i){
                            if(ss3[i] == '\n'){
                                code = 10;
                                break;
                            }
                            if(ss3[i] <= '9' && ss3[i] >= '0'){
                                code = 52;
                                break;
                            }
                        }
                        if(code == 52){
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

    void run() {
        instructionFetch();
        instructionDecode();
        execution();
        memoryAccess();
        writeBack();
    }


};

mips Kernel = mips();

#endif //MIPS_MIPS_MAIN_H
