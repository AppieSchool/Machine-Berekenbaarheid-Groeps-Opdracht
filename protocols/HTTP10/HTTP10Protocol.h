//
// Created by Skip on 01/12/2025.
//

#ifndef HTTP10PROTOCOL_H
#define HTTP10PROTOCOL_H

#include "../Protocol.h"
#include <vector>
#include <string>
#include "HTTP10Tokenizer.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "HTTPrequest.h"
#include "HTTP10_semantics.h"
#include "../grammers/PDA.h"
class HTTP10Protocol : public Protocol {
public:
    std::vector<Token> tokenize(const std::string& input) override;
    CFG getCFG() override;
    SemanticResult validateSemantics(const std::vector<Token>& tokens) override;
};

#endif

