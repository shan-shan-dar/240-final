#ifndef MENUUI_H
#define MENUUI_H

#include "MenuManager.h"
#include "User.h"
#include "Auth.h"

// Handles viewing dining hall menus and generated meal plans.
class MenuUI {
private:
    MenuManager& menuManager;
    User& currentUser;
    Auth& auth;

public:
    // Creates a MenuUI bound to the shared menu manager, user, and auth.
    MenuUI(MenuManager& menuMgrRef, User& userRef, Auth& authRef);

    // Shows a menu for a selected date and meal type.
    void showDailyMenu();

    // Generates and displays a meal plan, with an option to log it.
    void showMealGenerator();
};

#endif
