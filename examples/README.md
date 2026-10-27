# NLM Advanced Examples

This directory contains advanced example scripts for using the NLM (Neural Learning Machine) Python API.
These examples demonstrate sophisticated usage patterns and advanced features beyond the basic functionality.

## Overview

The NLM library provides a complete brain-inspired spiking neural network simulator with integrated memory, prediction, cognition, and neuromodulation systems. These advanced examples showcase how to leverage the full power of the NLM framework for complex neural simulations.

## Examples Structure

### Basic Examples (`examples/basic/`)
Simple examples for getting started with NLM:

- **hello.py**: Simple "Hello World" example demonstrating basic brain creation
- **visualization.py**: Brain visualization and statistics example
- **quick_test.py**: Quick performance test

### Advanced Examples (`examples/advanced/`)
Comprehensive examples demonstrating advanced features:

- **examples.py**: Main examples class with all advanced demonstrations
- **simple_brain.py**: Simple brain with custom configuration
- **agent_world.py**: Complete agent-world interaction with learning
- **learning_session.py**: Learning session with checkpointing and replay
- **development_stages.py**: Development stages and maturation
- **configuration_comparison.py**: Configuration comparison and optimization

### Custom Examples (`examples/custom/`)
Place your custom example scripts here.

## Running Examples

### Using the Example Manager

The easiest way to run examples is through the `examples` module:

```bash
# List all available examples
python -m examples --list

# Run a basic example
python -m examples basic hello

# Run an advanced example
python -m examples advanced simple

# Run a custom example
python -m examples custom my_example.py

# Run all advanced examples
python -m examples advanced all
```

### Direct Execution

You can also run individual example files directly:

```bash
# Basic examples
python examples/basic/hello.py
python examples/basic/visualization.py
python examples/basic/quick_test.py

# Advanced examples
python examples/advanced/simple_brain.py
python examples/advanced/agent_world.py
python examples/advanced/learning_session.py
python examples/advanced/development_stages.py
python examples/advanced/configuration_comparison.py

# Advanced examples module
python examples/advanced/examples.py
```

## Example Categories

### 1. Simple Brain Examples

The `simple_brain.py` example demonstrates:
- Creating custom configurations for different use cases
- Basic brain initialization and simulation
- Configuration file saving and loading (JSON/YAML support)

### 2. Agent-World Interaction Examples

The `agent_world.py` and `learning_session.py` examples demonstrate:
- Complete agent-world interaction loops
- Sensory processing and motor output
- Reward modulation and learning
- Curiosity-driven exploration
- Development system updates

### 3. Development and Maturation Examples

The `development_stages.py` example demonstrates:
- Neural development stages (Initial, Critical Period, Maturation, Adult, Aging)
- Plasticity changes over time
- Structural plasticity (synaptogenesis and pruning)

### 4. Configuration Management Examples

The `configuration_comparison.py` example demonstrates:
- Creating different configuration profiles (simple, advanced, debug, production)
- Performance benchmarking with different settings
- Saving configuration comparisons to JSON files

### 5. Advanced Learning Examples

The `learning_session.py` example demonstrates:
- Checkpoint saving and loading for long-running simulations
- Memory management and consolidation
- Agent persistence across episodes
- Complex reward structures

## Advanced Features Demonstrated

### Configuration System

- **JSON/YAML Support**: Load and save configurations in modern formats
- **Auto-detection**: Automatic file format detection based on extension
- **Backward Compatibility**: Support for legacy key=value format
- **Configuration Validation**: Basic validation of configuration values

### Pythonic APIs

- **Context Managers**: Proper resource management
- **Property Access**: Clean Pythonic interfaces
- **Exception Handling**: Comprehensive error handling
- **Type Checking**: Type safety in Python

### Performance Monitoring

- **Timing**: Performance benchmarking
- **Statistics**: Detailed brain statistics logging
- **Progress Reporting**: Real-time progress monitoring

### File I/O

- **Multiple Formats**: JSON, YAML, and legacy text formats
- **Cross-platform**: Platform-independent file paths
- **Error Handling**: Robust file I/O with error recovery

## Customization Guide

### Creating Custom Examples

To create your own example, follow these patterns:

1. **Import the library**:
```python
import pynlm
import json
import time
```

2. **Create a configuration**:
```python
def create_config(config_type="advanced"):
    config = pynlm.createDefaultConfig()
    # Set your configuration values
    return config
```

3. **Create and initialize the brain**:
```python
def run_example():
    config = create_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    # Your simulation code here
```

4. **Save results and clean up**:
```python
# Save checkpoints
brain.save("checkpoint.bin")

# Log statistics
brain.logStatus()
```

### Extending the Examples

The examples directory structure supports extension:

```
examples/
├── __init__.py              # Package initialization
├── basic/                   # Basic examples
│   ├── hello.py
│   ├── visualization.py
│   └── quick_test.py
├── advanced/                # Advanced examples
│   ├── __init__.py
│   ├── examples.py         # Main examples class
│   ├── simple_brain.py
│   ├── agent_world.py
│   ├── learning_session.py
│   ├── development_stages.py
│   └── configuration_comparison.py
└── custom/                  # Custom examples
    └── your_example.py
```

## Requirements

### Python Dependencies

```bash
pip install pynlm numpy
```

### System Dependencies

The NLM library requires:
- C++20 compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- Python 3.8+ with development headers
- pybind11 2.11.0+
- scikit-build-core
- numpy (optional but recommended)

## Troubleshooting

### Common Issues

1. **Import Errors**
   ```bash
   # Make sure you're in the correct directory
   cd /path/to/nlm/project
   
   # Install the NLM package
   pip install -e .
   ```

2. **Configuration File Errors**
   ```python
   # Check file paths and permissions
   import os
   os.makedirs("checkpoints", exist_ok=True)
   
   # Validate JSON/YAML syntax
   import json
   with open("config.json", 'r') as f:
       json.load(f)  # Will raise exception if invalid
   ```

3. **Performance Issues**
   ```python
   # Use smaller configurations for testing
   config.set("brain.neuron_count", 100)
   
   # Monitor performance
   import time
   start_time = time.time()
   # Your simulation code
   elapsed = time.time() - start_time
   print(f"Simulation completed in {elapsed:.2f}s")
   ```

4. **Memory Issues**
   ```python
   # Use checkpointing for long simulations
   brain.save("checkpoint.bin")
   
   # Clear memory when not needed
   import gc
   gc.collect()
   ```

### Getting Help

- **Documentation**: Check `README.md` and `HOW_TO_USE.md` for basic usage
- **Examples**: Run examples with `--list` to see all available options
- **Community**: Join the NLM community for advanced discussions

## License

This example code is licensed under the MIT License, same as the NLM library.
See the LICENSE file for details.

## Version

NLM Advanced Examples v0.1.0
NLM Core Library v0.1.0 (Phase 6: Final Integration)

For more information about NLM, visit the official documentation and GitHub repository.
