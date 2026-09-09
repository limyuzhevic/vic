# Makefile for NLM - Simple build alternative to CMake
# This Makefile provides faster, simpler builds for common operations

# Project configuration
PROJECT_NAME := nlm
PROJECT_ROOT := ..

# Directories
SRC_DIR := src
BUILD_DIR := build
TARGET_DIR := bin

# Compiler settings
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -O2
INCLUDES := -I$(PROJECT_ROOT)/src -I.

# Source files
CORE_SOURCES := $(wildcard $(PROJECT_ROOT)/src/core/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/brain/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/dynamics/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/plasticity/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/development/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/neuromodulation/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/memory/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/cognition/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/sensory/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/prediction/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/motor/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/environment/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/experiments/*.cpp) \
                 $(wildcard $(PROJECT_ROOT)/src/visualization/*.cpp)

AGENT_SOURCES := $(wildcard $(PROJECT_ROOT)/src/agent/*.cpp)
WORLD_SOURCES := $(wildcard $(PROJECT_ROOT)/src/world/*.cpp)
MAIN_SOURCES := $(PROJECT_ROOT)/src/main.cpp

# Library and executable targets
CORE_LIB := $(TARGET_DIR)/libnlm_core.a
AGENT_LIB := $(TARGET_DIR)/libnlm_agent.a
WORLD_LIB := $(TARGET_DIR)/libnlm_world.a
MAIN_EXE := $(TARGET_DIR)/$(PROJECT_NAME)

# Phase 3 and 4 libraries
PHASE3_LIB := $(TARGET_DIR)/libnlm_phase3.a
PHASE4_LIB := $(TARGET_DIR)/libnlm_phase4.a

PHASE3_EXE := $(TARGET_DIR)/nlm_phase3_demo
PHASE4_EXE := $(TARGET_DIR)/nlm_phase4_demo

# Python bindings (if available)
PYTHON_BINDINGS := $(PROJECT_ROOT)/python/bindings.cpp

# Default target
all: $(MAIN_EXE) $(PHASE3_EXE) $(PHASE4_EXE) $(PHASE3_LIB) $(PHASE4_LIB)

# Create directories
$(shell mkdir -p $(TARGET_DIR))

# Main executable - depends on all libraries
$(MAIN_EXE): $(MAIN_SOURCES) $(CORE_LIB) $(AGENT_LIB) $(WORLD_LIB)
	@echo "Building main executable: $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(MAIN_SOURCES) \
		$(CORE_LIB) $(AGENT_LIB) $(WORLD_LIB)

# Core library
$(CORE_LIB): $(filter-out $(PROJECT_ROOT)/src/experiments/Phase6Demo.cpp, $(CORE_SOURCES))
	@echo "Building core library: $@"
	ar rcs $@ $^

# Agent library
$(AGENT_LIB): $(AGENT_SOURCES)
	@echo "Building agent library: $@"
	ar rcs $@ $^

# World library
$(WORLD_LIB): $(WORLD_SOURCES)
	@echo "Building world library: $@"
	ar rcs $@ $^

# Phase 3 library
$(PHASE3_LIB): $(PROJECT_ROOT)/src/experiments/Phase3Experiment.cpp
	@echo "Building phase 3 library: $@"
	ar rcs $@ $^

# Phase 4 library
$(PHASE4_LIB): $(PROJECT_ROOT)/src/experiments/Phase4Experiment.cpp
	@echo "Building phase 4 library: $@"
	ar rcs $@ $^

# Phase 3 demo
$(PHASE3_EXE): $(PROJECT_ROOT)/src/experiments/Phase3Demo.cpp $(PHASE3_LIB) $(CORE_LIB) $(AGENT_LIB) $(WORLD_LIB)
	@echo "Building phase 3 demo: $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(PROJECT_ROOT)/src/experiments/Phase3Demo.cpp \
		$(PHASE3_LIB) $(CORE_LIB) $(AGENT_LIB) $(WORLD_LIB)

# Phase 4 demo
$(PHASE4_EXE): $(PROJECT_ROOT)/src/experiments/Phase4Demo.cpp $(PHASE4_LIB) $(PHASE3_LIB) $(CORE_LIB) $(AGENT_LIB) $(WORLD_LIB)
	@echo "Building phase 4 demo: $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(PROJECT_ROOT)/src/experiments/Phase4Demo.cpp \
		$(PHASE4_LIB) $(PHASE3_LIB) $(CORE_LIB) $(AGENT_LIB) $(WORLD_LIB)

# Phase 6 demo
$(TARGET_DIR)/nlm_phase6_demo: $(PROJECT_ROOT)/src/experiments/Phase6Demo.cpp $(CORE_LIB)
	@echo "Building phase 6 demo: $@"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(PROJECT_ROOT)/src/experiments/Phase6Demo.cpp \
		$(CORE_LIB)

# Quick build target (faster, fewer features)
quick: CXXFLAGS += -O1
quick: $(CORE_LIB) $(MAIN_EXE)

# Development build with debug info
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: all

# Test target
test: all
	@echo "Running tests..."
	@if [ -f "$(BUILD_DIR)/nlm_test" ]; then \
		echo "Found CTest in build directory"; \
		cd $(BUILD_DIR) && ctest --output-on-failure; \
	else \
		echo "CTest not found. Run: cmake .. && make test"; \
	fi

# Clean target
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(TARGET_DIR)/*.a $(TARGET_DIR)/* $(BUILD_DIR)

# Help target
help:
	@echo "Available targets:"
	@echo "  all          Build everything (default)"
	@echo "  quick        Fast build with fewer features"
	@echo "  debug        Build with debug information"
	@echo "  test         Run tests (if available)")
	@echo "  clean        Remove build artifacts"
	@echo "  help         Show this help")
	@echo ""
	@echo "Common usage:"
	@echo "  make         # Build everything"
	@echo "  make quick   # Fast build"
	@echo "  make clean   # Clean build"
	@echo "  ./$(TARGET_DIR)/$(PROJECT_NAME)  # Run main demo"
	@echo "  ./$(TARGET_DIR)/nlm_phase3_demo    # Run Phase 3 demo"
	@echo "  ./$(TARGET_DIR)/nlm_phase4_demo    # Run Phase 4 demo"
	@echo "  ./$(TARGET_DIR)/nlm_phase6_demo    # Run Phase 6 demo"

.PHONY: all quick debug test clean help

