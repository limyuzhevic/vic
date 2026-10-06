# NLM Python Bindings Enhancement

## Overview

This document describes the enhancements made to the NLM Python bindings in `python/bindings.cpp`. The bindings provide a Python interface to the NLM C++ neural simulation framework, enabling users to create and simulate artificial brains with advanced learning capabilities.

---

## Binding Overview

The pybind11 module `pynlm` exposes the following main components:

### Core Brain Components
- `Brain` - Central neural simulation brain class
- `Config` - Configuration management system
- `AgentBrain` - Agent interface connecting brain to world
- `SimpleWorld` - Environment simulation
- `SensoryInput` and subclasses - Sensory processing
- `Action` and subclasses - Motor output actions
- `WorldObject` - World entities and objects

### Supporting Classes
- `NeuronId`, `SynapseId`, `RegionId`, `PopulationId` - Unique identifiers
- Enumerations: `NeuronType`, `SynapseType`, `DevelopmentalStage`, etc.

### Helper Classes
- `AgentBody` - Agent state and physics
- `SensoryPercept` - Collected sensory information
- `ActionResult` - Results of actions

---

## Class Documentation

### Config Class

**Purpose**: Configuration management system for NLM

**Key Methods**:
- `loadFromFile(filepath)` - Load configuration from file
- `loadFromArgs(argc, argv)` - Load from command line arguments
- `saveToFile(filepath)` - Save configuration to file
- `set(key, value, source)` - Set configuration value
- `get(key)` - Get configuration value
- `getOr(key, default)` - Get with default value

**Python Usage**:
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Modify configuration
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)

# Get values
neuron_count = config.getOr("brain.neuron_count", 1000)
has_stdp = config.has("plasticity.stdp.enable")

# Save configuration
config.saveToFile("my_config.json")
```

### Brain Class

**Purpose**: Central neural simulation engine with LIF neurons and learning systems

**Key Methods**:
- `initialize()` - Initialize brain with configuration
- `step(step, time)` - Perform simulation step
- `reset()` - Reset brain state
- `receiveSensoryInput(input)` - Process sensory data
- `produceAction()` - Generate motor action

**Statistics**:
- `getTotalNeuronCount()` - Total neurons in brain
- `getTotalSynapseCount()` - Total synaptic connections
- `getFiringNeuronCount()` - Currently active neurons
- `getAverageFiringRate()` - Average firing rate
- `getExcitationInhibitionRatio()` - E/I balance
- `getTotalSpikeCount()` - Total spikes recorded

**Python Usage**:
```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for step in range(1000):
    brain.step(step)
    print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
```

### AgentBrain Class

**Purpose**: Interface connecting NLM brain to world environment

**Key Methods**:
- `initialize(world)` - Initialize with world interface
- `processSensoryInput(percept)` - Process sensory percept
- `decodeMotorCommand()` - Convert brain activity to action
- `applyRewardModulation(reward, predicted_reward)` - Reward learning
- `updateDevelopment(timestep)` - Brain development

**Subsystems**:
- Reward modulation (learning from rewards)
- Structural plasticity (growing new connections)
- Development (brain maturation)
- Curiosity (exploration behavior)

**Python Usage**:
```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run agent simulation
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
```

### SimpleWorld Class

**Purpose**: Simple 2D environment for NLM agents

**Key Methods**:
- `configure(width, height, visionWidth, visionHeight)` - Set world dimensions
- `reset()` - Reset world to initial state
- `update(timestep)` - Advance world state
- `setAgentStart(x, y)` - Set agent starting position
- `applyMotorCommand(cmd, time)` - Apply action to world

**Sensory Processing**:
- `getSensoryPercept()` - Get current sensory percept
- Vision, touch, internal signals processing

**Agent State**:
- `getAgentBody()` - Get agent position and physics

**World Objects**:
- `addObject(obj)` - Add world object
- `removeObject(x, y)` - Remove object
- `isValidPosition(x, y)` - Check position validity

**Python Usage**:
```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Run world
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Print agent position
    body = world.getAgentBody()
    print(f"Agent at ({body.x:.1f}, {body.y:.1f})")
```

### SensoryInput Classes

#### Vision Class
**Purpose**: Visual sensory input with width/height and color channels

**Key Methods**:
- `getWidth()`, `getHeight()`, `getChannels()` - Get dimensions
- `setData(data)` - Set pixel data

#### Audio Class
**Purpose**: Audio sensory input with sample rate and samples

**Key Methods**:
- `getSampleRate()`, `getNumSamples()` - Audio properties
- `setData(data)` - Set audio samples

#### InternalSignals Class
**Purpose**: Internal brain state signals

**Key Methods**:
- `addSignal(value)` - Add signal value
- `clearSignals()` - Clear all signals

### Action Classes

#### Movement Actions
- `ActionType.MoveForward` - Move forward
- `ActionType.MoveBackward` - Move backward
- `ActionType.MoveLeft` - Move left
- `ActionType.MoveRight` - Move right
- `ActionType.TurnLeft` - Turn left
- `ActionType.TurnRight` - Turn right

#### Perception Actions
- `ActionType.Look` - Look in current direction
- `ActionType.LookUp` - Look up
- `ActionType.LookDown` - Look down
- `ActionType.LookLeft` - Look left
- `ActionType.LookRight` - Look right

#### Interaction Actions
- `ActionType.Interact` - Interact with object
- `ActionType.Eat` - Consume food
- `ActionType.Drink` - Drink water
- `ActionType.Rest` - Rest/renew energy
- `ActionType.Wait` - Wait without action

### WorldObject Class

**Purpose**: Physical objects in the world

**Key Attributes**:
- `x`, `y` - Position coordinates
- `radius` - Object size
- `type` - Object type (Resource, Hazard, Wall, etc.)
- `value` - Object value (energy, reward)
- `active` - Object active state

**Python Usage**:
```python
import pynlm

# Create world object
obj = pynlm.WorldObject(
    x=5.0, y=5.0,
    type=pynlm.WorldObjectType.Resource,
    value=10.0,
    radius=0.5
)

# Add to world
world.addObject(obj)

# Check object type
if obj.type == pynlm.WorldObjectType.Resource:
    print(f"Found resource with value {obj.value}")
```

---

## Advanced Usage Patterns

### Pattern 1: Complete Agent with All Learning Systems

```python
import pynlm

def run_complete_agent_simulation(steps=2000):
    """Run a complete agent simulation with all learning systems enabled."""
    
    # Advanced configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    config.set("neuromod.dopamine.scale", 1.5f)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.novelty.enable", True)
    config.set("replay_interval", 100)
    config.set("consolidation_interval", 500)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=32, height=32, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(16.0, 16.0)
    
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    print("=== Advanced Agent Simulation ===")
    for step in range(steps):
        world.update(0.05)
        
        # Process sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Generate action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Periodic development
        if step % 200 == 0:
            agent.updateDevelopment(0.1)
            
            # Log progress
            print(f"Step {step}:")
            print(f"  - Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  - Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  - Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  - Development: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_agent_simulation()
```

### Pattern 2: Memory-Enhanced Learning

```python
import pynlm

def run_memory_enhanced_simulation(steps=1500):
    """Run simulation with focus on memory systems."""
    
    # Configuration optimized for memory learning
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("replay_interval", 75)
    config.set("consolidation_interval", 300)
    config.set("working_memory_capacity", 150)
    config.set("episodic_memory.max_episodes", 1000)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=24, height=24, visionWidth=10, visionHeight=10)
    world.reset()
    
    agent.initialize(world)
    
    # Enable memory systems
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    print("=== Memory-Enhanced Learning Simulation ===")
    
    for step in range(steps):
        world.update(0.1)
        
        # Process input and learn
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reinforcement learning
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Monitor memory systems
        if step % 100 == 0:
            wm = brain.getWorkingMemory()
            em = brain.getEpisodicMemory()
            
            print(f"Step {step}:")
            if wm:
                print(f"  - Working memory: {wm.getActiveTraces()} traces")
            if em:
                print(f"  - Episodic memory: {em.getEpisodeCount()} episodes")
            print(f"  - Development stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run memory-enhanced simulation
brain, agent, world = run_memory_enhanced_simulation()
```

### Pattern 3: Neuromodulation Focus

```python
import pynlm

def run_neuromodulation_simulation(steps=1000):
    """Run simulation focusing on neuromodulation systems."""
    
    # Configuration with strong neuromodulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    config.set("neuromod.dopamine.scale", 2.0f)
    config.set("neuromod.curiosity.enable", True)
    config.set("neuromod.curiosity.threshold", 0.4f)
    config.set("neuromod.novelty.enable", True)
    config.set("neuromod.novelty.sensitivity", 1.2f)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("=== Neuromodulation Focus Simulation ===")
    
    for step in range(steps):
        world.update(0.1)
        
        # Process input and learn
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Log neuromodulation levels
        if step % 100 == 0:
            print(f"Step {step}:")
            if brain.getDopamine():
                print(f"  - Dopamine level: {brain.getDopamine().getLevel():.3f}")
            print(f"  - Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  - Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  - Reward prediction: {agent.getNeuromodulationLevel():.3f}")
    
    return brain, agent, world

# Run neuromodulation simulation
brain, agent, world = run_neuromodulation_simulation()
```

---

## Performance Tips

### Memory Management
1. **Reuse objects**: Create and reuse agent/world objects instead of creating new ones
2. **Configure appropriately**: Use minimal configuration for testing, advanced for production
3. **Monitor memory**: Use `brain.getWorkingMemory()` and `brain.getEpisodicMemory()` to monitor memory usage

### Simulation Optimization
1. **Batch operations**: Group similar operations when possible
2. **Adjust timestep**: Use appropriate timesteps for different simulation types
3. **Enable only needed subsystems**: Disable unused learning systems
4. **Monitor progress**: Log periodically but avoid excessive output

### Configuration Best Practices
1. **Start simple**: Use default configuration for initial testing
2. **Incremental changes**: Modify one parameter at a time to measure impact
3. **Save configurations**: Save optimized configurations for reuse
4. **Document settings**: Keep track of effective parameter values

---

## Error Handling

### Common Issues and Solutions

**Import Errors**:
```python
import pynlm
# Error: ImportError: No module named 'pynlm'
# Solution: pip install pynlm or pip install -e .
```

**Runtime Errors**:
```python
brain = pynlm.createBrain(config)
brain.initialize()
# Error: Segmentation fault
# Solution: Check that initialize() is called before step()
```

**Configuration Errors**:
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", -100)  # Invalid value
# Solution: Use valid parameter ranges
```

### Robust Code Example

```python
import pynlm
import sys

def safe_run_simulation():
    try:
        # Create and initialize brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent and world
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent.initialize(world)
        
        # Run simulation
        for step in range(1000):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Periodic logging
            if step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        return True
        
    except Exception as e:
        print(f"Simulation failed: {e}")
        return False

if __name__ == "__main__":
    if safe_run_simulation():
        print("Simulation completed successfully!")
    else:
        print("Simulation failed.")
        sys.exit(1)
```

---

## Migration Guide

### From Basic to Advanced Usage

**Basic Example**:
```python
# Original simple usage
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
```

**Enhanced Example**:
```python
# Advanced usage with full features
import pynlm

# Configure advanced settings
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.curiosity.enable", True)

# Create brain and agent
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### From Single Agent to Multi-Agent

```python
# Single agent (original)
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Multi-agent (advanced)
import pynlm

# Create separate brains and agents
brains = []
agents = []
worlds = []

for i in range(3):
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Set different starting positions
    world.setAgentStart(i * 10.0 + 5.0, i * 10.0 + 5.0)
    
    agent.initialize(world)
    
    brains.append(brain)
    agents.append(agent)
    worlds.append(world)

# Run multi-agent simulation
for step in range(500):
    for i in range(3):
        worlds[i].update(0.1)
        agents[i].processSensoryInput(worlds[i].getSensoryPercept())
        brains[i].step(step)
        action = agents[i].decodeMotorCommand()
        worlds[i].applyMotorCommand(action, worlds[i].getSimulationTime())
```

---

## Conclusion

The enhanced NLM Python bindings provide comprehensive access to the NLM artificial brain simulation framework. By following these patterns and best practices, users can:

1. **Create sophisticated agents** with advanced learning capabilities
2. **Configure complex simulation environments** with custom parameters
3. **Implement memory-based learning** using NLM's memory systems
4. **Utilize neuromodulation** for curiosity-driven exploration
5. **Build robust simulation applications** with proper error handling

The bindings are designed to be both accessible to beginners and powerful enough for advanced research applications, making NLM suitable for a wide range of artificial intelligence and neuroscience applications.

---

## References

1. NLM Documentation: https://nlm.readthedocs.io
2. Phase 6 Integration Guide: Available in docs/ directory
3. Scientific Papers: Available in docs/ directory
4. GitHub Repository: https://github.com/nlm-project/nlm

---

**Version**: 2.0
**Updated**: 2026
**Authors**: NLM Development Team