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
static int reg1 = 0;
static int reg2 = 0;
static int reg3 = 0;
static int reg4 = 0;
static int reg5 = 0;
static int reg6 = 0;
static int nextLine = 0;
static bool haveJump = false;
#define DADDR unsigned char
_WORD regNum[regNUMMAX];
DADDR mem[memoryMAX];
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

inline void debugMess(const string &rhs, const string &topic, int newLine = 0){
    cout << "Debug:[" << topic << "]" << (newLine == 1 ? "\n" : "" )<< rhs << endl;
}

inline void deleteCertainChar(string& rhs, char ch){
    reg1++;
    int pos = 0;
    int posEnd = (int)rhs.length();
    while(pos < (int)rhs.length()) if(rhs[pos] == ch) rhs.erase(pos, 1); else pos++;
}

inline string decodeSpecial(const string &rhs) {
    reg2++;

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

inline int string2int(const string& rhs){
    reg3++;

    int final = 0;
    bool neg = false;
    if(rhs[0] == '-') neg = true;
    for(int i = 0; i < rhs.length();i++){
        if(rhs[i] <= '9' && rhs[i] >= '0') final = final * 10 + rhs[i] - '0';
    }
    return neg ? -final : final;
}


inline bool haveBrackets(const string &rhs){
    reg4++;

    for(int i = 0;i < rhs.length(); ++i)
        if(rhs[i] == '(' || rhs[i] == ')') return true;
    return false;
}

inline string splitWithCertainChar(string &rhs, char ch, int k = 1){
    reg5++;

    string string1(rhs);
    string ret = "";
    int i = 0;
    for(i = 0;i < rhs.length(); ++i){
        if(string1[i] != ch) ret += string1[i];
        if(string1[i] == ch) break;
    }
    rhs = "";
    for(int j = i + k; j < string1.length(); ++j){
        rhs += string1[j];
    }
    return ret;
}

inline bool haveAlpha(const string& rhs){
    reg6++;
    if(Parser.LabelAddr(rhs) != -1) return true;
    return false;
}


inline bool allNumber(const string& rhs){
    for(int i = 0;i < rhs.length(); ++i)
        if((rhs[i] < '0' || rhs[i] > '9') && (rhs[i] != '-') && (i == 0)) return false;

    return true;
}
#endif //MIPS_UTILITIES_H
