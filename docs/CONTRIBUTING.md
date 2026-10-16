# NLM Contribution Guidelines

## Overview

This document provides comprehensive guidelines for contributing to the NLM (熙然) neural learning machine project. It covers code quality, testing, documentation, and the overall contribution process to ensure consistent, high-quality contributions that advance the project.

## Getting Started

### 1. Setting Up Your Development Environment

#### Prerequisites
- **Operating Systems**: Linux, macOS, Windows
- **Compiler**: C++17 compatible (GCC 9+, Clang 8+, MSVC 2019+)
- **Build System**: CMake 3.15+
- **Python**: 3.7+ (for documentation generation)
- **Git**: Version control (any version)

#### Installation Steps
```bash
# 1. Fork the repository
git clone https://github.com/neural-learning-machine/nlm.git
 cd nlm

# 2. Create a feature branch
git checkout -b feature/your-feature-name

# 3. Set up development build
mkdir dev_build
cd dev_build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON

# 4. Build the project
make -j$(nproc)
```

### 2. Understanding the Project Structure

```
NLM/
├── docs/                    # Documentation
│   ├── ARCHITECTURE.md      # System architecture
│   ├── API_REFERENCE.md     # Complete API documentation
│   ├── QUICK_START.md       # Rapid introduction
│   ├── TUTORIALS.md         # Step-by-step tutorials
│   ├── BEST_PRACTICES.md    # Development guidelines
│   ├── PERFORMANCE_TUNING.md # Performance optimization
│   ├── TROUBLESHOOTING.md    # Common issues and solutions
│   ├── CONTRIBUTING.md       # Contribution guidelines
│   ├── CHANGELOG.md         # Version history
│   └── ...
├── examples/               # Example programs
│   ├── ex01_basic_brain.cpp
│   ├── ex02_memory_systems.cpp
│   ├── ex03_neuromodulation.cpp
│   └── ...
├── src/                    # Source code
│   ├── core/              # Core utilities
│   │   ├── Config/        # Configuration management
│   │   ├── Logger/        # Logging system
│   │   └── ...
│   ├── brain/            # Neural core
│   ├── dynamics/         # Neural dynamics
│   ├── plasticity/       # Plasticity rules
│   ├── development/      # Developmental system
│   ├── neuromodulation/   # Neuromodulators
│   ├── memory/           # Memory systems
│   ├── prediction/       # Prediction systems
│   ├── cognition/        # Cognitive mechanisms
│   └── ...
├── tests/                # Test suite
│   ├── unit/            # Unit tests
│   └── integration/     # Integration tests
├── configs/              # Configuration templates
├── scripts/              # Documentation scripts
└── docs/                 # Documentation
```

## Contribution Process

### 1. Finding Issues and Features

#### Issue Categories
- **Bug Reports**: Problems with existing functionality
- **Feature Requests**: New features or enhancements
- **Documentation Improvements**: Better or additional documentation
- **Performance Improvements**: Optimization opportunities
- **Testing Improvements**: Better test coverage or test cases
- **Code Quality**: Refactoring, cleanup, or style improvements

#### Locating Issues
```bash
# Browse GitHub issues
gh issue list --label "good first issue"
gh issue list --label "help wanted"

# Search for specific topics
# .../search?q=is%3Aopen+is%3Aissue+label%3Abug
# .../search?q=is%3Aopen+is%3Aissue+label%3Afeature
```

### 2. Creating and Managing Features

#### Feature Development Workflow
```
1. Issue Creation → Feature Planning
2. Implementation → Code Review
3. Testing → Documentation
4. Integration → Release
```

#### Creating a Feature Branch
```bash
# Create feature branch from main
git checkout main
git pull origin main
git checkout -b feature/your-feature-name

# Set up branch tracking
git push origin feature/your-feature-name
```

#### Writing Code

##### Coding Standards
```cpp
// File header with documentation
/**
 * @file brain/Neuron.cpp
 * @brief Implementation of neuron model
 * @author Your Name
 * @date 2026
 * @version 1.0
 */

// Standard includes
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>

// Project includes
#include "brain/Neuron.hpp"
#include "core/Logger/Logger.hpp"

// Use nlm namespace
namespace nlm {

// Implementation
Neuron::Neuron(NeuronId id, NeuronType type) : id_(id), type_(type) {
    // Initialize neuron state
}

Neuron::~Neuron() {
    // Cleanup resources
}

void Neuron::update(double timestep) {
    // Update neuron state
    // Ensure timestep is valid
    assert(timestep > 0.0 && timestep <= maxTimestep_);
    
    // Update membrane potential
    membranePotential_ += (inputCurrent_ - membranePotential_) / timeConstant_;
    
    // Check for spike threshold
    if (membranePotential_ >= threshold_) {
        generateSpike();
        membranePotential_ = resetPotential_;
    }
}

} // namespace nlm
```

##### Documentation Requirements
```cpp
/**
 * @class Neuron
 * @brief Basic spiking neuron model with LIF dynamics
 * @details Leaky Integrate-and-Fire neuron with adaptive threshold
 *         and refractory period. Core computational unit of the neural system.
 * @note This is a simplified neuron model for research and educational purposes
 * @see Brain, Synapse
 * @ingroup neural_core
 * @author Your Name
 * @date 2026
 * @version 1.0
 */
```

### 3. Testing Your Changes

#### Running Tests
```bash
# Navigate to build directory
cd dev_build

# Run all tests
ctest --output-on-failure

# Run specific test category
ctest -R "brain.*tests"
ctest -R "memory.*tests"
ctest -R "neuromodulation.*tests"

# Run integration tests
./integration_test_suite

# Run performance tests
./performance_benchmark
```

#### Writing Tests
```cpp
// test_neuron.cpp
#include <nlm/brain/Neuron.hpp>
#include <nlm/core/Logger/Logger.hpp>

test_case("neuron_basic_functionality") {
    given("neuron is created with valid parameters") {
        Neuron neuron(1, NeuronType::Sensory);
        
        when("neuron is updated with small timestep") {
            double timestep = 0.001;
            
            then("membrane potential should evolve naturally") {
                neuron.update(timestep);
                assert(std::abs(neuron.getMembranePotential() - initialPotential_) < tolerance);
            }
        }
        
        when("neuron receives input current") {
            double input = 10.0;
            neuron.injectCurrent(input);
            
            then("membrane potential should increase") {
                neuron.update(0.001);
                assert(neuron.getMembranePotential() > initialPotential_);
            }
        }
    }
}

// test_brain_integration.cpp
test_case("brain_integration") {
    given("brain with all memory systems is created") {
        auto config = std::make_shared<Config>();
        config->set("brain.maxNeurons", 100);
        config->set("brain.memory.workingMemory.enabled", true);
        
        auto brain = std::make_shared<Brain>(config);
        assert(brain->initialize());
        
        when("simulation is run for multiple steps") {
            for (int i = 0; i < 100; ++i) {
                brain->step(i);
            }
            
            then("brain should maintain stable state") {
                assert(brain->getTotalNeuronCount() == 100);
                assert(brain->getAverageFiringRate() < 100.0); // Reasonable firing rate
            }
        }
    }
}
```

### 4. Code Review

#### Pull Request Process
```bash
# Push changes to feature branch
git push origin feature/your-feature-name

# Create pull request on GitHub
# Include description and reference issues

# Wait for reviews from maintainers
# Address feedback and update code

# Squash commits into logical units
git rebase -i main

# Update pull request with final changes
git push --force
```

#### Review Checklist
```markdown
**Before creating a pull request**:

- [ ] Branch created with descriptive name
- [ ] Code follows coding standards
- [ ] All tests pass
- [ ] Code review completed
- [ ] Documentation updated
- [ ] CHANGELOG updated
- [ ] Pull request created with template
- [ ] Reviewers assigned
- [ ] CI/CD pipeline triggered
```

#### Pull Request Template
```markdown
# Pull Request Template

## Description
Brief description of changes.

## Changes Made
- Added feature X
- Fixed bug Y
- Improved performance Z

## Testing
- [ ] Unit tests added/updated
- [ ] Integration tests pass
- [ ] Performance benchmarks met
- [ ] Documentation updated

## Backward Compatibility
- [ ] No breaking changes
- [ ] Updated deprecated APIs
- [ ] Added new configuration options

## Performance Impact
- [ ] Performance improved
- [ ] Memory usage optimized
- [ ] CPU usage reduced

## Documentation
- [ ] API documentation updated
- [ ] Examples added/updated
- [ ] User guides updated

Co-authored-by: Your Name <your.email@example.com>
```

### 5. Documentation

#### Documentation Files
- **API Reference**: Complete API documentation in `docs/API_REFERENCE.md`
- **Tutorials**: Step-by-step guides in `docs/TUTORIALS.md`
- **Best Practices**: Development guidelines in `docs/BEST_PRACTICES.md`
- **Performance Tuning**: Optimization guide in `docs/PERFORMANCE_TUNING.md`
- **Troubleshooting**: Common issues in `docs/TROUBLESHOOTING.md`

#### Example Documentation
```cpp
/**
 * @file examples/ex01_basic_brain.cpp
 * @brief Basic brain simulation example
 * @details Demonstrates fundamental NLM functionality:
 *   - Brain initialization and configuration
 *   - Basic neural simulation
 *   - State checkpointing and saving
 *   - Performance monitoring
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 * 
 * @example
 * This example shows how to create and run a basic neural simulation:
 * @code
 * #include <nlm/Brain.hpp>
 * #include <nlm/Config.hpp>
 * #include <nlm/Logger.hpp>
 * #include <iostream>
 * 
 * int main() {
 *     auto config = std::make_shared<nlm::Config>();
 *     config->set("brain.maxNeurons", 100);
 *     // ... rest of example
 * }
 * @endcode
 */
```

### 6. Integration and Release

#### Integration Testing
```bash
# Test integration with existing codebase
./integration_test_suite --test memory_integration
./integration_test_suite --test neuromodulation_integration
./integration_test_suite --test development_integration
```

#### Performance Testing
```bash
# Run performance benchmarks
./performance_benchmark --neurons 100 --neurons 500 --neurons 1000
./performance_benchmark --test spike_processing
./performance_benchmark --test plasticity_updates
```

#### Continuous Integration
```yaml
# .github/workflows/ci.yml
name: NLM CI/CD Pipeline

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build_type: [Debug, Release]
        compiler: [g++, clang++]
    
    steps:
    - uses: actions/checkout@v3
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
                 -DCMAKE_CXX_COMPILER=${{ matrix.compiler }} \
                 -DBUILD_TESTS=ON \
                 -DBUILD_EXAMPLES=ON
    - name: Build
      run: |
        cd build
        make -j$(nproc)
    - name: Test
      run: |
        cd build
        ctest --output-on-failure
    - name: Performance
      run: |
        cd build
        ./performance_benchmark --neurons 100
```

## Quality Assurance

### 1. Code Quality Standards

#### Linting and Formatting
```bash
# Run code formatting check
make format-check

# Fix formatting issues
make format

# Run linting
make lint

# Type checking
make typecheck
```

#### Coverage Requirements
- **Unit Tests**: Minimum 80% coverage for core components
- **Integration Tests**: Full integration testing for major features
- **Performance Tests**: Performance benchmarks for critical paths

### 2. Security Considerations

#### Security Best Practices
```cpp
// Use secure coding practices

// 1. Avoid buffer overflows
std::vector<float> safeBuffer(100);
std::copy(input.begin(), input.end(), safeBuffer.begin());

// 2. Validate inputs
void validateInput(const std::vector<float>& input) {
    for (float value : input) {
        assert(std::isfinite(value));
        assert(value >= -1000.0f && value <= 1000.0f);
    }
}

// 3. Use secure memory allocation
std::vector<float> secureData(1000);
std::fill(secureData.begin(), secureData.end(), 0.0f);

// 4. Log security-relevant events
if (securityEvent) {
    Logger::getInstance().logSecurity(std::string("Security event: ") + eventDetails);
}
```

## Community Guidelines

### 1. Communication

#### Effective Communication
- Be clear and concise in technical discussions
- Use appropriate technical terminology
- Provide evidence and reasoning for technical decisions
- Respect diverse perspectives and expertise

#### Code Review Etiquette
- Be constructive and specific in feedback
- Focus on code quality, not personal preferences
- Encourage diverse perspectives
- Respect time and expertise of reviewers

### 2. Collaboration

#### Open Source Contributions
- Follow project license requirements
- Respect copyright and attribution
- Be transparent about contributions
- Share knowledge and expertise

#### Documentation Standards
- Keep documentation concise but comprehensive
- Include code examples for complex concepts
- Use consistent formatting and structure
- Update documentation with code changes

## Copyright and Licensing

### Copyright Guidelines
```cpp
/*
 * Copyright 2026 NLM Contributors
 *
 * Licensed under the MIT License (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.opensource.org/licenses/mit-license.php
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
```

### License Requirements
- All contributions must be licensed under MIT
- Include copyright notice in each file
- Preserve existing license information
- Follow open source best practices

## Conclusion

Contributing to NLM offers an exciting opportunity to advance neural learning machine research. By following these contribution guidelines, you can create high-quality code that advances the project while maintaining the high standards that make NLM successful.

**Key Principles**:

1. **Code Quality**: Write clean, efficient, and maintainable code
2. **Testing**: Write comprehensive tests for all changes
3. **Documentation**: Keep documentation up-to-date and comprehensive
4. **Performance**: Optimize for performance and efficiency
5. **Community**: Collaborate with other developers and share knowledge
6. **Quality**: Ensure code meets high standards before merging

**Getting Started**:

1. **Fork the repository**: Create your own copy
2. **Create a feature branch**: Work on specific features
3. **Write tests**: Ensure your changes are thoroughly tested
4. **Document changes**: Update documentation and comments
5. **Create pull requests**: Share your changes with the community
6. **Review code**: Participate in code reviews

The NLM project welcomes contributions from developers at all experience levels. Whether you're fixing a bug, adding a new feature, or improving documentation, your contributions help advance neural learning machine research and build a better future for artificial brain systems.

Happy coding! 🚀