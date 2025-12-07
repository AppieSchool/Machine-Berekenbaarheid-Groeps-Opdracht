#include "HTTP10_semantics.h"
#include <regex>

SemanticResult HTTP10_semantics::validateRequest(const HTTPRequest& req) {
    SemanticResult r;

    r = validateMethod(req);
    if (!r) return r;

    r = validateURI(req);
    if (!r) return r;

    r = validateVersion(req);
    if (!r) return r;

    r = validateHeaders(req);
    if (!r) return r;

    return SemanticResult::success();
}

SemanticResult HTTP10_semantics::validateMethod(const HTTPRequest& req) {
    if (req.method != "GET" &&
        req.method != "POST" &&
        req.method != "HEAD")
        return SemanticResult::failure(
            "Invalid HTTP method: " + req.method,
            "invalid-method"
        );

    return SemanticResult::success();
}

SemanticResult HTTP10_semantics::validateURI(const HTTPRequest& req) {
    if (req.uri.empty())
        return SemanticResult::failure(
            "Missing URI in request",
            "missing-uri"
        );

    if (req.uri[0] != '/')
        return SemanticResult::failure(
            "URI must begin with '/'",
            "invalid-uri"
        );

    return SemanticResult::success();
}

SemanticResult HTTP10_semantics::validateVersion(const HTTPRequest& req) {
    if (req.version != "HTTP/1.0")
        return SemanticResult::failure(
            "Unsupported HTTP version: " + req.version,
            "invalid-version"
        );

    return SemanticResult::success();
}

SemanticResult HTTP10_semantics::validateHeaders(const HTTPRequest& req) {
    for (const auto& h : req.headers) {
        if (h.name.empty())
            return SemanticResult::failure(
                "Header name is empty",
                "empty-header-name"
            );

        if (h.value.empty())
            return SemanticResult::failure(
                "Header value for '" + h.name + "' is empty",
                "empty-header-value"
            );

        if (!isAscii(h.value))
            return SemanticResult::failure(
                "Header value contains non-ASCII characters",
                "invalid-header-value"
            );
    }

    return SemanticResult::success();
}

bool HTTP10_semantics::isAscii(const std::string& s) {
    for (unsigned char c : s)
        if (c < 32 || c > 126)
            return false;
    return true;
}

static SemanticResult validateRequest(const HTTPRequest& req)
{
    // method
    if (req.method != "GET" &&
        req.method != "POST" &&
        req.method != "HEAD") {
        return SemanticResult::failure("Unsupported method", "invalid-method");
    }

    // version
    if (req.version != "HTTP/1.0") {
        return SemanticResult::failure("Invalid HTTP version", "invalid-version");
    }

    // headers
    for (const auto& h : req.headers) {
        if (h.name.empty())
            return SemanticResult::failure("Header name empty", "header-empty-name");

        for (char c : h.value)
            if ((unsigned)c < 32)
                return SemanticResult::failure("Header contains control chars", "header-control-char");
    }

    return SemanticResult::success();
}
