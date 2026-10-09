# Development Guidelines - NLM/熙然

## Overview

This document provides comprehensive development guidelines for working with the NLM (Neural Learning Machine) project. It covers:

- Development process and methodology
- Testing procedures and validation
- Code quality standards
- Debugging and troubleshooting
- Best practices for contribution
- Project structure and organization

## Development Process

### Project Structure

The NLM project follows a modular, phased development approach:

```
NLM/
├── src/
│   ├── core/           # Core utilities
│   ├── brain/          # Neural components
│   ├── dynamics/       # Neural dynamics
│   ├── plasticity/     # Plasticity rules
│   ├── development/    # Developmental system
│   ├── neuromodulation/ # Neuromodulators
│   ├── memory/        # Memory systems
│   ├── prediction/     # Prediction systems
│   ├── cognition/      # Cognitive mechanisms
│   ├── sensory/       # Sensory processing
│   ├── motor/         # Motor system
│   ├── environment/   # Environment interface
│   ├── experiments/   # Experiment framework
│   └── visualization/ # Visualization
├── python/              # Python bindings
├── docs/               # Documentation
├── tests/              # Test suite
├── CMakeLists.txt      # Build configuration
├── README.md          # Project documentation
└── LICENSE            # License information
```

### Development Phases

NLM development follows a structured, iterative approach:

#### Phase 1: Core Foundations (COMPLETED)
- Project skeleton and build system
- Core types and configuration
- Neural interfaces

#### Phase 2: Neural Dynamics (COMPLETED)
- LIF neuron dynamics
- Event-driven spike propagation
- STDP and Hebbian plasticity
- Structural plasticity

#### Phase 3: World Interaction (COMPLETED)
- World interaction loop
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

#### Phase 4: Emerging Cognition (COMPLETED)
- Neural prediction system
- Working memory
- Episodic-like memory
- Concept formation
- Neural attention (NOT Transformer)
- Predictive planning
- Self-model
- Social learning
- Continual learning

#### Phase 5: Performance Optimizations (COMPLETED)
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

#### Phase 6: Final Integration (COMPLETED)
- All systems integrated into coherent brain loop
- Memory systems connected to neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Development affects plasticity rates
- Checkpoint save/load working
- Replay and consolidation functional

#### Phase 7: Refinement and Enhancement (IN PROGRESS)
- Documentation improvements
- Additional integration points
- Performance optimizations
- Scientific validation

## Coding Standards

### Code Style

1. **C++ Standards**
   - C++20 with specific requirements:
     - Strong typing with `NeuronId`, `SynapseId`, etc.
     - Deterministic random number generators
     - Explicit random seeds for reproducibility
     - Event-driven spike propagation
     - No high-level abstractions - data-oriented design

2. **Python Bindings**
   - Use pybind11 for Python bindings
   - Clear documentation with examples
   - Type hints and error handling
   - Context managers for resource cleanup

### File Organization

#### Source Files
```
src/
├── core/              # Core utilities and types
│   ├── Types/Types.hpp # Type definitions
│   ├── Config/        # Configuration management
│   ├── Random/        # Random number generators
│   └── Logger/        # Structured logging
├── brain/             # Neural components
│   ├── Brain.hpp       # Brain interface
│   └── Brain.cpp       # Brain implementation
├── memory/            # Memory systems
│   ├── Memory.hpp     # Memory interface
│   ├── NeuralWorkingMemory.hpp # Working memory
│   └── NeuralEpisodicMemory.hpp # Episodic memory
└── ...               # Other components
```

#### Documentation
```
docs/
├── ARCHITECTURE.md   # System architecture
├── SCIENECE.md        # Scientific background
├── ROADMAP.md        # Development roadmap
├── EXPERIMENTS.md     # Experiment descriptions
├── LIMITATIONS.md     # Known limitations
├── PHASE6_FINAL_AUDIT.md # Phase 6 audit report
├── PYTHON_BINDINGS.md # Python bindings documentation
├── CONFIGURATION.md   # Configuration options
└── API_REFERENCE.md   # Complete API reference
```

### Naming Conventions

- **Classes**: CamelCase (`Brain`, `Config`, `AgentBrain`)
- **Types**: CamelCase with `Id` suffix (`NeuronId`, `SynapseId`)
- **Functions**: snake_case (`getNeuronCount()`, `setValue()`)
- **Variables**: snake_case (`neuron_count`, `time_step`)
- **Files**: snake_case (`brain.hpp`, `memory.cpp`)

### Comments and Documentation

1. **Code Comments**
   - Explain complex algorithms and data structures
   - Document function parameters and return values
   - Note important design decisions and limitations

2. **Doxygen-style Comments**
   - Use Doxygen-style comments for public APIs
   - Document classes, functions, and parameters
   - Include examples where helpful

3. **Python Documentation**
   - Use docstrings for all public functions
   - Include examples in docstrings
   - Document parameters and return values

### Error Handling

1. **C++ Error Handling**
   - Use standard C++ exceptions for runtime errors
   - Validate inputs at function boundaries
   - Provide meaningful error messages

2. **Python Error Handling**
   - Use standard Python exceptions
   - Wrap C++ exceptions in Python-appropriate exceptions
   - Provide helpful error messages

3. **Error Types**
   - `RuntimeError`: General runtime errors
   - `KeyError`: Configuration key errors
   - `ValueError`: Invalid parameter values

## Testing Procedures

### Test Framework

NLM uses a comprehensive test framework with multiple test types:

#### Unit Tests
- Test individual components in isolation
- Mock dependencies for focused testing
- High coverage of critical paths

#### Integration Tests
- Test system interactions
- Test complete workflows
- Test end-to-end functionality

#### Performance Tests
- Profile and optimize hot paths
- Test scalability with larger systems
- Validate performance optimizations

#### Regression Tests
- Automated testing of known issues
- Continuous integration testing
- Validation of fixes

### Test Organization

```
tests/
├── unit/              # Unit tests
│   ├── test_brain.cpp      # Brain component tests
│   ├── test_memory.cpp     # Memory system tests
│   └── test_plasticity.cpp # Plasticity tests
├── integration/        # Integration tests
│   ├── test_agent.cpp     # Agent integration
│   ├── test_world.cpp     # World integration
│   └── test_complete.cpp  # Complete workflow tests
├── performance/        # Performance tests
│   ├── benchmark.cpp     # Performance benchmarks
│   └── scalability.cpp   # Scalability tests
├── regression/        # Regression tests
│   ├── test_fixes.cpp     # Test known fixes
│   └── test_validation.cpp # Validation tests
└── cmake/             # CMake test configuration
    └── Test.cmake      # Test build configuration
```

### Test Development Guidelines

1. **Test Structure**
   - One test file per component
   - Test fixtures for shared resources
   - Parameterized tests for different scenarios

2. **Test Coverage**
   - Aim for 80%+ code coverage
   - Focus on critical paths and edge cases
   - Test error conditions and boundary cases

3. **Test Quality**
   - Tests should be deterministic and reproducible
   - Use known test data and expected outputs
   - Test both expected behavior and error conditions

### Testing Best Practices

1. **Test-Driven Development**
   - Write tests before implementation
   - Use tests to drive design decisions
   - Maintain high test coverage throughout development

2. **Automated Testing**
   - Integrate tests into CI/CD pipeline
   - Run tests on every code change
   - Validate performance regressions

3. **Manual Testing**
   - Visual inspection of complex behaviors
   - Manual verification of experimental results
   - Documentation of manual test procedures

### Test Example

```cpp
// Example unit test for Brain class
TEST(BrainTest, InitializationTest) {
    // Arrange
    auto config = createDefaultConfig();
    
    // Act
    Brain brain(config);
    brain.initialize();
    
    // Assert
    EXPECT_EQ(brain.getTotalNeuronCount(), config.getInt("brain.neuronCount"));
    EXPECT_FALSE(brain.isRunning());  // Should not be running until step() called
}

// Example integration test for agent-world interaction
TEST(AgentWorldIntegrationTest, CompleteWorkflow) {
    // Arrange
    auto config = createDefaultConfig();
    Brain brain(config);
    brain.initialize();
    
    SimpleWorld world;
    world.configure(10, 10, 8, 8);
    world.reset();
    
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Act
    for (int step = 0; step < 100; ++step) {
        world.update(0.1f);
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        brain.step(step);
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
    }
    
    // Assert
    EXPECT_GT(brain.getTotalSpikeCount(), 0);
    EXPECT_TRUE(agent.getCuriosityLevel() > 0.0f);
    EXPECT_TRUE(world.getSensoryPercept().getVisionWidth() == 8);
}
```

## Debugging and Troubleshooting

### Debugging Strategies

1. **Systematic Debugging**
   - Reproduce the issue consistently
   - Isolate the problematic component
   - Test hypotheses by modifying code
   - Use print statements for initial debugging

2. **Debugging Tools**
   - GDB for C++ debugging
   - Valgrind for memory debugging
   - AddressSanitizer for bug detection
   - Python's pdb for debugging

3. **Log-Based Debugging**
   - Use structured logging throughout the codebase
   - Log key decisions and state changes
   - Implement log level filtering for production

### Common Issues and Solutions

| Issue | Symptoms | Solution |
|-------|----------|----------|
| Brain not responding | No neural activity, zero firing rates | Check if `initialize()` was called before `step()` |
| Memory issues | High memory usage, crashes | Use context managers, implement proper cleanup |
| Configuration errors | KeyError, missing parameters | Validate configuration before use |
| Performance problems | Slow simulation, high CPU | Use optimized configurations, enable SIMD |
| Simulation crashes | Segmentation faults, exceptions | Implement comprehensive error handling |
| Integration failures | Components not working together | Verify interface compatibility and data types |

### Debugging Tools

#### C++ Debugging
```bash
# Run with valgrind for memory debugging
gdb ./nlm
# Inside gdb: set pagination off, break main, continue
# Use commands like: bt, frame, info locals
```

#### Python Debugging
```bash
# Use Python's debugger
python -m pdb script.py

# Or use IDE debugger (VS Code, PyCharm, etc.)
```

#### Performance Debugging
```bash
# Use performance profilers
perf record ./nlm
perf report

# Or use callgrind for detailed profiling
gdb --args valgrind --tool=callgrind ./nlm
```

### Debugging Checklist

1. **Reproduce the Issue**
   - Document the exact steps to reproduce
   - Capture error messages and stack traces
   - Note system configuration and parameters

2. **Isolate the Problem**
   - Test individual components in isolation
   - Use minimal configurations to test basic functionality
   - Verify dependencies and interfaces

3. **Analyze the Root Cause**
   - Review code logic and data flow
   - Check for null pointer dereferences
   - Validate parameter ranges and types

4. **Implement Fixes**
   - Fix the immediate issue
   - Add tests to prevent regression
   - Document the fix and lessons learned

5. **Verify the Solution**
   - Run comprehensive tests
   - Test edge cases and boundary conditions
   - Validate performance impact

## Best Practices for Contribution

### Code Contribution Guidelines

1. **Code Quality**
   - Follow project coding standards
   - Write clean, maintainable code
   - Add comments for complex algorithms
   - Use meaningful variable and function names

2. **Testing**
   - Write tests for new functionality
   - Add regression tests for bug fixes
   - Maintain high test coverage

3. **Documentation**
   - Update relevant documentation
   - Add examples and usage notes
   - Document API changes

4. **Code Review**
   - Follow project pull request process
   - Get approval from maintainers
   - Address feedback and suggestions

### Project Contribution Workflow

1. **Fork the Repository**
   - Create your own copy of the NLM repository
   - Work in your own branch

2. **Create Feature Branch**
   - Use feature/ prefix for new features
   - Use bugfix/ prefix for bug fixes
   - Keep branch names descriptive

3. **Implement Changes**
   - Write code following project standards
   - Add tests for new functionality
   - Update documentation as needed

4. **Testing and Validation**
   - Run existing test suite
   - Add new tests for your changes
   - Test in different environments

5. **Code Review**
   - Submit pull request to main repository
   - Address feedback from reviewers
   - Keep commit messages clear and descriptive

6. **Merge and Release**
   - Integrate changes into main branch
   - Update version information
   - Create release documentation

### Contribution Checklist

- [ ] Code follows project coding standards
- [ ] Tests added for new functionality
- [ ] Documentation updated
- [ ] Code reviewed by maintainers
- [ ] Tests pass in CI/CD pipeline
- [ ] No regressions introduced
- [ ] Performance impact validated

### Project Policies

#### Code Ownership

- **Core Components**: Owned by project maintainers
- **New Features**: Contributors with proper documentation and testing
- **Bug Fixes**: Contributors with validation tests

#### Licensing

- All contributions must be under MIT license
- Contributors retain copyright to their work
- Project maintains copyright for integration

#### Attribution

- Contributors are acknowledged in project documentation
- Special contributions may be noted in changelog
- Collaborative development is encouraged

## Project Lifecycle Management

### Development Planning

1. **Sprint Planning**
   - Define sprint goals and objectives
   - Prioritize features and bug fixes
   - Assign tasks to team members

2. **Implementation Planning**
   - Break down features into tasks
   - Estimate effort and resources
   - Create detailed implementation plans

3. **Testing Planning**
   - Define test strategies and approaches
   - Plan test automation and coverage
   - Schedule performance testing

### Quality Assurance

1. **Code Quality Assurance**
   - Automated code style checks
   - Code review process
   - Dependency validation

2. **Testing Quality Assurance**
   - Test automation and coverage
   - Performance validation
   - Security testing

3. **Documentation Quality Assurance**
   - Documentation style checks
   - Accuracy and completeness validation
   - User experience testing

### Release Management

1. **Release Planning**
   - Define release objectives and timeline
   - Plan feature set for each release
   - Define quality gates and criteria

2. **Release Process**
   - Code freeze before release
   - Integration testing
   - Documentation updates
   - Release candidate testing

3. **Post-Release**
   - Monitor release performance
   - Collect user feedback
   - Plan next release

## Documentation Maintenance

### Documentation Standards

1. **Technical Documentation**
   - Use consistent formatting and style
   - Include code examples where helpful
   - Cross-reference related documentation

2. **User Documentation**
   - Write for end-users, not developers
   - Include getting started guides
   - Provide troubleshooting information

3. **API Documentation**
   - Document all public APIs
   - Include parameter and return value documentation
   - Provide usage examples

### Documentation Updates

1. **Version Control**
   - Commit documentation changes with code changes
   - Maintain documentation in version control
   - Track documentation changes over time

2. **Review Process**
   - Technical review of documentation
   - User review of user-facing documentation
   - Integration testing of documentation examples

3. **Maintenance**
   - Regularly review and update documentation
   - Fix broken links and incorrect information
   - Add examples and use cases

### Documentation Example

```markdown
# NLM Configuration Documentation

## Overview

This document provides comprehensive documentation for NLM configuration options, including both command-line interface (CLI) options and configuration file formats.

## Configuration System Architecture

NLM supports multiple configuration sources with this priority order:

1. **Command-line arguments** (highest priority)
2. **Configuration file** (`.json` format)
3. **Default values** (lowest priority)

## Configuration File Format

### Basic Structure

NLM configuration files use JSON format with hierarchical keys:

```json
{
    "brain": {
        "neuronCount": 1000,
        "synapseDensity": 0.1,
        "plasticity": {
            "stdp": {
                "enabled": true,
                "learningRate": 0.001
            }
        }
    }
}
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuronCount` | int | 1000 | Total number of neurons |
| `brain.synapseDensity` | float | 0.1 | Synapse connectivity density |
| `brain.connectionProbability` | float | 0.05 | Probability of connection between neurons |

## Best Practices for Contributors

- [ ] Code follows project coding standards
- [ ] Tests added for new functionality
- [ ] Documentation updated
- [ ] Code reviewed by maintainers
- [ ] Tests pass in CI/CD pipeline
- [ ] No regressions introduced

## Further Reading

- [README.md](README.md) - Basic setup and installation
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
```

## Conclusion

This document provides comprehensive development guidelines for working with the NLM project. It covers:

1. **Development Process**: Phased development approach and project structure
2. **Coding Standards**: Code style, naming conventions, and documentation
3. **Testing Procedures**: Test framework, organization, and best practices
4. **Debugging and Troubleshooting**: Systematic debugging strategies and common issues
5. **Contribution Guidelines**: Standards for contributing to the project
6. **Project Lifecycle Management**: Planning, quality assurance, and release management
7. **Documentation Maintenance**: Documentation standards and update procedures

These guidelines ensure consistent, high-quality development across the NLM project while supporting the modular, phased development approach that has led to successful completion of all major phases.

### Key Principles

1. **Modularity**: Each phase builds on previous work while maintaining clear interfaces
2. **Integration**: Focus on connection over addition - making existing systems work together
3. **Documentation**: Comprehensive documentation for all components and procedures
4. **Testing**: Comprehensive testing at all levels to ensure reliability
5. **Collaboration**: Open contribution process with clear standards and guidelines
6. **Quality**: High standards for code quality, testing, and documentation

The NLM project demonstrates that complex systems can be successfully developed through careful, phased integration while maintaining high standards for quality, testing, and documentation.

---

*This document is continuously improved based on project experience and user feedback.*

*For questions about development guidelines, please contact the project maintainers.*

*Need help? Visit the NLM documentation or report issues on GitHub.*

*This documentation is part of the NLM research project and is provided as-is.*

*For research purposes and collaboration, please reference: NLM Research Project. (2026). NLM/熙然 - Neural Learning Machine.*

*All documentation is subject to change based on project needs and feedback.*

*Version: 1.0 (Phase 7 Development Guidelines)*
*Last Updated: 2026-10-09*

*For the most up-to-date development guidelines, please check the official NLM documentation repository.*

*Need help with development? Visit the NLM documentation or report issues on GitHub.*

*This document is part of the NLM research project and is provided as-is.*

*For research purposes and collaboration, please reference: NLM Research Project. (2026). NLM/熙然 - Neural Learning Machine.*

*All development guidelines are subject to change based on project experience.*

*Contributors are encouraged to improve and extend these guidelines.*

*Version: 1.0.0 - Initial release of development guidelines*
*License: MIT License*