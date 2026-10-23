# NLM Quick Start Guide

## Overview

Welcome to the NLM (Neural Learning Machine) project! This is an experimental artificial developmental brain project that simulates learning through interaction. You're currently working with Phase 6: Final Integration, which integrates all existing systems into a coherent artificial brain.

## Quick Start

### 1. Install Dependencies

First, install all necessary dependencies:

```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install -y g++ cmake make

# Install Python dependencies
pip install pytest numpy scikit-build-core pybind11

# Install NLM in development mode (builds Python bindings)
pip install -e .
```

### 2. Run Tests

#### Run Python Tests

```bash
# Run the Python test suite
python -m pytest tests/test_python_bindings.py -v

# Or use the test runner
python test_runner.py
```

#### Run C++ Tests

```bash
# Build C++ tests
mkdir build_cpp
cd build_cpp
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Run C++ tests
./nlm_test
```

### 3. Run Examples

```bash
# Run basic usage examples
python examples/basic_usage.py
```

### 4. Install Python Bindings

If you just want the Python bindings (for research or development):

```bash
pip install pynlm
```

## Project Structure

```
.
├── src/
│   ├── brain/           # Neural computation engine
│   ├── agent/           # Agent-brain interface
│   ├── world/           # Environment simulation
│   ├── core/            # Core systems (Config, Types, etc.)
│   └── ...              # Other core modules
│
├── python/              # Python bindings (C++ source)
├── tests/               # Test suites
│   ├── test_python_bindings.py    # Python tests
│   ├── test_config_error_handling.cpp  # C++ error handling tests
│   └── ...              # Other C++ tests
│
├── examples/            # Usage examples
│   └── basic_usage.py   # Basic usage examples
├── configs/             # Configuration files
├── docs/                # Scientific documentation
└── .github/             # CI/CD workflows
```

## Key Features

### 1. Memory Systems
- Working memory with persistent activity
- Episodic memory with replay
- Associative memory

### 2. Neuromodulation
- Dopamine system
- Curiosity and novelty detection

### 3. Prediction System
- Integrated into brain loop with prediction error signals

### 4. Development Integration
- Age-dependent plasticity rates
- Structural changes over time

### 5. Persistence
- Checkpoint save/load functionality

### 6. Replay and Consolidation
- Episodic memory replay during rest

## API Overview

### Python Bindings

The Python bindings provide access to all major NLM systems:

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create brain with configuration
brain = pynlm.createBrain(config)

# Create world
world = pynlm.createSimpleWorld()

# Create agent brain
agent_brain = pynlm.createAgentBrain(brain)

# Initialize systems
brain.initialize()
agent_brain.initialize(world)
```

### Error Handling

The bindings include comprehensive error handling with custom exceptions:

```python
try:
    # Code that might throw exceptions
    config = pynlm.createDefaultConfig()
    config.set("invalid_key", "invalid_value")
except pynlm.ConfigKeyNotFoundException as e:
    print(f"Key not found: {e}")
except pynlm.ConfigInvalidValueException as e:
    print(f"Invalid value: {e}")
except pynlm.ConfigException as e:
    print(f"Configuration error: {e}")
```

## Configuration

### Default Configuration File

The project includes a default configuration file at `configs/default.cfg`. Key parameters include:

```
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.1
dopamine_baseline = 0.1
```

### Configuration Loading

Configurations can be loaded from files or command line arguments:

```python
# Load from file
config.loadFromFile("my_config.cfg")

# Load from command line
config.loadFromArgs(len(sys.argv), sys.argv)

# Save to file
config.saveToFile("output_config.cfg")
```

### Configuration Validation

The configuration system includes validation to prevent runtime errors:

```python
# Validate specific key
config.validate("neuron_count")

# Validate all keys
if config.validateAll():
    print("All configuration values are valid")
else:
    print("Some configuration values are invalid")
```

## Development Workflow

### Building

Use the provided test runner for a complete build and test cycle:

```bash
python test_runner.py
```

This will:
1. Build the C++ library
2. Install Python bindings
3. Run Python tests
4. Run C++ tests

### Testing

#### Python Tests
```bash
python -m pytest tests/test_python_bindings.py -v
```

#### C++ Tests
```bash
cd build_cpp
./nlm_test
```

### Code Quality

The project includes a GitHub Actions CI/CD pipeline that automatically:
- Builds the project for Debug and Release modes
- Runs all tests
- Performs style checks
- Generates documentation

## Troubleshooting

### Common Issues

#### "Failed to import pynlm"

**Solution:** Build the Python bindings:

```bash
pip install -e .
```

#### "Configuration file not found"

**Solution:** Ensure the file path is correct and the file exists:

```python
import os
config_path = "configs/default.cfg"
if not os.path.exists(config_path):
    print(f"Config file not found: {config_path}")
```

#### "Test failures"

**Solution:** Check the error messages in the test output. Common issues include:
- Missing dependencies
- Build configuration problems
- Environment setup issues

### Getting Help

1. **Check the documentation:** `docs/` directory contains detailed information
2. **Run examples:** `python examples/basic_usage.py` shows basic usage
3. **Review test output:** The test runner provides detailed feedback
4. **Check GitHub issues:** See if your issue has already been reported

## Contributing

### Code Standards

- Follow existing C++ and Python coding conventions
- Use descriptive variable and function names
- Add comments for complex code sections
- Include tests for new functionality

### Adding New Tests

1. For C++ tests, add to the `tests/` directory and update `CMakeLists.txt`
2. For Python tests, add to `tests/test_python_bindings.py`
3. Follow the existing test patterns and include appropriate error handling

### Documentation

1. Add docstrings to new functions and classes
2. Update the `examples/basic_usage.py` with new examples
3. Add to the `IMPROVEMENTS_SUMMARY.md` document

## Support

This project is part of the NLM (熙然) experimental brain research initiative. For technical support or research collaborations, please refer to the project documentation or contact the research team.

## License

MIT License - See LICENSE file for details.

## Acknowledgements

- This project builds on extensive research in computational neuroscience
- Special thanks to all contributors and researchers in the field
- The project implements advanced concepts from artificial developmental biology

## Version

Phase 6: Final Integration - Improvements Released

---

*This guide was automatically generated as part of the NLM project improvements.*
