//
// Created by Skip on 01/12/2025.
//

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class BaseToken {
    SP,             // space
    CRLF,           // \r\n
    COLON,          // :
    SLASH,          // /
    DOT,            // .
    IDENT,          // generic identifier (header name, path segment, etc.)
    END_OF_INPUT,   // EOF marker
    ERROR           // tokenizer failure
};

class Token {
public:
    BaseToken base;     // shared token category
    int subtype;        // protocol-specific token type
    std::string lexeme; // original text
    int position;       // character position in input (optional but useful)

    // Base token
    Token(BaseToken b, std::string lex, int pos = -1)
        : base(b), subtype(-1), lexeme(std::move(lex)), position(pos) {}

    // Base token + protocol-specific subtype
    Token(BaseToken b, int sub, std::string lex, int pos = -1)
        : base(b), subtype(sub), lexeme(std::move(lex)), position(pos) {}

    Token() : base(BaseToken::ERROR), subtype(-1), lexeme(""), position(-1) {}
};

#endif // TOKEN_H
