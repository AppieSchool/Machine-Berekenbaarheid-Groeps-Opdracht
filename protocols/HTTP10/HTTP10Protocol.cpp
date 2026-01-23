#include "HTTP10Protocol.h"


// ----------------------------------------------------------
// 1. Tokenize input using HTTP10Tokenizer
// ----------------------------------------------------------
std::vector<Token> HTTP10Protocol::tokenize(const std::string& input) {
    HTTP10Tokenizer tokenizer;
    return tokenizer.tokenize(input);
}

// ----------------------------------------------------------
// 2. Get CFG for SLR parsing (handcrafted, SLR-friendly)
// ----------------------------------------------------------
CFG HTTP10Protocol::getCFG() {
    // For SLR parsing, use the handcrafted grammar
    // This grammar is optimized for deterministic bottom-up parsing
    return CFG("protocols/HTTP10/http10.json");
}

// ----------------------------------------------------------
// 3. Get CFG from PDA conversion (for CYK and theoretical demonstration)
// ----------------------------------------------------------
CFG HTTP10Protocol::getCFGFromPDA() {
    // Load PDA specification
    PDA pda("protocols/HTTP10/http10_pda.json");

    // Convert PDA to CFG using standard algorithm
    CFG cfg = pda.toCFG();

    // Apply grammar simplification
    cfg.removeUnreachableSymbols();
    cfg.removeUselessProductions();

    return cfg;
}

// ----------------------------------------------------------
// 4. Semantic validation
// ----------------------------------------------------------
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