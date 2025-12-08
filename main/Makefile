# Compiler and basic build settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = meal_tracker

# Python virtualenv configuration for menu/solver scripts
PYTHON   = python3
VENV_DIR = ../.venv
VENV_PIP = $(VENV_DIR)/bin/pip

# C++ source and object files
SRCS = main.cpp UI.cpp Auth.cpp MenuManager.cpp UIUtils.cpp AuthUI.cpp MenuUI.cpp LoggerUI.cpp ProfileUI.cpp
OBJS = $(SRCS:.cpp=.o)

# C++ header files
HEADERS = User.h UI.h Auth.h MenuManager.h UIUtils.h AuthUI.h MenuUI.h LoggerUI.h ProfileUI.h

# Default target: ensure Python env exists, then build the binary
all: solver-env $(TARGET)

# Create or update the local Python virtualenv and install dependencies
solver-env:
	@echo ">> Ensuring Python virtualenv and dependencies"
	@if [ ! -d "$(VENV_DIR)" ]; then \
		echo "Creating Python virtualenv in $(VENV_DIR)"; \
		$(PYTHON) -m venv $(VENV_DIR) || { echo "Error: failed to create venv."; exit 1; }; \
	fi
	@echo "Installing Python dependencies (cvxpy, ecos, numpy, pandas, requests) into $(VENV_DIR) (this takes a bit, but only happens once)"
	@"$(VENV_PIP)" install cvxpy ecos numpy pandas requests >/dev/null 2>&1 || { \
		echo "Error: failed to install Python dependencies (cvxpy, numpy, pandas, requests)."; \
		echo "       Try running: $(VENV_PIP) install cvxpy numpy pandas requests"; \
		exit 1; \
	}
	@echo "✓ Python environment ready."

# Link all object files into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
	@echo ""
	@echo "✓ Build successful!"
	@echo "Run with: ./$(TARGET)"

# Compile each C++ source file into an object file
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create data directory and initial JSON files if they do not exist
setup:
	@mkdir -p ../data
	@if [ ! -f ../data/users.json ]; then \
		echo "[]" > ../data/users.json; \
		echo "✓ Created users.json"; \
	fi
	@if [ ! -f ../data/menu.json ]; then \
		echo "[]" > ../data/menu.json; \
		echo "✓ Created menu.json"; \
	fi
	@echo "✓ Data directory setup complete"

# Remove compiled object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "✓ Cleaned build files"

# Remove build files, data files, and the Python virtualenv
cleanall: clean
	rm -rf ../data
	rm -rf $(VENV_DIR)
	@echo "✓ Cleaned all files including data and virtualenv"

# Run the program after ensuring the Python env and binary exist
run: solver-env $(TARGET)
	./$(TARGET)

# Install data files and build the binary
install: setup all

# Print available Makefile targets and their purpose
help:
	@echo "Macro Meal Tracker - Makefile Commands"
	@echo "======================================"
	@echo "make          - Build the project (and set up Python venv+deps)"
	@echo "make run      - Build and run the program"
	@echo "make setup    - Create data directory and files"
	@echo "make install  - Setup and build"
	@echo "make clean    - Remove build files"
	@echo "make cleanall - Remove build files, data, and virtualenv"
	@echo "make help     - Show this help message"

.PHONY: all clean cleanall run setup install help solver-env
