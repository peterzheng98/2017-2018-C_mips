//
// Created by Peter Zheng on 2018/06/30.
//

#include "stdc++.h"
#include "mips_main.h"
#include "utilities.h"


using namespace std;
extern bool controlDebug;
extern mips Kernel;

int main(int argc, char *argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);

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
    if (controlDebug) debugMess(data, "Receive Message From File", 1);
    Kernel.setInstructionNew(data);
    Kernel.run();
    if (controlDebug) debugMess("Execute Finish.", "General");
    printf("\n[%d][%d][%d][%d][%d][%d]\n", reg1, reg2, reg3, reg4, reg5, reg6);
    printf("SW Stage[%d] LW Stage[%d]\n", arga1, arga2);
}