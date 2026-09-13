# NLM - Contributing Guidelines

Thank you for your interest in contributing to the NLM (Neural Learning Machine) project! This document outlines the guidelines and processes for contributing to our research project.

## Welcome

NLM is an experimental brain simulation project focused on understanding computational intelligence through neural systems. We're looking for contributors who are passionate about:

- Brain simulation and neural networks
- Scientific research and experimentation
- Open source software development
- Performance optimization and systems programming

## Getting Started

### Prerequisites

Before making any contributions, please ensure you have:

1. **Development Environment**
   - C++20 compatible compiler
   - CMake 3.16 or higher
   - Python 3.8+ for bindings
   - Git for version control

2. **Build Tools**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j$(nproc)
   ```

3. **Python Dependencies**
   ```bash
   pip install scikit-build-core pybind11 pytest numpy
   pip install -e .
   ```

### Project Structure

```
NLM/
├── src/
│   ├── brain/          # Neural components
│   ├── agent/          # Agent control system
│   ├── world/          # Environment interface
│   ├── memory/         # Memory systems
│   └── ...             # Other core modules
├── tests/              # Unit and integration tests
├── docs/               # Documentation (this folder)
├── examples/           # Usage examples (this folder)
└── python/             # Python bindings
```

## Making Your First Contribution

### 1. Find an Issue

Look for issues labeled `good first issue` or `help wanted`:

- Browse the GitHub repository issues
- Look for bugs, documentation updates, or small improvements

### 2. Fork the Repository

1. Fork the NLM repository to your own GitHub account
2. Clone it locally:
   ```bash
   git clone https://github.com/yourusername/nlm.git
   cd nlm
   ```

### 3. Set Up Your Development Branch

```bash
# Create a feature branch based on main
git checkout main
git pull origin main
git checkout -b your-feature-name
```

### 4. Write a Test

Before implementing any fix, write a test that reproduces the issue:

```cpp
// Example test structure
void testAgentBrainMotorCommands() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 100, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agent(brain);
    
    // Test that motor commands work
    nlm::MotorCommand cmd = agent.decodeMotorCommand();
    assert(cmd == nlm::MotorCommand::Wait || cmd != nlm::MotorCommand::Wait);
}
```

### 5. Implement the Fix

- Follow the existing code style and conventions
- Add null pointer validation where needed
- Implement proper error handling
- Update documentation if necessary

### 6. Update Documentation

- Update examples in the `examples/` directory
- Fix documentation in `easy_usage.md`, `HOW_TO_USE.md`
- Update comments in source code if needed

### 7. Test Your Changes

```bash
# Run unit tests
./nlm_test

# Run Python bindings test
python -c "import pynlm; print('Python bindings work!')"

# Test integration
./nlm_phase6_demo
```

### 8. Submit Your Changes

```bash
# Stage your changes
git add .

# Create a commit message
git commit -m "FIX: Add LookLeft/LookRight motor commands

- Fix off-by-one error in AgentBrain
- Add new motor commands LookLeft and LookRight
- Implement proper constructor initialization
- Add null pointer validation
- Add destructor for cleanup

Co-authored-by: openhands <openhands@all-hands.dev>"

# Push to your branch
git push origin your-feature-name

# Create a pull request on GitHub
```

## Contribution Types

### Bug Fixes

When fixing bugs:
1. **Reproduce the issue** with a test case
2. **Identify the root cause** and create a minimal fix
3. **Add validation** to prevent similar issues
4. **Update tests** to cover the fix
5. **Document the fix** for future reference

### New Features

For new features:
1. **Write comprehensive tests** before implementation
2. **Follow existing patterns** in the codebase
3. **Add examples** in the examples/ directory
4. **Update documentation** with clear usage instructions
5. **Add performance tests** if performance is critical

### Documentation Updates

Documentation contributions:
1. **Update code comments** to clarify complex sections
2. **Improve existing documentation** with missing information
3. **Add examples** that demonstrate new functionality
4. **Create tutorials** for complex features
5. **Fix typos and errors** in existing docs

## Code Quality Standards

### C++ Coding Style

- **Documentation**: Every public method should have a docstring
- **Error Handling**: Add null pointer checks and input validation
- **Memory Management**: Use smart pointers where possible
- **Performance**: Pre-allocate vectors when possible
- **Testing**: Every function should have test coverage

### Python API

- **Consistency**: Follow existing Python binding patterns
- **Documentation**: Use docstrings for all public functions
- **Examples**: Provide working examples in the examples/ directory

## Testing Guidelines

### Unit Tests

- Test all public APIs
- Test edge cases (null pointers, empty inputs, boundary values)
- Test error conditions and exception safety
- Test performance-critical code paths

### Integration Tests

- Test the complete agent-world interaction
- Test neural network initialization and propagation
- Test memory systems and plasticity
- Test neuromodulation and development

### Performance Tests

- Benchmark critical operations
- Profile memory usage
- Test scalability with larger networks

## Performance Optimization

### Memory Management

- Use `reserve()` for vectors when sizes are known
- Clear vectors in destructors
- Avoid unnecessary copying

### Computational Efficiency

- Use bulk operations instead of element-wise
- Cache frequently used values
- Pre-compute what can be pre-computed

### Code Organization

- Group related functionality together
- Use header guards and include guards
- Forward declare types to reduce includes

## Branching Strategy

### Main Branch

- Always deployable and stable
- Contains the latest released version

### Feature Branches

- Branch from `main`
- Prefix: `feature/` for new features, `fix/` for bug fixes
- Merge back to `main` via pull request

### Development Branch

- Optional: Use for ongoing development
- Can be unstable

## Code Review Process

### Pull Request Review

1. **Author**: Creates pull request with clear description
2. **Reviewer**: Reviews code, tests, and documentation
3. **Feedback**: Provides constructive feedback
4. **Iteration**: Fixes any issues raised
5. **Merge**: After approval, merges to main branch

### Checklist for Review

- [ ] Code follows project conventions
- [ ] All tests pass
- [ ] Code is well-documented
- [ ] Performance is acceptable
- [ ] No memory leaks
- [ ] Error handling is robust

## Common Issues and Solutions

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Communication

### Development Discussions

- Use GitHub issues for bug reports and feature requests
- Use pull requests for code changes
- For real-time discussion, use the project Discord/Telegram channel

### Code of Conduct

Please treat everyone with respect and follow these guidelines:
- Be respectful and inclusive
- Give constructive feedback
- Help others learn and grow
- Focus on the code, not the person

## Acknowledgements

Thanks to all contributors who have helped make NLM a better project. Your contributions are valued and appreciated!

Special thanks to:
- Early adopters who tested the software
- Contributors who reported bugs and suggested improvements
- Researchers who inspired the project

## License

This project is licensed under the MIT License. See LICENSE file for details.
