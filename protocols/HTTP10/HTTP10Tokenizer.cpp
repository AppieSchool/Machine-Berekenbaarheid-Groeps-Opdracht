//
// Created by Skip on 01/12/2025.
//

#include "HTTP10Tokenizer.h"
#include <cctype>

bool HTTP10Tokenizer::match(const std::string& text, const std::string& target) {
    if (text.compare(pos, target.size(), target) == 0) {
        return true;
    }
    return false;
}

std::vector<Token> HTTP10Tokenizer::tokenize(const std::string& input) {
    std::vector<Token> tokens;
    pos = 0;

    while (pos < input.size()) {
        char c = input[pos];

        // ----- CRLF (proper HTTP) or just LF (for testing) -----
        if (match(input, "\r\n")) {
            tokens.emplace_back(BaseToken::CRLF, "\\r\\n", pos);
            pos += 2;
            continue;
        }
        // Also accept just \n (common in test files on macOS/Linux)
        if (c == '\n') {
            tokens.emplace_back(BaseToken::CRLF, "\\n", pos);
            pos++;
            continue;
        }

        // ----- Space -----
        if (c == ' ') {
            tokens.emplace_back(BaseToken::SP, " ", pos);
            pos++;
            continue;
        }

        // ----- Slash -----
        if (c == '/') {
            tokens.emplace_back(BaseToken::SLASH, "/", pos);
            pos++;
            continue;
        }

        // ----- Dot -----
        if (c == '.') {
            tokens.emplace_back(BaseToken::DOT, ".", pos);
            pos++;
            continue;
        }

        // ----- Colon -----
        if (c == ':') {
            tokens.emplace_back(BaseToken::COLON, ":", pos);
            pos++;
            continue;
        }

        // ----- Methods -----
        if (match(input, "GET")) {
            tokens.emplace_back(BaseToken::IDENT, (int)HTTPToken::METHOD_GET, "GET", pos);
            pos += 3;
            continue;
        }
        if (match(input, "POST")) {
            tokens.emplace_back(BaseToken::IDENT, (int)HTTPToken::METHOD_POST, "POST", pos);
            pos += 4;
            continue;
        }
        if (match(input, "HEAD")) {
            tokens.emplace_back(BaseToken::IDENT, (int)HTTPToken::METHOD_HEAD, "HEAD", pos);
            pos += 4;
            continue;
        }

        // ----- HTTP/1.0 -----
        if (match(input, "HTTP/1.0")) {
            tokens.emplace_back(BaseToken::IDENT, (int)HTTPToken::HTTP_VERSION_1_0, "HTTP/1.0", pos);
            pos += 8;
            continue;
        }

        // ----- Identifier (alphanumeric or '-') -----
        if (std::isalnum(c) || c == '-' ) {
            tokens.push_back(readIdentifier(input));
            continue;
        }

        // ----- Unknown character -----
        tokens.emplace_back(BaseToken::ERROR, std::string(1, c), pos);
        pos++;
    }

    tokens.emplace_back(BaseToken::END_OF_INPUT, "EOF", pos);
    return tokens;
}

Token HTTP10Tokenizer::readIdentifier(const std::string& input) {
    size_t start = pos;
    while (pos < input.size() && (std::isalnum(input[pos]) || input[pos] == '-' || input[pos] == '_')) {
        pos++;
    }
    return Token(BaseToken::IDENT, input.substr(start, pos - start), start);
}
