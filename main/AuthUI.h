#ifndef AUTHUI_H
#define AUTHUI_H

#include "Auth.h"
#include "User.h"

class AuthUI {
private:
    Auth& auth;
    User& currentUser;
    bool& isLoggedIn;

public:
    AuthUI(Auth& authRef, User& userRef, bool& loggedInRef);
    void showWelcomeScreen();
    void showLoginScreen();
    void showRegisterScreen();
};

#endif
