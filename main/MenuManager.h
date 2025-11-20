#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "User.h"
#include <string>
#include <vector>

class MenuManager {
private:
    std::string menuFilePath;
    std::vector<FoodItem> currentMenu;
    
    void loadMenu();
    std::vector<FoodItem> loadMenuFromFile(const std::string& filepath);
    
public:
    MenuManager(const std::string& filepath = "../data/menu.json");
    
    std::vector<FoodItem> getDailyMenu(const std::string& mealType, const std::string& date);
    void displayMenu(const std::vector<FoodItem>& menu);
    void displayMenuTable(const std::vector<FoodItem>& menu);
    std::vector<FoodItem> generateMealPlan(const User& user);
    bool logMeal(User& user, const std::string& mealType, const std::string& date, 
                 int menuNumber, double servings);
    
    // Calculate daily nutrition totals
    struct DailyTotals {
        int calories;
        double protein;
        double carbs;
        double fats;
    };
    DailyTotals calculateDailyTotals(const User& user, const std::string& date);
};

#endif
