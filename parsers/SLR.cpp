//
// Created by abdir on 30/11/2025.
//

#include "SLR.h"

SLR::SLR(CFG &cfg) : cfg_ref(cfg){
    // Build the parser
    build();
}

void SLR::print_states() {
    std::cout << "Printing SLR states" << std::endl;

}

void SLR::print_parsing_table() {
    std::cout << "Printing SLR parsing table" << std::endl;

}

void SLR::build() {
    std::cout << "Building SLR parser" << std::endl;
    start_symbol = cfg_ref.getStartSymbol() + "'";
    production augmented = production(start_symbol, {cfg_ref.getStartSymbol()});
    cfg_ref.addProduction(augmented);
    cfg_ref.setStartSymbol( start_symbol);


    // Steps:
    // 1. Construct canonical LR(0) item sets
    auto I0 = std::set<Item>{ Item{0, 0} }; // S' -> .S
    auto C = std::set<std::set<Item>>{};
    C.insert(closure(I0));

    bool changed = true;
    while(changed) {
        changed = false;
        // for each set in C
        for(const auto &I : C) {
            // for each grammar symbol X
            auto allSymbols = cfg_ref.getTerminals();
            auto vars = cfg_ref.getVariables();
            allSymbols.insert(allSymbols.end(), vars.begin(), vars.end());
            for(const auto &X : allSymbols) {
                auto J = goto_state(I, X);
                if(!J.empty() && C.insert(J).second) {
                    changed = true; // new set added
                }
            }
        }
    }

    // 2. Compute FOLLOW sets for all non-terminals

    // 3. Construct Parsing Table

}

std::set<Item> SLR::closure(const std::set<Item> &I) {
    auto &prods = cfg_ref.getProductions();
    auto &vars = cfg_ref.getVariables();

    std::set<Item> J = I;

    // repeat until no new items added
    bool changed = true;
    while(changed) {
        changed = false;

        // for each item in J
        for(auto it : J) {
            // if dot before a nonterminal B
            const auto &prod = prods[it.prod_index];
            if(it.dot_pos < prod.body.size()) {
                std::string B = prod.body[it.dot_pos];
                if(std::find(vars.begin(), vars.end(), B) != vars.end()) {
                    // for each production of B
                    for(int k = 0; k < prods.size(); ++k) {
                        if(prods[k].lhs == B) {
                            // add all productions of B with dot at start
                            Item newItem{ k, 0 };

                            // add if not already in J
                            if (J.insert(newItem).second) {
                                changed = true; // new item -> loop again
                            }
                        }
                    }
                }
            }

        }

    }

    return J;
}

std::set<Item> SLR::goto_state(const std::set<Item> &I, const std::string &symbol) {
    auto J = std::set<Item>{};
    auto &prods = cfg_ref.getProductions();

    for(const auto &it : I) {
        const auto &prod = prods[it.prod_index];
        // if dot before symbol
        if(it.dot_pos < prod.body.size() && prod.body[it.dot_pos] == symbol) {
            // move dot over symbol
            Item moved{ it.prod_index, it.dot_pos + 1 };
            J.insert(moved);
        }
    }

    return closure(J);
}

std::vector<std::string> SLR::followSet(const std::string &symbol) {
    return {};
}

bool SLR::parse(const std::vector<std::string> &tokens) {
    return false;
}
