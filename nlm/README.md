# NLM (Neural Learning Machine) - Python Bindings

NLM (熙然) is an experimental artificial developmental brain simulation framework.
This package provides comprehensive Python bindings for the NLM C++ neural simulation
framework, enabling you to create and interact with virtual brains using Python.

## Overview

NLM is a **brain simulator** for computers. It lets you create virtual brains that can:
- Learn from experience
- Remember patterns and experiences  
- Make decisions and take actions
- Develop over time
- Adapt to their environment

Think of NLM as a simplified model of how biological brains work, featuring:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns and experiences

## Why Use NLM?

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

## Current Phase

**PHASE 6: FINAL INTEGRATION**

Phase 6 focuses on integrating all existing systems into a coherent artificial brain.
Previous phases built individual components; Phase 6 ensures they work together as a unified system.

## Quick Start (5 Minutes)

### Example 1: Simplest Brain

```python
import nlm

# One line to create a virtual brain
brain = nlm.createBrain(nlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### Example 2: Full Agent in a World

```python
import nlm

# Setup
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

## Installation

### Prerequisites

- Python 3.8+
- pip

### Install NLM

```bash
pip install nlm
```

### Development Installation

```bash
pip install -e .
```

### Build from Source

```bash
# Clone the repository
# Navigate to the source directory
cd /path/to/nlm-source

# Create build directory and build Python bindings
mkdir build
cd build
cmake ..
make

# Install Python package
cd python
pip install -e ..
```

## Quick Reference

| What you want | Python Code |
|--------------|-------------|
| Create brain | `nlm.createBrain(nlm.createDefaultConfig())` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `nlm.createSimpleWorld()` |
| Create agent | `nlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

## Basic Usage Examples

### 1. Simple Neural Network

```python
import nlm

# Create brain with default settings
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Agent in World

```python
import nlm

# Setup
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Agent completed 200 steps in the world!")
```

### 3. Phase 3 World Interaction

```python
import nlm

# Create and initialize brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Create world for Phase 3
world = nlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = nlm.createAgentBrain(brain)

# Run Phase 3 experiment
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Phase 3 experiment completed!")
```

### 4. Custom Configuration

```python
import nlm

# Create custom configuration
config = nlm.Config()
config.set('neuron_count', 1000)
config.set('region_count', 4)
config.set('simulation_timestep', 0.001)
config.set('enable_development', True)
config.set('enable_checkpointing', False)

# Create brain with custom configuration
brain = nlm.createBrain(config)
```

### 5. Save and Load Brain State

```python
import nlm

# Create and initialize brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run simulation
for i in range(500):
    brain.step(i)

# Save brain state
brain.save("my_brain.nml")
print("Brain state saved to my_brain.nml")

# Create new brain and load saved state
brain2 = nlm.createBrain(nlm.createDefaultConfig())
brain2.load("my_brain.nml")
print("Brain state loaded successfully")
```

## Complete Application Example

```python
import nlm

def run_nlm_simulation(config_file=None, steps=1000):
    """
    Complete NLM simulation with brain, world, and agent.
    
    Args:
        config_file: Optional configuration file path
        steps: Number of simulation steps
        
    Returns:
        Simulation results dictionary
    """
    # Load or create configuration
    if config_file:
        config = nlm.Config()
        if not config.load_from_file(config_file):
            print(f"Warning: Could not load config file {config_file}")
        config = nlm.createDefaultConfig()
    else:
        config = nlm.createDefaultConfig()
    
    # Create and initialize brain
    brain = nlm.createBrain(config)
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
    
    # Create world
    world = nlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create and initialize agent
    agent = nlm.createAgentBrain(brain)
    if not agent.initialize(world):
        raise RuntimeError("Failed to initialize agent")
    
    # Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    results = {
        'steps_completed': 0,
        'total_spikes': 0,
        'final_firing_rate': 0.0,
        'brain_status': None
    }
    
    for step in range(steps):
        # World update
        world.update(0.1)
        
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Action generation and execution
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect results every 100 steps
        if step % 100 == 0:
            results['steps_completed'] = step + 1
            results['total_spikes'] = brain.getTotalSpikeCount()
            results['final_firing_rate'] = brain.getAverageFiringRate()
            results['brain_status'] = {
                'neuron_count': brain.getTotalNeuronCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'development_stage': brain.getDevelopmentalStage().value
            }
    
    results['steps_completed'] = steps
    results['total_spikes'] = brain.getTotalSpikeCount()
    results['final_firing_rate'] = brain.getAverageFiringRate()
    
    return results

# Run complete simulation
if __name__ == "__main__":
    results = run_nlm_simulation(steps=1000)
    
    print("\n=== Simulation Results ===")
    print(f"Steps completed: {results['steps_completed']}")
    print(f"Total spikes: {results['total_spikes']}")
    print(f"Final firing rate: {results['final_firing_rate']:.2f} Hz")
    
    status = results['brain_status']
    print(f"Neurons: {status['neuron_count']}")
    print(f"Active neurons: {status['active_neurons']}")
    print(f"Firing neurons: {status['firing_neurons']}")
    print(f"Development stage: {status['development_stage']}")
    
    print("\nSimulation completed successfully!")
```

## API Documentation

### Core Classes

#### Config
Configuration management for NLM neural simulation.

- `Config()` - Create empty configuration
- `set(key, value, source)` - Set configuration value
- `get(key, default)` - Get configuration value
- `has(key)` - Check if key exists
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file
- `createDefault()` - Create default configuration

#### RandomGenerator
Random number generator for NLM.

- `uniform_real(min, max)` - Generate uniform float
- `uniform_int(min, max)` - Generate uniform integer
- `normal(mean, std)` - Generate normal float

#### Brain
Central neural simulation brain class.

- `initialize()` - Initialize brain with configuration
- `step(step)` - Perform simulation step
- `receiveSensoryInput(input)` - Inject sensory input
- `injectCurrent(neuron, current)` - Inject current to neuron
- `produceAction()` - Produce motor action
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `getTotalNeuronCount()` - Get neuron count
- `getTotalSynapseCount()` - Get synapse count

#### Neuron
LIF neuron implementation.

- `initialize(neuron_id)` - Create neuron with ID
- `setType(type)` - Set neuron type
- `setMembranePotential(potential)` - Set membrane potential
- `stepLIF(current_time, dt)` - Step LIF neuron
- `recordSpike(timestamp)` - Record spike

#### Synapse
Synaptic connection between neurons.

- `initialize(id, source, dest)` - Create synapse
- `setWeight(weight)` - Set synaptic weight
- `recordPreSpike(timestamp)` - Record presynaptic spike
- `recordPostSpike(timestamp)` - Record postsynaptic spike

#### SimpleWorld
Simple 2D world for NLM simulation.

- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `update(timestep)` - Update world
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body

#### AgentBrain
Agent brain interface connecting NLM brain to world.

- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `enableRewardModulation(enable)` - Enable reward modulation

### Factory Functions

#### Factory Functions
Factory functions for creating NLM components.

- `createDefaultConfig()` - Create default configuration
- `createBrain(config)` - Create brain with configuration
- `createSimpleWorld()` - Create simple world
- `createAgentBrain(brain)` - Create agent brain interface

## Examples

### Phase 3 Experiment

```python
import nlm

# Create and initialize brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Create world for Phase 3
world = nlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = nlm.createAgentBrain(brain)

# Run Phase 3 experiment
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Phase 3 experiment completed!")
```

### Data Analysis

```python
import nlm

# Run simulation
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

for step in range(500):
    brain.step(step)

# Collect statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Development

### Running Tests

```bash
# Run Python tests
pytest tests/ -v

# Run all tests
pytest
```

### Build from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build Python bindings
make -j4

# Install Python package
cd python
pip install -e ..
```

## Documentation

### NLM Documentation

For detailed documentation about NLM neural network simulation:
- [README.md](/README.md) - Project overview
- [easy_usage.md](/easy_usage.md) - Easy usage guide
- [HOW_TO_USE.md](/HOW_TO_USE.md) - Detailed usage instructions

### Python API Documentation

Complete Python API documentation is available in the `nlm/` package.

## Examples

### Phase 3 Experiment

```python
import nlm

# Create and initialize brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Create world for Phase 3
world = nlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = nlm.createAgentBrain(brain)

# Run Phase 3 experiment
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Phase 3 experiment completed!")
```

### Data Analysis

```python
import nlm

# Run simulation
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

for step in range(500):
    brain.step(step)

# Collect statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

## Troubleshooting

### Common Issues

#### Import Error
```python
# Error: No module named 'nlm'
# Solution: Install with pip
pip install nlm
```

#### Configuration Error
```python
# Error: Failed to initialize brain
# Solution: Check configuration
config = nlm.Config()
config.set('neuron_count', 500)
brain = nlm.createBrain(config)
```

#### Build Issues
```bash
# Error: Could not find pybind11
# Solution: Install dependencies
sudo apt-get install pybind11-dev  # Ubuntu
brew install pybind11            # macOS
```

### Getting Help

If you encounter issues:
1. Check the examples and documentation
2. Read `easy_usage.md` for quick start
3. Read `HOW_TO_USE.md` for detailed instructions
4. Check GitHub issues
5. Submit an issue with error details

## License

MIT License

## Authors

NLM Authors

## Version

0.1.0

---

**Note**: This package provides Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework. The core functionality is implemented in C++ and exposed to Python via pybind11 bindings.
