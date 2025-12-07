#ifndef HTTP10_SEMANTICS_H
#define HTTP10_SEMANTICS_H

#include <string>
#include <vector>

#include "HTTPRequest.h"
#include "../SemanticResult.h"

class HTTP10_semantics {
public:
    // Validate entire request
    static SemanticResult validateRequest(const HTTPRequest& req);

private:
    // Individual semantic checks
    static SemanticResult validateMethod(const HTTPRequest& req);
    static SemanticResult validateURI(const HTTPRequest& req);
    static SemanticResult validateVersion(const HTTPRequest& req);
    static SemanticResult validateHeaders(const HTTPRequest& req);

    // Helper utility
    static bool isAscii(const std::string& s);
};

#endif
