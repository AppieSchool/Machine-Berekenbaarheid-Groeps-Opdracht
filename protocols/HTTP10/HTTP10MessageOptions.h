#ifndef HTTP10_MESSAGE_OPTIONS_H
#define HTTP10_MESSAGE_OPTIONS_H

#include <string>
#include <vector>

enum class HTTP10ExampleKind {
    Valid,
    Invalid
};

// describes a single header to include
struct HeaderOption {
    std::string name;
    std::string value;
    bool enabled = false;
    bool forceInvalid = false;   // if true → generator purposely breaks this header
};

struct HTTP10MessageOptions {
    std::string method = "GET";      // GET / POST / HEAD
    std::vector<std::string> path;   // e.g. {"images", "logo"}
    std::string extension = "html";  // e.g. html, png, txt

    std::vector<HeaderOption> headers;

    HTTP10ExampleKind validity = HTTP10ExampleKind::Valid;
};

#endif
