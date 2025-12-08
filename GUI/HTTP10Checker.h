// gui/HTTP10Checker.h
#pragma once

#include <string>
#include <vector>

#include "../grammers/CFG.h"   // because DiagnosticInfo lives there

struct ProtocolCheckResult {
    bool syntaxOk = false;
    bool semanticsOk = false;

    std::string syntaxMessage;
    std::string semanticsMessage;

    bool hasParseTree = false;
    std::string parseTreePath;

    std::string logText;
    std::vector<std::string> tokens;

    DiagnosticInfo diagnostic;    // <<< REQUIRED
};


bool runHTTP10Check(const std::string& input, ProtocolCheckResult& outResult);
