#ifndef PROFILE_UI_H
#define PROFILE_UI_H

#include "User.h"
#include "Auth.h"

// Handles profile editing for calorie goals, macros, and password.
class ProfileUI {
private:
    User& currentUser;
    Auth& auth;

public:
    // Creates a ProfileUI bound to the current user and auth manager.
    ProfileUI(User& userRef, Auth& authRef);

    // Shows the profile editor menu and applies chosen updates.
    void showProfileEditor();
};

#endif // PROFILE_UI_H
    