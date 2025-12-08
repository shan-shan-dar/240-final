#include "UI.h"
#include "UIUtils.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>

using namespace std;

// Constructs the root UI object and initializes sub-UIs with references.
UI::UI() 
    : menuManager(), 
      auth(), 
      currentUser(), 
      isLoggedIn(false),
      authUI(auth, currentUser, isLoggedIn),
      menuUI(menuManager, currentUser, auth),
      loggerUI(menuManager, currentUser, auth),
      profileUI(currentUser, auth)
{
}

// Runs the top-level loop for welcome, login/registration, and exit.
void UI::run() {
    int choice;
    
    do {
        authUI.showWelcomeScreen();
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(1000, '\n');
            choice = 0;
        }
        
        switch (choice) {
            case 1:
                authUI.showLoginScreen();
                if (!currentUser.username.empty()) {
                    isLoggedIn = true;
                    showMainMenu();
                    isLoggedIn = false;
                    currentUser = User();
                }
                break;
            case 2:
                authUI.showRegisterScreen();
                break;
            case 3: {
                UIUtils::clearScreen();
                cout << "\n\n";
                const string CYAN    = "\033[36m";
                const string MAGENTA = "\033[35m";
                const string GREEN   = "\033[32m";
                const string GRAY    = "\033[100m";
                const string BOLD    = "\033[1m";
                const string RESET   = "\033[0m";
                
                cout << "  " << CYAN << BOLD << "╔═══════════════════════════════════════════════════╗" << RESET << "\n";
                cout << "  " << CYAN << BOLD << "║                                                   ║" << RESET << "\n";
                cout << "  " << CYAN << BOLD << "║           " << MAGENTA << "Thank you for using" << CYAN << "              ║" << RESET << "\n";
                cout << "  " << CYAN << BOLD << "║           " << MAGENTA << "MACRO MEAL TRACKER" << CYAN << "               ║" << RESET << "\n";
                cout << "  " << CYAN << BOLD << "║                                                   ║" << RESET << "\n";
                cout << "  " << CYAN << BOLD << "║      " << GREEN << "Stay healthy and eat well! 🍎" << CYAN << "          ║" << RESET << "\n";
                cout << "  " << CYAN << BOLD << "║                                                   ║" << RESET << "\n";
                cout << "  " << CYAN << BOLD << "╚═══════════════════════════════════════════════════╝" << RESET << "\n";
                cout << "\n\n";
                break;
            }
            default:
                cout << "\n\n";
                cout << "     Invalid choice. Please enter 1, 2, or 3.\n";
                UIUtils::waitForEnter();
        }
    } while (choice != 3);
}

// Shows the main logged-in menu and routes to feature-specific screens.
void UI::showMainMenu() {
    const string CYAN   = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN  = "\033[32m";
    const string RED    = "\033[31m";
    const string BOLD   = "\033[1m";
    const string GRAY   = "\033[100m";
    const string RESET  = "\033[0m";

    while (isLoggedIn) {
        UIUtils::clearScreen();
        UIUtils::printHeader("MAIN MENU");
        
        cout << "\n";
        cout << "     Welcome back, " << CYAN << BOLD << currentUser.username << RESET << "!\n";
        cout << "\n";
        
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char dateStr[100];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", ltm);
        
        auto totals = menuManager.calculateDailyTotals(currentUser, dateStr);
        
        cout << "     " << BOLD << "Today's Progress:" << RESET << "\n";
        UIUtils::printSeparator();
        
        int calPercent = (currentUser.calorieGoal > 0)
            ? (totals.calories * 100 / currentUser.calorieGoal)
            : 0;
        
        double proteinGoal = (currentUser.calorieGoal * currentUser.macroRatio.protein) / 4.0;
        double carbsGoal   = (currentUser.calorieGoal * currentUser.macroRatio.carbs)   / 4.0;
        double fatsGoal    = (currentUser.calorieGoal * currentUser.macroRatio.fats)    / 9.0;
        
        int proteinPercent = (proteinGoal > 0) ? (totals.protein * 100 / proteinGoal) : 0;
        int carbsPercent   = (carbsGoal   > 0) ? (totals.carbs   * 100 / carbsGoal)   : 0;
        int fatsPercent    = (fatsGoal    > 0) ? (totals.fats    * 100 / fatsGoal)    : 0;
        
        string calColor  = (calPercent > 100) ? RED : GRAY;
        string calStatus = (calPercent > 100) ? "OVER" : to_string(calPercent) + "%";
        
        string calVal = to_string((int)totals.calories) + " / "
                      + to_string((int)currentUser.calorieGoal);
        cout << "     " << left << setw(10) << "Calories:"
             << right << setw(15) << calVal << "  [" << calColor;
        int calBars = min(calPercent / 5, 20);
        for (int i = 0; i < calBars; i++) cout << "█";
        for (int i = calBars; i < 20; i++) cout << "░";
        cout << RESET << "] " << calStatus << RESET << "\n";
        
        string protColor  = (proteinPercent > 100) ? GREEN : YELLOW;
        string protStatus = (proteinPercent > 100)
            ? "DONE"
            : to_string(proteinPercent) + "%";
        string protVal = to_string((int)totals.protein) + " / "
                       + to_string((int)proteinGoal) + "g";
        cout << "     " << left << setw(10) << "Protein:"
             << right << setw(15) << protVal << "  [" << protColor;
        int protBars = min(proteinPercent / 5, 20);
        for (int i = 0; i < protBars; i++) cout << "█";
        for (int i = protBars; i < 20; i++) cout << "░";
        cout << RESET << "] " << protColor << protStatus << RESET << "\n";
        
        string carbsColor  = (carbsPercent > 100) ? RED : GREEN;
        string carbsStatus = (carbsPercent > 100)
            ? "OVER"
            : to_string(carbsPercent) + "%";
        string carbsVal = to_string((int)totals.carbs) + " / "
                        + to_string((int)carbsGoal) + "g";
        cout << "     " << left << setw(10) << "Carbs:"
             << right << setw(15) << carbsVal << "  [" << carbsColor;
        int carbsBars = min(carbsPercent / 5, 20);
        for (int i = 0; i < carbsBars; i++) cout << "█";
        for (int i = carbsBars; i < 20; i++) cout << "░";
        cout << RESET << "] " << carbsColor << carbsStatus << RESET << "\n";
        
        string fatsColor  = (fatsPercent > 100) ? RED : CYAN;
        string fatsStatus = (fatsPercent > 100)
            ? "OVER"
            : to_string(fatsPercent) + "%";
        string fatsVal = to_string((int)totals.fats) + " / "
                       + to_string((int)fatsGoal) + "g";
        cout << "     " << left << setw(10) << "Fats:"
             << right << setw(15) << fatsVal << "  [" << fatsColor;
        int fatsBars = min(fatsPercent / 5, 20);
        for (int i = 0; i < fatsBars; i++) cout << "█";
        for (int i = fatsBars; i < 20; i++) cout << "░";
        cout << RESET << "] " << fatsColor << fatsStatus << RESET << "\n";
        
        cout << "\n";
        int avgPercent = calPercent;
        if (avgPercent == 0) {
            cout << "  " << YELLOW << "💡 Tip:" << RESET
                 << " Start logging meals to track your progress!\n";
        } else if (avgPercent < 50) {
            cout << "  " << GREEN << "💪 Keep going!" << RESET
                 << " You have " << (100 - avgPercent)
                 << "% left to reach your goals.\n";
        } else if (avgPercent < 90) {
            cout << "  " << GREEN << "🔥 Great progress!" << RESET
                 << " You're over halfway there!\n";
        } else if (avgPercent <= 100) {
            cout << "  " << GREEN << "🎯 Almost there!" << RESET
                 << " You're very close to your goals!\n";
        } else {
            cout << "  " << RED << "⚠️  Over goal!" << RESET
                 << " Consider lighter options for remaining meals.\n";
        }
        
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n";
        cout << "     " << YELLOW << "[1]" << RESET << " Log a meal\n";
        cout << "     " << YELLOW << "[2]" << RESET << " View/Edit logged foods\n";
        cout << "     " << YELLOW << "[3]" << RESET << " View menus\n";
        cout << "     " << YELLOW << "[4]" << RESET << " Generate meal plan\n";
        cout << "     " << YELLOW << "[5]" << RESET << " Edit profile\n";
        cout << "     " << YELLOW << "[6]" << RESET << " Logout\n";
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n";
        cout << "  >> Choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                loggerUI.showFoodLogger();
                break;
            case 2:
                loggerUI.showLoggedFoods();
                break;
            case 3:
                menuUI.showDailyMenu();
                break;
            case 4:
                menuUI.showMealGenerator();
                break;
            case 5:
                profileUI.showProfileEditor();
                break;
            case 6:
                UIUtils::clearScreen();
                UIUtils::printHeader("LOGOUT");
                cout << "\n\n";
                cout << "     Thank you for using Macro Meal Tracker, "
                     << CYAN << currentUser.username << RESET << "!\n";
                cout << "     " << GREEN << "Your progress has been saved." << RESET << "\n";
                cout << "\n";
                cout << "     Come back soon to track your nutrition goals!\n\n";
                isLoggedIn = false;
                UIUtils::waitForEnter();
                break;
            default:
                cout << "\n\n";
                cout << "     Invalid choice. Please enter a number between 1 and 6.\n";
                UIUtils::waitForEnter();
        }
    }
}
