#ifndef LOGGER_UI_H
#define LOGGER_UI_H

#include "MenuManager.h"
#include "User.h"
#include "Auth.h"
#include <string>

// Handles UI flows for logging foods and viewing logged meals.
class LoggerUI {
private:
    MenuManager& menuManager;
    User& currentUser;
    Auth& auth;

public:
    // Creates a logger UI bound to a menu manager, user, and auth manager.
    LoggerUI(MenuManager& menuMgrRef, User& userRef, Auth& authRef);

    // Shows the interface for logging foods for today's meals.
    void showFoodLogger();

    // Shows today's logged foods and allows deleting entries.
    void showLoggedFoods();
};

#endif // LOGGER_UI_H
