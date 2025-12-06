#ifndef CFG_CYK_H
#define CFG_CYK_H

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include "json.hpp"


struct Derivation {
    bool isTerminal;        // Is dit een A -> a regel?
    std::string leftChild;  // Voor A -> BC: dit is B
    std::string rightChild; // Voor A -> BC: dit is C
    int splitPoint;         // De index k waar de string gesplitst is
    std::string ruleStr;    // Tekstuele representatie (voor debug/print)

    // Constructor voor Terminal (A -> a)
    Derivation(std::string val)
        : isTerminal(true), splitPoint(-1), ruleStr(val) {}

    // Constructor voor Binair (A -> B C)
    Derivation(std::string left, std::string right, int k, std::string r)
        : isTerminal(false), leftChild(left), rightChild(right), splitPoint(k), ruleStr(r) {}
};

class CFG
{
private:
    std::vector<std::string> V;
    std::vector<std::string> T;
    std::vector<std::pair<std::string, std::string>> P;
    std::string S;

    std::map<std::string, std::set<std::string>> terminalRules;
    std::map<std::pair<std::string, std::string>, std::set<std::string>> binaryRules;


    // Tabel[i][j] bevat een map: Variabele -> Lijst van manieren om die te maken
    using ParseForest = std::vector<std::vector<std::map<std::string, std::vector<Derivation>>>>;

    // Recursieve helper om bomen te genereren
    void getParseTrees(
        const ParseForest& forest,
        const std::string& var,
        int i,
        int j,
        std::vector<std::string>& results,
        int limit);

public:
    CFG(const std::string& filename);
    void accepts(const std::string& w); // Oude methode, mag blijven staan

    // De hoofdfunctie die nu ook ambiguïteit checkt
    void analyze(const std::string& w);
};

#endif