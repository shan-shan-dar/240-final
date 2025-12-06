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

void MenuUI::showMealGenerator() {
    const string CYAN   = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN  = "\033[32m";
    const string BOLD   = "\033[1m";
    const string RESET  = "\033[0m";

    UIUtils::clearScreen();
    UIUtils::printHeader("PERSONALIZED MEAL PLAN GENERATOR");

    // 1. Ask MenuManager to generate the plan
    MenuManager::MealPlanResult plan = menuManager.generateMealPlan(currentUser);

    // 2. Show daily goals (from the plan)
    cout << "\n";
    cout << "  " << BOLD << "Your Daily Goals:" << RESET << "\n";
    cout << "  - Calories: " << GREEN  << plan.calorieGoal << RESET << "\n";
    cout << "  - Protein:  " << YELLOW << static_cast<int>(plan.proteinGoal) << "g" << RESET << "\n";
    cout << "  - Carbs:    " << GREEN  << static_cast<int>(plan.carbsGoal)   << "g" << RESET << "\n";
    cout << "  - Fats:     " << CYAN   << static_cast<int>(plan.fatsGoal)    << "g" << RESET << "\n";
    UIUtils::printSeparator();

    // 3. Check if anything can be generated
    bool anyGenerated = !plan.selectedMeals.empty();
    bool bLogged = plan.mealLogged.count("breakfast") && plan.mealLogged["breakfast"];
    bool lLogged = plan.mealLogged.count("lunch")     && plan.mealLogged["lunch"];
    bool dLogged = plan.mealLogged.count("dinner")    && plan.mealLogged["dinner"];
    bool allLogged = bLogged && lLogged && dLogged;

    if (!anyGenerated) {
        cout << "\n";
        if (allLogged) {
            cout << "  " << GREEN << "All meals for today are already logged. No plan to generate." << RESET << "\n";
        } else {
            cout << "  " << YELLOW << "No menus available to generate a meal plan for today." << RESET << "\n";
        }
        UIUtils::waitForEnter();
        return;
    }

    // 4. Display the generated plan
    cout << "\n  " << BOLD << "Generated Meal Plan for Today:" << RESET << "\n\n";

    double totalCalories = plan.loggedTotals.calories;
    double totalProtein  = plan.loggedTotals.protein;
    double totalCarbs    = plan.loggedTotals.carbs;
    double totalFats     = plan.loggedTotals.fats;

    vector<string> mealOrder = {"breakfast", "lunch", "dinner"};
    for (const string& mealType : mealOrder) {
        string mealName = mealType;
        mealName[0] = toupper(mealName[0]);  // Capitalize

        bool isLogged = false;
        auto itLogged = plan.mealLogged.find(mealType);
        if (itLogged != plan.mealLogged.end()) {
            isLogged = itLogged->second;
        }

        if (isLogged) {
            cout << "  " << GREEN << "✓ " << mealName << ": " << RESET << "(Already Logged)" << "\n\n";
            continue;
        }

        auto itSel = plan.selectedMeals.find(mealType);
        if (itSel != plan.selectedMeals.end()) {
            const FoodItem& item = itSel->second;

            cout << "  " << YELLOW << "⦿ " << mealName << ": " << RESET << BOLD << item.name << RESET << "\n";
            cout << "    "
                 << GREEN  << item.calories << " cal" << RESET << " | "
                 << YELLOW << "P:" << static_cast<int>(item.protein) << "g" << RESET << " "
                 << GREEN  << "C:" << static_cast<int>(item.carbs)   << "g" << RESET << " "
                 << CYAN   << "F:" << static_cast<int>(item.fats)    << "g" << RESET << "\n\n";

            totalCalories += item.calories;
            totalProtein  += item.protein;
            totalCarbs    += item.carbs;
            totalFats     += item.fats;
        }
    }

    // 5. Plan summary
    UIUtils::printSeparator();
    cout << "\n  " << BOLD << "Plan Summary:" << RESET << "\n";
    cout << "  - Total Calories: " << GREEN  << static_cast<int>(totalCalories)
         << " / " << plan.calorieGoal << RESET << "\n";
    cout << "  - Total Protein:  " << YELLOW << static_cast<int>(totalProtein)
         << "g / " << static_cast<int>(plan.proteinGoal) << "g" << RESET << "\n";
    cout << "  - Total Carbs:    " << GREEN  << static_cast<int>(totalCarbs)
         << "g / " << static_cast<int>(plan.carbsGoal)   << "g" << RESET << "\n";
    cout << "  - Total Fats:     " << CYAN   << static_cast<int>(totalFats)
         << "g / " << static_cast<int>(plan.fatsGoal)    << "g" << RESET << "\n\n";

    // 6. Ask whether to log the suggested meals
    cout << "  Do you want to add this meal plan to your daily log? (y/n): ";
    char response;
    cin >> response;

    if (response == 'y' || response == 'Y') {
        cout << "\n";
        for (const auto& pair : plan.selectedMeals) {
            const string& mType    = pair.first;
            const FoodItem& item   = pair.second;

            // Skip logging for meals already logged
            auto itLogged = plan.mealLogged.find(mType);
            if (itLogged != plan.mealLogged.end() && itLogged->second) {
                continue;
            }

            menuManager.logFoodItem(currentUser, mType, plan.dateStr, item.name, 1.0);

            string displayMeal = mType;
            displayMeal[0] = toupper(displayMeal[0]);
            cout << "  " << GREEN << "✓ Added " << displayMeal << ": " << RESET << item.name << "\n";
        }
        auth.updateUser(currentUser);
        cout << "\n  " << GREEN << "Successfully added meal plan to your log!" << RESET << "\n";
    }

    UIUtils::waitForEnter();
}

