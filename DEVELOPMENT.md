# NLM Development Guide

This document provides comprehensive guidelines for developing, testing, and contributing to the NLM (Neural Learning Machine) project.

## Overview

NLM is an experimental computational brain project that simulates neural dynamics, learning, and cognition. This guide covers:

- Building and running NLM
- Testing procedures
- Code quality standards
- Contribution guidelines
- Performance optimization
- Debugging and troubleshooting

## Table of Contents

1. [Build Instructions](#build-instructions)
2. [Testing Procedures](#testing-procedures)
3. [Code Quality Standards](#code-quality-standards)
4. [Contribution Guidelines](#contribution-guidelines)
5. [Performance Optimization](#performance-optimization)
6. [Debugging and Troubleshooting](#debugging-and-troubleshooting)
7. [Development Best Practices](#development-best-practices)
8. [Continuous Integration](#continuous-integration)
9. [Documentation Standards](#documentation-standards)
10. [Migration Guides](#migration-guides)

## Build Instructions

### Prerequisites

#### Linux/macOS
```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake git g++ make

# For Ubuntu/Debian:
sudo apt-get install -y libboost-all-dev

# For RHEL/CentOS:
sudo yum install -y gcc-g++ cmake make git

# For Fedora:
sudo dnf install -y gcc-g++ cmake make git
```

#### Windows
```powershell
# Install Visual Studio with C++ support
# Install Chocolatey package manager
# Install CMake via Chocolatey
choco install cmake

# Clone the repository and build
# git clone https://github.com/nlm-project/nlm.git
cd nlm
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64

# Or using CMake with MinGW
choco install mingw
# Use MinGW's cmake or download from cmake.org
```

### Building NLM

#### Standard Build
```bash
# Create build directory and configure
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with parallel jobs (adjust as needed)
make -j$(nproc)
```

#### Build with Specific Options
```bash
# Build with Python bindings
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/usr/bin/python3
make -j$(nproc)

# Build with debugging information
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Build with optimizations for specific CPU features
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -march=native -mtune=native"
make -j$(nproc)
```

#### Build for Python Package
```bash
# Install scikit-build-core if not already installed
pip install scikit-build-core pybind11

# Configure for Python bindings
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/usr/bin/python3

# Build Python module
python -m pip install .
```

### Running NLM

#### Basic Execution
```bash
# Run the main NLM simulation
./nlm

# Run with custom configuration
./nlm --config my_config.json

# Run with verbose output
./nlm --verbose

# Run with custom timestep
./nlm --simulation_timestep 0.0005
```

#### Phase 4 Experiments
```bash
# Run Phase 4 integration experiments
./nlm_phase4_demo [num_trials]

# Run with specific number of trials
./nlm_phase4_demo 100

# Run with custom configuration
./nlm_phase4_demo --config phase4_config.json
```

#### Phase 6 Integration Demo
```bash
# Run Phase 6 integration demo
./nlm_phase6_demo

# Run with custom configuration
./nlm_phase6_demo --config phase6_config.json
```

### Installation

#### System Installation
```bash
# Install system-wide
sudo make install

# Or install to custom location
make install DESTDIR=/opt/nlm
```

#### Python Package Installation
```bash
# Install Python package
pip install .

# Or install from source with specific options
pip install --no-build-isolation --no-deps scikit-build-core pybind11
python setup.py build_ext --inplace
```

## Testing Procedures

### Unit Tests

#### Running Unit Tests
```bash
# Run all unit tests
cd tests
pytest

# Run specific test modules
pytest test_brain.py
pytest test_neuron.py
pytest test_synapse.py

# Run tests with verbose output
pytest -v

# Run tests with coverage reporting
pytest --cov=nlm --cov-report=html
```

#### Test Structure

NLM uses pytest for testing. Tests are organized as follows:

1. **Unit Tests** (`test_*.py`):
   - Test individual components
   - Mock external dependencies
   - Focus on specific functionality

2. **Integration Tests** (`integration_*.py`):
   - Test component interactions
   - System-level behavior
   - End-to-end scenarios

3. **Performance Tests** (`perf_*.py`):
   - Performance benchmarks
   - Stress testing
   - Scalability analysis

#### Writing Tests

##### Test File Structure
```python
import pytest
from nlm.brain import Brain
from nlm.neuron import Neuron
from nlm.synapse import Synapse
from nlm.config import Config

class TestBrainInitialization:
    """Test brain initialization functionality"""
    
    def test_brain_initialization(self):
        """Test basic brain initialization"""
        config = Config()
        config.set("neuron_count", 1000)
        config.set("region_count", 1)
        
        brain = Brain(config)
        assert brain.initialize()
        assert brain.getRegionCount() == 1
        assert brain.getTotalNeuronCount() == 1000
    
    def test_invalid_configuration(self):
        """Test brain initialization with invalid configuration"""
        config = Config()
        config.set("neuron_count", 0)  # Invalid: zero neurons
        
        brain = Brain(config)
        assert not brain.initialize()  # Should fail

class TestNeuronFunctionality:
    """Test neuron-specific functionality"""
    
    def test_neuron_creation(self):
        """Test neuron creation"""
        neuron = Neuron(1)
        assert neuron.getId() == 1
        assert neuron.getType() == "Internal"
        assert not neuron.isFiring()
    
    def test_neuron_firing(self):
        """Test neuron firing behavior"""
        neuron = Neuron(1)
        neuron.setType("Sensory")
        neuron.injectCurrent(50.0)  # Strong excitation
        
        # Step neuron (simulate)
        neuron.step(0.0, 0.001)
        
        # Neuron should fire if threshold is reached
        # (actual behavior depends on neuron dynamics)

class TestConfiguration:
    """Test configuration loading and validation"""
    
    def test_config_from_file(self):
        """Test configuration loading from file"""
        config = Config()
        assert config.loadFromFile("test_config.json")
        assert config.get("neuron_count").value == 2000
    
    def test_config_validation(self):
        """Test configuration validation"""
        config = Config()
        config.set("neuron_count", -100)  # Invalid
        config.set("simulation_timestep", 0.0)  # Invalid
        
        # Configuration should accept the values but system may validate
        assert config.has("neuron_count")
        assert config.get("simulation_timestep").value == 0.0
```

##### Testing Best Practices

1. **Isolate Tests**: Each test should be independent and not rely on external state
2. **Mock Dependencies**: Use mocking for external dependencies (file I/O, network, etc.)
3. **Test Edge Cases**: Test boundary conditions and error cases
4. **Test Performance**: Include performance benchmarks where appropriate
5. **Clean Up**: Ensure tests clean up resources (files, memory, etc.)

### Integration Tests

#### Integration Test Structure
```python
import pytest
from nlm.brain import Brain
from nlm.environment import SimpleWorld
from nlm.agent import Agent

class TestBrainEnvironmentIntegration:
    """Test brain-environment integration"""
    
    def setup_method(self):
        """Setup before each test"""
        self.config = Config()
        self.config.set("neuron_count", 500)
        self.brain = Brain(self.config)
        self.brain.initialize()
        
        self.world = SimpleWorld()
        self.agent = Agent(self.brain)
    
    def teardown_method(self):
        """Cleanup after each test"""
        if hasattr(self, 'brain'):
            self.brain.reset()
    
    def test_sensorimotor_loop(self):
        """Test sensorimotor loop integration"""
        # Create sensory input
        input_data = [0.5, 0.8, 0.3]
        sensory_input = SensoryInput(input_data)
        
        # Process sensory input
        self.brain.receiveSensoryInput(sensory_input)
        
        # Generate action
        action = self.brain.produceAction()
        assert action is not None
        
        # Apply action to world
        result = self.world.applyAction(action)
        assert result is not None
    
    def test_memory_integration(self):
        """Test memory system integration"""
        # Store something in working memory
        self.brain.getWorkingMemory().store(123, 0.8)
        
        # Retrieve from memory
        retrieved = self.brain.getWorkingMemory().retrieve(123)
        assert abs(retrieved - 0.8) < 0.001
```

#### Performance Testing
```python
import time
import pytest
from nlm.brain import Brain
from nlm.config import Config

class TestBrainPerformance:
    """Test brain performance characteristics"""
    
    def setup_method(self):
        """Setup for performance tests"""
        self.config = Config()
        self.config.set("neuron_count", 1000)
        self.config.set("region_count", 2)
    
    def test_initialization_performance(self):
        """Test brain initialization performance"""
        start_time = time.time()
        
        brain = Brain(self.config)
        brain.initialize()
        
        elapsed = time.time() - start_time
        
        # Should initialize within reasonable time
        assert elapsed < 10.0  # 10 seconds
        
        print(f"Brain initialization time: {elapsed:.3f}s")
    
    def test_simulation_performance(self):
        """Test simulation performance"""
        config = self.config.copy()
        config.set("neuron_count", 2000)  # Larger network for performance test
        
        brain = Brain(config)
        brain.initialize()
        
        # Run simulation for a fixed number of steps
        start_time = time.time()
        steps = 1000
        
        for step in range(steps):
            brain.step(step, step * config.get("simulation_timestep"))
        
        elapsed = time.time() - start_time
        
        # Calculate performance
        steps_per_second = steps / elapsed
        assert steps_per_second > 100  # Should process at least 100 steps/second
        
        print(f"Simulation performance: {steps_per_second:.1f} steps/second")
```

### Test Coverage

#### Coverage Metrics
```bash
# Generate coverage report
pytest --cov=nlm --cov-report=html --cov-report=term-missing

# Check coverage thresholds
pytest --cov=nlm --cov-threshold=80

# Generate coverage XML for CI
pytest --cov=nlm --cov-report=xml --cov-report=term
```

#### Coverage Requirements

- **Core Components** (Brain, Neuron, Synapse): > 90% coverage
- **Configuration System**: > 85% coverage
- **Memory Systems**: > 80% coverage
- **Plasticity Systems**: > 85% coverage
- **Integration Tests**: > 70% coverage

### Continuous Testing

#### GitHub Actions
Create `.github/workflows/` directory with:

```yaml
# .github/workflows/test.yml
name: NLM Tests

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM

jobs:
  test:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        python-version: [3.8, 3.9, "3.10", "3.11"]
        build-type: [Release, Debug]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python ${{ matrix.python-version }}
      uses: actions/setup-python@v3
      with:
        python-version: ${{ matrix.python-version }}
    
    - name: Install dependencies
      run: |
        pip install scikit-build-core pybind11 pytest pytest-cov
        pip install -e .
    
    - name: Build and test
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build-type }}
        make -j$(nproc)
        cd ..
        pytest --cov=nlm --cov-report=xml
    
    - name: Upload coverage
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: coverage-${{ matrix.python-version }}-${{ matrix.build-type }}
        path: coverage.xml
```

## Code Quality Standards

### Naming Conventions

#### Variables and Functions
- Use camelCase for variables and functions
- Use PascalCase for classes and enums
- Use UPPER_SNAKE_CASE for constants

```python
# Examples
def processNeuronInputs(input_data, time_step):
    """Process neuron inputs with given timestep"""
    # Function implementation

class NeuralNetwork:
    """Represents a neural network"""
    
    def __init__(self, config):
        # Constructor implementation

CONSTANT_VALUE = 1.0  # Constant
```

#### File Naming
- Source files: `filename.cpp`, `filename.hpp`
- Test files: `test_filename.py`, `integration_filename.py`
- Configuration files: `filename.json`, `filename.yaml`, `filename.cfg`
- Documentation files: `README.md`, `CONTRIBUTING.md`, `API.md`

### Code Style

#### Indentation
- Use 4 spaces per indentation level
- No tabs
- Maximum line length: 120 characters

#### Comments
- Use Javadoc-style comments for public methods
- Document parameters, return values, and exceptions
- Include example usage where helpful

```python
def processInput(self, input_data, timestamp):
    """Process external sensory input.
    
    Args:
        input_data: List of sensory input values
        timestamp: Time when input was received
        
    Returns:
        bool: True if input was processed successfully
        
    Raises:
        ValueError: If input data is invalid
    """
    # Implementation
```

#### Error Handling
- Use exceptions for error reporting
- Provide clear error messages
- Include error codes where appropriate

```python
def criticalOperation(self):
    """Perform critical operation that may fail"""
    try:
        # Perform operation
        result = self.doOperation()
        return result
    except std::runtime_error& e:
        # Handle runtime errors
        self.logger.error("Runtime error in critical operation: " + std::string(e.what()));
        throw;
    except std::exception& e:
        # Handle all other exceptions
        self.logger.error("Unexpected error in critical operation: " + std::string(e.what()));
        throw std::runtime_error("Critical operation failed");
```

### Refactoring Guidelines

#### Code Smells to Watch For

1. **Large Functions**: Split functions longer than 50 lines
2. **Deep Nesting**: Reduce nesting levels below 4
3. **Duplicate Code**: Extract common functionality into reusable functions
4. **Magic Numbers**: Replace with named constants
5. **Long Parameter Lists**: Use structs or classes for many parameters

#### Refactoring Process

1. Identify code smells
2. Prioritize by impact and frequency
3. Create tests before refactoring
4. Make small, focused changes
5. Verify functionality after each change

## Contribution Guidelines

### Getting Started

1. **Fork the Repository**: Create your own copy of the NLM repository
2. **Set Up Development Environment**: Install dependencies and set up build system
3. **Understand the Architecture**: Read the documentation and code to understand the system
4. **Choose an Issue**: Pick an issue from the repository that matches your skills
5. **Create a Branch**: Create a feature branch for your work

### Making Changes

1. **Follow the Existing Style**: Use the project's coding conventions
2. **Write Tests**: Add tests for new functionality and existing functionality
3. **Document Changes**: Update documentation and add comments
4. **Update Documentation**: Update any affected documentation files
5. **Commit Changes**: Use descriptive commit messages
6. **Create Pull Request**: Submit your changes for review

### Code Review Process

1. **Self-Review**: Review your own changes before submitting
2. **Peer Review**: Have another developer review your changes
3. **Testing Review**: Ensure all tests pass
4. **Documentation Review**: Verify documentation is complete and accurate
5. **Merge Review**: Final review by maintainers before merging

### Pull Request Template

```markdown
## Description
Brief description of the changes

## Changes Made
- List of files modified
- Description of changes

## Testing
- Tests added/modified
- Test results

## Breaking Changes
- Any breaking changes to the API
- Migration guide if needed

## Performance Impact
- Performance improvements or degradation
- Profiling results if applicable

## Documentation
- Documentation updated
- Examples added if applicable

## Known Issues
- Any remaining issues or limitations
```

## Performance Optimization

### General Optimization Strategies

1. **Memory Management**
   - Use move semantics where appropriate
   - Minimize dynamic allocations in hot paths
   - Use object pooling for frequently created objects

2. **CPU Optimization**
   - Minimize function calls in tight loops
   - Use local variables instead of member variables where possible
   - Optimize data structures for cache locality

3. **Parallelization**
   - Use SIMD instructions for vector operations
   - Parallelize independent computations
   - Use concurrent data structures where appropriate

### NLM-Specific Optimizations

1. **Neuron Dynamics**
   - Optimize LIF neuron computation
   - Use lookup tables for common functions
   - Vectorize spike detection

2. **Synaptic Processing**
   - Optimize synaptic weight updates
   - Use efficient data structures for connectivity
   - Parallelize synaptic input computation

3. **Memory Systems**
   - Optimize working memory access patterns
   - Use efficient algorithms for memory consolidation
   - Minimize memory copy operations

### Profiling and Benchmarking

#### Using Performance Tools
```bash
# GProf (GCC profiling)
g++ -pg -O2 -o nlm_main main.cpp
sudo ./nlm_main
pprof --text nlm_main gmon.out > profile.txt

# Valgrind (memory profiling)
valgrind --tool=callgrind ./nlm --config config.json
callgrind_control --session=auto

# Perf (Linux performance profiling)
sudo perf record -g ./nlm
perf report
```

#### Benchmarking Scripts
```python
import time
import statistics
from nlm.brain import Brain
from nlm.config import Config

def benchmark_brain_performance():
    """Benchmark brain performance with different configurations"""
    
    configurations = [
        {"neuron_count": 1000, "region_count": 1},
        {"neuron_count": 5000, "region_count": 2},
        {"neuron_count": 10000, "region_count": 4},
    ]
    
    for config_dict in configurations:
        print(f"Testing with {config_dict} neurons")
        
        config = Config()
        for key, value in config_dict.items():
            config.set(key, value)
        
        # Warm-up
        brain = Brain(config)
        brain.initialize()
        
        # Benchmark initialization
        start_time = time.time()
        brain = Brain(config)
        brain.initialize()
        init_time = time.time() - start_time
        
        # Benchmark simulation
        start_time = time.time()
        steps = 1000
        for step in range(steps):
            brain.step(step, step * config.get("simulation_timestep"))
        sim_time = time.time() - start_time
        
        # Calculate metrics
        init_perf = 1.0 / init_time if init_time > 0 else 0
        sim_perf = steps / sim_time if sim_time > 0 else 0
        
        print(f"  Initialization: {init_perf:.1f} instances/second")
        print(f"  Simulation: {sim_perf:.1f} steps/second")
        
        # Calculate neurons processed per second
        neurons_per_second = config_dict["neuron_count"] * sim_perf
        print(f"  Neurons processed: {neurons_per_second:.0f}/second")

if __name__ == "__main__":
    benchmark_brain_performance()
```

## Debugging and Troubleshooting

### Common Issues and Solutions

1. **Initialization Failures**
   - Check configuration files for errors
   - Verify system resources (memory, CPU)
   - Check for conflicting installations

2. **Simulation Crashes**
   - Check for null pointer dereferences
   - Validate configuration parameters
   - Review error messages and stack traces

3. **Performance Problems**
   - Profile the application to identify bottlenecks
   - Check for inefficient algorithms
   - Optimize data structures and algorithms

### Debugging Techniques

#### Using Debuggers
```bash
# GDB (Linux/macOS)
gdb ./nlm
(gdb) run --config config.json
(gdb) bt  # Show backtrace
(gdb) print variable_name

# LLDB (macOS)
lldb ./nlm
(lldb) launch -- config.json
(lldb) bt
(lldb) frame variable
```

#### Debugging Strategies

1. **Add Print Statements**: Use logging for debugging
2. **Use Debugger**: Use GDB/LLDB for runtime debugging
3. **Analyze Logs**: Review logs for error patterns
4. **Test with Minimal Configuration**: Start with minimal working configuration
5. **Reproduce Issues**: Create test cases that reproduce the issue

### Debugging Scripts
```python
import sys
import logging
from nlm.brain import Brain
from nlm.config import Config

# Configure logging
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

def debug_brain_initialization(config_file):
    """Debug brain initialization"""
    try:
        logger.info(f"Loading configuration from {config_file}")
        config = Config()
        
        if config_file:
            if not config.loadFromFile(config_file):
                raise ValueError(f"Failed to load configuration from {config_file}")
        
        logger.info("Creating brain instance")
        brain = Brain(config)
        
        logger.info("Initializing brain")
        if not brain.initialize():
            raise RuntimeError("Brain initialization failed")
        
        logger.info("Brain initialization successful")
        logger.info(f"Brain has {brain.getRegionCount()} regions")
        logger.info(f"Brain has {brain.getTotalNeuronCount()} neurons")
        logger.info(f"Brain has {brain.getTotalSynapseCount()} synapses")
        
        return brain
        
    except Exception as e:
        logger.error(f"Error during brain initialization: {e}")
        raise

def debug_simulation(brain, steps):
    """Debug simulation"""
    try:
        logger.info(f"Starting simulation for {steps} steps")
        
        for step in range(steps):
            brain.step(step, step * brain.getConfig().get("simulation_timestep"))
            
            # Log progress every 100 steps
            if step % 100 == 0:
                logger.info(f"Step {step}/{steps} completed")
        
        logger.info("Simulation completed successfully")
        
    except Exception as e:
        logger.error(f"Error during simulation: {e}")
        raise

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python debug_script.py <config_file>")
        sys.exit(1)
    
    config_file = sys.argv[1]
    brain = debug_brain_initialization(config_file)
    debug_simulation(brain, 1000)
```

## Development Best Practices

### Code Organization

1. **Modular Design**: Keep modules small and focused
2. **Separation of Concerns**: Separate data structures from algorithms
3. **Single Responsibility**: Each class/function should have one purpose
4. **Abstraction Layers**: Provide clear interfaces between components

### Testing Strategy

1. **Test-Driven Development**: Write tests before implementing functionality
2. **Mock External Dependencies**: Use mocking for external systems
3. **Integration Testing**: Test component interactions
4. **Performance Testing**: Include performance benchmarks
5. **Regression Testing**: Ensure existing functionality continues to work

### Documentation Strategy

1. **Inline Documentation**: Document code with comments
2. **API Documentation**: Use Doxygen-style comments for public APIs
3. **User Documentation**: Provide user guides and tutorials
4. **Developer Documentation**: Provide development and contribution guides

### Version Control

1. **Branching Strategy**: Use feature branches for new development
2. **Code Reviews**: Require code reviews for all changes
3. **Continuous Integration**: Use automated testing and deployment
4. **Backup Strategy**: Regular backups of the repository

## Continuous Integration

### GitHub Actions Workflow
```yaml
# .github/workflows/ci.yml
name: NLM CI

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]
  schedule:
    - cron: '0 2 * * *'  # Daily at 2 AM

jobs:
  build:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        build-type: [Release, Debug]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up environment
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake python3 python3-pip
    
    - name: Install Python dependencies
      run: |
        pip3 install scikit-build-core pybind11 pytest pytest-cov
        pip3 install -e .
    
    - name: Configure
      run: |
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=${{ matrix.build-type }}
    
    - name: Build
      run: |
        cd build
        make -j$(nproc)
    
    - name: Test
      run: |
        cd ..
        pytest --cov=nlm --cov-report=term-missing
    
    - name: Upload coverage
      uses: actions/upload-artifact@v3
      if: always()
      with:
        name: coverage-${{ matrix.build-type }}
        path: coverage.xml
```

### Code Quality Checks
```yaml
# .github/workflows/quality.yml
name: Code Quality

on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

jobs:
  quality:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v3
      with:
        python-version: "3.10"
    
    - name: Install quality tools
      run: |
        pip install black isort flake8 mypy
    
    - name: Check code formatting
      run: |
        black --check .
        isort --check-only .
    
    - name: Lint code
      run: |
        flake8 src/
    
    - name: Type checking
      run: |
        mypy src/nlm/
```

### Release Process
```yaml
# .github/workflows/release.yml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  release:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Set up Python
      uses: actions/setup-python@v3
      with:
        python-version: "3.10"
    
    - name: Build package
      run: |
        pip install build
        python -m build
    
    - name: Test package
      run: |
        pip install ./dist/*.whl
        pytest
    
    - name: Create release
      uses: actions/create-release@v1
      with:
        tag_name: ${{ github.ref_name }}
        release_name: Release ${{ github.ref_name }}
        draft: false
        prerelease: false
    
    - name: Upload release assets
      uses: actions/upload-release-asset@v1
      with:
        upload_url: ${{ steps.create_release.outputs.upload_url }}
        asset_path: ./dist/*.whl
        asset_name: nlm-${{ github.ref_name }}.whl
```

## Documentation Standards

### API Documentation

Use Doxygen-style comments for public APIs:

```cpp
/**
 * @class Brain
 * @brief Represents an artificial brain with neural populations
 * 
 * The Brain class implements a complete neural system with:
 * - Multiple neural regions
 * - Event-driven neuron dynamics
 * - Synaptic plasticity mechanisms
 * - Integrated memory systems
 * - Neuromodulation
 * - Cognitive functions
 * 
 * The brain operates as a closed-loop system:
 * 1. Receives sensory input
 * 2. Processes through neural dynamics
 * 3. Generates actions
 * 4. Receives rewards/predictions
 * 5. Updates synaptic connections
 * 
 * @see Neuron, Synapse, MemorySystem, PlasticityRule
 */
class Brain {
public:
    /**
     * @brief Initialize the brain with configuration
     * 
     * @param config Configuration parameters
     * @return true if initialization successful, false otherwise
     * @throws std::invalid_argument if configuration is invalid
     * 
     * @note This method must be called before any other brain operations
     * @warning Not thread-safe - should be called from a single thread
     */
    bool initialize(std::shared_ptr<Config> config);
    
    /**
     * @brief Execute one simulation step
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time
     * 
     * @note This method advances the brain state by one timestep
     * @warning Computationally intensive - may block
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Get the number of neural regions
     * 
     * @return Number of regions in the brain
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get the total number of neurons
     * 
     * @return Total number of neurons in the brain
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the total number of synapses
     * 
     * @return Total number of synapses in the brain
     */
    size_t getTotalSynapseCount() const;
};
```

### Code Commenting Standards

1. **Header Comments**: Document file purpose, author, date, and copyright
2. **Function Comments**: Document function parameters, return values, and behavior
3. **Inline Comments**: Explain complex algorithms and data structures
4. **TODO Comments**: Mark incomplete or problematic code
5. **FIXME Comments**: Mark bugs that need fixing

```cpp
// File: src/brain/Brain.cpp
// Author: NLM Development Team
// Date: 2026-10-03
// Copyright: MIT License
// Purpose: Implementation of the artificial brain simulation

// TODO: Implement STDP for reward-modulated learning
// FIXME: Memory leak in spike handler registration

void Brain::initializeSystems() {
    // Initialize working memory system
    // Working memory maintains persistent activity through recurrent connections
    pImpl->workingMemory->initialize(this);
    
    // Set memory capacity based on brain size
    // Capacity scales with number of neurons for realistic memory limits
    size_t capacity = pImpl->config->getOr<size_t>("neuron_count", 1000) / 10;
    pImpl->workingMemory->setCapacity(capacity);
}
```

## Migration Guides

### Migration from Phase 2 to Phase 6

This guide describes how to migrate from NLM Phase 2 to Phase 6:

#### Configuration Changes

**Phase 2 Configuration**:
```bash
# config.cfg
neuron_count = 500
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
```

**Phase 6 Configuration**:
```json
{
  "neuron_count": 500,
  "region_count": 1,
  "connection_probability": 0.1,
  "simulation_timestep": 0.001,
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "stdp_tau": 20.0,
  "replay_interval": 100,
  "consolidation_interval": 1000,
  "checkpoint_dir": "./checkpoints",
  "enable_checkpointing": true,
  "enable_replay": true,
  "enable_development": true,
  "log_level": "info",
  "output_format": "text"
}
```

#### Code Changes

**Phase 2 Brain Class**:
```cpp
// Brain.hpp - Phase 2
class Brain {
public:
    bool initialize();
    void step(SimulationStep step);
    // ... other methods
};
```

**Phase 6 Brain Class**:
```cpp
// Brain.hpp - Phase 6
class Brain {
public:
    bool initialize();
    void step(SimulationStep step, Timestamp time);
    void receiveSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> produceAction();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    // ... other methods
};
```

#### API Changes

**Phase 2**:
```cpp
// Only basic spike system
SpikeSystem* spikeSystem = brain->getSpikeSystem();
```

**Phase 6**:
```cpp
// Multiple systems available
SpikeSystem* spikeSystem = brain->getSpikeSystem();
STDP* stdp = brain->getSTDP();
Hebbian* hebbian = brain->getHebbian();
StructuralPlasticity* structuralPlasticity = brain->getStructuralPlasticity();

// Memory systems
WorkingMemory* workingMemory = brain->getWorkingMemory();
EpisodicMemory* episodicMemory = brain->getEpisodicMemory();

// Neuromodulation
Dopamine* dopamine = brain->getDopamine();
Curiosity* curiosity = brain->getCuriosity();
```

#### Testing Changes

**Phase 2 Tests**:
```python
# Simple tests for basic functionality
def test_basic_connectivity():
    config = Config()
    brain = Brain(config)
    assert brain.initialize()
    assert brain.getTotalNeuronCount() == 500
```

**Phase 6 Tests**:
```python
# Comprehensive tests for integrated systems
def test_memory_integration():
    config = Config()
    brain = Brain(config)
    assert brain.initialize()
    
    # Test working memory
    working_memory = brain.getWorkingMemory()
    working_memory.store(123, 0.8)
    assert abs(working_memory.retrieve(123) - 0.8) < 0.001
    
    # Test episodic memory
    episodic_memory = brain.getEpisodicMemory()
    # Store episodes and test replay
    
def test_plasticity_integration():
    config = Config()
    brain = Brain(config)
    assert brain.initialize()
    
    # Test STDP
    stdp = brain.getSTDP()
    # Test weight updates based on spike timing
    
def test_neuromodulation():
    config = Config()
    brain = Brain(config)
    assert brain.initialize()
    
    # Test dopamine effects
    dopamine = brain.getDopamine()
    # Test plasticity modulation
```

#### Performance Considerations

**Phase 2**:
- Smaller networks (500 neurons)
- Simpler architecture
- Faster execution but limited functionality

**Phase 6**:
- Larger networks (1000+ neurons)
- Complex integrated systems
- Slower execution but more features
- Better memory management and resource utilization

#### Backward Compatibility

Phase 6 maintains backward compatibility for:
- Basic configuration options
- Core simulation functionality
- Simple command-line interface

New features added in Phase 6:
- JSON/YAML configuration support
- Advanced CLI options
- Comprehensive error handling
- Checkpoint and persistence
- Development stages
- Enhanced visualization

#### Migration Steps

1. **Update Configuration**: Convert configuration to new format
2. **Update Code**: Add new API calls for Phase 6 features
3. **Add Tests**: Update tests to cover new functionality
4. **Performance Testing**: Benchmark against Phase 2 performance
5. **Documentation**: Update documentation for new features

#### Example Migration Script
```python
#!/usr/bin/env python3
"""
Migration script from NLM Phase 2 to Phase 6
"""

import json
import yaml
import argparse
import sys
from pathlib import Path

def migrate_config_file(input_file, output_file):
    """Migrate configuration file from Phase 2 to Phase 6 format"""
    
    # Load input configuration
    if input_file.suffix == '.json':
        with open(input_file, 'r') as f:
            config = json.load(f)
    elif input_file.suffix in ['.yaml', '.yml']:
        with open(input_file, 'r') as f:
            config = yaml.safe_load(f)
    elif input_file.suffix == '.cfg':
        # Parse simple key=value format
        config = {}
        with open(input_file, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    if '=' in line:
                        key, value = line.split('=', 1)
                        config[key.strip()] = value.strip()
    else:
        raise ValueError(f"Unsupported configuration file format: {input_file.suffix}")
    
    # Add Phase 6 default values
    phase6_config = {
        "replay_interval": 100,
        "consolidation_interval": 1000,
        "checkpoint_dir": "./checkpoints",
        "enable_checkpointing": True,
        "enable_replay": True,
        "enable_development": True,
        "log_level": "info",
        "output_format": "text"
    }
    
    # Merge with input configuration (Phase 6 values override)
    for key, value in phase6_config.items():
        if key not in config:
            config[key] = value
    
    # Save as Phase 6 configuration
    if output_file.suffix == '.json':
        with open(output_file, 'w') as f:
            json.dump(config, f, indent=2)
    elif output_file.suffix in ['.yaml', '.yml']:
        with open(output_file, 'w') as f:
            yaml.dump(config, f, default_flow_style=False)
    else:
        raise ValueError(f"Unsupported output file format: {output_file.suffix}")
    
    print(f"Migrated {input_file} -> {output_file}")

def main():
    parser = argparse.ArgumentParser(description='Migrate NLM configuration from Phase 2 to Phase 6')
    parser.add_argument('input', type=Path, help='Input configuration file')
    parser.add_argument('--output', type=Path, help='Output configuration file (default: auto)')
    
    args = parser.parse_args()
    
    if not args.input.exists():
        print(f"Error: Input file {args.input} does not exist")
        sys.exit(1)
    
    if not args.output:
        # Auto-generate output filename
        stem = args.input.stem
        if args.input.suffix == '.cfg':
            args.output = args.input.with_suffix('.json')
        elif args.input.suffix in ['.yaml', '.yml']:
            args.output = args.input.with_suffix('.yaml')
        else:
            args.output = Path(f"{stem}_phase6.json")
    
    try:
        migrate_config_file(args.input, args.output)
        print("Migration completed successfully")
    except Exception as e:
        print(f"Migration failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
```

This migration guide provides comprehensive instructions for upgrading from NLM Phase 2 to Phase 6, covering configuration changes, code updates, testing requirements, and performance considerations.

---

*Last updated: 2026-10-03*
*NLM Development Guide - Phase 6 Integration*

This guide provides comprehensive instructions for developing, testing, and maintaining the NLM project. It covers building, testing, code quality standards, contribution guidelines, performance optimization, debugging, and migration strategies.

For more specific information about particular components, refer to the detailed API documentation and component-specific guides.