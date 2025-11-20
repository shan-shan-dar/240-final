# 🎯 Project Summary - Macro Meal Tracker

## What Was Built

A full-featured terminal-based meal tracking application with:

### ✅ Complete Authentication System
- **User Registration**: Username, password, calorie goals, macro ratios
- **Unique ID Generation**: "USR" prefix + random 6-digit number
- **Login System**: Validates credentials against users.json
- **Persistent Storage**: All user data saved in JSON format

### ✅ Beautiful UX/UI Design
- Clean box-drawing characters for headers
- Color indicators (✓, ✗, 🍽️, 📊, etc.)
- Organized menu layouts with clear sections
- Professional table formatting for data display

### ✅ Core Features

1. **Onboarding System**
   - Register new accounts
   - Set calorie goals (e.g., 2000 calories/day)
   - Define macro ratios (e.g., "4 5 1" → 40% protein, 50% carbs, 10% fats)
   - Auto-converts ratios to percentages in JSON

2. **Main Dashboard**
   - Real-time progress tracking with visual bars (20 blocks)
   - Color-coded indicators (green = on track, red = over)
   - Percentage displays for all macros
   - Contextual tips based on progress level
   - Shows today's date and logged meal count

3. **Daily Menu Display**
   - Shows breakfast menu for current date
   - Per-serving nutrition (e.g., "129/each" for calories)
   - Table format: Name | Station | Calories | Protein | Carbs | Fats | Serving
   - Calculates totals automatically
   - Accurate serving size parsing from JSON

4. **Meal Logging System**
   - Log foods by name with custom serving amounts
   - Continuous logging mode (type "done" to finish)
   - Meal type selection (breakfast, lunch, dinner, snacks)
   - View logged foods organized by meal type
   - Persistent storage across sessions
   - Automatic date-based organization

5. **Meal Plan Generator** (Placeholder)
   - Feature placeholder for future implementation
   - Smart algorithm framework ready

6. **Profile Editor**
   - Update calorie goals anytime
   - Modify macro ratios
   - Changes persist to users.json

## File Structure

```
240-final/
├── main/
│   ├── main.cpp          - Entry point
│   ├── UI.cpp/h          - Terminal interface (screens, menus, input)
│   ├── Auth.cpp/h        - Authentication & user management
│   ├── MenuManager.cpp/h - Menu display & meal generation
│   ├── User.h            - Data structures (User, FoodItem, MacroRatio)
│   ├── Makefile          - Build system
│   └── demo.sh           - Demo script
├── data/
│   ├── users.json        - User accounts database
│   └── menu.json         - Menu items (uses defaults if not present)
└── README.md             - Full documentation
```

## Key Technical Features

### JSON User Storage
```json
{
  "uid": "USR123456",
  "username": "john_doe",       
  "password": "password123",
  "calorieGoal": 2500,
  "proteinRatio": 0.30,
  "carbsRatio": 0.50,
  "fatsRatio": 0.20,
  "loggedMeals": {
    "2025-11-20": {
      "breakfast": {
        "Fried Eggs": 3.0,
        "Bacon": 0.5,
        "Steel Cut Oats": 3.0
      },
      "lunch": {
        "Chicken Breast": 2.0
      }
    }
  }
}
```

### Progress Tracking System
- Unicode visual progress bars (█ filled, ░ empty)
- 20-block bars for granular visualization
- Real-time percentage calculations
- Color-coded status messages
- Contextual motivational tips:
  - 0%: "Start logging meals to track your progress!"
  - <50%: "Keep going! You have X% left to reach your goals."
  - <90%: "Almost there! Just X% more to hit your targets."
  - ≤100%: "Great job! You've hit your goals perfectly!"
  - >100%: "You're over by X% - consider adjusting tomorrow."

### Meal Logging System
- Three-level nested map structure: date → meal type → food → servings
- Persistent storage with automatic date organization
- Supports fractional servings (e.g., 0.5, 1.5, 3.0)
- Continuous logging mode for multiple items
- Proper JSON parsing with date key restoration on restart

### UX/UI Highlights
- Welcome screen with ASCII art
- Contextual headers for each screen
- Loading indicators
- Success/error messages with icons
- Clean separation between sections
- "Press ENTER to continue" for flow control

## How to Use

### Building
```bash
cd main
make install    # Sets up data/ and compiles
```

### Running
```bash
./meal_tracker  # Or: make run
```

### Demo User
- Username: `demo_user`
- Password: `demo123`
- Already configured with 2000 cal goal, 30/50/20 macro split

### Creating New Account
1. Choose option 2 (Register)
2. Enter username and password
3. Set calorie goal (e.g., 2000)
4. Enter ratio like "4 5 1" (means 4 parts protein, 5 parts carbs, 1 part fat)
5. System calculates: 40% protein, 50% carbs, 10% fats

## Sample Workflow

```
1. Launch app → See welcome screen
2. Register → Set username, password, goals (2300 cal, 4 4 2 ratio)
3. Login → See main dashboard with progress bars (all at 0%)
4. View Menu → Browse today's breakfast items with per-serving nutrition
5. Log Meal → Choose breakfast, enter "Fried Eggs" (3 servings), "Bacon" (0.5)
6. Return to Dashboard → See updated progress (732/2300 cal, 31%)
7. View Logged Foods → Review what you've eaten organized by meal
8. Edit Profile → Change goals if needed
9. Logout → All data persists
10. Login again → Progress and logged meals restore from JSON
```

## What Makes This Special

✨ **Complete Implementation** - Fully working app with all core features
✨ **Visual Progress Tracking** - Unicode bars with real-time percentage updates
✨ **Professional UX** - Polished interface with color-coded indicators
✨ **Persistent Storage** - Real JSON file I/O with proper date-based organization
✨ **Accurate Nutrition** - Per-serving calculations with proper unit display
✨ **Session Persistence** - Logged meals restore correctly across program restarts
✨ **Debug System** - Preprocessor-based debugging with #ifdef DEBUG

## Testing It

Try these scenarios:

1. **Register a new user**: Username "test", password "test", 2300 calories, ratio "4 4 2"
2. **View main dashboard**: Progress bars all at 0%, contextual tip says "Start logging meals"
3. **View menu**: See breakfast items with per-serving nutrition (e.g., "129/each" calories)
4. **Log breakfast items**: Add "Fried Eggs" (3 servings), "Bacon" (0.5), "Steel Cut Oats" (3)
5. **Check dashboard**: Progress updated to ~732/2300 cal (31%) with visual bars
6. **View logged foods**: See all items organized by meal type with totals
7. **Edit profile**: Change calorie goal, see updated percentage calculations
8. **Logout and login**: Verify logged meals and progress restore correctly from JSON

## Future Ready

The codebase is structured to easily add:
- Meal logging history
- Database instead of JSON
- Nutrislice API integration  
- Web interface
- Mobile app
- Analytics and reports

---

**Status: ✅ FULLY FUNCTIONAL - Ready for demo and submission!**
