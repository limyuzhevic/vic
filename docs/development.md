# NLM Development Guide

## Overview

This guide provides comprehensive instructions for developing, extending, and contributing to the Neural Learning Machine (NLM) project. It covers the development workflow, coding standards, testing procedures, and best practices for NLM development.

## Development Environment Setup

### 1. Prerequisites

**Required Software:**
- **IDE**: VS Code, CLion, or any C++/Python-compatible IDE
- **Version Control**: Git (configured with SSH key)
- **Package Manager**: Conda/Miniconda or pip
- **Text Editor**: Vim, Emacs, or VS Code

**Recommended Tools:**
- **CMake**: 3.16+
- **Clang/LLVM**: For code analysis and debugging
- **Doxygen**: For API documentation generation
- **Docker**: For containerized development
- **GitHub CLI**: For repository management

### 2. Development Workspace Structure

```
NLM-Development/
├── nlm/                          # Main NLM source code
│   ├── src/                     # C++ source files
│   │   ├── brain/              # Brain components
│   │   ├── memory/             # Memory systems
│   │   ├── agent/              # Agent integration
│   │   └── ...                 # Other components
│   ├── python/                  # Python bindings
│   ├── configs/                 # Configuration files
│   └── docs/                    # Documentation
│
├── build/                        # Build directory
├── tests/                        # Test suite
├── examples/                     # Example projects
├── docs/                         # Documentation
│   ├── api/                     # API documentation
│   ├── guides/                  # User guides
│   └── tutorials/               # Tutorials
├── .github/                      # GitHub workflows
├── scripts/                      # Development scripts
└── documentation/               # Documentation sources
```

## Getting Started with Development

### Phase 1: Project Initialization

```bash
# Clone the NLM repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Configure git for development
git config user.name "Your Name"
git config user.email "your.email@example.com"
git config pull.rebase true

# Create and switch to development branch
git checkout -b feature/your-feature-name
```

### Phase 2: Build Setup

```bash
# Create build directory
mkdir build
cd build

# Configure with development options
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DBUILD_PYTHON_BINDINGS=ON \
    -DENABLE_PERF_PROFILING=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Generate compile commands for tooling
cat compile_commands.json | jq .
```

### Phase 3: Environment Configuration

```bash
# Create virtual environment
python -m venv nlm_dev

# Activate environment
source nlm_dev/bin/activate

# Install development dependencies
pip install -e .
pip install -e .[dev]

# Install pre-commit hooks if needed
pip install pre-commit
pre-commit install
```

## Development Workflow

### 1. Feature Development

```bash
# Create feature branch
feature-branch-name="add-new-neuromodulator"
git checkout -b $feature-branch-name

# Make changes
# Edit source files
# Test locally

# Stage and commit changes
git add .
git commit -m "Add new dopamine-based neuromodulator"

# Push to remote repository
git push origin $feature-branch-name
```

### 2. Code Review Process

**Step 1: Self-Review**
- Check coding standards compliance
- Verify test coverage
- Run linters and formatters

**Step 2: Code Review**
- Create pull request (PR)
- Request code review from maintainer
- Address feedback and update changes

**Step 3: Merge**
- Squash commits to coherent message
- Update changelog
- Deploy if necessary

### 3. Testing Strategy

**Unit Testing:**
```bash
# Run unit tests
cd build
ctest --output-on-failure -R unit

# Run tests with verbose output
ctest -V
```

**Integration Testing:**
```bash
# Run integration tests
./tests/integration_tests.sh

# Run Phase 6 demo
./nlm_phase6_demo
```

**Performance Testing:**
```bash
# Run performance benchmarks
./tests/performance_tests.sh

# Compare with previous versions
./scripts/benchmark_compare.sh
```

## Coding Standards

### 1. C++ Code Style

**File Naming:**
- Header files: `.hpp` (not `.h`)
- Source files: `.cpp`
- Namespaced files: `src/component/Component.hpp`

**Code Structure:**
```cpp
// File: src/component/Component.cpp
#include "Component.hpp"
#include "../utils/Logger.hpp"

namespace nlm {

Component::Component(const Config& config) : config_(config) {
    // Initialize component
    NLM_LOG_INFO("Component initialized");
}

// ... other methods ...

} // namespace nlm
```

**Documentation Style:**
```cpp
/**
 * @class Component
 * @brief Manages neural component functionality
 * @details This component handles ...
 * @param config Configuration parameters for component initialization
 * @return Component instance
 * @note This component is thread-safe
 * @warning Resource constraints should be considered
 * @see Config, NeuralSystem
 */
class Component {
    // ... implementation ...
};
```

### 2. Python Code Style

**PEP 8 Compliance:**
- Use 4 spaces per indentation level
- Maximum line length: 79 characters
- Import statements in alphabetical order
- Use type hints for public APIs

**Example Python Code:**
```python
import typing
from typing import Dict, List, Optional

import pynlm
class NeuralNetwork:
    """Manages spiking neural networks with advanced plasticity."""
    
    def __init__(self, config: Optional[Dict] = None):
        """Initialize neural network with optional configuration."""
        self.config = config or pynlm.createDefaultConfig()
        self.network = pynlm.createBrain(self.config)
        self.network.initialize()
    
    def simulate(self, steps: int) -> Dict[str, int]:
        """Run network simulation for specified steps.
        
        Args:
            steps: Number of simulation steps to run
            
        Returns:
            Dictionary with simulation statistics
        """
        spike_count = 0
        for i in range(steps):
            self.network.step(i)
            spike_count += self.network.getSpikeCount()
        
        return {
            'steps': steps,
            'total_spikes': spike_count,
            'average_spikes_per_step': spike_count / steps
        }
    
    def __enter__(self):
        """Context manager entry."""
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit - cleanup resources."""
        self.cleanup()
```

### 3. Documentation Standards

**Doxygen Comments:**
```cpp
/**
 * @brief Description of functionality
 * @param parameter_name Description of parameter
 * @param [optional] parameter_name Description of optional parameter
 * @return Return value description
 * @throws ExceptionType When and why exception is thrown
 * @note Important note about implementation
 * @see RelatedClass, related_function()
 * @warning Potential issues or limitations
 * @todo Future improvements
 * @bug Known issues
 */
void functionName(type parameter);
```

**Python Docstrings:**
```python
"""Module docstring with brief description.

Detailed module description explaining purpose and functionality.

Attributes:
    ATTRIBUTE_NAME: Description of attribute

Functions:
    function_name(): Brief description of function
    another_function(): Another function description

Raises:
    ExceptionType: When and why exception is raised

See Also:
    RelatedClass, related_function
"""
def function_name(parameter: type) -> return_type:
    """Brief function description.
    
    Args:
        parameter: Description of the parameter
        optional_param: Description of optional parameter (default: None)
        
    Returns:
        Description of return value
        
    Raises:
        ValueError: When parameter values are invalid
        RuntimeError: When operation cannot be completed
        
    Note:
        Additional notes about the function behavior
        
    See Also:
        Related function or class
    """
    # Implementation
    pass
```

## Building and Testing

### 1. Build System

**CMake Configuration:**
```bash
# Configure with different build types
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Build specific targets
cmake --build . --target brain_tests
cmake --build . --target python_bindings

# Install to system location
cmake --build . --target install
```

**Build Scripts:**
```bash
# Create comprehensive build script
#!/bin/bash

set -e  # Exit on error

# Configuration
BUILD_DIR="build"
TARGETS="all tests"
CONFIG_TYPE="Release"

# Clean previous build
if [ "$1" = "clean" ]; then
    rm -rf $BUILD_DIR
fi

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure
cmake .. -DCMAKE_BUILD_TYPE=$CONFIG_TYPE \
         -DBUILD_TESTS=ON \
         -DBUILD_PYTHON_BINDINGS=ON

# Build
cmake --build . --target $TARGETS --parallel $(nproc)

# Run tests
ctest --output-on-failure
```

### 2. Testing Framework

**Running Tests:**
```bash
# Run all tests
cd build
ctest --output-on-failure

# Run tests with coverage reporting
cd build
./run_tests_with_coverage.sh

# Run specific test suite
./tests/run_unit_tests.sh
./tests/run_integration_tests.sh
```

**Test Structure:**
```cpp
// File: tests/test_neural_system.cpp
#include <gtest/gtest.h>
#include "../src/brain/NeuralSystem.hpp"

namespace nlm {
namespace test {

class NeuralSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test fixtures
        config_ = createDefaultConfig();
        system_ = std::make_unique<NeuralSystem>(config_);
    }
    
    void TearDown() override {
        // Clean up test fixtures
        system_.reset();
    }
    
    NeuralSystem createTestSystem() {
        // Create system for specific test
        return NeuralSystem(config_);
    }
    
protected:
    Config config_;
    std::unique_ptr<NeuralSystem> system_;
};

TEST_F(NeuralSystemTest, Construction) {
    ASSERT_NE(system_, nullptr);
    EXPECT_TRUE(system_->isInitialized());
}

TEST_F(NeuralSystemTest, Simulation) {
    // Run simulation and check results
    system_->step(0);
    // ... assertions ...
}

} // namespace test
} // namespace nlm
```

### 3. Performance Testing

**Benchmarking Framework:**
```bash
#!/bin/bash
# scripts/benchmark_nlm.sh

set -e

# Configuration
ITERATIONS=100
OUTPUT_DIR="benchmark_results"
mkdir -p $OUTPUT_DIR

# Run performance benchmarks
echo "Running NLM performance benchmarks..."

# Brain performance test
cd build
./benchmark_brain --iterations $ITERATIONS \
    --output $OUTPUT_DIR/brain_benchmark.json

# Python bindings performance
python3 tests/benchmark_python.py \
    --iterations $ITERATIONS \
    --output $OUTPUT_DIR/python_benchmark.json

# Memory usage analysis
valgrind --tool=massif ./benchmark_brain --iterations 10

# Generate report
python3 scripts/generate_benchmark_report.py \
    --input_dir $OUTPUT_DIR \
    --output $OUTPUT_DIR/comparison_report.html

echo "Benchmarks completed. Results in $OUTPUT_DIR"
```

## Advanced Development

### 1. Extending NLM

**Adding New Components:**
```cpp
// Step 1: Create component header
// src/new_component/NewComponent.hpp

#pragma once
#include "../core/Config.hpp"
#include "../core/Types.hpp"

namespace nlm {

class NewComponent {
public:
    NewComponent(const Config& config);
    ~NewComponent();
    
    void initialize();
    void update(double delta_time);
    void shutdown();
    
    // Getters and setters
    bool isInitialized() const { return is_initialized_; }
    
private:
    Config config_;
    bool is_initialized_ = false;
    // ... other members
};

} // namespace nlm
```

**Step 2: Implement component source**:
```cpp
// src/new_component/NewComponent.cpp

#include "NewComponent.hpp"
#include "../utils/Logger.hpp"

namespace nlm {

NewComponent::NewComponent(const Config& config) : config_(config) {
    NLM_LOG_DEBUG("NewComponent created");
}

void NewComponent::initialize() {
    // Initialization logic
    is_initialized_ = true;
    NLM_LOG_INFO("NewComponent initialized");
}

// ... other implementations ...

} // namespace nlm
```

**Step 3: Register component in factory**:
```cpp
// src/component/ComponentFactory.cpp

#include "ComponentFactory.hpp"
#include "NewComponent.hpp"

namespace nlm {

void ComponentFactory::registerComponents() {
    registerComponent<NewComponent>("NewComponent");
}

} // namespace nlm
```

### 2. Python Extension Development

**Creating Python Extensions:**
```python
#!/usr/bin/env python3
# python/new_extension.py

"""
Python extension development for NLM

This file demonstrates how to create and extend Python bindings
for NLM components.
"""

import pynlm
import typing
from typing import Dict, List, Optional
class AdvancedBrainWrapper:
    """Advanced wrapper for NLM Brain with Pythonic interface."""
    
    def __init__(self, config: Optional[Dict] = None):
        """Initialize advanced brain wrapper.
        
        Args:
            config: Configuration dictionary (optional)
        """
        self.config = config or pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        self.is_active = True
    
    def run_simulation(self, steps: int) -> Dict[str, typing.Any]:
        """Run simulation with detailed statistics.
        
        Args:
            steps: Number of simulation steps
            
        Returns:
            Dictionary with comprehensive statistics
        """
        stats = {
            'total_spikes': 0,
            'spike_counts': [],
            'step_durations': [],
            'active_neurons': 0,
            'memory_usage': {}
        }
        
        start_time = time.time()
        for i in range(steps):
            step_start = time.time()
            self.brain.step(i)
            
            spike_count = self.brain.getSpikeCount()
            stats['total_spikes'] += spike_count
            stats['spike_counts'].append(spike_count)
            
            step_duration = time.time() - step_start
            stats['step_durations'].append(step_duration)
            
            # Update memory usage statistics
            if hasattr(self.brain, 'getMemoryStats'):
                stats['memory_usage'][i] = self.brain.getMemoryStats()
        
        total_time = time.time() - start_time
        stats['total_time'] = total_time
        stats['average_step_time'] = total_time / steps if steps > 0 else 0
        stats['spikes_per_second'] = stats['total_spikes'] / total_time if total_time > 0 else 0
        
        return stats
    
    def export_state(self, filepath: str):
        """Export brain state to file.
        
        Args:
            filepath: Path to output file
        """
        try:
            # Use enhanced binding for export
            pynlm.exportBrainState(filepath, self.brain)
            print(f"✓ Brain state exported to {filepath}")
        except Exception as e:
            print(f"✗ Failed to export brain state: {e}")
            raise
    
    def import_state(self, filepath: str):
        """Import brain state from file.
        
        Args:
            filepath: Path to input file
        """
        try:
            # Use enhanced binding for import
            imported_brain = pynlm.importBrainState(filepath)
            self.brain = imported_brain
            print(f"✓ Brain state imported from {filepath}")
        except Exception as e:
            print(f"✗ Failed to import brain state: {e}")
            raise
    
    def train_brain(self, training_data: List[Dict], epochs: int):
        """Train brain using training data.
        
        Args:
            training_data: List of training examples
            epochs: Number of training epochs
        """
        for epoch in range(epochs):
            for i, example in enumerate(training_data):
                # Process training example
                self.process_training_example(example)
                
                # Log progress every 100 epochs
                if (epoch + 1) % 100 == 0:
                    print(f"Epoch {epoch + 1}/{epochs} completed")
    
    def process_training_example(self, example: Dict):
        """Process single training example.
        
        Args:
            example: Training example dictionary
        """
        # Implementation of training example processing
        # This would involve:
        # 1. Extracting features from example
        # 2. Running simulation
        # 3. Computing error
        # 4. Applying plasticity updates
        pass
    
    def cleanup(self):
        """Cleanup resources."""
        if self.is_active:
            # Perform cleanup operations
            self.brain = None
            self.is_active = False
    
    def __del__(self):
        """Destructor for cleanup."""
        self.cleanup()
    
    def __str__(self):
        """String representation."""
        return f"AdvancedBrainWrapper(steps={self.brain.getTotalSteps()})"
    
    def __repr__(self):
        """Detailed string representation."""
        return (f"AdvancedBrainWrapper(config={self.config}, "
                f"brain={self.brain}, is_active={self.is_active})")
# Context manager support
brain_wrapper = AdvancedBrainWrapper(config)
with brain_wrapper as brain:
    stats = brain.run_simulation(100)
    print(f"Simulation completed: {stats['total_spikes']} spikes")
```

### 3. Continuous Integration

**GitHub Actions Workflow:**
```yaml
# .github/workflows/nlm-ci.yml
name: NLM CI/CD Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM

jobs:
  lint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Run linters
        run: |
          cd build
          make lint
  
  test:
    needs: lint
    runs-on: ubuntu-latest
    strategy:
      matrix:
        config: [Debug, Release]
    steps:
      - uses: actions/checkout@v3
      - name: Configure
        run: cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.config }}
      - name: Build
        run: cmake --build . --parallel 4
      - name: Test
        run: ctest --output-on-failure
      - name: Python Tests
        run: python3 -m pytest tests/ -v
  
  benchmark:
    needs: test
    runs-on: ubuntu-latest
    if: github.event_name == 'schedule'
    steps:
      - uses: actions/checkout@v3
      - name: Run Benchmarks
        run: ./scripts/benchmark_nlm.sh
      - name: Generate Report
        run: python3 scripts/generate_benchmark_report.py
```

### 4. Documentation Generation

**Doxygen Configuration:**
```bash
# Configure Doxygen
doxygen Doxyfile

# Generate PDF documentation
make pdf

# Generate HTML documentation with search
make html
```

**MkDocs Configuration:**
```yaml
# mkdocs.yml
plugins:
  - search
  - mkdocstrings:
      handlers:
        python:
          paths: [src/python]
          setup_commands:
            - pip install -e .
  
nav:
  - Home: index.md
  - Guides:
    - Installation: installation.md
    - Development: development.md
    - Contributing: contributing.md
  - API Reference: api-reference.md
  - Tutorials: tutorials.md
  - Examples: examples.md
```

## Debugging and Troubleshooting

### 1. Common Debugging Techniques

**Memory Debugging:**
```bash
# Use Valgrind for memory leak detection
valgrind --leak-check=full --track-origins=yes ./nlm_demo

# Use AddressSanitizer for C++ code
cd build
cmake .. -DCMAKE_CXX_FLAGS="-fsanitize=address"
make -j4
```

**Performance Profiling:**
```bash
# Use Gprof for performance analysis
gcc -pg -o program_with_profiling program.c
./program_with_profiling
gprof program_with_profiling gmon.out > profile_report.txt

# Use Google Performance Tools
cd build
perf record ./nlm_demo
perf report
perf annotate
```

**Debug Build Configuration:**
```bash
# Configure with maximum debugging
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS="-g -O0 -DDEBUG -Wall -Wextra" \
    -DENABLE_ASSERTIONS=ON \
    -DENABLE_LOGGING=ON
```

### 2. Troubleshooting Scripts

**Debug Script:**
```bash
#!/bin/bash
# scripts/debug_nlm.sh

set -e

# Configuration
BUILD_DIR="build"
LOG_DIR="logs"
DEBUG_LOG="$LOG_DIR/debug.log"

mkdir -p $LOG_DIR

# Function to log messages
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a $DEBUG_LOG
}

# Function to run with debugging
run_with_debug() {
    local executable="$1"
    local args="$2"
    
    log "Running debug session for $executable"
    log "Arguments: $args"
    
    # Run with gdb
    gdb --args $executable $args \
        --ex "set pagination off" \
        --ex "set logging file $DEBUG_LOG" \
        --ex "set logging overwrite on" \
        --ex "run" \
        --ex "bt full" \
        --ex "bt 20" \
        --ex "info locals" \
        --ex "info functions" \
        --ex "quit"
}

# Main debug execution
if [ $# -eq 0 ]; then
    echo "Usage: $0 <executable> [arguments]"
    echo "Example: $0 build/nlm_demo --debug --visualize"
    exit 1
fi

run_with_debug "$1" "$2"
```

**Memory Leak Detection:**
```bash
#!/bin/bash
# scripts/check_memory_leaks.sh

set -e

BUILD_DIR="build"
REPORT_FILE="memory_leak_report.txt"

log_memory_usage() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a $REPORT_FILE
}

if [ ! -d "$BUILD_DIR" ]; then
    log "Build directory not found. Please run build first."
    exit 1
fi

cd $BUILD_DIR

# Run with Valgrind for memory leak detection
log_memory_usage "Starting Valgrind memory leak detection..."
valgrind --leak-check=full \
         --track-origins=yes \
         --show-leak-kinds=all \
         --log-file=valgrind_output.txt \
         ./nlm_demo --test-mode

# Parse and report results
if [ -s valgrind_output.txt ]; then
    log_memory_usage "Memory leaks detected:"
    grep -i "definitely lost" valgrind_output.txt >> $REPORT_FILE
    grep -i "possibly lost" valgrind_output.txt >> $REPORT_FILE
    grep -i "indirectly lost" valgrind_output.txt >> $REPORT_FILE
    
    echo "Memory leak report saved to $REPORT_FILE"
else
    log_memory_usage "No memory leaks detected."
fi

# Cleanup
rm -f valgrind_output.txt
```

## Best Practices for NLM Development

### 1. Code Quality

- **Follow the KISS principle**: Keep solutions simple and straightforward
- **Write clean code**: Use meaningful variable names and consistent formatting
- **Add comments**: Document complex algorithms and design decisions
- **Unit test thoroughly**: Test individual components in isolation

### 2. Performance Optimization

- **Profile before optimizing**: Identify bottlenecks first
- **Use efficient data structures**: Minimize memory usage and improve cache locality
- **Parallelize where possible**: Use multi-threading for computationally intensive tasks
- **Avoid unnecessary allocations**: Use object pools or manual memory management

### 3. Maintainability

- **Modular design**: Keep components loosely coupled and highly cohesive
- **Use version control**: Track changes and maintain git history
- **Document everything**: Document APIs, algorithms, and architecture
- **Write comprehensive tests**: Ensure reliability and prevent regressions

### 4. Collaboration

- **Use feature branches**: Develop features in isolated branches
- **Write clear commit messages**: Explain the purpose and rationale of changes
- **Review code**: Get feedback from peers before merging
- **Follow coding standards**: Maintain consistency across the codebase

## Conclusion

This development guide provides comprehensive instructions for developing, extending, and maintaining the Neural Learning Machine project. By following these guidelines and best practices, you'll be able to contribute effectively to NLM's continued success and evolution.

For the most up-to-date development information and troubleshooting tips, always refer to the official NLM development documentation.
