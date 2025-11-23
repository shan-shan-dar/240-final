import os
import json
import csv

menus_directory = '../data/menus'
distinct_food_names = set()
for filename in os.listdir(menus_directory):
    if filename.endswith(".json"):
        filepath = os.path.join(menus_directory, filename)
        with open(filepath, 'r') as f:
            menu_data = json.load(f)
            for restaurant, food_items in menu_data.items():
                for item in food_items:
                    distinct_food_names.add(item['name'])

output_csv_file = 'distinct_foods.csv'

with open(output_csv_file, 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(['Food Name'])  # header row
    for food_name in sorted(list(distinct_food_names)):  # Sort for consistency
        writer.writerow([food_name])
