# HOW_TO_USE.md

# NLM - Comprehensive Usage Guide

This document provides advanced usage patterns, troubleshooting tips, and feature demonstrations for NLM users.

## Installation

### Build from Source

```bash
# Create build directory and configure
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# Run tests (if available)
ctest --output-on-failure
```

### Install Python Package

```bash
# Using pip (will build from source if no wheels available)
pip install .

# Or develop mode
pip install -e .
```

## Python API Reference

### Core Brain Interface

#### Creating and Initializing Brains

```python
import pynlm
import numpy as np

# Basic brain creation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Brain with custom configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Brain Simulation Loop

```python
import pynlm
import time

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Main simulation loop
for step in range(1000):
    # Update environment
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory data
    agent.processSensoryInput(percept)
    
    # Brain processing
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Optional: Apply neuromodulation
    # reward = world.getAgentBody().energy
    # predicted = agent.getPredictionError()
    # agent.applyRewardModulation(reward, predicted)
    
    # Log statistics periodically
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
```

#### Brain State Management

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Save brain state
save_path = "brain_state.json"
brain.save(save_path)
print(f"Brain state saved to {save_path}")

# Load brain state
load_brain = pynlm.createBrain(pynlm.createDefaultConfig())
load_brain.load(save_path)
print("Brain state loaded successfully")

# Reset brain
brain.reset()
```

#### Configuration Management

```python
import pynlm
import json

# Create configuration
config = pynlm.createDefaultConfig()

# Access and modify configuration parameters
config.clear()

# Add custom configuration keys
config.set("brain.neuron.count", 1000)
config.set("brain.plasticity.stdp.learning_rate", 0.01)
config.set("brain.development.growth_rate", 0.1)
config.set("environment.energy.decay_rate", 0.001)

# Check configuration
if config.has("brain.neuron.count"):
    print(f"Neuron count: {config.get('brain.neuron.count')}")

# Save configuration
config.saveToFile("my_config.json")

# Load configuration
config.loadFromFile("my_config.json")

# Get configuration summary
print(config.summary())
```

#### Advanced Brain Regions

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Add neural regions
brain.addRegion("sensory_cortex")
brain.addRegion("motor_cortex")
brain.addRegion("hippocampus")

# Access regions
regions = brain.getRegions()
print(f"Created {len(regions)} regions:")
for region_id in brain.getRegionIds():
    region = brain.getRegion(region_id)
    print(f"  Region {region_id}: neuron count = {region.getNeuronCount()}")

# Create region-specific populations
population = brain.createPopulation("sensory_cortex", 100, pynlm.NeuronType.Excitatory)
```

#### Sensory Processing

```python
import pynlm
import numpy as np

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

# Get sensory percepts
percept = world.getSensoryPercept()

# Access different sensory modalities
if hasattr(percept, 'getVision'):
    vision = percept.getVision()
    print(f"Vision shape: {vision.getWidth()}x{vision.getHeight()}")

if hasattr(percept, 'getTouch'):
    touch = percept.getTouch()
    print(f"Touch data: {len(touch)} signals")

if hasattr(percept, 'getInternal'):
    internal = percept.getInternal()
    print(f"Internal signals: {len(internal.getAllSignals())}")

# Create custom sensory input
sensory = pynlm.Vision(width=8, height=8, channels=3)
data = np.random.rand(8, 8, 3).astype(np.float32)
sensory.setData(data)
percept.setVision(sensory)
```

#### Motor Actions

```python
import pynlm

# Create actions
move_action = pynlm.Action(pynlm.ActionType.MoveForward)
print(f"Action type: {move_action.getType()}")
print(f"Action name: {move_action.getName()}")

# Custom action with parameters
custom_action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 1.0, 2.0])
params = custom_action.getParameters()
print(f"Custom parameters: {params}")

# Action types enumeration
for action_type in pynlm.ActionType:
    print(f"{action_type.name}: {action_type.value}")
```

#### Neuromodulation and Learning

```python
import pynlm

# Create agent brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
agent = pynlm.createAgentBrain(brain)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Check learning status
print(f"Reward modulation enabled: {agent.isRewardModulationEnabled()}")
print(f"Curiosity enabled: {agent.isCuriosityEnabled()}")
print(f"Development enabled: {agent.isDevelopmentEnabled()}")
print(f"Structural plasticity enabled: {agent.isStructuralPlasticityEnabled()}")

# Apply neuromodulation
reward = 1.0
predicted = agent.getPredictionError()
agent.applyRewardModulation(reward, predicted)

# Monitor neuromodulation levels
print(f"Current neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
print(f"Current curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Current novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Current prediction error: {agent.getPredictionError():.3f}")

# Update development
agent.updateDevelopment(0.1)
print(f"New developmental stage: {agent.getDevelopmentalStage()}")
```

#### World Management

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setAgentStart(10, 10)  # Set agent starting position

# Add objects to world
world_obj = pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, value=10.0)
world.addObject(world_obj)

# Update world
for step in range(100):
    world.update(0.1)
    
    # Check environment state
    body = world.getAgentBody()
    print(f"Agent at ({body.x:.1f}, {body.y:.1f}), energy: {body.energy:.2f}")

# Check if position is valid
if world.isValidPosition(15, 15):
    print("Position is valid")
```

### Advanced Features

#### Custom Brain Configurations

```python
import pynlm

# Create specialized configuration for cognitive tasks
config = pynlm.createDefaultConfig()

# Optimize for cognitive tasks
config.set("brain.cognition.planner.enabled", True)
config.set("brain.memory.episodic.enabled", True)
config.set("brain.memory.working.capacity", 50)
config.set("brain.plasticity.stdp.enable", True)
config.set("brain.development.enable", True)

# Create brain with configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Multi-Agent Systems

```python
import pynlm

# Create multiple brains for multi-agent simulation
brains = []
for i in range(5):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    brains.append(brain)

# Create agents for each brain
agents = []
for i, brain in enumerate(brains):
    agent = pynlm.createAgentBrain(brain)
    agents.append(agent)

# Run multi-agent simulation
for step in range(100):
    # Each agent processes sensory input
    for i, agent in enumerate(agents):
        agent.processSensoryInput(percept)
        brain = agent.getBrain()
        brain.step(step)
        action = agent.decodeMotorCommand()
        # Apply action to respective world/agent
```

#### Experimental Protocols

```python
import pynlm

# Setup experiment configuration
experiment_config = {
    "name": "memory_learning",
    "duration": 1000,
    "features": ["reward_modulation", "curiosity", "development"],
    "metrics": ["firing_rate", "memory_retention", "learning_speed"]
}

# Run controlled experiment
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Enable all features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run experiment with monitoring
for step in range(experiment_config["duration"]):
    world.update(0.1)
    
    # Collect data for analysis
    data = {
        "step": step,
        "firing_rate": brain.getAverageFiringRate(),
        "total_spikes": brain.getTotalSpikeCount(),
        "development_stage": brain.getDevelopmentalStage(),
        "neuromodulation": agent.getNeuromodulationLevel()
    }
    
    # Store data for post-experiment analysis
    world.update(0.1)
```

#### Performance Monitoring

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Monitor brain performance over time
performance_metrics = {
    "steps": [],
    "firing_rates": [],
    "spike_counts": [],
    "neuron_counts": [],
    "excitation_inhibition_ratios": []
}

start_time = time.time()
for step in range(1000):
    brain.step(step)
    
    # Collect performance metrics
    performance_metrics["steps"].append(step)
    performance_metrics["firing_rates"].append(brain.getAverageFiringRate())
    performance_metrics["spike_counts"].append(brain.getTotalSpikeCount())
    performance_metrics["neuron_counts"].append(brain.getTotalNeuronCount())
    performance_metrics["excitation_inhibition_ratios"].append(
        brain.getExcitationInhibitionRatio()
    )

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
print(f"Average steps per second: {1000 / (end_time - start_time):.2f}")

# Calculate statistics
avg_firing_rate = sum(performance_metrics["firing_rates"]) / len(performance_metrics["firing_rates"])
print(f"Average firing rate over simulation: {avg_firing_rate:.2f} Hz")
```

### Troubleshooting

#### Common Issues and Solutions

**"My brain isn't doing anything"**

* Check: Did you call `brain.initialize()`? Always initialize before use.
* Solution: Ensure you're calling `brain.step()` in your simulation loop.
* Try: Increase simulation steps to allow brain "warm up"

**"The agent isn't moving"**

* Check: Are you calling `world.applyMotorCommand()` after `agent.decodeMotorCommand()`?
* Solution: Verify that `world.update()` is being called to advance the simulation

**"Everything is 0"**

* Check: Brain needs time to "warm up" - try more steps
* Ensure sensory input is connected to the agent's percept processing
* Verify that motor neurons are properly connected to action output

**"Configuration errors"**

* Check: Configuration keys use dot notation (e.g., "brain.neuron.count")
* Solution: Use `config.has()` to check if keys exist before accessing
* Try: Reset configuration to defaults with `config.clear()`

**"Memory issues during long simulations"**

* Solution: Monitor brain state and save periodically to reduce memory usage
* Try: Reduce simulation duration or increase checkpoint frequency
* Consider: Using a more powerful machine for long simulations

#### Advanced Debugging

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable detailed logging
brain.logStatus()

# Debug step-by-step processing
for step in range(10):
    print(f"\n=== Step {step} ===")
    brain.step(step)
    
    # Check neuron states
    active_count = brain.getActiveNeuronCount()
    firing_count = brain.getFiringNeuronCount()
    print(f"Active neurons: {active_count}")
    print(f"Firing neurons: {firing_count}")
    
    # Check brain health
    if brain.getTotalNeuronCount() == 0:
        print("ERROR: Brain has no neurons!")
        break
```

### Best Practices

#### For Beginners

1. **Always initialize**: Call `brain.initialize()` before using the brain
2. **Start small**: Begin with simple simulations (10-100 steps) before scaling up
3. **Use the easy usage guide**: Start with `easy_usage.md` for basic patterns
4. **Monitor metrics**: Track firing rates, spike counts, and development stages
5. **Save progress**: Save brain states periodically for long simulations

#### For Intermediate Users

1. **Customize configurations**: Modify configuration parameters for specific tasks
2. **Experiment with features**: Enable/disable different learning systems
3. **Multi-step processing**: Implement complex behavior with sensory-motor loops
4. **Performance optimization**: Monitor and optimize simulation speed
5. **Error handling**: Implement robust error checking and recovery

#### For Advanced Users

1. **Custom environments**: Create specialized worlds with unique dynamics
2. **Neural architectures**: Design custom brain regions and connection patterns
3. **Research protocols**: Implement controlled experiments with proper data collection
4. **Performance tuning**: Optimize for specific use cases and hardware
5. **Integration**: Combine NLM with other systems or frameworks

### Additional Resources

- **Phase 6 Demo**: Run `./nlm_phase6_demo` in the build directory for comprehensive integration test
- **Architecture Documentation**: See `docs/ARCHITECTURE.md` for technical details
- **Scientific Background**: Refer to `docs/SCIENCE.md` for research context
- **Development Roadmap**: Check `docs/ROADMAP.md` for future directions
- **Phase 6 Final Audit**: Review `docs/PHASE6_FINAL_AUDIT.md` for implementation status

## API Quick Reference

| Method | Class | Description |
|--------|-------|-------------|
| `createBrain()` | Module | Create a new brain instance |
| `createSimpleWorld()` | Module | Create a simple 2D world |
| `createAgentBrain()` | Module | Create agent interface |
| `createDefaultConfig()` | Module | Create default configuration |
| `initialize()` | Brain/AgentBrain | Initialize the brain or agent |
| `step()` | Brain | Process one simulation step |
| `processSensoryInput()` | AgentBrain | Process sensory percepts |
| `decodeMotorCommand()` | AgentBrain | Get motor action from brain |
| `applyMotorCommand()` | World | Apply action to world state |
| `getSensoryPercept()` | World | Get current sensory input |
| `getAgentBody()` | World | Get agent physical state |
| `save()`/`load()` | Brain | Save/load brain state |

## License

MIT License. See LICENSE file for details.
