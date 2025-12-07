#ifndef HTTP10_HTTPREQUEST_H
#define HTTP10_HTTPREQUEST_H

#include <string>
#include <vector>
#include "../Token.h"

struct HTTPHeader {
    std::string name;
    std::string value;
};

struct HTTPRequest {
    std::string method;
    std::string uri;
    std::string version;

    std::vector<HTTPHeader> headers;

    static HTTPRequest fromTokens(const std::vector<Token>& tokens);
};

#endif
