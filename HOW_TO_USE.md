# NLM (Neural Learning Machine) - Enhanced HOW TO USE

## Overview

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

This document covers:
1. Building the C++ project from source
2. Installing the Python library
3. Using the enhanced NLM Python API

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

## Part 3: Using the Enhanced Python Library

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

### Enhanced Complete Agent Example

```python
import pynlm
import time

def run_agent_simulation_enhanced(num_steps=1000):
    """Run a complete agent simulation with enhanced features."""
    
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
    
    # 6. Enable all learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    print(f"Starting enhanced simulation with {num_steps} steps...")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
    # 7. Run simulation loop with monitoring
    start_time = time.time()
    
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
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Enhanced monitoring every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Simulation time: {world.getSimulationTime():.2f}s")
    
    end_time = time.time()
    
    # 8. Print comprehensive final statistics
    print(f"\nEnhanced simulation complete!")
    print(f"Total time: {end_time - start_time:.2f} seconds")
    print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
    print(f"Total simulation time: {world.getSimulationTime():.2f} seconds")
    print(f"Total reward received: {agent.getNeuromodulationLevel():.3f}")
    
    # 9. Get comprehensive brain statistics
    brain_stats = brain.getNeuronStats()
    print(f"\nBrain Statistics:")
    for key, value in brain_stats.items():
        print(f"  {key}: {value}")
    
    return {
        'brain': brain,
        'agent': agent,
        'world': world,
        'duration': end_time - start_time,
        'brain_stats': brain_stats
    }

# Run the enhanced simulation
if __name__ == "__main__":
    results = run_agent_simulation_enhanced(1000)
```

### Key Classes and Methods (Enhanced)

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

# Development and learning
brain.getDevelopmentalStage()     # Get developmental stage
brain.setDevelopmentalStage(stage) # Set developmental stage
brain.updateDevelopment(timestep)  # Update development

# Enhanced methods
brain.simulateEpisode(sensory_input, num_steps)  # Run episode
brain.compareBrains(brain2)                    # Compare with another brain
brain.getNeuronStats()                          # Get comprehensive stats
brain.getDevelopmentStats()                     # Get development stats
```

#### Config Class (Enhanced)

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# Validation and utilities
config.validate()                         # Validate configuration
config.getInt("key", 0)                   # Get with default
config.getFloat("key", 0.0)               # Get with default
config.getString("key", "")               # Get with default
```

#### AgentBrain Class (Enhanced)

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
agent.getLearningStats()                  # Get learning statistics
agent.getNeuromodulationLevel()
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()

# Enhanced methods
agent.runEpisode(world, num_steps)         # Run complete episode
```

#### SimpleWorld Class (Enhanced)

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

# World objects (enhanced)
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Enhanced object creation
resource = world.createResourceObject(x, y, value)
hazard = world.createHazardObject(x, y, radius)
wall = world.createWallObject(x, y, radius)

# Enhanced query
nearest = world.findNearestObject(x, y, type)
```

#### SensoryPercept Class (Enhanced)

```python
# Creation
percept = pynlm.SensoryPercept()

# Data access and manipulation
percept.setVision(data)
percept.setTouch(data)
percept.setInternal(data)
percept.setProprioception(data)
percept.setAudio(data)

# Enhanced methods
percept.hasVision()                      # Check for vision data
percept.hasTouch()                       # Check for touch data
percept.hasInternal()                    # Check for internal signals
percept.hasProprioception()              # Check for proprioception
percept.hasAudio()                       # Check for audio data

# Data processing
percept.getAllSignalsAsDict()            # Get as dictionary
percept.setFromDict(data)                # Set from dictionary
```

### Utility Functions

```python
import pynlm

# Data processing utilities
data = [1.0, 2.0, 3.0, 4.0, 5.0]
normalized = pynlm.normalize(data)                    # Normalize to [0, 1]
normalized = pynlm.normalize(data, min=-1.0, max=1.0) # Normalize to custom range
sliced = pynlm.slice(data, 1, 4)                      # Extract subvector
mean_val = pynlm.mean([1.0, 2.0, 3.0, 4.0, 5.0])      # Calculate mean
std_val = pynlm.standardDeviation([1.0, 2.0, 3.0, 4.0, 5.0])  # Calculate std dev

# Comparison utilities
comparison = pynlm.compareBrains(brain1, brain2)     # Compare two brains
```

### Enhanced Brain Analysis

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("random_seed", 42)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Get comprehensive analysis
analysis = {
    'total_neurons': brain.getTotalNeuronCount(),
    'total_synapses': brain.getTotalSynapseCount(),
    'firing_neurons': brain.getFiringNeuronCount(),
    'active_neurons': brain.getActiveNeuronCount(),
    'total_spikes': brain.getTotalSpikeCount(),
    'average_firing_rate': brain.getAverageFiringRate(),
    'e_i_ratio': brain.getExcitationInhibitionRatio(),
    'developmental_stage': brain.getDevelopmentalStage(),
    'brain_stats': brain.getNeuronStats(),
    'development_stats': brain.getDevelopmentStats()
}

print("Brain Analysis:")
for key, value in analysis.items():
    print(f"  {key}: {value}")
```

### Advanced Agent Learning

```python
import pynlm

# Create agent system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()

agent.initialize(world)

# Enable advanced learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run episode with learning
episode_result = agent.runEpisode(world, 200)

print("Episode Results:")
for key, value in episode_result.items():
    print(f"  {key}: {value}")

# Get detailed learning statistics
learning_stats = agent.getLearningStats()
print("\nLearning Statistics:")
for key, value in learning_stats.items():
    print(f"  {key}: {value}")
```

### Examples and Demos

#### Hello World Example

```python
import pynlm

# Run the built-in hello world example
pynlm.runHelloWorldExample()
```

#### Complete Agent Example

```python
import pynlm

# Run the complete agent example
pynlm.runAgentExample()
```

### Enumerations

```python
import pynlm

# Neuron types
print(pynlm.NeuronType.Excitatory)
print(pynlm.NeuronType.Inhibitory)
print(pynlm.NeuronType.Sensory)
print(pynlm.NeuronType.Motor)

# Developmental stages
print(pynlm.DevelopmentalStage.Initial)
print(pynlm.DevelopmentalStage.CriticalPeriod)
print(pynlm.DevelopmentalStage.Maturation)
print(pynlm.DevelopmentalStage.Adult)

# Action types
print(pynlm.ActionType.MoveForward)
print(pynlm.ActionType.TurnLeft)
print(pynlm.ActionType.Interact)
print(pynlm.ActionType.Eat)

# World object types
print(pynlm.WorldObjectType.Resource)
print(pynlm.WorldObjectType.Hazard)
print(pynlm.WorldObjectType.Wall)
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

---

## Part 7: Further Reading

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [README.md](python/README.md) - Enhanced Python bindings documentation

## Version Information

```python
import pynlm
print(f"NLM Version: {pynlm.__version__}")
print(f"Author: {pynlm.__author__}")
print(f"Description: {pynlm.__description__}")
```

## License

MIT

## Authors

Research project — See docs for scientific background.