# NLM Contributing Guide

Welcome to the NLM (熙然) project! This document provides guidelines for contributing to the project, including code quality standards, linting, and formatting requirements.

## Overview

NLM is an experimental artificial developmental brain project. This guide ensures code quality and maintainability across the project.

## Code Quality Standards

### 1. Code Formatting

The project uses **clang-format** for consistent code style across all C++ files.

#### Installation
```bash
# Ubuntu/Debian
 sudo apt-get install -y clang-format

# macOS
brew install clang-format

# Or use package manager
# Windows ( Chocolatey ) choco install clang-format
```

#### Usage
```bash
# Check code formatting (dry run)
clang-format --dry-run -Werror src/**/*.cpp src/**/*.hpp

# Auto-format code
clang-format -i src/**/*.cpp src/**/*.hpp

# Run the automation script
./scripts/run_linting.sh check  # Just check formatting
./scripts/run_linting.sh full   # Full linting suite
```

#### Configuration
The project uses `.clang-format` configuration file located in the project root.

### 2. Static Analysis

The project uses **clang-tidy** and **cppcheck** for static code analysis.

#### Installation
```bash
# Ubuntu/Debian
sudo apt-get install -y clang-tidy cppcheck

# macOS
brew install clang-tidy cppcheck
```

#### Usage
```bash
# Run clang-tidy analysis
clang-tidy src/**/*.cpp -- -I src/

# Run cppcheck analysis
cppcheck --enable=all --inconclusive --std=c++20 src/**/*.cpp

# Run full analysis using the automation script
./scripts/run_linting.sh full
```

#### Configuration
- `.clang-tidy` - Clang-tidy configuration
- `.cppcheck` - Cppcheck configuration

### 3. Build and Testing

#### Building the Project
```bash
# Standard build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Running Tests
```bash
# Run CTest tests
cd build
ctest --output-on-failure

# Run CI checks target
make ci_checks
```

#### Running Phase Validation
```bash
# Run Phase 6 integration test (full brain integration)
./build/nlm_phase6_demo

# Run specific phase tests
make run_phase_3_validation
make run_phase_4_validation
make run_phase_6_integration
```

## Automation Script

The project provides a comprehensive automation script at `scripts/run_linting.sh` with the following modes:

### Modes

1. **check** (default)
   - Run basic linting checks
   - Check code formatting only
   - Fast feedback for development

2. **format**
   - Auto-format all C++ code
   - Apply clang-format to all source files
   - Create git diffs for review

3. **full**
   - Run complete linting suite
   - Check formatting, clang-tidy, cppcheck
   - Build project
   - Run existing test targets
   - Most comprehensive but takes longer

4. **quick**
   - Fast formatting check only
   - Minimal checks for rapid feedback
   - Best for iterative development

### Script Features

- Comprehensive logging with timestamps
- Detailed progress reporting
- Automatic dependency checking
- Graceful error handling
- Detailed log file generation
- Cross-platform support

## CI/CD Integration

The project includes GitHub Actions workflow for automated code quality checks:

### `.github/workflows/linting.yml`

This workflow runs on:
- Push to main and development branches
- Pull requests to main branch
- Manual workflow dispatch

#### CI Checks Include:
1. **Code Formatting Check** - clang-format validation
2. **Clang-Tidy Analysis** - Static code analysis
3. **CppCheck Analysis** - Additional static analysis
4. **CMake Configuration** - CMake validation
5. **Project Build** - Build verification
6. **Existing Tests** - Test suite execution

#### Auto-Format Workflow
Separate workflow for auto-formatting code when needed.

## Pre-commit Hooks

For local development, consider using pre-commit hooks to automatically run linting before commits:

### Installation
```bash
# Install pre-commit
curl https://pre-commit.com/install-local | bash

# Install the hooks
pre-commit install
```

### Pre-commit Config
Create `.pre-commit-config.yaml`:
```yaml
repos:
  - repo: local
    hooks:
      - id: clang-format
        name: clang-format
        entry: bash -c 'clang-format -i'
        language: system
        files: \.(cpp|hpp|cc|cxx|h)$
        pass_filenames: true
```

## Development Workflow

### 1. Initial Development
```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Run initial linting check
./scripts/run_linting.sh check

# If issues found, fix formatting
./scripts/run_linting.sh format
```

### 2. Regular Development
```bash
# Quick check before committing
./scripts/run_linting.sh quick

# Full validation before merge
./scripts/run_linting.sh full
```

### 3. Code Review
- Ensure all linting checks pass
- Check for any formatting issues
- Review any new linting warnings
- Ensure CI/CD passes

## Common Issues and Solutions

### Issue: "No such file or directory" for linting tools

**Solution:** Install required tools:
```bash
sudo apt-get install -y clang-format clang-tidy cppcheck
```

### Issue: Code formatting check fails

**Solution:** Auto-format the code:
```bash
./scripts/run_linting.sh format
git add .
git commit -m "format: auto-format code"
```

### Issue: Too many linting warnings

**Solution:**
1. Review new code for potential issues
2. Check if existing code needs updates
3. Consider if warnings are false positives
4. Fix critical warnings first

### Issue: CI/CD pipeline failing

**Solution:**
1. Run the same commands locally as CI/CD does
2. Check the detailed logs in the CI/CD run
3. Fix any issues found locally first

## Configuration Customization

### Customize Clang-Format
Edit `.clang-format` to adjust formatting rules:
- `IndentWidth` - Number of spaces for indentation
- `ColumnLimit` - Maximum line length
- `BreakStringLiterals` - Whether to break string literals
- `IncludeCategories` - Order of include statements

### Customize Clang-Tidy
Edit `.clang-tidy` to add/remove checks:
- `Checks` - List of enabled/disabled checks
- `DisabledChecks` - Checks to disable entirely
- `CheckOptions` - Specific check options

### Customize CppCheck
Edit `.cppcheck` to adjust analysis:
- `enable` - Checks to enable
- `disable` - Checks to disable
- `define` - Macros to define
- `include` - Include directories

## Troubleshooting

### Linting tools not found
```bash
# Verify installation
which clang-format
which clang-tidy
which cppcheck

# Reinstall if necessary
sudo apt-get install -y clang-format clang-tidy cppcheck
```

### Build fails during linting
```bash
# Check CMake configuration
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

### Large number of linting errors
```bash
# Run just the formatting check first (easiest to fix)
./scripts/run_linting.sh format

# Then run clang-tidy separately
clang-tidy src/**/*.cpp -- -I src/
```

## Support

For questions about code quality or linting, refer to:
- This CONTRIBUTING.md guide
- Source code comments in configuration files
- GitHub Issues for bug reports
- Project documentation in `docs/` directory

## License

This contributing guide is part of the NLM project and is covered by the MIT License.

---

*Last updated: $(date '+%Y-%m-%d')*

For more information about NLM, visit the [project documentation](docs/) or refer to the [README](README.md).
