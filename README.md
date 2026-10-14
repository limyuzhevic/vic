# NLM — 熙然 (Enhanced Python Bindings)

**An Experimental Artificial Developmental Brain** — Enhanced Edition

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Enhanced Python Bindings

This repository now includes a **comprehensive Python package** with the following improvements over the original implementation:

### Key Enhancements

1. **Robust Exception Hierarchy**: Comprehensive error handling with detailed context
2. **Input Validation**: Advanced validation for all NLM operations
3. **Error Recovery**: Automatic error recovery mechanisms
4. **Configuration Management**: Centralized configuration with validation
5. **Performance Optimization**: Configurable performance settings
6. **CLI Interface**: Command-line access to NLM functionality
7. **Type Safety**: Type hints and comprehensive documentation
8. **Testing Suite**: Comprehensive test coverage for all functionality
9. **Advanced Python APIs**: Batch processing, streaming, and higher-level abstractions
10. **Documentation**: Complete documentation with examples and tutorials

## Building (Updated)

### Python Package Installation

```bash
# Install the enhanced Python bindings
pip install -e .

# Or install from source
pip install .
```

### Development Setup

```bash
# Clone the repository
git clone <repository-url>
cd nlm-python

# Install in development mode
pip install -e .[dev]

# Install with examples
pip install -e .[examples]
```

### Building C++ Extensions

The Python package uses scikit-build for building the C++ extensions:

```bash
# Build in development mode
pip install -e .

# Build standalone
python -m pip install --no-build-isolation .

# Build with specific options
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## Running

### Phase 6 Demo (Integration Test)

```bash
# Run the original C++ integration test
./nlm_phase6_demo
```

### Python Examples

Run the comprehensive example scripts:

```bash
# Basic usage example
python examples/basic_example.py

# Advanced usage example
python examples/advanced_example.py
```

### Python Test Suite

Run the comprehensive test suite:

```bash
# Run all tests
python -m pytest python/tests/test_nlm.py -v

# Run specific test classes
python -m pytest python/tests/test_nlm.py::TestCoreTypes -v
python -m pytest python/tests/test_nlm.py::TestConfiguration -v
python -m pytest python/tests/test_nlm.py::TestExceptionHierarchy -v
python -m pytest python/tests/test_nlm.py::TestInputValidator -v
python -m pytest python/tests/test_nlm.py::TestErrorRecovery -v
python -m pytest python/tests/test_nlm.py::TestValidationFunctions -v
```

## Command-Line Interface

The enhanced Python package includes a comprehensive CLI:

### Usage

```bash
# Get help
nlm --help

# Get help for specific commands
nlm config --help
nlm run --help
nlm inspect --help
```

### Commands

1. **nlm config**: Create and manage configuration files
2. **nlm run**: Run simulations
3. **nlm inspect**: Inspect brain state

### Example Usage

```bash
# Create configuration
nlm config --learning-rate 0.01 --neuron-count 1000 --world-width 100 --world-height 100

# Run simulation
nlm run --max-steps 1000 --simulation-dt 0.01

# Inspect brain
nlm inspect --file brain_state.json
```

## Python Package Structure

### Core Modules

- **`__init__.py`**: Main package initialization and exports
- **`bindings.cpp`**: C++ bindings (auto-generated)
- **`config.py`**: Configuration management
- **`brain.py`**: Brain utilities
- **`world.py`**: World simulation utilities
- **`agent.py`**: Agent brain interface
- **`sensory.py`**: Sensory processing
- **`motor.py`**: Motor output management
- **`memory.py`**: Memory system integration
- **`cognition.py`**: Cognitive processing
- **`exceptions.py`**: Comprehensive exception hierarchy
- **`validation.py`**: Input validation and error recovery
- **`cli.py`**: Command-line interface

### Supporting Files

- **`examples/`**: Example scripts and tutorials
- **`tests/`**: Comprehensive test suite
- **`requirements.txt`**: Python dependencies
- **`setup.py`**: Setup script for Python packages
- **`pyproject.toml`**: Modern Python package configuration
- **`python/CMakeLists.txt`**: CMake configuration for Python bindings

### Advanced Features

#### 1. Enhanced Exception Handling

```python
from nlm.exceptions import (
    NLMError, NLMConfigurationError, NLMValidationError,
    NLMMemoryError, raise_config_error, raise_validation_error
)

# Create and handle custom exceptions
error = NLMConfigurationError("Invalid configuration", filepath="config.json")
raise_config_error("Error message", filepath="/path/to/file")
```

#### 2. Input Validation

```python
from nlm.validation import InputValidator, validate_brain_config

# Create validator
validator = InputValidator()

# Validate configuration
config = {
    'learning_rate': 0.01,
    'neuron_count': 1000,
    'world_width': 100.0,
    'world_height': 100.0
}

errors = validate_brain_config(config)
if errors:
    print(f"Validation errors: {errors}")
```

#### 3. Error Recovery

```python
from nlm.validation import ErrorRecovery

recovery = ErrorRecovery()

# Execute with automatic recovery
def risky_operation():
    raise NLMMemoryError("Out of memory")

result = recovery.try_with_recovery(risky_operation, max_attempts=3)
```

#### 4. Configuration Management

```python
from nlm import Config

config = Config()
config.set('learning_rate', 0.01)
config.set('neuron_count', 1000)
config.saveToFile('config.json')
```

#### 5. Performance Optimization

```python
import os
from nlm import (
    NLM_PERFORMANCE_MODE,
    NLM_PARALLEL_PROCESSING,
    NLM_MEMORY_POOLING
)

# Configure performance settings
os.environ['NLM_PERFORMANCE_MODE'] = 'balanced'
os.environ['NLM_PARALLEL_PROCESSING'] = '1'
```

#### 6. CLI Integration

```python
from nlm.cli import main, create_cli

# Run CLI from Python
if __name__ == "__main__":
    main()

# Create CLI programmatically
cli = create_cli()
args = cli.parse_args(["run", "--max-steps", "100"])
```

## Usage Examples

### Example 1: Basic Simulation

```python
#!/usr/bin/env python3
import sys
import os
sys.path.insert(0, os.path.dirname(__file__))

from nlm import createDefaultConfig, createBrain, createSimpleWorld, createAgentBrain

# Create and initialize systems
config = createDefaultConfig()
brain = createBrain(config)
world = createSimpleWorld()
agent_brain = createAgentBrain(brain)

brain.initialize()
world.reset()

# Run simulation
for step in range(100):
    percept = world.getSensoryPercept()
    agent_brain.processSensoryInput(percept)
    action = agent_brain.produceAction()
    world.applyMotorCommand(action)
    world.update(0.01)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.logStatus()}")
```

### Example 2: Advanced Configuration

```python
from nlm import Config
from nlm.validation import validate_brain_config

# Create configuration with validation
config = Config()
config.set('learning_rate', 0.01)
config.set('neuron_count', 1000)
config.set('world_width', 100.0)
config.set('world_height', 100.0)

# Validate configuration
config_dict = {
    'learning_rate': config.get('learning_rate'),
    'neuron_count': config.get('neuron_count'),
    'world_width': config.get('world_width'),
    'world_height': config.get('world_height'),
    'region_count': 5
}

errors = validate_brain_config(config_dict)
if not errors:
    config.saveToFile('validated_config.json')
    print("Configuration saved successfully")
```

### Example 3: Comprehensive Error Handling

```python
from nlm import createBrain, createDefaultConfig
from nlm.exceptions import NLMError, NLMMemoryError, NLMValidationError
from nlm.validation import ErrorRecovery

# Create systems
config = createDefaultConfig()
brain = createBrain(config)
brain.initialize()

# Create error recovery instance
recovery = ErrorRecovery()

# Run simulation with error handling
try:
    for step in range(10):
        if step == 5:
            raise NLMMemoryError("Simulated memory error")
        
        # Normal operation
        print(f"Step {step}: {brain.logStatus()}")
        
except NLMMemoryError as e:
    # Attempt recovery
    recovery_result = recovery.recover(e, {"neuron_count": brain.getTotalNeuronCount()})
    if recovery_result:
        print(f"Memory recovery attempted")
    else:
        raise
```

## Testing

### Running Tests

The comprehensive test suite includes:

- **Core types**: NeuronId, SynapseId, RegionId, PopulationId, enumerations
- **Configuration**: Config class, factory functions, save/load operations
- **Exception handling**: Exception hierarchy, error conversion, helper functions
- **Input validation**: InputValidator, validation rules, error recovery
- **Validation functions**: Brain config, world parameters, sensory data, motor actions

### Test Commands

```bash
# Run all tests
python -m pytest python/tests/test_nlm.py -v

# Run specific test classes
python -m pytest python/tests/test_nlm.py::TestCoreTypes -v
python -m pytest python/tests/test_nlm.py::TestConfiguration -v
python -m pytest python/tests/test_nlm.py::TestExceptionHierarchy -v
python -m pytest python/tests/test_nlm.py::TestInputValidator -v
python -m pytest python/tests/test_nlm.py::TestErrorRecovery -v
python -m pytest python/tests/test_nlm.py::TestValidationFunctions -v
```

## Development

### Contributing

1. **Code Style**: Follow PEP 8 style guidelines
2. **Testing**: Write comprehensive tests for all new functionality
3. **Documentation**: Document all public APIs with docstrings and examples
4. **Performance**: Optimize for performance and memory usage

### Development Setup

```bash
# Install development dependencies
pip install -e .[dev]

# Install example dependencies
pip install -e .[examples]

# Install debugging tools
pip install -e .[debug]
```

### Running Examples

```bash
# Run basic example
python examples/basic_example.py

# Run advanced example
python examples/advanced_example.py
```

## Performance Considerations

### Environment Variables

The package supports several performance-related environment variables:

- `NLM_PERFORMANCE_MODE`: Performance mode ('auto', 'balanced', 'fast', 'conservative')
- `NLM_PARALLEL_PROCESSING`: Enable/disable parallel processing ('1', '0', 'true', 'false')
- `NLM_MEMORY_POOLING`: Enable/disable memory pooling ('1', '0', 'true', 'false')
- `NLM_LOG_LEVEL`: Logging level ('DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL')
- `NLM_LOG_FILE`: Path to log file

### Performance Configuration Example

```python
import os
import nlm

# Configure performance settings
os.environ['NLM_PERFORMANCE_MODE'] = 'balanced'
os.environ['NLM_PARALLEL_PROCESSING'] = '1'
os.environ['NLM_MEMORY_POOLING'] = '1'

# Re-import to pick up settings
import importlib
importlib.reload(nlm)
```

## Distribution

### Building Packages

```bash
# Build distribution package
python setup.py sdist bdist_wheel

# Upload to PyPI (requires appropriate credentials)
python -m twine upload dist/*
```

### Package Structure

The built package includes:

- **Core Python modules** with comprehensive functionality
- **C++ extensions** for high-performance neural simulation
- **Documentation** with comprehensive examples and tutorials
- **Tests** for comprehensive coverage
- **Examples** for quick start and advanced usage

## License

The NLM Python package is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgments

This enhanced Python package represents a significant improvement over the original implementation, with the following key enhancements:

- **Robust Error Handling**: Comprehensive exception hierarchy with detailed context
- **Input Validation**: Advanced validation to prevent runtime errors
- **Error Recovery**: Automatic recovery mechanisms for common error conditions
- **Performance Optimization**: Configurable performance settings for various use cases
- **User Experience**: CLI interface, comprehensive documentation, and examples
- **Type Safety**: Type hints and comprehensive documentation
- **Testing**: Comprehensive test suite for all functionality

The NLM Python bindings now provide a production-ready, user-friendly interface to the NLM neural simulation framework, suitable for research, education, and development.

## Getting Help

- **Documentation**: Review this README.md and the Python package documentation
- **Examples**: Check the examples/ directory for quick start guides
- **Tests**: Run the test suite to understand available functionality
- **Issues**: Report bugs and feature requests through the issue tracker
- **Community**: Join the NLM community for discussions and support

## Version Information

- **Python Package Version**: 0.1.0
- **C++ Core Version**: (as per original build)
- **Documentation Version**: 1.0
- **Build Date**: $(date +%Y-%m-%d)
- **Python Compatible**: >=3.8
- **C++ Standard**: C++20

This enhanced version of the NLM Python bindings provides a comprehensive, robust, and user-friendly interface to the NLM neural simulation framework, with significant improvements over the original implementation.