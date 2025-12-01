//
// Created by Skip on 01/12/2025.
//

#ifndef HTTP10TOKENIZER_H
#define HTTP10TOKENIZER_H

#include <string>
#include <vector>
#include "../Token.h"
#include "HTTPtoken.h"

class HTTP10Tokenizer {
public:
    std::vector<Token> tokenize(const std::string& input);

private:
    size_t pos = 0;

    bool match(const std::string& text, const std::string& target);
    Token readCRLF(const std::string& input);
    Token readMethod(const std::string& input);
    Token readVersion(const std::string& input);
    Token readIdentifier(const std::string& input);
};

#endif
