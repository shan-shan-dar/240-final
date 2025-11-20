#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main() {
    ifstream file("../data/menus/breakfast-2025-11-19.json");
    if (!file.is_open()) {
        cout << "Cannot open file" << endl;
        return 1;
    }
    
    string line;
    int nameCount = 0;
    int stationCount = 0;
    
    while (getline(file, line)) {
        if (line.find("\"name\"") != string::npos) {
            nameCount++;
        }
        if (line.find("\":") != string::npos && line.find("[") != string::npos) {
            stationCount++;
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\"", start);
            cout << "Station: " << line.substr(start, end - start) << endl;
        }
    }
    
    cout << "\nTotal stations: " << stationCount << endl;
    cout << "Total items: " << nameCount << endl;
    
    return 0;
}
