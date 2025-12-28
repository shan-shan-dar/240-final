# Mensa: AI-Powered Macro Optimization Engine

## Overview
**Mensa** is a high-performance meal planning application designed to solve the "Diet Problem" using **convex optimization** and **linear algebra**. Unlike standard calorie trackers, Mensa doesn't just log what you eat—it mathematically constructs the optimal meal for you.

It connects to university dining hall APIs (Nutrislice), fetches daily menus in real-time, and uses a mixed-integer programming solver to find the exact combination of food items that minimizes the deviation from your specific macronutrient goals (Calories, Protein, Carbs, Fat).

## The Mathematics: Linear Algebra & Optimization
At its core, Mensa treats nutrition as a vector space problem in $\mathbb{R}^4$.

### 1. Vector Representation
Each food item $j$ on the menu is modeled as a vector $a_j$:

$$
a_j = \begin{bmatrix} 
\text{calories} \\\\
\text{protein} \\\\
\text{carbs} \\\\
\text{fat} 
\end{bmatrix}
$$

We construct a **Macro Coefficient Matrix** $A \in \mathbb{R}^{4 \times n}$, where $n$ is the number of available menu items and the $j$-th column is the vector $a_j$.

### 2. The Optimization Model
Let $g \in \mathbb{R}^4$ be the user's target goal vector. We solve for the serving vector $x \in \mathbb{R}^n$ that minimizes the squared Euclidean distance between the total nutrition and the goal.

**Objective Function:**

$$
\min_{x} \| Ax - g \|_2^2
$$

**Constraints:**
This is a **Mixed-Integer Convex Programming (MICP)** problem because servings cannot be arbitrary continuous values (you can't take 0.314 scoops of rice).
1.  **Discrete Domain:** $x_i \in \{0, 0.5, 1.0, 1.5, \dots, 3.0\}$.
2.  **Sparsity/Cardinality:** $\sum \mathbb{I}(x_i > 0) \le K$ (Limit total number of distinct items to prevent unrealistic meals).

We utilize **CVXPY** with the **ECOS_BB** (Branch and Bound) solver to navigate the non-convex search space efficiently, finding the global optimum in under 2 seconds.

## System Architecture
Mensa utilizes a hybrid architecture to leverage the performance of C++ and the scientific computing ecosystem of Python.

*   **Core Application (C++)**: 
    *   Handles the main event loop, user authentication, and memory management.
    *   Manages persistent data storage using custom JSON parsing.
    *   Implements the "Adaptive Budgeting" algorithm, which recalculates macro targets for future meals based on logged history.
*   **Optimization Engine (Python)**: 
    *   Acts as a computational microservice.
    *   Ingests simplified menu data and builds the optimization model using `cvxpy` and `numpy`.
*   **Data Pipeline**:
    1.  **Ingest**: Fetches raw JSON from Nutrislice API.
    2.  **Normalize**: Python scripts clean ingredient text and filter out "noise" stations (e.g., Condiment bars) using regex.
    3.  **Solve**: C++ invokes the Python solver via subprocess pipelines.
    4.  **Visualize**: Results are piped back to C++ for rendering in the CLI.

## Features
*   **Automated Menu Ingestion**: Real-time scraping of dining hall menus; no manual entry required.
*   **Mathematical Meal Generation**: Generates the mathematically "perfect" meal for your specific goals.
*   **Adaptive Budgeting**: If you overeat at breakfast, the algorithm automatically tightens the constraints for lunch and dinner.
*   **User Profiles**: Secure login system with persistent tracking of goals and history.
*   **Interactive CLI**: Robust command-line interface with color-coded progress bars and formatted data tables.

## Tech Stack
*   **Languages**: C++ (C++17), Python 3.9+
*   **Math/AI Libraries**: NumPy, CVXPY, ECOS (Embedded Conic Solver)
*   **Data**: JSON, Pandas
*   **API**: REST (Nutrislice)

## Installation & Usage

### Prerequisites
*   C++ Compiler (g++ or clang++)
*   Python 3
*   Make

### Build & Run
1.  **Install Python Dependencies**:
    ```bash
    pip install -r requirements.txt
    ```

2.  **Compile the C++ Application**:
    ```bash
    cd main
    make
    ```

3.  **Run Mensa**:
    ```bash
    ./meal_tracker
    ```

---
*Built by David Nathanson, Darshan Shah, Tien Nguyen Chu.*
