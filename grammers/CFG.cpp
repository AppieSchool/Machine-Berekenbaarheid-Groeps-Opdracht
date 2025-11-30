//
// Created by abdir on 05/10/2025.
//

#include "CFG.h"

#include <utility>


production::production(std::string lhs, std::vector<std::string> body)
        : lhs(std::move(lhs)), body(std::move(body)) {}

void production::print() const {
    std::cout << lhs << " -> `";
    for (int i = 0; i < body.size(); ++i) {
        std::cout << body[i];
        if (i != body.size() - 1) std::cout << " ";
    }
    std::cout << "`" << std::endl;
}

CFG::CFG(std::string jsonFile) : filename(std::move(jsonFile)) {
    std::ifstream input(filename);
    if (!input.is_open()) {
        std::cerr << "Error: Unable to open file." << std::endl;
        return;
    }

    // Read the JSON content
    json j;
    input >> j;

    readJsonObject(j);
}

CFG::CFG(json &jsonObj) {
    readJsonObject(jsonObj);
}

void CFG::readJsonObject(json &jsonObj) {
    auto variablesJson = jsonObj["Variables"];
    auto terminalsJson = jsonObj["Terminals"];
    auto productionsJson = jsonObj["Productions"];
    auto startJson = jsonObj["Start"];

    // Parse variables
    for (const auto &var : variablesJson) {
        variables.push_back(var);
    }
    // Parse terminals
    for (const auto & term : terminalsJson) {
        terminals.push_back(term);
    }

    // Parse productions
    for (const auto &prod : productionsJson) {
        std::string lhs = prod["head"];
        std::string rhs;

        // Convert the body array to a space-separated string
        std::vector<std::string> body;
        for (const auto &symbol : prod["body"]) {
            body.push_back(symbol);
        }
        productions.emplace_back(lhs, body);
    }

    // Parse start symbol
    startSymbol = startJson;


}

void CFG::print() {
    std::sort(variables.begin(), variables.end());
    std::sort(terminals.begin(), terminals.end());

    std::cout << "V = {";
    for (int i = 0; i < variables.size(); i++) {
        std::cout << variables[i];
        if (i != variables.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    std::cout << "T = {";
    for (int i = 0; i < terminals.size(); i++) {
        std::cout << terminals[i];
        if (i != terminals.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "}" << std::endl;

    // Sort productions in ASCII order
    std::sort(productions.begin(), productions.end(),
              [](const production &a, const production &b) {
                  if (a.lhs == b.lhs) {
                      // turn vector<string> into one string
                      std::string rhsA, rhsB;
                      for (const auto &sym : a.body) rhsA += sym + " ";
                      for (const auto &sym : b.body) rhsB += sym + " ";
                      return rhsA < rhsB;
                  }
                  return a.lhs < b.lhs;
              });


    std::cout << "P = {" << std::endl;
    for (production &prod : productions) {
        std::cout << "      ";
        prod.print();
    }
    std::cout << "}" << std::endl;

    std::cout << "S = " << startSymbol << std::endl;
}


void CFG::ll() {
    json result;

    json firstJson;
    for (const auto &var : variables) {
        firstJson[var] = firstSet(var);
    }

    json followJson;
    for (const auto &var : variables) {
        followJson[var] = followSet(var);
    }

    result["first"] = firstJson;
    result["follow"] = followJson;

    auto ll1Table = buildLL1Table();

    result["ll1_table"] = ll1Table;

    std::cout << result.dump(4) << std::endl;

}

void CFG::setProductions(const std::vector<production> &prods) {CFG::productions = prods;}

void CFG::setVariables(const std::vector<std::string> &vars) {CFG::variables = vars;}

void CFG::setTerminals(const std::vector<std::string> &terms) {CFG::terminals = terms;}

void CFG::setStartSymbol(const std::string &startSym) { CFG::startSymbol = startSym;}

const std::vector<production> &CFG::getProductions() const { return productions; }

const std::vector<std::string> &CFG::getVariables() const { return variables;}

const std::vector<std::string> &CFG::getTerminals() const { return terminals; }

const std::string &CFG::getStartSymbol() const { return startSymbol;}

std::vector<std::string> CFG::firstSet(const std::string& symbol) {
    if(std::find(terminals.begin(), terminals.end(), symbol) != terminals.end()) {
        return {symbol};
    }
    std::vector<std::string> result;
    for(const auto &prod : productions) {
        if(prod.lhs == symbol) {
            if(prod.body.empty()) {
                result.push_back("");
            } else {
                // Left most-symbol
                const std::string& firstSymbol = prod.body[0];
                // if it's a terminal, add it to the result
                if(std::find(terminals.begin(), terminals.end(), firstSymbol) != terminals.end()) {
                    if(std::find(result.begin(), result.end(), firstSymbol) == result.end()) {
                        result.push_back(firstSymbol);
                    }
                } else {
                    // if it's a variable, recursively get its first set
                    std::vector<std::string> firstSetOfFirstSymbol = firstSet(firstSymbol);
                    for (const auto &sym: firstSetOfFirstSymbol) {
                        if (std::find(result.begin(), result.end(), sym) == result.end()) {
                            result.push_back(sym);
                        }
                    }
                }

            }
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<std::string> CFG::followSet(const std::string& symbol) {
    std::vector<std::string> result;

    // Add <EOS> if the symbol is the start symbol
    if (symbol == startSymbol) {
        result.push_back("<EOS>");
    }

    for (const auto& prod : productions) {
        for (size_t i = 0; i < prod.body.size(); ++i) {
            if (prod.body[i] == symbol) {
                // Case 1: Symbol is followed by other symbols in the production
                if (i + 1 < prod.body.size()) {
                    const std::string& nextSymbol = prod.body[i + 1];

                    if (std::find(variables.begin(), variables.end(), nextSymbol) == variables.end()) {
                        // If the next symbol is a terminal, add it to the result
                        result.push_back(nextSymbol);
                    } else {
                        // If the next symbol is a variable, add its FIRST set
                        std::vector<std::string> firstSetOfNext = firstSet(nextSymbol);
                        for (const auto& sym : firstSetOfNext) {
                            if (sym != "" && std::find(result.begin(), result.end(), sym) == result.end()) {
                                result.push_back(sym);
                            }
                        }

                        // If epsilon is in the FIRST set, continue to the next symbol
                        if (std::find(firstSetOfNext.begin(), firstSetOfNext.end(), "") != firstSetOfNext.end()) {
                            continue;
                        }
                    }
                }

                // Case 2: Symbol is at the end of the production
                if (i + 1 == prod.body.size()) {
                    std::vector<std::string> followSetOfLHS = followSet(prod.lhs);
                    for (const auto& sym : followSetOfLHS) {
                        if (std::find(result.begin(), result.end(), sym) == result.end()) {
                            result.push_back(sym);
                        }
                    }
                }
            }
        }
    }

    std::sort(result.begin(), result.end());

    return result;
}

std::string CFG::stringifyBody(const std::vector<std::string> &body) {
    if (body.empty()) return "";
    std::string s;
    for (size_t i = 0; i < body.size(); ++i) {
        s += body[i];
        if (i + 1 < body.size()) s += " ";
    }
    return s;
}

void CFG::printLL1Table(const std::map<std::string, std::map<std::string, std::string>> &table,
                        const std::vector<std::string> &headers) {
    // Header row
    std::cout << "     |";
    for (const auto &t : headers)
        std::cout << " " << std::setw(8) << std::left << t << "|";
    std::cout << "\n|----|";
    for (size_t i = 0; i < headers.size(); ++i) std::cout << "----------|";
    std::cout << "\n";

    // Rows for each variable
    for (const auto &A : variables) {
        std::cout << "| " << std::setw(3) << std::left << A << " |";
        for (const auto &t : headers) {
            std::string val = table.at(A).count(t) ? table.at(A).at(t) : "";
            if (!val.empty()) val = "`" + val + "`";
            std::cout << " " << std::setw(8) << std::left << val << "|";
        }
        std::cout << "\n";
    }

    std::cout << "|----|";
    for (size_t i = 0; i < headers.size(); ++i) std::cout << "----------|";
    std::cout << "\n";
}

json CFG::buildLL1Table() {
    std::map<std::string, std::map<std::string, std::vector<std::string>>> table;
    std::vector<std::string> headers = terminals;
    headers.push_back("<EOS>");

    // Build table based on FIRST/FOLLOW
    for (const auto& p : productions) {
        std::string A = p.lhs;
        std::vector<std::string> alpha = p.body;

        // Get FIRST(alpha)
        std::vector<std::string> firstAlpha;
        // Flag to indicate if we should continue to the next symbol
        bool continueToNext = true;

        for (int i = 0; i < alpha.size() && continueToNext; ++i) {
            const std::string& currentSymbol = alpha[i];

            if (std::find(terminals.begin(), terminals.end(), currentSymbol) != terminals.end()) {
                // Terminal
                firstAlpha.push_back(currentSymbol);
                continueToNext = false;
            } else {
                // Non-terminal
                std::vector<std::string> firstSetOfCurrent = firstSet(currentSymbol);
                bool hasEpsilon = false;

                for (const auto& sym : firstSetOfCurrent) {
                    if (sym == "") {
                        hasEpsilon = true;
                    } else {
                        firstAlpha.push_back(sym);
                    }
                }
                continueToNext = hasEpsilon;
            }
        }

        // If all symbols can derive epsilon
        bool hasEpsilon = continueToNext;

        // add production to table
        for (const auto& terminal : firstAlpha) {
            table[A][terminal] = alpha;
        }

        // add to FOLLOW if epsilon in FIRST
        if (hasEpsilon) {
            std::vector<std::string> followA = followSet(A);
            for (const auto& terminal : followA) {
                // Don't overwrite existing entries
                if (!table[A].count(terminal)) {
                    if (alpha.empty()) {
                        table[A][terminal] = {""};
                    } else {
                        table[A][terminal] = alpha;
                    }
                }else{

                }
            }
        }
    }

    // Fill empty cells with <ERR>
    for (const auto& A : variables)
        for (const auto& t : headers)
            if (!table[A].count(t)){
                table[A][t] = {"<ERR>"};
            }

    json tableJson;

    for (const auto &A : variables) {
        json rowJson;
        for (const auto &t : terminals) {
            rowJson[t] = table[A][t];
        }
        rowJson["<EOS>"] = table[A]["<EOS>"];
        tableJson[A] = rowJson;
    }
    return tableJson;
}