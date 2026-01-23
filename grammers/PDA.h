//
// Created by abdir on 06/10/2025.
//

#ifndef MB_PDA_H
#define MB_PDA_H
#include <iostream>
#include "CFG.h"

struct transition {
    std::string fromState;
    std::string toState;
    std::string inputSymbol;
    std::string stackTop;
    std::vector<std::string> stackPush;
};

class PDA {
public:
    explicit PDA(std::string jsonFile);
    CFG toCFG();
    ~PDA() = default;
private:
    std::string filename;
    std::vector<std::string> states;
    std::vector<std::string> alphabet;
    std::vector<std::string> stackAlphabet;
    std::string startState;
    std::string startStack;
    std::vector<transition> transitions;
};


#endif //MB_PDA_H
