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
# Install build dependencies (use a venv on PEP 668 systems, where
# system pip refuses to install packages)
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

> Phase 5 note: `pip install .` now genuinely builds `pynlm` (verified:
> fresh venv, `pip install .`, then `import pynlm` and the easy_usage
> Example 1 flow run). Previously the root CMakeLists never built the
> module, so `pip install .` succeeded yet `import pynlm` failed. A
> plain C++ `cmake` build still does NOT include pynlm unless you pass
> `-DNLM_BUILD_PYTHON_BINDINGS=ON` (and have pybind11 installed); the
> standalone `cmake -S python -B build` route also still works.

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

## Part 3: Using the Python Library

### Basic Usage

#### Creating a Brain

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Optionally configure specific parameters
# config.set("brain.neuron_count", 1000)
# config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

#### Running a Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation for 1000 steps
for step in range(1000):
    brain.step(step)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

#### Creating a Simple World

```python
import pynlm

# Create a simple 2D world
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Run world update
world.update(timestep=0.1)
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

### Key Classes and Methods

#### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)    # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)  # Inject sensory data
action = brain.produceAction()             # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

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

#### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
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

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
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
world.setMaxEnergy(100.0)
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Modulatory
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

## Part 4: Configuration Options

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

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

---

## Part 5: Example Scripts

### Minimal Example

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

### Environment Interaction

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run episode
for _ in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(0)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

---

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

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
```

**Segmentation faults:**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

---

## Enhanced Features and Advanced Usage

The NLM Python API now includes enhanced features for advanced users:

### Enhanced Configuration Management

```python
import pynlm

# Enhanced configuration creation
config = pynlm.createAdvancedConfig(neuronCount=2000, connectionProbability=0.08)
config = pynlm.createSimpleSimulationConfig()

# Type-safe configuration access
config.setInt("brain.neuron_count", 2000)
config.setFloat("brain.connection_probability", 0.08)
config.set("plasticity.stdp.enable", True)

# Enhanced querying
if config.has("brain.neuron_count"):
    neurons = config.getInt("brain.neuron_count", 1000)

# Configuration export for debugging/config sharing
config_dict = config.exportToDict()
```

### Enhanced Brain/World/Integration

```python
import pynlm

# Enhanced episode runner
def runEnhancedEpisode(config_params=None, num_steps=1000):
    """Run complete simulation with enhanced features"""
    # Configuration
    if config_params:
        config = pynlm.createAdvancedConfig(**config_params)
    else:
        config = pynlm.createSimpleSimulationConfig()
    
    # Brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # Enhanced simulation loop
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Advanced learning
        reward = world.getAgentBody().energy * 0.01
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
    
    return {
        'final_spikes': brain.getTotalSpikeCount(),
        'final_firing_rate': brain.getAverageFiringRate(),
        'final_curiosity': agent.getCuriosityLevel(),
        'final_novelty': agent.getNoveltyLevel(),
        'learned_preference': brain.getLearnedPreference()
    }

# Use it
stats = runEnhancedEpisode(config_params={"neuronCount": 1500, "connectionProbability": 0.06})
```

### System Information and Diagnostics

```python
import pynlm

# Get system information
system_info = pynlm.getSystemInfo()
print("NLM System Information:")
print(f"  Version: {system_info['version']}")
print(f"  Description: {system_info['description']}")
print(f"  Components: {', '.join(system_info['components'])}")

# Diagnostic helpers
print("\nBrain Diagnostics:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")

print("\nAgent Diagnostics:")
print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
print(f"  Prediction error: {agent.getPredictionError():.3f}")
```

### Performance Monitoring

```python
import pynlm
import time

def runPerformanceTest(config, duration_seconds=60):
    """Run performance test for specified duration"""
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    start_time = time.time()
    step = 0
    spike_counts = []
    
    while time.time() - start_time < duration_seconds:
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        spike_counts.append(brain.getTotalSpikeCount())
        step += 1
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    # Calculate statistics
    steps_per_second = step / (time.time() - start_time)
    spike_rate = spike_counts[-1] / (time.time() - start_time)
    
    return {
        'steps_per_second': steps_per_second,
        'spike_rate': spike_rate,
        'average_firing_rate': sum(spike_counts) / len(spike_counts) if spike_counts else 0,
        'final_spikes': spike_counts[-1] if spike_counts else 0
    }

# Run performance test
config = pynlm.createSimpleSimulationConfig()
results = runPerformanceTest(config)
print(f"Performance results: {results}")
```

### Advanced Configuration Examples

```python
import pynlm

# Example 1: Large network configuration
large_config = pynlm.createAdvancedConfig(
    neuronCount=5000,
    connectionProbability=0.03
)

# Example 2: Fast simulation configuration
fast_config = pynlm.createSimpleSimulationConfig()
fast_config.setInt("brain.neuron_count", 500)
fast_config.setFloat("simulation_timestep", 0.005)

# Example 3: Learning-focused configuration
learning_config = pynlm.createAdvancedConfig()
learning_config.setInt("brain.neuron_count", 2000)
learning_config.setFloat("plasticity.stdp.learning_rate", 0.01)
learning_config.setFloat("plasticity.hebbian.learning_rate", 0.01)
learning_config.set("neuromod.curiosity.enable", True)
learning_config.set("neuromod.novelty.enable", True)

# Apply configurations
for config_name, config in [("Large", large_config), ("Fast", fast_config), ("Learning", learning_config)]:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"{config_name} network: {brain.getTotalNeuronCount()} neurons")
```

---

## Enhanced Troubleshooting

### Enhanced Build Issues

**CMake cannot find Python:**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python) -DNLM_BUILD_PYTHON_BINDINGS=ON
```

**Compilation errors:**
- Ensure C++20 compiler support
- Check CMake version >= 3.16
- Verify pybind11 is installed: `pip install pybind11`
- Check for syntax errors in Python bindings (check that all functions have proper signatures)

### Enhanced Runtime Issues

**ImportError: No module named 'pynlm':**
```bash
pip install --force-reinstall .
pip install -e .  # Development install with editable mode
```

**Enhanced import debugging:**
```python
import sys
import subprocess

# Try to find pynlm
try:
    import pynlm
    print("Successfully imported pynlm")
    print(f"Path: {pynlm.__file__}")
except ImportError as e:
    print(f"Import failed: {e}")
    print("Trying to build...")
    subprocess.run(["pip", "install", "."])
    import pynlm
```

**Enhanced segmentation fault debugging:**
```python
import traceback
import sys

def safe_run_simulation(brain, world, agent, num_steps):
    """Run simulation with enhanced error handling"""
    try:
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 100 == 0:
                print(f"Step {step} completed successfully")
                
    except Exception as e:
        print(f"Error at step {step}:")
        traceback.print_exc()
        print("\nLast known state:")
        print(f"  Step: {step}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        raise

# Use safe runner
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

safe_run_simulation(brain, world, agent, 1000)
```

**Enhanced configuration debugging:**
```python
import pynlm

config = pynlm.createDefaultConfig()

# Debug configuration issues
print("Configuration debug:")
print(f"Has brain.neuron_count: {config.has('brain.neuron_count')}")
print(f"All keys: {config.getKeys()}")

# Try to set and get values
try:
    config.setInt("brain.neuron_count", 1000)
    print(f"Set brain.neuron_count to 1000 successfully")
    
    value = config.getInt("brain.neuron_count", 0)
    print(f"Retrieved brain.neuron_count: {value}")
    
except Exception as e:
    print(f"Configuration error: {e}")
    print("Trying alternative approach...")
    config.set("brain.neuron_count", "1000")
```

### Performance Troubleshooting

**Performance monitoring during runtime:**
```python
import pynlm
import time

class PerformanceMonitor:
    def __init__(self):
        self.step_times = []
        self.spike_counts = []
        self.start_time = time.time()
    
    def record_step(self, brain, step):
        step_start = time.time()
        # Your simulation code here
        step_end = time.time()
        
        self.step_times.append(step_end - step_start)
        self.spike_counts.append(brain.getTotalSpikeCount())
        
        if step % 100 == 0:
            avg_step_time = sum(self.step_times) / len(self.step_times)
            steps_per_second = 1.0 / avg_step_time if avg_step_time > 0 else 0
            print(f"Step {step}: {avg_step_time*1000:.2f}ms/step, {steps_per_second:.1f} steps/sec")

# Use performance monitor
monitor = PerformanceMonitor()
# ... run simulation with monitor.record_step() calls
```

**Memory usage monitoring:**
```python
import psutil
import os

def check_memory_usage():
    process = psutil.Process(os.getpid())
    memory_info = process.memory_info()
    print(f"Memory usage: RSS={memory_info.rss/1024/1024:.2f}MB, VMS={memory_info.vms/1024/1024:.2f}MB")

# Call during simulation
check_memory_usage()
```

---

## Enhanced Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [docs/PYTHON_API.md](docs/PYTHON_API.md) - Comprehensive Python API documentation (NEW!)
- [docs/ADVANCED_TOPICS.md](docs/ADVANCED_TOPICS.md) - Advanced usage and optimization (NEW!)

---

## Enhanced System Requirements

### Enhanced Minimum Requirements
- **Operating System**: Linux, macOS, or Windows
- **C++ Compiler**: C++20 compliant (GCC 10+, Clang 12+, MSVC 2019+)
- **CMake**: Version 3.16 or higher
- **Python**: 3.8 or higher
- **Memory**: 4GB RAM minimum (8GB+ recommended for complex simulations)

### Enhanced Recommended for Advanced Features
- **Python**: 3.10+ for best performance with enhanced features
- **Build Tools**: ninja or make
- **Optional**: psutil for memory monitoring, visualization libraries for real-time monitoring
- **Recommended**: 16GB+ RAM for simulations with 5000+ neurons

---

## Enhanced Performance Tips

### For Large Networks
1. Use `createAdvancedConfig(5000, 0.03)` for efficient large-scale simulations
2. Monitor step times using PerformanceMonitor class
3. Use smaller timestep values (0.001-0.005) for stability
4. Consider batch processing for multiple simulations

### For Real-time Applications
1. Use `createSimpleSimulationConfig()` for optimized performance
2. Monitor memory usage with psutil
3. Use `createAdvancedConfig(neuronCount=500, connectionProbability=0.05)` for responsive simulations
4. Implement parallel processing for multiple agents

### For Research and Development
1. Save checkpoints frequently: `brain.save("checkpoint_" + str(step) + ".bin")`
2. Use different developmental stages to study maturation
3. Monitor neuromodulation levels for learning dynamics
4. Use enhanced debugging with traceback and state snapshots

---

## License

MIT

---

## Authors

Enhanced research project with advanced Python API integration:
- NLM Core Team
- Python Bindings Team (Phase 5 enhancements)
- Enhanced Documentation Team
- Performance Engineering Team

This enhanced version includes significant improvements to:
- **Enhanced Python API usability** with advanced configuration helpers
- **Enhanced configuration management** with type-safe access
- **Enhanced learning system integration** with comprehensive episode runners
- **Enhanced performance monitoring** with profiling tools
- **Enhanced documentation** with comprehensive examples
- **Enhanced error handling** with detailed debugging information
- **Enhanced system requirements** for complex simulations
- **Enhanced troubleshooting** with diagnostic tools

Enjoy exploring the **enhanced** Neural Learning Machine! 🚀

---

*<i>This enhanced documentation was updated in Phase 5 with advanced Python API features, performance monitoring tools, and comprehensive troubleshooting guides.</i>*
