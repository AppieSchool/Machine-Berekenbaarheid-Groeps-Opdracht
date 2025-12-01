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

// FIXED: Improved firstSet with proper epsilon handling for sequences
std::vector<std::string> CFG::firstSet(const std::string& symbol) {
    // Check if we've already computed this
    if (firstCache.count(symbol)) {
        return firstCache[symbol];
    }

    std::vector<std::string> result;

    // Base case: if it's a terminal, return it
    if(std::find(terminals.begin(), terminals.end(), symbol) != terminals.end()) {
        result.push_back(symbol);
        firstCache[symbol] = result;
        return result;
    }

    // For variables, look at all productions
    for(const auto &prod : productions) {
        if(prod.lhs == symbol) {
            if(prod.body.empty()) {
                // Empty production means epsilon
                if(std::find(result.begin(), result.end(), "") == result.end()) {
                    result.push_back("");
                }
            } else {
                // Process the sequence of symbols
                bool allHaveEpsilon = true;
                for (size_t i = 0; i < prod.body.size(); ++i) {
                    const std::string& currentSymbol = prod.body[i];

                    if(std::find(terminals.begin(), terminals.end(), currentSymbol) != terminals.end()) {
                        // Terminal: add it and stop
                        if(std::find(result.begin(), result.end(), currentSymbol) == result.end()) {
                            result.push_back(currentSymbol);
                        }
                        allHaveEpsilon = false;
                        break;
                    } else {
                        // Variable: get its FIRST set
                        std::vector<std::string> firstSetOfCurrent = firstSet(currentSymbol);
                        bool hasEpsilon = false;

                        for (const auto &sym: firstSetOfCurrent) {
                            if (sym == "") {
                                hasEpsilon = true;
                            } else if (std::find(result.begin(), result.end(), sym) == result.end()) {
                                result.push_back(sym);
                            }
                        }

                        // If this symbol doesn't have epsilon, stop here
                        if (!hasEpsilon) {
                            allHaveEpsilon = false;
                            break;
                        }
                    }
                }

                // If all symbols in the sequence can derive epsilon, add epsilon
                if (allHaveEpsilon && std::find(result.begin(), result.end(), "") == result.end()) {
                    result.push_back("");
                }
            }
        }
    }

    std::sort(result.begin(), result.end());
    firstCache[symbol] = result;
    return result;
}

// FIXED: followSet with memoization and cycle detection
std::vector<std::string> CFG::followSet(const std::string& symbol) {
    // Check if we've already computed this
    if (followCache.count(symbol)) {
        return followCache[symbol];
    }

    // Check if we're currently computing this (cycle detection)
    if (followInProgress.count(symbol)) {
        // Return empty set to break the cycle
        return {};
    }

    // Mark that we're computing this symbol's FOLLOW set
    followInProgress.insert(symbol);

    std::vector<std::string> result;

    // Add <EOS> if the symbol is the start symbol
    if (symbol == startSymbol) {
        result.push_back("<EOS>");
    }

    for (const auto& prod : productions) {
        for (size_t i = 0; i < prod.body.size(); ++i) {
            if (prod.body[i] == symbol) {
                // Look at what follows this symbol
                bool allFollowingHaveEpsilon = true;

                for (size_t j = i + 1; j < prod.body.size(); ++j) {
                    const std::string& nextSymbol = prod.body[j];

                    if (std::find(terminals.begin(), terminals.end(), nextSymbol) != terminals.end()) {
                        // Terminal: add it and we're done
                        if (std::find(result.begin(), result.end(), nextSymbol) == result.end()) {
                            result.push_back(nextSymbol);
                        }
                        allFollowingHaveEpsilon = false;
                        break;
                    } else {
                        // Variable: add its FIRST set (except epsilon)
                        std::vector<std::string> firstSetOfNext = firstSet(nextSymbol);
                        bool hasEpsilon = false;

                        for (const auto& sym : firstSetOfNext) {
                            if (sym == "") {
                                hasEpsilon = true;
                            } else if (std::find(result.begin(), result.end(), sym) == result.end()) {
                                result.push_back(sym);
                            }
                        }

                        // If this symbol doesn't have epsilon, stop here
                        if (!hasEpsilon) {
                            allFollowingHaveEpsilon = false;
                            break;
                        }
                    }
                }

                // If at end OR all following symbols can derive epsilon,
                // add FOLLOW(lhs) unless lhs == symbol (to avoid immediate recursion)
                if (allFollowingHaveEpsilon && prod.lhs != symbol) {
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

    // Remove from in-progress set and cache the result
    followInProgress.erase(symbol);
    followCache[symbol] = result;

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
    // Clear caches before building table
    firstCache.clear();
    followCache.clear();
    followInProgress.clear();

    std::map<std::string, std::map<std::string, std::vector<std::string>>> table;
    std::vector<std::string> headers = terminals;
    headers.push_back("<EOS>");

    // Build table based on FIRST/FOLLOW
    for (const auto& p : productions) {
        std::string A = p.lhs;
        std::vector<std::string> alpha = p.body;

        // Get FIRST(alpha) - compute for the entire sequence
        std::vector<std::string> firstAlpha;
        bool continueToNext = true;

        for (size_t i = 0; i < alpha.size() && continueToNext; ++i) {
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

        // If all symbols can derive epsilon (or alpha is empty)
        bool hasEpsilon = continueToNext;

        // Add production to table for each terminal in FIRST(alpha)
        for (const auto& terminal : firstAlpha) {
            if (table[A].count(terminal)) {
                // CONFLICT DETECTED!
                std::cerr << "WARNING: LL(1) conflict detected for [" << A << ", " << terminal << "]" << std::endl;
                std::cerr << "  Existing: " << A << " -> ";
                for (const auto& s : table[A][terminal]) std::cerr << s << " ";
                std::cerr << std::endl << "  New:      " << A << " -> ";
                for (const auto& s : alpha) std::cerr << s << " ";
                std::cerr << std::endl;
            } else {
                table[A][terminal] = alpha;
            }
        }

        // If epsilon in FIRST(alpha), add to FOLLOW(A)
        if (hasEpsilon) {
            std::vector<std::string> followA = followSet(A);
            for (const auto& terminal : followA) {
                if (table[A].count(terminal)) {
                    // Only report conflict if it's a different production
                    bool isDifferent = (table[A][terminal].size() != alpha.size());
                    if (!isDifferent) {
                        for (size_t i = 0; i < alpha.size(); ++i) {
                            if (table[A][terminal][i] != alpha[i]) {
                                isDifferent = true;
                                break;
                            }
                        }
                    }
                    if (isDifferent) {
                        std::cerr << "WARNING: LL(1) conflict detected for [" << A << ", " << terminal << "]" << std::endl;
                    }
                } else {
                    table[A][terminal] = alpha.empty() ? std::vector<std::string>{""} : alpha;
                }
            }
        }
    }

    // Fill empty cells with <ERR>
    for (const auto& A : variables) {
        for (const auto& t : headers) {
            if (!table[A].count(t)) {
                table[A][t] = {"<ERR>"};
            }
        }
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

void CFG::addProduction(const production &prod) {
    productions.push_back(prod);
    // Clear caches when grammar changes
    firstCache.clear();
    followCache.clear();
    followInProgress.clear();
}