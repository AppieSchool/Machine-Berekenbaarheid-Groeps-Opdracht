//
// Created by Skip on 01/12/2025.
//

#ifndef SEMANTICRESULT_H
#define SEMANTICRESULT_H
#include <iostream>

struct SemanticResult {
    bool ok;
    std::string message;

    static SemanticResult OK() { return {true, ""}; }
    static SemanticResult Error(const std::string& msg) {
        return {false, msg};
    }
};

#endif //SEMANTICRESULT_H

