#include "LoggerUI.h"
#include "UIUtils.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <algorithm>

using namespace std;

// Creates a logger UI with references to menu manager, user, and auth.
LoggerUI::LoggerUI(MenuManager& menuMgrRef, User& userRef, Auth& authRef)
    : menuManager(menuMgrRef), currentUser(userRef), auth(authRef) {}

// Shows the flow for logging foods for one meal on the current day.
void LoggerUI::showFoodLogger() {
    const string CYAN   = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN  = "\033[32m";
    const string RED    = "\033[31m";
    const string BOLD   = "\033[1m";
    const string RESET  = "\033[0m";
    
    UIUtils::clearScreen();
    UIUtils::printHeader("LOG A MEAL");
    
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char dateStr[100];
    char friendlyDate[100];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", ltm);
    strftime(friendlyDate, sizeof(friendlyDate), "%A, %B %d, %Y", ltm);
    
    cout << "\n";
    cout << "  Date: " << CYAN << BOLD << friendlyDate << RESET << "\n";
    cout << "\n";
    UIUtils::printSeparator();
    cout << "\n";
    cout << "     Which meal would you like to log?\n\n";
    cout << "     " << YELLOW << "[1]" << RESET << " Breakfast\n";
    cout << "     " << YELLOW << "[2]" << RESET << " Lunch\n";
    cout << "     " << YELLOW << "[3]" << RESET << " Dinner\n";
    cout << "     " << YELLOW << "[4]" << RESET << " Go back\n";
    cout << "\n";
    UIUtils::printSeparator();
    cout << "\n";
    cout << "  >> Enter your choice (1-4): ";
    
    int mealChoice;
    cin >> mealChoice;
    
    if (mealChoice < 1 || mealChoice > 4) {
        cout << "\n\n";
        cout << "     Invalid choice.\n";
        UIUtils::waitForEnter();
        return;
    }
    
    if (mealChoice == 4) {
        return;
    }

    string mealType;
    switch (mealChoice) {
        case 1: mealType = "breakfast"; break;
        case 2: mealType = "lunch"; break;
        case 3: mealType = "dinner"; break;
    }

    UIUtils::fetchMenuFor(dateStr, mealType);
    auto menu = menuManager.getDailyMenu(mealType, dateStr);
    
    if (menu.empty()) {
        UIUtils::clearScreen();
        UIUtils::printHeader("LOG A MEAL - " + string(mealType));
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n";
        cout << "     No menu available for " << mealType << " on " << friendlyDate << "\n";
        cout << "     Please check back later or try a different meal.\n";
        cout << "\n";
        UIUtils::printSeparator();
        UIUtils::waitForEnter();
        return;
    }
    
    string input;
    while (true) {
        UIUtils::clearScreen();
        UIUtils::printHeader("LOG A MEAL - " + string(mealType));
        cout << "\n";
        cout << "  Date: " << CYAN << BOLD << friendlyDate << RESET << "\n";
        cout << "\n";
        
        menuManager.displayMenuTable(menu);
        
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n";
        cout << "  >> Enter item number (or type '" << YELLOW << "done" << RESET << "' to finish): ";
        
        cin >> input;
        
        if (input == "done" || input == "Done" || input == "DONE") {
            break;
        }
        
        int itemNumber;
        try {
            itemNumber = stoi(input);
        } catch (...) {
            cout << "\n";
            cout << "     " << RED << "Invalid input." << RESET << " Please enter a number or 'done'.\n";
            UIUtils::waitForEnter();
            continue;
        }
        
        if (itemNumber < 1 || itemNumber > static_cast<int>(menu.size())) {
            cout << "\n";
            cout << "     " << RED << "Invalid item number." << RESET << " Please try again.\n";
            UIUtils::waitForEnter();
            continue;
        }
        
        cout << "\n";
        cout << "  >> Enter number of servings: ";
        
        double servings;
        cin >> servings;
        
        if (cin.fail() || servings <= 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\n";
            cout << "     " << RED << "Invalid servings amount." << RESET << " Please try again.\n";
            UIUtils::waitForEnter();
            continue;
        }
        
        bool success = menuManager.logMeal(currentUser, mealType, dateStr, itemNumber, servings);
        
        if (success) {
            auth.updateUser(currentUser);
            cout << "\n";
            UIUtils::printSeparator();
            cout << "\n";
            cout << "     " << GREEN << BOLD << "✓ Logged!" << RESET << " "
                 << CYAN << menu[itemNumber - 1].name << RESET 
                 << " x" << YELLOW << fixed << setprecision(1) << servings << RESET 
                 << " (" << GREEN
                 << static_cast<int>(menu[itemNumber - 1].calories * servings)
                 << " cal" << RESET << ")\n";
            cout << "\n";
            UIUtils::printSeparator();
            cout << "\n";
            cout << "  Press ENTER to log another item...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        } else {
            cout << "\n";
            cout << "     " << RED << BOLD << "Failed to log meal." << RESET << " Please try again.\n";
            UIUtils::waitForEnter();
        }
    }
}

// Shows today's logged foods with nutrition and supports deletion by index.
void LoggerUI::showLoggedFoods() {
    const string CYAN    = "\033[36m";
    const string YELLOW  = "\033[33m";
    const string GREEN   = "\033[32m";
    const string MAGENTA = "\033[35m";
    const string RED     = "\033[31m";
    const string BOLD    = "\033[1m";
    const string RESET   = "\033[0m";
    
    while (true) {
        UIUtils::clearScreen();
        UIUtils::printHeader("TODAY'S LOGGED FOODS");
        
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char dateStr[100];
        char friendlyDate[100];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", ltm);
        strftime(friendlyDate, sizeof(friendlyDate), "%A, %B %d, %Y", ltm);
        
        cout << "\n  " << CYAN << BOLD << friendlyDate << RESET << "\n\n";
        
        auto dateIt = currentUser.loggedMeals.find(dateStr);
        if (dateIt == currentUser.loggedMeals.end() || dateIt->second.empty()) {
            UIUtils::printSeparator();
            cout << "\n  No foods logged today yet.\n";
            cout << "  Use option [3] from main menu to log meals.\n\n";
            UIUtils::printSeparator();
            UIUtils::waitForEnter();
            return;
        }
        
        const vector<string> mealTypes = {"breakfast", "lunch", "dinner"};
        bool hasAnyFood = false;
        
        struct LoggedItemRef {
            string mealType;
            string foodName;
        };
        vector<LoggedItemRef> itemMap;
        int displayIndex = 1;
        
        for (const auto& mealType : mealTypes) {
            auto mealIt = dateIt->second.find(mealType);
            if (mealIt != dateIt->second.end() && !mealIt->second.empty()) {
                hasAnyFood = true;
                
                string displayMeal = mealType;
                displayMeal[0] = toupper(displayMeal[0]);
                
                cout << "  " << MAGENTA << BOLD << displayMeal << RESET << "\n";
                UIUtils::printSeparator();
                
                auto menu = menuManager.getDailyMenu(mealType, dateStr);
                
                for (const auto& foodEntry : mealIt->second) {
                    string foodName = foodEntry.first;
                    double servings = foodEntry.second;
                    
                    itemMap.push_back({mealType, foodName});
                    
                    int totalCals = 0;
                    double totalProtein = 0;
                    double totalCarbs = 0;
                    double totalFats = 0;
                    
                    for (const auto& item : menu) {
                        if (item.name == foodName) {
                            totalCals    = static_cast<int>(item.calories * servings);
                            totalProtein = item.protein * servings;
                            totalCarbs   = item.carbs * servings;
                            totalFats    = item.fats * servings;
                            break;
                        }
                    }
                    
                    cout << "  " << YELLOW << "[" << displayIndex++ << "] " << RESET
                         << CYAN << foodName << RESET
                         << " x" << fixed << setprecision(1) << servings << "\n";
                    cout << "      " << GREEN << totalCals << " cal" << RESET
                         << " | " << YELLOW << "P:" << setprecision(0) << totalProtein << "g" << RESET
                         << " " << GREEN << "C:" << totalCarbs << "g" << RESET
                         << " " << CYAN << "F:" << totalFats << "g" << RESET << "\n";
                }
                cout << "\n";
            }
        }
        
        if (!hasAnyFood) {
            UIUtils::printSeparator();
            cout << "\n  No foods logged today yet.\n";
            cout << "  Use option [3] from main menu to log meals.\n\n";
            UIUtils::printSeparator();
            UIUtils::waitForEnter();
            return;
        } else {
            auto totals = menuManager.calculateDailyTotals(currentUser, dateStr);
            UIUtils::printSeparator();
            cout << "\n  " << BOLD << "Daily Totals:" << RESET << "\n";
            cout << "  " << GREEN << totals.calories << " calories" << RESET
                 << " | " << YELLOW << "P:" << fixed << setprecision(0) << totals.protein << "g" << RESET
                 << " " << GREEN << "C:" << totals.carbs << "g" << RESET
                 << " " << CYAN << "F:" << totals.fats << "g" << RESET << "\n\n";
            UIUtils::printSeparator();
            
            cout << "\n  Enter number to " << RED << "DELETE" << RESET
                 << " item, or " << YELLOW << "0" << RESET << " to go back: ";
            int choice;
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(1000, '\n');
                choice = -1;
            }
            
            if (choice == 0) {
                return;
            } else if (choice > 0 && choice <= static_cast<int>(itemMap.size())) {
                const auto& item = itemMap[choice - 1];
                if (menuManager.removeLoggedMeal(currentUser, dateStr,
                                                 item.mealType, item.foodName)) {
                    auth.updateUser(currentUser);
                    cout << "\n  " << GREEN << "Item deleted." << RESET << "\n";
                } else {
                    cout << "\n  " << RED << "Error deleting item." << RESET << "\n";
                    UIUtils::waitForEnter();
                }
            } else {
                cout << "\n  " << RED << "Invalid choice." << RESET << "\n";
                UIUtils::waitForEnter();
            }
        }
    }
}
