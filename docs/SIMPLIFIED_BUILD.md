# Simplified NLM Build Configuration

## Quick Start Guide

### Prerequisites

```bash
# Install system dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake python3 python3-pip

# Install Python dependencies
pip install numpy pytest scikit-build-core pybind11
```

### Single Command Build

```bash
# Clone the repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build everything in one go
pip install .
```

### Alternative: Build with CMake Only

```bash
# Navigate to project
cd nlm

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the core library and demo executables
make -j4
```

### Running Demos

```bash
# Run the Phase 6 integration demo
./build/nlm_phase6_demo

# Or run the main executable
./build/nlm
```

### Python Usage

```python
import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run for 1000 simulation steps
for i in range(1000):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} firing neurons")
```

## Configuration Options

### Simple Configuration File

Create a `config.txt` file with key=value pairs:

```
neuron_count = 1000
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012
stdp_tau = 20.0
```

Load it with:

```python
config.loadFromFile("config.txt")
```

### Command Line Arguments

```bash
./nlm --neuron_count=5000 --region_count=4 --connection_probability=0.05 --simulation_timestep=0.0001
```

## Testing

### Unit Tests

```bash
# Run C++ unit tests
cd build
ctest

# Or run with verbose output
ctest -V
```

### Python Tests

```bash
pip install pytest numpy
pytest
```

## Performance Tips

### For Large Networks

1. **Increase regions**: Multiple regions reduce O(N²) complexity
2. **Lower connection probability**: Reduces total synapses
3. **Adjust neuron count**: Balance performance and biological realism
4. **Enable structural plasticity**: Maintains efficient connectivity

### Sample Configurations

```python
# High-performance config
config = pynlm.createDefaultConfig()
config.set("neuron_count", 2000)
config.set("region_count", 4)
config.set("connection_probability", 0.05)

# Realistic brain config
config.set("neuron_count", 10000)
config.set("region_count", 10)
config.set("connection_probability", 0.01)
config.set("stdp_ltp_weight", 0.005)
config.set("stdp_ltd_weight", 0.015)
```

## Common Issues and Solutions

### "My brain isn't doing anything"

**Solution**: Ensure `brain.initialize()` is called before `brain.step()`

### "The agent isn't moving"

**Solution**: Check that `world.update()` is called in your simulation loop

### "Everything is 0"

**Solution**: Brains need time to "warm up" - try more steps or add initial input

### Build Issues

**CMake cannot find Python**:
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**ImportError: No module named 'pynlm'**:
```bash
pip install --force-reinstall .
```

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

## Quick Reference

### Core Classes

| Class | Purpose |
|-------|---------|
| `Brain` | Main neural system |
| `Config` | Configuration management |
| `Action` | Motor commands |
| `World` | Environment simulation |

### Key Methods

| Method | Description |
|--------|-------------|
| `brain.initialize()` | Initialize the brain |
| `brain.step(step)` | Process one simulation step |
| `config.set(key, value)` | Set configuration parameter |
| `world.update(dt)` | Update world simulation |

That's it! You're now ready to use NLM for neural simulations.
