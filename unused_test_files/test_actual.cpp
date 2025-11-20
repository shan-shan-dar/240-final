#include "MenuManager.h"
#include <iostream>
using namespace std;

int main() {
    MenuManager mm;
    auto menu = mm.getDailyMenu("breakfast", "2025-11-19");
    
    cout << "Loaded " << menu.size() << " items\n\n";
    
    string lastStation = "";
    for (size_t i = 0; i < menu.size(); i++) {
        if (menu[i].station != lastStation) {
            cout << "\n=== " << menu[i].station << " ===\n";
            lastStation = menu[i].station;
        }
        cout << "[" << (i+1) << "] " << menu[i].name << " (" << menu[i].calories << "cal)\n";
    }
    
    return 0;
}
