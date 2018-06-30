//
// Created by Peter Zheng on 2018/06/30.
//

#include "stdc++.h"
#include "mips_main.h"
using namespace std;
extern bool controlDebug;
extern mips Kernel;

void debugMess(const string &rhs, const string &topic, int newLine = 0){
    cout << "Debug:[" << topic << "]" << (newLine == 1 ? "\n" : "" )<< rhs << endl;
}

int main(int argc, char* argv[]){
    if(argc < 2){
        cout << "MIPS Simulator v0.1\nCopyright to Zheng Wenxin.\n\nPlease entry the Correct Source File.\n";
        return 0;
    }
    if(argc == 3){
        if(strncmp(argv[2], "-d", 2)) {
            cout << "MIPS Simulator v0.1\nCopyright to Zheng Wenxin.\n\nUndefined Control Switch.\n";
            return 0;
        }
        controlDebug = true;
        cout << "MIPS Simulator v0.1\nCopyright to Zheng Wenxin.\n\nDebug Mode is enabled.\n";
    }
    if(argc > 3){
        cout << "MIPS Simulator v0.1\nCopyright to Zheng Wenxin.\n\nPlease entry the Correct Source File.\n";
        return 0;
    }
    ifstream File(argv[1]);
    stringstream buf;
    buf << File.rdbuf();
    string data = buf.str();
    if(controlDebug) debugMess(data, "Receive Message From File", 1);
    //TODO : mips core
    if(controlDebug) debugMess("Execute Finish.", "General");
}