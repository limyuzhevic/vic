# NLM Makefile
# Simplified build system for the improved NLM codebase
# Maintains backward compatibility while adding new features

# Variables
CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -Wpedantic -I./include
LDFLAGS = -pthread -lm
TARGET = nlm
BUILD_DIR = build
SRC_DIR = src

# Source files
MAIN_SRCS = $(SRC_DIR)/main.cpp
EXPERIMENTS_SRCS = $(wildcard $(SRC_DIR)/experiments/*.cpp)
BRAIN_SRCS = $(wildcard $(SRC_DIR)/brain/*.cpp)
CORE_SRCS = $(wildcard $(SRC_DIR)/core/*.cpp)

# Python bindings (if any)
PYTHON_BINDINGS = $(wildcard $(SRC_DIR)/python/*.cpp)

# Default target
all: $(BUILD_DIR)/$(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile main.cpp and all source files
$(BUILD_DIR)/$(TARGET): $(BUILD_DIR) $(MAIN_SRCS) $(EXPERIMENTS_SRCS) $(BRAIN_SRCS) $(CORE_SRCS)
	@echo "Building NLM Neural Learning Machine..."
	$(CXX) $(CXXFLAGS) $(MAIN_SRCS) $(EXPERIMENTS_SRCS) $(BRAIN_SRCS) $(CORE_SRCS) \
		$(PYTHON_BINDINGS) $(LDFLAGS) -o $(BUILD_DIR)/$(TARGET)
	@echo "Build complete: $(BUILD_DIR)/$(TARGET)"

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Install dependencies (if any)
deps:
	@echo "Checking for dependencies..."
	@if ! command -v g++ &> /dev/null; then echo "Error: g++ not found"; exit 1; fi
	@echo "Dependencies check passed"

# Run tests
test:
	@echo "Running NLM improvement tests..."
	cd /workspace/app && python3 test_improvements.py

# Build with debug flags
debug:
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -g -O0"

# Build with maximum optimizations
optimize:
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -O3 -march=native -flto"

# Help target
help:
	@echo "Available targets:"
	@echo "  all     - Build the NLM application (default)"
	@echo "  clean   - Remove build artifacts"
	@echo "  deps    - Check for build dependencies"
	@echo "  test    - Run improvement tests"
	@echo "  debug   - Build with debug symbols"
	@echo "  optimize - Build with maximum optimizations"

.PHONY: all clean deps test debug optimize help