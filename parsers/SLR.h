//
// Created by abdir on 30/11/2025.
//

#ifndef MACHINE_BEREKENBAARHEID_GROEPS_OPDRACHT_SLR_H
#define MACHINE_BEREKENBAARHEID_GROEPS_OPDRACHT_SLR_H

#include <vector>
#include <set>
#include <map>
#include <string>
#include "../utils/json.hpp"
#include "../grammers/CFG.h"

using json = nlohmann::json;


struct Item {
    int prod_index;   // index into CFG productions vector
    int dot_pos;      // dot location in RHS of production (0..body.size())

    // used for comparing items
    bool operator<(const Item& o) const {
        return (prod_index != o.prod_index)
               ? prod_index < o.prod_index
               : dot_pos < o.dot_pos;
    }

    bool operator==(const Item& o) const {
        return prod_index == o.prod_index && dot_pos == o.dot_pos;
    }
};


using State = std::vector<Item>;


class SLR {
public:
    explicit SLR(CFG &cfg);

    // parse a token sequence
    [[nodiscard]] bool parse(const std::vector<std::string> &tokens);

    // Debug printing of LR(0) item sets
    void print_states();

    // Debug printing of ACTION/GOTO parsing table
    void print_parsing_table();
private:
    std::set<Item> closure(const std::set<Item>& I);
    std::set<Item> goto_state(const std::set<Item>& I, const std::string &symbol);

    // Build complete SLR parsing tables
    void build();

    // References
    CFG &cfg_ref;
    std::string start_symbol;
    std::vector<production> prods;
    std::vector<std::string> vars;
    std::vector<std::string> terms;

    // Data structures
    std::map<std::pair<int, std::string>, std::string> ACTION;
    std::map<std::pair<int, std::string>, int> GOTO;
    std::set<std::set<Item>> C; // Canonical collection of LR(0) items
};

#endif // MACHINE_BEREKENBAARHEID_GROEPS_OPDRACHT_SLR_H
