# NLM Python API Documentation

**Version:** 0.1.0
**Last Updated:** 2026-10-26

Comprehensive documentation for the NLM Python bindings (`pynlm`).

## Overview

The `pynlm` Python module provides Python bindings for the NLM (Neural Learning Machine) C++ neural simulation framework. These bindings expose the complete NLM functionality through a Pythonic interface, making it easy to:

- Create and simulate artificial brains
- Run world simulations with agents
- Use memory systems and neuromodulators
- Access development and plasticity mechanisms
- Persist brain state and checkpoints

## Installation

The Python bindings are built as part of the NLM C++ project. See the main README for build instructions.

```bash
# For development with editable installation
pip install -e .

# For regular installation  
pip install .
```

## Module Structure

```python
import pynlm

# Core classes
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Helper functions
config = pynlm.createDefaultConfig()
```

## Core Classes

### Config

Configuration class for NLM system settings.

**Methods:**
- `loadFromFile(filepath)` - Load configuration from JSON file
- `loadFromArgs(argc, argv)` - Load from command line arguments  
- `saveToFile(filepath)` - Save configuration to JSON file
- `has(key)` - Check if configuration key exists
- `getKeys()` - Get all configuration keys
- `clear()` - Clear all configuration entries
- `summary()` - Get configuration summary string

**Example:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.saveToFile("my_config.json")
```

### Brain

Central neural simulation brain class - the main computational engine.

**Methods:**
- `initialize()` - Initialize brain with configuration
- `step(step_number)` - Run one simulation step
- `step(step_number, time)` - Run step with timestamp
- `receiveSensoryInput(input)` - Inject sensory data
- `injectCurrent(neuron, current)` - Inject current to specific neuron
- `injectCurrentToNeurons(type, current)` - Inject to all neurons of type
- `produceAction()` - Get motor action based on neural activity
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `addRegion(name)` - Add neural region
- `getRegion(id)` - Get region by ID
- `getTotalNeuronCount()` - Get total neurons
- `getTotalSynapseCount()` - Get total synapses
- `getFiringNeuronCount()` - Get currently firing neurons
- `getAverageFiringRate()` - Get average firing rate
- `getTotalSpikeCount()` - Get total spike count
- `getDevelopmentalStage()` - Get developmental stage

**Example:**
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

brain = pynlm.createBrain(config)
brain.initialize()

for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### AgentBrain

Interface connecting NLM brain to world environment - the agent's control system.

**Methods:**
- `initialize(world)` - Initialize with world
- `getSensoryInputSize()` - Get expected sensory input size
- `getMotorOutputSize()` - Get expected motor output size
- `processSensoryInput(percept)` - Process sensory percept and inject to brain
- `decodeMotorCommand()` - Decode brain activity into motor command
- `applyRewardModulation(reward, predictedReward)` - Apply reward-based neuromodulation
- `updateDevelopment(timestep)` - Update development system
- `getCuriosityLevel()` - Get curiosity level
- `getNoveltyLevel()` - Get novelty level
- `getPredictionError()` - Get prediction error
- `enableRewardModulation(bool)` - Enable/disable reward modulation
- `enableStructuralPlasticity(bool)` - Enable/disable structural plasticity
- `enableDevelopment(bool)` - Enable/disable development
- `enableCuriosity(bool)` - Enable/disable curiosity

**Example:**
```python
import pynlm

def run_agent_simulation(num_steps=1000):
    # Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.1)
```

### SimpleWorld

Simple 2D world for NLM simulation - the environment the agent operates in.

**Methods:**
- `configure(width, height, visionWidth, visionHeight)` - Configure world dimensions
- `reset()` - Reset to initial state
- `setAgentStart(x, y)` - Set agent starting position
- `update(timestep)` - Update world by timestep
- `applyMotorCommand(cmd, currentTime)` - Apply motor command
- `getSensoryPercept()` - Get sensory percept from world
- `getAgentBody()` - Get agent body state
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object at position
- `isValidPosition(x, y)` - Check if position is valid
- `getWidth()`, `getHeight()` - Get world dimensions
- `getSimulationTime()` - Get current simulation time
- `setRandomSeed(seed)` - Set random seed

**Example:**
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    # Process percept...
```

## Enumerations

### NeuronType
- `Excitatory` - Excitatory neurons
- `Inhibitory` - Inhibitory neurons  
- `Modulatory` - Neuromodulatory neurons
- `Sensory` - Sensory neurons
- `Motor` - Motor neurons
- `Internal` - Internal state neurons

### ActionType
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `MoveLeft` - Move left
- `MoveRight` - Move right
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `Look` - Look
- `LookUp` - Look up
- `LookDown` - Look down
- `Interact` - Interact with environment
- `Eat` - Eat
- `Drink` - Drink
- `Rest` - Rest
- `Wait` - Wait

### DevelopmentalStage
- `Initial` - Initial developmental stage
- `CriticalPeriod` - Critical period for learning
- `Maturation` - Maturation phase
- `Adult` - Adult stage
- `Aging` - Aging stage

### MotorCommand
- `MoveForward` - Move forward
- `MoveBackward` - Move backward
- `TurnLeft` - Turn left
- `TurnRight` - Turn right
- `LookLeft` - Look left
- `LookRight` - Look right
- `Interact` - Interact
- `Wait` - Wait

### WorldObjectType
- `Empty` - Empty space
- `Resource` - Resource object
- `Hazard` - Hazardous object
- `Wall` - Wall
- `Marker` - Marker

## Configuration Parameters

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

## Getting Started Examples

### Example 1: Silent Brain
```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think
for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Example 2: Brain Watching a World
```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Example 3: Complete Agent
```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
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

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**ImportError: No module named 'pynlm'**
```bash
pip install --force-reinstall .
```

**Segmentation faults**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Best Practices

1. **Always initialize** your brain, world, and agent before use
2. **Use consistent timesteps** across brain, world, and agent
3. **Enable learning subsystems** early if you want learning behavior
4. **Save checkpoints** periodically during long simulations
5. **Monitor simulation progress** with brain statistics
6. **Use development stages** to control learning rates

## Advanced Usage

### Custom Configuration
```python
import json

# Create custom config
config = {
    "brain.neuron_count": 2000,
    "brain.synapse_density": 0.05,
    "plasticity.stdp.learning_rate": 0.01,
    "neuromod.dopamine.scale": 0.5
}

# Apply to brain
brain_config = pynlm.createDefaultConfig()
for key, value in config.items():
    brain_config.set(key, value)
```

### Simulation Control
```python
# Run simulation with custom timestep
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for specific duration
time_step = 0.01
simulation_time = 10.0  # 10 seconds
steps = int(simulation_time / time_step)

for step in range(steps):
    world.update(time_step)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Performance Tips

1. **Use larger timesteps** for faster simulation (but less precision)
2. **Batch multiple steps** when possible
3. **Monitor memory usage** during long simulations
4. **Save intermediate checkpoints** to avoid recomputation
5. **Use world with efficient sensing** for faster simulations
6. **Adjust neuron count** based on your simulation requirements

## Integration with Other Tools

### NumPy Integration
While the NLM is primarily a C++ simulation framework, you can:
- Use NumPy for array operations on sensory data
- Visualize simulation results with matplotlib
- Process simulation outputs with data analysis libraries

### Visualization
The NLM includes visualization capabilities through the C++ visualization modules. For Python-based visualization, consider using:
- matplotlib for time series plots
- opencv for world visualization  
- plotly for interactive simulations

## Future Development

The Python API is actively developed. Future improvements may include:

1. **More comprehensive configuration API** with type-safe setters
2. **Event-driven simulation** for parallel processing
3. **Enhanced visualization tools** in Python
4. **Distributed simulation** capabilities
5. **Machine learning integration** for training neural architectures
6. **Web-based simulation interface** using Jupyter notebooks

## Citing NLM

If you use the NLM Python bindings in your research, please cite:

```
@software{nlm_python_2026,
    author = {NLM Research Team},
    title = {NLM (熙然) - Neural Learning Machine Python Bindings},
    url = {https://github.com/nlm-project/nlm},
    version = {0.1.0},
    year = {2026},
    note = {An experimental artificial developmental brain with Python bindings}
}
```

## License

MIT License. See the main README for details.

---

*This documentation is part of the NLM/熙然 project. For more information, visit the project documentation.*
