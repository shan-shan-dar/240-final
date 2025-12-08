import numpy as np
import cvxpy as cp
import pandas as pd
import json
import sys


def solve_meal_plan(menu, target, verbose: bool = True):
    """
    Solve a simple meal-planning optimization problem for the given menu and macro targets.

    menu   – list of items with 'nutrition' and 'serving_size' fields
    target – dict with keys: calories, g_protein, g_carbs, g_fat
    """
    # Desired macro vector (goal).
    g = np.array([
        target["calories"],
        target["g_protein"],
        target["g_carbs"],
        target["g_fat"]
    ])

    # Candidate serving sizes per item (in "menu servings").
    serving_sizes = np.arange(0.5, 3.5, 0.5)  # 0.5, 1.0, ..., 3.0
    n_sizes = len(serving_sizes)

    food_items = []      # Items that pass validation.
    macro_rows = {       # Rows for the macro matrix A.
        "calories": [],
        "g_protein": [],
        "g_carbs": [],
        "g_fat": [],
    }
    item_id_map = []     # Optional mapping back to original IDs.

    # Filter to items with numeric macro entries and build macro rows.
    for item in menu:
        try:
            n = item["nutrition"]
            row = [n["calories"], n["g_protein"], n["g_carbs"], n["g_fat"]]
            # Skip items with missing or non-numeric macros.
            if all(isinstance(x, (int, float)) for x in row):
                food_items.append(item)
                item_id_map.append(item["id"])
                for key, val in zip(macro_rows.keys(), row):
                    macro_rows[key].append(val)
        except Exception:
            # If anything goes wrong, quietly skip that item.
            continue

    n_items = len(food_items)

    # Macro matrix: 4 × n_items (rows: calories, protein, carbs, fat).
    A = np.array([
        macro_rows["calories"],
        macro_rows["g_protein"],
        macro_rows["g_carbs"],
        macro_rows["g_fat"]
    ])

    # Binary decision variables b[i][k] = 1 if item i is taken at serving_sizes[k].
    b = [
        [cp.Variable(boolean=True) for _ in range(n_sizes)]
        for _ in range(n_items)
    ]

    # x_i = total "servings" for item i (weighted sum over size choices).
    x_vars = [
        cp.sum([b[i][k] * serving_sizes[k] for k in range(n_sizes)])
        for i in range(n_items)
    ]

    # Total macros for chosen servings: A * x.
    total_macros = A @ cp.hstack(x_vars)

    # Objective: minimize squared error between actual macros and target macros.
    objective = cp.Minimize(cp.sum_squares(total_macros - g))

    constraints = []

    # At most one serving-size option per item (or none).
    for i in range(n_items):
        constraints.append(cp.sum(b[i]) <= 1)

    # Optional global bound: limit the total number of chosen servings.
    max_items = 15
    constraints.append(
        cp.sum([b[i][k] for i in range(n_items) for k in range(n_sizes)]) <= max_items
    )

    # Mixed-integer convex problem: use ECOS_BB which wraps ECOS + branch-and-bound.
    problem = cp.Problem(objective, constraints)
    problem.solve(solver=cp.ECOS_BB, verbose=False)

    result = []
    x_values = np.array([x.value for x in x_vars])

    # Convert non-trivial serving amounts into the output structure.
    for i, val in enumerate(x_values):
        if val and val > 0.01:
            entry = dict(food_items[i])
            entry["servings"] = round(val, 2)
            result.append(entry)

    # Compute actual macro totals for the solution.
    actual_macros = A @ x_values
    macro_labels = ["Calories", "Protein (g)", "Carbs (g)", "Fat (g)"]

    if verbose:
        # Show chosen items, servings, and human-readable serving sizes.
        print("\n--- Solved Meal ---")
        df = pd.DataFrame([
            {
                "name": i["name"],
                "servings": i["servings"],
                "serving size": (
                    i["serving_size"]["serving_size_amount"]
                    + " "
                    + i["serving_size"]["serving_size_unit"]
                ),
            }
            for i in result
        ])
        print(df)

        # Print macro totals vs goal with errors.
        print("\n--- Macro Results ---")
        for label, actual, goal_val in zip(macro_labels, actual_macros, g):
            diff = actual - goal_val
            print(f"{label:<15}: {actual:.2f} (error: {diff:+.2f})")

    # Structured dictionary result: goal, actual totals, and chosen meal.
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
    # CLI entry: read a simplified menu, run the solver, and print name/servings pairs.
    if len(sys.argv) != 6:
        print("Usage: python solver.py <simplified_menu_path> <calories> <g_protein> <g_carbs> <g_fat>")
        sys.exit(1)

    menu_path = sys.argv[1]

    # Parse numeric macro targets from the command line.
    try:
        calories = float(sys.argv[2])
        g_protein = float(sys.argv[3])
        g_carbs = float(sys.argv[4])
        g_fat = float(sys.argv[5])
    except ValueError:
        print("Error: target macros must be numeric.")
        sys.exit(1)

    # Load simplified menu structure (dict of stations or flat list).
    with open(menu_path, "r") as f:
        data = json.load(f)

    if isinstance(data, dict):
        # Flatten all station lists into one list of items.
        menu_items = []
        for station, items in data.items():
            if isinstance(items, list):
                menu_items.extend(items)
    elif isinstance(data, list):
        # Already a plain list of items.
        menu_items = data
    else:
        print("Error: unsupported menu JSON structure")
        sys.exit(1)

    # Build target macro dictionary in the format solve_meal_plan expects.
    target = {
        "calories": calories,
        "g_protein": g_protein,
        "g_carbs": g_carbs,
        "g_fat": g_fat,
    }

    # Solve the optimization problem without printing intermediate tables.
    result = solve_meal_plan(menu_items, target, verbose=False)

    # C++ side expects tab-separated lines: "<name>\t<servings>".
    for entry in result.get("meal", []):
        name = entry.get("name", "")
        servings = entry.get("servings", 0.0)
        if name and servings:
            print(f"{name}\t{servings}")
