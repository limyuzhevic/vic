# NLM Development Guide

## Overview

This guide provides detailed information for developers working on the NLM project, including setup instructions, development workflows, and technical details.

## Project Structure

### Core Components

```
src/
├── core/           # Core utilities and infrastructure
├── agent/          # Agent-brain interface
├── world/          # Simple world simulation
├── brain/          # Neural components (integrated)
├── dynamics/       # Neural dynamics
├── plasticity/     # Plasticity rules
├── development/    # Developmental system
├── neuromodulation/ # Neuromodulators (integrated)
├── memory/        # Memory systems (integrated)
├── prediction/     # Prediction systems (integrated)
├── cognition/      # Cognitive mechanisms (integrated)
├── sensory/       # Sensory processing
├── motor/         # Motor system
├── environment/   # Environment interface
├── experiments/   # Experiment framework (Phase 6)
└── visualization/  # Visualization
```

### Python Bindings

```
python/
├── bindings.cpp     # C++ to Python bindings
└── CMakeLists.txt    # Python bindings build configuration
```

### Configuration

```
configs/
└── default.cfg       # Default configuration file
```

### Documentation

```
docs/
├── ARCHITECTURE.md   # System architecture overview
├── SCIENCE.md        # Scientific background
├── ROADMAP.md        # Development roadmap
├── EXPERIMENTS.md    # Experiment descriptions
├── LIMITATIONS.md    # Current limitations
└── PHASE6_FINAL_AUDIT.md  # Phase 6 final audit
```

### Build System

```
CMakeLists.txt        # Main CMake configuration
build/                # Build output directory
```

## Building from Source

### Prerequisites

```bash
# Ubuntu/Debian
apt-get update
apt-get install -y g++ cmake python3 python3-pip python3-venv

# macOS
brew install cmake python3

# For Python bindings
apt-get install -y pybind11-dev
```

### Build Instructions

#### Standard C++ Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Python Bindings Build

```bash
# Create virtual environment
python3 -m venv venv
source venv/bin/activate

# Install dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build Python bindings
pip install -e .
```

#### Build Options

```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# With Python bindings
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON

# Custom install prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

## Running Tests

### C++ Tests

```bash
# Run unit tests
cd build
./nlm_test

# Run integration tests
ctest --output-on-failure

# Run phase-specific tests
./nlm_phase3_demo --help  # Check available options
./nlm_phase4_demo --help
./nlm_phase6_demo --help
```

### Python Tests

```bash
source venv/bin/activate
pytest tests/  # If Python tests exist
```

### Custom Testing Modes

```bash
# Test learning behavior
./nlm_test --test-learning

# Test plasticity
./nlm_test --test-plasticity

# Test world interaction
./nlm_phase3_demo --test-mode

# Run full integration
./nlm_phase6_demo --integration-test
```

## Development Workflow

### 1. Setting Up Your Development Environment

```bash
# Clone the repository
git clone <repository-url>
cd nlm

# Create development branch
git checkout -b feature/your-feature-name

# Build and test
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
./nlm_test
```

### 2. Coding Standards

#### C++ Style

- **Language**: C++20
- **Indentation**: 2 spaces
- **Line length**: 100 characters maximum
- **Braces**: K&R style
- **Comments**: Use `///` for documentation
- **Naming**: snake_case for variables/functions
- **Headers**: Include guards with `_HPP` suffix
- **Namespaces**: Use `nlm::` namespace

#### Python Style

- **Language**: Python 3.8+
- **Indentation**: 4 spaces
- **Line length**: 79 characters maximum
- **Naming**: snake_case for functions/variables
- **Documentation**: Use docstrings

#### Commit Messages

```
feat: add new neural plasticity rule
fix: correct learning rate calculation
docs: update user guide with new examples
```

### 3. Testing Strategy

#### Unit Testing

- Each brain component should have corresponding unit tests
- Test edge cases and error conditions
- Mock dependencies when testing independently
- Ensure tests are deterministic (set seeds)

#### Integration Testing

- Phase 3: Test world navigation and interaction
- Phase 4: Test memory and prediction accuracy
- Phase 5: Test performance optimizations
- Phase 6: Test full brain integration

#### Performance Testing

- Use benchmarks to measure simulation performance
- Track memory usage over time
- Compare before/after performance for optimizations

### 4. Debugging

#### Common Debugging Techniques

```bash
# Use GDB for debugging
cd build
gdb ./nlm_test
# or
gdb ./nlm_phase3_demo
```

#### Logging

The NLM system uses a comprehensive logging framework:

```cpp
NLM_LOG_INFO("Message: %d", value);
NLM_LOG_ERROR("Error: %s", msg.c_str());
```

Log levels:
- DEBUG: Detailed debugging information
- INFO: Normal operational messages
- WARNING: Potentially problematic situations
- ERROR: Error conditions

#### Profiling

```bash
# Use valgrind for memory debugging
valgrind --leak-check=full ./nlm_test

# Use perf for performance analysis
perf record ./nlm_test
perf report
```

### 5. Performance Optimization

#### Current Performance Areas

1. **Neural computation**: Spiking dynamics calculation
2. **Memory management**: Memory pool usage
3. **Plasticity computation**: STDP and Hebbian learning
4. **World simulation**: Agent-environment interaction

#### Optimization Strategies

- Use SIMD instructions for vector operations
- Minimize dynamic allocations in hot paths
- Cache frequently accessed data
- Use constexpr where possible
- Profile before optimizing

## Advanced Development

### 1. Extending the Brain

#### Adding New Neuron Types

1. Create new neuron class inheriting from base Neuron
2. Implement update() method with specific dynamics
3. Update brain configuration to support new type
4. Add Python bindings if needed

#### New Plasticity Rules

1. Implement new plasticity rule class
2. Register rule with brain's plasticity system
3. Update configuration for rule parameters
4. Test with existing experiments

### 2. Custom Worlds

#### Creating New Environments

1. Implement Environment interface
2. Override sensing and action methods
3. Configure world parameters
4. Test with agent brain

### 3. Python Extensions

#### Creating Custom Python Modules

```python
import pynlm
import numpy as np

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add custom Python functionality
class CustomAgent:
    def __init__(self, brain, world):
        self.brain = brain
        self.world = world
    
    def run_episode(self, steps):
        # Custom agent behavior
        for step in range(steps):
            # Custom logic here
            pass
```

### 4. Advanced Configuration

#### Configuration Management

```cpp
// Load configuration
auto config = std::make_shared<Config>();
config->loadFromFile("my_config.json");

// Modify configuration
config->set("brain.neuron_count", 2000);
config->set("plasticity.stdp.enable", true);

// Validate configuration
if (!config->validate()) {
    // Handle invalid configuration
}
```

## Phase Development Process

### Phase 1: Foundation

- Core data structures and types
- Basic configuration system
- Simple brain interface
- Documentation and examples

### Phase 2: Neural Computation

- LIF neuron dynamics
- Event-driven spike propagation
- Basic plasticity rules (STDP, Hebbian)
- Structural plasticity

### Phase 3: World Interaction

- Sensory processing
- Motor output
- Reward prediction error
- Developmental stages

### Phase 4: Cognitive Integration

- Neural prediction system
- Working memory
- Episodic memory
- Concept formation
- Attention and planning

### Phase 5: Performance Optimization

- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

### Phase 6: Full Integration

- All systems working together
- Comprehensive testing
- Performance benchmarks
- User documentation

## Troubleshooting

### Common Issues

#### Build Failures

```bash
# Common causes and solutions

# Missing dependencies
Check CMake output for missing libraries
cmake .. -DCMAKE_PREFIX_PATH=/usr/local

# Python bindings
pip install pybind11 scikit-build-core
pip install -e .  # Reinstall with proper setup
```

#### Runtime Errors

```bash
# Segmentation faults
options:
1. Run with valgrind: valgrind ./executable
2. Add assertions: assert(pointer != nullptr)
3. Check null pointers before use
```

#### Test Failures

```bash
# Check test output for clues
cd build
./nlm_test --verbose

# Run specific tests
ctest -R "test_name"
```

### Getting Help

1. **GitHub Issues**: Create an issue with detailed reproduction steps
2. **Discussions**: Ask questions in the discussion forum
3. **Code Review**: Request code review for complex changes
4. **Mentoring**: Work with experienced maintainers

## Best Practices

### Code Quality

- **Modularity**: Keep components focused and independent
- **Testability**: Write testable code with clear interfaces
- **Performance**: Profile before optimizing
- **Documentation**: Keep code and docs in sync
- **Error Handling**: Validate inputs and handle errors gracefully

### Collaboration

- **Code Reviews**: Review changes for quality and consistency
- **Documentation**: Update docs when modifying functionality
- **Testing**: Ensure comprehensive test coverage
- **Backward Compatibility**: Consider impact on existing users

### Maintainability

- **Comments**: Explain complex algorithms and design decisions
- **Version Control**: Use meaningful commit messages
- **Refactoring**: Clean up technical debt regularly
- **Architecture**: Maintain clear separation of concerns

## Future Directions

### Research Areas

1. **Advanced Neuromodulation**: More sophisticated reward systems
2. **Learning Algorithms**: Better generalization from experience
3. **Developmental Processes**: More realistic maturation
4. interdisciplinarity: Integration with cognitive science

### Technical Improvements

1. **Scalability**: Support for larger neural networks
2. **Real-time Performance**: Faster simulation speeds
3. **Visualization**: Better tools for understanding brain activity
4. **Education**: Simplified interfaces for teaching

## Conclusion

NLM is an evolving research project that benefits from contributions from the open source community. By following these development guidelines, we can maintain code quality, ensure comprehensive testing, and continue advancing our understanding of artificial developmental brains.

Remember: Even small contributions can make a big difference in research projects!
