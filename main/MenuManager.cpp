#include "MenuManager.h"
#include "UIUtils.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <map>
#include <cmath>
#include <cstdlib>

using namespace std;

// Trims leading and trailing whitespace from a string.
static string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, last - first + 1);
}

// Constructs a menu manager; menus are loaded on demand from JSON files.
MenuManager::MenuManager(const string& filepath) {
    (void)filepath;
}

// Parses a menu JSON file into a list of FoodItem records.
vector<FoodItem> MenuManager::loadMenuFromFile(const string& filename) {
    vector<FoodItem> menu;
    ifstream file(filename);

    if (!file.is_open()) {
        return menu;
    }

    string line;
    string currentStation;

    // Skip the opening brace of the top-level JSON object.
    getline(file, line);

    while (getline(file, line)) {
        // Detect station labels like "Provisions 804": [ while ignoring keys
        // such as "icons" or "serving_size".
        if (line.find("\":") != string::npos && line.find("[") != string::npos) {
            size_t start = line.find("\"") + 1;
            size_t end = line.find("\"", start);
            string possibleStation = line.substr(start, end - start);

            if (possibleStation != "icons" && possibleStation != "serving_size") {
                currentStation = possibleStation;
            }
            continue;
        }

        // Detect the start of a food item by looking for its "name" field.
        if (line.find("\"name\"") != string::npos) {
            FoodItem item;
            item.station = currentStation;

            try {
                // Read item name from the current line.
                size_t start = line.find(":") + 1;
                size_t end = line.find_last_of("\"");
                start = line.find("\"", start) + 1;
                item.name = line.substr(start, end - start);

                // Skip ingredients line.
                getline(file, line);

                // Skip "nutrition": { line.
                getline(file, line);

                // Parse calories.
                getline(file, line);
                start = line.find(":") + 1;
                string calStr = line.substr(start);
                size_t commaPos = calStr.find(",");
                if (commaPos != string::npos) {
                    calStr = calStr.substr(0, commaPos);
                }
                calStr = trim(calStr);
                item.calories = static_cast<int>(stod(calStr));

                // Parse g_fat.
                getline(file, line);
                start = line.find(":") + 1;
                string fatStr = line.substr(start);
                commaPos = fatStr.find(",");
                if (commaPos != string::npos) {
                    fatStr = fatStr.substr(0, commaPos);
                }
                fatStr = trim(fatStr);
                item.fats = stod(fatStr);

                // Parse g_carbs.
                getline(file, line);
                start = line.find(":") + 1;
                string carbStr = line.substr(start);
                commaPos = carbStr.find(",");
                if (commaPos != string::npos) {
                    carbStr = carbStr.substr(0, commaPos);
                }
                carbStr = trim(carbStr);
                item.carbs = stod(carbStr);

                // Parse g_protein.
                getline(file, line);
                start = line.find(":") + 1;
                string proteinStr = line.substr(start);
                commaPos = proteinStr.find(",");
                if (commaPos != string::npos) {
                    proteinStr = proteinStr.substr(0, commaPos);
                }
                proteinStr = trim(proteinStr);
                item.protein = stod(proteinStr);

                // Skip closing brace of the nutrition object.
                getline(file, line);

                // Parse serving size block if present.
                getline(file, line);
                if (line.find("serving_size") != string::npos) {
                    // Parse serving_size_amount.
                    getline(file, line);
                    start = line.find(":") + 1;
                    string amountStr = line.substr(start);
                    commaPos = amountStr.find(",");
                    if (commaPos != string::npos) {
                        amountStr = amountStr.substr(0, commaPos);
                    }
                    amountStr = trim(amountStr);
                    if (!amountStr.empty() && amountStr.front() == '"') {
                        amountStr = amountStr.substr(1);
                    }
                    if (!amountStr.empty() && amountStr.back() == '"') {
                        amountStr.pop_back();
                    }
                    item.servingAmount = amountStr;

                    // Parse serving_size_unit.
                    getline(file, line);
                    start = line.find(":") + 1;
                    string unitStr = line.substr(start);
                    commaPos = unitStr.find(",");
                    if (commaPos != string::npos) {
                        unitStr = unitStr.substr(0, commaPos);
                    }
                    unitStr = trim(unitStr);
                    if (!unitStr.empty() && unitStr.front() == '\"') {
                        unitStr = unitStr.substr(1);
                    }
                    if (!unitStr.empty() && unitStr.back() == '\"') {
                        unitStr.pop_back();
                    }
                    size_t bracePos = unitStr.find('}');
                    if (bracePos != string::npos) {
                        unitStr = unitStr.substr(0, bracePos);
                    }
                    unitStr = trim(unitStr);
                    item.servingUnit = unitStr;
                } else {
                    item.servingAmount = "1";
                    item.servingUnit = "serving";
                }

                menu.push_back(item);
            } catch (const exception& e) {
#ifdef DEBUG
                cerr << "Error parsing item '" << item.name
                     << "': " << e.what() << endl;
#endif
                continue;
            }
        }
    }

    file.close();

    // Sort items by station, then alphabetically within each station.
    sort(menu.begin(), menu.end(), [](const FoodItem& a, const FoodItem& b) {
        if (a.station != b.station) {
            return a.station < b.station;
        }
        return a.name < b.name;
    });

    return menu;
}

// Returns the per-meal menu for a given date using stored JSON files.
vector<FoodItem> MenuManager::getDailyMenu(const string& mealType,
                                           const string& date) {
    string filepath = "../data/menus/" + mealType + "-" + date + ".json";
    return loadMenuFromFile(filepath);
}

// Prints a simple numbered list of all items in a menu.
void MenuManager::displayMenu(const vector<FoodItem>& menu) {
    cout << "\n";
    for (size_t i = 0; i < menu.size(); i++) {
        cout << "  " << (i + 1) << ". " << menu[i].name << "\n";
    }
}

// Prints a formatted table view of the menu grouped by station.
void MenuManager::displayMenuTable(const vector<FoodItem>& menu) {
    const string CYAN   = "\033[36m";
    const string YELLOW = "\033[33m";
    const string GREEN  = "\033[32m";
    const string RESET  = "\033[0m";
    const string BOLD   = "\033[1m";

    string currentStation;
    int itemNum = 1;

    for (const auto& item : menu) {
        // When the station changes, print a new header block.
        if (item.station != currentStation) {
            if (!currentStation.empty()) {
                cout << "\n";
            }
            currentStation = item.station;
            cout << "  " << CYAN << BOLD << currentStation << RESET << "\n";
            cout << "  " << string(95, '-') << "\n";
            cout << "  " << setw(5) << left << "#"
                 << "| " << setw(22) << "Name"
                 << "| " << setw(12) << "Cals/Serving"
                 << "| " << setw(12) << "Protein(g)"
                 << "| " << setw(11) << "Carbs(g)"
                 << "| " << setw(10) << "Fats(g)" << "\n";
            cout << "  " << string(95, '-') << "\n";
        }

        string calDisplay = to_string(item.calories) + "/" + item.servingUnit;

        stringstream proteinDisplay, carbsDisplay, fatsDisplay;
        proteinDisplay << fixed << setprecision(1) << item.protein;
        carbsDisplay   << fixed << setprecision(1) << item.carbs;
        fatsDisplay    << fixed << setprecision(1) << item.fats;

        cout << "  " << YELLOW << "[" << setw(3) << itemNum << "]" << RESET
             << "| " << setw(22) << left << item.name.substr(0, 21)
             << "| " << GREEN << setw(12) << calDisplay.substr(0, 11) << RESET
             << "| " << setw(12) << proteinDisplay.str()
             << "| " << setw(11) << carbsDisplay.str()
             << "| " << setw(10) << fatsDisplay.str() << "\n";

        itemNum++;
    }
}

// Generates a meal plan for today using Nutrislice menus and solver.py.
MenuManager::MealPlanResult MenuManager::generateMealPlan(const User& user) {
    MealPlanResult result{};

    time_t now = time(nullptr);
    tm* ltm = localtime(&now);
    char dateBuf[11];
    strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", ltm);
    result.dateStr = dateBuf;

    result.calorieGoal = user.calorieGoal;
    result.proteinGoal = (user.calorieGoal * user.macroRatio.protein) / 4.0;
    result.carbsGoal   = (user.calorieGoal * user.macroRatio.carbs)   / 4.0;
    result.fatsGoal    = (user.calorieGoal * user.macroRatio.fats)    / 9.0;

    result.loggedTotals = calculateDailyTotals(user, result.dateStr);

    double remainingCalories = result.calorieGoal - result.loggedTotals.calories;
    double remainingProtein  = result.proteinGoal - result.loggedTotals.protein;
    double remainingCarbs    = result.carbsGoal   - result.loggedTotals.carbs;
    double remainingFats     = result.fatsGoal    - result.loggedTotals.fats;

    bool bLogged = false, lLogged = false, dLogged = false;
    auto dateIt = user.loggedMeals.find(result.dateStr);
    if (dateIt != user.loggedMeals.end()) {
        const auto& dayLog = dateIt->second;
        auto itB = dayLog.find("breakfast");
        if (itB != dayLog.end() && !itB->second.empty()) bLogged = true;
        auto itL = dayLog.find("lunch");
        if (itL != dayLog.end() && !itL->second.empty()) lLogged = true;
        auto itD = dayLog.find("dinner");
        if (itD != dayLog.end() && !itD->second.empty()) dLogged = true;
    }

    result.mealLogged["breakfast"] = bLogged;
    result.mealLogged["lunch"]     = lLogged;
    result.mealLogged["dinner"]    = dLogged;

    struct MealTargets {
        double calories;
        double protein;
        double carbs;
        double fats;
    };

    map<string, MealTargets> mealBudgets;

    double bWeight = 0.3;
    double lWeight = 0.4;
    double dWeight = 0.3;
    double totalWeight = 0.0;

    if (!bLogged) totalWeight += bWeight;
    if (!lLogged) totalWeight += lWeight;
    if (!dLogged) totalWeight += dWeight;

    if (totalWeight <= 0.0) {
        return result;
    }

    auto clamp = [](double x) { return x < 0.0 ? 0.0 : x; };

    if (!bLogged) {
        double ratio = bWeight / totalWeight;
        mealBudgets["breakfast"] = {
            clamp(remainingCalories * ratio),
            clamp(remainingProtein  * ratio),
            clamp(remainingCarbs    * ratio),
            clamp(remainingFats     * ratio)
        };
    }
    if (!lLogged) {
        double ratio = lWeight / totalWeight;
        mealBudgets["lunch"] = {
            clamp(remainingCalories * ratio),
            clamp(remainingProtein  * ratio),
            clamp(remainingCarbs    * ratio),
            clamp(remainingFats     * ratio)
        };
    }
    if (!dLogged) {
        double ratio = dWeight / totalWeight;
        mealBudgets["dinner"] = {
            clamp(remainingCalories * ratio),
            clamp(remainingProtein  * ratio),
            clamp(remainingCarbs    * ratio),
            clamp(remainingFats     * ratio)
        };
    }

    // For each unlogged meal, call menu.py and solver.py to build a plan.
    for (const auto& budgetPair : mealBudgets) {
        const string& mealType = budgetPair.first;
        const MealTargets& targets = budgetPair.second;

        UIUtils::fetchMenuFor(result.dateStr, mealType);

        string baseFilename       = mealType + "-" + result.dateStr + ".json";
        string simplifiedFilename = "simplified-" + baseFilename;
        string simplifiedPath     = "../data/menus/" + simplifiedFilename;
        string planPath           = "../data/menus/plan-" + mealType + "-" + result.dateStr + ".txt";

#ifndef _WIN32
        const string PY = "../.venv/bin/python";
        string simplifyCmd =
            PY + " -c \"from menu import simplify_menu_file; "
            "simplify_menu_file('" + baseFilename + "')\""
            " > /dev/null 2>&1";
#else
        string simplifyCmd =
            "python -c \"from menu import simplify_menu_file; "
            "simplify_menu_file('" + baseFilename + "')\""
            " > NUL 2>&1";
#endif

        int simplifyStatus = std::system(simplifyCmd.c_str());
        if (simplifyStatus != 0) {
            cerr << "Error: simplify_menu_file failed for "
                 << baseFilename
                 << " with status " << simplifyStatus << endl;
            continue;
        }

        ostringstream targetStream;
        targetStream << targets.calories << " "
                     << targets.protein  << " "
                     << targets.carbs    << " "
                     << targets.fats;

#ifndef _WIN32
        string solverCmd =
            PY + " solver.py \"" + simplifiedPath + "\" " + targetStream.str() +
            " > \"" + planPath + "\" 2>/dev/null";
#else
        string solverCmd =
            "python solver.py \"" + simplifiedPath + "\" " + targetStream.str() +
            " > \"" + planPath + "\" 2>NUL";
#endif

        int solverStatus = std::system(solverCmd.c_str());
        if (solverStatus != 0) {
            cerr << "Error: solver.py failed for "
                 << simplifiedPath
                 << " with status " << solverStatus << endl;
            continue;
        }

        ifstream planFile(planPath);
        if (!planFile.is_open()) {
            continue;
        }

        vector<FoodItem> fullMenu = getDailyMenu(mealType, result.dateStr);
        if (fullMenu.empty()) {
            continue;
        }

        string line;
        while (getline(planFile, line)) {
            if (line.empty()) continue;
            size_t tabPos = line.find('\t');
            if (tabPos == string::npos) continue;

            string itemName    = line.substr(0, tabPos);
            string servingsStr = line.substr(tabPos + 1);

            try {
                double servings = stod(servingsStr);

                auto it = find_if(
                    fullMenu.begin(), fullMenu.end(),
                    [&](const FoodItem& fi) { return fi.name == itemName; }
                );
                if (it != fullMenu.end() && servings > 0.0) {
                    MealPlanResult::PlannedItem planned;
                    planned.item     = *it;
                    planned.servings = servings;
                    result.selectedMeals[mealType].push_back(planned);
                }
            } catch (...) {
                continue;
            }
        }
    }

    return result;
}

// Logs a menu entry selected by index for a specific meal and date.
bool MenuManager::logMeal(User& user, const string& mealType,
                          const string& date, int menuNumber,
                          double servings) {
    vector<FoodItem> menu = getDailyMenu(mealType, date);

    if (menuNumber < 1 || menuNumber > static_cast<int>(menu.size())) {
        return false;
    }

    FoodItem item = menu[menuNumber - 1];
    user.loggedMeals[date][mealType][item.name] = servings;

    return true;
}

// Logs a specific food item by name for a given meal and date.
bool MenuManager::logFoodItem(User& user, const string& mealType,
                              const string& date, const string& foodName,
                              double servings) {
    user.loggedMeals[date][mealType][foodName] += servings;
    return true;
}

// Removes a logged food item entry for a date and meal.
bool MenuManager::removeLoggedMeal(User& user, const string& date,
                                   const string& mealType,
                                   const string& foodName) {
    auto dateIt = user.loggedMeals.find(date);
    if (dateIt == user.loggedMeals.end()) return false;

    auto mealIt = dateIt->second.find(mealType);
    if (mealIt == dateIt->second.end()) return false;

    auto foodIt = mealIt->second.find(foodName);
    if (foodIt == mealIt->second.end()) return false;

    mealIt->second.erase(foodIt);
    return true;
}

// Computes total calories and macros for all meals logged on a date.
MenuManager::DailyTotals MenuManager::calculateDailyTotals(
    const User& user, const string& date) {
    DailyTotals totals{0, 0.0, 0.0, 0.0};

    auto dateIt = user.loggedMeals.find(date);
    if (dateIt == user.loggedMeals.end()) {
        return totals;
    }

    for (const auto& mealEntry : dateIt->second) {
        string mealType = mealEntry.first;
        vector<FoodItem> menu = getDailyMenu(mealType, date);

        for (const auto& foodEntry : mealEntry.second) {
            string foodName = foodEntry.first;
            double servings = foodEntry.second;

            for (const auto& item : menu) {
                if (item.name == foodName) {
                    totals.calories += static_cast<int>(item.calories * servings);
                    totals.protein  += item.protein * servings;
                    totals.carbs    += item.carbs * servings;
                    totals.fats     += item.fats * servings;
                    break;
                }
            }
        }
    }

    return totals;
}
