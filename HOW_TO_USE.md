# NLM - HOW TO USE NLM

## Table of Contents

1. [Introduction](#introduction)
2. [Quick Start](#quick-start)
3. [Core Concepts](#core-concepts)
4. [Python API Reference](#python-api-reference)
5. [Configuration](#configuration)
6. [Building](#building)
7. [Running Examples](#running-examples)
8. [Phase 6 Features](#phase-6-features)
9. [Advanced Usage](#advanced-usage)
10. [Troubleshooting](#troubleshooting)
11. [References](#references)

---

## Introduction

NLM (Neural Learning Machine) is an experimental computational brain project that simulates artificial brains with neurons, synapses, learning mechanisms, and memory systems. The goal is to create a system that learns from experience and develops increasingly complex abilities.

**Key Features:**
- **Experimental**: Not claiming to reproduce biological brains
- **Developmental**: Brains start primitive and develop over time
- **Learning**: Synaptic plasticity through STDP and Hebbian learning
- **Memory**: Working memory, episodic memory, and associative memory
- **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error
- **Integration**: All systems work together as a complete brain
- **Configurable**: Extensive configuration options for different experiments

---

## Quick Start

### Basic Usage (Phase 6)

```python
import pynlm

# 1. Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# 2. Initialize the brain
brain.initialize()

# 3. Enable all Phase 6 features
agent = pynlm.createAgentBrain(brain)
agent.initialize(pynlm.createSimpleWorld())

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# 4. Run a simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# 5. Get results
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

### Minimal Usage (Phase 2)

```python
import pynlm

# Create and run a simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

---

## Core Concepts

### Brain
The **Brain** is the central component containing neurons, synapses, and all neural processing mechanisms.

### World
The **World** provides the environment where the brain operates, with sensory input and motor output capabilities.

### Agent
The **Agent** connects the Brain to the World, handling sensory transduction and motor decoding.

### Neuron
Basic processing units that receive inputs, integrate signals, and fire when threshold is reached.

### Synapse
Connections between neurons that can be strengthened or weakened through learning.

### Plasticity
The ability of synapses to change their strength based on activity patterns (STDP, Hebbian learning).

### Neuromodulation
Regulatory signals (dopamine, curiosity, novelty) that influence learning and behavior.

### Memory
Different memory systems:
- **Working Memory**: Short-term storage of current information
- **Episodic Memory**: Storage of specific experiences
- **Associative Memory**: Pattern completion and recall

---

## Python API Reference

### Core Classes

#### Config
Configuration class for NLM system parameters.

**Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary string

#### Brain
Central neural simulation brain class.

**Methods:**
- `initialize()` - Initialize the brain with configuration
- `step(step, time)` - Perform simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current into specific neuron
- `injectCurrentToNeurons(type, current)` - Inject to neurons of specific type
- `produceAction()` - Produce motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name)` - Add a new neural region
- `getRegion(id)` - Get region by ID
- `getTotalNeuronCount()` - Get total neuron count
- `getTotalSynapseCount()` - Get total synapse count
- `getFiringNeuronCount()` - Get count of firing neurons
- `getAverageFiringRate()` - Get average firing rate
- `getExcitationInhibitionRatio()` - Get E/I balance
- `getTotalSpikeCount()` - Get total spike count
- `getDevelopmentalStage()` - Get current developmental stage
- `logStatus()` - Log brain status

#### AgentBrain
Interface connecting NLM brain to world.

**Methods:**
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process percept and inject into brain
- `decodeMotorCommand()` - Decode neural activity into motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward-based modulation
- `updateDevelopment(timestep)` - Update development system
- `enableRewardModulation(enable)` - Enable/disable reward modulation
- `enableStructuralPlasticity(enable)` - Enable/disable structural plasticity
- `enableDevelopment(enable)` - Enable/disable development
- `enableCuriosity(enable)` - Enable/disable curiosity

#### SimpleWorld
Simple 2D world for NLM simulation.

**Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset world
- `update(timestep)` - Update world by timestep
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept (reference)
- `getAgentBody()` - Get agent body state (reference)
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object at position
- `isValidPosition(x, y)` - Check if position is valid

### Factory Functions

#### `createDefaultConfig()`
Create a default configuration.

#### `createBrain(config)`
Create a new brain with configuration.

#### `createSimpleWorld()`
Create a new simple world.

#### `createAgentBrain(brain)`
Create a new agent brain interface.

### Constants

#### NeuronType
- `Excitatory`
- `Inhibitory`
- `Modulatory`
- `Sensory`
- `Motor`
- `Internal`

#### ActionType
- `MoveForward`
- `MoveBackward`
- `MoveLeft`
- `MoveRight`
- `TurnLeft`
- `TurnRight`
- `Look`
- `LookUp`
- `LookDown`
- `Interact`
- `Eat`
- `Drink`
- `Rest`
- `Wait`
- `Custom`

#### MotorCommand
- `MoveForward`
- `MoveBackward`
- `TurnLeft`
- `TurnRight`
- `LookLeft`
- `LookRight`
- `Interact`
- `Wait`

---

## Configuration

NLM uses a configuration system based on key-value pairs. Configuration can be loaded from JSON files or command line arguments.

### Default Configuration
```json
{
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 1,
    "connection_probability": 0.15,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001,
    "dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "visualization_enabled": false
}
```

### Setting Configuration

```python
import pynlm

# Create config
config = pynlm.createDefaultConfig()

# Set values
config.set("random_seed", 42)
config.set("neuron_count", 2000)
config.set("connection_probability", 0.2)

# Save config
config.saveToFile("my_config.json")
```

### Loading Configuration

```python
import pynlm

# Load from file
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Or from command line (in C++ API)
// Config config;
// config.loadFromArgs(argc, argv);
```

---

## Building

### Prerequisites
- CMake 3.16 or higher
- C++20 compatible compiler (GCC 9+, Clang 10+, MSVC 2019+)
- Python 3.8+ (for Python bindings)

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Build specific targets
make nlm                    # Main executable (Phase 6)
make nlm_phase6_demo        # Phase 6 demo
make nlm_phase4_demo        # Phase 4 demo
make nlm_phase3_demo        # Phase 3 demo
make pynlm                  # Python bindings
```

### Installation (Optional)

```bash
# Install to system location (requires sudo)
sudo make install

# Or install to local prefix
make install DESTDIR=/path/to/local
```

---

## Running Examples

### Phase 6 Main Demo

```bash
./nlm
```

This runs the complete Phase 6 integration demo showing all brain systems working together.

### Phase 6 Demo

```bash
./nlm_phase6_demo
```

This runs a focused integration test for Phase 6 systems.

### Python Examples

Save the following as `example.py`:

```python
import pynlm

# Simple brain simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(1000):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Final: {brain.getTotalSpikeCount()} total spikes")
```

Run with:

```bash
python3 example.py
```

### Complete Agent Simulation

Save as `complete_example.py`:

```python
import pynlm

# Setup world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Setup brain
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Setup agent with all features
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    
    # Get what agent sees
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action from brain
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")

print("Simulation complete!")
```

---

## Phase 6 Features

Phase 6 implements the final integration of all brain systems. Here are the key features:

### Memory Integration
- **Working Memory**: Connected to neural processing for active information
- **Episodic Memory**: Stores experiences with encoding and replay
- **Associative Memory**: Hebbian pattern associations

### Neuromodulation Integration
- **Dopamine**: Affects neural excitability and plasticity
- **Curiosity**: Drives exploration behavior
- **Novelty**: Detects unexpected sensory input
- **Prediction Error**: Compares expected vs actual outcomes

### Cognitive Integration
- **Prediction System**: Integrated into brain loop with error signals
- **Neural Planner**: Evaluates action sequences
- **Concept Formation**: Forms patterns from experience
- **Attention**: Competitive selection dynamics

### Development Integration
- **Developmental Stages**: Affect plasticity rates over time
- **Structural Plasticity**: Brain grows and prunes connections
- **Plasticity Modulation**: Age-dependent learning capabilities

### System Integration
- **Checkpoint System**: Save/load brain state
- **Replay System**: Recreate important experiences
- **Complete Brain Loop**: All systems work together coherently

---

## Advanced Usage

### Custom Configuration

```python
import pynlm
import json

# Create custom configuration
config_data = {
    "random_seed": 12345,
    "neuron_count": 2000,
    "region_count": 2,
    "connection_probability": 0.2,
    "simulation_timestep": 0.001,
    "enable_checkpointing": True,
    "enable_replay": True,
    "enable_development": True,
    "enable_curiosity": True,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.018,
    "development_synaptogenesis_rate": 0.002,
    "development_pruning_rate": 0.0002,
    "reward_discount_factor": 0.95
}

# Create config from data
config = pynlm.createDefaultConfig()
# Note: The Python bindings don't expose direct JSON loading yet
# This would require additional pybind11 bindings

# Save custom config
config.saveToFile("custom_config.json")
```

### Performance Monitoring

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

start_time = time.time()
steps_performed = 0

for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    steps_performed += 1

end_time = time.time()
elapsed = end_time - start_time

print(f"Performance: {steps_performed/elapsed:.1f} steps/second")
print(f"Memory usage: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
```

### Experiment Control

```python
import pynlm

def run_experiment(config_name, num_steps=1000, enable_features=True):
    """Run an NLM experiment with given configuration."""
    
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    if config_name == "minimal":
        config.set("neuron_count", 500)
    elif config_name == "large":
        config.set("neuron_count", 5000)
    elif config_name == "complex":
        config.set("neuron_count", 2000)
        config.set("region_count", 3)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Configure features
    if enable_features:
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    # Run experiment
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    # Return results
    return {
        "config_name": config_name,
        "total_spikes": brain.getTotalSpikeCount(),
        "firing_neurons": brain.getFiringNeuronCount(),
        "avg_firing_rate": brain.getAverageFiringRate(),
        "development_stage": brain.getDevelopmentalStage(),
        "curiosity_level": agent.getCuriosityLevel(),
        "novelty_level": agent.getNoveltyLevel()
    }

# Run different experiments
experiments = [
    run_experiment("minimal", 500, True),
    run_experiment("large", 2000, False),
    run_experiment("complex", 1000, True)
]

for exp in experiments:
    print(f"\n{exp['config_name'].upper()} experiment results:")
    for key, value in exp.items():
        if key != "config_name":
            print(f"  {key}: {value}")
```

---

## Troubleshooting

### Common Issues

**"Brain initialization fails"**
- Check if configuration is valid
- Ensure all required files are present
- Verify C++ compiler supports C++20

**"Python bindings not found"**
- Build with `make pynlm`
- Check Python version compatibility
- Ensure pybind11 is installed

**"Performance is too slow"**
- Reduce number of neurons
- Decrease simulation timestep
- Disable visualization
- Use optimized build flags

**"Memory usage is high"**
- Reduce neuron count
- Decrease region count
- Use sparse connectivity
- Consider using Phase 2 for smaller experiments

### Error Messages

**Initialization Errors**
```
NLM_LOG_ERROR("Failed to initialize brain!")
```
* Check configuration parameters
* Verify all required components are available
* Ensure dependencies are properly linked

**Configuration Errors**
```
NLM_LOG_ERROR("Configuration key not found!")
```
* Use `config.has(key)` to check before accessing
* Provide default values with `config.getOr(key, default)`

**Memory Errors**
```
NLM_LOG_ERROR("Memory allocation failed!")
```
* Reduce neuron count
* Use more efficient data structures
* Check system memory availability

### Debug Mode

For debugging, you can enable verbose logging:

```python
import pynlm

# Enable logging (if supported)
# This depends on the specific logging implementation
```

In C++ API:
```cpp
Logger::getInstance().setLevel(Logger::Level::Debug);
```

### Getting Help

- **Documentation**: See `docs/` directory for detailed documentation
- **API Reference**: This file provides Python API reference
- **Examples**: See `easy_usage.md` for beginner examples
- **Issues**: Check `docs/FAILURES.md` for known issues and solutions

---

## References

### Technical Documentation

- `docs/ARCHITECTURE.md` - Detailed system architecture
- `docs/SCIENCE.md` - Scientific background and principles
- `docs/ROADMAP.md` - Development roadmap and timeline
- `docs/EXPERIMENTS.md` - Experiment descriptions and results
- `docs/LIMITATIONS.md` - Current limitations and future directions
- `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 implementation audit
- `docs/PHASE6_ARCHITECTURE.md` - Phase 6 specific architecture
- `docs/PHASE6_FINAL_RESULTS.md` - Phase 6 final results

### Research Papers

- "Computational Modeling of Artificial Developmental Brains" - NLM research framework
- "Integrated Neuromodulation in Neural Simulation Systems" - Neuromodulation implementation
- "Emergent Cognition through Neural Integration" - Cognitive capabilities

### Related Projects

- [Other NLM phases](#running-examples) - Individual phase demonstrations
- [Phase 3 experiments](#running-examples) - World interaction research
- [Phase 4 experiments](#running-examples) - Cognitive development research
- [Phase 5 experiments](#running-examples) - Performance optimization research

### Contact

For questions, suggestions, or collaboration opportunities:
- Project: NLM (熙然) Neural Learning Machine
- Email: nlm-project@research.org
- Documentation: https://nlm.readthedocs.io
- Repository: https://github.com/nlm-project/nlm.git

---

*This documentation covers the Phase 6 final integration release. For earlier phase documentation, see individual phase demo outputs.*

---

**Last updated**: Current release (Phase 6.0)
**Version**: 0.1.0
**License**: MIT