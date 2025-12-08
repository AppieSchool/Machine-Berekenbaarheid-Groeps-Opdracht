#ifndef HTTP10_MESSAGE_GENERATOR_H
#define HTTP10_MESSAGE_GENERATOR_H

#include <string>
#include "HTTP10MessageOptions.h"

class HTTP10MessageGenerator {
public:
    static std::string generate(const HTTP10MessageOptions& opt);

private:
    static std::string buildValid(const HTTP10MessageOptions& opt);
    static std::string buildInvalid(const HTTP10MessageOptions& opt);

    static std::string buildURI(const HTTP10MessageOptions& opt, bool valid);
    static std::string buildHeader(const HeaderOption& h, bool overallInvalid);
};

#endif
