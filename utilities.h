//
// Created by Peter Zheng on 2018/06/30.
//

#ifndef MIPS_UTILITIES_H
#define MIPS_UTILITIES_H

#include "Base.h"
#include "stdc++.h"
#include "parser.h"
using namespace std;

const int regNUMMAX = 35;
const int memoryMAX = 4 * 1024 * 1024;
static parser Parser;

unsigned short sizeT(Base &t) {
    return t.getSize();
}

template <class T>
T atoi(const string &rhs){
    T ret;
    stringstream ss;
    ss << rhs;
    ss >> ret;
    return ret;
}

void debugMess(const string &rhs, const string &topic, int newLine = 0){
    cout << "Debug:[" << topic << "]" << (newLine == 1 ? "\n" : "" )<< rhs << endl;
}

void deleteCertainChar(string& rhs, char ch){
    int pos = 0;
    int posEnd = (int)rhs.length();
    while(pos < (int)rhs.length()) if(rhs[pos] == ch) rhs.erase(pos, 1); else pos++;
}

string decodeSpecial(const string &rhs) {
    string ret = "";
    int pos = 0;
    char c;
    while (pos < rhs.length()) {
        c = rhs[pos++];
        if (c == '\\' && pos != rhs.length()) {
            switch (rhs[pos]) {
                case 'a':
                    c = '\a';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 'f':
                    c = '\f';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'v':
                    c = '\v';
                    break;
                case '\\':
                    c = '\\';
                    break;
                case '?':
                    c = '\?';
                    break;
                case '\'':
                    c = '\'';
                    break;
                case '\"':
                    c = '\"';
                    break;
                case '0':
                    c = '\0';
                    break;
                default:
                    continue;
            }
            pos++;
        }
        ret += c;
    }
    return ret;
}

int string2int(const string& rhs){
    int final = 0;
    bool neg = false;
    if(rhs[0] == '-') neg = true;
    for(int i = 0; i < rhs.length();i++){
        if(rhs[i] <= '9' && rhs[i] >= '0') final = final * 10 + rhs[i] - '0';
    }
    return neg ? -final : final;
}


#endif //MIPS_UTILITIES_H
