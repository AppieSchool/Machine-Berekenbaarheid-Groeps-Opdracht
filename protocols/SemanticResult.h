#ifndef SEMANTICRESULT_H
#define SEMANTICRESULT_H

#include <string>

struct SemanticResult {
    bool ok;
    std::string message;
    std::string code;  // semantic error code, e.g. "empty-header-value"

    operator bool() const { return ok; }

    static SemanticResult success() {
        return {true, "", ""};
    }

    static SemanticResult failure(const std::string& msg,
                                  const std::string& code = "") {
        return {false, msg, code};
    }
};

#endif
