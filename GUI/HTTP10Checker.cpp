#include "HTTP10Checker.h"

#include <sstream>
#include <iostream>
#include <filesystem>
#include <algorithm> // max/min

#include "../protocols/HTTP10/HTTP10Protocol.h"
#include "../protocols/HTTP10/HTTP10Tokenizer.h"
#include "../parsers/SLR.h"
#include "../grammers/CFG.h"
#include "../visualization/HTTPTreeBuilder.h"
#include "../visualization/DotGenerator.h"

using namespace std;

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

    // --- TOKENIZATION ---
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

    // --- Convert tokens → parser terminals + mapping to original token index ---
    std::vector<std::string> terminalSeq;
    terminalSeq.reserve(tokens.size());

    std::vector<int> termToTokIdx;          // <--- NEW
    termToTokIdx.reserve(tokens.size());

    for (int i = 0; i < (int)tokens.size(); ++i) {
        if (tokens[i].base == BaseToken::END_OF_INPUT) continue;
        terminalSeq.push_back(HTTPTreeBuilder::tokenToTerminal(tokens[i]));
        termToTokIdx.push_back(i);          // <--- NEW: parser index -> tokens[i]
    }

    // --- PARSING ---
    log << "--- Step 3: Parsing (SLR) ---\n";
    HTTP10Protocol protocol;
    CFG grammar = protocol.getCFG();
    log << "Loaded grammar\n";

    SLR parser(grammar);
    log << "Parsing token sequence...\n";

    bool parseResult = parser.parse(terminalSeq);

    log << "\n--- Step 4: Syntax Results ---\n";

    // Helper: haal 1 specifieke lijn uit input (1-based)
    auto getLineText = [&](int targetLine) -> std::string {
        int line = 1;
        size_t start = 0;
        for (size_t i = 0; i <= input.size(); ++i) {
            if (i == input.size() || input[i] == '\n') {
                if (line == targetLine) {
                    size_t end = i;
                    if (end > start && input[end - 1] == '\r') end--; // strip \r
                    return input.substr(start, end - start);
                }
                line++;
                start = i + 1;
            }
        }
        return "";
    };

    auto printCaret = [&](const std::string& lineText, int col1based) {
        log << "  " << lineText << "\n";
        log << "  ";
        for (int i = 1; i < col1based; ++i) log << " ";
        log << "^\n";
    };

    if (!parseResult) {
        auto& d = parser.lastDiagnostic;

        out.syntaxOk = false;
        out.syntaxMessage = d.message;
        out.diagnostic = d;

        // ---- TOP: Line/Column caret output ----
        int err = parser.lastErrorIndex; // parser index

        if (err >= 0 && err < (int)termToTokIdx.size()) {
            int tokIndex = termToTokIdx[err];

            // requires Token to have line/col (as you added)
            int line = tokens[tokIndex].line; // 1-based
            int col  = tokens[tokIndex].col;  // 1-based

            std::string lineText = getLineText(line);

            log << "ERROR at Line " << line << ", Column " << col << ":\n";
            if (!lineText.empty()) {
                printCaret(lineText, col);
            } else {
                log << "  (could not extract line text)\n";
            }
            log << "\n";
        }

        // ---- Minimal Syntax Diagnostic (NO interpretation/likely/examples) ----
        log << "Syntax Diagnostic:\n";
        log << d.message << "\n";
        log << "Expected:\n";
        for (auto& e : d.expected) log << "  " << e << "\n";
        log << "Got:\n  " << d.got << "\n";

        // ---- Simple suggestion (optional) ----
        if (!d.expected.empty()) {
            log << "\nSuggestion: insert '" << d.expected[0]
                << "' before '" << d.got << "'.\n";
        }

        out.logText = log.str();
        return false;
    }


    // ... rest blijft identiek ...
    out.syntaxOk = true;
    out.syntaxMessage = "Syntax OK";
    log << "✓ SYNTAX VALID: The request is syntactically correct!\n";

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

    log << "\n--- Step 6: Generating Parse Tree Visualization ---\n";
    auto tree = HTTPTreeBuilder::build(tokens);

    filesystem::path outputDir = filesystem::current_path() / "visualization" / "output";
    filesystem::create_directories(outputDir);

    string output = (outputDir / "parse_tree_gui.png").string();
    DotGenerator::generateImage(tree, output);

    out.hasParseTree = true;
    out.parseTreePath = output;

    log << "Parse tree saved to " << output << "\n";
    log << "\n=== Pipeline Complete ===\n";

    out.logText = log.str();
    return true;
}

