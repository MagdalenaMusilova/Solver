# Compiler
CXX = CC

# Flags
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -Iinclude

# Directories
SRC_DIR = src
BUILD_DIR = build

# Files
MAIN = main.cpp
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Objects (include main.cpp separately)
OBJS = $(BUILD_DIR)/main.o $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Target
TARGET = $(BUILD_DIR)/app

# Default
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# Compile main.cpp (root)
$(BUILD_DIR)/main.o: $(MAIN)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile src files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR)

# Run
run: all
	./$(TARGET)

.PHONY: all clean run