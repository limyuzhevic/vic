# NLM CLI Makefile

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Wpedantic -I./src -I./src/core -I./src/brain -I./src/cli
LDFLAGS = -pthread

# Directories
SRC_DIR = src/cli
BUILD_DIR = build
EXECUTABLE = $(BUILD_DIR)/nlm_cli

# Source files
CLI_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Default target
all: $(EXECUTABLE)

# Link executable
$(EXECUTABLE): $(BUILD_DIR) $(CLI_SOURCES)
	$(CXX) $(CXXFLAGS) -o $@ $(CLI_SOURCES) -lstdc++

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Install
install: $(EXECUTABLE)
	install -m 755 $(EXECUTABLE) /usr/local/bin/nlm

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build the CLI executable"
	@echo "  clean    - Clean build artifacts"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  help     - Show this help"

.PHONY: all clean install help