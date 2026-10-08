# Advanced NLM Commands Build System
# This makefile builds the advanced commands and integrates them with NLM

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -I./src -I./src/advanced -I./src/core -I./src/brain -I./src/agent -I./src/memory -I./src/prediction -I./src/cognition -I./src/neuromodulation -I./src/plasticity -I./src/sensory -I./src/motor -I./src/environment -I./src/experiments -I./src/dynamics -I./src/performance -I./src/visualization -I./src/world -Wall -Wextra -O2
LDFLAGS = -pthread -lm -lstdc++

# Directories
BUILD_DIR = build
ADVANCED_DIR = src/advanced
SRC_DIR = src
CORE_DIR = src/core

# Source files
ADVANCED_SOURCES = $(wildcard $(ADVANCED_DIR)/*.cpp)
MAIN_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
CORE_SOURCES = $(wildcard $(CORE_DIR)/*.cpp)

# Object files
ADVANCED_OBJS = $(patsubst $(ADVANCED_DIR)/%.cpp,$(BUILD_DIR)/advanced_%.o,$(notdir $(ADVANCED_SOURCES)))
MAIN_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/main_%.o,$(notdir $(MAIN_SOURCES)))
CORE_OBJS = $(patsubst $(CORE_DIR)/*.cpp,$(BUILD_DIR)/core_%.o,$(notdir $(CORE_SOURCES)))

# Target executable
TARGET = nlm_advanced

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile advanced commands
$(BUILD_DIR)/advanced_%.o: $(ADVANCED_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile main sources
$(BUILD_DIR)/main_%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile core sources
$(BUILD_DIR)/core_%.o: $(CORE_DIR)/*.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link target executable
$(TARGET): $(BUILD_DIR) $(ADVANCED_OBJS) $(MAIN_OBJS) $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(ADVANCED_OBJS) $(MAIN_OBJS) $(CORE_OBJS) $(LDFLAGS)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)
	rm -f *.o
	rm -f *.so
	rm -f *.a

# Install advanced commands
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	rm -f /usr/local/include/advanced/
	mkdir -p /usr/local/include/advanced
	cp $(ADVANCED_DIR)/*.hpp /usr/local/include/advanced/
	cp $(BUILD_DIR)/*.a /usr/local/lib/ 2>/dev/null || true

# Run tests
 tests:
	@echo "Running advanced commands tests..."
	@./$(TARGET) --test

# Help target
help:
	@echo "Available targets:"
	@echo "  all      : Build the advanced NLM commands"
	@echo "  clean    : Remove build files"
	@echo "  install  : Install advanced commands system-wide"
	@echo "  tests    : Run tests for advanced commands"
	@echo "  help     : Show this help message"

# Phony targets
.PHONY: all clean install tests help
