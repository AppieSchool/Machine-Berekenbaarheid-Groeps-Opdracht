#ifndef MACHINE_BEREKENBAARHEID_GROEPS_OPDRACHT_HTTPTREEBUILDER_H
#define MACHINE_BEREKENBAARHEID_GROEPS_OPDRACHT_HTTPTREEBUILDER_H

#include "ParseTree.h"
#include "../protocols/Token.h"
#include <vector>

using namespace std;

class HTTPTreeBuilder {
public:
    // Build a parse tree from HTTP tokens
    static ParseTree build(const vector<Token>& tokens);
    
    // Convert a token to its grammar terminal name
    static string tokenToTerminal(const Token& token);
};

#endif
