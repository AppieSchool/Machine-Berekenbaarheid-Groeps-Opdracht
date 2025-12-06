#include "CFG_CYK.h"
#include <fstream>
#include <algorithm>

CFG::CFG(const std::string& filename) {
    std::ifstream input(filename);
    nlohmann::json j;
    input >> j;

    V = j["Variables"].get<std::vector<std::string>>();
    T = j["Terminals"].get<std::vector<std::string>>();

    for (const auto& production : j["Productions"]) {
        std::string head = production["head"].get<std::string>();
        std::vector<std::string> body = production["body"].get<std::vector<std::string>>();

        std::string bodyStr = "";
        for (size_t i = 0; i < body.size(); i++) {
            bodyStr += body[i];
            if (i < body.size() - 1) bodyStr += " ";
        }

        P.push_back({head, bodyStr});
    }
    S = j["Start"].get<std::string>();
    // Sorteren voor nette interne opslag
    std::sort(P.begin(), P.end(), [](const std::pair<std::string, std::string>& a,
                                      const std::pair<std::string, std::string>& b) {
        std::string strA = a.first + " -> `" + a.second + "`";
        std::string strB = b.first + " -> `" + b.second + "`";
        return strA < strB;
    });

    // Build terminal rules map
    for (const auto& terminal : T) {
        for (const auto& production : P) {
            if (production.second == terminal && production.second.length() == 1) {
                terminalRules[terminal].insert(production.first);
            }
        }
    }

    // Build binary rules map (A -> B C)
    for (const auto& production : P) {
        std::string body = production.second;
        size_t spacePos = body.find(' ');

        if (spacePos != std::string::npos && spacePos == body.rfind(' ')) {
            std::string first = body.substr(0, spacePos);
            std::string second = body.substr(spacePos + 1);

            bool firstIsVar = std::find(V.begin(), V.end(), first) != V.end();
            bool secondIsVar = std::find(V.begin(), V.end(), second) != V.end();

            if (firstIsVar && secondIsVar) {
                binaryRules[{first, second}].insert(production.first);
            }
        }
    }
}

void CFG::accepts(const std::string& w) {
    int n = w.length();
    std::vector<std::vector<std::set<std::string>>> P_table(n, std::vector<std::set<std::string>>(n));

    // Fill diagonal with terminal productions
    for (int i = 0; i < n; ++i) {
        std::string terminal(1, w[i]);
        if (terminalRules.find(terminal) != terminalRules.end()) {
            P_table[i][i] = terminalRules[terminal];
        }
    }

    // Fill table using CYK algorithm
    for (int l = 2; l <= n; ++l) {
        for (int i = 0; i <= n - l; ++i) {
            int j = i + l - 1;
            for (int k = i; k < j; ++k) {
                for (const auto& B : P_table[i][k]) {
                    for (const auto& C : P_table[k + 1][j]) {
                        auto ruleKey = std::make_pair(B, C);
                        if (binaryRules.find(ruleKey) != binaryRules.end()) {
                            for (const auto& A : binaryRules[ruleKey]) {
                                P_table[i][j].insert(A);
                            }
                        }
                    }
                }
            }
        }
    }

    // Print CYK table from top to bottom
    for (int length = n; length >= 1; --length) {
        std::cout << "| ";
        for (int start = 0; start <= n - length; ++start) {
            int end = start + length - 1;

            // Build the cell content
            std::string cellContent = "{";
            bool first = true;
            std::vector<std::string> vars(P_table[start][end].begin(),
                                          P_table[start][end].end());
            std::sort(vars.begin(), vars.end());
            for (const auto& var : vars) {
                if (!first) cellContent += ", ";
                cellContent += var;
                first = false;
            }
            cellContent += "}";

            // Print with padding to make each cell 12 characters wide
            std::cout << cellContent;
            for (size_t i = cellContent.length(); i < 12; ++i) {
                std::cout << " ";
            }

            if (start < n - length) std::cout << "| ";
        }
        std::cout << "|" << std::endl;
    }

    // Check if start symbol is in top-right cell
    if (P_table[0][n - 1].find(S) != P_table[0][n - 1].end()) {
        std::cout << "true" << std::endl;
    } else {
        std::cout << "false" << std::endl;
    }
}

// int main() {
//     CFG cfg("input-cyk1.json");
//     cfg.accepts("baaba"); // moet true geven
//     cfg.accepts("abba");  // moet false geven
//     return 0;
// }

// Recursieve functie om bomen te bouwen (Bracket Notation)
// Stopt zodra 'limit' (bijv. 2) bereikt is
void CFG::getParseTrees(const ParseForest& forest, const std::string& var, int i, int j, std::vector<std::string>& results, int limit) {
    if (results.size() >= limit) return;

    // Check of er uberhaupt data is voor deze cel
    const auto& cell = forest[i][j];
    if (cell.find(var) == cell.end()) return;

    const std::vector<Derivation>& derivs = cell.at(var);

    for (const auto& d : derivs) {
        if (results.size() >= limit) break;

        if (d.isTerminal) {
            // Basis: (A a)
            results.push_back("(" + var + " " + d.ruleStr + ")");
        } else {
            // Recursie: A -> BC
            std::vector<std::string> leftTrees, rightTrees;

            getParseTrees(forest, d.leftChild, i, d.splitPoint, leftTrees, limit);
            getParseTrees(forest, d.rightChild, d.splitPoint + 1, j, rightTrees, limit);

            // Combineer resultaten (Cartesisch product)
            for (const auto& l : leftTrees) {
                for (const auto& r : rightTrees) {
                    if (results.size() >= limit) break;
                    results.push_back("(" + var + " " + l + " " + r + ")");
                }
            }
        }
    }
}

void CFG::analyze(const std::string& w) {
    int n = w.length();
    if (n == 0) return;

    // Initialiseer Parse Forest (3D structuur: i, j, map<Var, Derivations>)
    ParseForest forest(n, std::vector<std::map<std::string, std::vector<Derivation>>>(n));

    // 1. Initialisatie (Terminals)
    for (int i = 0; i < n; ++i) {
        std::string terminal(1, w[i]);
        if (terminalRules.find(terminal) != terminalRules.end()) {
            for (const auto& var : terminalRules[terminal]) {
                // Sla op: Variabele 'var' komt van terminal w[i]
                forest[i][i][var].push_back(Derivation(terminal));
            }
        }
    }

    // 2. CYK Loop (Bottom-up)
    for (int l = 2; l <= n; ++l) {              // Length
        for (int i = 0; i <= n - l; ++i) {      // Start
            int j = i + l - 1;                  // End
            for (int k = i; k < j; ++k) {       // Split

                // Kijk naar alle variabelen in linker deel [i, k]
                for (auto const& [B, derivsB] : forest[i][k]) {
                    // Kijk naar alle variabelen in rechter deel [k+1, j]
                    for (auto const& [C, derivsC] : forest[k+1][j]) {

                        // Zoek regels A -> B C
                        auto ruleKey = std::make_pair(B, C);
                        if (binaryRules.find(ruleKey) != binaryRules.end()) {
                            for (const auto& A : binaryRules[ruleKey]) {
                                // Sla op: A is gemaakt uit B en C op splitpunt k
                                std::string ruleDbg = A + "->" + B + C;
                                forest[i][j][A].push_back(Derivation(B, C, k, ruleDbg));
                            }
                        }
                    }
                }
            }
        }
    }

    // 3. Print Tabel (Visualisatie)
    std::cout << "CYK Table for input \"" << w << "\":" << std::endl;
    for (int length = n; length >= 1; --length) {
        std::cout << "| ";
        for (int start = 0; start <= n - length; ++start) {
            int end = start + length - 1;

            std::string cellContent = "{";
            bool first = true;

            // Haal variabelen uit de map keys
            std::vector<std::string> vars;
            for(auto const& [key, val] : forest[start][end]) {
                vars.push_back(key);
            }
            std::sort(vars.begin(), vars.end());

            for (const auto& var : vars) {
                if (!first) cellContent += ",";
                cellContent += var;
                first = false;
            }
            cellContent += "}";

            std::cout << cellContent;
            for (size_t i = cellContent.length(); i < 12; ++i) std::cout << " ";
            if (start < n - length) std::cout << "| ";
        }
        std::cout << "|" << std::endl;
    }

    // 4. Resultaat & Ambiguïteit Check
    std::cout << "\nAnalysis Result:" << std::endl;
    if (forest[0][n - 1].count(S)) {
        std::cout << "  Membership: TRUE" << std::endl;

        // Probeer 2 bomen te genereren
        std::vector<std::string> trees;
        getParseTrees(forest, S, 0, n - 1, trees, 2);

        if (trees.size() > 1) {
            std::cout << "  Status:     AMBIGUOUS" << std::endl;
            std::cout << "  Counterexample (Input): " << w << std::endl;
            std::cout << "  Derivation 1: " << trees[0] << std::endl;
            std::cout << "  Derivation 2: " << trees[1] << std::endl;
        } else {
            std::cout << "  Status:     UNAMBIGUOUS (for this input)" << std::endl;
            std::cout << "  Derivation: " << trees[0] << std::endl;
        }
    } else {
        std::cout << "  Membership: FALSE" << std::endl;
    }
    std::cout << "----------------------------------------------------" << std::endl;
}
