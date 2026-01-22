//
// Created by Skip on 01/12/2025.
//

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class BaseToken {
    SP,                 // space
    CRLF,               // \r\n
    COLON,              // :
    SLASH,              // /
    DOT,                // .
    IDENT,              // generic identifier

    END_OF_INPUT,       // EOF marker
    ERROR               // tokenizer failure
};


class Token {
public:
    BaseToken base;     // shared token category
    int subtype;        // protocol-specific token type
    std::string lexeme; // original text
    int position;       // character position in input (optional but useful)
    int line;
    int col;

    // Base token
    Token(BaseToken b, std::string lex, int pos = -1, int line = -1, int col = -1)
        : base(b), subtype(-1), lexeme(std::move(lex)), position(pos), line(line), col(col)  {}

    // Base token + protocol-specific subtype
    Token(BaseToken b, int sub, std::string lex, int pos = -1, int line = -1, int col = -1)
        : base(b), subtype(sub), lexeme(std::move(lex)), position(pos), line(line), col(col) {}

    Token() : base(BaseToken::ERROR), subtype(-1), lexeme(""), position(-1), line(-1), col(-1) {}
};

#endif // TOKEN_H
