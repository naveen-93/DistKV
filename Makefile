CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -pthread
LDFLAGS = -pthread

# Directories
SRC_DIR = src
TEST_DIR = tests
OBJ_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Exclude main.o for library
LIB_OBJECTS = $(filter-out $(OBJ_DIR)/main.o $(OBJ_DIR)/client.o, $(OBJECTS))

# Targets
TARGET = $(BIN_DIR)/distkv
TEST_TARGET = $(BIN_DIR)/test_kvstore

.PHONY: all clean test dirs

all: dirs $(TARGET)

test: dirs $(TEST_TARGET)
	@echo "Running tests..."
	@cd $(BIN_DIR) && ./test_kvstore

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) storage

# Main executable
$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@
	@echo "Built $(TARGET)"

# Test executable
$(TEST_TARGET): $(TEST_OBJECTS) $(LIB_OBJECTS)
	$(CXX) $(LDFLAGS) $^ -o $@
	@echo "Built $(TEST_TARGET)"

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) storage
	@echo "Cleaned build artifacts"

# Dependencies
-include $(OBJECTS:.o=.d)
-include $(TEST_OBJECTS:.o=.d)