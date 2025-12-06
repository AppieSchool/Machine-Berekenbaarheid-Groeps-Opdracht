#include "ParseTree.h"
#include "DotGenerator.h"
#include <iostream>

using namespace std;

// Tree structure:
//       E
//      /|\
//     E + T
//     |   |
//     T   F
//     |   |
//     F   id
//     |
//     id
ParseTree createSampleTree() {
    auto id1 = make_shared<ParseTreeNode>("id");
    auto F1 = make_shared<ParseTreeNode>("F", vector<ParseTree>{id1});
    auto T1 = make_shared<ParseTreeNode>("T", vector<ParseTree>{F1});
    
    auto plus = make_shared<ParseTreeNode>("+");
    
    auto id2 = make_shared<ParseTreeNode>("id");
    auto F2 = make_shared<ParseTreeNode>("F", vector<ParseTree>{id2});
    auto T2 = make_shared<ParseTreeNode>("T", vector<ParseTree>{F2});
    
    auto E1 = make_shared<ParseTreeNode>("E", vector<ParseTree>{T1});
    
    auto root = make_shared<ParseTreeNode>("E", 
                vector<ParseTree>{E1, plus, T2});
    
    return root;
}

int main() {
    cout << "=== Parse Tree Visualization Test ===\n\n";
    
    ParseTree tree = createSampleTree();
    
    string dot = DotGenerator::generate(tree);
    cout << "Generated DOT format:\n";
    cout << dot << "\n";
    
    // Save to output folder (keeps generated files separate from code)
    DotGenerator::saveToFile(tree, "visualization/output/parse_tree.dot");
    
    return 0;
}
