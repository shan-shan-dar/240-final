#include "ProfileUI.h"
#include "UIUtils.h"
#include <iostream>
#include <iomanip>

using namespace std;

// Constructs a ProfileUI for editing the current user's settings.
ProfileUI::ProfileUI(User& userRef, Auth& authRef)
    : currentUser(userRef), auth(authRef) {}

// Shows the profile editor and lets the user adjust goals and password.
void ProfileUI::showProfileEditor() {
    const string CYAN   = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN  = "\033[32m";
    const string RED    = "\033[31m";
    const string BOLD   = "\033[1m";
    const string RESET  = "\033[0m";
    
    UIUtils::clearScreen();
    UIUtils::printHeader("EDIT PROFILE & PREFERENCES");
    
    cout << "\n";
    cout << "     Your Current Settings:\n";
    UIUtils::printSeparator();
    cout << "\n";
    cout << "       Daily Calorie Goal: " << GREEN << currentUser.calorieGoal << " calories" << RESET << "\n";
    cout << "       " << YELLOW << "Protein" << RESET << ": " << fixed << setprecision(0) 
         << (currentUser.macroRatio.protein * 100) << "%\n";
    cout << "       " << GREEN << "Carbs"   << RESET << ": " << (currentUser.macroRatio.carbs * 100) << "%\n";
    cout << "       " << CYAN   << "Fats"   << RESET << ": " << (currentUser.macroRatio.fats * 100) << "%\n";
    cout << "\n";
    UIUtils::printSeparator();
    cout << "\n";
    cout << "     What would you like to update?\n\n";
    cout << "     " << YELLOW << "[1]" << RESET << " Update calorie goal\n";
    cout << "     " << YELLOW << "[2]" << RESET << " Update macro ratios\n";
    cout << "     " << YELLOW << "[3]" << RESET << " Change password\n";
    cout << "     " << YELLOW << "[4]" << RESET << " Return to main menu\n";
    cout << "\n";
    UIUtils::printSeparator();
    cout << "\n";
    cout << "  >> Enter your choice (1-4): ";
    
    int choice;
    cin >> choice;
    
    if (choice == 1) {
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n";
        cout << "  >> Enter new daily calorie goal: ";
        cin >> currentUser.calorieGoal;
        auth.updateUser(currentUser);
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n";
        cout << "     " << GREEN << BOLD << "SUCCESS!" << RESET
             << " Calorie goal updated to "
             << GREEN << currentUser.calorieGoal << " calories" << RESET << ".\n";
        cout << "\n";
        UIUtils::printSeparator();
        UIUtils::waitForEnter();

    } else if (choice == 2) {
        int p, c, f;
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n";
        cout << "     Enter new macro ratio (Protein Carbs Fats)\n";
        cout << "     Example: 3 5 2 for 30% protein, 50% carbs, 20% fats\n";
        cout << "\n";
        cout << "  >> New ratio: ";
        cin >> p >> c >> f;
        
        int total = p + c + f;
        currentUser.macroRatio.protein = static_cast<double>(p) / total;
        currentUser.macroRatio.carbs   = static_cast<double>(c) / total;
        currentUser.macroRatio.fats    = static_cast<double>(f) / total;
        
        auth.updateUser(currentUser);
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n";
        cout << "     " << GREEN << BOLD << "SUCCESS!" << RESET << " Macro ratios updated to:\n";
        cout << "       " << YELLOW << "Protein" << RESET << ": "
             << fixed << setprecision(0) << (currentUser.macroRatio.protein * 100) << "%\n";
        cout << "       " << GREEN  << "Carbs"   << RESET << ": "
             << (currentUser.macroRatio.carbs * 100) << "%\n";
        cout << "       " << CYAN   << "Fats"    << RESET << ": "
             << (currentUser.macroRatio.fats * 100) << "%\n";
        cout << "\n";
        UIUtils::printSeparator();
        UIUtils::waitForEnter();

    } else if (choice == 3) {
        string oldPass, newPass;
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n";
        cout << "  >> Enter current password: ";
        cin >> oldPass;
        
        if (oldPass != currentUser.password) {
            cout << "\n     " << RED << "Incorrect password." << RESET << "\n";
            UIUtils::waitForEnter();
            return;
        }
        
        cout << "  >> Enter new password: ";
        cin >> newPass;
        
        currentUser.password = newPass;
        auth.updateUser(currentUser);
        
        cout << "\n";
        UIUtils::printSeparator();
        cout << "\n";
        cout << "     " << GREEN << BOLD << "SUCCESS!" << RESET << " Password updated.\n";
        cout << "\n";
        UIUtils::printSeparator();
        UIUtils::waitForEnter();
    }
}
