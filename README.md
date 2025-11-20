# 240-final

### `menu.py`

Fetches a single meal’s menu from Nutrislice (DHall only) and writes a cleaned JSON file for that date+meal.

#### Usage

From repo root:

```
# Explicit date + meal
python menu.py YYYY-MM-DD [breakfast|lunch|dinner]

# Use today's date automatically
python menu.py [breakfast|lunch|dinner]
```

Output file (in menus/):

```
menus/{meal}-{YYYY}-{MM}-{DD}.json
# e.g. menus/breakfast-2025-11-19.json
```

#### Resulting JSON Example

```
{
  "Provisions 804": [
    {
      "name": "Bacon",
      "ingredients": "bacon",
      "nutrition": {
        "calories": 390.0,
        "g_fat": 30.0,
        "g_carbs": 0.0,
        "g_protein": 29.0
      },
      "serving_size": {
        "serving_size_amount": "3",
        "serving_size_unit": "oz"
      },
      "icons": [
        "GFI"
      ],
      "id": 0
    },
    {
        "name": "Another Item",
        ...
    }
  ],
  "Another Section": [
    ...
  ]
}
```

(`id` is unique per file, incremented across all sections)
