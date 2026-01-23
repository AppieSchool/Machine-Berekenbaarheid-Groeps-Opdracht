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

    // 3) Producties op basis van de overgangen van de PDA
    //
    // We ondersteunen hier twee vormen:
    //  - Pop: A → ε   (stackPush leeg)
    //  - Vervang: A → B    (stackPush met lengte 1)
    //
    // Voor complexere stackPush (lengte > 1) kan je later uitbreiden.
    for (const auto& t : transitions) {
        // Pop-transitie: (p, a, A → q, ε)
        if (t.stackPush.empty()) {
            json prod;
            prod["head"] = varName(t.fromState, t.stackTop, t.toState);
            prod["body"] = json::array();

            // Epsilon-overgangen: lege body
            if (!t.inputSymbol.empty()) {
                // Transitie leest een echt symbool
                prod["body"].push_back(t.inputSymbol);
            }

            productions.push_back(prod);
            continue;
        }

        // Enkele push: (p, a, A → q, B)
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

        // Voor nu: complexere stackPush (lengte > 1) niet ondersteunen
        // Je kunt dit later uitbreiden met de volledige constructie.
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
