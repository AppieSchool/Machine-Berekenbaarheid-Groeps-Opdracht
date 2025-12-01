//
// Created by Skip on 01/12/2025.
//

#ifndef HTTP10_TESTS_H
#define HTTP10_TESTS_H

#include <string>

class HTTP10Tests {
public:
    static void runAll();
    static bool runSingle(const std::string& filename);
};

#endif
