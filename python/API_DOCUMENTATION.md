# NLM Python API Documentation

**Version**: 0.1.0
**Phase**: 6 (Final Integration)

## Overview

The NLM Python API provides a high-level interface to the NLM (Neural Learning Machine) computational brain simulation framework. It wraps the C++ neural simulation library with Pythonic interfaces that make it easy to create, train, and interact with artificial brains.

## Quick Start

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Core Classes

### Brain

The main brain simulation class that implements the complete Phase 6 integrated architecture.

#### Creation

```python
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Key Methods

- **`brain.step(step_number)`** - Run one simulation step (Phase 6 integrated loop)
- **`brain.step(step_number, time)`** - Run step with explicit timestamp
- **`brain.initialize()`** - Initialize the brain with configuration
- **`brain.reset()`** - Reset brain to initial state

#### Statistics

```python
brain.getTotalNeuronCount()          # Total neurons in brain
brain.getTotalSynapseCount()         # Total synapses in brain
brain.getFiringNeuronCount()         # Currently firing neurons
brain.getAverageFiringRate()          # Average firing rate (Hz)
brain.getTotalSpikeCount()            # Total spikes generated
brain.getDevelopmentalStage()       # Current developmental stage
brain.getExcitationInhibitionRatio() # E/I balance ratio
```

#### State Management

```python
brain.save("checkpoint.bin")    # Save brain state
brain.load("checkpoint.bin")    # Load brain state
brain.logStatus()                # Print brain status
```

#### Regions

```python
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

### Config

Configuration class for NLM system parameters.

#### Creation

```python
config = pynlm.createDefaultConfig()
```

#### Configuration

```python
config.set("brain.neuron_count", 1000)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 1.0)
```

#### File I/O

```python
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### Query

```python
if config.has("brain.neuron_count"):
    count = config.getOr("brain.neuron_count", 1000)
```

### AgentBrain

Bridge connecting NLM brain to world simulation with all Phase 6 integrated features.

#### Creation

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Sensory Processing

```python
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
```

#### Motor Decoding

```python
action = agent.decodeMotorCommand()
```

#### Neuromodulation

```python
agent.applyRewardModulation(reward, predicted_reward)
```

#### Development

```python
agent.updateDevelopment(timestep)
```

#### Feature Control

```python
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

#### Statistics

```python
agent.getCuriosityLevel()      # Current curiosity level (0.0-1.0)
agent.getNoveltyLevel()        # Current novelty detection level
agent.getPredictionError()     # Prediction error signal
agent.getNeuromodulationLevel() # Current neuromodulation level
agent.getDevelopmentalStage()  # Current developmental stage
```

### SimpleWorld

2D world simulation for agent-environment interaction.

#### Creation

```python
world = pynlm.createSimpleWorld()
```

#### Configuration

```python
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
```

#### Control

```python
world.reset()
world.update(timestep=0.1)
world.setAgentStart(x, y)
```

#### Interaction

```python
world.applyMotorCommand(action, currentTime)
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

#### World Objects

```python
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)
```

#### Properties

```python
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
```

## Enums

### NeuronType

```python
pynlm.NeuronType.Excitatory   # Excitatory neuron (depolarizing)
pynlm.NeuronType.Inhibitory   # Inhibitory neuron (hyperpolarizing)
pynlm.NeuronType.Sensory      # Sensory neuron
pynlm.NeuronType.Motor        # Motor neuron
pynlm.NeuronType.Modulatory   # Neuromodulatory neuron
pynlm.NeuronType.Internal     # Internal neuron
```

### SynapseType

```python
pynlm.SynapseType.Excitatory    # Excitatory synapse
pynlm.SynapseType.Inhibitory    # Inhibitory synapse
pynlm.SynapseType.Modulatory    # Modulatory synapse
pynlm.SynapseType.Electrical   # Electrical synapse
pynlm.SynapseType.GapJunction  # Gap junction synapse
```

### DevelopmentalStage

```python
pynlm.DevelopmentalStage.Initial      # Initial developmental stage (high plasticity)
pynlm.DevelopmentalStage.CriticalPeriod # Critical period (high learning)
pynlm.DevelopmentalStage.Maturation    # Maturation (medium plasticity)
pynlm.DevelopmentalStage.Adult        # Adult stage (low plasticity)
pynlm.DevelopmentalStage.Aging        # Aging stage (declining function)
```

### ActionType

```python
pynlm.ActionType.MoveForward  # Move forward
pynlm.ActionType.MoveBackward # Move backward
pynlm.ActionType.TurnLeft     # Turn left
pynlm.ActionType.TurnRight    # Turn right
pynlm.ActionType.Look         # Look around
pynlm.ActionType.Interact    # Interact with object
pynlm.ActionType.Eat         # Eat/consume
pynlm.ActionType.Drink       # Drink
pynlm.ActionType.Rest        # Rest/idle
pynlm.ActionType.Wait        # Wait for next step
```

### MotorCommand

```python
pynlm.MotorCommand.MoveForward  # Move forward
pynlm.MotorCommand.MoveBackward # Move backward
pynlm.MotorCommand.TurnLeft     # Turn left
pynlm.MotorCommand.TurnRight    # Turn right
pynlm.MotorCommand.LookLeft     # Look left
pynlm.MotorCommand.LookRight    # Look right
pynlm.MotorCommand.Interact    # Interact
pynlm.MotorCommand.Wait        # Wait
```

### WorldObjectType

```python
pynlm.WorldObjectType.Empty     # Empty space
pynlm.WorldObjectType.Resource  # Resource (food, energy)
pynlm.WorldObjectType.Hazard   # Hazard (danger)
pynlm.WorldObjectType.Wall      # Wall/obstacle
pynlm.WorldObjectType.Marker    # Marker/target
```

## Helper Functions

### Convenience Functions

```python
# Simple aliases
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
```

### run_simulation (Convenience Function)

```python
def run_simulation(brain_obj, world_obj, agent_obj, steps=100):
    """Run a complete simulation loop."""
    for step in range(steps):
        world_obj.update(0.1)
        percept = world_obj.getSensoryPercept()
        agent_obj.processSensoryInput(percept)
        brain_obj.step(step)
        action = agent_obj.decodeMotorCommand()
        world_obj.applyMotorCommand(action, world_obj.getSimulationTime())
        
        # Optional: Add neuromodulation
        reward = world_obj.getSensoryPercept().getInternal()[0] if world_obj.getSensoryPercept().getInternal() else 0.0
        agent_obj.applyRewardModulation(reward, 0.0)
        
        # Optional: Update development
        agent_obj.updateDevelopment(0.1)
    
    return get_statistics()
```

### create_advanced_brain (Convenience Function)

```python
def create_advanced_brain(neuron_count=1000, learning_rate=0.01, 
                          enable_plasticity=True, enable_development=True,
                          enable_curiosity=True):
    """Create a brain with advanced configuration for complex simulations."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Set advanced parameters
    config.set("brain.neuron_count", neuron_count)
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("plasticity.hebbian.enable", enable_plasticity)
    config.set("plasticity.structural.enable", enable_plasticity)
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", enable_curiosity)
    config.set("neuromod.novelty.enable", True)
    
    # Create components
    brain_obj = pynlm.createBrain(config)
    brain_obj.initialize()
    
    world_obj = pynlm.createSimpleWorld()
    world_obj.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world_obj.reset()
    
    agent_obj = pynlm.createAgentBrain(brain_obj)
    agent_obj.initialize(world_obj)
    
    # Enable advanced features
    agent_obj.enableRewardModulation(True)
    agent_obj.enableStructuralPlasticity(enable_plasticity)
    agent_obj.enableDevelopment(enable_development)
    agent_obj.enableCuriosity(enable_curiosity)
    
    return brain_obj, agent_obj, world_obj
```

### example_complete_agent_simulation (Complete Example)

```python
def example_complete_agent_simulation(num_steps=1000):
    """Complete agent simulation example for easy use.
    
    This provides a ready-to-use example that demonstrates all major features
    including sensory processing, motor control, reward-based learning, and
    development.
    """
    # Create advanced brain and environment
    brain_obj, agent_obj, world_obj = create_advanced_brain(
        neuron_count=500,
        learning_rate=0.01,
        enable_plasticity=True,
        enable_development=True,
        enable_curiosity=True
    )
    
    # Set agent starting position
    world_obj.setAgentStart(10.0, 10.0)
    
    # Run the simulation
    print(f"Running complete agent simulation with {num_steps} steps...")
    results = run_simulation(brain_obj, world_obj, agent_obj, num_steps)
    
    # Print final statistics
    print("\n=== SIMULATION RESULTS ===")
    print(f"Steps completed: {results['steps_completed']}")
    print(f"Total time: {results['total_time']:.2f}s")
    print(f"Final firing rate: {results['final_firing_rate']:.2f} Hz")
    print(f"Total spikes: {results['total_spikes']}")
    print(f"Final curiosity level: {results['final_curiosity']:.3f}")
    print(f"Final novelty level: {results['final_novelty']:.3f}")
    print(f"Final development stage: {results['final_development_stage']}")
    
    return brain_obj, agent_obj, world_obj, results
```

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Installation

### Prerequisites

```bash
pip install pybind11 scikit-build-core pytest numpy
```

### From Source

```bash
# Install in development mode
pip install -e .

# Or build from source
pip install build
python -m build

# Install from wheel
pip install dist/*.whl
```

### Verification

```python
import pynlm
print(pynlm.__version__)

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Testing

### Running Tests

```bash
# Run Python tests (if any)
pytest tests/

# Run C++ tests (requires building)
./build/tests/nlm_test
```

### Example Tests

```python
import pynlm

# Test brain creation and basic functionality
brain = pynlm.createBrain(pynlm.createDefaultConfig())
assert brain.getTotalNeuronCount() > 0

# Test agent creation
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent = pynlm.createAgentBrain(brain)

# Test simulation
for step in range(100):
    brain.step(step)

assert brain.getTotalSpikeCount() >= 0
print("All tests passed!")
```

## Migration Guide

### From Phase 2 to Phase 6

The Phase 6 API maintains backward compatibility while adding new integrated features:

#### Old API (Phase 2)

```python
# Phase 2: Basic brain creation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for step in range(1000):
    brain.step(step)
```

#### New API (Phase 6)

```python
# Phase 6: Integrated brain with all features
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced with agent and world
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete simulation with all systems
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Key Differences

1. **Integrated Systems**: Phase 6 automatically integrates memory, neuromodulation, prediction, and cognition systems
2. **Enhanced Configuration**: More configuration options for advanced use cases
3. **Convenience Functions**: Helper functions for common simulation patterns
4. **Complete Agent API**: Full agent-world integration with all Phase 6 features

## Troubleshooting

### Import Errors

```bash
# Fix: Install with development mode
pip install -e .
```

### ImportError: No module named 'pynlm'

```bash
# Fix: Force reinstall
pip install --force-reinstall .
```

### Segmentation Faults

```
# Check: Always call initialize() before step()
# Check: Call reset() before re-running simulation
```

### Performance Issues

```
# Tip: Use create_advanced_brain() for better performance
# Tip: Increase neuron_count for more complex simulations
```

## License

MIT

## Authors

NLM Research Team

## Version History

- **0.1.0**: Initial release with Phase 6 integrated architecture
- **0.0.0**: Legacy Phase 2 API (deprecated)

## Future Enhancements

- **Phase 7**: Advanced learning algorithms and transfer learning
- **Phase 8**: Multi-agent coordination and social learning
- **Phase 9**: Enhanced visualization and interactive debugging
- **Phase 10**: Cloud-based distributed simulations

## References

- [NLM Documentation](https://nlm.readthedocs.io)
- [Phase 6 Audit](docs/PHASE6_FINAL_AUDIT.md)
- [Architecture Guide](docs/ARCHITECTURE.md)
- [Scientific Background](docs/SCIENCE.md)