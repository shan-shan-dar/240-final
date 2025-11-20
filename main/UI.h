#ifndef UI_H
#define UI_H

#include "User.h"
#include "Auth.h"
#include "MenuManager.h"
#include <string>

class UI {
private:
    Auth auth;
    MenuManager menuManager;
    User currentUser;
    bool isLoggedIn;
    
    void clearScreen();
    void printHeader(const std::string& title);
    void printSeparator();
    void waitForEnter();
    
    void showWelcomeScreen();
    void showLoginScreen();
    void showRegisterScreen();
    void showMainMenu();
    void showProfileEditor();
    void showDailyMenu();
    void showMealGenerator();
    void showFoodLogger();
    void showLoggedFoods();
    
public:
    UI();
    void run();
};

#endif
