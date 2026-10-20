# NLM Contributing Guide - How to Contribute to the Project

## Overview

This guide provides comprehensive instructions for contributing to the NLM (Neural Learning Machine) project. It covers the contribution process, code quality standards, testing requirements, documentation guidelines, and best practices for working with the NLM codebase.

## Getting Started

### 1. Project Setup

#### Prerequisites

Before contributing, ensure you have:

- **Development Environment**:
  - C++17 compatible compiler (GCC 9+, Clang 10+, or MSVC 2019+)
  - CMake 3.16 or higher
  - Python 3.8 or higher
  - git (version control)
  - Optional: IDE with C++ support (CLion, VS Code, etc.)

- **Build Tools**:
  - CMake build system
  - Python package management (pip)
  - Build automation tools (make, ninja)

#### Setting Up the Development Environment

```bash
# Clone the NLM repository
git clone https://github.com/nlm-project/nlm.git

cd nlm

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Install dependencies (if needed)
pip install -r ../requirements.txt
```

#### Running Tests

After setting up the environment, run the tests to verify everything works:

```bash
# Run unit tests
cd build
ctest --output-on-failure

# Or build and run all tests
make -j4
ctest
```

### 2. Understanding the Project Structure

NLM is organized into several key components:

```
NLM/
├── src/                    # Core source code
│   ├── brain/            # Brain components
│   ├── agent/            # Agent systems
│   ├── world/            # World simulation
│   ├── memory/           # Memory systems
│   ├── plasticity/       # Plasticity mechanisms
│   ├── neuromodulation/   # Neuromodulation systems
│   └── ...               # Other components
│
├── docs/                   # Documentation
│   ├── API documentation  # Auto-generated Doxygen docs
│   ├── tutorials         # Tutorial series
│   └── guides            # Contributing guides
│
├── examples/               # Example applications
├── tests/                  # Test suites
├── build/                  # Build artifacts
├── python/                 # Python bindings
│   ├── bindings.cpp       # C++ to Python bindings
│   └── CMakeLists.txt    # Python binding build configuration
└── .github/                # GitHub workflows and issues
```

## Contribution Process

### 1. Finding Issues and Projects

#### Issue Tracking

NLM uses GitHub for issue tracking. You can:

- **Browse existing issues**: Look for bugs, feature requests, or improvement opportunities
- **Report new issues**: File detailed bug reports or feature requests
- **Claim issues**: Take ownership of existing issues to work on them

#### Project Management

NLM uses GitHub Projects for tracking work:

- **Backlog**: Untriaged issues and feature requests
- **Todo**: Issues being planned or worked on
- **In Progress**: Issues currently being implemented
- **Review**: Issues ready for code review
- **Done**: Completed issues

### 2. Setting Up Your Development Environment

#### Forking the Repository

1. Go to the [NLM GitHub repository](https://github.com/nlm-project/nlm)
2. Click the "Fork" button in the top-right corner
3. Clone your forked repository:

```bash
git clone https://github.com/your-username/nlm.git
```

#### Creating a Development Branch

```bash
# Navigate to the project directory
cd nlm

# Create and switch to a new branch for your feature/bug fix
feature/your-feature-name
```

#### Installing Dependencies

```bash
# Install build dependencies
sudo apt-get install build-essential cmake

# Install Python dependencies
pip install -r requirements.txt

# Install development tools (optional)
pip install pytest black flake8 mypy
```

### 3. Making Changes

#### Code Quality Standards

NLM follows these coding standards:

1. **C++17**: Use C++17 language features
2. **Google Style Guide**: Follow Google C++ style guide
3. **Documentation**: Use Doxygen for API documentation
4. **Testing**: Write comprehensive unit tests
5. **Performance**: Optimize for performance and memory efficiency

#### Code Formatting

NLM uses modern formatting tools:

```bash
# Install formatting tools
pip install black isort flake8 mypy

# Format Python code
black python/

# Format C++ code (using clang-format)
find src -name "*.cpp" -name "*.hpp" -exec clang-format -i {} \;
```

#### Linting and Type Checking

Run linting and type checking before submitting:

```bash
# Run Python linting
black --check python/
flake8 python/

# Run C++ linting (if configured)
find src -name "*.cpp" -name "*.hpp" -exec cpplint {} \;

# Run type checking (if configured)
# mypy python/
```

### 4. Writing Tests

#### Testing Philosophy

NLM follows a comprehensive testing approach:

1. **Unit Tests**: Test individual functions and classes
2. **Integration Tests**: Test component interactions
3. **System Tests**: Test complete functionality
4. **Performance Tests**: Test performance characteristics
5. **Regression Tests**: Prevent regressions

#### Writing Unit Tests

NLM uses Google Test (gtest) for C++ testing:

```cpp
// Example unit test
TEST(NeuronTest, BasicProperties) {
    // Create neuron
    Neuron neuron(NeuronId(1));
    
    // Test basic properties
    EXPECT_TRUE(neuron.getId().value() == 1);
    EXPECT_EQ(neuron.getType(), NeuronType::Internal);
    
    // Test state
    EXPECT_TRUE(neuron.isFiring() == false);
    EXPECT_TRUE(neuron.isRefractory() == false);
}

TEST(NeuronTest, LIFDynamics) {
    // Create neuron with specific parameters
    Neuron neuron(NeuronId(1));
    neuron.setMembranePotential(-70.0f);
    neuron.setThreshold(-50.0f);
    
    // Test LIF update
    bool spiked = neuron.stepLIF(10.0f); // 10ms timestep
    
    // Verify results
    if (neuron.getMembranePotential() > -50.0f) {
        EXPECT_TRUE(spiked);
    } else {
        EXPECT_FALSE(spiked);
    }
}
```

#### Python Testing

NLM uses pytest for Python testing:

```python
# Example Python test
import pytest
import pynlm

class TestNLM:
    def setup_method(self):
        # Create default configuration
        self.config = pynlm.createDefaultConfig()
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
    
    def test_basic_functionality(self):
        # Test basic brain operations
        assert self.brain.getTotalNeuronCount() > 0
        assert self.brain.getTotalSynapseCount() > 0
        
        # Run simulation
        for i in range(100):
            self.brain.step(i)
        
        # Verify results
        assert self.brain.getTotalSpikeCount() >= 0
    
    def test_configuration(self):
        # Test configuration
        assert self.config.has("brain.neuron_count")
        assert self.config.get("brain.neuron_count") > 0
```

### 5. Code Review Process

#### Pull Request Workflow

1. **Commit Changes**: Make meaningful, focused commits
   ```bash
   git add src/ tests/
   git commit -m "Implement feature X with improved performance"
   ```

2. **Push to Branch**: Push your changes to your forked repository

3. **Create Pull Request**: Create a pull request from your branch to the main repository
   - Provide clear description
   - Link related issues
   - Include test results
   - Mention any breaking changes

4. **Code Review**: Your changes will be reviewed by maintainers
   - Check code quality
   - Verify tests pass
   - Ensure documentation is updated
   - Confirm performance requirements

5. **Merge**: After successful review, changes will be merged

#### Review Checklist

When reviewing pull requests, maintainers check:

- [ ] **Code Quality**: Follows coding standards and conventions
- [ ] **Tests**: All tests pass
- [ ] **Documentation**: Documentation is updated
- [ ] **Performance**: Performance is maintained or improved
- [ ] **Compatibility**: Changes are backward compatible
- [ ] **Security**: No security vulnerabilities introduced

### 6. Building and Deploying

#### Building the Project

```bash
# Build the project
cd nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Running Tests

```bash
# Run all tests
cd build
ctest --output-on-failure

# Or build with tests
make test
```

#### Generating Documentation

NLM uses Doxygen for API documentation:

```bash
# Generate API documentation
cd nlm
doxygen Doxyfile.in
```

#### Running Examples

Run example applications:

```bash
# Run example executables
cd build
./example_application
```

### 7. Contributing Code

#### Pull Request Guidelines

- **Keep changes focused**: Address one issue or feature per pull request
- **Write clear commits**: Use descriptive commit messages
- **Use feature branches**: Work in feature branches
- **Update documentation**: Update relevant documentation
- **Add tests**: Write or update tests as needed
- **Follow conventions**: Follow project conventions and style

#### Working with the Community

- **Be respectful**: Treat all contributors with respect
- **Help others**: Assist others in understanding the codebase
- **Share knowledge**: Document your contributions
- **Celebrate success**: Acknowledge others' contributions

## Documentation Contribution

### 1. API Documentation

NLM uses Doxygen for API documentation. To contribute:

1. Add Doxygen comments to your code:
   ```cpp
   /**
    * @brief Gets the neuron's ID
    * @return NeuronId The neuron's ID
    */
   NeuronId Neuron::getId() const {
       return pImpl->id;
   }
   ```

2. Run Doxygen to generate documentation:
   ```bash
   doxygen Doxyfile.in
   ```

3. Review generated documentation for completeness and accuracy

### 2. Tutorial Documentation

NLM has a tutorial series for different skill levels:

- **Beginner Tutorials**: Introduction to NLM concepts
- **Intermediate Tutorials**: Advanced NLM features
- **Expert Tutorials**: Custom NLM applications

To contribute a tutorial:
1. Write tutorial content in Markdown
2. Include code examples
3. Provide step-by-step instructions
4. Test examples before adding

### 3. Guide Documentation

Contribute guides for:

- **Configuration examples**: How to configure NLM for different use cases
- **Performance optimization**: How to optimize NLM performance
- **Integration guides**: How to integrate NLM with external systems
- **Migration guides**: How to migrate from other frameworks
- **Troubleshooting guides**: How to debug and fix issues

## Code Quality Requirements

### 1. C++ Code Quality

#### Style Requirements

- **Indentation**: 2 spaces per indent level
- **Line length**: Maximum 80 characters
- **Naming**: camelCase for variables, PascalCase for types and functions
- **Comments**: Use Doxygen format for API documentation
- **Whitespace**: No trailing whitespace

#### Testing Requirements

- **Unit tests**: All functions must have unit tests
- **Test coverage**: Minimum 80% code coverage
- **Edge cases**: Test boundary conditions and edge cases
- **Error handling**: Test error conditions and exceptions

#### Performance Requirements

- **Optimization**: Maintain performance characteristics
- **Memory usage**: Optimize memory usage
- **Scalability**: Ensure scalability with increasing problem sizes

### 2. Python Code Quality

#### Style Requirements

- **PEP 8**: Follow PEP 8 style guide
- **Type hints**: Use type hints for better code clarity
- **Documentation**: Use docstrings with examples
- **Testing**: Write comprehensive tests

#### Dependencies

- **Requirements.txt**: List all Python dependencies
- **Setup.py**: Proper package configuration
- **Tests**: Comprehensive test suite

## Best Practices

### 1. Testing Best Practices

- **Write tests first**: Write tests before implementing features
- **Test edge cases**: Test boundary conditions and error cases
- **Mock external dependencies**: Use mocking for external dependencies
- **Test performance**: Include performance tests
- **Continuous integration**: Use CI/CD pipelines

### 2. Documentation Best Practices

- **Clear examples**: Provide clear, working examples
- **Comprehensive documentation**: Document all public APIs
- **Tutorials**: Provide step-by-step tutorials
- **API reference**: Generate API documentation
- **Best practices**: Document best practices and common patterns

### 3. Code Review Best Practices

- **Code reviews**: Regularly review code with team members
- **Peer programming**: Work with others on complex features
- **Code standards**: Follow established code standards
- **Documentation**: Keep documentation up to date

## Getting Help

### 1. Online Resources

- **GitHub Issues**: Ask questions and report problems
- **Documentation**: Read documentation for guidance
- **Examples**: Look at existing examples for patterns
- **Community**: Join the NLM community

### 2. In-Person Help

- **Code reviews**: Get feedback on your code
- **Pair programming**: Work with others on challenging problems
- **Mentoring**: Get guidance from experienced contributors

### 3. Communication

- **Clear communication**: Be clear and concise
- **Documentation**: Document decisions and reasoning
- **Collaboration**: Work collaboratively with others
- **Respect**: Treat all contributors with respect

## Conclusion

Contributing to NLM is an exciting opportunity to improve the neural simulation community. By following these guidelines, you can:

1. **Make meaningful contributions**: Create valuable features and fixes
2. **Maintain code quality**: Ensure high-quality code and documentation
3. **Collaborate effectively**: Work well with other contributors
4. **Grow as a developer**: Learn from the experience and improve your skills
5. **Impact the community**: Make a difference in the neural simulation community

The NLM Contributing Guide provides the foundation for creating high-quality contributions that:

- **Address real needs**: Solve real problems for NLM users
- **Maintain quality**: Follow coding standards and best practices
- **Support the community**: Help other developers and users
- **Drive innovation**: Push the boundaries of neural simulation technology

By contributing to NLM, you become part of a community of developers, researchers, and enthusiasts working together to advance neural simulation technology.