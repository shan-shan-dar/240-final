#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

static string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

struct FoodItem {
    string name;
    string station;
    int calories;
};

int main() {
    vector<FoodItem> menu;
    ifstream file("../data/menus/breakfast-2025-11-19.json");
    
    if (!file.is_open()) {
        cout << "Cannot open file!" << endl;
        return 1;
    }
    
    string line;
    string currentStation = "";
    
    getline(file, line); // Skip opening brace
    
    int lineNum = 0;
    while (getline(file, line)) {
        lineNum++;
        
        // Check for station name
        if (line.find("\":") != string::npos && line.find("[") != string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\"", start);
            currentStation = line.substr(start, end - start);
            cout << "Found station at line " << lineNum << ": " << currentStation << endl;
            continue;
        }
        
        // Look for name field
        if (line.find("\"name\"") != string::npos) {
            FoodItem item;
            item.station = currentStation;
            
            size_t start = line.find(":") + 1;
            size_t end = line.find_last_of("\"");
            start = line.find("\"", start) + 1;
            item.name = line.substr(start, end - start);
            
            // Skip next 3 lines to get to calories
            getline(file, line); // ingredients
            getline(file, line); // nutrition: {
            getline(file, line); // calories line
            
            start = line.find(":") + 1;
            string calStr = line.substr(start);
            size_t commaPos = calStr.find(",");
            if (commaPos != string::npos) {
                calStr = calStr.substr(0, commaPos);
            }
            calStr = trim(calStr);
            
            try {
                item.calories = static_cast<int>(stod(calStr));
                menu.push_back(item);
            } catch (...) {
                cout << "Error parsing: " << item.name << " at line " << lineNum << endl;
            }
        }
    }
    
    cout << "\n========== RESULTS ==========\n";
    cout << "Loaded " << menu.size() << " items\n\n";
    
    // Show first 10 and last 10
    cout << "First 10:\n";
    for (int i = 0; i < min(10, (int)menu.size()); i++) {
        cout << "  [" << i+1 << "] " << menu[i].station << " - " << menu[i].name << " (" << menu[i].calories << "cal)\n";
    }
    
    cout << "\nLast 10:\n";
    for (int i = max(0, (int)menu.size() - 10); i < (int)menu.size(); i++) {
        cout << "  [" << i+1 << "] " << menu[i].station << " - " << menu[i].name << " (" << menu[i].calories << "cal)\n";
    }
    
    return 0;
}
