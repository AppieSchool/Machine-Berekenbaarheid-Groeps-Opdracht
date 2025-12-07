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
        if (tokens[i].lexeme != terminal && tokens[i].lexeme != " " && tokens[i].lexeme != "\\r\\n") {
            cout << " (\"" << tokens[i].lexeme << "\")";
        }
        cout << endl;
    }
}

int main(int argc, char* argv[]) {
    cout << "========================================" << endl;
    cout << "   HTTP/1.0 Request Validator" << endl;
    cout << "========================================" << endl;

    // Get input file
    string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        filename = "protocols/HTTP10/cases/valid_request_1.txt";
        cout << "\nNo file specified, using default: " << filename << endl;
    }

    // ========== STEP 1: Read input ==========
    cout << "\n--- Step 1: Reading input ---" << endl;
    string input = readFile(filename);
    if (input.empty()) {
        return 1;
    }
    cout << "Input (" << input.size() << " bytes):" << endl;
    cout << "--------------------" << endl;
    cout << input << endl;
    cout << "--------------------" << endl;

    // ========== STEP 2: Tokenize ==========
    cout << "\n--- Step 2: Tokenizing ---" << endl;
    HTTP10Tokenizer tokenizer;
    vector<Token> tokens = tokenizer.tokenize(input);
    printTokens(tokens);

    // Convert tokens to terminal names for parser (skip END_OF_INPUT, parser adds its own)
    vector<string> terminalSequence;
    for (const auto& token : tokens) {
        if (token.base == BaseToken::END_OF_INPUT) continue;  // Parser adds <EOS> itself
        terminalSequence.push_back(HTTPTreeBuilder::tokenToTerminal(token));
    }

    // ========== STEP 3: Parse ==========
    cout << "\n--- Step 3: Parsing (SLR) ---" << endl;
    
    // Load grammar
    CFG grammar("protocols/HTTP10/http10.json");
    cout << "Loaded grammar from http10.json" << endl;
    
    // Create parser
    SLR parser(grammar);
    
    // Parse the token sequence
    cout << "Parsing token sequence..." << endl;
    bool parseResult = parser.parse(terminalSequence);

    // ========== STEP 4: Results ==========
    cout << "\n--- Step 4: Results ---" << endl;
    if (parseResult) {
        cout << "✓ SYNTAX VALID: The HTTP request is syntactically correct!" << endl;
        
        // ========== STEP 5: Visualization ==========
        cout << "\n--- Step 5: Generating Parse Tree Visualization ---" << endl;
        
        // Build parse tree from tokens
        ParseTree tree = HTTPTreeBuilder::build(tokens);
        
        // Generate PNG image
        string outputPath = "visualization/output/parse_tree.png";
        DotGenerator::generateImage(tree, outputPath);
        
        cout << "\nTo view the parse tree, run:" << endl;
        cout << "  open " << outputPath << endl;
        
    } else {
        cout << "✗ SYNTAX ERROR: The HTTP request has syntax errors." << endl;
        cout << "Check the parser output above for details." << endl;
    }

    cout << "\n========================================" << endl;
    cout << "   Pipeline Complete" << endl;
    cout << "========================================" << endl;

    return parseResult ? 0 : 1;
}
