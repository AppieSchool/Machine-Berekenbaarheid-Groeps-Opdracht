#ifndef MACHINE_BEREKENBAARHEID_GROEPS_OPDRACHT_PARSETREE_H
#define MACHINE_BEREKENBAARHEID_GROEPS_OPDRACHT_PARSETREE_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

struct ParseTreeNode {
    string label;
    vector<shared_ptr<ParseTreeNode>> children;
    
    explicit ParseTreeNode(string lbl) : label(std::move(lbl)) {}
    
    ParseTreeNode(string lbl, vector<shared_ptr<ParseTreeNode>> kids)
        : label(std::move(lbl)), children(std::move(kids)) {}
};

using ParseTree = shared_ptr<ParseTreeNode>;
#endif
