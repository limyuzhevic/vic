# NLM Python API Documentation

## Overview

The NLM (Neural Learning Machine) project provides an enhanced Python interface for neural simulation with Pythonic API design, comprehensive error handling, and advanced features for both beginners and advanced users.

## Quick Start

### Installation

```bash
pip install .
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.Brain()
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)

print(f"Simulation completed: {brain.get_neuron_count()} neurons,")
print(f"{brain.get_firing_count()} firing neurons")
```

## Enhanced API Features

### 1. Pythonic Property Access

The enhanced API provides direct property access with automatic caching:

```python
import pynlm

brain = pynlm.Brain()
brain.initialize()

# Direct property access (auto-cached)
neuron_count = brain.neuron_count  # Returns int
synapse_count = brain.synapse_count  # Returns int
firing_rate = brain.firing_rate  # Returns float
total_spikes = brain.total_spikes  # Returns int
e_i_ratio = brain.e_i_ratio  # Returns float

# Property access with statistics
stats = brain.statistics  # Returns comprehensive stats dict
memory_state = brain.memory_state  # Returns memory state dict
```

### 2. Context Managers

Use context managers for automatic resource management:

```python
import pynlm

# Context manager for automatic initialization/reset
with pynlm.SimulationContext(brain) as sim:
    for step in range(100):
        sim.step()
        print(f"Step {step}: {sim.get_statistics()}")

# Context manager for world management
with pynlm.WorldContext(world) as w:
    w.update(0.1)
    percept = w.get_sensory_percept()
```

### 3. Enhanced Initialization

Initialize brain with configuration dictionary:

```python
import pynlm

# Create brain with custom configuration
config = pynlm.Config({
    'brain.neuron_count': 1000,
    'brain.synapse_density': 0.1,
    'simulation_timestep': 0.001,
    'plasticity.enable_stdp': True,
    'plasticity.enable_hebbian': True,
    'neuromodulation.enable_curiosity': True,
    'neuromodulation.enable_novelty': True
})

brain = pynlm.Brain(config)
brain.initialize(config)
```

### 4. Advanced Simulation Methods

Convenient simulation methods:

```python
import pynlm

brain = pynlm.Brain()
brain.initialize()

# Run for specific number of steps
brain.run_for_steps(500)

# Run until specific time
brain.run_until_time(10.0)

# Run with specific timestep
brain.run_for_steps(1000, timestep=0.0005)
```

### 5. Serialization and Export

Export brain state in multiple formats:

```python
import pynlm

brain = pynlm.Brain()
brain.initialize()

# Export to Python dictionary
brain_dict = brain.to_dict()
print(brain_dict)

# Export to JSON string
json_str = brain.to_json()
with open('brain_state.json', 'w') as f:
    f.write(json_str)

# Export to YAML string (if PyYAML available)
yaml_str = brain.to_yaml()
with open('brain_state.yaml', 'w') as f:
    f.write(yaml_str)
```

### 6. Configuration Management

Comprehensive configuration management:

```python
import pynlm

# Create and manage configuration
config = pynlm.Config({
    'brain.neuron_count': 2000,
    'simulation_timestep': 0.001
})

# Load from file
config.load_from_file('config.json')

# Save to file
config.save_to_file('config.json')

# Configuration validation
if config.validate():
    print("Configuration is valid")
else:
    errors = config.get_validation_errors()
    print("Validation errors:", errors)

# Export/import JSON/YAML
json_data = config.to_json()
config.from_json(json_data)
```

### 7. Agent World Integration

Complete agent-world simulation:

```python
import pynlm

# Setup agent simulation
def run_agent_simulation(num_steps=1000):
    # 1. Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # 6. Run simulation loop
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
    
    print("Agent simulation complete!")

# Run the simulation
run_agent_simulation(1000)
```

### 8. Benchmarking and Performance

Performance benchmarking:

```python
import pynlm

brain = pynlm.Brain()
brain.initialize()

# Quick benchmark
benchmark_result = brain.benchmark(1000, warmup=100)
print(f"Performance: {benchmark_result['steps_per_second']:.2f} steps/sec")
print(f"Average step time: {benchmark_result['average_step_time_ms']:.3f} ms")

# High-precision benchmark
stats = brain.get_statistics()
print(f"Brain statistics: {stats}")
```

### 9. Error Handling and Validation

Comprehensive error handling:

```python
import pynlm

brain = pynlm.Brain()
brain.initialize()

# Error handling for simulation steps
try:
    for step in range(1000):
        brain.step(step)
except pynlm.RuntimeError as e:
    print(f"Simulation error: {e}")

# Configuration validation
try:
    config = pynlm.Config({
        'brain.neuron_count': -1,  # Invalid value
        'simulation_timestep': 0.0  # Invalid value
    })
    if not config.validate():
        errors = config.get_validation_errors()
        print("Configuration validation failed:")
        for error in errors:
            print(f"  - {error}")
except pynlm.ValueError as e:
    print(f"Invalid configuration: {e}")
```

### 10. Advanced Features

#### World Objects and Environment

```python
import pynlm

world = pynlm.SimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()

# Add objects to world
world.addObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0, radius=0.5)
world.addObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, value=5.0, radius=1.0)
world.addObject(25.0, 25.0, pynlm.WorldObjectType.Marker, value=1.0, radius=0.2)

# Check if position is valid
if world.isValidPosition(20.0, 20.0):
    print("Position is valid")
```

#### Action System

```python
import pynlm

# Create actions
action1 = pynlm.Action(pynlm.ActionType.MoveForward, [0.5, 0.2])
action2 = pynlm.Action(pynlm.ActionType.Eat)
action3 = pynlm.Action.from_dict({
    "type": pynlm.ActionType.Custom,
    "parameters": [0.1, 0.9, 0.3],
    "name": "CustomAction"
})

# Action utility functions
action1_dict = action1.to_dict()
action_copy = action2.clone()
```

#### Sensory Processing

```python
import pynlm

# Create sensory inputs
vision = pynlm.Vision(8, 8)
vision.setData([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8])

audio = pynlm.Audio(44100, 1024)
audio.setData(list(range(1024)))

internal_signals = pynlm.InternalSignals()
internal_signals.addSignal(0.5)
internal_signals.addSignal(0.3)
internal_signals.addSignal(0.8)

# Create sensory percept
percept = pynlm.SensoryPercept()
percept.setVision(vision)
percept.setInternal(internal_signals)
```

### 11. Debugging and Development

#### Debug Mode

```python
import pynlm

# Enable debug mode
config = pynlm.Config()
config.enableDebugMode()

brain = pynlm.Brain(config)
brain.initialize()
```

#### Performance Optimization

```python
import pynlm

# Optimize configuration for simulation
config = pynlm.Config()
config.optimizeForSimulation()

brain = pynlm.Brain(config)
brain.initialize()
```

## Advanced API Reference

### Core Classes

#### Brain

| Property | Type | Description |
|----------|------|-------------|
| `neuron_count` | int | Total number of neurons |
| `synapse_count` | int | Total number of synapses |
| `firing_count` | int | Number of currently firing neurons |
| `firing_rate` | float | Average firing rate (Hz) |
| `total_spikes` | int | Total spike count |
| `e_i_ratio` | float | Excitation/inhibition ratio |
| `developmental_stage` | int | Current developmental stage |

**Methods:**
- `initialize(config=None)` - Initialize brain with optional configuration
- `step(step, time)` - Perform a simulation step
- `reset()` - Reset brain state
- `to_dict()` - Export to Python dictionary
- `to_json()` - Export to JSON string
- `run_for_steps(steps, timestep)` - Run simulation for specific steps
- `run_until_time(target_time, timestep)` - Run simulation until time
- `benchmark(steps, warmup)` - Performance benchmark
- `get_statistics()` - Get comprehensive statistics
- `get_memory_state()` - Get memory system state
- `get_top_neurons(count)` - Get top neurons with metadata

#### Config

**Methods:**
- `Config(values)` - Create from dictionary
- `loadFromFile(filepath)` - Load from file
- `saveToFile(filepath)` - Save to file
- `has(key)` - Check if key exists
- `set(key, value)` - Set configuration value
- `validate()` - Validate configuration
- `to_json()` - Export to JSON
- `from_json(json_str)` - Import from JSON
- `to_yaml()` - Export to YAML
- `from_yaml(yaml_str)` - Import from YAML

#### AgentBrain

**Methods:**
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Decode motor command
- `applyRewardModulation(reward, predicted_reward)` - Apply reward
- `updateDevelopment(timestep)` - Update development system
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableCuriosity(bool)` - Enable/disable curiosity

#### SimpleWorld

**Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world
- `reset()` - Reset world state
- `update(timestep)` - Update world
- `setAgentStart(x, y)` - Set agent start position
- `applyMotorCommand(action, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept
- `getAgentBody()` - Get agent body state
- `addObject(x, y, type, value, radius)` - Add world object
- `removeObject(x, y)` - Remove object

### Enumerations

- `NeuronType`: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
- `ActionType`: MoveForward, MoveBackward, MoveLeft, MoveRight, TurnLeft, TurnRight, Look, LookUp, LookDown, Interact, Eat, Drink, Rest, Wait, Custom
- `DevelopmentalStage`: Initial, CriticalPeriod, Maturation, Adult, Aging
- `FiringState`: Resting, Active, Refractory, Inhibited
- `WorldObjectType`: Empty, Resource, Hazard, Wall, Marker

### Factory Functions

- `pynlm.createDefaultConfig()` - Create default configuration
- `pynlm.createBrain(config)` - Create brain
- `pynlm.createSimpleWorld()` - Create simple world
- `pynlm.createAgentBrain(brain)` - Create agent brain interface

### Context Managers

- `SimulationContext(brain)` - Brain context manager
- `WorldContext(world)` - World context manager
- `AgentContext(agent)` - Agent context manager

### Error Classes

- `pynlm.RuntimeError` - General runtime errors
- `pynlm.ValueError` - Invalid values
- `pynlm.IndexError` - Index out of bounds
- `pynlm.NLMError` - NLM-specific errors

## Migration Guide

### From Legacy to Enhanced API

#### Legacy Pattern

```python
import pynlm

# Legacy approach
config = pynlm.Config()
brain = pynlm.Brain(config)
brain.initialize()

for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(brain.getFiringNeuronCount())
```

#### Enhanced Pattern

```python
import pynlm

# Enhanced approach with Pythonic API
config = pynlm.Config({
    'brain.neuron_count': 1000,
    'simulation_timestep': 0.001
})

brain = pynlm.Brain(config)
brain.initialize()

# Use context manager
with pynlm.SimulationContext(brain) as sim:
    for step in range(100):
        sim.step()
        if step % 10 == 0:
            print(sim.firing_count)
```

### Configuration Migration

#### Legacy Config File Format

```
brain.neuron_count = 500
brain.synapse_density = 0.1
simulation_timestep = 0.001
```

#### Enhanced Config Dictionary

```python
config = pynlm.Config({
    'brain.neuron_count': 500,
    'brain.synapse_density': 0.1,
    'simulation_timestep': 0.001
})
```

## Performance Considerations

### Memory Optimization

1. **Use Context Managers**: Automatically manage resources
2. **Batch Operations**: Use batch methods where possible
3. **Cache Statistics**: Property access is automatically cached

### Simulation Optimization

1. **Use Enhanced Initialization**: Faster startup with config dict
2. **Benchmark Performance**: Use built-in benchmarking
3. **Optimize Configuration**: Use `optimizeForSimulation()`

### Large-Scale Simulations

```python
import pynlm

# For large-scale simulations
config = pynlm.Config({
    'brain.neuron_count': 10000,  # Large brain
    'brain.synapse_density': 0.05,  # Sparse connections
    'simulation_timestep': 0.0001,  # Small timestep
    'performance.use_sparse_connectivity': True,
    'performance.use_simd': True
})

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation with performance monitoring
stats = brain.get_statistics()
print(f"Simulation running with {stats['neuron_count']} neurons")
```

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"

**Solution:** Ensure you call `initialize()` before `step()`:

```python
import pynlm

brain = pynlm.Brain()
brain.initialize()  # Required!

for step in range(100):
    brain.step(step)
```

#### "The agent isn't moving"

**Solution:** Check that you call `world.update()` and `world.applyMotorCommand()`:

```python
import pynlm

brain = pynlm.createBrain()
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Ensure world updates are called
for step in range(100):
    world.update(0.1)  # Required!
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())  # Required!
```

#### "Everything is 0"

**Solution:** Brains need time to "warm up":

```python
import pynlm

brain = pynlm.Brain()
brain.initialize()

# Run more steps for brain to "warm up"
for step in range(1000):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringCount()} neurons firing")
```

## Further Reading

### Documentation

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions
- [README.md](README.md) - Project overview

### Examples

- [easy_usage.md](easy_usage.md) - Simple usage examples
- [HOW_TO_USE.md](HOW_TO_USE.md) - Comprehensive usage guide

### API Reference

- Complete API documentation is available through `help(pynlm)` in Python
- Type hints available through py.typed file (if present)
- Interactive examples in documentation

## Support

For questions, issues, or feature requests:

1. Check the [GitHub Issues](https://github.com/nlm-project/nlm/issues)
2. Search existing documentation
3. Submit feature requests or bug reports
4. Contribute to the project

## License

MIT License - See [LICENSE](LICENSE) for details.
