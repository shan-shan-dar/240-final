#include "UI.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <ctime>
#include <algorithm>
#include <vector>

using namespace std;

// Constructor
UI::UI() : isLoggedIn(false) {}

// Clear the terminal screen
void UI::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Print a fancy header with the given title
void UI::printHeader(const string& title) {
    const string CYAN = "\033[36m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    cout << "\n";
    cout << "  " << CYAN << BOLD << "=== " << title << " ===" << RESET << "\n";
}

// Print a line separator
void UI::printSeparator() {
    cout << "  " << string(50, '-') << "\n";
}

// Wait for user to press Enter
void UI::waitForEnter() {
    cout << "\n  Press ENTER...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Fetch/generate the menu JSON for a given date and meal type by
// invoking the Python script menu.py. This ensures that the latest
// Nutrislice data is available before loading it in C++.
bool UI::fetchMenuFor(const string& date, const string& mealType) {
    // Build the base command arguments: date and meal type.
    string args = " " + date + " " + mealType;

#ifdef _WIN32
    // On Windows just call `python` and let the environment decide.
    string cmd = "python menu.py" + args;
#else
    // On Unix-like systems, try python3 first, then fall back to python.
    // Suppress script output so it doesn't clutter the UI.
    string cmd =
        "python3 menu.py" + args + " > /dev/null 2>&1"
        " || python menu.py" + args + " > /dev/null 2>&1";
#endif

    int result = system(cmd.c_str());
    (void)result; // We rely on later file loading to detect failures.
    return true;
}

// Show the welcome screen with logo and options
void UI::showWelcomeScreen() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string MAGENTA = "\033[35m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    clearScreen();
    cout << "\n\n";
    cout << "  " << CYAN << BOLD << "╔═══════════════════════════════════════╗" << RESET << "\n";
    cout << "  " << CYAN << BOLD << "║   " << MAGENTA << "🍽️  MACRO MEAL TRACKER  🍽️" << CYAN << "    ║" << RESET << "\n";
    cout << "  " << CYAN << BOLD << "╚═══════════════════════════════════════╝" << RESET << "\n";
    cout << "\n";
    cout << "  " << GREEN << "Track nutrition. Meet goals. Stay healthy." << RESET << "\n";
    cout << "\n";
    printSeparator();
    cout << "\n";
    cout << "     " << YELLOW << "[1]" << RESET << " " << BOLD << "Login" << RESET << "\n";
    cout << "     " << YELLOW << "[2]" << RESET << " " << BOLD << "Register" << RESET << "\n";
    cout << "     " << YELLOW << "[3]" << RESET << " " << BOLD << "Exit" << RESET << "\n";
    cout << "\n";
    printSeparator();
    cout << "\n";
    cout << "  >> Choice: ";
}

// Show the login screen
void UI::showLoginScreen() {
    const string CYAN = "\033[36m";
    const string GREEN = "\033[32m";
    const string RED = "\033[31m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    clearScreen();
    printHeader("LOGIN TO YOUR ACCOUNT");
    
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
        // Small delay for UX
    }
    cout << "\n\n";
    
    if (auth.login(username, password, currentUser)) {
        isLoggedIn = true;
        printSeparator();
        cout << "\n";
        cout << "     " << GREEN << BOLD << "SUCCESS!" << RESET << " Welcome back, " << CYAN << currentUser.username << RESET << "!\n";
        cout << "\n";
        printSeparator();
        waitForEnter();
    } else {
        printSeparator();
        cout << "\n";
        cout << "     " << RED << BOLD << "ERROR:" << RESET << " Invalid username or password.\n";
        cout << "     Please check your credentials and try again.\n";
        cout << "\n";
        printSeparator();
        waitForEnter();
    }
}

// Show the registration screen
void UI::showRegisterScreen() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string RED = "\033[31m";
    const string MAGENTA = "\033[35m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    clearScreen();
    printHeader("CREATE YOUR ACCOUNT");
    
    string username, password;
    int calorieGoal, proteinRatio, carbRatio, fatRatio;
    
    cout << "\n";
    cout << "     " << MAGENTA << BOLD << "STEP 1:" << RESET << " Account Information\n";
    printSeparator();
    cout << "\n";
    cout << "  >> Choose a " << CYAN << "username" << RESET << ": ";
    cin >> username;
    
    cout << "  >> Choose a " << CYAN << "password" << RESET << ": ";
    cin >> password;
    
    cout << "\n";
    cout << "     " << MAGENTA << BOLD << "STEP 2:" << RESET << " Nutrition Goals\n";
    printSeparator();
    cout << "\n";
    cout << "     Set your daily calorie target (typical: " << YELLOW << "1800-2500" << RESET << ")\n";
    cout << "  >> Daily " << GREEN << "calorie goal" << RESET << ": ";
    cin >> calorieGoal;
    
    cout << "\n";
    cout << "     " << MAGENTA << BOLD << "STEP 3:" << RESET << " Macro Ratio\n";
    printSeparator();
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
        printSeparator();
        cout << "\n";
        cout << "     " << GREEN << BOLD << "SUCCESS!" << RESET << " Account created successfully!\n";
        cout << "     You can now log in with your credentials.\n";
        cout << "\n";
        printSeparator();
    } else {
        printSeparator();
        cout << "\n";
        cout << "     " << RED << BOLD << "ERROR:" << RESET << " Username '" << CYAN << username << RESET << "' already exists.\n";
        cout << "     Please try a different username.\n";
        cout << "\n";
        printSeparator();
    }
    
    waitForEnter();
}

// Show the main menu after login
void UI::showMainMenu() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string RED = "\033[31m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    while (isLoggedIn) {
        clearScreen();
        printHeader("MAIN MENU");
        
        // Get current date for daily tracking
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char dateStr[100];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", ltm);
        
        // Calculate today's totals and remaining
        auto totals = menuManager.calculateDailyTotals(currentUser, dateStr);
        
        double proteinGoal = (currentUser.calorieGoal * currentUser.macroRatio.protein) / 4.0;
        double carbsGoal = (currentUser.calorieGoal * currentUser.macroRatio.carbs) / 4.0;
        double fatsGoal = (currentUser.calorieGoal * currentUser.macroRatio.fats) / 9.0;
        
        cout << "\n";
        cout << "  User: " << CYAN << BOLD << currentUser.username << RESET << "\n";
        cout << "\n";
        printSeparator();
        
        // Calculate percentages
        int calPercent = (currentUser.calorieGoal > 0) ? (totals.calories * 100 / currentUser.calorieGoal) : 0;
        int proteinPercent = (proteinGoal > 0) ? (totals.protein * 100 / proteinGoal) : 0;
        int carbsPercent = (carbsGoal > 0) ? (totals.carbs * 100 / carbsGoal) : 0;
        int fatsPercent = (fatsGoal > 0) ? (totals.fats * 100 / fatsGoal) : 0;
        
        // Display progress with visual bars
        cout << "  " << BOLD << "Today's Progress:" << RESET << "\n\n";
        
        // Calories
        string calColor = (calPercent <= 100) ? GREEN : RED;
        string calStatus = (calPercent <= 100) ? "On Track" : "Over Goal";
        cout << "  " << GREEN << "●" << RESET << " Calories: " << BOLD << totals.calories << "/" 
             << currentUser.calorieGoal << RESET << " (" << calColor << calPercent << "%" << RESET << ")\n";
        cout << "    " << calColor;
        int calBars = min(calPercent / 5, 20);
        for (int i = 0; i < calBars; i++) cout << "█";
        for (int i = calBars; i < 20; i++) cout << "░";
        cout << RESET << " " << calColor << calStatus << RESET << "\n";
        
        // Protein
        string proteinColor = (proteinPercent <= 100) ? YELLOW : RED;
        string proteinStatus = (proteinPercent <= 100) ? "On Track" : "Over Goal";
        cout << "  " << YELLOW << "●" << RESET << " Protein: " << BOLD << static_cast<int>(totals.protein) 
             << "/" << static_cast<int>(proteinGoal) << "g" << RESET << " (" << proteinColor 
             << proteinPercent << "%" << RESET << ")\n";
        cout << "    " << proteinColor;
        int proteinBars = min(proteinPercent / 5, 20);
        for (int i = 0; i < proteinBars; i++) cout << "█";
        for (int i = proteinBars; i < 20; i++) cout << "░";
        cout << RESET << " " << proteinColor << proteinStatus << RESET << "\n";
        
        // Carbs
        string carbsColor = (carbsPercent <= 100) ? GREEN : RED;
        string carbsStatus = (carbsPercent <= 100) ? "On Track" : "Over Goal";
        cout << "  " << GREEN << "●" << RESET << " Carbs: " << BOLD << static_cast<int>(totals.carbs) 
             << "/" << static_cast<int>(carbsGoal) << "g" << RESET << " (" << carbsColor 
             << carbsPercent << "%" << RESET << ")\n";
        cout << "    " << carbsColor;
        int carbsBars = min(carbsPercent / 5, 20);
        for (int i = 0; i < carbsBars; i++) cout << "█";
        for (int i = carbsBars; i < 20; i++) cout << "░";
        cout << RESET << " " << carbsColor << carbsStatus << RESET << "\n";
        
        // Fats
        string fatsColor = (fatsPercent <= 100) ? CYAN : RED;
        string fatsStatus = (fatsPercent <= 100) ? "On Track" : "Over Goal";
        cout << "  " << CYAN << "●" << RESET << " Fats: " << BOLD << static_cast<int>(totals.fats) 
             << "/" << static_cast<int>(fatsGoal) << "g" << RESET << " (" << fatsColor 
             << fatsPercent << "%" << RESET << ")\n";
        cout << "    " << fatsColor;
        int fatsBars = min(fatsPercent / 5, 20);
        for (int i = 0; i < fatsBars; i++) cout << "█";
        for (int i = fatsBars; i < 20; i++) cout << "░";
        cout << RESET << " " << fatsColor << fatsStatus << RESET << "\n";
        
        // Overall status
        cout << "\n";
        int avgPercent = calPercent;//(calPercent + proteinPercent + carbsPercent + fatsPercent) / 4;
        if (avgPercent == 0) {
            cout << "  " << YELLOW << "💡 Tip:" << RESET << " Start logging meals to track your progress!\n";
        } else if (avgPercent < 50) {
            cout << "  " << GREEN << "💪 Keep going!" << RESET << " You have " << (100 - avgPercent) << "% left to reach your goals.\n";
        } else if (avgPercent < 90) {
            cout << "  " << GREEN << "🔥 Great progress!" << RESET << " You're over halfway there!\n";
        } else if (avgPercent <= 100) {
            cout << "  " << GREEN << "🎯 Almost there!" << RESET << " You're very close to your goals!\n";
        } else {
            cout << "  " << RED << "⚠️  Over goal!" << RESET << " Consider lighter options for remaining meals.\n";
        }
        
        cout << "\n";
        printSeparator();
        cout << "\n";
        cout << "     " << YELLOW << "[1]" << RESET << " View menus\n";
        cout << "     " << YELLOW << "[2]" << RESET << " View logged foods\n";
        cout << "     " << YELLOW << "[3]" << RESET << " Log a meal\n";
        cout << "     " << YELLOW << "[4]" << RESET << " Generate meal plan\n";
        cout << "     " << YELLOW << "[5]" << RESET << " Edit profile\n";
        cout << "     " << YELLOW << "[6]" << RESET << " Logout\n";
        cout << "\n";
        printSeparator();
        cout << "\n";
        cout << "  >> Choice: ";
        
        int choice;
        cin >> choice;
        
        switch (choice) {
            case 1:
                showDailyMenu();
                break;
            case 2:
                showLoggedFoods();
                break;
            case 3:
                showFoodLogger();
                break;
            case 4:
                showMealGenerator();
                break;
            case 5:
                showProfileEditor();
                break;
            case 6:
                clearScreen();
                printHeader("LOGOUT");
                cout << "\n\n";
                cout << "     Thank you for using Macro Meal Tracker, " << CYAN << currentUser.username << RESET << "!\n";
                cout << "     " << GREEN << "Your progress has been saved." << RESET << "\n";
                cout << "\n";
                cout << "     Come back soon to track your nutrition goals!\n\n";
                isLoggedIn = false;
                waitForEnter();
                break;
            default:
                cout << "\n\n";
                cout << "     Invalid choice. Please enter a number between 1 and 5.\n";
                waitForEnter();
        }
    }
}

// Show profile editing screen
void UI::showProfileEditor() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    clearScreen();
    printHeader("EDIT PROFILE & PREFERENCES");
    
    cout << "\n";
    cout << "     Your Current Settings:\n";
    printSeparator();
    cout << "\n";
    cout << "       Daily Calorie Goal: " << GREEN << currentUser.calorieGoal << " calories" << RESET << "\n";
    cout << "       " << YELLOW << "Protein" << RESET << ": " << fixed << setprecision(0) 
              << (currentUser.macroRatio.protein * 100) << "%\n";
    cout << "       " << GREEN << "Carbs" << RESET << ": " << (currentUser.macroRatio.carbs * 100) << "%\n";
    cout << "       " << CYAN << "Fats" << RESET << ": " << (currentUser.macroRatio.fats * 100) << "%\n";
    cout << "\n";
    printSeparator();
    cout << "\n";
    cout << "     What would you like to update?\n\n";
    cout << "     " << YELLOW << "[1]" << RESET << " Update calorie goal\n";
    cout << "     " << YELLOW << "[2]" << RESET << " Update macro ratios\n";
    cout << "     " << YELLOW << "[3]" << RESET << " Return to main menu\n";
    cout << "\n";
    printSeparator();
    cout << "\n";
    cout << "  >> Enter your choice (1-3): ";
    
    int choice;
    cin >> choice;
    
    if (choice == 1) {
        cout << "\n";
        printSeparator();
        cout << "\n";
        cout << "  >> Enter new daily calorie goal: ";
        cin >> currentUser.calorieGoal;
        auth.updateUser(currentUser);
        cout << "\n";
        printSeparator();
        cout << "\n";
        cout << "     " << GREEN << BOLD << "SUCCESS!" << RESET << " Calorie goal updated to " << GREEN << currentUser.calorieGoal << " calories" << RESET << ".\n";
        cout << "\n";
        printSeparator();
        waitForEnter();
    } else if (choice == 2) {
        int p, c, f;
        cout << "\n";
        printSeparator();
        cout << "\n";
        cout << "     Enter new macro ratio (Protein Carbs Fats)\n";
        cout << "     Example: 3 5 2 for 30% protein, 50% carbs, 20% fats\n";
        cout << "\n";
        cout << "  >> New ratio: ";
        cin >> p >> c >> f;
        
        int total = p + c + f;
        currentUser.macroRatio.protein = static_cast<double>(p) / total;
        currentUser.macroRatio.carbs = static_cast<double>(c) / total;
        currentUser.macroRatio.fats = static_cast<double>(f) / total;
        
        auth.updateUser(currentUser);
        cout << "\n";
        printSeparator();
        cout << "\n";
        cout << "     " << GREEN << BOLD << "SUCCESS!" << RESET << " Macro ratios updated to:\n";
        cout << "       " << YELLOW << "Protein" << RESET << ": " << fixed << setprecision(0) << (currentUser.macroRatio.protein * 100) << "%\n";
        cout << "       " << GREEN << "Carbs" << RESET << ": " << (currentUser.macroRatio.carbs * 100) << "%\n";
        cout << "       " << CYAN << "Fats" << RESET << ": " << (currentUser.macroRatio.fats * 100) << "%\n";
        cout << "\n";
        printSeparator();
        waitForEnter();
    }
}

void UI::showDailyMenu() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string RED = "\033[31m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";

    clearScreen();
    printHeader("VIEW DINING HALL MENUS");

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
        printSeparator();
        cout << "\n  " << RED << "Invalid choice. Returning to main menu." << RESET << "\n";
        printSeparator();
        waitForEnter();
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
            printSeparator();
            cout << "\n  " << RED << "Invalid choice. Returning to main menu." << RESET << "\n";
            printSeparator();
            waitForEnter();
            return;
    }

    // Call menu.py to fetch/generate JSON for this date + meal
    fetchMenuFor(dateStr, mealType);

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

    clearScreen();
    printHeader("DINING HALL MENU");

    cout << "\n  " << CYAN << BOLD << friendlyDateStr << RESET
         << " - " << YELLOW << mealType << RESET << "\n\n";

    try {
        auto menu = menuManager.getDailyMenu(mealType, dateStr);

        if (menu.empty()) {
            cout << "\n";
            printSeparator();
            cout << "\n  No menu items found for this selection.\n";
            printSeparator();
        } else {
            menuManager.displayMenuTable(menu);

            cout << "\n";
            printSeparator();
            cout << "\n  " << GREEN << BOLD << "Your Targets:" << RESET
                 << " Cal:" << GREEN << currentUser.calorieGoal << RESET
                 << " " << YELLOW << "P:" << fixed << setprecision(0)
                 << (currentUser.calorieGoal * currentUser.macroRatio.protein / 4.0) << "g" << RESET
                 << " " << GREEN << "C:" << (currentUser.calorieGoal * currentUser.macroRatio.carbs / 4.0) << "g" << RESET
                 << " " << CYAN << "F:" << (currentUser.calorieGoal * currentUser.macroRatio.fats / 9.0) << "g" << RESET << "\n";
            printSeparator();
        }
    } catch (const exception& e) {
        cout << "\n";
        printSeparator();
        cout << "\n  " << RED << BOLD << "ERROR:" << RESET << " " << e.what() << "\n";
        printSeparator();
    }

    waitForEnter();
}

// Helper struct to hold meal-specific nutritional targets
struct MealTargets {
    double calories;
    double protein;
    double carbs;
    double fats;
};

void UI::showMealGenerator() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";

    clearScreen();
    printHeader("PERSONALIZED MEAL PLAN GENERATOR");

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
    printSeparator();
    
    // 2. Define Meal Budgets (e.g., 30% Breakfast, 40% Lunch, 30% Dinner)
    map<string, MealTargets> mealBudgets;
    mealBudgets["breakfast"] = {currentUser.calorieGoal * 0.30, proteinGoal * 0.30, carbsGoal * 0.30, fatsGoal * 0.30};
    mealBudgets["lunch"] = {currentUser.calorieGoal * 0.40, proteinGoal * 0.40, carbsGoal * 0.40, fatsGoal * 0.40};
    mealBudgets["dinner"] = {currentUser.calorieGoal * 0.30, proteinGoal * 0.30, carbsGoal * 0.30, fatsGoal * 0.30};

    // 3. Fetch All Available Menus for Today
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char dateStr[11];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", ltm);

    fetchMenuFor(dateStr, "breakfast");
    fetchMenuFor(dateStr, "lunch");
    fetchMenuFor(dateStr, "dinner");

    map<string, vector<FoodItem>> allMenus;
    allMenus["breakfast"] = menuManager.getDailyMenu("breakfast", dateStr);
    allMenus["lunch"] = menuManager.getDailyMenu("lunch", dateStr);
    allMenus["dinner"] = menuManager.getDailyMenu("dinner", dateStr);

    if (allMenus["breakfast"].empty() && allMenus["lunch"].empty() && allMenus["dinner"].empty()) {
        cout << "\n  " << YELLOW << "No menus available for today to generate a meal plan." << RESET << "\n";
        waitForEnter();
        return;
    }

    // 4. Algorithm: Find the Best Item for Each Meal's Budget
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
    
    // --- 5. Display the Generated Plan ---
    cout << "\n  " << BOLD << "Generated Meal Plan for Today:" << RESET << "\n\n";
    double totalCalories = 0, totalProtein = 0, totalCarbs = 0, totalFats = 0;

    vector<string> mealOrder = {"breakfast", "lunch", "dinner"};
    for (const string& mealType : mealOrder) {
        if (selectedMeals.count(mealType)) {
            const auto& item = selectedMeals[mealType];
            string mealName = mealType;
            mealName[0] = toupper(mealName[0]); // Capitalize
            
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

    printSeparator();
    cout << "\n  " << BOLD << "Plan Summary:" << RESET << "\n";
    cout << "  - Total Calories: " << GREEN << static_cast<int>(totalCalories) << " / " << currentUser.calorieGoal << RESET << "\n";
    cout << "  - Total Protein:  " << YELLOW << static_cast<int>(totalProtein) << "g / " << static_cast<int>(proteinGoal) << "g" << RESET << "\n";
    cout << "  - Total Carbs:    " << GREEN << static_cast<int>(totalCarbs) << "g / " << static_cast<int>(carbsGoal) << "g" << RESET << "\n";
    cout << "  - Total Fats:     " << CYAN << static_cast<int>(totalFats) << "g / " << static_cast<int>(fatsGoal) << "g" << RESET << "\n\n";

    waitForEnter();
}

// // Show the meal plan generator
// void UI::showMealGenerator() {
//     const string YELLOW = "\033[33m";
//     const string CYAN = "\033[36m";
//     const string BOLD = "\033[1m";
//     const string RESET = "\033[0m";
    
//     clearScreen();
//     printHeader("PERSONALIZED MEAL PLAN GENERATOR");
    
//     cout << "\n";
//     cout << "     " << YELLOW << BOLD << "This feature is currently under development." << RESET << "\n";
//     cout << "\n";
//     printSeparator();
//     cout << "\n";
//     cout << "     " << CYAN << "Coming soon:" << RESET << "\n";
//     cout << "       - AI-powered meal recommendations\n";
//     cout << "       - Optimized macro targeting\n";
//     cout << "       - Personalized nutrition plans\n";
//     cout << "\n";
//     printSeparator();
    
//     waitForEnter();
// }

// Show food logger screen
void UI::showFoodLogger() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string RED = "\033[31m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    clearScreen();
    printHeader("LOG A MEAL");
    
    // Get current date in YYYY-MM-DD format
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char dateStr[100];
    char friendlyDate[100];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", ltm);  // e.g., "2025-11-19"
    strftime(friendlyDate, sizeof(friendlyDate), "%A, %B %d, %Y", ltm);  // e.g., "Tuesday, November 19, 2025"
    
    cout << "\n";
    cout << "  Date: " << CYAN << BOLD << friendlyDate << RESET << "\n";
    cout << "\n";
    printSeparator();
    cout << "\n";
    cout << "     Which meal would you like to log?\n\n";
    cout << "     " << YELLOW << "[1]" << RESET << " Breakfast\n";
    cout << "     " << YELLOW << "[2]" << RESET << " Lunch\n";
    cout << "     " << YELLOW << "[3]" << RESET << " Dinner\n";
    cout << "     " << YELLOW << "[4]" << RESET << " Go back\n";
    cout << "\n";
    printSeparator();
    cout << "\n";
    cout << "  >> Enter your choice (1-4): ";
    
    int mealChoice;
    cin >> mealChoice;
    
    if (mealChoice < 1 || mealChoice > 4) {
        cout << "\n\n";
        cout << "     Invalid choice.\n";
        waitForEnter();
        return;
    }
    
    if (mealChoice == 4) {
        return;  // Go back to main menu
    }

    string mealType;
    switch (mealChoice) {
        case 1: mealType = "breakfast"; break;
        case 2: mealType = "lunch"; break;
        case 3: mealType = "dinner"; break;
    }

    // Fetch/generate the menu JSON for this date and meal before loading it.
    fetchMenuFor(dateStr, mealType);

    // Load menu once
    auto menu = menuManager.getDailyMenu(mealType, dateStr);
    
    if (menu.empty()) {
        clearScreen();
        printHeader("LOG A MEAL - " + string(mealType));
        cout << "\n";
        printSeparator();
        cout << "\n";
        cout << "     No menu available for " << mealType << " on " << friendlyDate << "\n";
        cout << "     Please check back later or try a different meal.\n";
        cout << "\n";
        printSeparator();
        waitForEnter();
        return;
    }
    
    // Keep logging until user types "done"
    string input;
    while (true) {
        clearScreen();
        printHeader("LOG A MEAL - " + string(mealType));
        cout << "\n";
        cout << "  Date: " << CYAN << BOLD << friendlyDate << RESET << "\n";
        cout << "\n";
        
        menuManager.displayMenuTable(menu);
        
        cout << "\n";
        printSeparator();
        cout << "\n";
        cout << "  >> Enter item number (or type '" << YELLOW << "done" << RESET << "' to finish): ";
        
        cin >> input;
        
        // Check if user wants to finish
        if (input == "done" || input == "Done" || input == "DONE") {
            break;
        }
        
        // Try to parse as number
        int itemNumber;
        try {
            itemNumber = stoi(input);
        } catch (...) {
            cout << "\n";
            cout << "     " << RED << "Invalid input." << RESET << " Please enter a number or 'done'.\n";
            waitForEnter();
            continue;
        }
        
        if (itemNumber < 1 || itemNumber > static_cast<int>(menu.size())) {
            cout << "\n";
            cout << "     " << RED << "Invalid item number." << RESET << " Please try again.\n";
            waitForEnter();
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
            waitForEnter();
            continue;
        }
        
        // Log the meal
        bool success = menuManager.logMeal(currentUser, mealType, dateStr, itemNumber, servings);
        
        if (success) {
            auth.updateUser(currentUser);
            cout << "\n";
            printSeparator();
            cout << "\n";
            cout << "     " << GREEN << BOLD << "✓ Logged!" << RESET << " " << CYAN << menu[itemNumber - 1].name << RESET 
                 << " x" << YELLOW << fixed << setprecision(1) << servings << RESET 
                 << " (" << GREEN << static_cast<int>(menu[itemNumber - 1].calories * servings) << " cal" << RESET << ")\n";
            cout << "\n";
            printSeparator();
            cout << "\n";
            cout << "  Press ENTER to log another item...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        } else {
            cout << "\n";
            cout << "     " << RED << BOLD << "Failed to log meal." << RESET << " Please try again.\n";
            waitForEnter();
        }
    }
}

// Show logged foods for today
void UI::showLoggedFoods() {
    const string CYAN = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN = "\033[32m";
    const string MAGENTA = "\033[35m";
    const string BOLD = "\033[1m";
    const string RESET = "\033[0m";
    
    clearScreen();
    printHeader("TODAY'S LOGGED FOODS");
    
    // Get current date
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char dateStr[100];
    char friendlyDate[100];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", ltm);
    strftime(friendlyDate, sizeof(friendlyDate), "%A, %B %d, %Y", ltm);
    
    cout << "\n  " << CYAN << BOLD << friendlyDate << RESET << "\n\n";
    
    // Check if any foods logged today
    auto dateIt = currentUser.loggedMeals.find(dateStr);
    if (dateIt == currentUser.loggedMeals.end() || dateIt->second.empty()) {
        printSeparator();
        cout << "\n  No foods logged today yet.\n";
        cout << "  Use option [3] from main menu to log meals.\n\n";
        printSeparator();
        waitForEnter();
        return;
    }
    
    // Display logged foods by meal type
    const vector<string> mealTypes = {"breakfast", "lunch", "dinner"};
    bool hasAnyFood = false;
    
    for (const auto& mealType : mealTypes) {
        auto mealIt = dateIt->second.find(mealType);
        if (mealIt != dateIt->second.end() && !mealIt->second.empty()) {
            hasAnyFood = true;
            
            // Capitalize meal type for display
            string displayMeal = mealType;
            displayMeal[0] = toupper(displayMeal[0]);
            
            cout << "  " << MAGENTA << BOLD << displayMeal << RESET << "\n";
            printSeparator();
            
            // Load menu to get nutrition info
            auto menu = menuManager.getDailyMenu(mealType, dateStr);
            
            // Display each food item
            for (const auto& foodEntry : mealIt->second) {
                string foodName = foodEntry.first;
                double servings = foodEntry.second;
                
                // Find nutrition info
                for (const auto& item : menu) {
                    if (item.name == foodName) {
                        int totalCals = static_cast<int>(item.calories * servings);
                        double totalProtein = item.protein * servings;
                        double totalCarbs = item.carbs * servings;
                        double totalFats = item.fats * servings;
                        
                        cout << "  " << CYAN << foodName << RESET << " x" << fixed << setprecision(1) << servings << "\n";
                        cout << "    " << GREEN << totalCals << " cal" << RESET
                             << " | " << YELLOW << "P:" << setprecision(0) << totalProtein << "g" << RESET
                             << " " << GREEN << "C:" << totalCarbs << "g" << RESET
                             << " " << CYAN << "F:" << totalFats << "g" << RESET << "\n";
                        break;
                    }
                }
            }
            cout << "\n";
        }
    }
    
    if (!hasAnyFood) {
        printSeparator();
        cout << "\n  No foods logged today yet.\n";
        cout << "  Use option [3] from main menu to log meals.\n\n";
        printSeparator();
    } else {
        // Show daily totals
        auto totals = menuManager.calculateDailyTotals(currentUser, dateStr);
        printSeparator();
        cout << "\n  " << BOLD << "Daily Totals:" << RESET << "\n";
        cout << "  " << GREEN << totals.calories << " calories" << RESET
             << " | " << YELLOW << "P:" << fixed << setprecision(0) << totals.protein << "g" << RESET
             << " " << GREEN << "C:" << totals.carbs << "g" << RESET
             << " " << CYAN << "F:" << totals.fats << "g" << RESET << "\n\n";
        printSeparator();
    }
    
    waitForEnter();
}

// Main run loop for the application
void UI::run() {
    int choice;
    
    do {
        showWelcomeScreen();
        cin >> choice;
        
        switch (choice) {
            case 1:
                showLoginScreen();
                if (isLoggedIn) {
                    showMainMenu();
                }
                break;
            case 2:
                showRegisterScreen();
                break;
            case 3: {
                clearScreen();
                cout << "\n\n";
                const string CYAN = "\033[36m";
                const string MAGENTA = "\033[35m";
                const string GREEN = "\033[32m";
                const string BOLD = "\033[1m";
                const string RESET = "\033[0m";
                
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
                waitForEnter();
        }
    } while (choice != 3);
}
