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

### Example 2: Brain in a Simple World (With Context Manager)

```python
import pynlm

# Use context manager for automatic cleanup
with pynlm.SimpleWorld() as world:
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)

    # Run for 50 steps with enhanced error handling
    for step in range(50):
        try:
            world.update(0.1)  # Update world
            percept = world.getSensoryPercept()  # What does the agent see?
            agent.processSensoryInput(percept)  # Brain sees it
            brain.step(step)  # Brain thinks
            action = agent.decodeMotorCommand()  # Brain decides action
            world.applyMotorCommand(action, world.getSimulationTime())  # Do action
        except pynlm.NLMException as e:
            print(f"Simulation error at step {step}: {e}")
            break

print("Simulation finished!")
print("Firing neurons:", brain.firing_neurons)
print("Total spikes:", brain.total_spikes)
```

### Example 3: Batch Operations for Performance

```python
import pynlm
import numpy as np

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.Brain(config)
world = pynlm.SimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Collect sensory percepts for batch processing
percepts = []
for step in range(100):
    world.update(0.1)
    percepts.append(world.getSensoryPercept())

# Process all percepts at once using batch operations
print("Processing percepts with batch operations...")
actions = agent.processBatchPercepts(percepts)
print(f"Processed {len(actions)} actions in batch")

# Simulate all steps at once using batch processing
print("Running batch simulation...")
brain.batchStep(list(range(100)))

# Access performance metrics
metrics = brain.getPerformanceMetrics()
print(f"Performance metrics: {metrics}")
```

### Example 4: NumPy Integration

```python
import pynlm
import numpy as np

# Create vision data using NumPy
vision_data = np.random.randn(8, 8, 3).astype(np.float32)

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create vision sensory input
vision = pynlm.Vision(8, 8, 3)
vision.setData(vision_data)  # Direct NumPy array support

print(f"Vision shape: {vision.getWidth()}x{vision.getHeight()}x{vision.getChannels()}")

# Convert vision back to NumPy
numpy_array = vision.getDataAsNumpy()
print(f"Vision as NumPy array shape: {numpy_array.shape}")
print(f"Vision array dtype: {numpy_array.dtype}")

# NumPy mathematical operations
normalized = np.apply_along_axis(lambda x: (x - x.min()) / (x.max() - x.min() + 1e-8), axis=None, arr=numpy_array)
print(f"Normalized range: [{normalized.min():.3f}, {normalized.max():.3f}]")
```

### Example 5: Performance Monitoring and Progress Tracking

```python
import pynlm

# Create brain with configuration
config = pynlm.createDefaultConfig()
brain = pynlm.Brain(config)

# Use context manager for automatic cleanup
with pynlm.SimpleWorld() as world:
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)

    # Run simulation with progress tracking
    total_steps = 500
    print(f"Starting simulation with {total_steps} steps...")
    
    for step in range(total_steps):
        # Update world and get sensory input
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress every 100 steps
        if step % 100 == 0:
            progress = brain.getProgressInfo(total_steps)
            print(f"Step {step}: {progress['estimated_progress']:.1f}% complete")
            print(f"  Active neurons: {brain.active_neurons}")
            print(f"  Total spikes: {brain.total_spikes}")
            print(f"  Curiosity level: {agent.curiosity_level:.3f}")
            print(f"  Novelty level: {agent.novelty_level:.3f}")
            print()

print("Simulation complete!")
print("Final metrics:", brain.getPerformanceMetrics())
```

### Example 6: Visualization and Reporting

```python
import pynlm

# Create brain and run some simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.SimpleWorld()

# Add some objects to visualize
for i in range(5):
    obj = pynlm.WorldObject(i * 2.0, i * 2.0, pynlm.WorldObjectType.Resource, value=1.0)
    world.addObject(obj)

# Run simulation
brain.initialize(world)
for step in range(100):
    world.update(0.1)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)

# Collect data for visualization
brain_activity = []
for step in range(50):
    # In real implementation, you would collect actual brain activity data
    firing_rate = brain.getFiringNeuronCount() / max(brain.getTotalNeuronCount(), 1)
    brain_activity.append((step * 0.1, firing_rate))

sensory_data = []
for step in range(50):
    vision = world.getSensoryPercept().getVision()
    data = vision.getDataAsFlat()
    sensory_data.append(data)

connectivity = []
for i in range(brain.getTotalNeuronCount()):
    for j in range(i + 1, brain.getTotalNeuronCount()):
        if np.random.random() < 0.01:  # Random connections
            connectivity.append((i, j))

# Generate visualizations
brain.visualizeBrainActivity("brain_activity.csv", brain_activity)
brain.visualizeSensoryInput("sensory_input.csv", sensory_data)
brain.visualizeNeuralConnectivity("connectivity.csv", connectivity, brain.getTotalNeuronCount())

# Generate comprehensive report
sections = {
    "Simulation Overview": f"Total steps: 50, Total neurons: {brain.getTotalNeuronCount()}, Total synapses: {brain.getTotalSynapseCount()}",
    "Performance Metrics": f"Average firing rate: {brain.getAverageFiringRate():.3f}, E/I ratio: {brain.getExcitationInhibitionRatio():.3f}",
    "Development Stage": f"Current stage: {brain.developmental_stage}",
    "Neuromodulation": f"Dopamine: {agent.getNeuromodulationLevel():.3f}, Curiosity: {agent.getCuriosityLevel():.3f}, Novelty: {agent.getNoveltyLevel():.3f}"
}

brain.generateReport("simulation_report.md", "NLM Simulation Report", sections)

print("Visualizations and report generated!")
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

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
