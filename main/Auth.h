#ifndef AUTH_H
#define AUTH_H

#include "User.h"
#include <string>
#include <vector>

class Auth {
private:
    std::string usersFilePath;
    std::vector<User> users;
    
    void loadUsers();
    void saveUsers();
    std::string generateUID();
    
public:
    Auth(const std::string& filepath = "../data/users.json");
    
    bool login(const std::string& username, const std::string& password, User& user);
    bool registerUser(const std::string& username, const std::string& password, 
                     int calorieGoal, int proteinRatio, int carbRatio, int fatRatio);
    bool updateUser(const User& user);
    User* findUserByUsername(const std::string& username);
};

#endif
