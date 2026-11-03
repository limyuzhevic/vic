"""
Improved NLM Python Bindings - Version 0.1.0

This is the improved version of the NLM (Neural Learning Machine) Python bindings.
The bindings have been significantly enhanced to provide:

✓ BETTER API DESIGN
  - Template-based ID types to reduce code duplication
  - Enhanced Config API with dictionary support
  - Convenience methods on Action and other classes
  - Better error handling and validation

✓ IMPROVED DOCUMENTATION
  - Comprehensive docstrings for all methods
  - Extensive examples in examples/ directory
  - Quick start guide for beginners
  - Test suite for validation

✓ ADVANCED FEATURES
  - Text encoding and concept formation utilities
  - Enhanced world simulation
  - Performance monitoring tools
  - Advanced agent integration examples

✓ BETTER BUILD CONFIGURATION
  - CMakeLists.txt now enables Python bindings by default
  - Better installation paths for Python packages
  - Development build support
  - Comprehensive build options

✓ COMPREHENSIVE TESTING
  - Unit tests for all major features
  - Error handling validation
  - Performance monitoring
  - Integration testing

## Quick Start

### Installation

```bash
# Install in development mode (recommended for development)
pip install -e .

# Or install from source
pip install .
```

### Basic Usage

```python
import pynlm

# Create a brain
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
    
# Check results
print(f"Neurons: {brain.get_total_neuron_count()}")
print(f"Firing: {brain.get_firing_neuron_count()}")
print(f"Spikes: {brain.get_total_spike_count()}")
```

### Advanced Usage

```python
import pynlm
import numpy as np

# Create configuration from dictionary
config_dict = {
    "brain.neuron_count": 500,
    "brain.connection_probability": 0.1,
    "plasticity.stdp.enable": True,
}

config = pynlm.create_config_from_dict(config_dict)
brain = pynlm.create_brain(config)
brain.initialize()

# Create world and agent
world = pynlm.create_simple_world()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)

agent = pynlm.create_agent_brain(brain)
agent.initialize(world)

# Enable advanced features
agent.enable_reward_modulation(True)
agent.enable_structural_plasticity(True)
agent.enable_development(True)
agent.enable_curiosity(True)

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.process_sensory_input(world.get_sensory_percept())
    brain.step(step)
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.get_simulation_time())
```

### Text Processing

```python
import pynlm

# Text encoding
pattern1 = pynlm.encode_text("hello")
pattern2 = pynlm.encode_text("world")

# Concept formation
similarity = pynlm.compute_concept_similarity(pattern1, pattern2)
print(f"Similarity: {similarity:.3f}")
```

### Performance Monitoring

```python
import time

config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

start_time = time.time()
for step in range(1000):
    brain.step(step)
elapsed = time.time() - start_time

print(f"Performance: {1000/elapsed:.1f} steps/second")
```

## Files Included

### Python API (python/)
- **bindings.cpp**: The main Python bindings file (improved)
- **bindings.cpp.h**: Generated bindings header

### Examples (examples/)
- **comprehensive_examples.py**: 10 detailed example scripts
- **quick_start.py**: Quick start guide
- All examples demonstrate different features

### Tests (test_improved_bindings.py)
- Comprehensive test suite for the improved API
- Unit tests for all major features
- Error handling validation

### Documentation
- **README.md**: Comprehensive documentation
- **HOW_TO_USE.md**: Detailed usage instructions
- **easy_usage.md**: Beginner-friendly guide

### Build Configuration
- **CMakeLists.txt**: Improved build configuration
- **pyproject.toml**: Python package configuration
- **python/CMakeLists.txt**: Legacy Python-only build (deprecated)

## Building from Source

### Standard Build (Recommended)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Python Module Only

```bash
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make pynlm
```

### Development Build

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DNLM_BUILD_PYTHON_BINDINGS=ON
make -j4
```

## Testing

### Run Python Tests

```bash
python3 test_improved_bindings.py
```

### Run Examples

```bash
python3 examples/comprehensive_examples.py
python3 quick_start.py
```

### Run C++ Tests (if available)

```bash
mkdir build
cd build
cmake ..
make test
```

## Troubleshooting

### Installation Issues

**"ModuleNotFoundError: No module named 'pynlm'"**

```bash
# Force reinstall
pip install --force-reinstall -e .

# Or install from source
pip install .
```

**"ImportError: libpynlm.so not found"**

```bash
# Check library path
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/build
python3 -c "import pynlm; print(pynlm.__version__)"
```

**"Build failures with CMake"**

```bash
# Ensure required dependencies
pip install pybind11 scikit-build-core

# Or use system packages
apt-get update
apt-get install pybind11-dev cmake
```

### Usage Issues

**"Brain not firing neurons"**

- Ensure `brain.initialize()` is called
- Check that configuration is valid
- Verify that simulation steps are being executed

**"Agent not producing actions"**

- Check that `agent.initialize(world)` is called
- Verify that sensory input is being processed
- Ensure that the agent is enabled for action production

**"Performance issues"**

- Consider using the optimized release build
- Profile performance using the monitoring tools
- Adjust configuration for your hardware

## Migration Guide

If upgrading from an older version, note these API changes:

### Old API (Basic)
```python
# Old way
config = Config()
brain = Brain(config)

# New way
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
```

### Old API (Actions)
```python
# Old way
action = Action(ActionType.MoveForward)

# New way  
action = pynlm.Action(pynlm.ActionType.MoveForward)
# Plus convenience methods
if action.is_move_action():
    print("This is a movement action")
```

### Old API (Configuration)
```python
# Old way - limited config creation
config = Config()
config.set("key", "value")

# New way - multiple configuration options
config = pynlm.create_default_config()
config.set("key", "value")
# Plus dictionary support
config_dict = {"key": "value"}
config = pynlm.create_config_from_dict(config_dict)
```

### Deprecated APIs

Some APIs have been deprecated in favor of improved alternatives:

- `Config()` → `pynlm.create_default_config()`
- `Brain(config)` → `pynlm.create_brain(config)`
- `createConfig()` → `create_default_config()`

## Advanced Features

### Text Encoding

```python
# Encode text into neural patterns
pattern = pynlm.encode_text("artificial intelligence")

# Decode patterns back to text (if available)
# text = pynlm.decode_text(pattern)
```

### Concept Formation

```python
# Compare neural patterns
pattern1 = pynlm.encode_text("machine")
pattern2 = pynlm.encode_text("learning")

similarity = pynlm.compute_concept_similarity(pattern1, pattern2)
print(f"Conceptual similarity: {similarity:.3f}")
```

### Performance Monitoring

```python
import time

brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

start = time.perf_counter()
for i in range(1000):
    brain.step(i)
elapsed = time.perf_counter() - start

print(f"Simulation speed: {1000/elapsed:.1f} steps/second")
```

## License

MIT License. See LICENSE file for details.

## Authors

NLM Research Team

## Version

0.1.0 - Improved Python bindings with enhanced API and documentation

## Changelog

### Version 0.1.0
- Complete rewrite of Python bindings with template-based code generation
- Enhanced error handling and validation
- Comprehensive documentation and examples
- Improved CMake configuration
- Full test suite
- Advanced features for text processing and concept formation

## Acknowledgments

Special thanks to:
- The pybind11 team for the binding framework
- The CMake community for build tools
- Contributors who provided feedback and feature requests

## Support

For questions and support:
1. Check the examples and documentation
2. Run the test suite
3. Consult the issue tracker if problems persist
4. Join the NLM community for discussions

---

*Created with ❤️ for the NLM research project*
