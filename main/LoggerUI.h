#ifndef LOGGER_UI_H
#define LOGGER_UI_H

#include "MenuManager.h"
#include "User.h"
#include "Auth.h"
#include <string>

class LoggerUI {
private:
    MenuManager& menuManager;
    User& currentUser;
    Auth& auth;

public:
    LoggerUI(MenuManager& menuMgrRef, User& userRef, Auth& authRef);
    void showFoodLogger();
    void showLoggedFoods();
};

#endif // LOGGER_UI_H
