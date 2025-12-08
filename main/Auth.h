#ifndef AUTH_H
#define AUTH_H

#include "User.h"
#include <string>
#include <vector>

// Manages user accounts, authentication, and persistence.
class Auth {
private:
    std::string usersFilePath;
    std::vector<User> users;

    // Loads all users from disk into memory.
    void loadUsers();

    // Saves all users from memory to disk.
    void saveUsers();

    // Generates a new unique user ID.
    std::string generateUID();

public:
    // Creates an Auth manager using the given users JSON file.
    Auth(const std::string& filepath = "../data/users.json");

    // Attempts to log in and populate the output user on success.
    bool login(const std::string& username,
               const std::string& password,
               User& user);

    // Registers a new user with goals and macro ratios.
    bool registerUser(const std::string& username,
                      const std::string& password,
                      int calorieGoal,
                      int proteinRatio,
                      int carbRatio,
                      int fatRatio);

    // Updates an existing user record and persists it.
    bool updateUser(const User& user);

    // Finds a user by username, or returns nullptr if not found.
    User* findUserByUsername(const std::string& username);
};

#endif
