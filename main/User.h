#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <map>

struct MacroRatio {
    double protein;
    double carbs;
    double fats;
};

struct User {
    std::string uid;
    std::string username;
    std::string password;
    int calorieGoal;
    MacroRatio macroRatio;
    // Map of "YYYY-MM-DD" -> {"breakfast": {"menu-name": servings}, "lunch": {...}, "dinner": {...}}
    std::map<std::string, std::map<std::string, std::map<std::string, double>>> loggedMeals;
    
    User() : calorieGoal(2000) {
        macroRatio.protein = 0.3;
        macroRatio.carbs = 0.5;
        macroRatio.fats = 0.2;
    }
};

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

struct DailyMenu {
    std::string date;
    std::vector<FoodItem> items;
};

#endif
