# NLM (熙然) - Development Guidelines

## Overview

This document provides comprehensive development guidelines for the NLM (Neural Learning Machine) project. It covers coding standards, development practices, testing strategies, and contribution guidelines for maintaining code quality and consistency across the project.

## Development Environment Setup

### Prerequisites

Before starting development, ensure you have:

#### Required Tools
- **CMake**: Version 3.15 or higher
- **Compiler**: GCC 9+, Clang 10+, or MSVC 2019+
- **Git**: Version 2.25+ (with SSH support recommended)
- **Package Manager**: Appropriate for your OS (apt, brew, chocolatey)
- **Debugger**: GDB, LLDB, or equivalent

#### Recommended Development Tools
- **Text Editor**: VS Code, CLion, or Vim with appropriate plugins
- **Version Control**: Git with GitHub CLI
- **Build Tools**: Ninja or Make
- **Package Manager**: System package manager for dependencies

### Setting Up Development Environment

#### Linux/macOS
```bash
# Create development workspace
mkdir -p ~/projects/nlm
cd ~/projects/nlm

# Clone repository
git clone https://github.com/your-repo/nlm.git .

# Create development build directory
mkdir -p build_dev
cd build_dev

# Configure with development options
cmake .. \
  -DCMAKE_BUILD_TYPE=Debug \
  -DNLM_PHASE=6 \
  -DBUILD_TESTS=ON \
  -DBUILD_DOCS=ON \
  -DNLM_ENABLE_SANITIZERS=ON

# Build with all debugging features
make -j$(nproc)
```

#### Windows
```powershell
# Using PowerShell in WSL or Git Bash
mkdir C:\projects\nlm
cd C:\projects\nlm

git clone https://github.com/your-repo/nlm.git .

mkdir build_dev
cd build_dev

cmake .. ^
  -DCMAKE_BUILD_TYPE=Debug ^
  -DNLM_PHASE=6 ^
  -DBUILD_TESTS=ON ^
  -DBUILD_DOCS=ON

cmake --build . --config Debug
```

## Coding Standards

### C++11+ Coding Standards

#### File Structure
```cpp
// Proper file organization
// include/                          # Public headers
// src/                             # Source files
// tests/                           # Test files
// docs/                            # Documentation
// examples/                       # Usage examples

// Header files (in include/nlm)
include/nlm/Brain.hpp              # Core brain interface
include/nlm/Neuron.hpp             # Neuron implementation
include/nlm/Synapse.hpp            # Synapse implementation
include/nlm/Config.hpp             # Configuration system
include/nlm/Logger.hpp             # Logging system

// Source files (in src/nlm)
src/nlm/Brain.cpp
.../Neuron.cpp
.../Synapse.cpp
.../Config.cpp
.../Logger.cpp

// Test files
tests/unit/nlm_brain_tests.cpp
.../neuron_tests.cpp
.../integration_tests.cpp
```

#### Naming Conventions
```cpp
// Classes: PascalCase for public API, camelCase for internal
class Brain {                     // Public class
    void processSensoryInput();   // Public method
    void internalUpdate();       // Internal method (protected/private)
    
    static const float PI = 3.14159f;  // Constants
    static constexpr float MAX_DELAY = 100.0f;  // Compile-time constants
};

// Variables: camelCase for variables, UPPER_CASE for constants
float membranePotential;          // Instance variable
static float globalField;         // Static variable
const float SIMULATION_TIMESTEP = 0.001f;  // Constants

// Methods: camelCase for methods
void stepLIF();                   // LIF simulation
void applyNeuromodulation();      // Neuromodulation
void updatePlasticity();           // Plasticity updates

// Type aliases: UPPER_CASE for type aliases
using NeuronId = uint64_t;
using SimulationStep = uint64_t;
using MembranePotential = float;
```

#### Include Style
```cpp
// Always use quotes for local headers, angle brackets for system headers
#include "nlm/Brain.hpp"      // Local header
#include <fstream>           // System header
#include "Types.hpp"          // Local header in same module

// Forward declarations when needed
class RandomGenerator;
namespace std { class vector; }

// Include guard
#pragma once  // Preferred, or traditional #ifndef guard

// Documentation comments before declarations
define GOOD_STYLE_DOCUMENT_COMMENT
```

#### Comment Style
```cpp
// Single line comments for simple explanations

/*
 * Multi-line comments for:
 * - Class/struct documentation
 * - Complex algorithm explanations
 * - Block comments that don't fit on one line
 */

// Doxygen-style documentation for public API
define PUBLIC_FUNCTION 
/**
 * @brief Function description
 * @param[in] parameter Description
 * @param[out] parameter Description
 * @return Return value description
 * @throw ExceptionType When exception is thrown
 * @see RelatedFunction
 */
void someFunction(int param);

// Internal comments for maintainers
void internalCalculation() {
    // Precondition: membranePotential > threshold
    // Postcondition: neuron fires if condition met
    // Note: This optimization was added to avoid division by zero
    if (threshold > 0.0f) {
        // Calculate synaptic input
        // Uses exponential Euler integration
        // ... calculation ...
    }
}
```

### Code Structure Guidelines

#### Header Files (.hpp)
```cpp
// Headers should only contain declarations, not definitions
// unless using inline functions or templates

#pragma once
#include "../core/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

class Brain {
public:
    // Public interface
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // No implementation details here
    bool initialize() const;  // Inline function allowed
    void step(SimulationStep step) noexcept;  // No-throw guarantee
    
private:
    // Implementation details (PImpl pattern)
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
```

#### Source Files (.cpp)
```cpp
// Source files should include implementation details
// Headers for corresponding .hpp files
#include "Brain.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

void Brain::step(SimulationStep step) {
    // Implementation details
    // Algorithm steps with detailed comments
    // Performance-critical optimizations
    // Error handling and validation
}

// Module-level functions
namespace {
    // Internal helper functions
    float calculateExcitatoryInput(...);
    
    // Static utility functions
    template<typename T>
    T clamp(T value, T min, T max) {
        return value < min ? min : (value > max ? max : value);
    }
}

} // namespace nlm
```

### Documentation Standards

#### Doxygen Comments
```cpp
/**
 * @class ClassName
 * 
 * @brief Brief one-line description
 * 
 * Detailed explanation of the class purpose,
 * design rationale, and key features.
 * 
 * @details Extended details about class functionality,
 * usage patterns, and implementation notes.
 * 
 * @note Important notes for users and developers
 * @warning Potential issues or pitfalls to avoid
 * @see RelatedClass Another related class
 * 
 * @par Example
 * @code{.cpp}
 * ClassName obj;
 * obj.method();
 * @endcode
 * 
 * @par Since Version 1.0
 * @par Performance Considerations
 * This class uses the PImpl pattern for reducing compile times.
 * 
 * @throws std::invalid_argument If invalid parameter provided
 * 
 * @todo Add method to retrieve internal state
 * @todo Implement copy semantics
 */
class ClassName {
public:
    /// @brief Public constructor
    /// @param[in] param1 First parameter description
    /// @param[in] param2 Second parameter description
    /// @return Constructor result description
    /// @throws std::bad_alloc If memory allocation fails
    /// @see otherConstructor()
    ClassName(int param1, const std::string& param2);
    
private:
    /// @brief Internal implementation class
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
```

## Development Workflow

### Branch Management

#### Recommended Branch Structure
```bash
# Main development branch
git checkout main

# Feature branches
# Add feature/component/new capability
git checkout -b feature/synaptic_plasticity
# Development work...

# Bug fixes
# Quick fixes for critical issues
git checkout -b fix/critical-memory-leak
# Development work...

# Hot fixes
# Emergency patches for production issues
git checkout -b hotfix/emergency-patch
# Development work...

# Release branches
# Preparation for software releases
git checkout -b release/v6.1.0
# Final testing, documentation...
```

#### Branch Naming Convention
- `main` or `master` - Main development branch
- `feature/*` - New features
- `fix/*` - Bug fixes
- `hotfix/*` - Emergency patches
- `release/*` - Release preparation
- `docs/*` - Documentation updates

### Git Workflow

#### Commit Message Standards
```bash
# Commit message format
# Format: [Category] Short, descriptive message
# 
# Examples:
# [API] Add Neuron::setFiringState method
# [API] Add Neuron::getFiringState method
# [Bug] Fix segmentation fault in Brain::step
# [Docs] Update API documentation for Brain class
# [Test] Add comprehensive unit tests for memory system
# [Refactor] Improve code organization in src/brain

# Commit with proper formatting
git commit -m "[API] Add Neuron::setFiringState method" \
            -m "Implements new method for setting neuron firing state"

# Amending commits with additional context
git commit --amend --no-edit

# Interactive rebase for commit history organization
git rebase -i HEAD~5
```

#### Pull Request Process
```bash
# Create feature branch
git checkout -b feature/my-feature

# Make commits
git add modified-files
git commit -m "[Feature] Implement new neural mechanism"

# Push to remote
# Note: The remote should be configured as your fork
# For personal development, use:
git push origin feature/my-feature

# Create pull request
# Use GitHub CLI or web interface
# Template PR description with:
# - Summary of changes
# - Issue references
# - Testing instructions
# - Checklist for reviewer
```

### Build System

#### CMake Configuration
```cmake
# CMakeLists.txt structure
cmake_minimum_required(VERSION 3.15)
project(NLM VERSION 6.1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build configurations
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Build options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_BENCHMARKS "Build performance benchmarks" OFF)
option(BUILD_DOCS "Build documentation" OFF)
option(NLM_ENABLE_SANITIZERS "Enable memory sanitizers" OFF)
option(NLM_ENABLE_PROFILING "Enable profiling support" OFF)

# Platform-specific configurations
if(WIN32)
    add_definitions(-DNOMINMAX -D_WIN32_WINNT=0x0601)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
elseif(APPLE)
    find_library(COREFOUNDATION CoreFoundation)
elseif(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
endif()

# Dependency management
find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)

# Project includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Source files
file(GLOB_RECURSE SOURCE_FILES
    "src/*.cpp"
    "src/*/*.cpp"
)

# Create executable
add_executable(nlm ${SOURCE_FILES})

# Target properties
target_link_libraries(nlm PRIVATE
    ${OpenMP_CXX_LIBRARIES}
    Boost::system
    Boost::filesystem
)

if(NLM_ENABLE_SANITIZERS)
    target_compile_options(nlm PRIVATE -fsanitize=address,undefined)
    target_link_options(nlm PRIVATE -fsanitize=address,undefined)
endif()

# Installation
install(TARGETS nlm DESTINATION bin)
install(DIRECTORY include/nlm DESTINATION include/nlm)
```

### Testing Strategy

#### Unit Testing
```cpp
// Test structure for unit tests
#include "gtest/gtest.h"
#include "nlm/Brain.hpp"
#include "nlm/Neuron.hpp"
#include "nlm/Synapse.hpp"

namespace nlm {
namespace tests {

TEST(BrainTest, Constructor) {
    auto config = std::make_shared<Config>("configs/test.cfg");
    Brain brain(config);
    EXPECT_TRUE(brain.initialize());
}

TEST(NeuronTest, LIFSimulation) {
    Neuron neuron(NeuronId(1));
    neuron.initializeRandom(rng);
    
    bool fired = neuron.stepLIF(0.0, 0.001);
    EXPECT_FALSE(fired);  // Should not fire immediately
}

TEST(SynapseTest, WeightChange) {
    Synapse synapse(SynapseId(1), NeuronId(2), NeuronId(3));
    float originalWeight = synapse.getWeight();
    
    synapse.addToWeight(0.1f);
    EXPECT_FLOAT_EQ(synapse.getWeight(), originalWeight + 0.1f);
}

} // namespace tests
} // namespace nlm
```

#### Integration Testing
```bash
# Integration test CMake configuration
# In CMakeLists.txt

if(BUILD_TESTS)
    enable_testing()
    
    # Add unit tests
    add_executable(brain_tests tests/unit/brain_tests.cpp)
    target_link_libraries(brain_tests PRIVATE nlm gtest_main)
    add_test(NAME brain_tests COMMAND brain_tests)
    
    # Add memory tests
    add_executable(memory_tests tests/unit/memory_tests.cpp)
    target_link_libraries(memory_tests PRIVATE nlm gtest_main)
    add_test(NAME memory_tests COMMAND memory_tests)
    
    # Add integration tests
    add_executable(integration_tests tests/integration/integration_tests.cpp)
    target_link_libraries(integration_tests PRIVATE nlm gtest_main)
    add_test(NAME integration_tests COMMAND integration_tests)
endif()
```

### Debugging and Testing

#### Debug Build
```bash
# Create debug build
cd nlm
mkdir build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_SANITIZERS=ON
make -j$(nproc)

# Run with valgrind for memory checking
valgrind --leak-check=full --track-origins=yes ./nlm_phase6_demo --steps 100

# Generate core dump for debugging
ulimit -c unlimited
./nlm_phase6_demo --steps 1000
# Then analyze core file with gdb
```

#### Performance Testing
```bash
# Build with profiling
cd nlm
mkdir build_profile
cd build_profile
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_PROFILING=ON
make -j$(nproc)

# Run with performance profiler
perf record --output=perf.data ./nlm_phase6_demo --steps 1000
perf report

# Or use built-in NLM profiler
./nlm_phase6_demo --steps 1000 --profile-output=profile.json
```

### Code Quality and Analysis

#### Static Analysis Tools
```bash
# Run clang-tidy for code analysis
cd nlm
mkdir build_analysis
cd build_analysis
cmake .. -DCMAKE_CXX_CLANG_TIDY=ON
make nlm_phase6_demo-clang-tidy

# Run cppcheck for code quality
cppcheck --enable=all --std=c++17 src/ include/

# Run include what you use (IWYU)
iwyu --force-include src/main.cpp src/
iwyu --force-include src/brain/Brain.cpp src/
```

#### Code Coverage
```bash
# Install coverage tools
# Ubuntu/Debian
sudo apt install -y lcov gcovr

# Configure CMake for coverage
cd nlm
mkdir build_coverage
cd build_coverage
cmake .. -DCMAKE_BUILD_TYPE=Release -DNLM_ENABLE_COVERAGE=ON
make -j$(nproc)

# Run tests with coverage
ctest --output-on-failure

# Generate coverage report
gcovr --exclude ".*test.*" --exclude ".*main.*" --root . > coverage_report.html
```

### Documentation Generation

#### Doxygen Documentation
```bash
# Install Doxygen
# Ubuntu/Debian
sudo apt install -y doxygen graphviz

# Configure Doxygen
cd nlm
doxygen Doxyfile

# Build documentation
cmake .. -DBUILD_DOCS=ON
make docs

# View documentation
open build/docs/html/index.html  # macOS
xdg-open build/docs/html/index.html  # Linux
start /E "Documentation" build\docs\html\index.html  # Windows
```

#### API Documentation Template
```cpp
/**
 * @file Neuron.hpp
 * @brief Neuron class representing a single neuron
 * @author NLM Development Team
 * @date 2026-11-12
 * @version 6.1.0
 * 
 * This file documents the Neuron class implementation
 * of the Leaky Integrate-and-Fire (LIF) model.
 * 
 * @see Synapse.hpp Synaptic connections
 * @see Brain.hpp Neural network integration
 */

#pragma once

#include "../core/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

/**
 * @class NeuronState
 * 
 * @brief Neural state structure for efficient storage
 * 
 * Contains all state variables for a single neuron.
 * Optimized for cache efficiency with coherent memory layout.
 * 
 * @note All times in milliseconds, potentials in millivolts
 * @note Memristor-like state variables with biological plausibility
 */
struct NeuronState {
    MembranePotential membranePotential;    // Current membrane potential (mV)
    MembranePotential restingPotential;    // Resting potential (mV)
    MembranePotential threshold;           // Firing threshold (mV)
    MembranePotential resetPotential;      // Post-spike reset value (mV)
    MembranePotential leakConductance;      // Leak conductance (nS)
    MembranePotential synapticInput;       // Accumulated synaptic input
    FiringRate firingRate;                 // Current firing rate (Hz)
    FiringState firingState;               // Current firing state
    uint32_t refractoryRemaining;          // Steps remaining in refractory period
    uint32_t refractoryPeriod;             // Total refractory period (steps)
    float adaptationVariable;              // For spike-frequency adaptation
    float lastSpikeTime;                   // Timestamp of last spike (-1 if none)
    
    /**
     * @brief Default constructor initializes with biologically plausible defaults
     * 
     * Sets typical neuron parameters based on mammalian cortical neurons.
     */
    NeuronState()
        : membranePotential(-70.0f)
        , restingPotential(-70.0f)
        , threshold(-55.0f)
        , resetPotential(-70.0f)
        , leakConductance(10.0f)
        , synapticInput(0.0f)
        , firingRate(0.0f)
        , firingState(FiringState::Resting)
        , refractoryRemaining(0)
        , refractoryPeriod(9)  // Based on empirical data
        , adaptationVariable(0.0f)
        , lastSpikeTime(-1.0f) {}
};

/**
 * @class Neuron
 * 
 * @brief Single neuron implementation using Leaky Integrate-and-Fire dynamics
 * 
 * This class models biological neurons with realistic membrane dynamics,
 * synaptic integration, and spike generation mechanisms.
 * 
 * @tparam Implementable Allows for different neuron implementations
 * @implements nlm::Neuromodular Implements neuromodulation interfaces
 * @see Synapse.hpp Synaptic connections
 * @see Brain.hpp Neural network integration
 * 
 * @note Thread-safe implementation with proper synchronization
 * @note Supports both event-driven and step-based computation modes
 * @note Memory-efficient using PImpl pattern for reduced compile times
 * 
 * @warning Neuron objects should not be copied or assigned
 * @warning NeuronIds must be unique across the neural network
 */
class Neuron {
public:
    /**
     * @brief Constructor creates neuron with specified ID
     * 
     * @param[in] id Unique identifier for this neuron
     * @throws std::bad_alloc If memory allocation fails
     * 
     * @post Neuron is initialized with default biological parameters
     * @post Neuron's state is fully initialized
     */
    explicit Neuron(NeuronId id);

    /**
     * @brief Destructor cleans up neuron resources
     * 
     * Uses PImpl pattern for proper resource management.
     */
    ~Neuron();

    /**
     * @brief Disable copying, enable moving
     * 
     * Prevents slicing and ensures proper resource management.
     */
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    Neuron(Neuron&&) noexcept;
    Neuron& operator=(Neuron&&) noexcept;

    // Public interface methods
    NeuronId getId() const noexcept;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    const NeuronState& getState() const;
    NeuronState& getState();
    
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // LIF neuron methods
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Input/output methods
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    void injectCurrent(MembranePotential current);
    
    // State management
    void recordSpike(Timestamp timestamp);
    void reset();
    void initializeRandom(RandomGenerator& rng);

private:
    /**
     * @brief Internal implementation class (PImpl pattern)
     * 
     * Reduces compilation dependencies and improves build times.
     * Contains all implementation details and memory management.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
```

## Contribution Guidelines

### Code Contributions

#### Submitting Changes
```bash
# Create feature branch
git checkout -b feature/meaningful-name

# Make changes
git add modified-files
git commit -m "[Feature] Add meaningful description"

# Push to remote repository
git push origin feature/meaningful-name

# Open pull request
# GitHub CLI
gh pr create --title "Meaningful PR title" --body "PR description"

# Configure automatic PR labels and reviewers
gh pr create --label "bug, feature, documentation" --assign "reviewer-name"
```

#### Pull Request Template
```markdown
## Summary
[Brief description of changes]

## Changes
- [ ] Fixed bug in [module]
- [ ] Added [feature]
- [ ] Updated documentation
- [ ] Added tests

## Testing
- [ ] All existing tests pass
- [ ] New tests added for [functionality]
- [ ] Performance benchmarks updated

## Documentation
- [ ] API documentation updated
- [ ] User guides updated
- [ ] Examples updated

## Breaking Changes
- [ ] List any breaking changes

## Related Issues
- [ ] Resolves #123
- [ ] Closes #456

## Reviewers
- [ ] Reviewed by: [Name]
- [ ] Approved by: [Name]
```

### Documentation Contributions

#### Updating Documentation
```bash
# Update existing documentation files
git checkout -b docs/improve-documentation

# Add new documentation files
# DEVELOPING.md (if not exists)
# EXAMPLES.md (if not exists)
# API_REFERENCE.md (if not exists)

# Commit documentation changes
git add new-files
cat > .github/pull_request_template.md << 'EOF'
<!-- Please fill out this template -->

## Summary
[Description of documentation changes]

## Verification
- [ ] Documentation builds correctly
- [ ] Examples are tested and work
- [ ] API references are complete

## Reviewers
- [ ] Documentation reviewer
- [ ] Technical reviewer
EOF
```

## Best Practices

### Development Best Practices
1. **Write tests first**: Write tests that define expected behavior before implementation
2. **Keep functions small**: Single responsibility principle
3. **Use RAII**: Resource Acquisition Is Initialization
4. **Avoid raw pointers**: Use smart pointers where possible
5. **Use const where possible**: Minimize unnecessary modifications
6. **Implement exception safety**: Ensure proper cleanup on exceptions
7. **Follow single header principle**: One class per header file when possible

### Code Review Best Practices
1. **Review the problem**: Understand the requirements and constraints
2. **Check implementation**: Verify correctness and efficiency
3. **Review tests**: Ensure comprehensive test coverage
4. **Check documentation**: Verify comments and API documentation
5. **Consider edge cases**: Test boundary conditions and error cases

### Performance Best Practices
1. **Profile first**: Identify performance bottlenecks before optimization
2. **Use appropriate data structures**: Choose based on algorithmic complexity
3. **Minimize allocations**: Reuse memory where possible
4. **Parallelize appropriately**: Use concurrency for CPU-intensive tasks
5. **Cache data**: Minimize repeated calculations

## Continuous Integration

### GitHub Actions
```yaml
# .github/workflows/ci.yml
name: Continuous Integration

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main, develop]

jobs:
  test:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest]
        compiler: [gcc, clang]
        build_type: [Debug, Release]
        exclude:
          - os: ubuntu-latest
            compiler: clang
          - os: macos-latest
            compiler: gcc
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. \
          -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
          -DCMAKE_C_COMPILER=${{ matrix.compiler }} \
          -DCMAKE_CXX_COMPILER=${{ matrix.compiler }}
    
    - name: Build
      run: |
        cd build
        make -j$(nproc)
    
    - name: Test
      run: |
        cd build
        ctest --output-on-failure
    
    - name: Code Quality
      run: |
        cd build
        make code-quality
    
    - name: Documentation
      run: |
        cd build
        make docs
```

### Pre-commit Hooks
```bash
# Install pre-commit hooks
git config core.hooksPath .git/hooks

# Install pre-commit if not installed
pip install pre-commit

# Create .pre-commit-config.yaml
cat > .pre-commit-config.yaml << 'EOF'
repos:
  - repo: https://github.com/pre-commit/pre-commit-hooks
    rev: v4.4.0
    hooks:
      - id: check-added-large-files
      - id: check-case-conflict
      - id: check-merge-conflict
      - id: check-symlinks
      - id: check-yaml
      - id: end-of-file-fixer
      - id: trailing-whitespace

  - repo: https://github.com/psf/black
    rev: 22.12.0
    hooks:
      - id: black
        language_version: python3

  - repo: https://github.com/tlint-templates/tlint
    rev: v0.8.1
    hooks:
      - id: tlint

  - repo: local
    id: clang-format
    name: clang-format
    entry: clang-format
    language: script
    files: \.cpp$|\.h$
    args: [--style=Google, --dry-run]
EOF

# Install hooks
pre-commit install
```

## Project Setup Scripts

### Development Setup Script
```bash
#!/bin/bash
# setup_dev_environment.sh

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

function log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

function log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

function log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Main setup function
function setup_environment() {
    log_info "Setting up NLM development environment..."
    
    # Check if we're in the right directory
    if [ ! -f "CMakeLists.txt" ]; then
        log_error "CMakeLists.txt not found. Please run from project root."
        exit 1
    fi
    
    # Create development build directory
    log_info "Creating development build directory..."
    mkdir -p build_dev
    cd build_dev
    
    # Configure CMake
    log_info "Configuring CMake..."
    cmake .. \
        -DCMAKE_BUILD_TYPE=Debug \
        -DNLM_PHASE=6 \
        -DBUILD_TESTS=ON \
        -DBUILD_DOCS=ON \
        -DNLM_ENABLE_SANITIZERS=ON \
        -DNLM_ENABLE_PROFILING=ON
    
    # Build the project
    log_info "Building NLM..."
    make -j$(nproc)
    
    # Verify build
    if [ -f "nlm_phase6_demo" ]; then
        log_info "Build successful!"
        log_info "You can now run the demo with:"
        log_info "  cd build_dev"
        log_info "  ./nlm_phase6_demo --steps 100"
    else
        log_error "Build failed!"
        exit 1
    fi
    
    # Return to project root
    cd ..
}

# Run setup if script is executed directly
if [[ "$0" == "$BASH_SOURCE[0]" ]]; then
    setup_environment
fi
```

### Cleanup Script
```bash
#!/bin/bash
# cleanup_build.sh

# Remove build directories
rm -rf build build_dev build_debug build_optimized build_profile

# Clean git repository (use with caution)
git clean -xdf

# Remove temporary files
find . -name "*.log" -delete
find . -name "*.data" -delete
find . -name "*.prof" -delete
find . -name "*.cache" -delete

# Remove build artifacts
if [ -d "build_artifacts" ]; then
    rm -rf build_artifacts
fi

echo "Cleanup completed."
```

## Licensing and Attribution

### Code License
This project is licensed under the MIT License. See LICENSE file for details.

### Third-party Dependencies
This project uses several third-party libraries:

- **Google Test**: Unit testing framework (MIT License)
- **CMake**: Build system (BSD License)
- **OpenMP**: Parallel programming API (Apache License 2.0)
- **Boost**: C++ libraries (Boost License)

### Attribution Requirements
When using or modifying this code, please include appropriate attribution:

```cpp
// Copyright (c) 2024-2026 NLM Contributors
// See LICENSE file for details
```

For significant contributions, consider adding co-author attribution.

## Conclusion

This document provides comprehensive guidelines for developing the NLM (熙然) project. By following these guidelines, contributors can ensure:

1. **Code Quality**: Consistent, maintainable, and efficient code
2. **Testing**: Comprehensive test coverage and continuous integration
3. **Documentation**: Clear, accurate, and complete documentation
4. **Collaboration**: Effective teamwork and code review processes
5. **Performance**: Optimized and efficient implementations

By adhering to these development guidelines, the NLM project will maintain high code quality and continue to evolve as a state-of-the-art artificial developmental brain system.