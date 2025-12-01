#include "UIUtils.h"
#include <iostream>
#include <limits>
#include <cstdlib>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

void UIUtils::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void UIUtils::printHeader(const string& title) {
    const string CYAN = "\033[36m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    cout << "\n";
    cout << "  " << CYAN << BOLD << "=== " << title << " ===" << RESET << "\n";
}

void UIUtils::printSeparator() {
    cout << "  " << string(50, '-') << "\n";
}

void UIUtils::waitForEnter() {
    cout << "\n  Press ENTER...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

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
        // Ignore errors during cleanup
    }
}
