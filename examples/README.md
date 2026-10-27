# NLM Python Examples

This directory contains comprehensive examples for using the NLM (Neural Learning Machine)
Python API. These examples range from simple quick starts to advanced learning scenarios.

## Overview

The NLM Python API provides Python bindings for the NLM computational brain simulator.
This directory contains examples that demonstrate how to use the API for various use cases:

- Basic brain creation and simulation
- World interaction and agent behavior
- Advanced learning with all subsystems
- Brain checkpointing and persistence
- Multiple agent scenarios
- Batch processing

## Examples Included

### 1. Quick Start
- **File**: `quick_start.py`
- **Description**: The simplest possible example to get you started
- **Use case**: First-time users wanting minimal setup

### 2. Getting Started Tutorial
- **File**: `getting_started.py`
- **Description**: Step-by-step tutorial covering all major concepts
- **Use case**: Users wanting a structured learning path

### 3. Comprehensive Examples
- **File**: `comprehensive_examples.py`
- **Description**: Full-scale examples demonstrating advanced features
- **Use case**: Users wanting to see complete implementations

### 4. Usage Examples
- **File**: `usage_examples.py`
- **Description**: Practical examples for common workflows
- **Use case**: Users needing ready-to-use code patterns

## How to Run Examples

### Prerequisites

1. Build NLM with Python bindings:
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release
   make -j4
   pip install -e .
   ```

2. Ensure the pynlm module is available:
   ```bash
   python -c "import pynlm; print(f'pynlm version {pynlm.__version__}')"
   ```

### Running Examples

All examples can be run directly:

```bash
# Quick start (simplest)
python examples/quick_start.py

# Getting started tutorial
python examples/getting_started.py

# Comprehensive examples
python examples/comprehensive_examples.py

# Usage examples
python examples/usage_examples.py

# Test all examples
python examples/test_examples.py

# Run NLM API test suite
python test_nlm_api.py
```

## API Reference Used in Examples

### Core Classes

- **Brain**: Main neural simulation class
- **Config**: Configuration management
- **AgentBrain**: Interface between brain and world
- **SimpleWorld**: 2D simulation environment
- **SensoryPercept**: Sensory input representation
- **Action**: Motor output representation

### Key Methods

#### Brain Operations
- `createBrain(config)`: Create a new brain
- `initialize()`: Initialize brain with configuration
- `step(step_number)`: Run one simulation step
- `getTotalNeuronCount()`: Get total neuron count
- `getTotalSynapseCount()`: Get total synapse count
- `getFiringNeuronCount()`: Get currently firing neurons
- `getTotalSpikeCount()`: Get total spike count
- `getAverageFiringRate()`: Get average firing rate

#### Agent Operations
- `createAgentBrain(brain)`: Create agent interface
- `initialize(world)`: Initialize with world
- `processSensoryInput(percept)`: Process sensory input
- `decodeMotorCommand()`: Get motor command
- `enableRewardModulation(enable)`: Enable reinforcement learning
- `enableCuriosity(enable)`: Enable exploration behavior
- `enableDevelopment(enable)`: Enable structural development

#### World Operations
- `createSimpleWorld()`: Create simulation world
- `configure(width, height, visionWidth, visionHeight)`: Configure world
- `reset()`: Reset world to initial state
- `update(timestep)`: Update world state
- `getSensoryPercept()`: Get sensory perception
- `applyMotorCommand(command, time)`: Apply action to world

### Learning Subsystems

#### Neuromodulation
- **Dopamine**: Reward and reinforcement
- **Curiosity**: Exploration motivation
- **Novelty**: Novelty detection
- **Prediction Error**: Learning signal

#### Memory Systems
- **Working Memory**: Transient active information
- **Episodic Memory**: Experience storage
- **Associative Memory**: Pattern associations

#### Plasticity Rules
- **STDP**: Spike-timing-dependent plasticity
- **Hebbian**: "Neurons that fire together, wire together"
- **Structural Plasticity**: Grow and prune connections

## Example Workflows

### 1. Basic Simulation

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
```

### 2. World Interaction

```python
import pynlm

# Setup brain, world, and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)

# Run agent in world
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 3. Advanced Learning

```python
import pynlm

# Custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

# Create learning agent
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning simulation
for step in range(1000):
    # ... standard agent loop ...
    # ... with learning enabled ...
```

## Best Practices

### 1. Configuration Management

Use consistent configuration settings:

```python
# Default configuration
config = pynlm.createDefaultConfig()

# Custom settings
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
```

### 2. Error Handling

Always check for errors in critical operations:

```python
brain = pynlm.createBrain(config)
if not brain.initialize():
    print("Failed to initialize brain")
    # Handle error...
```

### 3. Resource Management

Use checkpoints for long simulations:

```python
# Save progress
brain.save("checkpoint_100.bin")

# Reset and reload
brain.reset()
brain.initialize()
brain.load("checkpoint_100.bin")
```

### 4. Performance Monitoring

Monitor key metrics during simulation:

```python
if step % 100 == 0:
    print(f"Step {step}: "
          f"Neurons firing={brain.getFiringNeuronCount()}, "
          f"Curiosity={agent.getCuriosityLevel():.3f}, "
          f"DevStage={brain.getDevelopmentalStage()}")
```

## Extending Examples

You can extend these examples by:

1. **Adding visualization**: Plot brain activity or world state
2. **Performance profiling**: Measure simulation speed
3. **Experiment automation**: Run multiple parameter sets
4. **Logging**: Record simulation data for analysis
5. **Parallel processing**: Run multiple simulations simultaneously

## Getting Help

For more information:

1. **Documentation**: Check `docs/` directory for detailed technical documentation
2. **Source code**: Examine `src/` directory for implementation details
3. **Tests**: Look at `tests/` directory for test examples
4. **GitHub**: Visit the NLM repository for additional resources

These examples should provide a comprehensive foundation for using the NLM Python API
in both research and production environments.