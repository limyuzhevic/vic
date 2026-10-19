# NLM Build System Makefile
# Simplified build system for Phase 6: FINAL INTEGRATION
# This Makefile provides easy commands for building, testing, and running NLM

# Project variables
PROJECT_NAME = nlm
VERSION = 0.1.0
BUILD_TYPE ?= Release
CMAKE_GENERATOR ?= "Unix Makefiles"
BUILD_DIR ?= build
SRC_DIR = src

# Default target
all: help

# Help target - show available commands
help:
	@echo "===================================="
	@echo "NLM Phase 6 Build System"
	@echo "Version: $(VERSION) - Final Integration"
	@echo "===================================="
	@echo ""
	@echo "Build Commands:"
	@echo "  make build              Build the project"
	@echo "  make build-debug        Build with debug symbols"
	@echo "  make rebuild            Clean and rebuild"
	@echo "  make install            Install to system (requires sudo)"
	@echo ""
	@echo "Testing Commands:"
	@echo "  make test               Run unit tests"
	@echo "  make integration-test   Run integration tests"
	@echo "  make benchmark          Run performance benchmarks"
	@echo ""
	@echo "Execution Commands:"
	@echo "  make run                Run Phase 6 integration demo"
	@echo "  make run-plasticity     Run plasticity experiment"
	@echo "  make run-stdp           Run STDP experiment"
	@echo "  make run-benchmark      Run performance benchmark"
	@echo "  make interactive        Run with interactive debugging"
	@echo ""
	@echo "Development Commands:"
	@echo "  make format             Format code with clang-format"
	@echo "  make analyze            Run static analysis"
	@echo "  make clean              Clean build artifacts"
	@echo "  make clean-all          Clean everything including dependencies"
	@echo ""
	@echo "Configuration Commands:"
	@echo "  make config-default     Create default config file"
	@echo "  make config-custom      Create custom config file template"
	@echo ""
	@echo "Utilities:"
	@echo "  make help               Show this help message"
	@echo "  make version            Show project version"

# Build targets
build:
	@echo "Building NLM Phase 6 with $(BUILD_TYPE) configuration..."
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir -p "$(BUILD_DIR)"; fi
	cd "$(BUILD_DIR)" && cmake -G $(CMAKE_GENERATOR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ..
	cd "$(BUILD_DIR)" && cmake --build . -- -j$$(nproc)
	@echo "Build complete! Executable: $(BUILD_DIR)/$(PROJECT_NAME)"

build-debug:
	@echo "Building NLM Phase 6 with Debug configuration..."
	@if [ ! -d "$(BUILD_DIR)" ]; then mkdir -p "$(BUILD_DIR)"; fi
	cd "$(BUILD_DIR)" && cmake -G $(CMAKE_GENERATOR) -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-O0 -g -DDEBUG" ..
	cd "$(BUILD_DIR)" && cmake --build . -- -j$$(nproc)
	@echo "Debug build complete! Executable: $(BUILD_DIR)/$(PROJECT_NAME)"

rebuild:
	@echo "Cleaning and rebuilding..."
	@if [ -d "$(BUILD_DIR)" ]; then rm -rf "$(BUILD_DIR)"; fi
	make build

# Testing targets
test:
	@echo "Running unit tests..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		cd "$(BUILD_DIR)" && ctest --output-on-failure; \
	else \
		echo "Build directory not found. Run 'make build' first." && false; \
	fi

integration-test:
	@echo "Running integration tests..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		cd "$(BUILD_DIR)" && ./nlm --benchmark --experiment demo; \
	else \
		echo "Build directory not found. Run 'make build' first." && false; \
	fi

benchmark:
	@echo "Running performance benchmarks..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		cd "$(BUILD_DIR)" && ./nlm --benchmark --verbose; \
	else \
		echo "Build directory not found. Run 'make build' first." && false; \
	fi

# Execution targets
run:
	@echo "Starting NLM Phase 6 integration demo..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		cd "$(BUILD_DIR)" && ./$(PROJECT_NAME) --experiment demo; \
	else \
		echo "Build directory not found. Run 'make build' first." && false; \
	fi

run-plasticity:
	@echo "Running NLM plasticity experiment..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		cd "$(BUILD_DIR)" && ./$(PROJECT_NAME) --experiment plasticity; \
	else \
		echo "Build directory not found. Run 'make build' first." && false; \
	fi

run-stdp:
	@echo "Running NLM STDP experiment..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		cd "$(BUILD_DIR)" && ./$(PROJECT_NAME) --experiment stdp; \
	else \
		echo "Build directory not found. Run 'make build' first." && false; \
	fi

run-benchmark:
	@echo "Running NLM performance benchmark..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		cd "$(BUILD_DIR)" && ./$(PROJECT_NAME) --benchmark --verbose; \
	else \
		echo "Build directory not found. Run 'make build' first." && false; \
	fi

interactive:
	@echo "Starting NLM with interactive debugging..."
	@if [ -d "$(BUILD_DIR)" ]; then \
		cd "$(BUILD_DIR)" && gdb ./$(PROJECT_NAME); \
	else \
		echo "Build directory not found. Run 'make build' first." && false; \
	fi

# Development targets
format:
	@echo "Formatting NLM source code..."
	@if command -v clang-format > /dev/null; then \
		find src -name "*.cpp" -o -name "*.hpp" -o -name "*.h" | xargs clang-format -i; \
		echo "Code formatting complete!"; \
	else \
		echo "clang-format not found. Install with: sudo apt install clang-format"; \
	fi

analyze:
	@echo "Running static analysis..."
	@if command -v cppcheck > /dev/null; then \
		cppcheck --enable=all --std=c++20 src 2>/dev/null | head -50; \
		@echo "Analysis complete (limited output). Check full report for details."; \
	else \
		echo "cppcheck not found. Install with: sudo apt install cppcheck"; \
	fi

clean:
	@echo "Cleaning build artifacts..."
	@if [ -d "$(BUILD_DIR)" ]; then rm -rf "$(BUILD_DIR)"; fi
	online:
	@echo "Cleaning everything..."
	@if [ -d "$(BUILD_DIR)" ]; then rm -rf "$(BUILD_DIR)"; fi
	@if [ -f "nlm.log" ]; then rm -f nlm.log; fi
	@if [ -f "nlm_phase6.log" ]; then rm -f nlm_phase6.log; fi
	@if [ -d "checkpoints" ]; then rm -rf checkpoints; fi
	@if [ -d "docs" ]; then rm -rf docs; fi
	@echo "Clean complete!"

# Configuration targets
config-default:
	@echo "Creating default configuration file..."
	@if [ ! -f "configs/default.cfg" ]; then \
		echo "# NLM Configuration File" > configs/default.cfg && \
		echo "# Phase 6: FINAL INTEGRATION Configuration" >> configs/default.cfg && \
		echo "random_seed = 42" >> configs/default.cfg && \
		echo "simulation_timestep = 0.001" >> configs/default.cfg && \
		echo "neuron_count = 500" >> configs/default.cfg && \
		echo "region_count = 3" >> configs/default.cfg && \
		echo "connection_probability = 0.15" >> configs/default.cfg && \
		echo "log_level = INFO" >> configs/default.cfg && \
		echo "max_simulation_steps = 100000" >> configs/default.cfg && \
		echo "integration_experiment_enabled = true" >> configs/default.cfg; \
	fi
	@echo "Default configuration created at: configs/default.cfg"

config-custom:
	@echo "Creating custom configuration template..."
	@if [ ! -f "configs/custom_template.cfg" ]; then \
		echo "# Custom NLM Configuration Template" > configs/custom_template.cfg && \
		echo "# Copy this file to default.cfg and modify as needed" >> configs/custom_template.cfg && \
		echo "# " >> configs/custom_template.cfg && \
		echo "# random_seed = 42" >> configs/custom_template.cfg && \
		echo "# simulation_timestep = 0.001" >> configs/custom_template.cfg && \
		echo "# neuron_count = 500" >> configs/custom_template.cfg && \
		echo "# region_count = 3" >> configs/custom_template.cfg && \
		echo "# connection_probability = 0.15" >> configs/custom_template.cfg && \
		echo "# plasticity_learning_rate = 0.02" >> configs/custom_template.cfg && \
		echo "# stdp_ltp_weight = 0.02" >> configs/custom_template.cfg && \
		echo "# stdp_ltd_weight = 0.015" >> configs/custom_template.cfg && \
		echo "# development_synaptogenesis_rate = 0.0005" >> configs/custom_template.cfg && \
		echo "# development_pruning_rate = 0.00005" >> configs/custom_template.cfg && \
		echo "# dopamine_baseline = 0.1" >> configs/custom_template.cfg && \
		echo "# curiosity_rate = 0.3" >> configs/custom_template.cfg && \
		echo "# novelty_threshold = 0.1" >> configs/custom_template.cfg && \
		echo "# working_memory_capacity = 100" >> configs/custom_template.cfg && \
		echo "# episodic_memory_max_episodes = 1000" >> configs/custom_template.cfg && \
		echo "# neural_planner_depth = 5" >> configs/custom_template.cfg && \
		echo "# prediction_accuracy_threshold = 0.8" >> configs/custom_template.cfg && \
		echo "# environment_name = ComplexWorld" >> configs/custom_template.cfg && \
		echo "# log_level = INFO" >> configs/custom_template.cfg && \
		echo "# log_to_file = true" >> configs/custom_template.cfg && \
		echo "# max_simulation_steps = 100000" >> configs/custom_template.cfg && \
		echo "# integration_experiment_enabled = true" >> configs/custom_template.cfg; \
	fi
	@echo "Custom configuration template created at: configs/custom_template.cfg"

# Utility targets
version:
	@echo "NLM Phase 6 - Neural Learning Machine"
	@echo "Version: $(VERSION)"
	@echo "Build Type: $(BUILD_TYPE)"
	@echo "Build Directory: $(BUILD_DIR)"

# Phony targets
.PHONY: help build build-debug rebuild test integration-test benchmark run run-plasticity run-stdp run-benchmark interactive format analyze clean clean-all config-default config-custom version