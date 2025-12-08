# Macro Meal Tracker

**Names:** Martin Sang, Tien Nguyen, David Nathanson, Darshan Shah  
**Date:** December 2025  
**Program:** Macro Meal Tracker  
**Program Description:**  
This program is a comprehensive, console-based nutrition tracking application designed for users who want to monitor their daily caloric and macronutrient intake. It features a robust user authentication system, fetches live dining hall menus via a Python helper script, and allows users to log meals, track their progress towards personal goals with visual progress bars, and even generate personalized meal plans. The application is built with a clean, multi-layered C++ architecture, separating UI, business logic, and data models for maintainability and scalability.

---

## Installation and Dependencies

Prerequisites:

1.  **C++ Compiler:** A modern C++ compiler (`g++`).

2.  **Python 3:** The application relies on a Python script (`menu.py`) to fetch dining hall menu data. Please ensure you have Python 3 installed on your system.

---

## Compile and Run

#### 1. First-Time Setup
Before the first compilation, run the `setup` command to create the necessary data directories and files.
```bash
make setup
```

#### 2. Compile and Run
The easiest way to build and run the program is with the `run` command:
```bash
make run
```
This will automatically compile the project and start the `meal_tracker` executable.

#### Step-by-Step Compilation
Alternatively, you can compile and run the program in separate steps:
```bash
# Compile the source code into an executable
make

# Run the compiled program
./meal_tracker
```

#### Cleaning Up
To remove the compiled object files and the executable, use the `clean` command:
```bash
make clean
```

---

## Example Usage

### First-time User Creating an Account

```text
  ╔═══════════════════════════════════════╗
  ║   🍽️  MACRO MEAL TRACKER  🍽️    ║
  ╚═══════════════════════════════════════╝

  Track nutrition. Meet goals. Stay healthy.

  --------------------------------------------------

     [1] Login
     [2] Register
     [3] Exit

  --------------------------------------------------

  >> Choice: 2

=== CREATE YOUR ACCOUNT ===

     STEP 1: Account Information
  --------------------------------------------------

  >> Choose a username: new_user
  >> Choose a password: password123

     STEP 2: Nutrition Goals
  --------------------------------------------------

     Set your daily calorie target (typical: 1800-2500)
  >> Daily calorie goal: 2200

     STEP 3: Macro Ratio
  --------------------------------------------------

     Define your macro split using simple ratios.
     Example: '4 5 1' = 40% protein, 50% carbs, 10% fats
     Common splits:
       - Balanced: 3 5 2 (30% protein, 50% carbs, 20% fats)
       - High Protein: 4 4 2 (40% protein, 40% carbs, 20% fats)
       - Low Carb: 3 2 5 (30% protein, 20% carbs, 50% fats)

  >> Enter ratio (Protein Carbs Fats): 4 4 2

     Creating your account...

  --------------------------------------------------

     SUCCESS! Account created successfully!
     You can now log in with your credentials.

  --------------------------------------------------
```

### User Interface after Signing in

```
=== MAIN MENU ===

     Welcome back, new_user!

     Today's Progress:
  --------------------------------------------------
     Calories:      0 / 2200  [░░░░░░░░░░░░░░░░░░░░] 0%
     Protein:       0 / 220g  [░░░░░░░░░░░░░░░░░░░░] 0%
     Carbs:         0 / 220g  [░░░░░░░░░░░░░░░░░░░░] 0%
     Fats:          0 / 49g   [░░░░░░░░░░░░░░░░░░░░] 0%

  💡 Tip: Start logging meals to track your progress!

  --------------------------------------------------

     [1] Log a meal
     [2] View/Edit logged foods
     [3] View menus
     [4] Generate meal plan
     [5] Edit profile
     [6] Logout

  --------------------------------------------------

  >> Choice: 1

=== LOG A MEAL ===

  Date: Friday, December 05, 2025

  --------------------------------------------------

     Which meal would you like to log?

     [1] Breakfast
     [2] Lunch
     [3] Dinner
     [4] Go back

  --------------------------------------------------

  >> Enter your choice (1-4): 2

<Menu for Lunch is displayed>

  >> Enter item number (or type 'done' to finish): 3
  >> Enter number of servings: 1.5

  --------------------------------------------------

     ✓ Logged! Grilled Chicken Breast x1.5 (450 cal)

  --------------------------------------------------

  Press ENTER to log another item...

<User types 'done' and is returned to the Main Menu>

=== MAIN MENU ===

     Welcome back, new_user!

     Today's Progress:
  --------------------------------------------------
     Calories:    450 / 2200  [████░░░░░░░░░░░░░░░░] 20%
     Protein:      80 / 220g  [███████░░░░░░░░░░░░░] 36%
     Carbs:         5 / 220g  [░░░░░░░░░░░░░░░░░░░░] 2%
     Fats:         12 / 49g   [████░░░░░░░░░░░░░░░░] 24%

  💪 Keep going! You have 80% left to reach your goals.

  --------------------------------------------------
```

---

## Screenshots

*Welcome Screen*
![Welcome Screen](img_README/welcome.png)

*Main Menu Dashboard*
![Main Menu Dashboard](img_README/mainmenu.png)

*Dining Hall Menu View*
![Dining Hall Menu View](img_README/menu.png)

*Meal Generator Interface*
![Meal Generator Interface](img_README/mealgenerator.png)

---

## Program Flow and Features

The application is structured around a central user session, with functionality broken down into several key modules.

*   **Welcome Screen:** The initial entry point where a user can choose to `Login`, `Register` a new account, or `Exit` the application.

*   **Main Menu:** After a successful login, this screen acts as the central dashboard. It provides:
    *   A real-time overview of the user's daily nutrition progress with visual bars for calories and macros.
    *   Motivational tips based on progress.
    *   Access to all other features of the application.

*   **Menu and Meal Features:**
    *   **Log a Meal:** Select a meal (Breakfast, Lunch, Dinner) to view the corresponding menu for the day and log items with specific serving sizes.
    *   **View/Edit Logged Foods:** See a list of all foods logged for the day, review daily totals, and remove items if necessary.
    *   **View Menus:** Browse the full dining hall menu for the current day or any other specified date.
    *   **Generate Meal Plan:** An intelligent feature that calculates the user's remaining nutritional needs for the day and suggests an optimal meal plan from the available menu items.

*   **Profile Management:**
    *   **Edit Profile:** Allows the user to update their daily calorie goal, change their macro-nutrient ratios, and update their account password.


### ***Hope you stay healthy and eat well!***
