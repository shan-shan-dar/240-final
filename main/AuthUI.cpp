#include "AuthUI.h"
#include "UIUtils.h"
#include <iostream>

using namespace std;

AuthUI::AuthUI(Auth& authRef, User& userRef, bool& loggedInRef) 
    : auth(authRef), currentUser(userRef), isLoggedIn(loggedInRef) {}

void AuthUI::showWelcomeScreen() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string MAGENTA = "\033[35m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    UIUtils::clearScreen();
    cout << "\n\n";
    cout << "  " << CYAN << BOLD << "╔═══════════════════════════════════════╗" << RESET << "\n";
    cout << "  " << CYAN << BOLD << "║   " << MAGENTA << "🍽️  MACRO MEAL TRACKER  🍽️" << CYAN << "    ║" << RESET << "\n";
    cout << "  " << CYAN << BOLD << "╚═══════════════════════════════════════╝" << RESET << "\n";
    cout << "\n";
    cout << "  " << GREEN << "Track nutrition. Meet goals. Stay healthy." << RESET << "\n";
    cout << "\n";
    UIUtils::printSeparator();
    cout << "\n";
    cout << "     " << YELLOW << "[1]" << RESET << " " << BOLD << "Login" << RESET << "\n";
    cout << "     " << YELLOW << "[2]" << RESET << " " << BOLD << "Register" << RESET << "\n";
    cout << "     " << YELLOW << "[3]" << RESET << " " << BOLD << "Exit" << RESET << "\n";
    cout << "\n";
    UIUtils::printSeparator();
    cout << "\n";
    cout << "  >> Choice: ";
}

void AuthUI::showLoginScreen() {
    const string CYAN = "\033[36m";
    const string GREEN = "\033[32m";
    const string RED = "\033[31m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    UIUtils::clearScreen();
    UIUtils::printHeader("LOGIN TO YOUR ACCOUNT");
    
    string username, password;
    
    cout << "\n";
    cout << "     Please enter your credentials:\n\n";
    cout << "  >> " << CYAN << "Username" << RESET << ": ";
    cin >> username;
    
    cout << "  >> " << CYAN << "Password" << RESET << ": ";
    cin >> password;
    
    cout << "\n";
    cout << "     Authenticating";
    for (int i = 0; i < 3; i++) {
        cout << "." << flush;
    }
    cout << "\n\n";
    
    if (auth.login(username, password, currentUser)) {
        isLoggedIn = true;
        UIUtils::printSeparator();
        cout << "\n";
        cout << "     " << GREEN << BOLD << "SUCCESS!" << RESET << " Welcome back, " << CYAN << currentUser.username << RESET << "!\n";
        cout << "\n";
        UIUtils::printSeparator();
        UIUtils::waitForEnter();
    } else {
        UIUtils::printSeparator();
        cout << "\n";
        cout << "     " << RED << BOLD << "ERROR:" << RESET << " Invalid username or password.\n";
        cout << "     Please check your credentials and try again.\n";
        cout << "\n";
        UIUtils::printSeparator();
        UIUtils::waitForEnter();
    }
}

void AuthUI::showRegisterScreen() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string RED = "\033[31m";
    const string MAGENTA = "\033[35m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    UIUtils::clearScreen();
    UIUtils::printHeader("CREATE YOUR ACCOUNT");
    
    string username, password;
    int calorieGoal, proteinRatio, carbRatio, fatRatio;
    
    cout << "\n";
    cout << "     " << MAGENTA << BOLD << "STEP 1:" << RESET << " Account Information\n";
    UIUtils::printSeparator();
    cout << "\n";
    cout << "  >> Choose a " << CYAN << "username" << RESET << ": ";
    cin >> username;
    
    cout << "  >> Choose a " << CYAN << "password" << RESET << ": ";
    cin >> password;
    
    cout << "\n";
    cout << "     " << MAGENTA << BOLD << "STEP 2:" << RESET << " Nutrition Goals\n";
    UIUtils::printSeparator();
    cout << "\n";
    cout << "     Set your daily calorie target (typical: " << YELLOW << "1800-2500" << RESET << ")\n";
    cout << "  >> Daily " << GREEN << "calorie goal" << RESET << ": ";
    cin >> calorieGoal;
    
    cout << "\n";
    cout << "     " << MAGENTA << BOLD << "STEP 3:" << RESET << " Macro Ratio\n";
    UIUtils::printSeparator();
    cout << "\n";
    cout << "     Define your macro split using simple ratios.\n";
    cout << "     Example: " << YELLOW << "'4 5 1'" << RESET << " = 40% protein, 50% carbs, 10% fats\n";
    cout << "     Common splits:\n";
    cout << "       - " << GREEN << "Balanced" << RESET << ": 3 5 2 (30% protein, 50% carbs, 20% fats)\n";
    cout << "       - " << YELLOW << "High Protein" << RESET << ": 4 4 2 (40% protein, 40% carbs, 20% fats)\n";
    cout << "       - " << CYAN << "Low Carb" << RESET << ": 3 2 5 (30% protein, 20% carbs, 50% fats)\n";
    cout << "\n";
    cout << "  >> Enter ratio (Protein Carbs Fats): ";
    cin >> proteinRatio >> carbRatio >> fatRatio;
    
    cout << "\n";
    cout << "     Creating your account";
    for (int i = 0; i < 3; i++) {
        cout << "." << flush;
    }
    cout << "\n\n";
    
    if (auth.registerUser(username, password, calorieGoal, proteinRatio, carbRatio, fatRatio)) {
        UIUtils::printSeparator();
        cout << "\n";
        cout << "     " << GREEN << BOLD << "SUCCESS!" << RESET << " Account created successfully!\n";
        cout << "     You can now log in with your credentials.\n";
        cout << "\n";
        UIUtils::printSeparator();
    } else {
        UIUtils::printSeparator();
        cout << "\n";
        cout << "     " << RED << BOLD << "ERROR:" << RESET << " Username '" << CYAN << username << RESET << "' already exists.\n";
        cout << "     Please try a different username.\n";
        cout << "\n";
        UIUtils::printSeparator();
    }
    
    UIUtils::waitForEnter();
}
