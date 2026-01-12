#include "DotGenerator.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

using namespace std;

string DotGenerator::generate(const ParseTree& root) {
    ostringstream out;
    out << "digraph ParseTree {\n";
    out << "  node [shape=box, style=rounded];\n";
    out << "  rankdir=TB;\n\n";
    
    int nodeCounter = 0;
    if (root) {
        generateHelper(root, out, nodeCounter);
    }
    
    out << "}\n";
    return out.str();
}

void DotGenerator::generateHelper(const ParseTree& node, ostringstream& out, int& nodeCounter) {
    if (!node) return;
    
    int currentId = nodeCounter++;
    
    out << "  node" << currentId << " [label=\"" << node->label << "\"];\n";
    
    for (const auto& child : node->children) {
        if (child) {
            int childId = nodeCounter;
            out << "  node" << currentId << " -> node" << childId << ";\n";
            generateHelper(child, out, nodeCounter);
        }
    }
}

void DotGenerator::saveToFile(const ParseTree& root, const string& filename) {
    string dot = generate(root);
    
    ofstream file(filename);
    if (file.is_open()) {
        file << dot;
        file.close();
        cout << "DOT file saved to: " << filename << "\n";
    } else {
        cerr << "Error: Could not open file " << filename << "\n";
    }
}

void DotGenerator::generateImage(const ParseTree& root, const string& pngFilename) {
    string dotFilename = pngFilename.substr(0, pngFilename.rfind('.')) + ".dot";
    saveToFile(root, dotFilename);
    
    string command = "dot -Tpng \"" + dotFilename + "\" -o \"" + pngFilename + "\"";
    int result = system(command.c_str());
    
    if (result == 0) {
        cout << "PNG image saved to: " << pngFilename << "\n";
    } else {
        cerr << "Error: Failed to generate PNG. Is Graphviz installed?\n";
        cerr << "Install with Mac: brew install graphviz\n";
        cerr << "Install with Ubuntu: sudo apt-get install graphviz\n";
        cerr << "Install with Windows: https://graphviz.org/download// \n";
    }
}
