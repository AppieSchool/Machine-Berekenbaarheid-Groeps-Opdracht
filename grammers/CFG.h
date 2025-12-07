//
// Created by abdir on 05/10/2025.
//

#ifndef MB_CFG_H
#define MB_CFG_H
#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <fstream>
#include <set>
#include <iostream>
#include <iomanip>
#include "../utils/json.hpp"
using json = nlohmann::json;

class production {
public:
    std::string lhs;
    std::vector<std::string> body;
    production(std::string lhs, std::vector<std::string> body);
    void print() const;
};



class CFG {
public:
    void print();
    void ll();

    void addProduction(const production &prod);

    void setProductions(const std::vector<production> &prods);

    void setVariables(const std::vector<std::string> &vars);

    void setTerminals(const std::vector<std::string> &terms);

    void setStartSymbol(const std::string &startSym);

    void printLL1Diagnostics(int state, const std::string& lookahead);

    void printExpectedTerminals(const std::vector<std::string>& expected, const std::string& got);

    std::vector<std::string> expectedFromLL1(const std::string& nonterminal);

    [[nodiscard]] const std::vector<production> &getProductions() const;

    [[nodiscard]] const std::vector<std::string> &getVariables() const;

    [[nodiscard]] const std::vector<std::string> &getTerminals() const;

    [[nodiscard]] const std::string &getStartSymbol() const;

    std::vector<std::string> firstSet(const std::string& symbol);

    std::vector<std::string> followSet(const std::string& symbol);

    explicit CFG(std::string jsonFile);
    CFG(json &jsonObj);
    CFG() = default;
    ~CFG() = default;
private:
    std::vector<production> productions;
    std::vector<std::string> variables;
    std::vector<std::string> terminals;
    std::string startSymbol;
    std::string filename;

    std::string stringifyBody(const std::vector<std::string> &body);
    void printLL1Table(const std::map<std::string, std::map<std::string, std::string>> &table,
                       const std::vector<std::string> &headers);

    json buildLL1Table();
    std::map<std::string, std::vector<std::string>> firstCache;
    std::map<std::string, std::vector<std::string>> followCache;
    std::set<std::string> followInProgress;
    void readJsonObject(json &jsonObj);
};


#endif //MB_CFG_H