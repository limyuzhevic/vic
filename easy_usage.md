# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Updated: More Accurate and Comprehensive Examples

### Essential Components (Working)

```python
import pynlm

# 1. Create a default brain configuration
config = pynlm.createDefaultConfig()

# 2. Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain created: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
```

### Basic Simulation Loop

```python
import pynlm

def basic_simulation(num_steps=1000):
    """Run a simple brain simulation without world interaction."""
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    for step in range(num_steps):
        brain.step(step)
        
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
    return brain

# Run the simulation
brain = basic_simulation(500)
```

### Complete Agent Simulation

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
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
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
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_agent_simulation(1000)
print("Agent simulation complete!")
```

### Configuration and Customization

```python
import pynlm

# Create configuration with custom parameters
config = pynlm.createDefaultConfig()

# Modify configuration parameters
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

# Create brain with custom configuration
brain = pynlm.createBrain(config)
brain.initialize()

print("Custom brain configuration loaded")
```

### Error Handling and Validation

```python
import pynlm
import traceback

# Safe brain creation with error handling
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized successfully with {brain.getTotalNeuronCount()} neurons")
    
    # Run a few simulation steps
    for i in range(10):
        brain.step(i)
        
    print("Simulation steps completed successfully")
    
except Exception as e:
    print(f"Error in simulation: {e}")
    traceback.print_exc()

# Check brain statistics
if brain:
    print(f"Final statistics:")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
```

---

## Common Troubleshooting

### "My brain isn't doing anything"
- **Problem**: Brain appears inactive after initialization
- **Solution**: 
  1. Did you call `brain.initialize()`? Always call this before `brain.step()`
  2. Try increasing the number of simulation steps
  3. Check that sensory input is properly connected

### "The agent isn't moving"
- **Problem**: Agent output shows no movement
- **Solution**:
  1. Did you call `world.applyMotorCommand()`?
  2. Check that `world.update()` is being called
  3. Verify that motor neurons are receiving input

### "Everything is 0"
- **Problem**: All statistics show zero values
- **Solution**:
  1. Brains need time to "warm up" - try more steps
  2. Some neurons need input to fire - ensure sensory input is connected
  3. Check that plasticity systems are enabled

---

## Updated Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `brain = pynlm.createBrain(pynlm.createDefaultConfig())` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step_number)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Save brain state | `brain.save("checkpoint.bin")` |
| Load brain state | `brain.load("checkpoint.bin")` |

---

## Mini Projects

### Project 1: Silent Brain (Basic Test)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

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

### Project 3: Complete Learning Agent

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
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## New: Advanced Features

### Checkpointing and State Management

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation steps
for i in range(100):
    brain.step(i)

# Save brain state
success = brain.save("my_checkpoint.bin")
if success:
    print("Brain state saved successfully")
else:
    print("Failed to save brain state")

# Create new brain with same configuration
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()

# Load saved state
if brain2.load("my_checkpoint.bin"):
    print("Brain state loaded successfully")
    print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")
```

### Development Stages

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check developmental stage
stage = brain.getDevelopmentalStage()
print(f"Initial developmental stage: {stage}")

# Simulate development over time
for step in range(0, 3000, 100):
    brain.step(step)
    stage = brain.getDevelopmentalStage()
    
    if step % 1000 == 0:
        print(f"Step {step}: Developmental stage is {stage}")
```

### Neuromodulation

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create agent with neuromodulation
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable neuromodulation systems
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    # Track neuromodulation levels
    if step % 50 == 0:
        print(f"Step {step}:")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Reward: {agent.getNeuromodulationLevel():.3f}")
```

---

## Complete Python API Reference

### Core Classes

#### Brain Class
```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)           # Run one simulation step
brain.step(step_number, time)     # Run step with timestamp
brain.reset()                     # Reset brain state

# Input/Output
brain.receiveSensoryInput(input)  # Inject sensory data
action = brain.produceAction()    # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()      # Currently firing
brain.getAverageFiringRate()       # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state

# Regions
region_id = brain.addRegion("cortex")
region = brain.getRegion(region_id)
```

#### Config Class
```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class
```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableCuriosity(True/False)
agent.enableDevelopment(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class
```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
```

### Available Functions

#### Factory Functions
```python
# Configuration
config = pynlm.createDefaultConfig()

# Brain
brain = pynlm.createBrain(config)

# World
world = pynlm.createSimpleWorld()

# Agent
agent = pynlm.createAgentBrain(brain)
```

#### Constants
```python
# Neuron types
pynlm.NeuronType.Excitatory
pynlm.NeuronType.Inhibitory
pynlm.NeuronType.Sensory
pynlm.NeuronType.Motor
pynlm.NeuronType.Internal

# Action types
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.Interact
pynlm.ActionType.Eat

# World object types
pynlm.WorldObjectType.Empty
pynlm.WorldObjectType.Resource
pynlm.WorldObjectType.Hazard
pynlm.WorldObjectType.Wall

# Developmental stages
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

---

## New: Advanced Usage Patterns

### Pattern 1: Sequential Loading

```python
import pynlm

# Create initial brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some learning
for i in range(500):
    brain.step(i)

# Save learned state
success = brain.save("learned_brain.bin")
if success:
    print("Learning saved")

# Create new brain with same configuration
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()

# Load learned state
if brain2.load("learned_brain.bin"):
    print("Learned state loaded")
    print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")
```

### Pattern 2: Multi-Agent Learning

```python
import pynlm

def create_agent_brain():
    """Factory function to create a learning brain."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    return brain

# Create multiple agents
agents = [create_agent_brain() for _ in range(3)]

# Run parallel learning
for step in range(100):
    for i, brain in enumerate(agents):
        brain.step(step)
        
        # Each brain can learn different things
        if i == 0:
            brain.setLearnedPreference(-1.0)  # Avoidant
        elif i == 1:
            brain.setLearnedPreference(0.0)   # Neutral
        else:
            brain.setLearnedPreference(1.0)   # Approach-oriented

# Save each agent's learned behavior
for i, brain in enumerate(agents):
    brain.save(f"agent_{i}_learned.bin")
```

### Pattern 3: Developmental Learning

```python
import pynlm

# Create developmental brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Developmental learning simulation")
print("Initial stage:", brain.getDevelopmentalStage())

# Simulate development over time
for day in range(10):
    for step in range(100):  # 100 simulation steps per day
        brain.step(step)
    
    # End of day: check development
    stage = brain.getDevelopmentalStage()
    if day % 3 == 0:
        print(f"Day {day}: Stage is {stage}")
        
        # Can save developmental state
        brain.save(f"day_{day}_state.bin")

print(f"Final stage: {brain.getDevelopmentalStage()}")
```

---

## Next Steps

When you're comfortable:

1. **Read Phase 6 integration** - Explore the complete integration features
2. **Experiment with configurations** - Try different brain parameters
3. **Explore Python bindings** - Check the full Python API documentation
4. **Add your own components** - Extend the framework with custom neurons, synapses, or plasticity rules

This is the complete, accurate NLM usage guide. The documentation now properly reflects the actual implementation and provides comprehensive examples for both beginners and advanced users.