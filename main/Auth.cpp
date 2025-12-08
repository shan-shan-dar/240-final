#include "Auth.h"
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <iomanip>
#include <iostream>

using namespace std;

// Creates an Auth manager and loads existing users from disk.
Auth::Auth(const string& filepath) : usersFilePath(filepath) {
    loadUsers();
}

// Generates a unique user ID with the form "USR" + 6 digits.
string Auth::generateUID() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(100000, 999999);

    string uid;
    bool unique = false;

    while (!unique) {
        uid = "USR" + to_string(dis(gen));
        unique = true;
        for (const auto& user : users) {
            if (user.uid == uid) {
                unique = false;
                break;
            }
        }
    }

    return uid;
}

// Loads all users from the JSON file into memory.
void Auth::loadUsers() {
    ifstream file(usersFilePath);
    if (!file.is_open()) {
        return;
    }

    users.clear();
    string line;

    getline(file, line);  // skip opening "["

    while (getline(file, line)) {
        if (line.find("\"uid\"") != string::npos) {
            User user;

            // Parse uid.
            size_t start = line.find(":") + 1;
            size_t end = line.find_last_of("\"");
            start = line.find("\"", start) + 1;
            user.uid = line.substr(start, end - start);

            // Parse username.
            getline(file, line);
            start = line.find(":") + 1;
            end = line.find_last_of("\"");
            start = line.find("\"", start) + 1;
            user.username = line.substr(start, end - start);

            // Parse password.
            getline(file, line);
            start = line.find(":") + 1;
            end = line.find_last_of("\"");
            start = line.find("\"", start) + 1;
            user.password = line.substr(start, end - start);

            // Parse calorie goal.
            getline(file, line);
            start = line.find(":") + 1;
            string calStr = line.substr(start);
            size_t commaPos = calStr.find(",");
            if (commaPos != string::npos) {
                calStr = calStr.substr(0, commaPos);
            }
            user.calorieGoal = stoi(calStr);

            // Parse protein ratio.
            getline(file, line);
            start = line.find(":") + 1;
            string proteinStr = line.substr(start);
            commaPos = proteinStr.find(",");
            if (commaPos != string::npos) {
                proteinStr = proteinStr.substr(0, commaPos);
            }
            user.macroRatio.protein = stod(proteinStr);

            // Parse carbs ratio.
            getline(file, line);
            start = line.find(":") + 1;
            string carbsStr = line.substr(start);
            commaPos = carbsStr.find(",");
            if (commaPos != string::npos) {
                carbsStr = carbsStr.substr(0, commaPos);
            }
            user.macroRatio.carbs = stod(carbsStr);

            // Parse fats ratio.
            getline(file, line);
            start = line.find(":") + 1;
            string fatsStr = line.substr(start);
            commaPos = fatsStr.find(",");
            if (commaPos != string::npos) {
                fatsStr = fatsStr.substr(0, commaPos);
            }
            user.macroRatio.fats = stod(fatsStr);

            // Parse optional loggedMeals structure.
            user.loggedMeals.clear();
            getline(file, line);
            if (line.find("loggedMeals") != string::npos) {
                while (getline(file, line) && line.find("}") == string::npos) {
                    if (line.find("\"") != string::npos && line.find(":") != string::npos) {
                        size_t dateStart = line.find("\"") + 1;
                        size_t dateEnd = line.find("\"", dateStart);
                        string dateKey = line.substr(dateStart, dateEnd - dateStart);

                        map<string, map<string, double>> mealsForDate;

                        while (getline(file, line) && line.find("}") == string::npos) {
                            if (line.find("\"") != string::npos && line.find(":") != string::npos) {
                                size_t mealStart = line.find("\"") + 1;
                                size_t mealEnd = line.find("\"", mealStart);
                                string mealType = line.substr(mealStart, mealEnd - mealStart);

                                map<string, double> foodItems;

                                while (getline(file, line) && line.find("}") == string::npos) {
                                    if (line.find("\"") != string::npos) {
                                        size_t foodStart = line.find("\"") + 1;
                                        size_t foodEnd = line.find("\"", foodStart);
                                        string foodName = line.substr(foodStart, foodEnd - foodStart);
                                        size_t colonPos = line.find(":", foodEnd);
                                        double servings = stod(line.substr(colonPos + 1));
                                        foodItems[foodName] = servings;
                                    }
                                }
                                mealsForDate[mealType] = foodItems;
                            }
                        }

                        user.loggedMeals[dateKey] = mealsForDate;
                    }
                }
            }

            users.push_back(user);
        }
    }

    file.close();
}

// Saves all users from memory back to the JSON file.
void Auth::saveUsers() {
    ofstream file(usersFilePath);
    if (!file.is_open()) {
        return;
    }

    file << "[\n";

    for (size_t i = 0; i < users.size(); i++) {
        const User& user = users[i];
        file << "  {\n";
        file << "    \"uid\": \"" << user.uid << "\",\n";
        file << "    \"username\": \"" << user.username << "\",\n";
        file << "    \"password\": \"" << user.password << "\",\n";
        file << "    \"calorieGoal\": " << user.calorieGoal << ",\n";
        file << "    \"proteinRatio\": " << fixed << setprecision(2) << user.macroRatio.protein << ",\n";
        file << "    \"carbsRatio\": " << user.macroRatio.carbs << ",\n";
        file << "    \"fatsRatio\": " << user.macroRatio.fats << ",\n";
        file << "    \"loggedMeals\": {\n";

        size_t dateIdx = 0;
        for (const auto& dateEntry : user.loggedMeals) {
            file << "      \"" << dateEntry.first << "\": {\n";
            size_t mealIdx = 0;
            for (const auto& mealEntry : dateEntry.second) {
                file << "        \"" << mealEntry.first << "\": {\n";
                size_t foodIdx = 0;
                for (const auto& food : mealEntry.second) {
                    file << "          \"" << food.first << "\": " << food.second;
                    if (foodIdx < mealEntry.second.size() - 1) file << ",";
                    file << "\n";
                    foodIdx++;
                }
                file << "        }";
                if (mealIdx < dateEntry.second.size() - 1) file << ",";
                file << "\n";
                mealIdx++;
            }
            file << "      }";
            if (dateIdx < user.loggedMeals.size() - 1) file << ",";
            file << "\n";
            dateIdx++;
        }
        file << "    }\n";
        file << "  }";

        if (i < users.size() - 1) {
            file << ",";
        }
        file << "\n";
    }

    file << "]\n";
    file.close();
}

// Checks credentials and returns true if a matching user is found.
bool Auth::login(const string& username, const string& password, User& user) {
    for (const auto& u : users) {
        if (u.username == username && u.password == password) {
            user = u;
            return true;
        }
    }
    return false;
}

// Registers a new user and initializes their calorie and macro goals.
bool Auth::registerUser(const string& username, const string& password,
                        int calorieGoal, int proteinRatio,
                        int carbRatio, int fatRatio) {
    for (const auto& user : users) {
        if (user.username == username) {
            return false;
        }
    }

    User newUser;
    newUser.uid = generateUID();
    newUser.username = username;
    newUser.password = password;
    newUser.calorieGoal = calorieGoal;

    int total = proteinRatio + carbRatio + fatRatio;
    newUser.macroRatio.protein = static_cast<double>(proteinRatio) / total;
    newUser.macroRatio.carbs   = static_cast<double>(carbRatio)   / total;
    newUser.macroRatio.fats    = static_cast<double>(fatRatio)    / total;

    users.push_back(newUser);
    saveUsers();

    return true;
}

// Updates a stored user record based on the given user object.
bool Auth::updateUser(const User& user) {
    for (auto& u : users) {
        if (u.uid == user.uid) {
            u = user;
            saveUsers();
            return true;
        }
    }
    return false;
}

// Returns a pointer to the user with the given username, or nullptr.
User* Auth::findUserByUsername(const string& username) {
    for (auto& user : users) {
        if (user.username == username) {
            return &user;
        }
    }
    return nullptr;
}
