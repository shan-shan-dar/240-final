#ifndef UIUTILS_H
#define UIUTILS_H

#include <string>

class UIUtils {
public:
    static void clearScreen();
    static void printHeader(const std::string& title);
    static void printSeparator();
    static void waitForEnter();
    static bool fetchMenuFor(const std::string& date, const std::string& mealType);
    static void cleanMenuCache();
};

#endif
