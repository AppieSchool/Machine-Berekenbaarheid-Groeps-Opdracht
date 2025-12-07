#include "HTTPRequest.h"
#include "../Token.h"
#include <stdexcept>

HTTPRequest HTTPRequest::fromTokens(const std::vector<Token>& tokens)
{
    HTTPRequest req;
    size_t i = 0;

    // -------------------------------
    // METHOD  (IDENT: GET | POST | HEAD)
    // -------------------------------
    if (i < tokens.size() && tokens[i].base == BaseToken::IDENT) {
        req.method = tokens[i].lexeme;   // "GET", "POST", ...
        i++;
    } else {
        throw std::runtime_error("Missing HTTP method");
    }

    // Skip spaces
    while (i < tokens.size() && tokens[i].base == BaseToken::SP)
        i++;

    // -------------------------------
    // URI (must start with '/')
    // -------------------------------
    if (i < tokens.size() && tokens[i].base == BaseToken::SLASH) {
        std::string uri = "/";
        i++;

        while (i < tokens.size()) {
            if (tokens[i].base == BaseToken::IDENT ||
                tokens[i].base == BaseToken::SLASH ||
                tokens[i].base == BaseToken::DOT)
            {
                uri += tokens[i].lexeme;
                i++;
            }
            else break;
        }

        req.uri = uri;
    } else {
        throw std::runtime_error("Expected URI after method");
    }

    // Skip spaces
    while (i < tokens.size() && tokens[i].base == BaseToken::SP)
        i++;

    // -------------------------------
    // VERSION (IDENT: must be "HTTP/1.0")
    // -------------------------------
    if (i < tokens.size() &&
        tokens[i].base == BaseToken::IDENT &&
        tokens[i].lexeme.rfind("HTTP/", 0) == 0)
    {
        req.version = tokens[i].lexeme;
        i++;
    }
    else {
        throw std::runtime_error("Missing or invalid HTTP version");
    }

    // Expect CRLF
    if (i < tokens.size() && tokens[i].base == BaseToken::CRLF)
        i++;
    else
        throw std::runtime_error("Expected CRLF after request line");

    // -------------------------------
    // HEADERS
    // -------------------------------
    while (i < tokens.size() && tokens[i].base == BaseToken::IDENT)
    {
        HTTPHeader h;
        h.name = tokens[i].lexeme;
        i++;

        // Expect colon
        if (i >= tokens.size() || tokens[i].base != BaseToken::COLON)
            throw std::runtime_error("Expected ':' after header name");
        i++;

        // Optional space
        if (i < tokens.size() && tokens[i].base == BaseToken::SP)
            i++;

        // Header value
        std::string value;
        while (i < tokens.size() && tokens[i].base != BaseToken::CRLF) {
            value += tokens[i].lexeme;
            i++;
        }

        // Empty value is allowed syntax-wise (HTTP/1.0 permits it)
        h.value = value;

        // End-of-header CRLF
        if (i < tokens.size() && tokens[i].base == BaseToken::CRLF)
            i++;
        else
            throw std::runtime_error("Expected CRLF after header value");

        req.headers.push_back(std::move(h));
    }

    return req;
}
