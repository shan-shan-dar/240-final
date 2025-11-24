import csv
import itertools
import os

INPUT_FOOD_LIST = 'distinct_foods.csv'
OUTPUT_PAIRS_FILE = 'pairs_to_process.csv'

def create_pairs_file():
    """
    Reads a list of foods and writes all unique combinations to a new CSV file.
    """
    if not os.path.exists(INPUT_FOOD_LIST):
        print(f"[ERROR]: Input file not found at '{INPUT_FOOD_LIST}'")
        return

    food_names = []
    with open(INPUT_FOOD_LIST, 'r', encoding='utf-8') as f:
        reader = csv.reader(f)
        next(reader)  # Skip header
        for row in reader:
            if row:  # Ensure the row is not empty
                food_names.append(row[0])

    food_pairs = list(itertools.combinations(food_names, 2))
    total_pairs = len(food_pairs)
    
    with open(OUTPUT_PAIRS_FILE, 'w', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        writer.writerow(['Food_X', 'Food_Y'])
        writer.writerows(food_pairs)

    print(f"\n[SUCCESS]: Created {total_pairs} pairs to process.")

if __name__ == "__main__":
    create_pairs_file()