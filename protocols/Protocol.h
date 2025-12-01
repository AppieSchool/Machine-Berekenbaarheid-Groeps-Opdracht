//
// Created by Skip on 01/12/2025.
//

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <vector>
#include "Token.h"
#include "../grammers/CFG.h"
#include "SemanticResult.h"

class Protocol {
public:
    virtual ~Protocol() = default;
    virtual std::vector<Token> tokenize(const std::string& input) = 0;
    virtual CFG getCFG() = 0;
};



#endif //PROTOCOL_H
