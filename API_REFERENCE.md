# NLM API Reference Guide

## Overview

The NLM Python API provides access to the Neural Learning Machine neural simulation framework. The API enables creating virtual brains, running simulations, and interacting with the world through an agent system.

## Current Status

**Important:** NLM is in Phase 6 (Final Integration) with approximately 35.8% system integration achieved. The neural core (LIF neurons, spikes, basic plasticity) works excellently, but many cognitive systems (memory, prediction, higher cognition) are disconnected from the main brain loop.

## Core Classes

### Brain Class

The `Brain` class represents the virtual neural system that performs computation.

#### Creation and Initialization
```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create brain
brain = pynlm.createBrain(config)

# Initialize brain (required before use)
brain.initialize()
```

#### Simulation Control
```python
# Run one simulation step
brain.step(step_number)

# Run step with timestamp
brain.step(step_number, current_time)

# Reset brain to initial state
brain.reset()
```

#### Statistics and Monitoring
```python
# Basic statistics
brain.getTotalNeuronCount()        # Total number of neurons (e.g., 1000)
brain.getTotalSynapseCount()       # Total number of synapses
brain.getFiringNeuronCount()       # Currently firing neurons
brain.getAverageFiringRate()       # Average firing rate across all neurons
brain.getExcitationInhibitionRatio()  # E/I balance

# Developmental tracking
brain.getDevelopmentalStage()     # Current developmental stage

# Spike information
brain.getTotalSpikeCount()         # Total spikes fired during simulation
```

#### State Management
```python
# Save brain state (NOT IMPLEMENTED - returns false)
brain.save("checkpoint.bin")

# Load brain state (NOT IMPLEMENTED - returns false)
brain.load("checkpoint.bin")

# Add/get regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Sensory Processing (STUBS)
```python
# Inject sensory data (NOT IMPLEMENTED)
brain.receiveSensoryInput(sensory_input)

# Produce motor action (NOT IMPLEMENTED)
action = brain.produceAction()
```

### Config Class

The `Config` class manages brain and simulation parameters.

#### Creation
```python
config = pynlm.createDefaultConfig()
```

#### Querying Values
```python
# Check if parameter exists
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Get parameter values
t neuron_count = config.getInt("brain.neuron_count")
# or
neuron_count = config.getValue("brain.neuron_count")  # returns generic type
```

#### Modifying Configuration
```python
# Set parameters
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.learning_rate", 0.001)
```

#### File I/O (NOT IMPLEMENTED)
```python
# Load configuration from file (STUB)
config.loadFromFile("config.json")

# Save configuration to file (NOT IMPLEMENTED)
config.saveToFile("config.json")
```

### AgentBrain Class

The `AgentBrain` class provides the interface between the brain and the world through sensory processing and action selection.

#### Creation and Initialization
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

#### Sensory Processing
```python
# Process sensory input from the world
agent.processSensoryInput(percept)

# The agent typically connects to sensory neurons in the brain
# (vision, touch, internal, proprioception)
```

#### Motor Decoding
```python
# Decode motor command from brain activity
motor_cmd = agent.decodeMotorCommand()

# Returns action as motor command
# (MoveForward, TurnLeft, TurnRight, Interact, Eat, Wait)
```

#### Neuromodulation
```python
# Apply reward modulation
agent.applyRewardModulation(reward, predicted_reward)

# Update development
agent.updateDevelopment(timestep)
```

#### Subsystem Control
```python
# Enable/disable learning subsystems
agent.enableRewardModulation(True/False)      # Dopamine-based learning
agent.enableCuriosity(True/False)             # Exploration drive
agent.enableDevelopment(True/False)           # Brain maturation
agent.enableStructuralPlasticity(True/False)  # New connections
```

#### Statistics
```python
# Get neuromodulation levels
agent.getCuriosityLevel()        # Current curiosity (0.0-1.0)
agent.getNoveltyLevel()          # Current novelty detection
agent.getPredictionError()       # Prediction error
agent.getNeuromodulationLevel()  # General neuromodulation level
```

### SimpleWorld Class

The `SimpleWorld` class represents the 2D environment in which the brain operates.

#### Creation and Configuration
```python
world = pynlm.createSimpleWorld()

# Configure world dimensions
world.configure(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8
)
```

#### World Control
```python
# Reset world to initial state
world.reset()

# Update world simulation
timestep = 0.1
world.update(timestep)

# Set random seed for reproducible experiments
world.setRandomSeed(42)

# Set agent starting position
world.setAgentStart(10.0, 10.0)
```

#### Agent Interaction
```python
# Apply motor command to world
world.applyMotorCommand(motor_cmd, current_time)

# Get sensory perception
percept = world.getSensoryPercept()

# Get agent body state
body = world.getAgentBody()
```

#### World Objects Management
```python
# Add objects to world
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))

# Remove objects
world.removeObject(x, y)

# Check valid positions
world.isValidPosition(x, y)
```

#### World Properties
```python
# Get world dimensions
world.getWidth()
world.getHeight()

# Get simulation state
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

### Enumerations

#### Neuron Types
```python
pynlm.NeuronType.Excitatory    # Excitatory neurons
pynlm.NeuronType.Inhibitory    # Inhibitory neurons
pynlm.NeuronType.Sensory       # Sensory neurons
pynlm.NeuronType.Motor         # Motor neurons
pynlm.NeuronType.Modulatory    # Neuromodulatory neurons
pynlm.NeuronType.Internal      # Internal neurons
```

#### Developmental Stages
```python
pynlm.DevelopmentalStage.Initial        # Initial development
pynlm.DevelopmentalStage.CriticalPeriod # Critical learning period
pynlm.DevelopmentalStage.Maturation     # Maturation phase
pynlm.DevelopmentalStage.Adult         # Adult stage
pynlm.DevelopmentalStage.Aging         # Aging phase
```

#### Action Types
```python
pynlm.ActionType.MoveForward  # Move forward
pynlm.ActionType.TurnLeft     # Turn left
pynlm.ActionType.TurnRight    # Turn right
pynlm.ActionType.Interact     # Interact with object
pynlm.ActionType.Eat         # Consume resource
pynlm.ActionType.Wait        # Do nothing
```

#### World Object Types
```python
pynlm.WorldObjectType.Empty     # Empty space
pynlm.WorldObjectType.Resource  # Resource object
pynlm.WorldObjectType.Hazard    # Hazard object
pynlm.WorldObjectType.Wall      # Wall object
pynlm.WorldObjectType.Marker    # Marker object
```

## Configuration Presets

### Basic Simulation
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.synapse_density", 0.1)
config.set("simulation.timestep", 0.001)
config.set("plasticity.stdp.enable", True)
```

### Learning-Focused
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.15)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("plasticity.stdp.learning_rate", 0.005)
config.set("plasticity.structural.enable", True)
```

### Memory Research
```python
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("memory.working.capacity", 500)
config.set("memory.episodic.enabled", True)
```

## Example Usage Patterns

### Pattern 1: Basic Brain Simulation
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

print(f"Final spike count: {brain.getTotalSpikeCount()}")
```

### Pattern 2: Complete Agent Environment
```python
import pynlm

def run_agent_simulation(steps=500):
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: curiosity={agent.getCuriosityLevel():.3f}")
    
    return brain, agent, world

brain, agent, world = run_agent_simulation(500)
```

### Pattern 3: Enhanced Configuration
```python
import pynlm

# Create enhanced configuration for learning
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 800)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)

brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with all subsystems
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print(f"Enhanced brain created with {brain.getTotalNeuronCount()} neurons")
```

## Known Limitations

### Current Status (Phase 6: 35.8% Integration)

The following systems are NOT integrated with the main brain loop:

1. **Memory Systems:** `Brain::getWorkingMemory()`, `Brain::getEpisodicMemory()` return nullptr
2. **Prediction System:** `Brain::getPredictionSystem()` returns nullptr
3. **Checkpoint System:** `Brain::save()` and `Brain::load()` are stubs
4. **Cognition Systems:** NeuralPlanner, ConceptFormation, etc. never instantiated
5. **Development:** Only affects structural plasticity rates
6. **Neuromodulation:** Partial (dopamine only scales STDP)

### What Works (Working Systems)

✅ **Neural Core:** LIF neurons, spikes, synaptic transmission, STDP, Hebbian plasticity
✅ **Agent System:** Sensory processing, motor decoding, reward modulation, curiosity, novelty detection  
✅ **World Simulation:** 2D environment, vision, touch, internal sensing
✅ **Basic Integration:** Brain-Agent-world loop works for Phase 3/4

### Recommendation

Focus on Phase 4 cognitive experiments that test the disconnected systems, or build on the working neural core foundation for your research needs.

## References

- [HOW_TO_USE.md](HOW_TO_USE.md) - Detailed usage guide
- [easy_usage.md](easy_usage.md) - Quick start guide
- [PHASE6_FINAL_AUDIT.md](docs/PHASE6_FINAL_AUDIT.md) - Current integration status
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture