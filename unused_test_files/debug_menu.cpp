#include <iostream>
#include <fstream>
#include <string>
using namespace std;

static string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

int main() {
    ifstream file("../data/menus/breakfast-2025-11-19.json");
    if (!file.is_open()) {
        cout << "ERROR: Cannot open file!" << endl;
        return 1;
    }
    
    string line;
    int itemCount = 0;
    
    // Skip opening brace
    getline(file, line);
    cout << "First line: " << line << endl;
    
    while (getline(file, line) && itemCount < 3) {
        if (line.find("\"name\"") != string::npos) {
            itemCount++;
            cout << "\n=== ITEM " << itemCount << " ===" << endl;
            
            // Parse name
            size_t start = line.find(":") + 1;
            size_t end = line.find_last_of("\"");
            start = line.find("\"", start) + 1;
            string name = line.substr(start, end - start);
            cout << "Name: " << name << endl;
            
            // Skip ingredients
            getline(file, line);
            cout << "Skipping: " << line << endl;
            
            // Skip nutrition opening
            getline(file, line);
            cout << "Nutrition line: " << line << endl;
            
            // Parse calories
            getline(file, line);
            cout << "Calories line: [" << line << "]" << endl;
            start = line.find(":") + 1;
            string calStr = line.substr(start);
            cout << "  After colon: [" << calStr << "]" << endl;
            
            size_t commaPos = calStr.find(",");
            if (commaPos != string::npos) {
                calStr = calStr.substr(0, commaPos);
            }
            cout << "  After comma removal: [" << calStr << "]" << endl;
            
            calStr = trim(calStr);
            cout << "  After trim: [" << calStr << "]" << endl;
            
            try {
                int cal = static_cast<int>(stod(calStr));
                cout << "  SUCCESS: " << cal << " calories" << endl;
            } catch (const exception& e) {
                cout << "  ERROR: " << e.what() << endl;
                return 1;
            }
        }
    }
    
    cout << "\nParsed " << itemCount << " items successfully!" << endl;
    return 0;
}
