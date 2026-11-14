# NLM Contributing Guide

## Welcome to NLM Development!

Thank you for your interest in contributing to the NLM (Neural Learning Machine) project. This guide provides information about how to contribute effectively to this experimental artificial developmental brain research platform.

## How to Contribute

### Types of Contributions

We welcome many different types of contributions to the NLM project:

#### 1. Code Contributions
- **Bug fixes**: Identify and fix issues in the C++ or Python code
- **Feature implementations**: Add new neural mechanisms, learning rules, or simulation components
- **Documentation improvements**: Update README files, API documentation, or examples
- **Performance optimizations**: Improve code efficiency and scalability
- **Testing enhancements**: Add new unit tests or integration tests

#### 2. Research Contributions
- **Experiment design**: Create new simulation paradigms
- **Results analysis**: Analyze simulation results and publish findings
- **Documentation**: Write technical papers or blog posts about discoveries
- **Benchmarks**: Compare performance with other neural models

#### 3. Community Contributions
- **Issue triage**: Help prioritize and address open issues
- **Code reviews**: Review pull requests and provide feedback
- **Mentorship**: Help new contributors get started
- **Conference/presentation**: Share results at relevant venues

## Getting Started

### 1. Set Up Your Development Environment

Follow the [Developer Guide](DEVELOPER.md) for complete setup instructions. Here's a quick summary:

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build the C++ core
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e ..
```

### 2. Choose an Issue

Look through our [GitHub Issues](https://github.com/nlm-project/nlm/issues) to find:

- **Good first issues**: Issues suitable for newcomers
- **Help wanted**: Issues needing additional contributors
- **Bug reports**: Issues with unclear reproduction steps
- **Feature requests**: New functionality suggestions

### 3. Get Involved

#### For Code Contributors:
1. **Fork the repository**
2. **Create a feature branch** from `main`
3. **Make your changes** following NLM coding standards
4. **Add tests** for new functionality
5. **Update documentation** if needed
6. **Run tests** to ensure everything works
7. **Submit a pull request**

#### For Documentation Contributors:
1. Follow the same process but focus on documentation files
2. Update examples in `README.md`, `easy_usage.md`, or `HOW_TO_USE.md`
3. Add comments to complex C++ code
4. Improve existing documentation in the `docs/` directory

#### For Research Contributors:
1. Fork the repository and create your experimental branch
2. Implement your research ideas
3. Document your methods and results
4. Consider submitting to relevant conferences or journals

## Coding Standards

### C++ Code Standards

#### General Principles:
- **Modern C++**: Use C++17/20 features where appropriate
- **Memory management**: Prefer `std::shared_ptr` for dynamic objects
- **Exception safety**: Ensure proper exception safety guarantees
- **Performance**: Profile before optimizing

#### Code Style:
- **Indentation**: 4 spaces, no tabs
- **Line length**: Maximum 100 characters
- **Naming conventions**: 
  - Classes: PascalCase
  - Functions: camelCase
  - Constants: UPPER_SNAKE_CASE
  - Variables: camelCase
- **Comments**: Explain the "why" not the "what"

#### Documentation:
- **Doxygen comments**: Document all public classes and methods
- **Code examples**: Include examples in documentation
- **References**: Cite relevant research papers

### Python Code Standards

#### Style:
- **PEP 8**: Follow Python style guidelines
- **Type hints**: Use type annotations for public APIs
- **Docstrings**: Comprehensive docstrings for all functions
- **Line length**: Maximum 79 characters for docstrings

#### Best Practices:
- **Error handling**: Use try-catch blocks appropriately
- **Configuration**: Use the configuration system properly
- **Testing**: Write comprehensive tests for new functionality

## Testing

### C++ Tests

Run the test suite after making changes:

```bash
cd build
ctest --output-on-failure
```

Or run specific test targets:
```bash
./tests/test_brain
./tests/test_neuron  
./tests/test_stdp
./tests/test_config
```

### Python Tests

```bash
pytest
```

### Adding New Tests

When adding new functionality:

1. **Write unit tests** for individual components
2. **Write integration tests** for system behavior
3. **Test edge cases** and error conditions
4. **Include performance benchmarks** where appropriate
5. **Document test cases** for future maintainers

## Documentation

### Updating README Files

The main documentation files are:

- **`README.md`**: Project overview and basic usage
- **`easy_usage.md`**: Quick start guide for beginners
- **`HOW_TO_USE.md`**: Comprehensive Python API guide
- **`DEVELOPER.md`**: Complete development guide (this file)

### Writing Documentation

Guidelines for documentation:

1. **Start with the audience in mind**: Are you writing for beginners or experts?
2. **Be comprehensive**: Include examples, error handling, and edge cases
3. **Keep it up to date**: Documentation should reflect current code
4. **Use consistent formatting**: Follow existing documentation patterns
5. **Include code examples**: Show usage in multiple languages (C++ and Python)

### Documentation Structure

For new documentation files:

```markdown
# [Title]

## Overview
[High-level description]

## Key Concepts
[Important background information]

## Getting Started
[Quick setup instructions]

## Advanced Usage
[Detailed information for power users]

## Examples
[Code snippets demonstrating usage]

## API Reference
[Detailed API documentation]

## Best Practices
[Common patterns and pitfalls]

## References
[Related papers and resources]
```

## Pull Request Process

### 1. Preparation

- **Branch from `main`**: Always work on a feature branch
- **Write tests**: Test your changes thoroughly
- **Update documentation**: Document any changes
- **Follow coding standards**: Ensure code meets project guidelines

### 2. Submission

- **Create a descriptive PR title**: Include what the PR fixes or implements
- **Write a clear description**: Explain the problem, solution, and rationale
- **Reference related issues**: Use `#issue-number` format
- **Include screenshots**: For UI changes or complex diagrams
- **Add reviewers**: Tag relevant maintainers for code review

### 3. Review Process

- **Automated checks**: CI runs tests and linting
- **Code review**: Peer review by project maintainers
- **Feedback incorporation**: Address reviewer comments
- **Final approval**: PR merged by maintainer

### 4. Post-Merge

- **Update CHANGELOG**: Document the changes
- **Version bump**: Increment version number if needed
- **Tag release**: Create a new Git tag for the release

## Project Management

### Issue Tracking

We use GitHub Issues for:

- **Bug reports**: Use the `bug` label
- **Feature requests**: Use the `enhancement` label
- **Help wanted**: Use the `help-wanted` label
- **Good first issues**: Use the `good-first-issue` label

### Development Workflow

The NLM project follows a **Phase-based development approach**:

- **Phase 1**: Project skeleton and core infrastructure
- **Phase 2**: Basic neural computation (real LIF neurons)
- **Phase 3**: World interaction and sensory-motor loop
- **Phase 4**: Cognitive mechanisms and memory
- **Phase 5**: Performance optimizations and checkpointing
- **Phase 6**: Final integration of all systems

When contributing:

1. **Understand the phase**: Know which phase your changes affect
2. **Follow phase goals**: Ensure changes align with phase objectives
3. **Document progress**: Update phase summaries in documentation
4. **Maintain backward compatibility**: Be careful with API changes

## Community Guidelines

### Code of Conduct

Please be respectful and considerate of other contributors. Everyone should feel welcome and safe to contribute.

### Communication Etiquette

- **Be patient**: Complex problems often require time
- **Ask questions**: If you're unsure about something, ask
- **Help others**: Share your knowledge and expertise
- **Be constructive**: Provide feedback in a helpful manner

## Getting Help

### Online Resources

- **GitHub Discussions**: Ask questions and share ideas
- **Issue Tracker**: Report bugs and request features
- **Documentation**: Check the latest documentation
- **Community**: Connect with other contributors

### Local Support

- **Development environment**: Check the Developer Guide
- **Build issues**: Visit the repository's help discussions
- **Code review**: Ask for help in GitHub issues

## Acknowledgements

We appreciate all contributions to the NLM project, including:

- **Code contributions**: All pull requests and bug fixes
- **Research contributions**: New findings and experimental results
- **Documentation contributions**: Better documentation and examples
- **Community contributions**: Help with issue triage and code reviews

Special thanks to all contributors who have made this experimental platform possible!

## License

This project is licensed under the MIT License. See `LICENSE` file for details.

---

*Last updated: $(date -I)
*For the latest version, check the `CHANGELOG` file or visit the project repository.