# NLM Contributing Guide

## Overview

Welcome to NLM's open source development community! This document provides guidelines for contributing to the project.

## Getting Started

### Prerequisites

Before you contribute, ensure you have:

- **C++20 compiler** (GCC 10+, Clang 12+, or MSVC 2019+)
- **CMake 3.16+**
- **Python 3.8+**
- **Git** 2.30+
- **Build tools**: `make`, `cmake`, `pip`

### Installing from Source

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd nlm
   ```

2. Build the project:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

3. Build Python bindings:
   ```bash
   pip install -e .
   ```

4. Run tests:
   ```bash
   ctest --output-on-failure
   ./nlm_test
   ```

## Development Workflow

### 1. Branching Strategy

We use GitHub Flow:

- **Main branch**: Always contains the latest production-ready code
- **Feature branches**: Create from `main` for each new feature or major refactor
- **Hotfix branches**: Create from `main` for urgent bug fixes

### 2. Creating a Pull Request

1. **Create a feature branch** from `main`:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes** (ensure they follow the code style)

3. **Run tests** before creating a PR:
   ```bash
   ./nlm_test
   ctest --output-on-failure
   ```

4. **Commit your changes** with a descriptive message:
   ```bash
   git add -A
   git commit -m "feature: add support for X in Y"
   ```

5. **Push your branch**:
   ```bash
   git push origin feature/your-feature-name
   ```

6. **Create a pull request** on GitHub

### 3. Code Review Process

Your PR will be reviewed by maintainers who will check:

- ✅ Code style and formatting
- ✅ Comprehensive test coverage
- ✅ No breaking changes
- ✅ Documentation updates
- ✅ Performance implications

### 4. Merging

After review and approval:

1. All CI checks must pass
2. At least one maintainer must approve
3. Changes are merged into `main`
4. A new release may be created

## Code Style Guidelines

### C++ Style

- **Language**: C++20
- **Indentation**: 2 spaces (no tabs)
- **Line length**: Maximum 100 characters
- **Braces**: K&R style (opening brace on same line)
- **Comments**: Use `///` for documentation comments
- **Naming**: snake_case for variables and functions

### Python Style

- **Language**: Python 3.8+
- **Indentation**: 4 spaces
- **Line length**: Maximum 79 characters
- **Naming**: snake_case for functions and variables

### Commit Messages

- Start with a short description (<= 50 chars)
- Use imperative mood: "Add feature", "Fix bug"
- Reference issues: "Fix #123"
- Separate summary from details with a blank line

## Testing

### Running Tests

1. **Unit tests**:
   ```bash
   ./nlm_test
   ```

2. **Integration tests**:
   ```bash
   ctest --output-on-failure
   ```

3. **Phase-specific tests**:
   ```bash
   ./nlm_phase3_demo  # --help for options
   ./nlm_phase4_demo
   ./nlm_phase6_demo
   ```

### Adding New Tests

1. **Location**: `tests/` directory
2. **Format**: Follow existing test patterns
3. **Coverage**: Ensure new functionality is tested
4. **Performance**: Tests should complete in reasonable time

## Bug Reports

If you encounter a bug:

1. **Search existing issues**: Look for similar reports
2. **Reproduce the issue**: Include minimal steps
3. **Log information**: Collect logs, config files, and outputs
4. **Report format**:
   - Clear title
   - Detailed description
   - Steps to reproduce
   - Expected vs actual behavior
   - Environment information (version, OS, hardware)
   - Log files (if applicable)

## Feature Requests

For feature requests:

1. **Describe the use case** clearly
2. **Provide examples** of desired behavior
3. **Discuss alternatives** if applicable
4. **Estimate complexity** (if you know)
5. **Suggest implementation approach**

## Documentation

### Updating Documentation

- **User docs**: Update `README.md`, `easy_usage.md`, `HOW_TO_USE.md`
- **Developer docs**: Update `DEVELOPMENT.md`
- **API docs**: Update `docs/API.md` (if exists)
- **Code comments**: Update docstrings and comments

### Documentation Standards

- **Clarity**: Write for the intended audience
- **Completeness**: Include examples where appropriate
- **Accuracy**: Keep documentation in sync with code
- **Style**: Follow existing formatting patterns

## Performance Considerations

### Profiling

- Use profiling tools for performance-critical code
- Ensure tests measure realistic workloads
- Profile both memory usage and execution time

### Optimization Guidelines

- Prefer vectorized operations where possible
- Minimize allocations in hot paths
- Use smart pointers for memory safety
- Consider using const references where appropriate

## Security Considerations

- Input validation is essential for all external interfaces
- Use safe string handling functions
- Implement proper error handling
- Avoid hardcoded secrets or credentials
- Use code scanning tools for security analysis

## Community Guidelines

- **Respectful discourse**: Be constructive in feedback
- **Help new contributors**: Guide beginners when possible
- **Share knowledge**: Document lessons learned
- **Be inclusive**: Welcome contributions from all backgrounds

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Code of Conduct

Please be respectful and constructive in all interactions. We follow a zero-tolerance policy for harassment or discrimination.

## Acknowledgments

Thanks to all contributors who have helped make NLM a better project! Their work makes this research possible.

---

*Need help? Ask in the repository's discussions or create an issue.*
