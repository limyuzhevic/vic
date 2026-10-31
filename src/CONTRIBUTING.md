# NLM Contributing Guide

## Welcome to NLM Development!

Thank you for your interest in contributing to the NLM (熙然) project! This document provides comprehensive guidelines for developers who want to contribute to this experimental artificial developmental brain project.

## Getting Started

### Prerequisites

Before contributing, ensure you have:

1. **Development Environment**
   - C++17 compatible compiler (GCC 9+, Clang 10+, or MSVC 2019+)
   - CMake 3.16 or higher
   - Python 3.8 or higher
   - Git for version control

2. **Build Tools**
   ```bash
   # Clone the repository
   git clone https://github.com/NLM-project/nlm.git
   cd nlm
   
   # Create build directory
   mkdir build
   cd build
   
   # Configure and build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   make -j$(nproc)
   ```

3. **Python Environment**
   ```bash
   # Install Python bindings dependencies
   pip install pytest numpy scikit-build-core pybind11
   
   # Install pynlm package
   pip install -e .
   ```

### Project Structure

```
NLM/
├── src/                    # Source code
│   ├── brain/             # Core neural components
│   ├── cognition/         # Cognitive functions
│   ├── memory/            # Memory systems
│   ├── neuromodulation/    # Motivation systems
│   ├── plasticity/        # Learning mechanisms
│   ├── sensory/           # Sensory processing
│   ├── motor/             # Action selection
│   ├── environment/       # Environment interface
│   ├── agent/             # Agent-brain interface
│   ├── world/             # World simulation
│   ├── experiments/       # Experiment framework
│   └── visualization/     # Visualization
├── tests/                  # Unit tests
├── configs/                # Configuration files
├── docs/                   # Documentation
├── CMakeLists.txt          # Build configuration
├── pyproject.toml          # Python packaging
└── README.md               # Main documentation
```

## Development Workflow

### 1. Creating a New Feature

1. **Fork the Repository**
   ```bash
   git clone https://github.com/NLM-project/nlm.git
   cd nlm
   ```

2. **Create a Feature Branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Implement the Feature**
   - Add new source files in appropriate directory
   - Update CMakeLists.txt if needed
   - Write unit tests in tests/

4. **Test Your Changes**
   ```bash
   # Run unit tests
   cd build
   ./test_runner
   
   # Run specific tests
   ./test_runner --brain-test --neuron-test
   ```

5. **Run Demos**
   ```bash
   # Test Phase 3
   ./nlm_phase3_demo
   
   # Test Phase 4
   ./nlm_phase4_demo
   
   # Test Phase 6
   ./nlm_phase6_demo
   ```

6. **Commit Changes**
   ```bash
   git add .
   git commit -m "Add your feature"
   git push origin feature/your-feature-name
   ```

7. **Create a Pull Request**
   - Go to your repository on GitHub
   - Click on "Pull Requests"
   - Click "New Pull Request"
   - Fill in the PR description with:
     - Summary of changes
     - Motivation for the feature
     - Testing performed
     - Any known issues

8. **Review and Iterate**
   - Address reviewer comments
   - Update documentation if needed
   - Run tests again

### 2. Fixing Bugs

1. **Report the Issue**
   - Create a GitHub issue with detailed reproduction steps
   - Include minimal reproduction case
   - Provide expected vs. actual behavior

2. **Debug and Fix**
   ```bash
   # Use gdb for debugging
   gdb ./nlm_phase6_demo
   
   # Add print statements or use logging
   # Make and test your fix
   ```

3. **Test the Fix**
   - Run comprehensive tests
   - Test edge cases
   - Ensure no regression

4. **Create a Fix Branch**
   ```bash
   git checkout -b bugfix/issue-number
   ```

5. **Commit and Push**
   ```bash
   git add .
   git commit -m "Fix issue #number: brief description"
   git push origin bugfix/issue-number
   ```

6. **Create a Pull Request**
   - Reference the issue in PR description
   - Include test results

## Testing Guidelines

### Running Tests

1. **Unit Tests**
   ```bash
   cd build
   ./test_runner --help  # See all test options
   ./test_runner          # Run all tests
   ```

2. **Test Categories**
   - `brain-test`: Neural computation tests
   - `neuron-test`: Neuron dynamics tests
   - `synapse-test`: Synaptic connection tests
   - `stdp-test`: STDP learning tests
   - `clock-test`: Simulation timing tests
   - `config-test`: Configuration loading tests
   - `random-test`: Random number generation
   - `types-test`: Type system tests

3. **Python Tests**
   ```bash
   # Install test dependencies
   pip install -e .[dev]
   
   # Run Python tests
   pytest tests/
   ```

### Writing Tests

#### Unit Test Structure

```cpp
// test_example.cpp
#include <gtest/gtest.h>
#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"

TEST(NeuronTest, BasicInitialization) {
    auto config = std::make_shared<Config>();
    Neuron neuron(config);
    
    EXPECT_FLOAT_EQ(neuron.getMembranePotential(), 0.0f);
    EXPECT_FLOAT_EQ(neuron.getThreshold(), 1.0f);
}

TEST(NeuronTest, CurrentInjection) {
    auto config = std::make_shared<Config>();
    Neuron neuron(config);
    neuron.initialize();
    
    neuron.injectCurrent(10.0f);
    // Simulate a few steps
    for (int i = 0; i < 10; ++i) {
        neuron.update(0.001);
    }
    
    EXPECT_TRUE(neuron.isFiring());
}
```

#### Python Test Structure

```python
# test_python_example.py
import pynlm
import pytest

def test_brain_creation():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    assert brain.getTotalNeuronCount() > 0
    assert brain.getTotalSpikeCount() == 0

def test_brain_simulation():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(100):
        brain.step(step)
    
    assert brain.getTotalSpikeCount() > 0
    assert brain.getAverageFiringRate() > 0
```

#### Testing Best Practices

1. **Test Coverage**
   - Aim for >90% code coverage
   - Test both success and error cases
   - Test edge cases and boundary conditions

2. **Test Isolation**
   - Each test should be independent
   - Use setup/teardown for test environment
   - Mock dependencies when testing

3. **Test Performance**
   - Tests should run in reasonable time
   - Use fixtures for expensive setup
   - Consider test parameterization

4. **Test Reliability**
   - Use deterministic tests where possible
   - Include assertions, not just prints
   - Use proper test frameworks

## Code Standards and Best Practices

### C++ Code Style

#### Naming Conventions
- **Classes**: PascalCase (`Brain`, `Neuron`, `Synapse`)
- **Functions**: camelCase (`initialize()`, `step()`, `getSpikes()`)
- **Variables**: snake_case (`neuron_count`, `simulation_timestep`)
- **Constants**: UPPER_SNAKE_CASE (`MAX_NEURONS`, `PI_VALUE`)
- **Namespaces**: `nlm`, `nlm::brain`, `nlm::memory`

#### File Organization
```cpp
// src/brain/Neuron.hpp
#ifndef NLM_BRAIN_NEURON_HPP
#define NLM_BRAIN_NEURON_HPP

#include "core/Config/Config.hpp"

namespace nlm {
    class Neuron {
    public:
        Neuron(const std::shared_ptr<Config>& config);
        ~Neuron();
        
        void initialize();
        void update(double time);
        void injectCurrent(float current);
        
        float getMembranePotential() const;
        float getThreshold() const;
        NeuronType getNeuronType() const;
        bool isFiring() const;
        
    private:
        // Implementation details
    };
}

#endif // NLM_BRAIN_NEURON_HPP
```

#### Memory Management
- Use smart pointers (`std::shared_ptr`, `std::unique_ptr`)
- Implement RAII for all resources
- Avoid raw pointers in public interfaces
- Be aware of memory pools for performance

#### Error Handling
```cpp
// Error handling best practices
bool riskyOperation() {
    // Validate inputs
    if (invalidInput) {
        NLM_LOG_ERROR("Invalid input: " + std::to_string(value));
        return false;
    }
    
    // Check resource availability
    if (!resourceAvailable()) {
        NLM_LOG_WARNING("Resource temporarily unavailable");
        return false;
    }
    
    try {
        // Perform operation
        performOperation();
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Operation failed: " + std::string(e.what()));
        return false;
    }
}
```

### Python Code Style

#### PEP 8 Compliance
- Use 4-space indentation
- Maximum line length: 79 characters
- Name imports on separate lines
- Use descriptive variable names

#### Python Binding Best Practices
```python
# Good: Clear parameter descriptions
import pynlm

def run_simulation(brain, world, steps=100):
    """Run a complete simulation with brain-world interaction.
    
    Args:
        brain: Neural brain instance
        world: Environment instance  
        steps: Number of simulation steps
        
    Returns:
        dict: Simulation statistics
    """
    results = {
        'steps': steps,
        'spikes': brain.getTotalSpikeCount(),
        'neurons': brain.getTotalNeuronCount(),
        'firing_rate': brain.getAverageFiringRate()
    }
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        brain.step(step)
        # ... rest of simulation
    
    return results

# Bad: Poor documentation and error handling
def bad_func(brain):  # Should use descriptive name
    x = 5  # Unclear purpose
    try:
        brain.step(x)  # No error handling
    except:  # Should catch specific exceptions
        pass
```

## Building and Distribution

### Building from Source

#### Standard Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

#### Development Build
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_ENABLE_TESTS=ON
make -j$(nproc)
```

#### Python Package Build
```bash
cd ..
pip install -e .
```

### Building Documentation

```bash
# Build with documentation
mkdir build
cd build
cmake .. -DBUILD_DOCS=ON
make docs
```

### Running Demos

#### Phase 3 Demo (Basic Neural Computation)
```bash
./nlm_phase3_demo
# Demonstrates: neuron dynamics, spike propagation, STDP learning
```

#### Phase 4 Demo (Advanced Cognitive Functions)
```bash
./nlm_phase4_demo
# Demonstrates: working memory, episodic memory, planning
```

#### Phase 6 Demo (Full Integration)
```bash
./nlm_phase6_demo
# Demonstrates: complete brain integration and behavior
```

### Running with Custom Configuration

Create a config file `my_config.cfg`:
```ini
[neuron_settings]
neuron_count = 1000
region_count = 2
connection_probability = 0.2

[plasticity_settings]
stdp_ltp_weight = 0.025
stdp_ltd_weight = 0.02
stdp_tau = 25.0

[development_settings]
synaptogenesis_rate = 0.001
pruning_rate = 0.000001
```

Run with custom config:
```bash
./nlm_phase6_demo --config my_config.cfg
```

## Python Environment

### Installing Dependencies

#### Development Environment
```bash
# Core dependencies (in pyproject.toml)
# Optional dev dependencies
pip install pytest>=7.0 numpy>=1.20
```

#### Runtime Environment
```bash
# Only core runtime dependencies needed
pip install pynlm
```

### Python API Usage

#### Basic Usage
```python
import pynlm

# Create brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Brain statistics:")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Spikes: {brain.getTotalSpikeCount()}")
print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

#### Complete Example (Agent in World)
```python
import pynlm

def run_agent_simulation(steps=200):
    """Complete agent simulation with learning."""
    
    # Create and initialize components
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Periodic status output
        if step % 50 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  Development: {brain.getDevelopmentalStage()}")
            print()
    
    return brain

# Run the simulation
simulation_result = run_agent_simulation(500)
```

#### Error Handling
```python
import pynlm

def safe_simulation_run():
    """Run simulation with proper error handling."""
    try:
        # Create components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        return brain
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        # Clean up resources
        if 'brain' in locals():
            brain.reset()
        if 'world' in locals():
            world.reset()
        raise

# Usage
try:
    result = safe_simulation_run()
    print("Simulation completed successfully!")
except Exception as e:
    print(f"Simulation error: {e}")
```

## Documentation Standards

### README Requirements
Each new component or feature should have:

1. **Purpose Section**: What the component does
2. **Features Section**: Key capabilities
3. **Usage Examples**: Code examples
4. **Configuration**: Optional settings
5. **Dependencies**: Required components
6. **Limitations**: Known constraints

### Code Documentation
- Use Doxygen-style comments for C++
- Include parameter types and descriptions
- Document return values and exceptions
- Add usage examples where helpful

### Markdown Documentation
- Use consistent heading hierarchy (H1, H2, H3)
- Include code blocks with language specification
- Use tables for comparisons and parameters
- Include diagrams where helpful

## Version Control

### Branching Strategy
- **Main**: Production-ready code
- **Development**: Current development work
- **feature/**: Feature branches
- **bugfix/**: Bug fix branches
- **hotfix/**: Emergency fixes

### Commit Message Guidelines
```
<type>(<scope>): <description>

<optional body>

<optional footer>
```

#### Commit Types
- **feat**: New feature
- **fix**: Bug fix
- **docs**: Documentation changes
- **style**: Code style changes (no logic)
- **refactor**: Code refactoring
- **test**: Adding tests
- **chore**: Maintenance tasks

#### Examples
```
feat(memory): add working memory system
  - Implements persistent activity neural network
  - Integrates with existing brain architecture
  - Includes comprehensive unit tests

fix(neuromodulation): STDP timing parameter error
  - Correct timing calculation for spike pairing
  - Resolves learning effectiveness issue
  - Adds test coverage for edge cases

docs(src/core): update configuration documentation
  - Documents new config options
  - Adds examples for complex settings
  - Updates parameter descriptions
```

### Pull Request Standards

#### Required Information
- **Title**: Clear, descriptive title
- **Description**: Detailed explanation of changes
- **Testing**: Summary of tests performed
- **Screenshots**: Visual evidence where helpful
- **Impact**: Effect on existing functionality

#### Review Checklist
- [ ] All tests pass
- [ ] Code follows project style
- [ ] Documentation updated
- [ ] No security vulnerabilities
- [ ] Performance acceptable
- [ ] Compatible with existing API

## Performance Optimization

### Profiling and Benchmarking

#### Profiling Tools
```bash
# Linux/GCC profiling
gcc -O2 -g -pg -o program program.cpp
./program
profiles/gmon.out
gprof program profiles/gmon.out > report.txt

# Valgrind memory analysis
valgrind --leak-check=full ./program
```

#### Performance Metrics
- **Neuron updates**: Time per neuron per timestep
- **Spike propagation**: Latency and throughput
- **Memory usage**: Peak RAM and allocation frequency
- **I/O operations**: Read/write speeds and patterns

### Optimization Guidelines

#### Memory Optimization
- Use `std::vector` with capacity pre-allocation
- Consider memory pools for fixed-size objects
- Use const references to avoid copying
- Profile memory allocation patterns

#### CPU Optimization
- Loop unrolling for small, tight loops
- Use SIMD instructions for numeric operations
- Minimize function call overhead in hot paths
- Cache frequently accessed data

#### Data Structure Optimization
```cpp
// Good: Cache-friendly layout
struct Neuron {
    float membranePotential;  // First member
    float threshold;
    float refractoryTime;
    NeuronId id;
    // ... other fields
};

// Bad: Poor cache locality
struct Neuron {
    NeuronId id;
    // ... other fields
    float membranePotential;  // Far from first members
    // ... more fields
};
```

## Security Considerations

### Safe Coding Practices
- Validate all external inputs
- Use safe string operations
- Check return values of system calls
- Use address-safe functions

### Code Review Security Checklist
- [ ] Input validation implemented
- [ ] Buffer overflows prevented
- [ ] Memory safety ensured
- [ ] Race conditions eliminated
- [ ] Resource leaks fixed
- [ ] Privilege escalation prevented

## Community Guidelines

### Code of Conduct
- Be respectful and inclusive
- Focus on technical details
- Provide constructive feedback
- Help others learn and grow

### Communication
- Use clear, concise language
- Document assumptions and decisions
- Ask questions when unsure
- Share credit appropriately

## License and Attribution

### License
This project is licensed under the MIT License. See LICENSE file for details.

### Attribution
When using or referencing NLM code:
```
Based on NLM (熙然) - Neural Learning Machine
https://github.com/NLM-project/nlm
License: MIT
```

## Getting Help

### Online Resources
- **GitHub Issues**: Bug reports and feature requests
- **Documentation**: Read the docs/ directory
- **Discussions**: Community Q&A
- **Examples**: demo programs and tutorials

### Local Support
- **README files**: Project setup and usage
- **API documentation**: Method details and examples
- **Component guide**: System architecture
- **Source code**: Implementation details

### Troubleshooting

#### Common Issues

**Issue**: Build fails with CMake
```bash
# Solution: Update CMake
cmake --version
cmake --help
```

**Issue**: Python bindings not found
```bash
# Solution: Install with development dependencies
pip install -e .[dev]

# Or install system-wide
pip install pynlm
```

**Issue**: Tests fail
```bash
# Solution: Run tests with verbose output
./test_runner --verbose

# Or run specific test
./test_runner --brain-test --neuron-test
```

## Support and Maintenance

### Maintenance Responsibilities
- Regular dependency updates
- Performance monitoring
- Documentation maintenance
- Test suite expansion
- Security audits

### Getting Support
- Check existing documentation first
- Report issues with reproduction steps
- Provide environment and system information
- Share debugging output when possible

This guide provides comprehensive instructions for contributing to the NLM project. By following these guidelines, we can maintain code quality, ensure reliable testing, and foster a productive development environment.
