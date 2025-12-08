#include "UI.h"
#include "UIUtils.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

using namespace std;

// Handles Ctrl+C by cleaning up cached menus before exiting.
void signalHandler(int signum) {
    (void)signum;
    cout << "\nInterrupted. Cleaning up...\n";
    UIUtils::cleanMenuCache();
    exit(signum);
}

// Entry point for the Macro Meal Tracker application.
int main() {
    signal(SIGINT, signalHandler);

    try {
        UI ui;
        ui.run();
        UIUtils::cleanMenuCache();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
