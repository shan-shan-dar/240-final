#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main() {
    ifstream file("../data/menus/breakfast-2025-11-19.json");
    if (!file.is_open()) {
        cout << "File not found!" << endl;
        return 1;
    }
    
    string line;
    int lineNum = 0;
    
    while (getline(file, line) && lineNum < 30) {
        cout << "Line " << lineNum << ": [" << line << "]" << endl;
        
        if (line.find("\"calories\"") != string::npos) {
            size_t start = line.find(":") + 1;
            string calStr = line.substr(start);
            cout << "  Raw value: [" << calStr << "]" << endl;
            
            size_t commaPos = calStr.find(",");
            if (commaPos != string::npos) {
                calStr = calStr.substr(0, commaPos);
            }
            cout << "  After comma removal: [" << calStr << "]" << endl;
            
            try {
                double val = stod(calStr);
                cout << "  Parsed: " << val << endl;
            } catch (const exception& e) {
                cout << "  ERROR: " << e.what() << endl;
            }
        }
        
        lineNum++;
    }
    
    return 0;
}
