#include "HTTP10MessageGenerator.h"
#include <sstream>
#include <cstdlib>

//
// --- URI builder ---
//
std::string HTTP10MessageGenerator::buildURI(const HTTP10MessageOptions& opt, bool valid) {
    std::stringstream ss;

    if (valid) {
        ss << "/";

        bool first = true;

        for (const auto& seg : opt.path) {
            // Skip empty segments or segments containing illegal characters
            if (seg.empty()) continue;
            if (seg.find(' ') != std::string::npos) continue;
            if (seg.find('/') != std::string::npos) continue;

            if (!first)
                ss << "/";

            ss << seg;
            first = false;
        }

        // Only add extension if we already have a filename
        if (!opt.extension.empty() && !first) {
            ss << "." << opt.extension;
        }

        // If no path segments were valid → default to "/index"
        if (first) {
            ss.str("");
            ss << "/";
            if (!opt.extension.empty())
                ss << "index." << opt.extension;
            else
                ss << "index";
        }

        return ss.str();
    }

    // INVALID CASES BELOW
    int r = rand() % 4;
    switch (r) {
        case 0: return "index..html";
        case 1: return "/bad path/file";
        case 2: return "noslash.html";
        case 3: return "/";
    }
    return "/invalid";
}


//
// --- HEADER builder ---
//
std::string HTTP10MessageGenerator::buildHeader(const HeaderOption& h, bool overallInvalid) {
    std::stringstream ss;

    bool breakHeader = h.forceInvalid || overallInvalid;

    if (!breakHeader) {
        // valid header
        ss << h.name << ": " << h.value << "\r\n";
        return ss.str();
    }

    // INVALID HEADER cases
    int r = rand() % 4;
    switch (r) {
        case 0: return h.name + " " + h.value + "\r\n";                // missing colon
        case 1: return h.name + ":: " + h.value + "\r\n";              // double colon
        case 2: return h.name + ": \r\n";                              // missing value
        case 3: return h.name + "@!: " + h.value + "\r\n";             // illegal chars
    }

    return h.name + ": broken\r\n";
}

//
// --- Build VALID message ---
//
std::string HTTP10MessageGenerator::buildValid(const HTTP10MessageOptions& opt) {
    std::stringstream msg;

    // Request-Line
    msg << opt.method << " "
        << buildURI(opt, true) << " "
        << "HTTP/1.0\r\n";

    // Headers
    for (auto& h : opt.headers) {
        if (!h.enabled) continue;
        msg << buildHeader(h, false);
    }

    msg << "\r\n";
    return msg.str();
}

//
// --- Build INVALID message ---
//
std::string HTTP10MessageGenerator::buildInvalid(const HTTP10MessageOptions& opt) {
    std::stringstream msg;

    // Randomly break method or version
    int r = rand() % 5;

    std::string method = opt.method;
    std::string version = "HTTP/1.0";

    if (r == 0) method = "GEX";       // invalid method
    if (r == 1) version = "HTTP/2.0"; // wrong version

    msg << method << " "
        << buildURI(opt, false) << " "
        << version << "\r\n";

    // Headers (may also be broken)
    for (auto& h : opt.headers) {
        if (!h.enabled) continue;
        msg << buildHeader(h, true);
    }

    // Sometimes miss the final CRLF
    if (rand() % 2 == 0)
        msg << "\r\n";

    return msg.str();
}

//
// --- Main entry ---
//
std::string HTTP10MessageGenerator::generate(const HTTP10MessageOptions& opt) {
    if (opt.validity == HTTP10ExampleKind::Valid)
        return buildValid(opt);
    return buildInvalid(opt);
}
