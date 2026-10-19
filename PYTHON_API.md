# Python API Documentation for NLM

## Overview

NLM provides a comprehensive Python API for accessing the C++ neural learning machine framework. The Python bindings enable users to create, configure, and interact with artificial neural systems without needing to understand the underlying C++ implementation.

### Key Features

- **Simple Setup**: Minimal dependencies, straightforward installation
- **Comprehensive Functionality**: Full access to all NLM brain systems
- **Flexible Configuration**: Extensive configuration options
- **Real-time Interaction**: Step-by-step brain control and monitoring
- **Memory Integration**: Access to working, episodic, and associative memory
- **Neuromodulation Control**: Control over dopamine, curiosity, and novelty systems
- **Development Stages**: Manage brain development and plasticity
- **Checkpointing**: Save and restore brain states

## Installation

### Quick Installation (Recommended)

```bash
pip install .
```

### Development Installation

```bash
pip install -e .
```

### From Source

```bash
pip install scikit-build-core pybind11 numpy pytest
pip install .
```

## Basic Usage Examples

### Example 1: Simple Brain Creation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Example 2: Brain in a World

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)

# Create brain and world
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("Brain and world ready for simulation")
```

### Example 3: Complete Agent Simulation

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
    
    print("Agent simulation complete!")

# Run the simulation
run_agent_simulation(1000)
```

## Configuration Options

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

## Advanced Configuration

### Configuration File I/O

```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.002)

# Save configuration to file
config.saveToFile("my_config.json")

# Load configuration from file
config2 = pynlm.createDefaultConfig()
config2.loadFromFile("my_config.json")

# Get configuration keys
keys = config2.getKeys()
for key in keys:
    print(f"{key}: {config2.get(key)}")
```

### Command Line Arguments

The NLM framework accepts command line arguments for configuration:

```bash
./nlm --brain.neuron_count=2000 --plasticity.stdp.learning_rate=0.002 --checkpoint_dir=./my_checkpoints
```

## API Reference

### Brain Class

| Method | Description |
|--------|-------------|
| `createBrain(config)` | Create a new brain with given configuration |
| `brain.initialize()` | Initialize the brain |
| `brain.step(step_number)` | Run one simulation step |
| `brain.reset()` | Reset brain state |
| `brain.save(path)` | Save brain state to file |
| `brain.load(path)` | Load brain state from file |
| `brain.getTotalNeuronCount()` | Get total number of neurons |
| `brain.getTotalSynapseCount()` | Get total number of synapses |
| `brain.getFiringNeuronCount()` | Get currently firing neurons |
| `brain.getAverageFiringRate()` | Get average firing rate |
| `brain.getExcitationInhibitionRatio()` | Get E/I balance |
| `brain.getDevelopmentalStage()` | Get current developmental stage |

### Config Class

| Method | Description |
|--------|-------------|
| `createDefaultConfig()` | Create a default configuration |
| `config.set(key, value)` | Set configuration value |
| `config.get(key)` | Get configuration value |
| `config.getOr(key, default)` | Get configuration value with default |
| `config.has(key)` | Check if configuration has key |
| `config.getKeys()` | Get all configuration keys |
| `config.loadFromFile(path)` | Load configuration from file |
| `config.saveToFile(path)` | Save configuration to file |

### AgentBrain Class

| Method | Description |
|--------|-------------|
| `createAgentBrain(brain)` | Create agent brain interface |
| `agent.initialize(world)` | Initialize agent with world |
| `agent.processSensoryInput(percept)` | Process sensory input |
| `agent.decodeMotorCommand()` | Get motor command from brain |
| `agent.applyRewardModulation(reward, predicted)` | Apply reward modulation |
| `agent.updateDevelopment(timestep)` | Update development |
| `agent.enableRewardModulation(enable)` | Enable/disable reward modulation |
| `agent.enableStructuralPlasticity(enable)` | Enable/disable structural plasticity |
| `agent.enableDevelopment(enable)` | Enable/disable development |
| `agent.enableCuriosity(enable)` | Enable/disable curiosity |
| `agent.getCuriosityLevel()` | Get current curiosity level |
| `agent.getNoveltyLevel()` | Get current novelty level |

### SimpleWorld Class

| Method | Description |
|--------|-------------|
| `createSimpleWorld()` | Create a simple world |
| `world.configure(width, height, visionWidth, visionHeight)` | Configure world |
| `world.reset()` | Reset world to initial state |
| `world.update(timestep)` | Update world |
| `world.setAgentStart(x, y)` | Set agent starting position |
| `world.applyMotorCommand(cmd, time)` | Apply motor command to world |
| `world.getSensoryPercept()` | Get sensory perception |

## Configuration Enumerations

### NeuronType

- `NeuronType.Excitatory`: Excitatory neuron
- `NeuronType.Inhibitory`: Inhibitory neuron
- `NeuronType.Sensory`: Sensory neuron
- `NeuronType.Motor`: Motor neuron
- `NeuronType.Modulatory`: Modulatory neuron
- `NeuronType.Internal`: Internal neuron

### DevelopmentalStage

- `DevelopmentalStage.Initial`: Initial stage
- `DevelopmentalStage.CriticalPeriod`: Critical period
- `DevelopmentalStage.Maturation`: Maturation stage
- `DevelopmentalStage.Adult`: Adult stage
- `DevelopmentalStage.Aging`: Aging stage

### ActionType

- `ActionType.MoveForward`: Move forward
- `ActionType.TurnLeft`: Turn left
- `ActionType.TurnRight`: Turn right
- `ActionType.MoveBackward`: Move backward
- `ActionType.Interact`: Interact with environment
- `ActionType.Wait`: Do nothing

### WorldObjectType

- `WorldObjectType.Empty`: Empty space
- `WorldObjectType.Resource`: Resource object
- `WorldObjectType.Hazard`: Hazard object
- `WorldObjectType.Wall`: Wall object
- `WorldObjectType.Marker`: Marker object

## Performance Tips

### Memory Optimization

1. **Use checkpointing**: Save brain state periodically to avoid re-initialization
2. **Configure memory limits**: Set appropriate capacity for memory systems
3. **Monitor memory usage**: Use the built-in memory monitoring

### Simulation Performance

1. **Batch operations**: Process multiple steps when possible
2. **Reduce logging**: Disable verbose logging in production
3. **Optimize parameters**: Tune simulation parameters for your use case
4. **Use vectorized operations**: Leverage NumPy for numerical operations

### Configuration Optimization

1. **Use minimal configuration**: Only set parameters you need
2. **Cache configurations**: Reuse configuration objects when possible
3. **Avoid repeated modifications**: Set configuration once and reuse

## Troubleshooting

### Common Issues

#### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected

#### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neurons are firing

#### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

#### "ImportError: No module named 'pynlm'"
```bash
pip install --force-reinstall .
```

#### "Segmentation faults"
- Ensure `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Check for null pointer dereferences in your code

### Debug Information

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get debug information
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Regions: {brain.getRegionCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

## Migration Guide

### From Version 0.0.x to 0.1.0

1. **Memory systems**: New memory system API available
2. **Prediction system**: Enhanced prediction system functionality
3. **Neuromodulation**: More comprehensive neuromodulation support
4. **Configuration**: Updated configuration options
5. **Checkpointing**: Improved checkpointing with memory system support

## Examples Gallery

### Mini Projects

1. **Silent Brain**: Basic brain without environment interaction
2. **Brain Watching a World**: Brain observing a simple world
3. **Complete Agent**: Full agent simulation with learning
4. **Memory Experiment**: Demonstrating memory systems
5. **Development Study**: Studying brain development over time
6. **Neuromodulation Study**: Exploring neuromodulation effects

### Advanced Examples

1. **Multi-Agent Simulation**: Multiple agents interacting
2. **Environment Design**: Custom world environments
3. **Learning Benchmarks**: Measuring learning performance
4. **Memory Testing**: Comprehensive memory system tests
5. **Development Tracking**: Long-term development studies

## Python Best Practices

### Error Handling

```python
import pynlm

try:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", -1)  # Invalid value
    brain = pynlm.createBrain(config)
    brain.initialize()
except Exception as e:
    print(f"Error: {e}")
    # Handle error appropriately
```

### Resource Management

```python
import pynlm
import gc

# Use context manager or explicit cleanup
def simulate():
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    try:
        brain.initialize()
        # Run simulation
        for step in range(100):
            brain.step(step)
    finally:
        # Clean up
        del brain
        del config
        gc.collect()

# Call simulation
simulate()
```

### Performance Monitoring

```python
import pynlm
import time

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

start_time = time.time()
steps_completed = 0

for step in range(1000):
    brain.step(step)
    steps_completed += 1
    
    # Monitor performance every 100 steps
    if step % 100 == 0:
        elapsed = time.time() - start_time
        steps_per_second = steps_completed / elapsed
        print(f"Step {step}: {steps_per_second:.1f} steps/second")

print(f"Simulation complete: {steps_completed} steps in {time.time() - start_time:.1f}s")
```

## Getting Help

### Documentation

- **Main Documentation**: This file (Python API Documentation)
- **Architecture**: `docs/ARCHITECTURE.md`
- **Science**: `docs/SCIENCE.md`
- **Experiments**: `docs/EXPERIMENTS.md`
- **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md`

### Examples

- **Basic Examples**: `easy_usage.md`
- **Advanced Examples**: `HOW_TO_USE.md`
- **Quick Reference**: README.md

### Community

- **GitHub Issues**: Report bugs and request features
- **Discussions**: Ask questions and share experiences
- **Examples**: Submit your own examples

## Acknowledgements

The NLM Python API is built on the excellent work of the NLM project team. Special thanks to all contributors who have made this framework possible.

## License

MIT License - See LICENSE file for details

---

*Last updated: 2026-10-19*
*Version: 0.1.0*