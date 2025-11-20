#include "UI.h"
#include <iostream>

using namespace std;

// Main entry point for the Macro Meal Tracker application
int main() {
    try {
        // Create and run the user interface
        UI ui;
        ui.run();
    } catch (const exception& e) {
        // Handle any errors that occur
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
