# Enhanced NLM Python API Documentation

This document provides comprehensive information about the NLM Python API, including all available classes, functions, and usage examples.

## Quick Start

### Installation
```bash
pip install pynlm
```

### Basic Usage
```python
import pynlm

# Create a brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run the brain for some steps
for i in range(100):
    brain.step(i)

print(f"Brain status: {brain.getFiringNeuronCount()} neurons firing")
```

## Core Classes

### Brain Class
Represents the central neural simulation brain with all integrated cognitive systems.

#### Constructor
```python
def createBrain(config: Config) -> Brain:
    """Create a new brain with the given configuration.
    
    Args:
        config: Configuration object for brain settings
        
    Returns:
        Brain instance
    """
```

#### Main Methods
| Method | Description | Example |
|--------|-------------|---------|
| `initialize()` | Initialize the brain with current configuration | brain.initialize() |
| `step(step_number)` | Process one simulation step | brain.step(10) |
| `receiveSensoryInput(input)` | Inject sensory input | brain.receiveSensoryInput(vision) |
| `produceAction()` | Generate motor output | action = brain.produceAction() |
| `reset()` | Reset brain to initial state | brain.reset() |

#### Query Methods
| Method | Returns | Description |
|--------|---------|-------------|
| `getTotalNeuronCount()` | int | Total number of neurons |
| `getTotalSynapseCount()` | int | Total number of synapses |
| `getFiringNeuronCount()` | int | Number of currently firing neurons |
| `getAverageFiringRate()` | float | Average firing rate across all neurons |
| `getDevelopmentalStage()` | int | Current developmental stage |

#### Memory Systems
| Method | Returns | Description |
|--------|---------|-------------|
| `getWorkingMemory()` | WorkingMemory | Working memory instance |
| `getEpisodicMemory()` | EpisodicMemory | Episodic memory instance |
| `getPredictionSystem()` | PredictionSystem | Prediction system instance |

#### Neuromodulation Systems
| Method | Returns | Description |
|--------|---------|-------------|
| `getDopamine()` | Dopamine | Dopamine neuromodulator |
| `getCuriosity()` | Curiosity | Curiosity neuromodulator |
| `getNovelty()` | Novelty | Novelty detection system |

### World Class
Represents the simulation environment.

#### Constructor
```python
def createSimpleWorld() -> SimpleWorld:
    """Create a new simple world for NLM simulation.
    
    Returns:
        SimpleWorld instance
    """
```

#### Configuration
```python
def configure(width: int, height: int, 
              visionWidth: int, visionHeight: int) -> None:
    """Configure world dimensions and sensing capabilities.
    
    Args:
        width: World width in cells
        height: World height in cells
        visionWidth: Vision sensor width
        visionHeight: Vision sensor height
    """
```

### AgentBrain Class
Connects brain to world, processing sensory input and producing motor output.

#### Constructor
```python
def createAgentBrain(brain: Brain) -> AgentBrain:
    """Create an agent brain that connects NLM brain to world.
    
    Args:
        brain: Brain instance to control
        
    Returns:
        AgentBrain instance
    """
```

#### Key Methods
| Method | Description | Example |
|--------|-------------|---------|
| `initialize(world)` | Connect agent to world | agent.initialize(world) |
| `processSensoryInput(percept)` | Process sensory input | agent.processSensoryInput(percept) |
| `decodeMotorCommand()` | Get action from brain | action = agent.decodeMotorCommand() |
| `applyRewardModulation(reward, predicted)` | Apply reward learning | agent.applyRewardModulation(reward, pred) |

#### Learning Control
```python
def enableRewardModulation(enable: bool) -> None:
    """Enable/disable reward-based learning."""

def enableCuriosity(enable: bool) -> None:
    """Enable/disable curiosity-driven exploration."""

def enableDevelopment(enable: bool) -> None:
    """Enable/disable developmental progression."""

def enableStructuralPlasticity(enable: bool) -> None:
    """Enable/disable structural plasticity (connection growth)."""
```

### Configuration Class
Manages brain parameters and settings.

#### Constructor
```python
def createDefaultConfig() -> Config:
    """Create a default configuration with reasonable settings.
    
    Returns:
        Config instance with default values
    """
```

#### Configuration Parameters
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuron_count` | int | 1000 | Total number of neurons |
| `region_count` | int | 1 | Number of neural regions |
| `connection_probability` | float | 0.1 | Probability of random connections |
| `simulation_timestep` | float | 0.001 | Time step for simulation |
| `random_seed` | int | 42 | Random seed for reproducibility |

#### Configuration Methods
| Method | Description | Example |
|--------|-------------|---------|
| `set(key, value, source)` | Set configuration parameter | config.set("neuron_count", 2000) |
| `getOr(key, default)` | Get value or return default | size = config.getOr("neuron_count", 1000) |
| `has(key)` | Check if parameter exists | if config.has("stdp_ltp_weight") |

### Memory System Classes

#### WorkingMemory
Transient storage for active neural patterns.
- `getActiveTraces()` - Number of active memory traces
- `storeToNeuron(neuron_id, activation)` - Store neuron activation
- `getMemoryNeurons()` - Get neurons currently in memory

#### EpisodicMemory
Long-term storage of experienced events.
- `getEpisodeCount()` - Number of stored episodes
- `storeEpisode(episode)` - Store new episode
- `getEpisodesForReplay(count)` - Get episodes for replay

### Neuromodulation Classes

#### Dopamine
Reward and reinforcement learning signal.
- `getLevel()` - Current dopamine concentration
- `signalReward(reward)` - Respond to rewards
- `signalRewardPredictionError(error)` - Respond to prediction errors

#### Curiosity
Exploration motivation based on novelty.
- `getCuriosityLevel()` - Current curiosity level
- `update(dt)` - Update based on novelty detection

#### Novelty
Detects novel sensory inputs.
- `getNoveltyLevel()` - Current novelty level
- `update(percept)` - Update based on new inputs

## Advanced Usage Examples

### 1. Complete Agent Simulation
```python
import pynlm

# Setup the complete agent-world-brain loop
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run complete simulation
for step in range(500):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### 2. Memory Investigation
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run to let memory systems engage
for step in range(200):
    brain.step(step)

# Examine memory system status
wm = brain.getWorkingMemory()
em = brain.getEpisodicMemory()

print(f"Working memory traces: {wm.getActiveTraces()}")
print(f"Episodic memory episodes: {em.getEpisodeCount()}")

if em.getEpisodeCount() > 0:
    episodes = em.getEpisodesForReplay(5)
    print(f"Retrieved {len(episodes)} episodes for replay")
```

### 3. Development Observation
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Observe developmental progression
print("Developmental stages:")
for stage in range(4):
    brain.setDevelopmentalStage(stage)
    for step in range(1000):
        brain.step(step)
    
    print(f"Stage {stage}: avg firing rate = {brain.getAverageFiringRate():.3f}")
```

### 4. Prediction System Usage
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# The prediction system is automatically integrated
# You can access it and monitor its operation
prediction_system = brain.getPredictionSystem()
prediction_error = brain.getPredictionErrorSignal()

print(f"Prediction error level: {prediction_error.getLevel()}")
```

## Error Handling

### Common Errors and Solutions

| Error | Cause | Solution |
|-------|-------|----------|
| "Brain not initialized" | Forgetting to call `initialize()` | Always call `brain.initialize()` before use |
| "Invalid sensory input" | Wrong input format | Use `SensoryInput`, `Vision`, or `Audio` classes |
| "Motor command failed" | Invalid action type | Check action types in `ActionType` enum |

### Exception Handling
```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.step(10)
except Exception as e:
    print(f"Error: {e}")
    # Handle error appropriately
```

## Performance Tips

### 1. Efficient Simulation
```python
# Use batch processing for multiple steps
for batch_start in range(0, total_steps, batch_size):
    for step in range(batch_size):
        brain.step(batch_start + step)
    # Optional: Save checkpoint periodically
    if (batch_start + batch_size) % checkpoint_interval == 0:
        brain.save(f"checkpoint_{batch_start + batch_size}.bin")
```

### 2. Memory Management
```python
# Clear memory traces periodically
wm = brain.getWorkingMemory()
wm.clear()  # If available

# Archive old episodes
em = brain.getEpisodicMemory()
while em.getEpisodeCount() > max_episodes:
    # Remove oldest episode (implementation dependent)
    pass
```

### 3. Resource Optimization
```python
# Monitor and adjust based on system resources
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Memory traces: {wm.getActiveTraces()}")

# Adjust simulation parameters based on load
if brain.getFiringNeuronCount() > max_firing:
    # Reduce neural activity
    pass
```

## Troubleshooting Guide

### "My brain isn't doing anything"
1. Did you call `brain.initialize()`?
2. Are there enough neurons and synapses?
3. Is sensory input being injected?

### "The agent isn't moving"
1. Check that `world.applyMotorCommand()` is being called
2. Verify that `world.update()` is being called
3. Ensure motor commands are valid

### "Everything is 0"
1. Brains need time to "warm up" - try more steps
2. Some neurons need input to fire - ensure sensory input is connected
3. Check initial conditions and parameters

## Migration Guide

### From Previous Versions

| Old Pattern | New Pattern |
|-------------|-------------|
| `NLMBrain()` | `pynlm.createBrain(config)` |
| `step(0, 0.0)` | `step(0)` |
| Manual memory management | Automatic memory management |
| Limited API | Complete API with documentation |

## Future Enhancements

The following features are planned for future releases:

1. **Enhanced Sensory Processing**: More sophisticated vision, audio, and internal signal processing
2. **Advanced Learning**: Meta-learning and continual learning capabilities
3. **Social Learning**: Multi-agent interaction and social learning
4. **External Interfaces**: Connection to real sensors and actuators
5. **Visualization Tools**: Advanced visualization of neural activity

## Conclusion

The enhanced NLM Python API provides:

- **Complete brain integration** with all cognitive systems
- **Easy-to-use API** with comprehensive documentation
- **Production-ready** with error handling and examples
- **Extensible architecture** for research and experimentation
- **Performance-optimized** for efficient simulation

Get started today with your artificial brain research!
