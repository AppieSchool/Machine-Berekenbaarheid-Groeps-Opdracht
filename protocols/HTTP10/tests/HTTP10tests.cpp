#include "HTTP10tests.h"
#include "../protocols/HTTP10/HTTP10Protocol.h"
#include <fstream>
#include <iostream>
#include <filesystem>

std::string loadFile(const std::string& path) {
    std::string abs = std::filesystem::absolute(path).string();

    std::ifstream file(abs);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Cannot open: " << abs << "\n";
        return "";
    }

    return std::string((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
}



bool HTTP10Tests::runSingle(const std::string& filename) {
    std::cout << "\n=== TEST: " << filename << " ===\n";

    HTTP10Protocol protocol;
    std::string input = loadFile(filename);

    if (input.empty()) {
        std::cout << "[FAIL] Empty or unreadable file.\n";
        return false;
    }

    auto tokens = protocol.tokenize(input);

    std::cout << "[TOKENS]\n";
    for (auto& t : tokens) {
        std::cout << "base=" << (int)t.base
                  << ", subtype=" << t.subtype
                  << ", lex='" << t.lexeme << "'\n";
    }

    CFG grammar = protocol.getCFG();
    std::cout << "[GRAMMAR] Loaded.\n";

    return true;
}

void HTTP10Tests::runAll() {
    static const char* cases[] = {
        "protocols/HTTP10/cases/valid_request_1.txt",
        "protocols/HTTP10/cases/valid_request_2.txt",
        "protocols/HTTP10/cases/invalid_header_format.txt",
        "protocols/HTTP10/cases/missing_method.txt",
        "protocols/HTTP10/cases/missing_uri.txt",
        "protocols/HTTP10/cases/invalid_version.txt",
        "protocols/HTTP10/cases/missing_crlf.txt",
        "protocols/HTTP10/cases/empty_header_value.txt",
    };



    for (auto f : cases) {
        runSingle(f);
    }
}
