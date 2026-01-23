//
// Created by abdir on 06/10/2025.
//

#include "PDA.h"

PDA::PDA(std::string jsonFile) : filename(std::move(jsonFile)) {
    std::ifstream input(filename);
    if (!input.is_open()) {
        std::cerr << "Error: Unable to open file." << std::endl;
        return;
    }

    // Read the JSON content
    json j;
    input >> j;

    auto statesJson = j["States"];
    auto alphabetJson = j["Alphabet"];
    auto stackAlphabetJson = j["StackAlphabet"];
    auto transitionsJson = j["Transitions"];
    auto startStateJson = j["StartState"];
    auto startStackJson = j["StartStack"];

    // Parse states
    for (const auto &state : statesJson) {
        states.push_back(state);
    }
    // Parse alphabet
    for (const auto & symbol : alphabetJson) {
        alphabet.push_back(symbol);
    }
    // Parse stack alphabet
    for (const auto & stackSym : stackAlphabetJson) {
        stackAlphabet.push_back(stackSym);
    }
    // Parse start state
    startState = startStateJson;
    // Parse start stack symbol
    startStack = startStackJson;

    // Parse transitions
    for(const auto & trans : transitionsJson){
        transition tr;
        tr.fromState = trans["from"];
        tr.toState = trans["to"];
        tr.inputSymbol = trans["input"];
        for(const auto& replacement : trans["replacement"]){
            tr.stackPush.push_back(replacement);
        }
        tr.stackTop = trans["stacktop"];
        transitions.push_back(tr);
    }
}

CFG PDA::toCFG() {
    json jsonObj;

    // Initialize components of the CFG
    std::set<std::string> variableSet;
    std::vector<json> productions;

    // Startsymbol van de CFG
    std::string startSymbol = "S";
    variableSet.insert(startSymbol);

    // Terminals zijn gewoon het invoeralfabet van de PDA
    std::vector<std::string> terminals = alphabet;

    // Helper lambda om een variabelenaam [p,A,q] te bouwen
    auto varName = [](const std::string& from,
                      const std::string& stackSym,
                      const std::string& to) {
        return "[" + from + "," + stackSym + "," + to + "]";
    };

    // 1) Maak alle variabelen [p, A, q] voor p,q ∈ Q en A ∈ Γ
    for (const auto& p : states) {
        for (const auto& q : states) {
            for (const auto& A : stackAlphabet) {
                variableSet.insert(varName(p, A, q));
            }
        }
    }

    // 2) Startsymbol S → [q0, Z0, q] voor alle q ∈ Q
    for (const auto& q : states) {
        json prod;
        prod["head"] = startSymbol;
        prod["body"] = json::array();
        prod["body"].push_back(varName(startState, startStack, q));
        productions.push_back(prod);
    }

    // Helper: genereer alle tuples (r1..r_{k-1}) van states (cartesisch product)
    // k = stackPush.size()
    auto generateStateTuples = [&](int len) {
        std::vector<std::vector<std::string>> tuples;
        if (len <= 0) {
            tuples.push_back({});
            return tuples;
        }

        // iteratief opbouwen: start met lege tuple
        tuples.push_back({});
        for (int i = 0; i < len; i++) {
            std::vector<std::vector<std::string>> next;
            next.reserve(tuples.size() * states.size());
            for (const auto& t : tuples) {
                for (const auto& s : states) {
                    auto copy = t;
                    copy.push_back(s);
                    next.push_back(std::move(copy));
                }
            }
            tuples = std::move(next);
        }
        return tuples;
    };

    // 3) Producties op basis van de overgangen van de PDA
    for (const auto& t : transitions) {

        // (p, a, A -> q, ε)
        if (t.stackPush.empty()) {
            json prod;
            prod["head"] = varName(t.fromState, t.stackTop, t.toState);
            prod["body"] = json::array();

            if (!t.inputSymbol.empty()) {
                prod["body"].push_back(t.inputSymbol);
            }
            productions.push_back(prod);
            continue;
        }

        // (p, a, A -> q, B)  (k=1)
        if (t.stackPush.size() == 1) {
            const std::string& B = t.stackPush[0];

            // [p, A, r] → a [q, B, r]  voor alle r ∈ Q
            for (const auto& r : states) {
                json prod;
                prod["head"] = varName(t.fromState, t.stackTop, r);
                prod["body"] = json::array();

                if (!t.inputSymbol.empty()) {
                    prod["body"].push_back(t.inputSymbol);
                }
                prod["body"].push_back(varName(t.toState, B, r));

                productions.push_back(prod);
            }
            continue;
        }

        // Algemeen geval: (p, a, A -> q, B1 B2 ... Bk), k >= 2
        const int k = static_cast<int>(t.stackPush.size());

        // kies rk (finale state) vrij in Q
        for (const auto& rk : states) {

            // kies (r1..r_{k-1}) als tuple in Q^(k-1)
            auto tuples = generateStateTuples(k - 1);

            for (const auto& mid : tuples) {
                // mid heeft lengte k-1: r1..r_{k-1}
                json prod;
                prod["head"] = varName(t.fromState, t.stackTop, rk);
                prod["body"] = json::array();

                if (!t.inputSymbol.empty()) {
                    prod["body"].push_back(t.inputSymbol);
                }

                // B1: [q, B1, r1]
                prod["body"].push_back(varName(t.toState, t.stackPush[0], mid[0]));

                // B2..B_{k-1}: [r_{i-1}, Bi, r_i]
                for (int i = 1; i < k - 1; i++) {
                    prod["body"].push_back(varName(mid[i - 1], t.stackPush[i], mid[i]));
                }

                // Bk: [r_{k-1}, Bk, rk]
                prod["body"].push_back(varName(mid[k - 2], t.stackPush[k - 1], rk));

                productions.push_back(prod);
            }
        }
    }

    // Zet variabelen om naar vector en sorteer
    std::vector<std::string> variables(variableSet.begin(), variableSet.end());
    std::sort(variables.begin(), variables.end());

    // Bouw JSON-object voor CFG
    jsonObj["Variables"] = variables;
    jsonObj["Productions"] = productions;
    jsonObj["Start"] = startSymbol;
    jsonObj["Terminals"] = terminals;

    return CFG(jsonObj);
}

