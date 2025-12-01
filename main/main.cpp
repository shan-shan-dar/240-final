#include "UI.h"
#include "UIUtils.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

using namespace std;

// Signal handler to ensure cleanup on Ctrl+C
void signalHandler(int signum) {
    (void)signum; // Suppress unused parameter warning
    cout << "\nInterrupted. Cleaning up...\n";
    UIUtils::cleanMenuCache();
    exit(signum);
}

// Main entry point for the Macro Meal Tracker application
int main() {
    // Register signal handler
    signal(SIGINT, signalHandler);

    try {
        // Create and run the user interface
        UI ui;
        ui.run();
        
        // Cleanup temporary menu files
        UIUtils::cleanMenuCache();
    } catch (const exception& e) {
        // Handle any errors that occur
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}
