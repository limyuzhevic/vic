# NLM Python API Reference

## Overview

NLM (熙然, meaning "serene flow") is an experimental computational brain project that implements a complete artificial neural system with spiking neurons, learning, memory, and cognition. The Python bindings provide convenient access to the C++ NLM framework.

## Installation

```bash
pip install pynlm
```

Or build from source:
```bash
pip install -e .
```

## Core Classes

### Config
Configuration management for NLM systems.

#### Basic Usage
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("brain.region_count", 3)
config.set("plasticity.stdp.enable", True)

# Save configuration
config.saveToFile("my_config.json")
```

#### Configuration Parameters
```python
# Brain configuration
config.set("brain.neuron_count", 1000)          # Total neurons
config.set("brain.synapse_density", 0.1)         # Connection density
config.set("brain.connection_probability", 0.05) # Connection chance

# Plasticity configuration
config.set("plasticity.stdp.enable", True)      # Enable STDP learning
config.set("plasticity.stdp.learning_rate", 0.001)
config.set("plasticity.hebbian.enable", True)   # Enable Hebbian learning

# Neuromodulation
config.set("neuromod.dopamine.scale", 1.0)      # Dopamine modulation

# Development
config.set("development.stage_transition_rate", 0.001)
```

### Brain
Central neural simulation brain class with integrated systems.

#### Basic Usage
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

#### Advanced Usage
```python
# Brain with regions
config = pynlm.createDefaultConfig()
config.set("brain.region_count", 5)
brain = pynlm.createBrain(config)
brain.initialize()

# Access specific regions
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    print(f"Region {region_id}: {region.getTotalNeuronCount()} neurons")

# Get statistics
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "total_spikes": brain.getTotalSpikeCount(),
    "e_i_ratio": brain.getExcitationInhibitionRatio(),
    "development_stage": brain.getDevelopmentalStage(),
}
print(stats)
```

### AgentBrain
Interface connecting NLM brain to the simulated world.

#### Basic Usage
```python
import pynlm

# Create complete agent setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# Run simulation episode
def run_episode(num_steps=500):
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Show progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Curiosity={agent.getCuriosityLevel():.3f}, "
                  f"Novelty={agent.getNoveltyLevel():.3f}, "
                  f"Stage={brain.getDevelopmentalStage()}")

run_episode(500)
```

### SimpleWorld
2D world simulation environment for NLM agents.

#### Basic Usage
```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(
    width=30,
    height=30,
    visionWidth=8,
    visionHeight=8
)

# Set agent starting position
world.setAgentStart(15.0, 15.0)

# Reset to initial state
world.reset()

# Access world properties
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Agent position: ({world.getAgentBody().x}, {world.getAgentBody().y})")

# Run world updates
for _ in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    
    # Process percept as needed...
    
    print(f"Simulation time: {world.getSimulationTime():.2f}")
```

### SensoryInput Classes
Sensory input data structures for different modalities.

#### Vision (Most Common)
```python
import pynlm

# Create vision sensory input
vision = pynlm.Vision(width=8, height=8, channels=3)

# Set vision data (example: grayscale image)
data = [0.1, 0.3, 0.5, 0.7, 0.9] * 16  # 5x16 = 80 values for 8x8x1
vision.setData(data)

print(f"Vision dimensions: {vision.getWidth()}x{vision.getHeight()}")
print(f"Vision data length: {len(vision.getData())}")
```

#### Audio
```python
import pynlm
import numpy as np

# Create audio sensory input
audio = pynlm.Audio(sampleRate=44100, numSamples=1024)

# Generate example audio signal
t = np.linspace(0, 1, 1024)
audio_data = np.sin(2 * np.pi * 440 * t)  # A4 note
audio.setData(audio_data.tolist())
audio.setSampleRate(44100)

print(f"Audio: {audio.getNumSamples()} samples @ {audio.getSampleRate()} Hz")
```

#### InternalSignals
```python
import pynlm

# Create internal signals (used for internal neuron state)
internal = pynlm.InternalSignals()
internal.addSignal(0.5)
internal.addSignal(0.8)
internal.addSignal(0.3)

print(f"Internal signals: {internal.getData()}")
```

### Action
Motor action representation for agent behavior.

#### Basic Actions
```python
import pynlm

# Create predefined actions
move_forward = pynlm.Action(pynlm.ActionType.MoveForward)
turn_left = pynlm.Action(pynlm.ActionType.TurnLeft)
interact = pynlm.Action(pynlm.ActionType.Interact)

# Create actions with parameters
custom_move = pynlm.Action(
    pynlm.ActionType.Custom,
    parameters=[1.5, 0.5]  # distance, angle
)

print(f"Move action type: {move_forward.getType()}")
print(f"Move action name: {move_forward.getName()}")
print(f"Custom action params: {custom_move.getParameters()}")
```

## Configuration Options Reference

### Brain Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |

### Plasticity Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable Spike-Timing-Dependent Plasticity |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP potentiation time constant (ms) |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP depression time constant (ms) |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity (synaptogenesis/pruning) |

### Neuromodulation Configuration
| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Enumerations

### NeuronType
- `NeuronType.Excitatory`: Excitatory neurons
- `NeuronType.Inhibitory`: Inhibitory neurons
- `NeuronType.Modulatory`: Modulatory neurons
- `NeuronType.Sensory`: Sensory input neurons
- `NeuronType.Motor`: Motor output neurons
- `NeuronType.Internal`: Internal state neurons

### ActionType
- `ActionType.MoveForward`: Move forward one unit
- `ActionType.MoveBackward`: Move backward one unit
- `ActionType.TurnLeft`: Turn left 90 degrees
- `ActionType.TurnRight`: Turn right 90 degrees
- `ActionType.Interact`: Interact with environment
- `ActionType.Eat`: Consume food resource
- `ActionType.Drink`: Drink water resource
- `ActionType.Rest`: Rest/stay in place
- `ActionType.Wait`: Wait one time step

### DevelopmentalStage
- `DevelopmentalStage.Initial`: Initial developmental stage
- `DevelopmentalStage.CriticalPeriod`: Critical period with high plasticity
- `DevelopmentalStage.Maturation`: Maturation phase
- `DevelopmentalStage.Adult`: Adult stable state
- `DevelopmentalStage.Aging`: Aging phase

## Working Examples

### 1. Simple Brain Simulation
```python
import pynlm

# Create and run simple brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} total spikes")
```

### 2. Complete Agent Simulation
```python
import pynlm

def complete_agent_simulation(steps=1000):
    """Complete agent with all learning subsystems enabled."""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    world.reset()
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        
        # Sense
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Think
        brain.step(step)
        
        # Act
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 200 == 0:
            print(f"Step {step}: "
                  f"Neurons={brain.getFiringNeuronCount()}, "
                  f"Curiosity={agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world

# Run example
brain, agent, world = complete_agent_simulation(500)
print("Simulation completed successfully!")
```

### 3. World Exploration Agent
```python
import pynlm
import random

def create_explorer_agent(neuron_count=500):
    """Create an agent that explores and learns about its environment."""
    
    # Configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.dopamine.scale", 1.5)
    
    # Create agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=7, visionHeight=7)
    world.reset()
    
    # Place agent randomly
    world.setAgentStart(random.uniform(2, 23), random.uniform(2, 23))
    agent.initialize(world)
    
    # Enable exploration
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    return brain, agent, world

# Run exploration
brain, agent, world = create_explorer_agent(300)

print("Starting exploration simulation...")
for step in range(300):
    world.update(0.1)
    
    # Sense environment
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Think and act
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect reward
    agent.applyRewardModulation(0.1, 0.0)  # Small reward

print(f"Exploration complete!")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Total neurons explored: {brain.getTotalNeuronCount()}")
```

## Error Handling

### Common Exceptions
```python
import pynlm

try:
    # Create brain without initialization
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    # This will fail when we try to step without initializing
    brain.step(0)
except pynlm.RuntimeError as e:
    print(f"Brain initialization required: {e}")
except Exception as e:
    print(f"Other error: {e}")
```

## Performance Tips

1. **Reuse configurations**: Create one config and modify it for multiple brains
2. **Batch operations**: Use the brain statistics methods for comprehensive info
3. **Memory management**: Let objects go out of scope for automatic cleanup
4. **Simulation speed**: Adjust timestep and steps based on your needs

## Migration Guide

### From C++ to Python
- **C++**: `std::shared_ptr<Config> config = std::make_shared<Config>();`
- **Python**: `config = pynlm.createDefaultConfig()`

- **C++**: `Brain brain(config);`
- **Python**: `brain = pynlm.createBrain(config)`

- **C++**: `brain.initialize();`
- **Python**: `brain.initialize()` (same)

- **C++**: `brain.step(step);`
- **Python**: `brain.step(step)` (same)

### Best Practices
1. Always call `initialize()` before using the brain
2. Use context managers for resource management
3. Configure your brain before initialization
4. Monitor simulation statistics regularly
5. Save checkpoints frequently for long simulations

## Next Steps

1. **Read**: `HOW_TO_USE.md` for detailed usage instructions
2. **Explore**: `docs/ARCHITECTURE.md` to understand system design
3. **Experiment**: Try different configurations and observe behavior
4. **Learn**: Study the source code to understand NLM internals
5. **Contribute**: Add new features or improve existing functionality

The NLM Python API provides a powerful, intuitive interface to a sophisticated computational brain. Start with the simple examples and gradually explore the advanced features as you become comfortable with the system.