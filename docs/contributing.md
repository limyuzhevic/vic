# NLM Contributing Guide

## Overview

This guide provides comprehensive instructions for contributing to the Neural Learning Machine (NLM) project. It covers the contribution process, coding standards, testing procedures, and best practices for effective collaboration.

## Welcome to NLM

Thank you for your interest in contributing to NLM! Your contributions help advance artificial brain research and make NLM more powerful and accessible to researchers and developers worldwide.

## How to Contribute

### 1. Getting Started

#### Step 1: Fork the Repository

```bash
# Fork the NLM repository on GitHub
# This creates your own copy of the repository

# Clone your forked repository
git clone https://github.com/your-username/nlm.git

cd nlm

# Add the original repository as an upstream source
git remote add upstream https://github.com/nlm-project/nlm.git
```

#### Step 2: Set Up Your Development Environment

```bash
# Create and activate a development environment
python -m venv nlm-dev
source nlm-dev/bin/activate

# Install NLM in development mode
pip install -e .

# Install development dependencies
pip install -e .[dev]
```

#### Step 3: Configure Your Environment

```bash
# Configure git for your contributions
git config user.name "Your Name"
git config user.email "your.email@example.com"

# Configure git to track upstream changes
git remote add upstream https://github.com/nlm-project/nlm.git
```

### 2. Finding Issues to Work On

#### Step 1: Browse Open Issues

- Visit the [GitHub Issues page](https://github.com/nlm-project/nlm/issues)
- Look for issues labeled:
  - `good first issue` - Great for beginners
  - `help wanted` - We need help with specific tasks
  - `documentation` - We need better documentation
  - `bug` - We need bug fixes
  - `enhancement` - We need new features

#### Step 2: Claim an Issue

```bash
# If you want to work on an issue, comment on it to claim it
# Example: "I can work on this issue. Let me know if there are any questions."
```

### 3. Working on Your Contribution

#### Step 1: Create a Feature Branch

```bash
# Create and switch to a new branch
git checkout -b feature/your-feature-name
```

#### Step 2: Make Your Changes

```bash
# Make your changes to the codebase
# Follow NLM coding standards
# Write tests for your changes
# Update documentation if needed

# Stage your changes
git add .

# Create a descriptive commit message
git commit -m "Add new [component/feature] to NLM

- Brief description of the changes
- Added [X] new [Y]
- Fixed [Z] issue
- Updated documentation

Co-authored-by: Your Name <your.email@example.com>"
```

#### Step 3: Push to Your Fork

```bash
# Push your branch to your forked repository
git push origin feature/your-feature-name
```

### 4. Creating a Pull Request

#### Step 1: Open a Pull Request

```bash
# Go to your forked repository on GitHub
# Click the "Compare & pull request" button on your feature branch

# Fill in the pull request template
# Provide a clear title and description
# Include screenshots or demonstrations if applicable
```

#### Step 2: Fill in the PR Template

**Title Template:**
```
[Feature/Component]: Brief description of changes
```

**Description Template:**
```
## Summary
Brief description of the changes and their purpose.

## What Changed
- [List the main changes]
- [Describe the implementation approach]

## Testing
- [Describe how changes were tested]
- [Mention any new test cases]

## Impact
- [Explain how this affects NLM functionality]
- [Describe any performance implications]

## Related Issues
- Fixes #123
- Closes #456
- Depends on #789

## Co-authored-by
Your Name <your.email@example.com>
```

#### Step 3: Wait for Review

- Reviewers will provide feedback on your changes
- Address any comments or requested changes
- Make additional commits if needed

#### Step 4: Merge

- Your changes will be merged by the maintainers
- The branch will be deleted automatically (unless you request otherwise)

## Contribution Types

### 1. Code Contributions

#### Types of Code Contributions:

- **Bug Fixes**: Fix existing bugs or issues
- **New Features**: Implement new functionality
- **Documentation**: Improve documentation and guides
- **Tests**: Add or improve test coverage
- **Performance**: Optimize existing code

#### Example Contribution Workflow:

```bash
# 1. Find a bug or feature request in the issues
# 2. Fork the repository
git clone https://github.com/your-username/nlm.git
cd nlm

# 3. Create a branch for your fix
feature/fix-null-pointer-bug

# 4. Make the necessary changes
# Edit the problematic code in src/brain/Brain.cpp

# 5. Test your fix
./run_tests.sh

# 6. Commit and push your changes
git add src/brain/Brain.cpp
git commit -m "Fix null pointer dereference in Brain::getWorkingMemory()

- Added null pointer check before dereferencing working memory
- Added appropriate error handling
- Updated documentation

Co-authored-by: Your Name <your.email@example.com>"

git push origin feature/fix-null-pointer-bug

# 7. Create a pull request
# 8. Wait for review and merge
```

### 2. Documentation Contributions

#### Improving Documentation:

- Fix typos and grammatical errors
- Add missing sections
- Update outdated information
- Create new guides or tutorials
- Add code examples and demonstrations

#### Example Documentation Contribution:

```bash
# 1. Find documentation issues
# 2. Fork the repository
cd nlm

# 3. Create a branch for documentation improvements
docs/fix-typos-in-how-to-use

# 4. Make documentation changes
# Edit docs/HOW_TO_USE.md to fix typos and add missing information

# 5. Verify changes
# 6. Commit and push your changes
git add docs/HOW_TO_USE.md
git commit -m "Fix typos and improve documentation in HOW_TO_USE.md

- Fixed grammatical errors in sections 2.1 and 3.2
- Added example for neural plasticity
- Improved readability of technical explanations

Co-authored-by: Your Name <your.email@example.com>"

git push origin docs/fix-typos-in-how-to-use

# 7. Create a pull request
```

### 3. Research and Experiment Contributions

#### Contributing Research:

- Submit research papers about NLM improvements
- Share experimental results and findings
- Contribute new experimental frameworks
- Suggest research directions

#### Contributing Experiments:

- Add new test cases to the test suite
- Create benchmark comparisons
- Develop new experimental protocols
- Share experimental data

## Code Standards

### 1. C++ Code Standards

#### File Structure:

```cpp
// File: src/component/Component.hpp
#pragma once
#include "../core/Config.hpp"
#include "../utils/Logger.hpp"

namespace nlm {

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
public:
    Component(const Config& config);
    ~Component();
    
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

#### Code Style:

- **Naming**: Use camelCase for variables and functions, PascalCase for classes
- **Comments**: Use Doxygen-style comments for API documentation
- **Formatting**: Follow Google's C++ style guide
- **Whitespace**: Use spaces, not tabs (4 spaces per indentation level)

### 2. Python Code Standards

#### Code Style:

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
    
    def simulate(self, steps: int) -> Dict[str, typing.Any]:
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
```

#### Documentation Style:

```python
"""Module docstring with brief description.

Detailed module description explaining purpose and functionality.

Attributes:
    ATTRIBUTE_NAME: Description of attribute

Functions:
    function_name(): Brief description of function

Raises:
    ExceptionType: When and why exception is raised

See Also:
    RelatedClass, related_function
"""
def function_name(parameter: type) -> return_type:
    """Brief function description.
    
    Args:
        parameter: Description of the parameter
        
    Returns:
        Description of return value
        
    Raises:
        ValueError: When parameter values are invalid
        RuntimeError: When operation cannot be completed
        
    See Also:
        Related function or class
    """
    # Implementation
    pass
```

## Testing Standards

### 1. Unit Testing

#### Running Tests:

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific test suite
./tests/run_unit_tests.sh

# Run tests with coverage reporting
cd build
./run_tests_with_coverage.sh
```

#### Test Structure:

```cpp
// File: tests/test_neural_system.cpp
#include <gtest/gtest.h>
#include "../src/brain/NeuralSystem.hpp"

namespace nlm {
namespace test {

class NeuralSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_ = createDefaultConfig();
        system_ = std::make_unique<NeuralSystem>(config_);
    }
    
    void TearDown() override {
        system_.reset();
    }
    
    NeuralSystem createTestSystem() {
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
    system_->step(0);
    // ... assertions ...
}

} // namespace test
} // namespace nlm
```

### 2. Integration Testing

#### Integration Test Structure:

```bash
#!/bin/bash
# tests/run_integration_tests.sh

set -e

# Run integration tests
./nlm_phase3_demo 5 200
./nlm_phase4_demo --test-mode
./nlm_phase6_demo --integration-test

# Verify expected outcomes
# Check for successful brain development
# Validate memory system integration
# Confirm cognitive function emergence
```

### 3. Performance Testing

#### Benchmark Tests:

```bash
#!/bin/bash
# scripts/benchmark_nlm.sh

set -e

# Run performance benchmarks
./benchmark_brain --iterations 100
python3 tests/benchmark_python.py --iterations 50

# Generate reports
./scripts/generate_benchmark_report.py
```

## Documentation Standards

### 1. Markdown Documentation

#### Documentation Structure:

```markdown
# Document Title

## Overview
Brief introduction to the topic.

## Section 1: Topic Name
Detailed explanation of section content.

### Subsection 1.1: Specific Topic
More detailed explanation with code examples:

```code
# Code example
def function_example():
    return "example"
```

## Best Practices
- List of best practices

## Common Pitfalls
- Common mistakes to avoid

## See Also
- Related documentation
```

### 2. API Documentation

#### API Reference Structure:

```markdown
# API Reference

## Python API
### pynlm module
#### Classes
- `pynlm.Brain`: Represents the neural network
- `pynlm.Config`: Configuration management
- `pynlm.World`: Environment simulation

#### Functions
- `pynlm.createBrain(config)`: Creates a neural brain
- `pynlm.createWorld(brain)`: Creates simulation environment
- `pynlm.createDefaultConfig()`: Creates default configuration

## C++ API
### Namespace: nlm
#### Classes
- `Brain`: Central neural network controller
- `Config`: Configuration management
- `NeuralPopulation`: Group of neurons

#### Functions
- `void Brain::initialize()`: Initializes the brain
- `void Brain::step(int time_step)`: Advances simulation
- `int Brain::getSpikeCount()`: Gets spike count
```

## Community Guidelines

### 1. Code of Conduct

NLM follows the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md).

#### Expected Behavior:

- Be respectful and professional
- Focus on facts and technical details
- Help others when possible
- Be constructive with feedback

#### Unacceptable Behavior:

- Harassment or discrimination
- Personal attacks
- Trolling or inflammatory comments
- Public or private harassment

### 2. Communication Guidelines

#### Effective Communication:

- Be clear and concise
- Provide context for your changes
- Use technical terminology appropriately
- Ask questions when unsure

#### Collaboration Tips:

- Review code before merging
- Provide constructive feedback
- Document decisions and changes
- Respect others' time and expertise

## Repository Structure

### 1. Project Root

```bash
NLM/
├── CMakeLists.txt              # Build configuration
├── README.md                   # Project overview
├── LICENSE                     # License information
├── CONTRIBUTING.md             # Contributing guidelines
├── docs/                       # Documentation
│   ├── ARCHITECTURE.md         # Architecture documentation
│   ├── SCIENCE.md             # Scientific background
│   ├── ROADMAP.md             # Project roadmap
│   ├── EXPERIMENTS.md          # Experimental framework
│   ├── LIMITATIONS.md          # Known limitations
│   └── PHASE6_FINAL_AUDIT.md   # Phase 6 audit
├── src/                        # Source code
│   ├── brain/                 # Brain components
│   ├── memory/                # Memory systems
│   ├── agent/                 # Agent integration
│   └── ...                    # Other components
├── python/                     # Python bindings
├── tests/                      # Test suite
├── examples/                   # Example projects
└── .github/                    # GitHub workflows
```

### 2. GitHub Workflows

#### CI/CD Configuration:

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
        run: make lint
  
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
```

### 3. Project Files

#### Key Files:

- **CMakeLists.txt**: Build configuration
- **README.md**: Project overview and quick start
- **LICENSE**: License information
- **CONTRIBUTING.md**: This file
- **CODE_OF_CONDUCT.md**: Code of conduct

## Support and Getting Help

### 1. Documentation

#### Available Resources:

- **Documentation Hub**: [docs/index.md](#)
- **User Guides**: Quick start, Python guide, C++ guide
- **Tutorials**: Step-by-step learning materials
- **Examples**: Working code examples
- **API Reference**: Complete API documentation

### 2. Community Support

#### Online Resources:

- **GitHub Issues**: [Report bugs or request features](https://github.com/nlm-project/nlm/issues)
- **Discussions**: [Community discussions](https://github.com/nlm-project/nlm/discussions)
- **Slack/Discord**: Join the NLM community chat
- **Email**: support@nlm-project.org

#### Support Process:

```bash
# 1. Check existing documentation first
# 2. Search for similar issues in GitHub
# 3. Create a new issue with:
# - Clear title describing the problem
# - Detailed reproduction steps
# - Expected vs actual behavior
# 4. Wait for community response
# 5. Iterate on the solution if needed
```

### 3. Reporting Issues

#### Issue Report Template:

```markdown
## Bug Report

### Description
Brief description of the bug.

### Steps to Reproduce
1. Step 1
2. Step 2
3. Step 3

### Expected Behavior
What should happen.

### Actual Behavior
What actually happens.

### Environment
- Operating System: [e.g., Ubuntu 20.04]
- NLM Version: [e.g., 1.0.0]
- Python Version: [e.g., 3.8.5]
- C++ Compiler: [e.g., gcc 9.3.0]

### Reproduction Script
```bash
# Provide a script that reproduces the issue
./nlm_phase3_demo 5 200
```

### Screenshots (if applicable)

### Additional Information
- Related issues
- Performance impact
- Testing details
```

## Acknowledgments

### Contributors
Thank you to all contributors who have helped make NLM a reality.

### Sponsors
Special thanks to our sponsors who support NLM development.

### Research Community
We acknowledge the research community for inspiring NLM's development.

## Conclusion

Contributing to NLM is an rewarding experience that helps advance artificial brain research. Whether you're fixing bugs, adding new features, improving documentation, or helping others, your contributions make NLM better for everyone.

We look forward to your contributions and working together to create the next generation of artificial brains!
