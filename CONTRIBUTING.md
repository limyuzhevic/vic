# Contributing to NLM

Thank you for your interest in contributing to NLM (Neural Learning Machine)! This document outlines the guidelines for contributing to our project.

## Getting Started

### 1. Fork and Clone

1. Fork the repository: https://github.com/nlm-project/nlm/fork
2. Clone your fork locally:
   ```bash
   git clone https://github.com/[your-username]/nlm.git
   cd nlm
   ```
3. Set up your development environment (see DEVELOPMENT.md for details)

### 2. Development Workflow

1. **Create a feature branch:**
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make your changes:**
   - Follow the code style guidelines
   - Add tests for new functionality
   - Update documentation as needed

3. **Commit your changes:**
   ```bash
   git add .
   git commit -m "Add your feature description"
   ```

4. **Push to your branch:**
   ```bash
   git push origin feature/your-feature-name
   ```

5. **Create a Pull Request:**
   - Go to your repository on GitHub
   - Click on "Pull Requests"
   - Click "New Pull Request"
   - Select your feature branch as the base
   - Provide a clear description of your changes
   - Request a review from maintainers

## Code Quality Guidelines

### 1. Code Style

- Follow the project's C++ and Python code style
- Use two-space indentation for C++ files
- Use four-space indentation for Python files
- Write descriptive commit messages
- Keep changes focused and minimal

### 2. Testing

NLM has comprehensive test coverage:

**C++ Tests:**
- Located in `tests/` directory
- Built with CMake (see CMakeLists.txt)
- Run through the CI pipeline
- Each component has dedicated test files

**Python Tests:**
- Located in `tests/` directory
- Use pytest framework
- Test Python bindings (`pynlm` module)
- Include unit and integration tests

**Testing Requirements:**
- All new code must include corresponding tests
- Tests should cover edge cases and error conditions
- Maintain at least 80% test coverage for new code

### 3. Documentation

- Document public APIs with Doxygen-style comments (for C++)
- Write docstrings for Python functions
- Update README.md when adding new features
- Maintain accurate documentation in DEVELOPMENT.md

## Project Structure

### Source Code

```
src/
├── core/           # Core utilities and types
├── brain/          # Neural components (neurons, synapses, etc.)
├── dynamics/       # Neural dynamics and simulation
├── plasticity/     # Learning rules (STDP, Hebbian, etc.)
├── development/    # Developmental processes
├── neuromodulation/ # Neuromodulators and reward systems
├── memory/         # Memory systems
├── prediction/     # Prediction and error signals
├── cognition/      # Cognitive mechanisms
├── sensory/        # Sensory processing
├── motor/          # Motor system
├── environment/    # Environment interface
├── experiments/    # Experiment framework
└── visualization/  # Visualization interfaces
```

### Key Libraries

- **nlm_core**: Core brain components (neurons, synapses, dynamics)
- **nlm_agent**: Agent-brain interface
- **nlm_world**: World interaction
- **nlm_phase3-nlm_phase6**: Phase-specific implementations

## Pull Request Process

1. **Self-review:** Ensure your changes follow all guidelines
2. **Code review:** Request review from project maintainers
3. **Tests pass:** All CI tests must pass
4. **Documentation:** Update relevant documentation
5. **Merge:** Once approved, your changes will be merged

## Issue Handling

### Reporting Issues

- Use GitHub Issues for bug reports and feature requests
- Include reproducible steps when reporting bugs
- Provide examples of expected vs actual behavior

### Requesting Features

- Create a feature request issue
- Discuss the proposed changes with maintainers
- Implement the feature following the contribution guidelines

## Code of Conduct

Please follow our [Code of Conduct](CODE_OF_CONDUCT.md) when participating in our project community.

## License

By contributing, you agree that your contributions will be licensed under the MIT License.

## Getting Help

- For general questions: GitHub Discussions
- For urgent issues: Contact maintainers through GitHub
- For development questions: Check existing issues and discussions

---

**Note:** NLM is a research project. Contributions should focus on scientific rigor, reproducibility, and adherence to the project's architectural philosophy.

## Emergency Contact

For critical issues affecting the project's functionality, please contact the project maintainers directly through GitHub.

---

Last updated: 2026-10-31

## Quick Reference

| Action | Command |
|--------|---------|
| Create feature branch | `git checkout -b feature/your-feature-name` |
| Test C++ components | `./build/nlm_test_suite` |
| Test Python bindings | `pytest tests/` |
| Build project | `mkdir build && cd build && cmake .. && make -j4` |
| Lint code | `clang-format --style=file -i src/**/*.cpp src/**/*.h` |