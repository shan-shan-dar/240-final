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

class UI {
private:
    MenuManager menuManager;
    Auth auth;
    User currentUser;
    bool isLoggedIn;

    // Sub-modules
    // We use unique_ptr or just members. 
    // Since they need references to members of this class, we must initialize them in constructor.
    // But currentUser changes on login. 
    // The sub-modules hold a reference to currentUser. 
    // If currentUser is a member of UI, its address is stable? 
    // Yes, as long as UI is not moved/copied.
    
    // However, AuthUI needs to update currentUser.
    // MenuUI needs to read currentUser.
    
    // Let's use pointers or references.
    // If we use members:
    // AuthUI authUI;
    // We need to initialize it in initializer list: authUI(auth, currentUser)
    
    // But wait, AuthUI constructor takes (Auth&, User&).
    // So we can do:
    // UI() : authUI(auth, currentUser), ... {}
    
    AuthUI authUI;
    MenuUI menuUI;
    LoggerUI loggerUI;
    ProfileUI profileUI;

    void showMainMenu();

public:
    UI();
    void run();
};

#endif // UI_H

