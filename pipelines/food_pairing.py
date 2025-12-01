import csv
import itertools
import json
import torch
from transformers import AutoTokenizer, AutoModelForCausalLM
import os

# Model and Tokenizer Setup
model_id = "meta-llama/Llama-3.1-8B-Instruct"
tokenizer = AutoTokenizer.from_pretrained(model_id)
tokenizer.pad_token = tokenizer.eos_token
model = AutoModelForCausalLM.from_pretrained(
    model_id,
    dtype=torch.bfloat16, # for more efficiency
    device_map="auto", # automatically use GPU if available
)

# Get Pairing Score
def get_food_pairing_score(food_x, food_y, model, tokenizer):
    """
    Queries the Llama 3.1 model to get a food pairing score.
    """
    # Llama 3.1 Instruct has a specific prompt format. 
    # It's critical to follow it for best results.
    messages = [
        {
            "role": "system",
            "content": """You are an expert culinary judge with a deep understanding of gastronomy, nutrition, and food science. Your task is to evaluate the compatibility of two food items. Provide a score from 1 to 10 based on the following criteria and return ONLY a valid JSON object.

Scoring Criteria:
- 10 (Perfect Harmony): A classic and celebrated pairing (e.g., Bacon and Eggs).
- 8-9 (Excellent Pairing): A very common and recommended combination.
- 6-7 (Good Pairing): A compatible pairing that works well.
- 5 (Neutral): These foods don't clash, but don't enhance each other.
- 3-4 (Poor Pairing): An unusual combination with clashing flavors.
- 1-2 (Very Poor/Unhealthy Pairing): A culinarily dissonant or very unhealthy combination.
"""
        },
        {
            "role": "user", 
            "content": f"Evaluate the pairing of Food X: {food_x} and Food Y: {food_y}. Provide the output in a JSON object with 'score' and 'reason' keys."
        },
    ]

    # This creates the full prompt string with the special tokens Llama needs.
    input_ids = tokenizer.apply_chat_template(
        messages,
        add_generation_prompt=True,
        return_tensors="pt"
    ).to(model.device)

    # We need to specify the token ID for the end of the turn to stop generation.
    terminators = [
        tokenizer.eos_token_id,
        tokenizer.convert_tokens_to_ids("<|eot_id|>")
    ]

    # Generate the model's response
    outputs = model.generate(
        input_ids,
        max_new_tokens=100,  # Limit the length of the response
        eos_token_id=terminators,
        do_sample=True,
        temperature=1,
        top_p=1,
    )
    
    # Decode the response and clean it up
    response_text = tokenizer.decode(outputs[0][input_ids.shape[-1]:], skip_special_tokens=True).strip()
    
    try:
        # Extract the JSON part
        json_response = json.loads(response_text)
        return {
            "score": json_response.get("score", 0),
            "reason": json_response.get("reason", "No reason provided.")
        }
    except (json.JSONDecodeError, AttributeError):
        # Handle cases of non-perfect JSON
        return {
            "score": 0,
            "reason": f"Failed to parse model output: {response_text}"
        }

# MAIN
# Read the distinct food names from CSV
distinct_foods_file = 'sample.csv'
food_names = []
if not os.path.exists(distinct_foods_file):
    print(f"[ERROR]: The input file was not found at {distinct_foods_file}")
    exit()

with open(distinct_foods_file, 'r', encoding='utf-8') as f:
    reader = csv.reader(f)
    next(reader)  # Skip header
    for row in reader:
        if row: # Make sure row is not empty
            food_names.append(row[0])

# Generate all unique pairs
food_pairs = list(itertools.combinations(food_names, 2))
total_pairs = len(food_pairs)

print(f"Found {total_pairs} unique pairs to evaluate.")

# Get scores for each pair, write to a new CSV
output_file = 'food_pair_scores.csv'

with open(output_file, 'w', newline='', encoding='utf-8') as f:
    writer = csv.writer(f)
    writer.writerow(['Food_X', 'Food_Y', 'Score', 'Reason'])

    for i, (food_x, food_y) in enumerate(food_pairs):
        print(f"[PROCESSING]: pair {i+1}/{total_pairs}: {food_x} & {food_y}")
        score_data = get_food_pairing_score(food_x, food_y, model, tokenizer)
        
        if score_data:
            writer.writerow([food_x, food_y, score_data['score'], score_data['reason']])
        else:
            writer.writerow([food_x, food_y, 0, "Error during processing."])

print(f"[FINISHED]: Results saved to {output_file}")