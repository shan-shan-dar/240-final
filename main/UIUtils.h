#ifndef UIUTILS_H
#define UIUTILS_H

#include <string>

// Provides shared terminal UI helpers and menu fetch utilities.
class UIUtils {
public:
    // Clears the terminal screen.
    static void clearScreen();

    // Prints a simple colored section header with the given title.
    static void printHeader(const std::string& title);

    // Prints a horizontal separator line.
    static void printSeparator();

    // Waits for the user to press ENTER before continuing.
    static void waitForEnter();

    // Fetches a menu JSON for a date and meal type using menu.py.
    static bool fetchMenuFor(const std::string& date,
                             const std::string& mealType);

    // Removes all cached menu files from the menus directory.
    static void cleanMenuCache();
};

#endif
