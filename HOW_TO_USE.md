# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the NLM Python API

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

# Build and install
pip install .

# Or install in development mode
pip install -e .
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
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

---

## Part 3: Enhanced Python Library Features

### 3.1 Advanced Error Handling

The improved Python bindings include comprehensive error handling:

```python
import pynlm

try:
    config = pynlm.Config()
    config.set("brain.neuron_count", -1)  # This will raise ValidationError
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
except pynlm.ValidationError as e:
    print(f"Validation error: {e}")
except pynlm.BrainError as e:
    print(f"Brain error: {e}")
except pynlm.NLMException as e:
    print(f"General NLM error: {e}")
```

### 3.2 Context Managers for Resource Management

Context managers ensure proper resource cleanup:

```python
import pynlm

# Manual resource management
config = pynlm.Config()
brain = pynlm.Brain(config)
world = pynlm.SimpleWorld()

# Resource management
config.loadFromFile("config.json")
brain.initialize(world)

# Manual cleanup
brain.save("checkpoint.bin")
config.saveToFile("backup.json")

# Enhanced with context managers (recommended)
with pynlm.Config() as config:
    with pynlm.SimpleWorld() as world:
        brain = pynlm.Brain(config)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Simulation code
        for step in range(100):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Resources automatically cleaned up
```

### 3.3 NumPy Integration

Seamless integration with NumPy for numerical operations:

```python
import pynlm
import numpy as np

# Create NumPy arrays for sensory input
vision_data = np.random.randn(8, 8, 3).astype(np.float32)
audio_data = np.random.randn(100).astype(np.float32)

# Use with Vision and Audio classes
vision = pynlm.Vision(8, 8, 3)
vision.setData(vision_data)  # Direct NumPy array support

# Convert back to NumPy
numpy_array = vision.getDataAsNumpy()

# NumPy mathematical operations
normalized = np.apply_along_axis(lambda x: (x - x.min()) / (x.max() - x.min() + 1e-8), axis=None, arr=numpy_array)
batch_normalized = pynlm.numpy_normalize(vision_data, 0.0, 1.0)

# Batch processing with NumPy
arrays = [np.random.randn(5, 5).astype(np.float32) for _ in range(10)]
batched_normalized = pynlm.numpy_batch_norm(arrays)

print(f"Original shape: {vision_data.shape}, dtype: {vision_data.dtype}")
print(f"NumPy array shape: {numpy_array.shape}, dtype: {numpy_array.dtype}")
```

### 3.4 Batch Operations for Performance

Efficient batch processing for large-scale simulations:

```python
import pynlm
import time

# Create simulation components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.SimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Method 1: Sequential processing (traditional)
print("Sequential processing:")
start_time = time.time()
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
sequential_time = time.time() - start_time
print(f"Sequential time: {sequential_time:.3f}s")

# Method 2: Batch processing (optimized)
print("Batch processing:")
percepts = []
for step in range(1000):
    world.update(0.1)
    percepts.append(world.getSensoryPercept())

# Process all percepts at once
batch_start = time.time()
actions = agent.processBatchPercepts(percepts)
brain.batchStep(list(range(1000)))
batch_time = time.time() - batch_start
print(f"Batch time: {batch_time:.3f}s")
print(f"Speedup: {sequential_time/batch_time:.2f}x")

# Method 3: Episode processing
print("Episode processing:")
percepts = []
timesteps = []
for step in range(500):
    world.update(0.1)
    percepts.append(world.getSensoryPercept())
    timesteps.append(0.1)

episode_start = time.time()
brain.processBatchEpisode(percepts, timesteps)
episode_time = time.time() - episode_start
print(f"Episode time: {episode_time:.3f}s")
```

### 3.5 Performance Monitoring and Debugging

Track and optimize simulation performance:

```python
import pynlm

# Create simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.SimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable monitoring
monitor = pynlm.startMonitoring()

# Run simulation with monitoring
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress every 200 steps
    if step % 200 == 0:
        metrics = brain.getPerformanceMetrics()
        progress = brain.getProgressInfo(1000)
        
        print(f"Step {step}:")
        print(f"  Performance: {progress['estimated_progress']:.1f}% complete")
        print(f"  Active neurons: {metrics['active_neurons']}")
        print(f"  Total spikes: {metrics['total_spikes']}")
        print(f"  Average firing rate: {metrics['average_firing_rate']:.3f}")
        print(f"  E/I ratio: {metrics['excitation_inhibition_ratio']:.3f}")
        print(f"  Curiosity level: {agent.curiosity_level:.3f}")
        print(f"  Novelty level: {agent.novelty_level:.3f}")

# Stop monitoring
pynlm.stopMonitoring(monitor)

print("Final performance metrics:", brain.getPerformanceMetrics())
```

### 3.6 Visualization and Reporting

Create visualizations and reports from simulation data:

```python
import pynlm
import numpy as np

# Run simulation and collect data
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.SimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# Add some objects for visualization
for i in range(10):
    obj = pynlm.WorldObject(i * 1.5, i * 1.5, pynlm.WorldObjectType.Resource, value=1.0)
    world.addObject(obj)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Collect data for visualization
brain_activity = []
sensory_data = []
connectivity = []

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Collect brain activity
    firing_rate = brain.getFiringNeuronCount() / max(brain.getTotalNeuronCount(), 1)
    brain_activity.append((step * 0.1, firing_rate))
    
    # Collect sensory data
    vision_data = percept.getVision().getDataAsFlat()
    sensory_data.append(vision_data)
    
    # Collect some connectivity data (sample)
    if step % 10 == 0:
        for i in range(20):
            j = (i * 7 + step) % brain.getTotalNeuronCount()
            if i != j:
                connectivity.append((i, j))

# Generate visualizations
brain.visualizeBrainActivity("brain_activity.csv", brain_activity)
brain.visualizeSensoryInput("sensory_input.csv", sensory_data)
brain.visualizeNeuralConnectivity("connectivity.csv", connectivity, brain.getTotalNeuronCount())

# Generate comprehensive report
sections = {
    "Simulation Overview": f"Duration: 100 steps, Total neurons: {brain.getTotalNeuronCount()}, Total synapses: {brain.getTotalSynapseCount()}",
    "Performance Summary": f"Average firing rate: {brain.getAverageFiringRate():.3f}, E/I ratio: {brain.getExcitationInhibitionRatio():.3f}",
    "Development": f"Stage: {brain.developmental_stage}, Age: {brain.getConfig().getOr('brain.age', 0)}",
    "Neuromodulation": f"Curiosity: {agent.curiosity_level:.3f}, Novelty: {agent.novelty_level:.3f}, Prediction error: {agent.prediction_error:.3f}",
    "Recommendations": "Consider increasing neuron density for faster learning, or enabling structural plasticity for exploration."
}

brain.generateReport("simulation_report.md", "NLM Advanced Simulation Report", sections)

print("Visualizations and report generated successfully!")
```

### 3.7 Pythonic API Features

Enhanced Pythonic interface for better usability:

```python
import pynlm

# Create and configure brain
config = pynlm.Config()
brain = pynlm.Brain(config)

# Pythonic property access
print(f"Brain size: {len(brain)} neurons")
print(f"Active neurons: {brain.active_neurons}")
print(f"Firing neurons: {brain.firing_neurons}")
print(f"Total spikes: {brain.total_spikes}")
print(f"Average firing rate: {brain.average_firing_rate:.3f}")
print(f"E/I ratio: {brain.excitation_inhibition_ratio:.3f}")

# Pythonic string representations
print(f"Brain representation: {brain}")
print(f"Config summary: {config}")

# Pythonic iteration and operations
neurons = list(range(brain.getTotalNeuronCount()))
print(f"First 10 neuron IDs: {neurons[:10]}")

# Special methods
brain_copy = brain  # Shallow copy (in real implementation would be deep copy)
print(f"Brain equals itself: {brain == brain}")
print(f"Brain string length: {len(str(brain))}")

# Context manager for automatic cleanup
with pynlm.Config() as config:
    brain = pynlm.Brain(config)
    print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
    # Brain automatically cleaned up

# Pythonic debugging
brain_state = brain.debugNeuronState(pynlm.INVALID_NEURON_ID)
print(f"Brain debug info: {brain_state}")
```

### 3.8 Advanced Configuration Management

Enhanced configuration capabilities:

```python
import pynlm

# Create configuration from template
config = pynlm.createDefaultConfig()

# Get configuration templates
templates = pynlm.getConfigurationTemplates()
print(f"Available templates: {list(templates.keys())}")

# Apply template
for name, template in templates.items():
    print(f"Template '{name}': {template.summary()}")

# Enhanced validation and merging
config.validate()  # Validate configuration

# Merge configurations
config1 = pynlm.createDefaultConfig()
config1.set("brain.width", 2000)

config2 = pynlm.createDefaultConfig()
config2.set("brain.height", 1000)

config1.merge(config2)  # Merge config2 into config1
print(f"Merged config: {config1.summary()}")

# Configuration sections
brain_config = config.getSection("brain")
print(f"Brain configuration: {brain_config}")
```

---
