import numpy as np
import cvxpy as cp
import pandas as pd
import json
import sys

def solve_meal_plan(menu, target, verbose: bool = True):

    # --- Define Target Vector ---
    g = np.array([
        target["calories"],
        target["g_protein"],
        target["g_carbs"],
        target["g_fat"]
    ])

    # --- Define Serving Sizes ---
    serving_sizes = np.arange(0.5, 3.5, 0.5)
    n_sizes = len(serving_sizes)

    # --- Construct Items & Macro Matrix ---
    food_items = []
    macro_rows = { "calories": [], "g_protein": [], "g_carbs": [], "g_fat": [] }
    item_id_map = []

    for item in menu:
        try:
            n = item["nutrition"]
            row = [n["calories"], n["g_protein"], n["g_carbs"], n["g_fat"]]
            if all(isinstance(x, (int, float)) for x in row):
                food_items.append(item)
                item_id_map.append(item["id"])
                for key, val in zip(macro_rows.keys(), row):
                    macro_rows[key].append(val)
        except:
            continue

    n_items = len(food_items)
    A = np.array([
        macro_rows["calories"],
        macro_rows["g_protein"],
        macro_rows["g_carbs"],
        macro_rows["g_fat"]
    ])

    # --- Decision Variables ---
    b = [
        [cp.Variable(boolean=True) for _ in range(n_sizes)]
        for _ in range(n_items)
    ]

    # final solution vector
    x_vars = [cp.sum([b[i][k] * serving_sizes[k] for k in range(n_sizes)]) for i in range(n_items)]
    total_macros = A @ cp.hstack(x_vars)

    # --- Solver Objective ---
    objective = cp.Minimize(cp.sum_squares(total_macros - g))

    # --- Constraints ---
    constraints = []

    # 1. Each item can only be selected once or 0 times
    for i in range(n_items):
        constraints.append(cp.sum(b[i]) <= 1)

    # 2. There can be at most max_item items in the meal
    max_items = 15
    constraints.append(cp.sum([b[i][k] for i in range(n_items) for k in range(n_sizes)]) <= max_items)

    # --- Solve ---
    problem = cp.Problem(objective, constraints)
    problem.solve(solver=cp.ECOS_BB, verbose=False)

    # --- Build Result JSON ---
    result = []
    x_values = np.array([x.value for x in x_vars])
    for i, val in enumerate(x_values):
        if val and val > 0.01:
            entry = dict(food_items[i])  # deep copy from input
            entry["servings"] = round(val, 2)
            result.append(entry)

    # --- Compute actual macros and optionally print summary ---
    actual_macros = A @ x_values
    macro_labels = ["Calories", "Protein (g)", "Carbs (g)", "Fat (g)"]

    if verbose:
        print("\n--- Solved Meal ---")
        df = pd.DataFrame([
            {
                "name": i["name"],
                "servings": i["servings"],
                "serving size": i["serving_size"]["serving_size_amount"]
                               + " "
                               + i["serving_size"]["serving_size_unit"],
            }
            for i in result
        ])
        print(df)

        print("\n--- Macro Results ---")
        for label, actual, goal_val in zip(macro_labels, actual_macros, g):
            diff = actual - goal_val
            print(f"{label:<15}: {actual:.2f} (error: {diff:+.2f})")

    return {
        "goal": target,
        "totals": {
            "calories": round(actual_macros[0], 2),
            "g_protein": round(actual_macros[1], 2),
            "g_carbs": round(actual_macros[2], 2),
            "g_fat": round(actual_macros[3], 2),
        },
        "meal": result,
    }

if __name__ == "__main__":
    """
    CLI usage:

        python solver.py <simplified_menu_path> <calories> <g_protein> <g_carbs> <g_fat>

    Reads the simplified menu JSON, flattens all station lists into one
    list of items, runs solve_meal_plan, and prints:

        <name>\t<servings>

    one per line on stdout, for C++ to parse.
    """
    if len(sys.argv) != 6:
        print("Usage: python solver.py <simplified_menu_path> <calories> <g_protein> <g_carbs> <g_fat>")
        sys.exit(1)

    menu_path = sys.argv[1]
    try:
        calories = float(sys.argv[2])
        g_protein = float(sys.argv[3])
        g_carbs = float(sys.argv[4])
        g_fat = float(sys.argv[5])
    except ValueError:
        print("Error: target macros must be numeric.")
        sys.exit(1)

    with open(menu_path, "r") as f:
        data = json.load(f)

    # Flatten sectioned menus into a single list
    if isinstance(data, dict):
        menu_items = []
        for station, items in data.items():
            if isinstance(items, list):
                menu_items.extend(items)
    elif isinstance(data, list):
        menu_items = data
    else:
        print("Error: unsupported menu JSON structure")
        sys.exit(1)

    target = {
        "calories": calories,
        "g_protein": g_protein,
        "g_carbs": g_carbs,
        "g_fat": g_fat,
    }

    result = solve_meal_plan(menu_items, target, verbose=False)

    for entry in result.get("meal", []):
        name = entry.get("name", "")
        servings = entry.get("servings", 0.0)
        if name and servings:
            print(f"{name}\t{servings}")
