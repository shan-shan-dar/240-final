#include "UIUtils.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// Clears the terminal screen for Windows or POSIX systems.
void UIUtils::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Prints a simple colored header line with the given title.
void UIUtils::printHeader(const string& title) {
    const string CYAN  = "\033[36m";
    const string BOLD  = "\033[1m";
    const string RESET = "\033[0m";
    
    cout << "\n";
    cout << "  " << CYAN << BOLD << "=== " << title << " ===" << RESET << "\n";
}

// Prints a standard horizontal separator bar.
void UIUtils::printSeparator() {
    cout << "  " << string(50, '-') << "\n";
}

// Waits for the user to press ENTER while discarding pending input.
void UIUtils::waitForEnter() {
    cout << "\n  Press ENTER...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Runs menu.py to fetch or update a menu for a date/meal combination.
bool UIUtils::fetchMenuFor(const string& date, const string& mealType) {
    string args = " " + date + " " + mealType;

#ifdef _WIN32
    string cmd = "python menu.py" + args;
#else
    string cmd =
        "python3 menu.py" + args + " > /dev/null 2>&1"
        " || python menu.py" + args + " > /dev/null 2>&1";
#endif

    int result = system(cmd.c_str());
    (void)result;
    return true;
}

// Deletes all files in the menus cache directory, if it exists.
void UIUtils::cleanMenuCache() {
    string path = "../data/menus/";
    try {
        if (fs::exists(path)) {
            for (const auto& entry : fs::directory_iterator(path)) {
                if (entry.is_regular_file()) {
                    fs::remove(entry.path());
                }
            }
        }
    } catch (...) {
    }
}
