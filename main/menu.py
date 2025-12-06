#!/usr/bin/env python3

import requests
from datetime import datetime
import os
import json
import re
import sys

DINING_HALL = "heilman-dining-hall"
DISTRICT = "richmond"
MEAL_TYPES = {"breakfast", "lunch", "dinner"}

DESTINATION_FOLDER = '../data/menus'

# Stations to drop when simplifying menus for meal-generation logic.
# You can edit these lists as needed.
EXCLUDED_STATIONS_CONFIG = {
    "breakfast": {
        # Station names to remove if they match exactly
        "exact": [
            "Omelet Station",
            "Yogurt & Oatmeal Bar",
            "Avocado Bar",
            "Toasted",
            "Nut Zone",
            "Condiment",
        ],
        # Substrings to remove if they appear anywhere in the station name
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
        # e.g. "Daily Bite" should match "Daily Bite", "Daily Bite - Chef's Choice", etc.
        "contains": ["Daily Bite"],
    },
}


def fetch_menu(dining_hall: str, meal_type: str, date: datetime):
    """
    Call Nutrislice weekly menu API for a given dining hall, meal type, and date.
    Returns the JSON payload (or None) plus the date components.
    """
    year = date.year
    month = date.strftime('%m')
    day = date.strftime('%d')

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
    if not isinstance(text, str):
        return ""
    # Remove anything inside () or []
    text = re.sub(r'\([^)]*\)', '', text)
    text = re.sub(r'\[[^\]]*\]', '', text)
    # Replace all punctuation with space
    text = re.sub(r'[^\w\s]', ' ', text)
    # Normalize all whitespace
    text = re.sub(r'\s+', ' ', text)
    return text.strip().lower()


def extract_day_menu(day_obj):
    """
    Take a single 'day' object from the Nutrislice JSON and produce the
    sectioned+IDed structure, but only keep:
      - name
      - ingredients
      - nutrition: {calories, g_fat, g_carbs, g_protein}
      - serving_size (all fields)
      - icons
      - id
    """
    sections = {}
    current_section = "Uncategorized"

    for item in day_obj.get("menu_items", []):
        # Section headers
        if item.get("is_section_title"):
            current_section = item.get("text", "Unnamed Section").strip()
            sections.setdefault(current_section, [])
            continue

        food = item.get("food")
        if not food:
            continue

        name = food.get("name", "")

        # Icons list
        icons = [
            icon.get("synced_name")
            for icon in food.get("icons", {}).get("food_icons", [])
            if "synced_name" in icon
        ]

        # Original rounded nutrition info from Nutrislice
        full_nutrition = food.get("rounded_nutrition_info", {}) or {}

        # Only keep the four fields you care about
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

    # Assign globally unique IDs across all sections
    global_id = 0
    for section_items in sections.values():
        for item in section_items:
            item["id"] = global_id
            global_id += 1

    return sections


def generate_extracted_file(date_str: str, meal_type: str, output_dir: str = DESTINATION_FOLDER):
    """
    High-level pipeline:
    - parse date
    - fetch weekly menu for that date & meal
    - find the specific day matching date_str
    - extract that day's menu
    - write one JSON: {meal}-{YYYY}-{MM}-{DD}.json
    """
    if meal_type not in MEAL_TYPES:
        raise ValueError(f"meal must be one of {MEAL_TYPES}, got {meal_type!r}")

    try:
        date = datetime.strptime(date_str, "%Y-%m-%d")
    except ValueError:
        raise ValueError("date must be in format YYYY-MM-DD")

    menu_data, year, month, day = fetch_menu(DINING_HALL, meal_type, date)
    if not menu_data:
        raise RuntimeError("Failed to fetch menu data from Nutrislice")

    # Find the specific day in the returned 'days' list
    target_date = date_str  # Nutrislice uses 'YYYY-MM-DD' strings
    target_day_obj = None
    for day_obj in menu_data.get("days", []):
        if day_obj.get("date") == target_date:
            target_day_obj = day_obj
            break

    if target_day_obj is None:
        raise RuntimeError(f"No menu found for {target_date} ({meal_type})")

    extracted = extract_day_menu(target_day_obj)

    # Ensure output dir exists
    os.makedirs(output_dir, exist_ok=True)

    # File name: {meal}-{year}-{month}-{day}.json
    filename = f"{meal_type}-{date.year}-{date.strftime('%m')}-{date.strftime('%d')}.json"
    out_path = os.path.join(output_dir, filename)

    with open(out_path, "w") as f:
        json.dump(extracted, f, indent=2)

    print(f"Saved extracted menu to {out_path}")

def simplify_menu_file(
    menu_filename: str,
    destination_folder: str = DESTINATION_FOLDER,
) -> str:
    """
    Create a simplified version of a Nutrislice-extracted menu JSON.

    - menu_filename: e.g. "lunch-2025-11-20.json" or a full path
    - destination_folder: where to look for it if menu_filename is just a name

    Writes: simplified-{original name}.json in the same directory.
    Returns: absolute path to the simplified file.
    """
    # Resolve source path
    if not os.path.isabs(menu_filename) and os.sep not in menu_filename:
        # Just a bare name: assume it lives under DESTINATION_FOLDER
        src_path = os.path.join(destination_folder, menu_filename)
    else:
        # Already a path: use it directly
        src_path = menu_filename
        destination_folder = os.path.dirname(src_path)

    src_path = os.path.abspath(src_path)
    destination_folder = os.path.abspath(destination_folder)

    if not os.path.exists(src_path):
        raise FileNotFoundError(src_path)

    with open(src_path, "r") as f:
        data = json.load(f)

    # Infer category (breakfast vs lunch/dinner) from filename prefix
    base = os.path.basename(src_path)
    if base.startswith("breakfast-"):
        category = "breakfast"
    elif base.startswith("lunch-") or base.startswith("dinner-"):
        category = "lunch_dinner"
    else:
        category = None  # fall back to leaving data unchanged

    # Filter out unwanted stations
    if isinstance(data, dict) and category is not None:
        rules = EXCLUDED_STATIONS_CONFIG.get(category, {})
        exact = set(rules.get("exact", []))
        contains = list(rules.get("contains", []))

        simplified = {}
        for station, items in data.items():
            # Keep any non-list top-level keys as-is (e.g. global "icons" etc.)
            if not isinstance(items, list):
                simplified[station] = items
                continue

            st_name = station.strip()

            # Exact matches
            if st_name in exact:
                continue

            # Substring matches ("Daily Bite" etc.)
            if any(substr in st_name for substr in contains):
                continue

            simplified[station] = items
    else:
        # If we can't identify the category or the structure is unexpected,
        # leave the JSON untouched.
        simplified = data

    out_name = f"simplified-{base}"
    out_path = os.path.join(destination_folder, out_name)

    with open(out_path, "w") as f:
        json.dump(simplified, f, indent=2)

    return out_path



if __name__ == "__main__":
    # Options:
    #   python menu.py YYYY-MM-DD [breakfast|lunch|dinner]
    #   python menu.py [breakfast|lunch|dinner]   (uses today's date)
    if len(sys.argv) == 3:
        # Explicit date + meal
        date_arg = sys.argv[1]
        meal_arg = sys.argv[2].lower()
    elif len(sys.argv) == 2:
        # Only meal given; use today's date
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
