#include "HTTPTreeBuilder.h"

using namespace std;

string HTTPTreeBuilder::tokenToTerminal(const Token& token) {
    switch (token.base) {
        case BaseToken::SP:     return "SP";
        case BaseToken::CRLF:   return "CRLF";
        case BaseToken::COLON:  return "COLON";
        case BaseToken::SLASH:  return "SLASH";
        case BaseToken::DOT:    return "DOT";
        case BaseToken::END_OF_INPUT: return "$";
        case BaseToken::IDENT:
            // Map to grammar terminal names (must match http10.json!)
            if (token.lexeme == "GET")  return "METHOD_GET";
            if (token.lexeme == "POST") return "METHOD_POST";
            if (token.lexeme == "HEAD") return "METHOD_HEAD";
            if (token.lexeme == "HTTP/1.0") return "HTTP_VERSION_1_0";
            return "IDENT";
        default:
            return "UNKNOWN";
    }
}

ParseTree HTTPTreeBuilder::build(const vector<Token>& tokens) {
    vector<ParseTree> children;
    
    // Find key positions in token stream
    size_t firstCRLF = 0;
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].base == BaseToken::CRLF) {
            firstCRLF = i;
            break;
        }
    }
    
    // Build RequestLine subtree
    vector<ParseTree> requestLineChildren;
    for (size_t i = 0; i < firstCRLF; i++) {
        string terminal = tokenToTerminal(tokens[i]);
        string label = terminal;
        // Show lexeme for identifiers (use \\n for DOT newlines)
        if (tokens[i].base == BaseToken::IDENT) {
            label = terminal + "\\n'" + tokens[i].lexeme + "'";
        }
        requestLineChildren.push_back(make_shared<ParseTreeNode>(label));
    }
    requestLineChildren.push_back(make_shared<ParseTreeNode>("CRLF"));
    auto requestLine = make_shared<ParseTreeNode>("RequestLine", requestLineChildren);
    children.push_back(requestLine);
    
    // Build Headers subtree
    vector<ParseTree> headersChildren;
    vector<ParseTree> currentHeader;
    
    for (size_t i = firstCRLF + 1; i < tokens.size(); i++) {
        if (tokens[i].base == BaseToken::END_OF_INPUT) break;
        
        string terminal = tokenToTerminal(tokens[i]);
        string label = terminal;
        if (tokens[i].base == BaseToken::IDENT) {
            label = terminal + "\\n'" + tokens[i].lexeme + "'";
        }
        
        if (tokens[i].base == BaseToken::CRLF) {
            currentHeader.push_back(make_shared<ParseTreeNode>("CRLF"));
            if (!currentHeader.empty()) {
                // Check if this is an empty line (end of headers)
                if (currentHeader.size() == 1) {
                    // Final CRLF
                    children.push_back(make_shared<ParseTreeNode>("CRLF\\n(end)"));
                } else {
                    headersChildren.push_back(
                        make_shared<ParseTreeNode>("Header", currentHeader)
                    );
                }
                currentHeader.clear();
            }
        } else {
            currentHeader.push_back(make_shared<ParseTreeNode>(label));
        }
    }
    
    if (!headersChildren.empty()) {
        auto headers = make_shared<ParseTreeNode>("Headers", headersChildren);
        children.push_back(headers);
    }
    
    return make_shared<ParseTreeNode>("Request", children);
}
