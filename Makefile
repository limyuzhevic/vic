# NLM Makefile - Comprehensive Build and Test System
# This Makefile provides targets for building, testing, and running NLM examples

# Compiler and tools
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
CMAKE = cmake
MAKE = make

# Directories
SRC_DIR = src
BUILD_DIR = build
EXAMPLE_DIR = example
DOCS_DIR = docs

# Targets
all: build test

# Main targets
build:
	@echo "Building NLM project..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -DCMAKE_BUILD_TYPE=Release
	@cd $(BUILD_DIR) && $(MAKE) -j$$(nproc)
	@echo "Build completed successfully."

build-debug:
	@echo "Building NLM project in debug mode..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -DCMAKE_BUILD_TYPE=Debug
	@cd $(BUILD_DIR) && $(MAKE) -j$$(nproc)
	@echo "Debug build completed successfully."

test: build
	@echo "Running NLM tests..."
	@if [ -f $(BUILD_DIR)/nlm ]; then \
	    echo "Running Phase 6 integration demo..."; \
	    $(BUILD_DIR)/nlm_phase6_demo; \
	else \
	    echo "Warning: NLM executable not found, skipping demo"; \
	fi
	@if [ -f $(BUILD_DIR)/test_nlm ]; then \
	    echo "Running unit tests..."; \
	    $(BUILD_DIR)/test_nlm --verbose; \
	else \
	    echo "Warning: Unit tests not found, skipping tests"; \
	fi
	@echo "Testing completed."

install:
	@echo "Installing NLM..."
	@if [ ! -d $(BUILD_DIR) ]; then \
	    echo "Building NLM first..."; \
	    $(MAKE) build; \
	fi
	@cd $(BUILD_DIR) && $(MAKE) install
	@echo "Installation completed."

# Documentation targets
docs:
	@echo "Building documentation..."
	@if [ -f Doxyfile ]; then \
	    doxygen Doxyfile; \
	else \
	    echo "Doxyfile not found, skipping documentation build"; \
	fi
	@echo "Documentation built."

# Example targets
example-build:
	@echo "Building NLM example..."
	@if [ ! -d $(BUILD_DIR) ]; then \
	    echo "Building NLM first..."; \
	    $(MAKE) build; \
	fi
	@mkdir -p $(EXAMPLE_DIR)/build
	@echo "Example build directory created."

example-run: example-build
	@echo "Running NLM example..."
	@if [ -f $(EXAMPLE_DIR)/build/example ]; then \
	    $(EXAMPLE_DIR)/build/example; \
	else \
	    echo "Error: Example executable not found. Building example..."; \
	    $(MAKE) example-build && $(EXAMPLE_DIR)/build/example; \
	fi

# Configuration targets
configure:
	@echo "Configuring NLM with CMake..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) ..
	@echo "Configuration completed."

clean:
	@echo "Cleaning NLM build artifacts..."
	@if [ -d $(BUILD_DIR) ]; then \
	    rm -rf $(BUILD_DIR); \
	    echo "Build directory cleaned."; \
	fi
	@if [ -d $(EXAMPLE_DIR)/build ]; then \
	    rm -rf $(EXAMPLE_DIR)/build; \
	    echo "Example build directory cleaned."; \
	fi
	@echo "Clean completed."

# Development targets
develop:
	@echo "Running in development mode..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) .. -DCMAKE_BUILD_TYPE=Debug
	@cd $(BUILD_DIR) && $(MAKE) -j$$(nproc)
	@echo "Development build completed."

dev-test: develop
	@echo "Running development tests..."
	@if [ -f $(BUILD_DIR)/test_nlm ]; then \
	    $(BUILD_DIR)/test_nlm --verbose; \
	else \
	    echo "Warning: Tests not found"; \
	fi

# Performance targets
benchmark:
	@echo "Running NLM benchmarks..."
	@if [ -f $(BUILD_DIR)/nlm_benchmark ]; then \
	    $(BUILD_DIR)/nlm_benchmark --help; \
	else \
	    echo "Warning: Benchmarks not found"; \
	fi

# Quick test targets
quick-test: build
	@echo "Running quick tests..."
	@if [ -f $(BUILD_DIR)/nlm ]; then \
	    echo "Running quick demo..."; \
	    $(BUILD_DIR)/nlm --help; \
	else \
	    echo "Error: NLM executable not found"; \
	    exit 1; \
	fi

# Help target
help:
	@echo "NLM (熙然) Build System"
	@echo "========================"
	@echo ""
	@echo "Available targets:"
	@echo "  all              Build and test the project (default)"
	@echo "  build            Build the project"
	@echo "  build-debug      Build in debug mode"
	@echo "  test             Build and run tests"
	@echo "  install          Build and install the project"
	@echo "  clean            Clean build artifacts"
	@echo "  configure        Configure with CMake"
	@echo "  docs             Build documentation"
	@echo "  example-build    Build example program"
	@echo "  example-run      Build and run example program"
	@echo "  develop          Build in development mode"
	@echo "  dev-test         Build in development mode and run tests"
	@echo "  benchmark        Run benchmarks"
	@echo "  quick-test       Quick test of the build"
	@echo ""
	@echo "Examples:"
	@echo "  make build              # Build the project"
	@echo "  make test              # Build and test"
	@echo "  make example-run       # Build and run example"
	@echo "  make clean             # Clean build artifacts"
	@echo ""
	@echo "For more information, see the NLM documentation."

# Phony targets
.PHONY: all build build-debug test install clean configure docs example-build example-run develop dev-test benchmark quick-test help
