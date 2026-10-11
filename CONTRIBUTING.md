# NLM Contributing Guide

This document provides guidelines for contributing to the Neural Learning Machine (NLM) project. It covers coding standards, development practices, and the contribution process.

## Introduction

Thank you for your interest in contributing to NLM! This guide outlines the contribution process and helps you get your work done efficiently and effectively.

## How to Contribute

### 1. Report Issues

If you encounter bugs or have feature requests:

1. **Check Existing Issues:** Search for similar issues in the [GitHub Issues](https://github.com/nlm/nlm/issues)
2. **Create a New Issue:** Provide clear steps to reproduce, expected behavior, and actual behavior
3. **Include Details:** Include relevant screenshots, error messages, and stack traces
4. **Label Appropriately:** Use relevant labels (bug, feature, enhancement, etc.)

### 2. Contribute Code

#### Step 1: Fork the Repository

1. Create a GitHub account
2. Fork the [NLM repository](https://github.com/nlm/nlm) to your account
3. Clone your fork:

```bash
git clone https://github.com/your-username/nlm.git
 cd nlm
```

#### Step 2: Set Up Development Environment

1. **Build the project** using the unified build system:

```bash
python3 build.py --components dev
```

2. **Run tests** to ensure everything works:

```bash
ctest --output-on-failure
```

3. **Install development tools** if needed:

```bash
# Install clang-format for code formatting
sudo apt-get install clang-format

# Install clang-tidy for static analysis
sudo apt-get install clang-tidy
```

#### Step 3: Create a Feature Branch

```bash
git checkout -b feature/your-feature-name
```

#### Step 4: Make Changes

1. **Follow coding standards** (see below)
2. **Write tests** for new functionality
3. **Update documentation** as needed
4. **Commit changes** with appropriate commit messages

#### Step 5: Submit Changes

1. **Push to your fork:**

```bash
git push origin feature/your-feature-name
```

2. **Create a Pull Request:** Go to your fork on GitHub and click "Compare & pull request"
3. **Fill out PR template:** Provide detailed description of changes
4. **Request reviews:** Tag maintainers or experienced contributors

### 3. Contribute Documentation

#### Step 1: Review Existing Documentation

- Check `README.md`, `INSTALLATION.md`, `CONTRIBUTING.md`, `BUILD.md`
- Look for areas that need improvement or updates

#### Step 2: Write Documentation

1. **Update existing files** or create new ones as needed
2. **Follow Markdown formatting** with proper headers and code blocks
3. **Include examples** where appropriate
4. **Use consistent style** with existing documentation

## Coding Standards

### Code Style

#### C++ Code

- **File Headers:** Include file headers with copyright, author, date, and brief description
- **Indentation:** 4 spaces (no tabs)
- **Line Length:** Maximum 120 characters
- **Naming Conventions:** Use camelCase for variables and functions, UPPER_CASE for constants
- **Comments:** Explain "why" not "what" - code should be self-documenting
- **Whitespace:** One blank line between logical sections

#### Python Code

- **File Headers:** Include shebang and module docstring
- **Indentation:** 4 spaces (PEP 8)
- **Line Length:** Maximum 79 characters
- **Naming Conventions:** snake_case for variables and functions
- **Comments:** Use docstrings for functions and classes
- **Whitespace:** One blank line between logical sections

#### CMake Code

- **File Headers:** Include project description and copyright
- **Indentation:** 2 spaces
- **Line Length:** No limit
- **Variable Naming:** UPPER_CASE for variables
- **Comments:** Use # for comments
- **Best Practices:** Follow CMake best practices

### Documentation Standards

#### Code Comments

- **Doxygen-style** for C++:

```cpp
/**
 * @brief Brief description of function
 * @param param1 Description of parameter 1
 * @param param2 Description of parameter 2
 * @return Description of return value
 * @note Additional notes
 */
void exampleFunction(int param1, const std::string& param2)
```

- **Python docstrings:** Use Google or NumPy style:

```python
"""
Brief description of function.

Args:
    param1: Description of parameter 1.
    param2: Description of parameter 2.

Returns:
    Description of return value.

Raises:
    ExceptionType: Description of when this exception is raised.
"""
def example_function(param1: int, param2: str) -> str:
    pass
```

#### Markdown Documentation

- **Headers:** Use proper heading hierarchy (#, ##, ###)
- **Lists:** Use bullet points for lists
- **Code Blocks:** Use triple backticks with language specifier
- **Tables:** Use proper table syntax
- **Links:** Use relative paths for internal links, absolute for external

### Testing Standards

#### Unit Tests

- **Test Structure:** One test per scenario
- **Test Naming:** `test_scenario_behavior`
- **Test Framework:** Google Test (if used) or custom test framework
- **Test Coverage:** Aim for high test coverage
- **Test Isolation:** Tests should not depend on each other

#### Integration Tests

- **Test Scenarios:** Test real-world scenarios
- **Test Data:** Use realistic test data
- **Test Environment:** Use isolated test environments
- **Test Validation:** Validate outputs against expected results

#### Performance Tests

- **Benchmarking:** Include performance benchmarks
- **Profiling:** Use profiling tools when appropriate
- **Regression Testing:** Ensure performance doesn't degrade

### Build and Packaging Standards

#### CMake

- **Modern Practices:** Use CMake 3.16+ features
- **Package Management:** Use Conan or vcpkg for dependencies
- **Installation:** Use GNUInstallDirs for installation paths
- **Testing:** Use ctest for test execution
- **Packaging:** Use CPack for package generation

#### Python

- **Python 3:** Use Python 3.6+
- **Virtual Environments:** Use virtual environments for development
- **Dependencies:** Use requirements.txt or pyproject.toml
- **Packaging:** Use setuptools or poetry

#### Shell Scripts

- **Shebang:** Include `#!/bin/bash` at the top
- **Error Handling:** Use `set -euo pipefail`
- **Path Handling:** Use absolute paths when possible
- **Logging:** Use informative log messages

## Project Structure

### Source Code

```
/src/
  /core/              # Core neural network functionality
  /brain/            # Brain simulation
  /agent/            # Agent system
  /world/            # Environment simulation
  /cognition/        # Cognitive functions
  /neuromodulation/  # Neuromodulation
  /plasticity/       # Plasticity rules
  /memory/           # Memory system
  /sensory/          # Sensory processing
  /prediction/       # Prediction system
  /motor/            # Motor system
  /environment/      # Environment
  /experiments/      # Experiment system
  /visualization/    # Visualization
  /cli/              # Command-line interface
  /plugins/          # Plugin system
/include/            # Header files
/tests/               # Test files
python/              # Python bindings
examples/            # Examples
```

### Documentation

```
docs/                # Generated documentation
README.md            # Project README
README-build.md      # Build documentation
INSTALLATION.md      # Installation guide
CONTRIBUTING.md      # Contributing guide
BUILD.md             # Build troubleshooting
```

### Build System

```
build.py              # Python build automation
install.sh            # Shell installation script
CMakeLists.txt        # Main CMake configuration
CMakePresets.json      # Build presets
CPackConfig.cmake     # Package configuration
conanfile.py          # Conan package configuration
vcpkg.json            # vcpkg configuration
```

## Pull Request Process

### 1. Prepare Your Changes

- **Rebase:** Rebase your branch on the latest main/develop
- **Resolve conflicts:** Ensure there are no merge conflicts
- **Run tests:** Ensure all tests pass
- **Code review:** Review your changes for quality

### 2. Create Pull Request

- **Title:** Use descriptive title with issue reference if applicable
- **Description:** Provide detailed description of changes
- **Checklist:** Check the checklist in PR template
- **Assignees:** Assign to appropriate maintainers
- **Labels:** Add relevant labels

### 3. Review Process

- **Team review:** Team members review and provide feedback
- **Code approval:** Ensure code meets quality standards
- **Testing:** Ensure tests are updated and passing
- **Documentation:** Ensure documentation is updated
- **Final review:** Final review before merging

### 4. Merge Process

- **Merge strategy:** Use appropriate merge strategy (squash, rebase, or merge)
- **Branch protection:** Follow branch protection rules
- **Changelog:** Update changelog if needed
- **Release notes:** Update release notes if needed

## Code Review Guidelines

### Pull Request Checklist

```markdown
- [ ] Code follows project coding standards
- [ ] Tests pass (unit, integration, performance)
- [ ] Documentation is updated
- [ ] Code is well-documented
- [ ] No linting errors
- [ ] No type errors
- [ ] Performance is not degraded
- [ ] Security considerations addressed
- [ ] Platform compatibility verified
- [ ] Dependencies are up to date
```

### Review Process

1. **Initial Review:** Check if the PR addresses the issue
2. **Code Quality:** Review code quality and standards
3. **Testing:** Verify tests pass and are comprehensive
4. **Documentation:** Check documentation is complete
5. **Performance:** Review performance impact
6. **Security:** Check for security vulnerabilities
7. **Integration:** Verify integration with existing code
8. **Final Approval:** Get final approval before merging

## Branching Strategy

### Branch Types

| Branch | Purpose | Naming |
|--------|---------|---------|
| main | Production-ready code | N/A |
| develop | Integration branch | N/A |
| feature/* | New features | feature/name |
| bugfix/* | Bug fixes | bugfix/name |
| docs/* | Documentation updates | docs/name |
| perf/* | Performance improvements | perf/name |

### Branch Workflow

1. **Create feature branch:** Based on `develop`
2. **Develop and test:** Implement features and tests
3. **Merge to develop:** Create pull request to `develop`
4. **Code review:** Team reviews and approves
5. **Merge:** Merge to `develop`
6. **Release preparation:** Prepare release from `develop`

## Quality Assurance

### Static Analysis

#### Linting

- **C++:** Use clang-format and clang-tidy
- **Python:** Use black, flake8, mypy
- **CMake:** Use cmake-format

#### Type Checking

- **C++:** Use type annotations where possible
- **Python:** Use mypy for type checking

#### Security Scanning

- **Static Analysis:** Use tools like clang-analyzer, semgrep
- **Dependency Scanning:** Use tools like snyk, dependabot

### Testing

#### Test Coverage

- **Unit Tests:** Aim for >80% coverage
- **Integration Tests:** Test integration scenarios
- **Performance Tests:** Benchmark critical paths
- **Regression Tests:** Prevent regressions

#### Test Automation

- **CI/CD:** Use GitHub Actions or GitLab CI
- **Test Reporting:** Generate test reports
- **Test Flakes:** Handle flaky tests

## Development Environment

### Local Setup

#### Linux (Ubuntu/Debian)

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y \
    cmake \
    ninja-build \
    g++ \
    python3 \
    python3-pip \
    git \
    clang-format \
    clang-tidy
```

#### macOS

```bash
# Install Homebrew if not present
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake ninja python3 git clang-format clang-tidy
```

#### Windows

```bash
# Use WSL (Windows Subsystem for Linux)
# Or use Visual Studio with C++ workload
# Or use Git Bash
```

### Virtual Environment

```bash
# Create virtual environment
python3 -m venv venv
source venv/bin/activate

# Install development dependencies
pip install -r requirements.txt
pip install -e .
```

## Development Workflow

### Daily Workflow

1. **Start of day:** Check for new issues and PRs
2. **Morning:** Work on assigned tasks
3. **Afternoon:** Code reviews and testing
4. **End of day:** Commit changes and update status

### Weekly Workflow

1. **Code review meeting:** Review PR submissions
2. **Technical debt review:** Address technical debt
3. **Plan next sprint:** Plan upcoming work

### Monthly Workflow

1. **Release planning:** Plan release features and fixes
2. **Documentation review:** Review documentation
3. **Performance review:** Review performance improvements

## Community Guidelines

### Communication

- **Respectful:** Be respectful and inclusive
- **Collaborative:** Work together to solve problems
- **Transparent:** Keep communication open and transparent
- **Supportive:** Help new contributors get started

### Code of Conduct

See `CODE_OF_CONDUCT.md` for the project's code of conduct.

## Legal and License

### License

NLM is licensed under the MIT License. See `LICENSE` file for details.

### Copyright

Copyright (c) 2026 NLM Team

### Attribution

When using NLM, please cite:

```
@article{nlm2026,
  title={Neural Learning Machine (NLM)},
  author={NLM Team},
  year={2026},
  url={https://github.com/nlm/nlm}
}
```

## Support

### Getting Help

- **GitHub Issues:** Submit issues at https://github.com/nlm/nlm/issues
- **Community Forum:** Discuss at https://community.nlm.org
- **Discord:** Join https://discord.nlm.org
- **Email:** Contact team@nlm.org for business inquiries

### Reporting Bugs

1. **Reproduce the bug:** Provide clear steps to reproduce
2. **Include relevant information:** Error messages, stack traces, system info
3. **Test first:** Check if the bug exists in the latest version
4. **Submit issue:** Create a new issue with all relevant information

### Requesting Features

1. **Check existing features:** Search for similar feature requests
2. **Provide rationale:** Explain why the feature is needed
3. **Provide examples:** Include examples of desired functionality
4. **Discuss implementation:** Discuss implementation approach

## Acknowledgments

### Contributors

Thank you to all contributors who have helped improve NLM:

- [Contributor 1](https://github.com/contributor1)
- [Contributor 2](https://github.com/contributor2)
- [Contributor 3](https://github.com/contributor3)

### Sponsors

Special thanks to our sponsors:

- [Sponsor 1](https://sponsor1.com)
- [Sponsor 2](https://sponsor2.com)

### Libraries and Tools

NLM uses many open-source libraries and tools:

- [nlohmann/json](https://github.com/nlohmann/json)
- [yaml-cpp](https://github.com/yaml-cpp/yaml-cpp)
- [pybind11](https://github.com/pybind/pybind11)
- [CMake](https://cmake.org)
- [Ninja](https://ninja-build.org)
- And many more...

## Conclusion

Contributing to NLM is a rewarding experience. Whether you're fixing a bug, adding a feature, or improving documentation, your contributions make NLM better for everyone.

We welcome all contributions, regardless of experience level. If you have questions or need help getting started, don't hesitate to reach out.

Thank you for your interest in contributing to NLM!

---

*Last updated: $(date -u +%Y-%m-%d)*
