# NLM Contributing Guidelines

## Overview

This document outlines the guidelines and processes for contributing to the NLM (熙然) neural learning machine project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**NLM is NOT**:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

**NLM IS intended to become**:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## How to Contribute

### 1. Getting Started

#### Prerequisites
- Basic understanding of C++ and Python programming
- Familiarity with CMake build system
- Understanding of neural networks and brain architectures

#### First Contributions
- Fix documentation issues and typos
- Add documentation for new parameters
- Create examples and tutorials
- Fix build issues for new platforms

### 2. Code Contributions

#### Coding Standards
1. **Follow existing patterns**: Look at existing code in `src/` for style and conventions
2. **Use appropriate naming**: PascalCase for classes, camelCase for methods/variables
3. **Add documentation**: Every public class and method should have Doxygen-style comments
4. **Write tests**: Add unit tests for new code
5. **Code reviews**: All code changes should be reviewed by other contributors

#### Pull Request Process
1. **Fork the repository**: Create your own fork
2. **Create a feature branch**: Use descriptive branch names
3. **Make your changes**: Implement your feature or fix
4. **Test your changes`: Run all tests locally
5. **Update documentation**: Add or update relevant documentation
6. **Submit a pull request`: Open a PR against the main branch
7. officer, Tests: Address feedback and iterate
8. **Merge`: Once approved and tested

### 3. Documentation Contributions

#### Documentation Guidelines
1. **Use Markdown`: All documentation uses Markdown format
2. **Consistent formatting`: Follow existing documentation style
3. **Include examples`: All documentation should include practical examples
4. **Cross-reference`: Link between related documentation sections
5. **Maintain accuracy`: Keep documentation synchronized with code changes

#### Documentation Sections
- **Configuration documentation`: Parameter descriptions, types, and examples
- **API documentation`: Python bindings and C++ interfaces
- **User guides`: Installation, usage, and examples
- **Developer documentation`: Architecture, design patterns, and best practices

### 4. Testing and Validation

#### Testing Guidelines
1. **Unit tests`: Each component should have comprehensive unit tests
2. **Integration tests`: Test system behavior with complete configurations
3. **Performance tests`: Profile and optimize performance
4. **Documentation tests`: Verify documentation examples work
5. **Regression tests`: Ensure existing functionality remains intact

#### Running Tests
```bash
# Run unit tests
cd build
ctest --output-on-failure

# Run integration tests
./nlm_phase6_demo --test

# Run performance benchmarks
./nlm_benchmark
```

### 5. Configuration Management

#### Configuration Guidelines
1. **Always specify random_seed`: For reproducible experiments
2. **Validate configuration`: Use `nlm config validate` before running
3. **Document configuration changes`: Add comments to configuration files
4. **Test configurations`: Test with small configurations first
5. **Backup important configurations`: Save working configurations

#### Configuration Examples
```python
# Example: Configuration with comments
# This configuration is optimized for research experiments
config = {
    "random_seed": 12345,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,ank (const unsigned int value) = value;
    "connection_probability": 0.1,
    // ... more parameters
}
```

### 6. Feature Development

#### Feature Proposal
1. **Open an issue`: Propose new features in GitHub issues
2. **Discuss design`: Review existing architecture
3. **Implement prototype`: Create a proof-of-concept
4. **Design documentation`: Document the feature
5. **Implement fully`: Add to main codebase
6. **Test thoroughly`: Ensure comprehensive coverage

#### Feature Categories
- **Core neural components`: New neuron types, plasticity rules
- **Memory systems`: Enhanced working memory, new memory types
- **Neuromodulation`: New neuromodulators, integration schemes
- **Prediction`: New prediction algorithms, multi-step prediction
- **Cognition`: New cognitive mechanisms, attention models
- **Development`: Enhanced developmental processes
- **Performance`: Optimizations, parallel processing
- **Testing`: New test frameworks, benchmark suites

### 7. Code Review Process

#### Review Checklist
- [ ] Code follows project coding standards
- [ ] Tests pass for all changes
- [ ] Documentation is updated
- [ ] Performance is not degraded
- [ ] No regression in existing functionality
- [ ] Appropriate error handling
- [ ] Memory management is correct
- [ ] Integration points are properly tested

#### Reviewer Responsibilities
- **Code reviewer**: Reviews code quality and standards
- **Functionality reviewer`: Tests new features and behavior
- **Documentation reviewer`: Verifies documentation accuracy
- **Performance reviewer`: Profiles performance impact
- **assumptions reviewer`: Validates design decisions

### 8. Community Guidelines

#### Communication
1. **Be respectful`: All contributors are welcome and respected
2. **Be constructive`: Provide helpful feedback and suggestions
3. **Ask for help`: Don't hesitate to ask questions
4. **Share knowledge`: Document your discoveries and solutions

#### Diversity and Inclusion
1. **Welcoming environment`: Project welcomes people from all backgrounds
2. **Different perspectives`: Value diverse approaches to problem-solving
3. **Accessibility`: Make project materials accessible to all
4. **Inclusive language`: Use inclusive and respectful language

### 9. License and Attribution

#### License Compliance
1. **MIT License`: All contributions are under the MIT License
2. **Attribution`: Proper attribution for all contributors
3. **No proprietary code`: All code must be open source
4. **License compatibility`: Ensure compatibility with existing license

#### License Headers
```cpp
// NLM (熙然) - Neural Learning Machine
// Copyright 2024-2026 The NLM Contributors
// MIT License - See LICENSE file for details
```

### 10. Project Maintenance

#### Issue Management
1. **Priority levels`: Bug fixes > Features > Documentation > Refactoring
2. **Release process`: Plan and document releases
3. **Version control`: Use semantic versioning
4. **Bug triage`: Regularly review and prioritize bugs

#### Release Process
```bash
# Create release branch
ggit checkout -b release/v2.0. SLS

# Update version in CMakeLists.txt
# Update version in documentation
# Run full test suite
# Create release notes
# Tag the release
ggit tag -a v2.0.0 -m "Version 2.0.0"

# Push to remote
# Create GitHub release
```

## Getting Help

### Community Support
- **GitHub Issues**: Report bugs and request features
- **Discussions**: General discussion and brainstorming
- **Pull Requests**: Submit code changes
- **Code Reviews**: Review other contributions

### Technical Support
- **Documentation**: Check project documentation
- **Examples**: Run example code from documentation
- **Stack Overflow**: Ask questions with `nlm` tag
- **Direct Contact**: Contact maintainers for urgent issues

### Contributing Resources
- **Documentation**: `docs/CONTRIBUTING.md`
- **Code Examples**: `examples/` directory
- **API Reference**: `docs/API_GUIDE.md`
- **Configuration Guide**: `docs/CONFIGURATION.md`
- **Best Practices**: `docs/BEST_PRACTICES.md`

## Conclusion

The NLM project thrives on community contributions. Whether you're fixing bugs, adding features, improving documentation, or creating examples, your contributions are valuable. This document aims to make it easy and productive for everyone to contribute to this exciting experimental brain simulation project.

Remember: **The best projects are made by the people who work on them together!**

---
*Contributing guidelines updated v2.0.0*
