#ifndef UI_H
#define UI_H

#include "MenuManager.h"
#include "Auth.h"
#include "User.h"
#include "AuthUI.h"
#include "MenuUI.h"
#include "LoggerUI.h"
#include "ProfileUI.h"
#include <string>
#include <memory>

// Coordinates the high-level application flow and screen transitions.
class UI {
private:
    MenuManager menuManager;
    Auth auth;
    User currentUser;
    bool isLoggedIn;

    AuthUI authUI;
    MenuUI menuUI;
    LoggerUI loggerUI;
    ProfileUI profileUI;

    // Shows the main menu and routes to sub-screens while logged in.
    void showMainMenu();

public:
    // Constructs the UI and wires submodules to shared state.
    UI();

    // Starts the application loop for login, main menu, and exit.
    void run();
};

#endif // UI_H
