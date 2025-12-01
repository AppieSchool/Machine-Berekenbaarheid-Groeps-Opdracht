//
// Created by abdir on 30/11/2025.
//

#include "SLR.h"

SLR::SLR(CFG &cfg) : cfg_ref(cfg){
    // Copy productions from CFG
    prods = cfg_ref.getProductions();
    vars = cfg_ref.getVariables();
    terms = cfg_ref.getTerminals();

    // Build the parser
    build();
}

void SLR::print_states() {
    int id = 0;
    for (const auto &state : C) {
        std::cout << "State " << id++ << ":\n";
        for (const auto &it : state) {
            const auto &p = prods[it.prod_index];
            std::cout << "  " << p.lhs << " -> ";
            for (size_t i = 0; i < p.body.size(); ++i) {
                if (i == static_cast<size_t>(it.dot_pos)) std::cout << ". ";
                std::cout << p.body[i] << " ";
            }
            if (it.dot_pos == static_cast<int>(p.body.size())) std::cout << ".";
            std::cout << "\n";
        }
        std::cout << std::endl;
    }
}

void SLR::print_parsing_table() {
    std::cout << "ACTION table:\n";
    for (const auto &entry : ACTION) {
        int state = entry.first.first;
        const std::string &sym = entry.first.second;
        const std::string &act = entry.second;
        std::cout << "  ACTION[" << state << ", " << sym << "] = " << act << "\n";
    }

    std::cout << "\nGOTO table:\n";
    for (const auto &entry : GOTO) {
        int state = entry.first.first;
        const std::string &sym = entry.first.second;
        int to = entry.second;
        std::cout << "  GOTO[" << state << ", " << sym << "] = " << to << "\n";
    }
}
void SLR::build() {
    // 0. Augment grammar
    start_symbol = cfg_ref.getStartSymbol() + "'";
    production augmented(start_symbol, { cfg_ref.getStartSymbol() });
    prods.insert(prods.begin(), augmented);
    vars.insert(vars.begin(), start_symbol);
    cfg_ref.setStartSymbol(start_symbol);
    cfg_ref.setProductions(prods);
    cfg_ref.setVariables(vars);


    // 1. Canonical LR(0) Collection
    std::set<Item> I0 { Item{0, 0} };
    C.insert(closure(I0));

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto &I : C) {
            std::vector<std::string> symbols = terms;
            symbols.insert(symbols.end(), vars.begin(), vars.end());

            for (const auto &X : symbols) {
                auto J = goto_state(I, X);
                if (!J.empty() && C.insert(J).second) {
                    changed = true;
                }
            }
        }
    }

    // 2. FOLLOW sets
    std::map<std::string, std::set<std::string>> follow_sets;

    for (const auto &A : vars) {
        auto f = cfg_ref.followSet(A);
        follow_sets[A] = std::set<std::string>(f.begin(), f.end());
    }

    // 3. Assign state numbers
    std::map<std::set<Item>, int> state_id;
    int next_id = 0;
    for (const auto &I : C) {
        state_id[I] = next_id++;
    }

    // 4. Build ACTION and GOTO tables
    ACTION.clear();
    GOTO.clear();

    for (const auto &I : C) {
        int state = state_id[I];

        // A. SHIFT: A → α . a β     (a terminal)
        for (const auto &item : I) {
            const auto &prod = prods[item.prod_index];

            if (item.dot_pos < prod.body.size()) {
                std::string a = prod.body[item.dot_pos];

                // Check if 'a' is terminal
                if (std::find(terms.begin(), terms.end(), a) != terms.end()) {
                    auto J = goto_state(I, a);
                    if (!J.empty()) {
                        int to = state_id[J];
                        ACTION[{state, a}] = "s" + std::to_string(to);
                    }
                }
            }
        }

        // B. REDUCE:  A → α .     (dot at end)
        for (const auto &item : I) {
            const auto &prod = prods[item.prod_index];

            if (item.dot_pos == prod.body.size()) {
                std::string A = prod.lhs;

                // ACCEPT rule for S' → S .
                if (A == start_symbol) {
                    ACTION[{state, "<EOS>"}] = "acc";
                } else {
                    for (const auto &b : follow_sets[A]) {
                        ACTION[{state, b}] = "r" + std::to_string(item.prod_index);
                    }
                }
            }
        }

        // C. GOTO entries for nonterminals
        for (const auto &X : vars) {
            auto J = goto_state(I, X);
            if (!J.empty()) {
                int to = state_id[J];
                GOTO[{state, X}] = to;
            }
        }
    }
}



std::set<Item> SLR::closure(const std::set<Item> &I) {
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

bool SLR::parse(const std::vector<std::string> &tokens)
{
    // Stack contains state numbers
    std::vector<int> stack;
    stack.push_back(0);

    // Input buffer with <EOS> appended
    std::vector<std::string> input = tokens;
    input.push_back("<EOS>");

    int ip = 0; // input pointer

    while (true)
    {
        int state = stack.back();
        std::string a = input[ip];

        auto action_it = ACTION.find({state, a});

        // Error: no ACTION entry
        if (action_it == ACTION.end()) {
            std::cout << "Parse error: no ACTION["
                      << state << ", " << a << "]\n";
            return false;
        }

        std::string action = action_it->second;

        // SHIFT
        if (action[0] == 's')
        {
            int next_state = std::stoi(action.substr(1));
            stack.push_back(next_state);
            ip++;
            continue;
        }

            // REDUCE
        else if (action[0] == 'r')
        {
            int prod_index = std::stoi(action.substr(1));
            const production &p = prods[prod_index];

            // pop |body| symbols from stack
            for (int i = 0; i < p.body.size(); i++) {
                stack.pop_back();
            }

            int state_after_pop = stack.back();

            // goto(state_after_pop, A)
            auto goto_it = GOTO.find({state_after_pop, p.lhs});
            if (goto_it == GOTO.end()) {
                std::cout << "Parse error: no GOTO["
                          << state_after_pop << ", " << p.lhs << "]\n";
                return false;
            }

            stack.push_back(goto_it->second);
            continue;
        }


            // ACCEPT
        else if (action == "acc") {
            return true;
        }

            // INVALID ACTION
        else
        {
            std::cout << "Parse error: invalid action '"
                      << action << "'\n";
            return false;
        }
    }
}


