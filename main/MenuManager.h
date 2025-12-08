#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include "User.h"
#include <string>
#include <vector>
#include <map>

// Manages menu loading, display, logging, and meal-plan generation.
class MenuManager {
private:
    // Loads a menu for one meal and date from a JSON file on disk.
    std::vector<FoodItem> loadMenuFromFile(const std::string& filepath);

public:
    // Constructs a menu manager; filepath is kept for legacy callers but unused.
    MenuManager(const std::string& filepath = "./data/menu.json");

    // Returns the menu for a given meal type ("breakfast", "lunch", "dinner")
    // and date string ("YYYY-MM-DD").
    std::vector<FoodItem> getDailyMenu(const std::string& mealType,
                                       const std::string& date);

    // Prints a simple numbered list of menu items.
    void displayMenu(const std::vector<FoodItem>& menu);

    // Prints a formatted table grouped by station with basic macro info.
    void displayMenuTable(const std::vector<FoodItem>& menu);

    // Aggregated nutrition totals for a single day.
    struct DailyTotals {
        int calories;
        double protein;
        double carbs;
        double fats;
    };

    // Full result of generating a meal plan for a single day.
    struct MealPlanResult {
        // Date for which the plan was generated ("YYYY-MM-DD").
        std::string dateStr;

        int calorieGoal;
        double proteinGoal;
        double carbsGoal;
        double fatsGoal;

        // Totals from all meals the user has already logged for this date.
        DailyTotals loggedTotals;

        // Flags indicating whether any food is logged for each meal type.
        std::map<std::string, bool> mealLogged;

        // A planned menu item plus the suggested number of servings.
        struct PlannedItem {
            FoodItem item;
            double servings;
        };

        // Planned items per meal type (keys: "breakfast", "lunch", "dinner").
        std::map<std::string, std::vector<PlannedItem>> selectedMeals;
    };

    // Generates a meal plan for today using the user's goals and logged meals.
    MealPlanResult generateMealPlan(const User& user);

    // Logs a menu choice by index for a given meal and date.
    bool logMeal(User& user, const std::string& mealType,
                 const std::string& date, int menuNumber, double servings);

    // Logs a food item by name for a given meal and date.
    bool logFoodItem(User& user, const std::string& mealType,
                     const std::string& date, const std::string& foodName,
                     double servings);

    // Removes a previously logged food item for a given date and meal.
    bool removeLoggedMeal(User& user, const std::string& date,
                          const std::string& mealType,
                          const std::string& foodName);

    // Computes daily nutrition totals for all logged meals on a date.
    DailyTotals calculateDailyTotals(const User& user, const std::string& date);
};

#endif
