#!/usr/bin/env python3

import requests
from datetime import datetime
import os
import json
import re
import sys

# Default Nutrislice configuration for this project.
DINING_HALL = "heilman-dining-hall"
DISTRICT = "richmond"
MEAL_TYPES = {"breakfast", "lunch", "dinner"}

# Folder where extracted and simplified menus are stored.
DESTINATION_FOLDER = '../data/menus'

# Station filters for building simplified menus used by the solver.
EXCLUDED_STATIONS_CONFIG = {
    "breakfast": {
        "exact": [
            "Omelet Station",
            "Yogurt & Oatmeal Bar",
            "Avocado Bar",
            "Toasted",
            "Nut Zone",
            "Condiment",
        ],
        "contains": [],
    },
    "lunch_dinner": {
        "exact": [
            "Panini",
            "Campus Deli",
            "Salad Bar",
            "Condiment",
            "Toasted",
            "Nut Zone",
            "Avocado Bar",
        ],
        "contains": ["Daily Bite"],
    },
}


def fetch_menu(dining_hall: str, meal_type: str, date: datetime):
    """Fetch the Nutrislice weekly menu JSON for one dining hall, meal, and date."""
    year = date.year
    month = date.strftime('%m')
    day = date.strftime('%d')

    # Nutrislice menu endpoint for a given school, menu type, and week.
    url = (
        f"https://{DISTRICT}.api.nutrislice.com/menu/api/weeks/"
        f"school/{dining_hall}/menu-type/{meal_type}/{year}/{month}/{day}/?format=json"
    )

    resp = requests.get(url)
    if resp.status_code == 200:
        return resp.json(), year, month, day
    else:
        print(f"Error: Nutrislice returned status {resp.status_code} for {url}")
        return None, year, month, day


def clean_ingredients(text):
    """Normalize an ingredient string by removing brackets, punctuation, and extra spaces."""
    if not isinstance(text, str):
        return ""

    # Remove parentheses contents: (…)
    text = re.sub(r'\([^)]*\)', '', text)
    # Remove bracket contents: […]
    text = re.sub(r'\[[^\]]*\]', '', text)
    # Replace punctuation with spaces.
    text = re.sub(r'[^\w\s]', ' ', text)
    # Collapse multiple spaces to one.
    text = re.sub(r'\s+', ' ', text)

    return text.strip().lower()


def extract_day_menu(day_obj):
    """Extract one day's sectioned menu with nutrition, serving size, icons, and IDs."""
    sections = {}
    current_section = "Uncategorized"

    # Nutrislice gives items inline; section titles are special items.
    for item in day_obj.get("menu_items", []):
        # Detect section titles that mark new stations.
        if item.get("is_section_title"):
            current_section = item.get("text", "Unnamed Section").strip()
            sections.setdefault(current_section, [])
            continue

        food = item.get("food")
        if not food:
            continue

        # Name displayed to users.
        name = food.get("name", "")

        # Extract high-level diet icons (e.g., VG, GFI).
        icons = [
            icon.get("synced_name")
            for icon in food.get("icons", {}).get("food_icons", [])
            if "synced_name" in icon
        ]

        # Rounded nutrition info (we only keep the main macros and calories).
        full_nutrition = food.get("rounded_nutrition_info", {}) or {}

        nutrition = {
            "calories": full_nutrition.get("calories"),
            "g_fat": full_nutrition.get("g_fat"),
            "g_carbs": full_nutrition.get("g_carbs"),
            "g_protein": full_nutrition.get("g_protein"),
        }

        extracted_entry = {
            "name": name,
            "ingredients": clean_ingredients(food.get("ingredients", "")),
            "nutrition": nutrition,
            "serving_size": food.get("serving_size_info", {}),
            "icons": icons,
        }

        sections.setdefault(current_section, []).append(extracted_entry)

    # Attach a global unique ID across all stations for easier referencing if needed.
    global_id = 0
    for section_items in sections.values():
        for item in section_items:
            item["id"] = global_id
            global_id += 1

    return sections


def generate_extracted_file(date_str: str, meal_type: str, output_dir: str = DESTINATION_FOLDER):
    """Fetch and save one day's menu as a structured JSON file."""
    if meal_type not in MEAL_TYPES:
        raise ValueError(f"meal must be one of {MEAL_TYPES}, got {meal_type!r}")

    try:
        date = datetime.strptime(date_str, "%Y-%m-%d")
    except ValueError:
        raise ValueError("date must be in format YYYY-MM-DD")

    # Pull the weekly menu and select the specific day.
    menu_data, year, month, day = fetch_menu(DINING_HALL, meal_type, date)
    if not menu_data:
        raise RuntimeError("Failed to fetch menu data from Nutrislice")

    target_date = date_str
    target_day_obj = None
    for day_obj in menu_data.get("days", []):
        if day_obj.get("date") == target_date:
            target_day_obj = day_obj
            break

    if target_day_obj is None:
        raise RuntimeError(f"No menu found for {target_date} ({meal_type})")

    # Convert Nutrislice day's structure into our flattened station→list format.
    extracted = extract_day_menu(target_day_obj)

    os.makedirs(output_dir, exist_ok=True)

    filename = f"{meal_type}-{date.year}-{date.strftime('%m')}-{date.strftime('%d')}.json"
    out_path = os.path.join(output_dir, filename)

    # Save full extracted menu for that meal/date.
    with open(out_path, "w") as f:
        json.dump(extracted, f, indent=2)

    print(f"Saved extracted menu to {out_path}")


def simplify_menu_file(
    menu_filename: str,
    destination_folder: str = DESTINATION_FOLDER,
) -> str:
    """
    Simplify a saved menu JSON by filtering stations and deduplicating items by name.

    Returns the path to the new simplified menu file.
    """
    # If just a filename is given, treat it as relative to the destination folder.
    if not os.path.isabs(menu_filename) and os.sep not in menu_filename:
        src_path = os.path.join(destination_folder, menu_filename)
    else:
        src_path = menu_filename
        destination_folder = os.path.dirname(src_path)

    src_path = os.path.abspath(src_path)
    destination_folder = os.path.abspath(destination_folder)

    if not os.path.exists(src_path):
        raise FileNotFoundError(src_path)

    with open(src_path, "r") as f:
        data = json.load(f)

    # Determine which filter rules to apply based on the meal embedded in filename.
    base = os.path.basename(src_path)
    if base.startswith("breakfast-"):
        category = "breakfast"
    elif base.startswith("lunch-") or base.startswith("dinner-"):
        category = "lunch_dinner"
    else:
        category = None

    # Filter out unwanted stations for the solver (e.g., salad bar, condiments).
    if isinstance(data, dict) and category is not None:
        rules = EXCLUDED_STATIONS_CONFIG.get(category, {})
        exact = set(rules.get("exact", []))
        contains = list(rules.get("contains", []))

        simplified = {}
        for station, items in data.items():
            if not isinstance(items, list):
                # Non-list entries (if any) are preserved as-is.
                simplified[station] = items
                continue

            st_name = station.strip()

            # Drop stations that match any exact exclusions.
            if st_name in exact:
                continue

            # Drop stations whose names contain configured substrings.
            if any(substr in st_name for substr in contains):
                continue

            simplified[station] = items
    else:
        # If structure is unexpected, fall back to passing data unchanged.
        simplified = data

    # Deduplicate items across all stations based on name (case-insensitive).
    if isinstance(simplified, dict):
        seen_names = set()
        for station, items in simplified.items():
            if not isinstance(items, list):
                continue

            deduped = []
            for item in items:
                name = (item.get("name") or "").strip()
                if not name:
                    # Items without a name are kept, to avoid dropping unknowns.
                    deduped.append(item)
                    continue

                key = name.lower()
                if key in seen_names:
                    # Skip duplicates seen in any station so far.
                    continue

                seen_names.add(key)
                deduped.append(item)

            simplified[station] = deduped

    out_name = f"simplified-{base}"
    out_path = os.path.join(destination_folder, out_name)

    # Write the simplified structure to disk for the solver.
    with open(out_path, "w") as f:
        json.dump(simplified, f, indent=2)

    return out_path


if __name__ == "__main__":
    # CLI entry point: generate one extracted menu JSON for a given date/meal.
    if len(sys.argv) == 3:
        # Form: python menu.py YYYY-MM-DD breakfast
        date_arg = sys.argv[1]
        meal_arg = sys.argv[2].lower()
    elif len(sys.argv) == 2:
        # Form: python menu.py breakfast   (defaults to today's date)
        meal_arg = sys.argv[1].lower()
        today = datetime.now().date()
        date_arg = today.strftime("%Y-%m-%d")
    else:
        print("Usage:")
        print("  python menu.py YYYY-MM-DD [breakfast|lunch|dinner]")
        print("  python menu.py [breakfast|lunch|dinner]   (uses today's date)")
        sys.exit(1)

    try:
        generate_extracted_file(date_arg, meal_arg)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)
