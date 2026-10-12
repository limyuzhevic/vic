# NLM (Neural Learning Machine) - HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the enhanced Python library
3. Using the enhanced NLM Python API with new features

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

## Part 2: Installing the Enhanced Python Library

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
print(pynlm.__version__ if hasattr(pynlm, '__version__') else "NLM Python API")

# Create a simple brain using enhanced presets
config = pynlm.createConfigFromPreset("standard")
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")

# Test enhanced features
brain.initialize()
brain.step(0)
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

---

## Part 3: Using the Enhanced Python Library

### Enhanced Basic Usage

#### Creating a Brain with Presets

```python
import pynlm

# Create using preset - no need to configure details
brain = pynlm.createBrainFromPreset("standard")
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation with Monitoring

```python
import pynlm
from pynlm_enhanced import run_simulation_loop, SimulationMonitor

# Create brain with preset
brain = pynlm.createBrainFromPreset("research")
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)

# Create agent with learning features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with monitoring
monitor = SimulationMonitor(brain, agent, world)
run_simulation_loop(
    brain, world, agent,
    steps=500, timestep=0.001,
    monitor=monitor,
    verbose=True
)

# Get performance summary
print("\n=== Simulation Summary ===")
print(f"Total steps: {monitor.get_summary()['total_steps']}")
print(f"Average step time: {monitor.get_summary()['avg_step_time_ms']:.2f} ms")
print(f"Steps per second: {monitor.get_summary()['steps_per_second']:.0f}")
```

#### Advanced Configuration with Presets

```python
import pynlm
from pynlm_enhanced import createConfigFromPreset

# Create configuration with custom parameters
config = createConfigFromPreset(
    "large",
    neuron_count=3000,  # Override preset
    plasticity_stdp_enable=True,
    neuromod_curiosity_enable=True,
    development_enable=True
)

# Save configuration
config.saveToFile("my_research_config.json")

# Load configuration later
new_config = pynlm.Config()
new_config.loadFromFile("my_research_config.json")
```

### Complete Agent Example with Enhanced Features

```python
import pynlm
from pynlm_enhanced import (
    createConfigFromPreset, 
    createBrainFromPreset, 
    run_simulation_loop,
    SimulationMonitor,
    analyze_brain_state,
    create_agent_with_features
)

def run_research_simulation(num_steps=2000):
    """Run a complete enhanced agent simulation."""
    
    # 1. Create configuration with research preset
    config = createConfigFromPreset("research")
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create enhanced agent with all features
    agent = create_agent_with_features(
        brain, world,
        enable_reward=True,
        enable_curiosity=True,
        enable_development=True,
        enable_plasticity=True
    )
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(25.0, 25.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Create monitor for performance tracking
    monitor = SimulationMonitor(brain, agent, world)
    
    # 7. Run simulation loop with monitoring
    run_simulation_loop(
        brain, world, agent, num_steps,
        timestep=0.001, monitor=monitor, verbose=True
    )
    
    # 8. Analyze results
    analysis = analyze_brain_state(brain)
    
    print("\n=== Research Simulation Results ===")
    print(f"Simulation steps: {num_steps}")
    print(f"Final firing rate: {analysis['average_firing_rate']:.2f}")
    print(f"Total spikes: {analysis['total_spikes']}")
    print(f"Excitation/Inhibition ratio: {analysis['excitation_inhibition_ratio']:.3f}")
    print(f"Development stage: {analysis['developmental_stage']}")
    
    # 9. Save experiment data
    experiment_data = {
        "steps": num_steps,
        "config": analysis,
        "performance": monitor.get_summary(),
        "agent_state": {
            "curiosity_level": agent.getCuriosityLevel(),
            "novelty_level": agent.getNoveltyLevel(),
        }
    }
    
    import json
    with open(f"experiment_research_{num_steps}.json", "w") as f:
        json.dump(experiment_data, f, indent=2)
    
    return analysis, monitor

# Run the research simulation
results, performance = run_research_simulation(2000)
```

### Key Classes and Methods (Enhanced)

#### Brain Class

```python
# Creation with presets
brain = pynlm.createBrainFromPreset("standard")
brain = pynlm.createBrain(config)  # Traditional way

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(input)  # Inject sensory data
action = brain.produceAction()    # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")      # Save state
brain.load("checkpoint.bin")      # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
# Create with presets
config = pynlm.createConfigFromPreset("large")
config = pynlm.createDefaultConfig()  # Traditional way

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class (Enhanced)

```python
# Creation with presets
brain = pynlm.createBrainFromPreset("standard")
agent = pynlm.createAgentBrain(brain)

# Initialize
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Enhanced feature management
agent = create_agent_with_features(
    brain, world,
    enable_reward=True,
    enable_curiosity=True,
    enable_development=True
)

# Feature status
agent.isRewardModulationEnabled()
agent.isCuriosityEnabled()
agent.isDevelopmentEnabled()
```

#### SimpleWorld Class

```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Modulatory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat
# ... and more

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall
pynlm.WorldObjectType.Marker
```

---

## Part 4: Enhanced Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |
| `brain.v_reset` | float | -75.0 | Reset potential after spike (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Enhanced Configuration Presets

The enhanced API provides several preset configurations:

#### `standard` (neuron_count=1000)
- Balanced configuration for general use
- All plasticity mechanisms enabled
- Moderate neuromodulation

#### `large` (neuron_count=5000)  
- Detailed simulations with more neurons
- All plasticity mechanisms enabled
- Enhanced neuromodulation
- Structural plasticity enabled

#### `small` (neuron_count=100)
- Quick testing with minimal resources
- Basic plasticity
- Minimal neuromodulation

#### `research` (neuron_count=10000)
- Advanced research configuration
- All plasticity mechanisms enabled
- Full neuromodulation
- Advanced development features
- Optimized for research experiments

---

## Part 5: Enhanced Example Scripts

### Minimal Example (Traditional)

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Minimal Example (Enhanced with Presets)

```python
import pynlm

# Create and initialize using preset
brain = pynlm.createBrainFromPreset("small")
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Environment Interaction (Enhanced)

```python
import pynlm
from pynlm_enhanced import create_agent_with_features

# Setup
brain = pynlm.createBrainFromPreset("standard")
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = create_agent_with_features(brain, world)
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### Performance Monitoring

```python
import pynlm
from pynlm_enhanced import SimulationMonitor, run_simulation_loop

# Create brain and world
brain = pynlm.createBrainFromPreset("large")
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Run simulation with monitoring
monitor = SimulationMonitor(brain, agent, world)
run_simulation_loop(
    brain, world, agent,
    steps=1000, timestep=0.001,
    monitor=monitor
)

# Get performance summary
print("Performance Summary:")
print(f"Total steps: {monitor.get_summary()['total_steps']}")
print(f"Average step time: {monitor.get_summary()['avg_step_time_ms']:.2f} ms")
```

---

## Part 6: Troubleshooting

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

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Enhanced Error Handling

```python
import pynlm
from pynlm_enhanced import InvalidConfigurationError

try:
    # This will raise an error for unknown preset
    brain = pynlm.createBrainFromPreset("unknown")
except ValueError as e:
    print(f"Configuration error: {e}")

try:
    # Create brain and run simulation
    brain = pynlm.createBrainFromPreset("standard")
    brain.initialize()
    
    for i in range(100):
        brain.step(i)  # Will fail if brain not initialized
        
except RuntimeError as e:
    print(f"Runtime error: {e}")
```

---

## Part 7: Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [Enhanced API Documentation](https://nlm.readthedocs.io/en/latest/python/enhanced_api.html) - Comprehensive Python API documentation

The enhanced Python API provides many new features for both beginners and advanced users. The preset system makes it easy to get started, while the monitoring and analysis tools provide deep insights into simulation performance.

## Enhanced API Version

This enhanced version includes:
- Configuration presets for easy setup
- Performance monitoring tools
- Brain state analysis utilities
- Enhanced error handling
- Backward compatibility with traditional API
- Additional modules in `pynlm_enhanced.py` for advanced features