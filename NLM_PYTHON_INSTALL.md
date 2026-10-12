# NLM Python Installation Guide

This guide provides comprehensive instructions for installing and using the NLM Python bindings.

## Prerequisites

Before installing NLM Python bindings, ensure you have:

- Python 3.8 or higher
- C++ compiler (GCC 10+, Clang 12+, or MSVC 2019+)
- CMake 3.16 or higher
- pip (Python package installer)

## Installation Methods

### 1. Quick Install (Recommended for Users)

The easiest way to install NLM is using pip:

```bash
pip install .
```

This will automatically compile the C++ extensions and install the `pynlm` package.

### 2. Development Installation

For development and testing, use editable install:

```bash
pip install -e .
```

This installs the package in development mode, allowing you to modify Python files without reinstallation.

### 3. Build from Source

For custom builds or troubleshooting, you can build the package manually:

```bash
pip install build
python -m build
pip install dist/*.whl
```

### 4. Alternative: Using scikit-build directly

If you prefer using scikit-build directly:

```bash
pip install scikit-build-core pybind11 numpy pytest
pip install .
```

## Verification

After installation, verify that NLM is properly installed:

```python
import pynlm

print(f"NLM version: {pynlm.__version__}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run a simple simulation
brain.step(0)
print(f"Brain simulation step successful")
print(f"Active neurons: {brain.getFiringNeuronCount()}")
```

## Common Installation Issues

### Issue: CMake cannot find Python

If you encounter CMake Python detection errors, install dependencies and specify Python:

```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

### Issue: No module named 'pynlm'

If the module import fails:

```bash
pip install --force-reinstall .
```

### Issue: Build errors

For complex build errors:

```bash
# Clean build
rm -rf build dist
pip install -e . --no-build-isolation
```

## Development Setup

### For Development

Create a virtual environment for development:

```bash
python -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate
pip install -e .[dev]
```

### Running Tests

Run the Python test suite (if available):

```bash
pytest
```

## Configuration

The NLM Python bindings use C++ configuration objects. Common configurations:

```python
import pynlm

# Default configuration
config = pynlm.createDefaultConfig()

# Custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")
```

## Troubleshooting

### "Brain isn't doing anything"
- Ensure `brain.initialize()` is called before `brain.step()`
- Check neuron parameters and input injection
- Try more simulation steps

### "ImportError: No module named 'pynlm'"
- Run installation with `--force-reinstall`
- Check Python path
- Verify pip installation

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters

## Performance Tips

### For Large Networks
```python
import pynlm
import time

config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()

start_time = time.time()
for step in range(1000):
    brain.step(step)

elapsed = time.time() - start_time
print(f"Simulation: {1000/elapsed:.0f} steps/second")
```

### Using Efficient Data Structures
```python
# For batch operations
inputs = [30.0f] * 10
for step in range(100):
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, inputs)
    brain.step(step)
```

## Advanced Features

### Saving and Loading
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Save brain state
brain.save("checkpoint.bin")

# Run simulation
for step in range(100):
    brain.step(step)

# Load brain state
brain.load("checkpoint.bin")
```

### Multi-threaded Usage
```python
import pynlm
import threading
import concurrent.futures

def run_simulation(brain_config):
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    for step in range(100):
        brain.step(step)
    return brain.getTotalSpikeCount()

# Parallel simulation execution
config = pynlm.createDefaultConfig()
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as executor:
    futures = [executor.submit(run_simulation, config) for _ in range(4)]
    results = [future.result() for future in futures]
```

## Next Steps

After installation:

1. **Read `easy_usage.md`** - Beginner-friendly usage guide
2. **Read `HOW_TO_USE.md`** - Detailed API documentation
3. **Read `docs/ARCHITECTURE.md`** - System architecture overview
4. **Read `docs/SCIENCE.md`** - Scientific background
5. **Experiment with different configurations** using the examples above
6. **Check `src/` directory** for C++ source code
7. **Review `tests/` directory** for unit tests

## Getting Help

If you encounter issues:

1. **Check the error messages carefully** - they often indicate the exact problem
2. **Verify your installation** with the test code above
3. **Review the documentation** in this folder
4. **Check the GitHub repository** for issues and discussions
5. **Create a GitHub issue** with detailed error information

## Related Projects

- **NLM C++ Documentation**: Check the `docs/` folder for detailed C++ documentation
- **Phase Demos**: Run `./nlm_phase4_demo` for comprehensive integration testing
- **Unit Tests**: Run `ctest --output-on-failure` in the build directory
- **Scaling Benchmarks**: Check `src/benchmark.cpp` for performance data

## License

MIT License. See the LICENSE file for details.
