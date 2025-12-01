#ifndef MENUUI_H
#define MENUUI_H

#include "MenuManager.h"
#include "User.h"
#include "Auth.h"

class MenuUI {
private:
    MenuManager& menuManager;
    User& currentUser;
    Auth& auth;

public:
    MenuUI(MenuManager& menuMgrRef, User& userRef, Auth& authRef);
    void showDailyMenu();
    void showMealGenerator();
};

#endif
