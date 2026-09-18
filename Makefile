# NLM Development Environment Script
# This script contains common development and testing commands

# Development shortcuts
.PHONY: help build test run run-main docs python clean rebuild all benchmark visualize

help:
	@echo "NLM Development Commands:"
	@echo "  make build      - Build NLM from source"
	@echo "  make test       - Run NLM tests"
	@echo "  make run        - Run NLM Phase 6 demo"
	@echo "  make run-main   - Run NLM main executable"
	@echo "  make docs       - Show documentation files"
	@echo "  make python     - Test Python bindings"
	@echo "  make clean      - Clean build directory"
	@echo "  make rebuild    - Complete rebuild"
	@echo "  make all        - Full build and test suite"
	@echo "  make benchmark  - Run performance benchmarks"
	@echo "  make visualize  - Generate visualization data"

build:
	@if [ ! -d "build" ]; then \
		echo "Creating build directory..."; \
		mkdir -p build; \
	fi
	@echo "Configuring CMake..."
	@cd build && cmake .. -DCMAKE_BUILD_TYPE=Release
	@echo "Building NLM..."
	@cd build && make -j$$(nproc)
	@echo "Build completed successfully!"

build-python:
	@if [ -f "pyproject.toml" ]; then \
		echo "Installing Python bindings..."; \
		pip install -e .; \
	else \
		echo "Warning: pyproject.toml not found. Skipping Python bindings installation."; \
	fi

python:
	@if [ -f "pyproject.toml" ]; then \
		python3 -c "import pynlm; print('Python bindings loaded successfully')" 2>/dev/null && echo "Python bindings are working" || echo "Error: Python bindings are not working"; \
	else \
		echo "Error: pyproject.toml not found. Cannot test Python bindings."; \
	fi

test:
	@if [ -d "build" ]; then \
		cd build && ctest --output-on-failure; \
	else \
		echo "Error: Build directory not found. Run 'make build' first."; \
	fi

run:
	@if [ -d "build" ]; then \
		cd build && ./nlm_phase6_demo; \
	else \
		echo "Error: Build directory not found. Run 'make build' first."; \
	fi

run-main:
	@if [ -d "build" ]; then \
		cd build && ./nlm; \
	else \
		echo "Error: Build directory not found. Run 'make build' first."; \
	fi

docs:
	@if [ -f "docs/PHASE6_FINAL_AUDIT.md" ]; then \
		echo "Phase 6 Final Audit documentation:"; \
		echo "  docs/PHASE6_FINAL_AUDIT.md"; \
	else \
		echo "Documentation not found."; \
	fi

clean:
	@if [ -d "build" ]; then \
		echo "Cleaning build directory..."; \
		rm -rf build/*; \
	fi

rebuild: clean build build-python

all: rebuild test

benchmark:
	@if [ -d "build" ] && [ -f "build/nlm_benchmark" ]; then \
		cd build && ./nlm_benchmark --mode comprehensive; \
	else \
		echo "Warning: Benchmark executable not found. Run 'make build' first."; \
	fi

visualize:
	@if [ -d "build" ] && [ -f "build/nlm_visualize" ]; then \
		cd build && ./nlm_visualize --input ./brain.bin --output ./frames/ --format animated; \
	else \
		echo "Warning: Visualization executable not found. Run 'make build' first."; \
	fi

# Phony targets
.PHONY: help build test run run-main docs python clean rebuild all benchmark visualize