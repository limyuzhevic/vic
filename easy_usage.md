# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**NEW in this version:** NLM now includes advanced features, comprehensive error handling, and an enhanced Python API with additional learning capabilities!

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

### NEW Example: Enhanced Brain with Advanced Features

```python
import pynlm

# Create a brain with advanced configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable advanced learning features
agent = pynlm.createAgentBrain(brain)
agent.enableAdvancedLearning(True)  # NEW: Enable advanced learning algorithms

print("Advanced brain created with learning capabilities!")
```

### Example 2: Brain in a Simple World (Enhanced)

```python
import pynlm

# Create everything with enhanced configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems (NEW)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableAdvancedLearning(True)

# Run for 50 steps with enhanced monitoring
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Enhanced simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
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

## NEW: Advanced Features Guide

### 1. Create Advanced Brain

```python
import pynlm

# Create brain with advanced configuration
brain = pynlm.createBrain(pynlm.createAdvancedConfig(
    neuronCount=2000,
    maxSteps=10000,
    enableLearning=True
))
brain.initialize()

# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
```

### 2. Create Learning Pipeline

```python
import pynlm

# Create a complete learning pipeline
pipeline = pynlm.createLearningPipeline(width=30, height=30)

# Run an episode
total_reward, steps, rewards = pipeline.runEpisode(1000)

print(f"Total reward: {total_reward}")
print(f"Steps completed: {steps}")

# Print statistics
pipeline.printStats()
```

### 3. Use Quick Test

```python
import pynlm

# Quick test of all functionality
result = pynlm.quickTest()

if result:
    print("All tests passed!")
```

### 4. Working with World Objects

```python
import pynlm

world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)

# Add objects
from pynlm import WorldObject, WorldObjectType

world.addObject(WorldObject(10, 10, WorldObjectType.Resource, value=1.0))
world.addObject(WorldObject(30, 30, WorldObjectType.Hazard, value=-1.0))

print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Max energy: {world.getMaxEnergy()}")
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

# NEW: Get advanced statistics
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Prediction error:", agent.getPredictionError())
print("Neuromodulation level:", agent.getNeuromodulationLevel())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enableAdvancedLearning(True)     # NEW: Enable advanced learning algorithms
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

## NEW: Advanced Examples

### Complete Agent with Learning

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createAdvancedConfig(1000, num_steps, True)
    
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
    
    # 6. Enable ALL subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enableAdvancedLearning(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
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

# Run the simulation
run_agent_simulation(1000)
```

### NEW: Batch Processing

```python
import pynlm

# Create multiple brains for batch processing
brains = []
for i in range(5):
    config = pynlm.createAdvancedConfig(500, 1000, True)
    config.set("brain.neuron_count", 500 + i * 100)
    brain = pynlm.createBrain(config)
    brain.initialize()
    brains.append(brain)

# Run experiments in parallel
results = []
for i, brain in enumerate(brains):
    total_spikes = brain.getTotalSpikeCount()
    firing_rate = brain.getAverageFiringRate()
    results.append({
        'id': i,
        'neurons': brain.getTotalNeuronCount(),
        'synapses': brain.getTotalSynapseCount(),
        'spikes': total_spikes,
        'firing_rate': firing_rate
    })
    print(f"Brain {i}: {total_spikes} spikes, {firing_rate:.2f} firing rate")

print("Batch processing complete!")
```

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

### NEW: Project 3: Complete Advanced Agent

```python
import pynlm

# Setup with advanced features
config = pynlm.createAdvancedConfig(800, 500, True)
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning and advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableAdvancedLearning(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Advanced agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create brain with config | `pynlm.createBrainWithAdvancedConfig("file.cfg")` |
| Start brain | `brain.initialize()` |
| Create brain and agent | `pynlm.createBrainAndAgent("file.cfg")` |
| Create learning pipeline | `pynlm.createLearningPipeline()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Get advanced stats | `agent.getCuriosityLevel()`, `agent.getNoveltyLevel()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try enabling advanced learning: `agent.enableAdvancedLearning(True)`

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Make sure `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Try using `agent.enableCuriosity(True)` to explore

**NEW: "Brain isn't learning"**
- Make sure `agent.enableRewardModulation(True)` is called
- Try `agent.enableAdvancedLearning(True)` for better learning
- Check that the agent is processing sensory input

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try using `pynlm.quickTest()` to verify everything works
5. Create your own learning pipelines with `pynlm.createLearningPipeline()`

That's it! You're now ready to use the enhanced NLM with all its advanced features, comprehensive error handling, and improved Python API.

