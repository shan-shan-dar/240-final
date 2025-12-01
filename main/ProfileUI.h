#ifndef PROFILE_UI_H
#define PROFILE_UI_H

#include "User.h"
#include "Auth.h"

class ProfileUI {
private:
    User& currentUser;
    Auth& auth;

public:
    ProfileUI(User& userRef, Auth& authRef);
    void showProfileEditor();
};

#endif // PROFILE_UI_H
