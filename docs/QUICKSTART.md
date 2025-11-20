# 🚀 Quick Start Guide

## Build & Run (First Time)

```bash
cd main
make install
./meal_tracker
```

## Try the Demo User

**Username:** `demo_user`  
**Password:** `demo123`

Already configured with 2000 calorie goal and 30/50/20 macro split!

## Create Your Own Account

1. Choose option **2** (Register)
2. Pick a username and password
3. Set your calorie goal (e.g., **2000**)
4. Enter macro ratio (e.g., **4 5 1** for 40% protein, 50% carbs, 10% fats)

## Main Menu Options

| Option | Feature | Description |
|--------|---------|-------------|
| 1 | 🍽️ View Daily Menu | See today's breakfast with per-serving nutrition |
| 2 | 📋 View Logged Foods | Review what you've eaten, organized by meal |
| 3 | ➕ Log a Meal | Add food items with custom serving amounts |
| 4 | ✨ Generate Meal Plan | Placeholder for future AI meal suggestions |
| 5 | ⚙️ Edit Profile | Update goals and preferences |
| 6 | 🚪 Logout | Exit safely (data auto-saves) |

## Understanding Macro Ratios

When you enter "4 5 1":
- **4 parts** protein (40%)
- **5 parts** carbs (50%)  
- **1 part** fats (10%)

The system automatically converts to percentages!

## File Locations

- **Executable:** `main/meal_tracker`
- **User Data:** `data/users.json`
- **Menu Data:** `data/menu.json`

## Makefile Commands

```bash
make          # Build only
make setup    # Create data directory
make install  # Setup + build
make run      # Build + run
make clean    # Remove build files
make help     # Show all commands
```

## Features at a Glance

✅ Secure authentication  
✅ Unique user IDs (USR######)  
✅ Customizable nutrition goals  
✅ Real-time progress tracking with visual bars  
✅ Meal logging with fractional servings  
✅ Daily menu with per-serving nutrition  
✅ Date-based meal organization  
✅ Profile editing  
✅ Persistent JSON storage across sessions  
✅ Beautiful terminal UI with color indicators  
✅ Context-aware motivational tips  

## Tips

- All data saves automatically
- You can edit goals anytime
- Progress bars update in real-time as you log meals
- Logged meals persist across program restarts
- Type "done" when finished logging to exit continuous mode
- Serving amounts can be fractional (0.5, 1.5, 2.0, etc.)
- Each day's meals are organized separately by date
- Use the demo user to explore features first!

---

**Need help?** Check `README.md` for full documentation!
