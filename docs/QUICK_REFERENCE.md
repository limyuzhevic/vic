# NLM Quick Reference Guide

This quick reference provides essential commands and patterns for working with NLM, covering both basic and advanced usage.

## Installation

### From Source (Recommended)
```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install
pip install .

# Or install in development mode
pip install -e .
```

### Verification
```python
import pynlm

# Check version
print(pynlm.__version__)

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Neurons: {brain.getTotalNeuronCount()}")
```

## Core Classes and Methods

### Brain Class - Core Neural Processing

| Method | Description | Example |
|--------|-------------|---------|
| `brain.initialize()` | Initialize brain with configuration | `brain.initialize()` |
| `brain.step(step_number)` | Run one simulation step | `brain.step(5)` |
| `brain.step(step_number, time)` | Run step with timestamp | `brain.step(5, 0.1)` |
| `brain.reset()` | Reset brain state | `brain.reset()` |
| `brain.getTotalNeuronCount()` | Get total neuron count | `brain.getTotalNeuronCount()` |
| `brain.getFiringNeuronCount()` | Get firing neuron count | `brain.getFiringNeuronCount()` |
| `brain.getAverageFiringRate()` | Get average firing rate | `brain.getAverageFiringRate()` |
| `brain.getTotalSpikeCount()` | Get total spike count | `brain.getTotalSpikeCount()` |
| `brain.getDevelopmentalStage()` | Get developmental stage | `brain.getDevelopmentalStage()` |

### Config Class - Configuration Management

| Method | Description | Example |
|--------|-------------|---------|
| `config.set(key, value)` | Set configuration value | `config.set("brain.neuron_count", 1000)` |
| `config.get(key)` | Get configuration value | `config.get("brain.neuron_count")` |
| `config.has(key)` | Check if key exists | `config.has("brain.neuron_count")` |
| `config.loadFromFile(filepath)` | Load from JSON file | `config.loadFromFile("config.json")` |
| `config.saveToFile(filepath)` | Save to JSON file | `config.saveToFile("config.json")` |
| `config.getKeys()` | Get all configuration keys | `config.getKeys()` |
| `config.summary()` | Get configuration summary | `config.summary()` |

### AgentBrain Class - World Interface

| Method | Description | Example |
|--------|-------------|---------|
| `agent.initialize(world)` | Initialize with world | `agent.initialize(world)` |
| `agent.processSensoryInput(percept)` | Process sensory input | `agent.processSensoryInput(percept)` |
| `agent.decodeMotorCommand()` | Get motor command | `agent.decodeMotorCommand()` |
| `agent.applyRewardModulation(reward, predicted)` | Apply reward learning | `agent.applyRewardModulation(1.0, 0.8)` |
| `agent.updateDevelopment(timestep)` | Update development | `agent.updateDevelopment(0.1)` |
| `agent.enableRewardModulation(True)` | Enable/disable features | `agent.enableRewardModulation(True)` |

### SimpleWorld Class - Environment Simulation

| Method | Description | Example |
|--------|-------------|---------|
| `world.configure(width, height, visionWidth, visionHeight)` | Configure world dimensions | `world.configure(20, 20, 8, 8)` |
| `world.update(timestep)` | Update world state | `world.update(0.1)` |
| `world.reset()` | Reset world to initial state | `world.reset()` |
| `world.applyMotorCommand(cmd, time)` | Apply agent action | `world.applyMotorCommand(cmd, time)` |
| `world.getSensoryPercept()` | Get agent's sensory input | `world.getSensoryPercept()` |
| `world.getAgentBody()` | Get agent body state | `world.getAgentBody()` |

## Essential Patterns

### Pattern 1: Basic Brain Simulation

```python
import pynlm

def basic_simulation(steps=100):
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    for step in range(steps):
        brain.step(step)
        
        # Log statistics
        if step % 10 == 0:
            print(f"Step {step}:")
            print(f"  Neurons: {brain.getTotalNeuronCount()}")
            print(f"  Firing: {brain.getFiringNeuronCount()}")
            print(f"  Rate: {brain.getAverageFiringRate():.3f}")
    
    return brain

brain = basic_simulation(500)
```

### Pattern 2: Complete Agent-Environment System

```python
import pynlm

def run_agent_simulation(steps=1000):
    # 1. Create brain and agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    
    # 2. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # 3. Initialize agent
    agent.initialize(world)
    
    # 4. Enable learning features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # 5. Run simulation loop
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and execute
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world

brain, agent, world = run_agent_simulation(1000)
```

### Pattern 3: Advanced Configuration

```python
import pynlm

def create_custom_brain(neurons=2000, learning_rate=0.01):
    # Create default config
    config = pynlm.createDefaultConfig()
    
    # Set advanced parameters
    config.set("brain.neuron_count", neurons)
    config.set("brain.connection_probability", 0.1)
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("neuromod.dopamine.scale", 1.0)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

# Use custom brain
brain = create_custom_brain(neurons=5000, learning_rate=0.005)
```

## Enumerations (Important Constants)

### Neuron Types
```python
pynlm.NeuronType.Excitatory      # Standard excitatory neurons
pynlm.NeuronType.Inhibitory      # Inhibitory neurons
pynlm.NeuronType.Modulatory       # Neuromodulatory neurons
pynlm.NeuronType.Sensory          # Sensory neurons
pynlm.NeuronType.Motor            # Motor neurons
pynlm.NeuronType.Internal         # Internal state neurons
```

### Developmental Stages
```python
pynlm.DevelopmentalStage.Initial      # Early development
pynlm.DevelopmentalStage.CriticalPeriod  # Sensitive learning period
pynlm.DevelopmentalStage.Maturation   # Maturation phase
pynlm.DevelopmentalStage.Adult        # Adult, stable state
pynlm.DevelopmentalStage.Aging        # Aging phase
```

### Action Types
```python
pynlm.ActionType.MoveForward     # Move forward
pynlm.ActionType.MoveBackward    # Move backward
pynlm.ActionType.TurnLeft        # Turn left
pynlm.ActionType.TurnRight       # Turn right
pynlm.ActionType.Interact        # Interact with object
pynlm.ActionType.Eat             # Eat resource
pynlm.ActionType.Drink           # Drink resource
pynlm.ActionType.Rest            # Rest/hold position
pynlm.ActionType.Wait            # Wait (do nothing)
```

### World Object Types
```python
pynlm.WorldObjectType.Empty      # Empty space
pynlm.WorldObjectType.Resource   # Resource object
pynlm.WorldObjectType.Hazard     # Hazardous object
pynlm.WorldObjectType.Wall       # Wall/collision object
pynlm.WorldObjectType.Marker     # Marker/teleport
```

## Troubleshooting

### "My brain isn't doing anything"
```python
# Common issue: forgetting to initialize
brain = pynlm.createBrain(config)
brain.initialize()  # <<< MISSING THIS STEP! >>>
```

### "The agent isn't moving"
```python
# Ensure world updates and actions are applied
world.update(0.1)                    # Update world physics
agent.processSensoryInput(percept)   # Agent perceives environment
brain.step(step)                     # Brain processes
action = agent.decodeMotorCommand()  # Brain decides action
world.applyMotorCommand(action, world.getSimulationTime())  # Execute action
```

### "Everything is 0"
```python
# Brains need time to "warm up" - try more steps
for i in range(100):  # More steps = better results
    brain.step(i)

# Make sure sensory input is connected
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()
```

## Quick Commands Summary

| Command | Use Case |
|---------|----------|
| `import pynlm` | Always first - import library |
| `pynlm.createBrain(config)` | Create neural brain |
| `brain.initialize()` | Initialize brain (NEVER skip!) |
| `brain.step(step)` | Run one simulation step |
| `pynlm.createSimpleWorld()` | Create environment |
| `world.configure(w,h,vw,vh)` | Set world dimensions |
| `world.reset()` | Reset environment |
| `pynlm.createAgentBrain(brain)` | Create agent interface |

## Advanced Commands (for Power Users)

```python
# All at once - enable all advanced features
agent.enableAllFeatures(True)

# Batch operations for performance
brain.step_multiple(100)  # Run 100 steps at once
world.spawn_resources_batch(50)  # Spawn 50 resources

# Advanced configuration
config.set_advanced(neuron_count=1000, learning_rate=0.01)

# Performance monitoring
stats = brain.get_performance_stats()
memory = brain.get_memory_usage()
```

## Next Steps

1. **Read [ADVANCED_USAGE.md](./ADVANCED_USAGE.md)** for advanced features
2. **Read [HOW_TO_USE.md](./HOW_TO_USE.md)** for detailed explanations
3. **Read [docs/ARCHITECTURE.md](./docs/ARCHITECTURE.md)** for system architecture
4. **Experiment with different configurations** and observe results

This reference provides everything you need to get started with NLM and grow into advanced usage as your skills develop!