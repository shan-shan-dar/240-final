#ifndef AUTHUI_H
#define AUTHUI_H

#include "Auth.h"
#include "User.h"

// Handles all authentication-related terminal UI screens.
class AuthUI {
private:
    Auth& auth;
    User& currentUser;
    bool& isLoggedIn;

public:
    // Creates an AuthUI bound to an Auth manager and shared login state.
    AuthUI(Auth& authRef, User& userRef, bool& loggedInRef);

    // Displays the initial welcome menu for login/registration.
    void showWelcomeScreen();

    // Runs the login flow and updates currentUser on success.
    void showLoginScreen();

    // Runs the registration flow and creates a new user.
    void showRegisterScreen();
};

#endif
