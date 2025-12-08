#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <map>

// Stores macro ratio fractions for protein, carbs, and fats.
struct MacroRatio {
    double protein;
    double carbs;
    double fats;
};

// Represents a user, their goals, and logged meals by date and meal type.
struct User {
    std::string uid;
    std::string username;
    std::string password;
    int calorieGoal;
    MacroRatio macroRatio;
    std::map<std::string,
             std::map<std::string,
                      std::map<std::string, double>>> loggedMeals;

    // Initializes a user with a default calorie goal and macro split.
    User() : calorieGoal(2000) {
        macroRatio.protein = 0.3;
        macroRatio.carbs   = 0.5;
        macroRatio.fats    = 0.2;
    }
};

// Describes a single menu item with nutrition and serving information.
struct FoodItem {
    std::string name;
    std::string station;
    int calories;
    double protein;
    double carbs;
    double fats;
    std::string servingAmount;
    std::string servingUnit;
};

// Represents all menu items available on a given date.
struct DailyMenu {
    std::string date;
    std::vector<FoodItem> items;
};

#endif
