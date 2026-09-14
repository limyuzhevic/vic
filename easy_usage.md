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

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

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

### Project 3: Complete Agent

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

---

## Quick Reference

# Mini Projects

### Project 1: Silent Brain (Just Neurons)

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

### Project 3: Complete Agent

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

### Project 4: Advanced Brain with Memory (NEW)

```python
import pynlm

# Create and initialize a sophisticated brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable all advanced subsystems
agent = pynlm.createAgentBrain(brain)

# Access and configure memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Configure prediction system
prediction_system = brain.getPredictionSystem()

# Configure development
brain.enableDevelopment(True)
development_system = brain.getDevelopmentSystem()

# Configure neuromodulation
brain.enableRewardModulation(True)
brain.enableCuriosity(True)
brain.enableNovelty(True)

# Run simulation with learning
for step in range(1000):
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent.initialize(world)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Memory capacity: {working_memory.getCapacity()}")
        print(f"  Working memory usage: {working_memory.getUsage()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

print("Advanced simulation complete!")
```

### Project 5: Sensory Processing Demo (NEW)

```python
import pynlm
import math

# Create brain with enhanced sensory processing
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create sophisticated sensory inputs
vision = pynlm.createVision(width=64, height=64, channels=3)

# Create a checkerboard pattern
for y in range(64):
    for x in range(64):
        if (x + y) % 2 == 0:
            vision.setData([1.0, 1.0, 1.0])
        else:
            vision.setData([0.0, 0.0, 0.0])

# Process through neural system
brain.receiveSensoryInput(vision)

# Apply image processing through brain
for step in range(10):
    brain.step(step)

# Apply Python-style image processing through brain
brain.receiveSensoryInput(vision)

# Enhanced processing through Python utilities
vision_normalized = vision.normalize()
vision_smoothed = vision.resizeBilinear(32, 32)

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=64, height=64, visionWidth=32, visionHeight=32)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print(f"Original resolution: {vision.getWidth()}x{vision.getHeight()}")
print(f"Normalized range: {vision_normalized.getWidth()}x{vision_normalized.getHeight()}")
print(f"Smoothed resolution: {vision_smoothed.getWidth()}x{vision_smoothed.getHeight()}")

print("Sensory processing demo complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

| Advanced Features | Code |
|------------------|------|
| Access working memory | `brain.getWorkingMemory()` |
| Access episodic memory | `brain.getEpisodicMemory()` |
| Access associative memory | `brain.getAssociativeMemory()` |
| Access prediction system | `brain.getPredictionSystem()` |
| Access development system | `brain.getDevelopmentSystem()` |
| Access neuromodulation | `brain.getDopamine()`, `brain.getCuriosity()`, etc. |
| Enhanced config methods | `config.get_int()`, `config.get_float()` |
| Vision processing | `vision.normalize()`, `vision.resizeBilinear()` |
| Audio effects | `audio.applyEffect("reverse")` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"Getting new errors with advanced features"**
- Check that you're using the updated pynlm bindings
- Some new methods may require recent builds
- Try simpler configurations first

### Project 4: Advanced Brain with Memory (NEW)

```python
import pynlm

# Create and initialize a sophisticated brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable all advanced subsystems
agent = pynlm.createAgentBrain(brain)

# Access and configure memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Configure prediction system
prediction_system = brain.getPredictionSystem()

# Configure development
brain.enableDevelopment(True)
development_system = brain.getDevelopmentSystem()

# Configure neuromodulation
brain.enableRewardModulation(True)
brain.enableCuriosity(True)
brain.enableNovelty(True)

# Run simulation with learning
for step in range(1000):
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    
    agent.initialize(world)
    
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Log progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Memory capacity: {working_memory.getCapacity()}")
        print(f"  Working memory usage: {working_memory.getUsage()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

print("Advanced simulation complete!")
```

### Project 5: Sensory Processing Demo (NEW)

```python
import pynlm
import math

# Create brain with enhanced sensory processing
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create sophisticated sensory inputs
vision = pynlm.createVision(64, 64, 3)

# Create a checkerboard pattern
for y in range(64):
    for x in range(64):
        if (x + y) % 2 == 0:
            vision.setData([1.0, 1.0, 1.0])
        else:
            vision.setData([0.0, 0.0, 0.0])

# Process through neural system
brain.receiveSensoryInput(vision)

# Apply image processing through brain
for step in range(10):
    brain.step(step)

# Apply Python-style image processing through brain
brain.receiveSensoryInput(vision)

# Enhanced processing through Python utilities
vision_normalized = vision.normalize()
vision_smoothed = vision.resizeBilinear(32, 32)

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=64, height=64, visionWidth=32, visionHeight=32)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print(f"Original resolution: {vision.getWidth()}x{vision.getHeight()}")
print(f"Normalized range: {vision_normalized.getWidth()}x{vision_normalized.getHeight()}")
print(f"Smoothed resolution: {vision_smoothed.getWidth()}x{vision_smoothed.getHeight()}")

print("Sensory processing demo complete!")
```
