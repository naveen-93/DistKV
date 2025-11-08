# Compiler and flags
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2 -I./include -stdlib=libc++
LDFLAGS = -pthread -stdlib=libc++

# Directories
SRC_DIR = src
APP_DIR = app
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include
TEST_DIR = tests

# Library name
LIB = $(BIN_DIR)/libdistkv.a

# Source files for library (core components)
LIB_SOURCES = $(SRC_DIR)/storage.cpp \
              $(SRC_DIR)/kvstore.cpp \
              $(SRC_DIR)/client.cpp \
              $(SRC_DIR)/server.cpp

# Object files for library
LIB_OBJECTS = $(BUILD_DIR)/storage.o \
              $(BUILD_DIR)/kvstore.o \
              $(BUILD_DIR)/client.o \
              $(BUILD_DIR)/server.o

# Application executables
CLIENT_APP = $(BIN_DIR)/client
SERVER_APP = $(BIN_DIR)/server

# Test executables
TEST_KVSTORE = $(BIN_DIR)/test_kvstore
TEST_STORAGE = $(BIN_DIR)/test_storage

# Default target
all: directories $(LIB) $(CLIENT_APP) $(SERVER_APP)

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Build library
$(LIB): $(LIB_OBJECTS)
	ar rcs $@ $^
	@echo "Library $(LIB) created successfully"

# Compile library source files to object files
$(BUILD_DIR)/storage.o: $(SRC_DIR)/storage.cpp $(INCLUDE_DIR)/storage.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/kvstore.o: $(SRC_DIR)/kvstore.cpp $(INCLUDE_DIR)/kvstore.hpp $(INCLUDE_DIR)/storage.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/client.o: $(SRC_DIR)/client.cpp $(INCLUDE_DIR)/client.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/server.o: $(SRC_DIR)/server.cpp $(INCLUDE_DIR)/server.hpp $(INCLUDE_DIR)/kvstore.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build client application
$(CLIENT_APP): $(APP_DIR)/client.cpp $(LIB)
	$(CXX) $(CXXFLAGS) $< -o $@ -L$(BIN_DIR) -ldistkv $(LDFLAGS)
	@echo "Client application built: $(CLIENT_APP)"

# Build server application
$(SERVER_APP): $(APP_DIR)/server.cpp $(LIB)
	$(CXX) $(CXXFLAGS) $< -o $@ -L$(BIN_DIR) -ldistkv $(LDFLAGS)
	@echo "Server application built: $(SERVER_APP)"

# Build tests
tests: directories $(LIB) $(TEST_KVSTORE) $(TEST_STORAGE)

$(TEST_KVSTORE): $(TEST_DIR)/test_kvstore.cpp $(LIB)
	$(CXX) $(CXXFLAGS) $< -o $@ -L$(BIN_DIR) -ldistkv $(LDFLAGS)
	@echo "Test built: $(TEST_KVSTORE)"

$(TEST_STORAGE): $(TEST_DIR)/test_storage.cpp $(LIB)
	$(CXX) $(CXXFLAGS) $< -o $@ -L$(BIN_DIR) -ldistkv $(LDFLAGS)
	@echo "Test built: $(TEST_STORAGE)"

# Run tests
run-tests: tests
	@echo "Running storage tests..."
	@$(TEST_STORAGE)
	@echo "Running kvstore tests..."
	@$(TEST_KVSTORE)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)/*.o $(BIN_DIR)/* storage/*.log
	@echo "Clean complete"

# Clean everything including directories
distclean: clean
	rm -rf $(BUILD_DIR) $(BIN_DIR) storage
	@echo "Distribution clean complete"

# Rebuild everything
rebuild: clean all

# Install (optional - for system-wide installation)
install: all
	@echo "Installing DistKV..."
	install -d /usr/local/bin
	install -m 755 $(CLIENT_APP) /usr/local/bin/distkv-client
	install -m 755 $(SERVER_APP) /usr/local/bin/distkv-server
	install -d /usr/local/lib
	install -m 644 $(LIB) /usr/local/lib/
	install -d /usr/local/include/distkv
	install -m 644 $(INCLUDE_DIR)/*.hpp /usr/local/include/distkv/
	@echo "Installation complete"

# Uninstall
uninstall:
	rm -f /usr/local/bin/distkv-client
	rm -f /usr/local/bin/distkv-server
	rm -f /usr/local/lib/libdistkv.a
	rm -rf /usr/local/include/distkv
	@echo "Uninstallation complete"

# Help target
help:
	@echo "DistKV Makefile targets:"
	@echo "  all          - Build library and applications (default)"
	@echo "  directories  - Create build and bin directories"
	@echo "  tests        - Build all tests"
	@echo "  run-tests    - Build and run all tests"
	@echo "  clean        - Remove build artifacts"
	@echo "  distclean    - Remove all generated files and directories"
	@echo "  rebuild      - Clean and rebuild everything"
	@echo "  install      - Install to system (requires sudo)"
	@echo "  uninstall    - Remove from system (requires sudo)"
	@echo "  help         - Show this help message"

# Phony targets
.PHONY: all directories tests run-tests clean distclean rebuild install uninstall help

