# NLM Makefile
# Comprehensive build system for NLM project

# Variables
PROJECT_NAME := nlm
PROJECT_VERSION := 0.1.0
BUILD_DIR := build
VENV_DIR := venv
PYTHON_PACKAGE := pynlm
CMAKE_BUILD_TYPE := Release
PARALLEL_JOBS := $(shell nproc)

# Colors
GREEN := \033[0;32m
BLUE := \033[0;34m
YELLOW := \033[1;33m
RED := \033[0;31m
NC := \033[0m

# Targets
.PHONY: all help setup build test clean coverage docs release docker python-benchmark

# Help target
help:
	@echo "$(BLUE)NLM Build System Help$(NC)"
	@echo ""
	@echo "$(GREEN)Available targets:$(NC)"
	@echo "  $(GREEN)all$(NC)                - Build the project with Python bindings (default)"
	@echo "  $(GREEN)setup$(NC)              - Set up development environment"
	@echo "  $(GREEN)build$(NC)              - Build the C++ library and executables"
	@echo "  $(GREEN)test$(NC)               - Run all tests (C++ and Python)"
	@echo "  $(GREEN)clean$(NC)              - Clean build artifacts"
	@echo "  $(GREEN)coverage$(NC)           - Generate code coverage report"
	@echo "  $(GREEN)docs$(NC)               - Build documentation"
	@echo "  $(GREEN)release$(NC)            - Create release distribution"
	@echo "  $(GREEN)docker$(NC)             - Build Docker image"
	@echo "  $(GREEN)python-benchmark$(NC)   - Run Python benchmarks"
	@echo ""
	@echo "$(GREEN)Quick commands:$(NC)"
	@echo "  $(YELLOW)make setup$(NC)      - Initialize development environment"
	@echo "  $(YELLOW)make build$(NC)      - Build the project"
	@echo "  $(YELLOW)make test$(NC)       - Run tests"
	@echo "  $(YELLOW)make clean$(NC)      - Clean build artifacts"
	@echo ""
	@echo "$(GREEN)Build scripts:$(NC)"
	@echo "  ./build_scripts/build.sh     - Full build script with options"
	@echo "  ./build_scripts/test.sh      - Test script with options"
	@echo ""$(NC)

# Default target
all: build python-bindings

# Setup development environment
setup:
	@echo "$(BLUE)Setting up NLM development environment...$(NC)"
	@if command -v python3 > /dev/null 2>&1; then \
		echo "$(GREEN)Creating Python virtual environment...$(NC)"; \
		python3 -m venv $(VENV_DIR); \
		echo "$(GREEN)Activating virtual environment...$(NC)"; \
		source $(VENV_DIR)/bin/activate; \
		echo "$(GREEN)Upgrading pip and setuptools...$(NC)"; \
		pip install --no-cache-dir --upgrade pip setuptools wheel; \
		echo "$(GREEN)Installing project dependencies...$(NC)"; \
		pip install --no-cache-dir scikit-build-core>=0.5.0 pybind11>=2.11.0; \
		echo "$(GREEN)Installing Python package in development mode...$(NC)"; \
		pip install --no-cache-dir -e .; \
		echo "$(GREEN)Installing development dependencies...$(NC)"; \
		pip install --no-cache-dir -e .[dev]; \
		echo "$(GREEN)Development environment setup complete!$(NC)"; \
	else \
		echo "$(YELLOW)Python3 not found. Please install Python3 to set up environment.$(NC)"; \
	fi

# Build the project
build:
	@echo "$(BLUE)Building NLM project...$(NC)"
	@if [ -d "$(BUILD_DIR)" ]; then \
		echo "$(YELLOW)Build directory already exists. Use 'make clean' first or 'make build-script'.$(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)Configuring CMake build...$(NC)"; \
	mkdir -p $(BUILD_DIR); \
	cd $(BUILD_DIR) && cmake .. \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
		-DCMAKE_CXX_STANDARD=20 \
		-DCMAKE_CXX_STANDARD_REQUIRED=ON \
		-DENABLE_TESTS=ON \
		-DENABLE_VALIDATION=ON \
		-DBUILD_EXAMPLES=ON \
		-DNLM_PYTHON_BINDINGS=ON \
		-DCMAKE_EXPORT_COMPILE_COMMANDS=ON; \
	@echo "$(GREEN)Building with $(PARALLEL_JOBS) parallel jobs...$(NC)"; \
	cd $(BUILD_DIR) && cmake --build . --parallel $(PARALLEL_JOBS); \
	echo "$(GREEN)Build complete!$(NC)"

# Build using the comprehensive build script
build-script:
	@echo "$(BLUE)Building NLM using comprehensive build script...$(NC)"
	@chmod +x build_scripts/build.sh
	@./build_scripts/build.sh

# Run tests
test:
	@echo "$(BLUE)Running NLM tests...$(NC)"
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		echo "$(YELLOW)Build directory not found. Run 'make setup' then 'make build'.$(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)Running C++ tests...$(NC)"; \
	cd $(BUILD_DIR) && ctest --output-on-failure --timeout 300; \
	@echo "$(GREEN)Running Python tests...$(NC)"; \
	source $(VENV_DIR)/bin/activate && pytest tests/ \
		--cov=pynlm \
		--cov-report=xml \
		--cov-report=html \
		--cov-report=term-missing \
		--cov-config=.coveragerc \
		--timeout 300; \
	echo "$(GREEN)All tests completed!$(NC)"

# Run tests using comprehensive test script
test-script:
	@chmod +x build_scripts/test.sh
	@./build_scripts/test.sh

# Clean build artifacts
clean:
	@echo "$(BLUE)Cleaning NLM build artifacts...$(NC)"
	@if command -v ./build_scripts/clean.sh > /dev/null 2>&1; then \
		./build_scripts/clean.sh; \
	else \
		@echo "$(GREEN)Removing build directory...$(NC)"; \
		rm -rf $(BUILD_DIR); \
		@echo "$(GREEN)Removing virtual environment...$(NC)"; \
		rm -rf $(VENV_DIR); \
		@echo "$(GREEN)Removing distribution directory...$(NC)"; \
		rm -rf dist/; \
		@echo "$(GREEN)Removing Python package files...$(NC)"; \
		rm -f *.egg-info/*.egg-info 2>/dev/null || true; \
		@echo "$(GREEN)Cleaning temporary files...$(NC)"; \
		find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true; \
		find . -name "*.pyc" -delete; \
		find . -name "*.pyo" -delete; \
		find . -name ".coverage" -delete; \
		find . -name "coverage.xml" -delete; \
		find . -name "*.log" -delete; \
		find . -name "*.tmp" -delete; \
		find . -name "*.temp" -delete; \
		find . -name "*~" -delete; \
		find . -name "*.bak" -delete; \
		find . -name "*.backup" -delete; \
		@echo "$(GREEN)Cleanup complete!$(NC)"; \
	fi

# Generate code coverage report
coverage:
	@echo "$(BLUE)Generating code coverage report...$(NC)"
	@if [ ! -d "$(BUILD_DIR)" ]; then \
		echo "$(YELLOW)Build directory not found. Run 'make setup' then 'make build'.$(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)Running C++ coverage analysis...$(NC)"; \
	cd $(BUILD_DIR) && \
	apply_coverage_policy 2>/dev/null || true; \
	@echo "$(GREEN)Running Python coverage analysis...$(NC)"; \
	source $(VENV_DIR)/bin/activate && \
	coverage report --fail-under=80 && \
	coverage html && \
	coverage xml; \
	echo "$(GREEN)Coverage report generated!$(NC)"

# Build documentation
docs:
	@echo "$(BLUE)Building NLM documentation...$(NC)"
	@if command -v python3 > /dev/null 2>&1; then \
		source $(VENV_DIR)/bin/activate; \
		pip install --no-cache-dir sphinx sphinx-rtd-theme breathe myst-parser; \
		if [ -d "docs" ]; then \
			echo "$(GREEN)Building Sphinx documentation...$(NC)"; \
			cd docs && \
				sphinx-build -b html . _build/html && \
				sphinx-build -b latex . _build/latex && \
				sphinx-build -b man . _build/man; \
				echo "$(GREEN)Documentation built!$(NC)"; \
			cd ..; \
		fi; \
		if command -v doxygen > /dev/null 2>&1; then \
			if [ -f "Doxyfile" ]; then \
				echo "$(GREEN)Building Doxygen documentation...$(NC)"; \
				doxygen Doxyfile; \
				echo "$(GREEN)Doxygen documentation built!$(NC)"; \
			fi; \
		fi; \
	else \
		echo "$(YELLOW)Python3 not found. Please install Python3 to build documentation.$(NC)"; \
	fi

# Build Python bindings
python-bindings:
	@echo "$(BLUE)Building Python bindings...$(NC)"
	@if command -v python3 > /dev/null 2>&1; then \
		source $(VENV_DIR)/bin/activate; \
		pip install --no-cache-dir build; \
		python -m build; \
		echo "$(GREEN)Python bindings built!$(NC)"; \
	else \
		echo "$(YELLOW)Python3 not found. Please install Python3 to build Python bindings.$(NC)"; \
	fi

# Create release
release:
	@echo "$(BLUE)Creating NLM release...$(NC)"
	@if command -v ./build_scripts/release.sh > /dev/null 2>&1; then \
		./build_scripts/release.sh --confirm; \
	else \
		@echo "$(YELLOW)Release script not found. Please check build_scripts/release.sh$(NC)"; \
	fi

# Build Docker image
docker:
	@echo "$(BLUE)Building Docker image...$(NC)"
	@if command -v docker > /dev/null 2>&1; then \
		docker build -t nlm:latest .; \
		docker tag nlm:latest nlm:$(PROJECT_VERSION); \
		echo "$(GREEN)Docker image built!$(NC)"; \
	else \
		echo "$(YELLOW)Docker not found. Please install Docker to build Docker image.$(NC)"; \
	fi

# Run Python benchmarks
python-benchmark:
	@echo "$(BLUE)Running Python benchmarks...$(NC)"
	@if command -v ./build_scripts/benchmark.sh > /dev/null 2>&1; then \
		./build_scripts/benchmark.sh --results-dir benchmark_results; \
	else \
		@echo "$(YELLOW)Benchmark script not found. Please check build_scripts/benchmark.sh$(NC)"; \
	fi

# Setup environment with comprehensive setup script
setup-full:
	@echo "$(BLUE)Setting up NLM development environment with comprehensive script...$(NC)"
	@chmod +x build_scripts/setup_env.sh
	@./build_scripts/setup_env.sh

# Validate project structure
validate:
	@echo "$(BLUE)Validating NLM project structure...$(NC)"
	@if [ ! -f "CMakeLists.txt" ]; then \
		echo "$(RED)CMakeLists.txt not found!$(NC)"; \
		exit 1; \
	fi
	@if [ ! -f "pyproject.toml" ]; then \
		echo "$(RED)pyproject.toml not found!$(NC)"; \
		exit 1; \
	fi
	@if [ ! -d "src" ]; then \
		echo "$(RED)src directory not found!$(NC)"; \
		exit 1; \
	fi
	@if [ ! -d "docs" ]; then \
		echo "$(YELLOW)docs directory not found. Some documentation may be missing.$(NC)"; \
	fi
	@echo "$(GREEN)Project structure validation complete!$(NC)"

# Check for build requirements
check-requirements:
	@echo "$(BLUE)Checking build requirements...$(NC)"
	@if command -v cmake > /dev/null 2>&1; then \
		CMAKE_VERSION=$$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+\.[0-9]+'); \
		if [ "$$CMAKE_VERSION" = "0.0.0" ]; then \
			CMAKE_VERSION=$$(cmake --version | head -n1 | grep -oE '[0-9]+\.[0-9]+'); \
		fi; \
		REQUIRED_VERSION="3.16"; \
		if (( $$(echo "$$CMAKE_VERSION >= $$REQUIRED_VERSION" | bc -l) )); then \
			echo "$(GREEN)CMake $$CMAKE_VERSION found (required: $$REQUIRED_VERSION)$(NC)"; \
		else \
			echo "$(RED)CMake $$CMAKE_VERSION found (required: $$REQUIRED_VERSION)$(NC)"; \
			exit 1; \
		fi; \
	else \
		echo "$(RED)CMake not found!$(NC)"; \
		exit 1; \
	fi
	@if command -v g++ > /dev/null 2>&1 || command -v clang++ > /dev/null 2>&1; then \
		echo "$(GREEN)C++ compiler found$(NC)"; \
	else \
		echo "$(RED)C++ compiler not found!$(NC)"; \
		exit 1; \
	fi
	@if command -v python3 > /dev/null 2>&1; then \
		PYTHON_VERSION=$$(python3 -c "import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}.{sys.version_info.micro}')"); \
		REQUIRED_VERSION="3.8"; \
		if (( $$(echo "$$PYTHON_VERSION >= $$REQUIRED_VERSION" | bc -l) )); then \
			echo "$(GREEN)Python $$PYTHON_VERSION found (required: $$REQUIRED_VERSION)$(NC)"; \
		else \
			echo "$(RED)Python $$PYTHON_VERSION found (required: $$REQUIRED_VERSION)$(NC)"; \
			exit 1; \
		fi; \
	else \
		echo "$(RED)Python3 not found!$(NC)"; \
		exit 1; \
	fi
	@echo "$(GREEN)All build requirements satisfied!$(NC)"

# Quick start target
quick-start:
	@echo "$(BLUE)NLM Quick Start Guide$(NC)"
	@echo ""
	@echo "$(GREEN)1. Set up development environment:"$(NC)
	@echo "   $(YELLOW)make setup-full$(NC)"
	@echo ""
	@echo "$(GREEN)2. Build the project:"$(NC)
	@echo "   $(YELLOW)make build$(NC)        # Basic build"
	@echo "   $(YELLOW)make build-script$(NC) # Full build with options"
	@echo ""
	@echo "$(GREEN)3. Run tests:"$(NC)
	@echo "   $(YELLOW)make test$(NC)         # Basic tests"
	@echo "   $(YELLOW)make test-script$(NC)  # Comprehensive tests"
	@echo ""
	@echo "$(GREEN)4. Build documentation:"$(NC)
	@echo "   $(YELLOW)make docs$(NC)"
	@echo ""
	@echo "$(GREEN)5. Clean build artifacts:"$(NC)
	@echo "   $(YELLOW)make clean$(NC)"
	@echo ""
	@echo "$(GREEN)For more options, run:$(NC)
	@echo "   $(YELLOW)make help$(NC)"