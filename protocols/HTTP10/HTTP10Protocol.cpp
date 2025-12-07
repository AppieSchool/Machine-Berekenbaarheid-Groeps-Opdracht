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

SemanticResult HTTP10Protocol::validateSemantics(const std::vector<Token>& tokens)
{
    HTTPRequest req;

    try {
        // Converts tokens → HTTPRequest (may throw on malformed structure)
        req = HTTPRequest::fromTokens(tokens);
    }
    catch (const std::exception& ex) {
        return SemanticResult::failure(
            std::string("Malformed HTTP structure: ") + ex.what(),
            "parser-structure-error"
        );
    }

    // Run semantic validation (returns SemanticResult directly)
    SemanticResult sem = HTTP10_semantics::validateRequest(req);

    if (!sem.ok)
        return sem;

    return SemanticResult::success();
}

