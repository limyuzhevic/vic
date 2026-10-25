# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API
4. Enhanced Python bindings and utilities

---

## Part 1: Building the C++ Project from Source

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **pybind11**: Version 2.11.0+ (for Python bindings)

### Build Steps

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you will have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |
| `nlm_test` | Executable | Unit test suite |

### Build Options

```bash
# Debug build (with symbols and assertions)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build with specific compiler
cmake .. -DCMAKE_CXX_COMPILER=g++ -DCMAKE_C_COMPILER=gcc

# Install to custom prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
make install
```

---

## Part 2: Installing the Python Library

### From Source (Recommended for Development)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install the enhanced Python bindings
pip install -e .

# Or install in development mode
pip install -e .[dev]
```

### Using pyproject.toml (Modern Python Packaging)

```bash
# Install with scikit-build-core (used automatically by pip)
pip install .

# Build only (without installing)
pip install build
python -m build

# Install specific build from wheel
pip install dist/*.whl
```

### Verifying Installation

```python
import pynlm_enhanced as nlm
import pynlm_utils as utils

# Check version
print(f"NLM Enhanced version: {nlm.__version__}")

# Create a simple brain using enhanced API
with nlm.create_enhanced_brain() as brain:
    print(f"Brain created with {brain.get_total_neuron_count()} neurons")
```

---

## Part 3: Using the Enhanced Python Library

### Enhanced Python Bindings Features

The enhanced bindings provide **Pythonic APIs** for better resource management and developer experience:

- **Context managers** for automatic resource cleanup
- **Property-based access** for cleaner code
- **Better error handling** with specific exception types
- **Type hints** and validation
- **Enhanced simulation utilities** and progress tracking

### Basic Usage with Enhanced Bindings

#### Creating an Enhanced Brain

```python
import pynlm_enhanced as nlm

# Create enhanced brain with context manager
with nlm.create_enhanced_brain() as brain:
    # Brain is automatically initialized
    brain.initialize()
    
    print(f"Total neurons: {brain.get_total_neuron_count()}")
    print(f"Total synapses: {brain.get_total_synapse_count()}")
```

#### Enhanced World Configuration

```python
import pynlm_enhanced as nlm

# Create enhanced world
with nlm.create_enhanced_world() as world:
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    
    world.reset()
    world.set_agent_start(10.0, 10.0)
```

#### Enhanced Agent Brain

```python
import pynlm_enhanced as nlm

# Create agent brain within world context
with nlm.create_enhanced_brain() as brain:
    with nlm.create_enhanced_world() as world:
        with nlm.create_enhanced_agent_brain(brain) as agent:
            agent.initialize(world)
            
            # Enable learning features
            agent.enable_reward_modulation(True)
            agent.enable_curiosity(True)
            agent.enable_development(True)
```

### Enhanced Simulation with Utilities

```python
import pynlm_enhanced as nlm
import pynlm_utils as utils

def run_enhanced_simulation(brain, world, agent, num_steps=1000):
    """Run simulation with enhanced utilities."""
    
    # Create statistics collector
    stats = utils.SimulationStats()
    
    # Create progress tracker
    tracker = utils.ProgressTracker(num_steps, "Enhanced Simulation")
    
    # Run simulation
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Process sensory input
        percept = world.get_sensory_percept()
        agent.process_sensory_input(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Decode and apply action
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply neuromodulation
        reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Track statistics
        stats.add_step_stats(
            step,
            brain.get_firing_neuron_count(),
            brain.get_average_firing_rate(),
            agent.get_curiosity_level(),
            agent.get_novelty_level(),
            agent.get_prediction_error(),
            agent.get_neuromodulation_level(),
            brain.get_developmental_stage(),
            0.1
        )
        
        # Update progress
        tracker.update(step, f"Step {step}, Firing: {brain.get_firing_neuron_count()}")
    
    tracker.finish()
    
    return stats
```

### Configuration Management

```python
import pynlm_enhanced as nlm
import pynlm_utils as utils

# Create enhanced configuration
config = nlm.create_enhanced_config()

# Load and validate configuration
try:
    config.load_from_file("config.json")
    if config.validate():
        print("Configuration is valid")
    else:
        print("Configuration is invalid")
except nlm.ConfigurationError as e:
    print(f"Configuration error: {e}")

# Set configuration values
config.set_int("brain.neuron_count", 2000)
config.set_double("brain.synapse_density", 0.15)
config.set_double("brain.v_thresh", -50.0)

# Save configuration
config.save_to_file("my_config.json")

# Convert to dictionary for debugging
config_dict = utils.config.to_dict(config)
```

### Error Handling

```python
import pynlm_enhanced as nlm

try:
    # Create and use brain
    with nlm.create_enhanced_brain() as brain:
        brain.initialize()
        brain.step(0)
        print("Simulation successful")
        
except nlm.SimulationError as e:
    print(f"Simulation failed: {e}")
    
except nlm.ConfigurationError as e:
    print(f"Configuration failed: {e}")
    
except nlm.NLMError as e:
    print(f"NLM error: {e}")
    
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Simulation Statistics

```python
import pynlm_utils as utils

# Create statistics collector
stats = utils.SimulationStats()

# Add simulation data
stats.add_step_stats(
    step=10,
    firing_neurons=150,
    firing_rate=7.5,
    curiosity=0.8,
    novelty=0.6,
    prediction_error=0.2,
    neuromodulation_level=0.9,
    developmental_stage=nlm.DevelopmentalStage.Maturation,
    step_time=0.1
)

# Export to dictionary
stats_dict = stats.to_dict()
print(f"Average firing rate: {stats_dict['average_firing_rate']:.3f}")
print(f"Max firing neurons: {stats_dict['max_firing_neurons']}")
```

### Progress Tracking

```python
import pynlm_utils as utils

# Create progress tracker
tracker = utils.ProgressTracker(1000, "NLM Simulation")

for step in range(1000):
    # Your simulation code here
    
    # Update progress (returns percentage)
    progress = tracker.update(step, f"Firing: {step * 10}")
    
    if progress == 1.0:
        break

tracker.finish()
```

## Part 4: Configuration Options

### Enhanced Configuration Access

The enhanced configuration provides **Pythonic access methods**:

```python
import pynlm_enhanced as nlm

config = nlm.create_enhanced_config()

# Pythonic property access (equivalent to has())
if config.has("brain.neuron_count"):
    neurons = config.get_int("brain.neuron_count")

# Direct setting with validation
config.set_int("brain.neuron_count", 1000)
config.set_double("brain.synapse_density", 0.1)
```

## Part 5: Example Scripts

### Basic Examples

The `examples/` directory contains comprehensive demonstration scripts:

- `basic_brain_simulation.py` - Basic enhanced brain simulation
- `agent_world_interaction.py` - Complete agent-world interaction
- `visualization_demo.py` - Data collection and visualization preparation
- `performance_benchmark.py` - Performance comparison
- `advanced_features.py` - Advanced usage patterns

Each example demonstrates different aspects of the enhanced bindings and utilities.

### Example Usage

```bash
# Run examples
python examples/basic_brain_simulation.py
python examples/agent_world_interaction.py
python examples/visualization_demo.py
python examples/performance_benchmark.py
python examples/advanced_features.py
```

## Troubleshooting

### Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify all source files are present

### Runtime Issues

**ImportError: No module named 'pynlm_enhanced':**
```bash
pip install --force-reinstall .
```

**ImportError: No module named 'pynlm_utils':**
```bash
pip install --force-reinstall .
```

**ImportError: No module named 'pynlm':** (basic bindings)
```bash
# Install basic bindings separately
pip install pynlm-legacy
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Python-Specific Issues

**Context manager errors:**
```python
# Make sure you're using enhanced bindings
import pynlm_enhanced as nlm

# Not using enhanced bindings
import pynlm  # This will use basic bindings
```

**Statistics collection errors:**
```python
# Make sure you're using pynlm_utils
import pynlm_utils as utils

# Not importing utilities
# This will cause AttributeError
```

### Performance Issues

**Long simulation times:**
```python
# Use progress tracker for long simulations
import pynlm_utils as utils
tracker = utils.ProgressTracker(10000, "Long Simulation")
```

**Memory usage:**
```python
# Clear statistics periodically
stats = utils.SimulationStats()
# Use stats.clear() periodically for long simulations
```

## Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [examples/README.md](examples/README.md) - Example scripts documentation

### Additional Resources

- **Basic API Reference**: Available in the legacy `pynlm` module
- **Enhanced API Reference**: Available in `pynlm_enhanced` module
- **Utility Functions**: Available in `pynlm_utils` module
- **Configuration Examples**: Check `examples/advanced_features.py`
- **Performance Examples**: Check `examples/performance_benchmark.py`