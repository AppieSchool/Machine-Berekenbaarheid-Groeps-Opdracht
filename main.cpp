#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "protocols/HTTP10/HTTP10Protocol.h"
#include "protocols/HTTP10/HTTP10Tokenizer.h"
#include "parsers/SLR.h"
#include "grammers/CFG.h"
#include "visualization/HTTPTreeBuilder.h"
#include "visualization/DotGenerator.h"

using namespace std;

// Helper: Read entire file into string
string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file: " << filename << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper: Print tokens nicely
void printTokens(const vector<Token>& tokens) {
    cout << "\n=== TOKENS ===" << endl;
    cout << "Found " << tokens.size() << " tokens:" << endl;
    for (size_t i = 0; i < tokens.size(); i++) {
        string terminal = HTTPTreeBuilder::tokenToTerminal(tokens[i]);
        cout << "  [" << i << "] " << terminal;
        if (tokens[i].lexeme != terminal &&
            tokens[i].lexeme != " " &&
            tokens[i].lexeme != "\\r\\n")
        {
            cout << " (\"" << tokens[i].lexeme << "\")";
        }
        cout << endl;
    }
}

int main(int argc, char* argv[]) {

    cout << "========================================" << endl;
    cout << "   HTTP/1.0 Request Validator" << endl;
    cout << "========================================" << endl;

    // Determine input file
    string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        filename = "protocols/HTTP10/cases/valid_request_2.txt";
        cout << "\nNo file specified, using default: " << filename << endl;
    }

    // STEP 1: Read input
    cout << "\n--- Step 1: Reading input ---" << endl;
    string input = readFile(filename);
    if (input.empty()) return 1;
    cout << "Input (" << input.size() << " bytes):\n--------------------\n"
         << input << "\n--------------------\n";

    // STEP 2: Tokenize
    cout << "\n--- Step 2: Tokenizing ---" << endl;
    HTTP10Tokenizer tokenizer;
    vector<Token> tokens = tokenizer.tokenize(input);
    printTokens(tokens);

    // Convert tokens → parser symbols
    vector<string> terminalSequence;
    for (const auto& token : tokens) {
        if (token.base == BaseToken::END_OF_INPUT) continue;
        terminalSequence.push_back(HTTPTreeBuilder::tokenToTerminal(token));
    }

    // STEP 3: Parse (SLR)
    cout << "\n--- Step 3: Parsing (SLR) ---" << endl;

    HTTP10Protocol protocol;
    CFG grammar = protocol.getCFG();
    cout << "Loaded grammar from http10.json" << endl;

    SLR parser(grammar);
    cout << "Parsing token sequence..." << endl;
    bool parseResult = parser.parse(terminalSequence);

    cout << "\n--- Step 4: Syntax Results ---" << endl;
    if (!parseResult) {
        cout << "✗ SYNTAX ERROR: The HTTP request has syntax errors." << endl;
        cout << "Check the parser output above for details." << endl;
        cout << "\n========================================\nPipeline Complete\n========================================\n";
        return 1;
    }

    cout << "✓ SYNTAX VALID: The HTTP request is syntactically correct!" << endl;

    // STEP 5: Semantic validation (NEW)
    cout << "\n--- Step 5: Semantic Validation ---" << endl;
    SemanticResult sem = protocol.validateSemantics(tokens);

    if (!sem.ok) {
        cout << "✗ SEMANTIC ERROR: " << sem.message << endl;
        if (!sem.code.empty()) cout << "Error code: " << sem.code << endl;
        cout << "\n========================================\nPipeline Complete\n========================================\n";
        return 1;
    }

    cout << "✓ SEMANTICS VALID: The request meaning is valid!" << endl;

    // STEP 6: Parse tree visualization (only if syntax + semantics OK)
    cout << "\n--- Step 6: Generating Parse Tree Visualization ---" << endl;

    ParseTree tree = HTTPTreeBuilder::build(tokens);
    string outputPath = "visualization/output/parse_tree.png";
    DotGenerator::generateImage(tree, outputPath);

    cout << "\nTo view the parse tree, run:\n  open " << outputPath << endl;

    cout << "\n========================================\nPipeline Complete\n========================================" << endl;
    return 0;
}
