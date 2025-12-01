#include "HTTP10Protocol.h"


// ----------------------------------------------------------
// 1. Tokenize input using HTTP10Tokenizer
// ----------------------------------------------------------
std::vector<Token> HTTP10Protocol::tokenize(const std::string& input) {
    HTTP10Tokenizer tokenizer;
    return tokenizer.tokenize(input);
}

// 2. Load the CFG from http10.json
CFG HTTP10Protocol::getCFG() {
    return CFG("protocols/HTTP10/http10.json");
}

