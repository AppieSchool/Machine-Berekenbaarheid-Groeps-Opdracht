#include "HTTP10Checker.h"

#include <sstream>
#include <iostream>

#include "../protocols/HTTP10/HTTP10Protocol.h"
#include "../protocols/HTTP10/HTTP10Tokenizer.h"
#include "../parsers/SLR.h"
#include "../grammers/CFG.h"
#include "../visualization/HTTPTreeBuilder.h"
#include "../visualization/DotGenerator.h"

bool runHTTP10Check(const std::string& input, ProtocolCheckResult& out)
{
    out = ProtocolCheckResult{};

    std::stringstream log;
    log << "=== HTTP/1.0 Request Validator ===\n";
    log << "Input size: " << input.size() << " bytes\n\n";

    if (input.empty()) {
        out.syntaxOk = false;
        out.syntaxMessage = "Input is empty";
        log << "Error: empty input\n";
        out.logText = log.str();
        return false;
    }

    //
    // --- TOKENIZATION ---
    //
    log << "--- Step 2: Tokenizing ---\n";
    HTTP10Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize(input);

    for (size_t i = 0; i < tokens.size(); ++i) {
        std::string term = HTTPTreeBuilder::tokenToTerminal(tokens[i]);
        std::string entry = "[" + std::to_string(i) + "] " + term;

        if (tokens[i].lexeme != term &&
            tokens[i].lexeme != " " &&
            tokens[i].lexeme != "\\r\\n")
        {
            entry += " (\"" + tokens[i].lexeme + "\")";
        }

        out.tokens.push_back(entry);
        log << "  " << entry << "\n";
    }
    log << "\n";

    //
    // Convert tokens → parser terminals
    //
    std::vector<std::string> terminalSeq;
    for (auto& t : tokens)
        if (t.base != BaseToken::END_OF_INPUT)
            terminalSeq.push_back(HTTPTreeBuilder::tokenToTerminal(t));

    //
    // --- PARSING ---
    //
    log << "--- Step 3: Parsing (SLR) ---\n";
    HTTP10Protocol protocol;
    CFG grammar = protocol.getCFG();
    log << "Loaded grammar\n";

    SLR parser(grammar);
    log << "Parsing token sequence...\n";

    bool parseResult = parser.parse(terminalSeq);

    log << "\n--- Step 4: Syntax Results ---\n";

    if (!parseResult) {
        //
        // Retrieve structured diagnostic info
        //
        auto& d = parser.lastDiagnostic;

        out.syntaxOk = false;
        out.syntaxMessage = d.message;
        out.diagnostic = d;                   // STORE FOR GUI

        //
        // Logging
        //
        log << d.message << "\n";
        log << "Expected:\n";
        for (auto& e : d.expected) log << "  " << e << "\n";

        log << "But got: " << d.got << "\n";

        if (!d.interpretation.empty())
            log << d.interpretation << "\n";

        if (!d.likelyError.empty())
            log << "Likely error: " << d.likelyError << "\n";

        if (!d.examples.empty()) {
            log << "Examples:\n";
            for (auto& ex : d.examples)
                log << "  " << ex << "\n";
        }

        out.logText = log.str();
        return false;
    }

    out.syntaxOk = true;
    out.syntaxMessage = "Syntax OK";
    log << "✓ SYNTAX VALID: The request is syntactically correct!\n";

    //
    // --- SEMANTICS ---
    //
    log << "\n--- Step 5: Semantic Validation ---\n";
    auto sem = protocol.validateSemantics(tokens);

    if (!sem.ok) {
        out.semanticsOk = false;
        out.semanticsMessage = sem.message;

        log << "✗ SEMANTIC ERROR: " << sem.message << "\n";
        if (!sem.code.empty())
            log << "Error code: " << sem.code << "\n";

        out.logText = log.str();
        return false;
    }

    out.semanticsOk = true;
    out.semanticsMessage = "Semantics OK";
    log << "✓ SEMANTICS VALID: The request meaning is valid!\n";

    //
    // --- PARSE TREE ---
    //
    log << "\n--- Step 6: Generating Parse Tree Visualization ---\n";
    auto tree = HTTPTreeBuilder::build(tokens);

    std::string output = "visualization/output/parse_tree_gui.png";
    DotGenerator::generateImage(tree, output);

    out.hasParseTree = true;
    out.parseTreePath = output;

    log << "Parse tree saved to " << output << "\n";
    log << "\n=== Pipeline Complete ===\n";

    out.logText = log.str();
    return true;
}
