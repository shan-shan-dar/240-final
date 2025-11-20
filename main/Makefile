# Macro Meal Tracker Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = meal_tracker

# Source files
SRCS = main.cpp UI.cpp Auth.cpp MenuManager.cpp
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = User.h UI.h Auth.h MenuManager.h

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
	@echo ""
	@echo "✓ Build successful!"
	@echo "Run with: ./$(TARGET)"

# Compile source files to object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create data directory and sample files
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

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "✓ Cleaned build files"

# Clean everything including data
cleanall: clean
	rm -rf ../data
	@echo "✓ Cleaned all files including data"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Install (just setup for now)
install: setup all

# Help target
help:
	@echo "Macro Meal Tracker - Makefile Commands"
	@echo "======================================"
	@echo "make          - Build the project"
	@echo "make setup    - Create data directory and files"
	@echo "make install  - Setup and build"
	@echo "make run      - Build and run the program"
	@echo "make clean    - Remove build files"
	@echo "make cleanall - Remove build files and data"
	@echo "make help     - Show this help message"

.PHONY: all clean cleanall run setup install help
