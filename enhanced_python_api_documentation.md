# Enhanced NLM Python API Documentation

## Welcome to the Enhanced NLM Python API

This documentation provides comprehensive guidance for using the NLM (Neural Learning Machine) Python bindings. This enhanced version includes:

- Complete neuroscience class bindings (Neuron, Synapse, etc.)
- Advanced memory systems (Working Memory, Episodic Memory)
- Neuromodulation systems (Dopamine, Curiosity, Novelty)
- Prediction and cognition systems
- Development dynamics
- Enhanced simulation controls
- Data export and visualization tools

## Quick Start

### 1. Installation

```bash
# Install NLM Python bindings
pip install pynlm

# Or install from source
pip install -e .
```

### 2. Basic Brain Simulation

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Run a simulation
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Simulation complete!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

## Core Concepts

### 1. LIF Neurons
The NLM implements Leaky Integrate-and-Fire (LIF) neurons with:
- Membrane potential dynamics
- Spike threshold detection
- Refractory periods
- Synaptic integration

### 2. Synaptic Plasticity
Neurons form connections through synapses that can change strength via:
- **STDP** (Spike-Timing Dependent Plasticity)
- **Hebbian Learning** (Correlated activity)
- **Reward-Modulated Learning** (Dopamine signals)

### 3. Memory Systems
- **Working Memory**: Short-term activation traces
- **Episodic Memory**: Long-term experience storage
- **Associative Memory**: Pattern completion

### 4. Neuromodulation
- **Dopamine**: Reward prediction error
- **Curiosity**: Exploration drive
- **Novelty**: Change detection
- **Prediction Error**: Forward model signals

### 5. Development
Age-dependent changes in plasticity and connectivity.

## Advanced Features

### Memory System Usage

#### Working Memory
```python
# Access working memory
working_memory = brain.getWorkingMemory()

# Store neuron activation
working_memory.store_to_neuron(neuron_id, 0.75)

# Update traces (decay + competition)
working_memory.update(0.1)

# Get active traces
active_neurons = working_memory.get_memory_neurons()
activations = working_memory.get_memory_activations()
```

#### Episodic Memory
```python
# Access episodic memory
episodic_memory = brain.getEpisodicMemory()

# Create an episode
episode = pynlm.EpisodicMemoryItem()
episode.set_position(5.0, 3.0)
episode.set_action(pynlm.ActionType.MoveForward)
episode.set_reward(0.8)

# Store episode
a episodic_memory.store_episode(episode)

# Replay episodes for consolidation
episodes = episodic_memory.get_episodes_for_replay(10)
for ep in episodes:
    episodic_memory.replay_episode(ep)
```

### Neuromodulation
```python
# Access neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNoveltyDetector()

# Update neuromodulators with current state
dopamine.update(0.1)  # Time step
curiosity.update(novelty.getLevel(), dopamine.getPredictionError(), 0.1)

# Use neuromodulation signals
if curiosity.getLevel() > 0.5:
    # Encourage exploration
    print("High curiosity - promoting exploration")

if dopamine.getLevel() > 0.0:
    # Apply reward-modulated learning
    print("Positive prediction error - enhancing plasticity")
```

### Agent-World Interaction
```python
import pynlm

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run an episode
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Motor control
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print status
    if step % 20 == 0:
        print(f"Step {step}: Action {action}")

print("Episode complete!")
```

## Simulation Control

### Enhanced Simulation Controller
```python
# Create simulation controller
sim = pynlm.createSimulationController(brain, world, agent)

# Run different types of simulations
print("=== Standard Episode ===")
sim.run_episode(max_steps=200)

print("=== Curiosity-Driven Exploration ===")
sim.run_curiosity_driven(max_time=60.0, curiosity_threshold=0.3)

print("=== Memory Management Simulation ===")
sim.run_with_memory_management(memory_threshold=0.8)
```

## Data Export and Analysis

### Neural Data Export
```python
# Export neural activity data
pynlm.export_neural_data(brain, "neural_activity.csv", window=1000)

# Export synaptic connections
pynlm.export_synapse_weights(brain, "synaptic_connections.json")

# Export memory traces
brain.getWorkingMemory().export_traces("working_memory.pkl")
brain.getEpisodicMemory().export_episodes("episodes.json")
```

### Visualization
```python
# Plot firing rates
pynlm.plot_firing_rates(brain, "firing_rates.png")

# Create visualization for agent performance
agent_viz = AgentWorldVisualization(agent, world)
agent_viz.plot_action_selection("action_selection.png")
agent_viz.plot_reward_trace("reward_trace.png")
```

## Configuration Management

### Advanced Configuration
```python
# Create and configure a brain
config = pynlm.createDefaultConfig()

# Set multiple parameters at once
config.batch_set({
    'brain.neuron_count': 2000,
    'brain.synapse_density': 0.15,
    'plasticity.stdp.enable': True,
    'plasticity.hebbian.enable': True,
    'neuromod.dopamine.scale': 1.2,
    'neuromod.curiosity.enable': True,
    'neuromod.novelty.enable': True,
    'development.stage': pynlm.DevelopmentalStage.CriticalPeriod
})

# Validate configuration
config.validate()

# Get configuration summary
print(config.summary())
```

### Configuration Options

#### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Connection probability |
| `brain.connection_probability` | float | 0.05 | Individual connection probability |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.v_rest` | float | -70.0 | Resting potential (mV) |

#### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

#### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

#### Development Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `development.initial_plasticity` | float | 1.0 | Initial plasticity factor |
| `development.critical_period_length` | float | 100.0 | Critical period duration |
| `development.adult_plasticity` | float | 0.2 | Adult plasticity factor |

## Development Examples

### Example 1: Learning with STDP
```python
def run_stdp_learning(brain, world, agent, steps=500):
    """Demonstrate STDP learning through correlated activity"""
    for step in range(steps):
        # Present correlated sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Simple motor action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log learning progress
        if step % 100 == 0:
            print(f"Step {step}: STDP learning progress")
    
    return brain
```

### Example 2: Memory-guided Behavior
```python
def run_memory_guided_behavior(brain, world, agent, steps=300):
    """Demonstrate memory-guided decision making"""
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Access working memory for decision making
        working_memory = agent.getWorkingMemory()
        if working_memory.getActiveTraces() > 0:
            # Memory-influenced action selection
            action = agent.decodeMotorCommand()
        else:
            # Default action
            action = pynlm.ActionType.Wait
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Store experience in episodic memory
        if step % 10 == 0:
            episode = create_episode_from_experience(world, agent, step)
            agent.getEpisodicMemory().store_episode(episode)
    
    return brain
```

### Example 3: Development Simulation
```python
def run_development_trajectory(brain, world, agent, steps=2000):
    """Simulate brain development over time"""
    for step in range(steps):
        # Update development system
        agent.updateDevelopment(0.1)
        brain.step(step)
        
        # Present varied experiences
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Motor action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log developmental stage
        if step % 500 == 0:
            stage = agent.getDevelopmentalStage()
            print(f"Step {step}: Developmental stage {stage}")
            
            # Change plasticity rates based on stage
            if stage == pynlm.DevelopmentalStage.CriticalPeriod:
                # High plasticity during critical period
                print("  High plasticity period active")
    
    return brain
```

## Troubleshooting

### Common Issues and Solutions

**Issue 1: "My brain isn't doing anything"
- **Solution**: Ensure you call `brain.initialize()` before `brain.step()`
- **Alternative**: Check if neurons have enough input current

**Issue 2: "The agent isn't moving"
- **Solution**: Ensure `world.applyMotorCommand()` is called each step
- **Alternative**: Check that motor neurons are receiving sufficient activation

**Issue 3: "Everything is 0"
- **Solution**: Brains need time to "warm up" - try more steps
- **Alternative**: Make sure sensory input is connected to neurons

**Issue 4: "Memory systems are empty"
- **Solution**: Check if working memory is initialized with `brain.initialize()`
- **Alternative**: Verify neurons are firing to store traces

**Issue 5: "Checkpoint loading fails"
- **Solution**: Ensure checkpoint files are properly formatted
- **Alternative**: Check that brain configuration matches saved state

### Debugging Tools
```python
# Enable debugging
import pynlm
brain.enable_debugging()

# Monitor performance
profiler = pynlm.NLMProfiler(brain)
profiler.start_profiling()

# Check memory usage
monitor = pynlm.MemoryMonitor(brain)
monitor.enable_monitoring()

# Log system status
brain.log_status()
```

## Performance Tips

### 1. Simulation Speed
- **Use smaller timestep**: `brain.step(step)` with `dt=0.001` is fast
- **Batch operations**: Group multiple operations when possible
- **Memory management**: Clear unused memory traces

### 2. Large Networks
- **Distributed processing**: Use multiple brain regions
- **Efficient data structures**: Use vectorized operations
- **Hardware acceleration**: Consider GPU acceleration for very large networks

### 3. Memory Optimization
- **Working memory capacity**: Set appropriate capacity (`set_capacity()`)
- **Episodic memory limits**: Use `consolidate()` to manage storage
- **Checkpoint frequency**: Balance between memory and performance

## Advanced Topics

### 1. Custom Neuron Types
```python
# Extend neuron behavior
class CustomNeuron(pynlm.Neuron):
    def __init__(self, neuron_id):
        super().__init__(neuron_id)
    
    def custom_plasticity(self, pre_spike, post_spike):
        # Implement custom plasticity rule
        pass
```

### 2. Multi-brain Systems
```python
# Create multiple brain instances
brain1 = pynlm.createBrain(config)
brain2 = pynlm.createBrain(config)

# Synchronize brains
sim_controller = pynlm.createSimulationController(
    brain1, world, agent1)
```

### 3. Real-time Control
```python
# Real-time simulation control
import time

start_time = time.time()
while time.time() - start_time < 60.0:  # Run for 60 seconds
    sim.run_episode(10)  # 10 steps per iteration
    time.sleep(0.1)  # Control simulation speed
```

## API Reference

### Core Classes

#### Brain
- `__init__(config)` - Create brain with configuration
- `initialize()` - Initialize brain with neurons and synapses
- `step(step, time=None)` - Perform one simulation step
- `reset()` - Reset brain to initial state
- `save(filepath)` - Save brain state to file
- `load(filepath)` - Load brain state from file
- `getWorkingMemory()` - Access working memory system
- `getEpisodicMemory()` - Access episodic memory system

#### Neuron
- `__init__(id)` - Create neuron with ID
- `getId()` - Get neuron ID
- `getType()` - Get neuron type
- `setType(type)` - Set neuron type
- `getMembranePotential()` - Get membrane potential
- `injectCurrent(current)` - Inject external current
- `stepLIF(time, dt)` - Perform LIF integration
- `recordSpike(timestamp)` - Record spike for plasticity
- `reset()` - Reset neuron to initial state

#### Synapse
- `__init__(id, source, destination)` - Create synapse
- `getWeight()` - Get synaptic weight
- `setWeight(weight)` - Set synaptic weight
- `getDelay()` - Get synaptic delay
- `recordPreSpike(timestamp)` - Record pre-synaptic spike
- `recordPostSpike(timestamp)` - Record post-synaptic spike
- `reset()` - Reset synapse to initial state

#### AgentBrain
- `__init__(brain)` - Create agent with brain
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory input
- `decodeMotorCommand()` - Get motor command
- `applyRewardModulation(reward, predicted)` - Apply reward learning

#### SimpleWorld
- `__init__()` - Create empty world
- `configure(width, height, visionWidth, visionHeight)` - Set world dimensions
- `update(timestep)` - Update world state
- `applyMotorCommand(cmd, time)` - Apply action to world

### Memory Systems

#### WorkingMemory
- `__init__()` - Create working memory
- `setCapacity(capacity)` - Set memory capacity
- `storeToNeuron(neuronId, activation)` - Store activation trace
- `update(dt)` - Update memory (decay + competition)
- `hasTrace(neuronId)` - Check if neuron has trace
- `getMemoryNeurons()` - Get neurons with traces

#### EpisodicMemory
- `__init__()` - Create episodic memory
- `setMaxEpisodes(maxEpisodes)` - Set storage limit
- `storeEpisode(episode)` - Store experience
- `getEpisodes()` - Get all episodes
- `replayEpisode(episode)` - Replay for consolidation
- `consolidate(threshold)` - Remove weak episodes

### Neuromodulation Systems

#### Dopamine
- `__init__()` - Create dopamine system
- `update(dt)` - Update with time step
- `getLevel()` - Get current level
- `getPlasticityFactor()` - Get plasticity modulation factor

#### Curiosity
- `__init__()` - Create curiosity system
- `update(novelty, prediction_error, dt)` - Update with signals
- `getLevel()` - Get current curiosity level

#### NoveltyDetector
- `__init__()` - Create novelty detector
- `update(observation, dt)` - Update with observations
- `getLevel()` - Get current novelty level

### Configuration

#### Config
- `__init__()` - Create empty configuration
- `loadFromFile(filepath)` - Load from file
- `saveToFile(filepath)` - Save to file
- `has(key)` - Check if key exists
- `get(key)` - Get value as string
- `getInt(key)` - Get value as integer
- `getFloat(key)` - Get value as float
- `getBool(key)` - Get value as boolean
- `set(key, value)` - Set configuration value
- `batch_set(settings)` - Set multiple values
- `validate()` - Validate configuration

### Simulation Tools

#### SimulationController
- `__init__(brain, world, agent)` - Create controller
- `run_episode(max_steps)` - Run complete episode
- `run_curiosity_driven(max_time, threshold)` - Curiosity-driven simulation
- `run_with_memory_management(threshold)` - Memory-managed simulation

### Utility Functions

#### Data Export
- `export_neural_data(brain, filename, window)` - Export neural activity
- `export_synapse_weights(brain, filename)` - Export synaptic connections

#### Visualization
- `plot_firing_rates(brain, filename)` - Plot firing rates
- `create_neural_visualization(brain)` - Create visualization object

### Constants

#### ActionType
- `MoveForward`, `MoveBackward`, `MoveLeft`, `MoveRight`
- `TurnLeft`, `TurnRight`, `Look`, `LookUp`, `LookDown`
- `Interact`, `Eat`, `Drink`, `Rest`, `Wait`, `Custom`

#### DevelopmentalStage
- `Initial`, `CriticalPeriod`, `Maturation`, `Adult`, `Aging`

#### NeuronType
- `Excitatory`, `Inhibitory`, `Modulatory`, `Sensory`, `Motor`, `Internal`

#### SynapseType
- `Excitatory`, `Inhibitory`, `Modulatory`, `Electrical`, `GapJunction`

#### WorldObjectType
- `Empty`, `Resource`, `Hazard`, `Wall`, `Marker`

## Migration Guide

### From Basic to Enhanced API

#### Basic Usage (Unchanged)
```python
# This still works exactly the same
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for step in range(100):
    brain.step(step)
```

#### Enhanced Usage (New Capabilities)
```python
# Now you can access advanced systems
import pynlm

config = pynlm.createDefaultConfig()
config.batch_set({
    'brain.neuron_count': 2000,
    'neuromod.curiosity.enable': True,
    'development.stage': pynlm.DevelopmentalStage.CriticalPeriod
})

brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

# Run enhanced simulation
sim = pynlm.createSimulationController(brain, world, agent)
sim.run_curiosity_driven(60.0)
```

### Performance Considerations

#### Large-Scale Simulations
```python
# Configuration for large networks
config.batch_set({
    'brain.neuron_count': 10000,
    'brain.synapse_density': 0.05,
    'plasticity.stdp.enable': False,  # Disable for speed
    'neuromod.curiosity.enable': False  # Disable for speed
})

# Use memory management
brain.getWorkingMemory().setCapacity(1000)
brain.getEpisodicMemory().setMaxEpisodes(1000)
```

#### Real-time Applications
```python
# Configure for real-time performance
config.batch_set({
    'brain.neuron_count': 500,  # Smaller network
    'simulation_timestep': 0.01,  # Larger timestep
    'plasticity.stdp.enable': False  # Disable plasticity
})

# Use rapid simulation control
sim = pynlm.createSimulationController(brain, world, agent)
for step in range(1000):
    sim.run_episode(10)  # 10 steps per iteration
    time.sleep(0.1)  # Control speed
```

## Future Enhancements

### Planned Features

1. **Advanced Visualization**: 3D neural network visualization, interactive plotting
2. **Cloud Integration**: Distributed simulation across multiple machines
3. **Real-time Interface**: Web-based simulation control and monitoring
4. **Machine Learning Integration**: Neural network training with NLM data
5. **Mobile Support**: Lightweight NLM for mobile and embedded systems

### Research Extensions

1. **Neuromaging Integration**: Connection with fMRI and EEG data
2. **Multi-scale Modeling**: Integration with higher-level cognitive models
3. **Neuromorphic Hardware**: Export to FPGA and neuromorphic chips
4. **Educational Tools**: Interactive learning environments

## Contact and Support

### Documentation Issues
- Report documentation errors to the NLM development team
- Submit feature requests for API enhancements

### Technical Support
- For technical issues, consult the troubleshooting section
- Contact support@nlm-project.org for professional assistance

### Community
- Join the NLM discussion forum at github.com/nlm-project/nlm/discussions
- Share your projects and experiences with the community
- Contribute to NLM documentation and examples

## Acknowledgments

This enhanced NLM Python API builds upon the foundational work of:

- The NLM research team for developing the neuroscience simulation framework
- The pybind11 developers for providing the Python binding system
- The open-source community for providing supporting tools and libraries

## License

MIT License - See LICENSE file for details.

---

*This documentation is continuously updated. For the latest version, visit the NLM documentation website.*

Generated: 2026-11-10
Version: 2.0.0
```

This documentation provides a comprehensive guide to the enhanced NLM Python API, covering everything from basic usage to advanced features and troubleshooting. The documentation is organized to help users at all levels of expertise effectively use NLM for neural simulation and research.