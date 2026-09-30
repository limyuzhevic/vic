# NLM Python Bindings

## Quick Start

This is the Python bindings module for the NLM (熙然) neural simulation framework.

### Installation

```bash
# Install from source
pip install -e .

# Or use the development version
pip install -e python/
```

### Basic Usage

```python
import pynlm

# Create a brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete! {brain.getTotalNeuronCount()} neurons")
```

## Python Module Structure

The `python/` directory contains:

- **`pynlm.py`** - Main user interface with Pythonic wrappers
- **`pynlm_core.py`** - Low-level C++ bindings interface
- **`cli.py`** - Command-line tools for common operations
- **`example.py`** - Example scripts and demonstrations
- **`test/`** - Comprehensive test suite
- **`setup.py`** - Python package configuration
- **`CMakeLists.txt`** - Build configuration for C++ extensions

## Key Features

### 1. Complete Python API
- All requested classes and methods are implemented
- Type hints throughout for better IDE support
- Comprehensive docstrings with examples
- Error handling with custom exceptions

### 2. Pythonic Wrappers
- `pynlm.py` provides a clean Python interface
- Wraps the low-level C++ bindings for ease of use
- Adds Python-specific functionality and optimizations

### 3. Command-Line Interface
- `nlm-explore` - Explore NLM capabilities
- `nlm-sim` - Run simulations
- `nlm-analyze` - Analyze results

### 4. Comprehensive Testing
- Unit tests for all classes and methods
- Integration tests for multi-component usage
- Error handling tests
- Performance benchmarking

### 5. Documentation
- `README.md` - Complete user guide
- Example scripts for common use cases
- API documentation with type hints

## Development Setup

### Prerequisites

```bash
# Install Python dependencies
pip install numpy pytest scikit-build-core pybind11

# Install system dependencies (may vary by OS)
# Linux: sudo apt-get install build-essential cmake
# macOS: xcode-select --install
# Windows: Install Visual Studio with C++ components
```

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the module
make -j4

# Install Python module
pip install -e ..
```

### Running Tests

```bash
# Run Python unit tests
pytest python/test/

# Run C++ tests (if available)
cd build && ctest --output-on-failure
```

## API Reference

See `python/README.md` for detailed API documentation.

### Core Classes

- **Config**: Brain configuration management
- **Brain**: Neural simulation engine
- **AgentBrain**: Agent-world interface
- **SimpleWorld**: 2D simulation environment
- **SensoryPercept**: Perceptual input representation
- **Action**: Motor output representation

### Enums

- **NeuronType**: Neural cell types
- **DevelopmentalStage**: Brain development phases
- **ActionType**: Motor action types
- **WorldObjectType**: Environmental object types

## Example Scripts

### Basic Simulation

`python/example.py` contains multiple example scripts:

1. **basic_simulation()** - Simple brain simulation
2. **configuration_example()** - Configuration management
3. **world_objects_example()** - World object handling
4. **enum_examples()** - Enumeration usage
5. **complete_example()** - Full NLM simulation

Run with:
```bash
python python/example.py
```

### Command-Line Tools

```bash
# Explore NLM capabilities
nlm-explore --steps 100 --neurons 1000

# Run simulation from config
nlm-sim config.json output.json

# Analyze results
nlm-analyze results.json --format json
```

## Error Handling

The module provides comprehensive error handling:

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.PyNLMInitializationError as e:
    print(f"Brain initialization failed: {e}")
except pynlm.PyNLMConfigurationError as e:
    print(f"Configuration error: {e}")
```

## Performance Considerations

1. **Reuse instances**: Create brain/world objects once and reuse
2. **Checkpoint frequently**: Use `save()` and `load()` for long simulations
3. **Monitor memory**: Use `getTotalNeuronCount()` and `getTotalSynapseCount()`
4. **Optimize timestep**: Choose appropriate timestep for accuracy/performance

## License

MIT License. See LICENSE file for details.

## Version

Version: 0.1.0

## Support

For questions, issues, or contributions:
- GitHub repository: https://github.com/nlm-project/nlm
- Documentation: https://nlm.readthedocs.io

---

This module provides a complete Python interface to the NLM neural simulation framework,
combining the power of C++ performance with the flexibility of Python.
