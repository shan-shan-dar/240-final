#include "MenuUI.h"
#include "UIUtils.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <map>
#include <cmath>
#include <algorithm>

using namespace std;

MenuUI::MenuUI(MenuManager& menuMgrRef, User& userRef, Auth& authRef) 
    : menuManager(menuMgrRef), currentUser(userRef), auth(authRef) {}

void MenuUI::showDailyMenu() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string RED = "\033[31m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";

    UIUtils::clearScreen();
    UIUtils::printHeader("VIEW DINING HALL MENUS");

    // Ask user: today's date or custom date
    cout << "\n";
    cout << "     " << YELLOW << "[1]" << RESET << " View today's menu\n";
    cout << "     " << YELLOW << "[2]" << RESET << " View menu for a specific date\n";
    cout << "\n";
    cout << "  >> Choice: ";
    int dateChoice;
    cin >> dateChoice;

    string dateStr;
    if (dateChoice == 1) {
        // Use today's date
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char dateBuffer[11];
        strftime(dateBuffer, sizeof(dateBuffer), "%Y-%m-%d", ltm);
        dateStr = dateBuffer;
    } else if (dateChoice == 2) {
        // Ask user for custom date
        cout << "\n";
        cout << "  Enter date (YYYY-MM-DD): ";
        cin >> dateStr;
    } else {
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n  " << RED << "Invalid choice. Returning to main menu." << RESET << "\n";
        UIUtils::printSeparator();
        UIUtils::waitForEnter();
        return;
    }

    // Ask user for meal
    cout << "\n";
    cout << "     " << YELLOW << "[1]" << RESET << " Breakfast\n";
    cout << "     " << YELLOW << "[2]" << RESET << " Lunch\n";
    cout << "     " << YELLOW << "[3]" << RESET << " Dinner\n";
    cout << "\n";
    cout << "  >> Meal choice: ";
    int mealChoice;
    cin >> mealChoice;

    string mealType;
    switch (mealChoice) {
        case 1: mealType = "breakfast"; break;
        case 2: mealType = "lunch"; break;
        case 3: mealType = "dinner"; break;
        default:
            cout << "\n";
            UIUtils::printSeparator();
            cout << "\n  " << RED << "Invalid choice. Returning to main menu." << RESET << "\n";
            UIUtils::printSeparator();
            UIUtils::waitForEnter();
            return;
    }

    // Call menu.py to fetch/generate JSON for this date + meal
    UIUtils::fetchMenuFor(dateStr, mealType);

    // Try to get a nice "Friday, November 21, 2025" style date for display
    string friendlyDateStr = dateStr;
    try {
        if (dateStr.size() >= 10) {
            int year  = stoi(dateStr.substr(0, 4));
            int month = stoi(dateStr.substr(5, 2));
            int day   = stoi(dateStr.substr(8, 2));
            tm t = {};
            t.tm_year = year - 1900;
            t.tm_mon  = month - 1;
            t.tm_mday = day;
            mktime(&t);
            char friendlyDate[100];
            strftime(friendlyDate, sizeof(friendlyDate), "%A, %B %d, %Y", &t);
            friendlyDateStr = friendlyDate;
        }
    } catch (...) {
        // If parsing fails, just use the raw string.
        friendlyDateStr = dateStr;
    }

    UIUtils::clearScreen();
    UIUtils::printHeader("DINING HALL MENU");

    cout << "\n  " << CYAN << BOLD << friendlyDateStr << RESET
         << " - " << YELLOW << mealType << RESET << "\n\n";

    try {
        auto menu = menuManager.getDailyMenu(mealType, dateStr);

        if (menu.empty()) {
            cout << "\n";
            UIUtils::printSeparator();
            cout << "\n  No menu items found for this selection.\n";
            UIUtils::printSeparator();
        } else {
            menuManager.displayMenuTable(menu);

            cout << "\n";
            UIUtils::printSeparator();
            cout << "\n  " << GREEN << BOLD << "Your Targets:" << RESET
                 << " Cal:" << GREEN << currentUser.calorieGoal << RESET
                 << " " << YELLOW << "P:" << fixed << setprecision(0)
                 << (currentUser.calorieGoal * currentUser.macroRatio.protein / 4.0) << "g" << RESET
                 << " " << GREEN << "C:" << (currentUser.calorieGoal * currentUser.macroRatio.carbs / 4.0) << "g" << RESET
                 << " " << CYAN << "F:" << (currentUser.calorieGoal * currentUser.macroRatio.fats / 9.0) << "g" << RESET << "\n";
            UIUtils::printSeparator();
        }
    } catch (const exception& e) {
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n  " << RED << BOLD << "ERROR:" << RESET << " " << e.what() << "\n";
        UIUtils::printSeparator();
    }

    UIUtils::waitForEnter();
}

// Helper struct to hold meal-specific nutritional targets
struct MealTargets {
    double calories;
    double protein;
    double carbs;
    double fats;
};

void MenuUI::showMealGenerator() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";

    UIUtils::clearScreen();
    UIUtils::printHeader("PERSONALIZED MEAL PLAN GENERATOR");

    // 1. Calculate Overall Daily Goals in Grams
    double proteinGoal = (currentUser.calorieGoal * currentUser.macroRatio.protein) / 4.0;
    double carbsGoal = (currentUser.calorieGoal * currentUser.macroRatio.carbs) / 4.0;
    double fatsGoal = (currentUser.calorieGoal * currentUser.macroRatio.fats) / 9.0;

    cout << "\n";
    cout << "  " << BOLD << "Your Daily Goals:" << RESET << "\n";
    cout << "  - Calories: " << GREEN << currentUser.calorieGoal << RESET << "\n";
    cout << "  - Protein:  " << YELLOW << static_cast<int>(proteinGoal) << "g" << RESET << "\n";
    cout << "  - Carbs:    " << GREEN << static_cast<int>(carbsGoal) << "g" << RESET << "\n";
    cout << "  - Fats:     " << CYAN << static_cast<int>(fatsGoal) << "g" << RESET << "\n";
    UIUtils::printSeparator();
    
    // 2. Check Logged Meals and Calculate Remaining Goals
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char dateStr[100];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", ltm);

    auto loggedTotals = menuManager.calculateDailyTotals(currentUser, dateStr);
    
    double remainingCalories = currentUser.calorieGoal - loggedTotals.calories;
    double remainingProtein = proteinGoal - loggedTotals.protein;
    double remainingCarbs = carbsGoal - loggedTotals.carbs;
    double remainingFats = fatsGoal - loggedTotals.fats;

    // Identify which meals are already logged
    bool bLogged = false, lLogged = false, dLogged = false;
    if (currentUser.loggedMeals.count(dateStr)) {
        auto& dayLog = currentUser.loggedMeals[dateStr];
        if (dayLog.count("breakfast") && !dayLog["breakfast"].empty()) bLogged = true;
        if (dayLog.count("lunch") && !dayLog["lunch"].empty()) lLogged = true;
        if (dayLog.count("dinner") && !dayLog["dinner"].empty()) dLogged = true;
    }

    // 3. Define Meal Budgets for Remaining Meals
    map<string, MealTargets> mealBudgets;
    
    // Base weights for distribution (30/40/30)
    double bWeight = 0.3, lWeight = 0.4, dWeight = 0.3;
    double totalWeight = 0.0;
    
    if (!bLogged) totalWeight += bWeight;
    if (!lLogged) totalWeight += lWeight;
    if (!dLogged) totalWeight += dWeight;

    if (totalWeight > 0) {
        if (!bLogged) {
            double ratio = bWeight / totalWeight;
            mealBudgets["breakfast"] = {remainingCalories * ratio, remainingProtein * ratio, remainingCarbs * ratio, remainingFats * ratio};
        }
        if (!lLogged) {
            double ratio = lWeight / totalWeight;
            mealBudgets["lunch"] = {remainingCalories * ratio, remainingProtein * ratio, remainingCarbs * ratio, remainingFats * ratio};
        }
        if (!dLogged) {
            double ratio = dWeight / totalWeight;
            mealBudgets["dinner"] = {remainingCalories * ratio, remainingProtein * ratio, remainingCarbs * ratio, remainingFats * ratio};
        }
    }

    // 4. Fetch All Available Menus for Today
    UIUtils::fetchMenuFor(dateStr, "breakfast");
    UIUtils::fetchMenuFor(dateStr, "lunch");
    UIUtils::fetchMenuFor(dateStr, "dinner");

    map<string, vector<FoodItem>> allMenus;
    allMenus["breakfast"] = menuManager.getDailyMenu("breakfast", dateStr);
    allMenus["lunch"] = menuManager.getDailyMenu("lunch", dateStr);
    allMenus["dinner"] = menuManager.getDailyMenu("dinner", dateStr);

    if (allMenus["breakfast"].empty() && allMenus["lunch"].empty() && allMenus["dinner"].empty()) {
        cout << "\n  " << YELLOW << "No menus available for today to generate a meal plan." << RESET << "\n";
        UIUtils::waitForEnter();
        return;
    }

    // 5. Algorithm: Find the Best Item for Each Meal's Budget
    map<string, FoodItem> selectedMeals;
    
    for (const auto& budgetPair : mealBudgets) {
        const string& mealType = budgetPair.first;
        const MealTargets& targets = budgetPair.second;
        const vector<FoodItem>& currentMenu = allMenus[mealType];

        if (currentMenu.empty()) continue;

        FoodItem bestItemForMeal;
        double bestScore = -1.0;

        for (const auto& item : currentMenu) {
            // Score based on how well the item fits the *meal's* budget.
            // A score of 1.0 is a perfect match, 0.0 is a terrible match.
            double calorieScore = 1.0 - (abs(item.calories - targets.calories) / max(targets.calories, 1.0));
            double proteinScore = 1.0 - (abs(item.protein - targets.protein) / max(targets.protein, 1.0));
            double carbsScore = 1.0 - (abs(item.carbs - targets.carbs) / max(targets.carbs, 1.0));
            double fatsScore = 1.0 - (abs(item.fats - targets.fats) / max(targets.fats, 1.0));

            // Weighted average of scores. Calories are most important.
            double finalScore = (0.5 * calorieScore) + (0.2 * proteinScore) + (0.15 * carbsScore) + (0.15 * fatsScore);
            
            if (finalScore > bestScore) {
                bestScore = finalScore;
                bestItemForMeal = item;
            }
        }
        selectedMeals[mealType] = bestItemForMeal;
    }
    
    // --- 6. Display the Generated Plan ---
    cout << "\n  " << BOLD << "Generated Meal Plan for Today:" << RESET << "\n\n";
    double totalCalories = loggedTotals.calories;
    double totalProtein = loggedTotals.protein;
    double totalCarbs = loggedTotals.carbs;
    double totalFats = loggedTotals.fats;

    vector<string> mealOrder = {"breakfast", "lunch", "dinner"};
    for (const string& mealType : mealOrder) {
        string mealName = mealType;
        mealName[0] = toupper(mealName[0]); // Capitalize

        bool isLogged = false;
        if (mealType == "breakfast") isLogged = bLogged;
        else if (mealType == "lunch") isLogged = lLogged;
        else if (mealType == "dinner") isLogged = dLogged;

        if (isLogged) {
             cout << "  " << GREEN << "✓ " << mealName << ": " << RESET << "(Already Logged)" << "\n\n";
        } else if (selectedMeals.count(mealType)) {
            const auto& item = selectedMeals[mealType];
            
            cout << "  " << YELLOW << "⦿ " << mealName << ": " << RESET << BOLD << item.name << RESET << "\n";
            cout << "    "
                 << GREEN << item.calories << " cal" << RESET << " | "
                 << YELLOW << "P:" << static_cast<int>(item.protein) << "g" << RESET << " "
                 << GREEN << "C:" << static_cast<int>(item.carbs) << "g" << RESET << " "
                 << CYAN << "F:" << static_cast<int>(item.fats) << "g" << RESET << "\n\n";

            totalCalories += item.calories;
            totalProtein += item.protein;
            totalCarbs += item.carbs;
            totalFats += item.fats;
        }
    }

    UIUtils::printSeparator();
    cout << "\n  " << BOLD << "Plan Summary:" << RESET << "\n";
    cout << "  - Total Calories: " << GREEN << static_cast<int>(totalCalories) << " / " << currentUser.calorieGoal << RESET << "\n";
    cout << "  - Total Protein:  " << YELLOW << static_cast<int>(totalProtein) << "g / " << static_cast<int>(proteinGoal) << "g" << RESET << "\n";
    cout << "  - Total Carbs:    " << GREEN << static_cast<int>(totalCarbs) << "g / " << static_cast<int>(carbsGoal) << "g" << RESET << "\n";
    cout << "  - Total Fats:     " << CYAN << static_cast<int>(totalFats) << "g / " << static_cast<int>(fatsGoal) << "g" << RESET << "\n\n";

    // Ask to log
    cout << "  Do you want to add this meal plan to your daily log? (y/n): ";
    char response;
    cin >> response;
    
    if (response == 'y' || response == 'Y') {
        cout << "\n";
        for (const auto& pair : selectedMeals) {
            string mType = pair.first;
            FoodItem item = pair.second;
            menuManager.logFoodItem(currentUser, mType, dateStr, item.name, 1.0);
            
            // Capitalize meal type for display
            string displayMeal = mType;
            displayMeal[0] = toupper(displayMeal[0]);
            cout << "  " << GREEN << "✓ Added " << displayMeal << ": " << RESET << item.name << "\n";
        }
        auth.updateUser(currentUser);
        cout << "\n  " << GREEN << "Successfully added meal plan to your log!" << RESET << "\n";
    }

    UIUtils::waitForEnter();
}
