# NLM Contribution Guide

## Overview

Welcome to the NLM (Neural Learning Machine) project! This document provides guidelines for contributing to the NLM project, including code contributions, documentation improvements, and development practices.

## Table of Contents

1. [Getting Started](#getting-started)
2. [Code Contribution Guidelines](#code-contribution-guidelines)
3. [Documentation Contribution Guidelines](#documentation-contribution-guidelines)
4. [Testing Requirements](#testing-requirements)
5. [Code Quality Standards](#code-quality-standards)
6. [Performance Guidelines](#performance-guidelines)
7. [Version Control Policies](#version-control-policies)
8. [Community Guidelines](#community-guidelines)
9. [Pull Request Process](#pull-request-process)
10. [License and Attribution](#license-and-attribution)

## Getting Started

### Prerequisites

Before contributing to NLM, ensure you have:

1. **Development Environment**
   - C++ compiler (GCC 10+, Clang 12+, or MSVC 2019+)
   - CMake 3.16 or higher
   - Python 3.8+ (for Python bindings)
   - Git for version control

2. **Build Environment**
   ```bash
   # Clone the repository
   git clone https://github.com/your-org/nlm.git
   cd nlm
   
   
   # Create build directory and build
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

3. **Read the Documentation**
   - Review existing documentation to understand the codebase
   - Study the architecture and design patterns
   - Understand the API and conventions used

### Finding Issues to Contribute

1. **Good First Issues**
   - Look for issues labeled "good first issue" or "beginner-friendly"
   - These are typically documentation improvements or small bug fixes

2. **Help Wanted**
   - Issues marked as "help wanted" indicate areas where contributors are needed
   - Look for tasks related to documentation, examples, or testing

3. **Major Features**
   - Browse the issue tracker for larger feature requests
   - These often require more significant development effort

## Code Contribution Guidelines

### Coding Standards

#### C++ Code Style

1. **File Organization**
   ```cpp
   // Header files (.hpp)
   // Implementation files (.cpp)
   // Configuration files (.cfg)
   // Documentation files (.md)
   ```

2. **Naming Conventions**
   - Classes: `ClassName` (PascalCase)
   - Functions: `functionName()` (camelCase)
   - Variables: `variable_name` (snake_case)
   - Constants: `CONSTANT_NAME` (UPPER_SNAKE_CASE)

3. **Include Structure**
   ```cpp
   #pragma once
   #include <iostream>
   #include "Header1.hpp"
   #include "Header2.hpp"
   ```

4. **Comments**
   - Use Doxygen-style comments for public API
   - Provide clear explanations for complex logic
   - Include examples where helpful

#### Example Code Standard

```cpp
/**
 * Class Name: Brief description of the class purpose.
 * 
 * Detailed explanation of what the class does, its responsibilities,
 * and any important design considerations.
 * 
 * @note Important implementation details or caveats
 * @see RelatedMethod()
 * @param parameterName Description of parameter
 * @return Description of return value
 * @throw ExceptionType Description of when exception is thrown
 */

class ExampleClass {
public:
    // Constructor
    ExampleClass(ParameterType parameter);
    
    // Destructor
    ~ExampleClass();
    
    // Core functionality
    ReturnType doSomething(InputType input);
    
    // Helper methods
    void helperMethod();
    
private:
    // Implementation details
    ImplementationType implementationVar;
};
```

### Pull Request Workflow

1. **Fork the Repository**
   - Create a fork of the NLM repository on GitHub
   - This creates your own copy for development

2. **Create a Feature Branch**
   ```bash
   git checkout main
   git pull origin main
   git checkout -b feature/your-feature-name
   ```

3. **Development**
   - Implement your changes
   - Follow the coding standards above
   - Write tests for new functionality
   - Update documentation as needed

4. **Commit Changes**
   ```bash
   # Add a meaningful commit message
   git add modified_files
   git commit -m "Brief description of changes
   
   
   - Detailed explanation of changes
   - Related issue references
   - Testing performed
   "
   ```

5. **Push to Feature Branch**
   ```bash
   git push origin feature/your-feature-name
   ```

6. **Create Pull Request**
   - Go to your fork on GitHub
   - Click "Compare & pull request"
   - Fill out the pull request template
   - Assign reviewers if appropriate
   - Request reviews from maintainers

### Commit Message Guidelines

#### Structure
```
<type>(<scope>): <subject>

<body>

<footer>
```

#### Types
- **feat**: New feature
- **fix**: Bug fix
- **docs**: Documentation changes
- **style**: Code style changes (formatting, etc.)
- **refactor**: Code refactoring
- **test**: Test additions or modifications
- **perf**: Performance improvements
- **chore**: Maintenance tasks, build system

#### Examples
```
feat(neuromodulation): Add dopamine-based reward prediction
fix(memory): Resolve segmentation fault in episodic memory retrieval
docs(api): Document new plasticity rule interface
style(code): Fix linting issues in example scripts
```

### Branch Naming Conventions

1. **Feature branches**: `feature/<descriptive-name>`
2. **Bug fix branches**: `fix/<issue-number>-<short-description>`
3. **Documentation branches**: `docs/<topic>-updates`
4. **Test branches**: `test/<feature-name>-coverage`

## Documentation Contribution Guidelines

### Markdown Documentation

#### File Structure
```
docs/
├── API_REFERENCE.md           # API documentation
├── TUTORIAL.md                # Beginner tutorials
├── CONFIG_GUIDE.md            # Configuration documentation
├── ARCHITECTURE.md            # System architecture
├── SCIENCE.md                # Scientific background
├── HOW_TO_USE.md              # Python API usage
├── TROUBLESHOOTING.md          # Common issues and solutions
├── PERFORMANCE.md            # Performance optimization
├── README.md                 # Directory summary
├── INDEX.md                  # Navigation index
└── CONTRIBUTE.md             # Contribution guidelines
```

#### Documentation Standards

1. **Headers**: Use Markdown heading levels (# through ######)
2. **Lists**: Use consistent formatting for code and bullet points
3. **Code Examples**: Format with proper syntax highlighting
4. **Cross-references**: Use relative links between documentation files
5. **Tables**: Use Markdown table format for comparisons

#### Example Documentation Section

```markdown
## Section Title

This is a paragraph explaining the section content.

### Subsection Title

More detailed information here.

#### Code Example

```cpp
// Example code with proper formatting
int exampleFunction(int parameter) {
    return parameter * 2;
}
```

#### Lists

- **Bulleted list** item 1
- **Bulleted list** item 2
  - Nested item
  - Another nested item

- **Numbered list** item 1
- **Numbered list** item 2
```

### Example Documentation Update

```markdown
### Bug Fix Documentation

#### Problem
The neural network was not learning due to incorrect learning rate parameters.

#### Solution
Updated the default learning rates in the configuration system:

- Changed STDP learning rate from 0.001 to 0.01
- Updated Hebbian learning rate from 0.01 to 0.05
- Added validation to ensure learning rates are within reasonable bounds

#### Files Modified
- `src/plasticity/STDP.cpp`: Updated learning rate parameters
- `src/plasticity/Hebbian.cpp`: Updated learning rate parameters
- `docs/CONFIG_GUIDE.md`: Updated configuration parameter descriptions

#### Testing
- Unit tests verify learning rule implementations
- Integration tests confirm network learning behavior
- Performance tests ensure reasonable training times
```

## Testing Requirements

### Unit Testing

#### Testing Strategy
1. **Test Coverage**: Aim for 80% code coverage for new code
2. **Test Organization**: Organize tests by component/module
3. **Test Isolation**: Ensure tests don't interfere with each other
4. **Edge Cases**: Test boundary conditions and error cases

#### Example Test Structure

```cpp
#include <gtest/gtest.h>
#include "brain/Neuron.hpp"
#include "plasticity/STDP.hpp"

class NeuronTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test objects
        config = std::make_shared<Config>();
        brain = std::make_shared<Brain>(config);
        brain->initialize();
    }
    
    void TearDown() override {
        // Clean up test objects
    }
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
};

TEST_F(NeuronTest, BasicInitialization) {
    EXPECT_TRUE(brain != nullptr);
    EXPECT_EQ(brain->getTotalNeuronCount(), 1000);
}

TEST_F(NeuronTest, FiringBehavior) {
    // Inject current and test firing
    brain->injectCurrentToNeurons(NeuronType::Excitatory, 50.0f);
    brain->step(0);
    EXPECT_GT(brain->getFiringNeuronCount(), 0);
}
```

### Integration Testing

#### Integration Test Examples

1. **Full Brain Simulation**
   ```python
   import pynlm
   
   def test_full_simulation():
       # Create brain with complex configuration
       config = pynlm.createDefaultConfig()
       config.set("brain.neuron_count", 1000, pynlm.ConfigSource.File)
       config.set("plasticity.stdp.enable", True, pynlm.ConfigSource.File)
       config.set("plasticity.hebbian.enable", True, pynlm.ConfigSource.File)
       
       brain = pynlm.createBrain(config)
       brain.initialize()
       
       # Run simulation loop
       for step in range(100):
           brain.step(step)
       
       # Verify results
       assert brain.getTotalSpikeCount() > 0
       assert brain.getAverageFiringRate() > 0.0
   ```

2. **Multi-Agent Simulation**
   ```python
   import pynlm
   
   def test_multi_agent():
       # Create multiple agents
       agents = []
       for i in range(3):
           config = pynlm.createDefaultConfig()
           config.set("brain.neuron_count", 500, pynlm.ConfigSource.File)
           brain = pynlm.createBrain(config)
           brain.initialize()
           agents.append(brain)
       
       # Simulate all agents
       for agent in agents:
           for step in range(50):
               agent.step(step)
       
       # Verify all agents are functioning
       for agent in agents:
           assert agent.getTotalSpikeCount() > 0
   ```

### Documentation Testing

#### Documentation Build Tests
```bash
# Test that all markdown files can be parsed
npm install -g markdownlint-cli
markdownlint docs/*.md

# Test that all code examples compile
cd build
./nlm_test  # Run unit tests
```

## Code Quality Standards

### Static Analysis

#### Tools and Linting
1. **C++ Code Analysis**
   - Use Clang-Tidy for code analysis
   - Check for common coding errors
   - Ensure memory safety

2. **Python Analysis**
   - Use pylint or flake8 for Python code
   - Check for style violations
   - Ensure type safety

#### Example Linting Configuration

```ini
# .clang-tidy
-checks:
- -*,
- performance*
- readability*
- portability*

# .pylintrc
[MASTER]
ignore-patterns=examples/.*
disable=missing-docstring,invalid-name
```

### Code Review Standards

#### Pull Request Checklist
- [ ] Code follows project coding standards
- [ ] Tests pass (unit, integration, documentation)
- [ ] Documentation updated
- [ ] No linting errors
- [ ] Performance impact assessed
- [ ] Dependencies updated (if any)
- [ ] Compatibility maintained

#### Review Process
1. **Initial Review**: Check for basic quality and adherence to standards
2. **Technical Review**: Verify technical correctness and design
3. **Performance Review**: Ensure performance is not degraded
4. **Documentation Review**: Verify documentation is complete and accurate

## Performance Guidelines

### Performance Considerations

1. **Memory Management**
   - Use smart pointers (shared_ptr, unique_ptr)
   - Avoid raw dynamic memory allocation
   - Implement custom deleters when needed

2. **Algorithm Efficiency**
   - Prefer O(1) and O(log n) operations
   - Avoid unnecessary computations
   - Use appropriate data structures

3. **Parallel Processing**
   - Consider parallel algorithms for large networks
   - Use threading where appropriate
   - Balance between performance and complexity

### Performance Testing

#### Benchmark Requirements
```bash
# Performance benchmark script
#!/bin/bash

# Configuration for testing
NEURON_COUNTS=(100 500 1000 5000)
TIMESTEPS=(0.001 0.01)

# Run benchmarks
for neurons in ${NEURON_COUNTS[@]}; do
    for timestep in ${TIMESTEPS[@]}; do
        echo "Testing: $neurons neurons, $timestep timestep"
        time ./nlm_test --neuron_count=$neurons --timestep=$timestep
    done
done
```

#### Performance Monitoring
```cpp
// Performance monitoring example
class PerformanceMonitor {
public:
    void startTiming() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    double stopTiming() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        return duration.count() / 1000000.0;
    }
    
    void logPerformance(const std::string& description, double time) {
        std::cout << "Performance: " << description << " - " << time << " seconds" << std::endl;
    }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
};
```

## Version Control Policies

### Git Workflow

1. **Branch Management**
   - Never push directly to main branch
   - Use feature branches for all development
   - Use release branches for version releases

2. **Commit Strategy**
   - Commit early and often
   - Write meaningful commit messages
   - Use feature flags for incomplete features

3. **Merge Strategy**
   - Use pull requests for all code merges
   - Require at least one approval
   - Use squash merges for small, focused changes

### Tagging Convention

```bash
# Major releases
semver major  # e.g., 1.0.0

# Minor features
semver minor  # e.g., 0.1.0

# Bug fixes and patches
semver patch  # e.g., 0.0.1

# Pre-releases
semver prerelease --preid rc  # e.g., 1.0.0-rc.1
```

## Community Guidelines

### Code of Conduct

1. **Respectful Communication**
   - Treat all contributors with respect
   - Use professional and constructive language
   - Focus on technical issues, not personal characteristics

2. **Open Collaboration**
   - Welcome contributions from all backgrounds
   - Provide help to new contributors
   - Share credit for collaborative work

3. **Diversity and Inclusion**
   - Encourage diverse perspectives
   - Support contributors from different backgrounds
   - Create inclusive development environment

### Contributor Guidelines

1. **Time Investment**
   - Understand the time commitment required
   - Set realistic expectations for contribution timeline
   - Be patient with the review process

2. **Communication**
   - Keep contributors informed of progress
   - Respond to reviews and requests promptly
   - Be open to feedback and suggestions

3. **Quality Assurance**
   - Ensure all code meets quality standards
   - Write comprehensive tests
   - Update documentation as needed

## Pull Request Process

### Pre-Submission Checklist

1. **Code Quality**
   - [ ] Code follows project coding standards
   - [ ] All linting checks pass
   - [ ] No syntax or compilation errors
   - [ ] Performance impact assessed

2. **Testing**
   - [ ] Unit tests pass
   - [ ] Integration tests pass
   - [ ] Performance tests pass (if applicable)
   - [ ] Documentation builds correctly

3. **Documentation**
   - [ ] API documentation updated
   - [ ] User documentation updated
   - [ ] Comments in code updated
   - [ ] Examples updated if necessary

4. **Compatibility**
   - [ ] Backwards compatibility maintained
   - [ ] Dependencies updated appropriately
   - [ ] Platform compatibility verified

### Review Process

1. **Initial Review** (Repository Maintainer)
   - Check for basic quality issues
   - Verify coding standards compliance
   - Ensure tests pass

2. **Technical Review** (Subject Matter Expert)
   - Verify technical correctness
   - Assess design choices
   - Review performance implications

3. **Documentation Review** (Technical Writer)
   - Check documentation completeness
   - Verify code examples are correct
   - Ensure user experience is good

4. **Final Review** (Repository Maintainer)
   - Final quality check
   - Merge the pull request
   - Update repository state

## License and Attribution

### Project License

NLM is licensed under the Apache License 2.0. See LICENSE file for details.

### Attribution Requirements

When using or referencing NLM in publications or other projects:

1. **Academic Citations**
   ```bibtexn
   @misc{NLM2026,
       title={NLM: Neural Learning Machine},
       author={NLM Contributors},
       year={2026},
       url={https://github.com/your-org/nlm}
   }
   ```

2. **Software Citation**
   ```
   If you use NLM in your research or commercial product, please cite:
   
   NLM Team (2026). NLM: Neural Learning Machine. https://github.com/your-org/nlm
   ```

3. **Acknowledgement**
   - Include acknowledgment in documentation
   - Mention specific contributions
   - Reference the version used

### Third-Party Credits

NLM includes or depends on several third-party libraries:

- **pybind11**: Python bindings for C++
- **scikit-build-core**: Build system for Python packages
- **GoogleTest**: Unit testing framework
- **Other libraries**: As specified in the project configuration

### License Compliance

1. **License Headers**
   ```cpp
   // Copyright (c) 2026, NLM Contributors
   // All rights reserved.
   // This file is part of the NLM project.
   // For license information, see LICENSE file.
   ```

2. **License Information**
   - Include license text in source files if required
   - Reference license in documentation
   - Maintain license compliance in all distributions

## Conclusion

Contributing to NLM is an opportunity to advance brain-inspired computing research and development. By following these guidelines, contributors can ensure high-quality code, comprehensive documentation, and successful integration into the NLM ecosystem.

The NLM project welcomes contributions from all interested individuals and organizations. Whether you're fixing a bug, adding a feature, or improving documentation, your contributions help advance the field of neural computation and artificial intelligence.

Happy contributing! 🚀
