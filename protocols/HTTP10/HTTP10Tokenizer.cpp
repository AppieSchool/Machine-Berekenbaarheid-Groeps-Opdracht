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

    int line = 1;
    int col  = 1;

    while (pos < input.size()) {
        char c = input[pos];

        // ----- CRLF -----
        if (match(input, "\r\n")) {
            tokens.emplace_back(BaseToken::CRLF, "\\r\\n", pos, line, col);
            pos += 2;
            line += 1;
            col = 1;
            continue;
        }
        if (c == '\n') {
            tokens.emplace_back(BaseToken::CRLF, "\\n", pos, line, col);
            pos += 1;
            line += 1;
            col = 1;
            continue;
        }

        // ----- Space -----
        if (c == ' ') {
            tokens.emplace_back(BaseToken::SP, " ", pos, line, col);
            pos++;
            col++;
            continue;
        }

        // ----- Slash -----
        if (c == '/') {
            tokens.emplace_back(BaseToken::SLASH, "/", pos, line, col);
            pos++;
            col++;
            continue;
        }

        // ----- Dot -----
        if (c == '.') {
            tokens.emplace_back(BaseToken::DOT, ".", pos, line, col);
            pos++;
            col++;
            continue;
        }

        // ----- Colon -----
        if (c == ':') {
            tokens.emplace_back(BaseToken::COLON, ":", pos, line, col);
            pos++;
            col++;
            continue;
        }

        // ----- Methods -----
        if (match(input, "GET")) {
            tokens.emplace_back(BaseToken::IDENT, (int)HTTPToken::METHOD_GET, "GET", pos, line, col);
            pos += 3;
            col += 3;
            continue;
        }
        if (match(input, "POST")) {
            tokens.emplace_back(BaseToken::IDENT, (int)HTTPToken::METHOD_POST, "POST", pos, line, col);
            pos += 4;
            col += 4;
            continue;
        }
        if (match(input, "HEAD")) {
            tokens.emplace_back(BaseToken::IDENT, (int)HTTPToken::METHOD_HEAD, "HEAD", pos, line, col);
            pos += 4;
            col += 4;
            continue;
        }

        // ----- HTTP/1.0 -----
        if (match(input, "HTTP/1.0")) {
            tokens.emplace_back(BaseToken::IDENT, (int)HTTPToken::HTTP_VERSION_1_0, "HTTP/1.0", pos, line, col);
            pos += 8;
            col += 8;
            continue;
        }

        // ----- Identifier -----
        if (std::isalnum((unsigned char)c) || c == '-' ) {
            size_t startPos = pos;
            int startLine = line;
            int startCol  = col;

            Token t = readIdentifier(input, line, col);   // moves pos forward
            // update col by consumed length (identifier never contains \n here)
            col += (int)(pos - startPos);

            // BUT: t was created without line/col, so either:
            // A) make readIdentifier return Token with line/col
            // or
            // B) patch it here:
            t.line = startLine;
            t.col  = startCol;

            tokens.push_back(std::move(t));
            continue;
        }

        // ----- Unknown character -----
        tokens.emplace_back(BaseToken::ERROR, std::string(1, c), pos, line, col);
        pos++;
        col++;
    }

    tokens.emplace_back(BaseToken::END_OF_INPUT, "EOF", pos, line, col);
    return tokens;
}


Token HTTP10Tokenizer::readIdentifier(const std::string& input, int line, int col) {
    size_t start = pos;
    while (pos < input.size() && (std::isalnum(input[pos]) || input[pos] == '-' || input[pos] == '_')) {
        pos++;
    }
    return Token(BaseToken::IDENT, input.substr(start, pos - start), start);
}
