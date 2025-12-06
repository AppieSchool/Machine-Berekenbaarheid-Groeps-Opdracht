#ifndef MACHINE_BEREKENBAARHEID_GROEPS_OPDRACHT_DOTGENERATOR_H
#define MACHINE_BEREKENBAARHEID_GROEPS_OPDRACHT_DOTGENERATOR_H

#include "ParseTree.h"
#include <string>
#include <sstream>

using namespace std;

class DotGenerator {
public:
    static string generate(const ParseTree& root);
    static void saveToFile(const ParseTree& root, const string& filename);
    static void generateImage(const ParseTree& root, const string& pngFilename);

private:
    static void generateHelper(const ParseTree& node, ostringstream& out, int& nodeCounter);
};

#endif
