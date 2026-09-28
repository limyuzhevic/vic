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

## Advanced API Usage

### Brain Configuration Advanced Options

```python
import pynlm

# Advanced brain configuration with custom parameters
config = pynlm.createDefaultConfig()
config.set("random_seed", 12345)
config.set("neuron_count", 1000)
config.set("region_count", 3)
config.set("connection_probability", 0.2)
config.set("stdp_ltp_weight", 0.05)
config.set("stdp_ltd_weight", 0.03)
config.set("stdp_tau", 30.0)
config.set("synaptogenesis_rate", 0.0001)
config.set("pruning_rate", 0.00001)
config.set("simulation_timestep", 0.001)

brain = pynlm.createBrain(config)
brain.initialize()
```

### Sensory Input Processing

```python
import pynlm
import numpy as np

# Create a more sophisticated sensory input
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)

# Advanced simulation with learning
for step in range(1000):
    # Update world dynamics
    world.update(0.1)
    
    # Get rich sensory percept
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    
    # Brain computes neural activity
    brain.step(step)
    
    # Generate motor command
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    action_result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation based on action outcome
    reward = action_result.reward if action_result else 0.0
    predicted_reward = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted_reward)
    
    # Log advanced metrics every 100 steps
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Action: {action}")
```

### Advanced Agent Behavior

```python
import pynlm
import math

def run_exploratory_behavior():
    """Example: Agent with curiosity-driven exploration"""
    # Setup brain with exploration bias
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.setAgentStart(15, 15)  # Center start
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track exploration metrics
    exploration_positions = [(15, 15)]
    visited_locations = set()
    novel_encounters = 0
    
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        
        # Use curiosity to decide exploration vs exploitation
        curiosity_level = agent.getCuriosityLevel()
        
        if curiosity_level > 0.7 and step % 10 == 0:
            # Explore: choose random movement
            print("  Curiosity-driven exploration!")
            action = pynlm.MotorCommand.Wait  # Stay still and observe
        else:
            # Exploit: normal agent behavior
            action = agent.decodeMotorCommand()
        
        # Execute action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update agent with experience
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Track exploration
        body = world.getAgentBody()
        if (body.x, body.y) not in visited_locations:
            visited_locations.add((body.x, body.y))
            novel_encounters += 1
        
        if step % 100 == 0:
            print(f"Exploration step {step}:")
            print(f"  Visited locations: {len(visited_locations)}")
            print(f"  Novel encounters: {novel_encounters}")
            print(f"  Brain neurons: {brain.getTotalNeuronCount()}")
            print(f"  Brain synapses: {brain.getTotalSynapseCount()}")
    
    return visited_locations, novel_encounters

# Run the exploration example
locations, encounters = run_exploratory_behavior()
print(f"Exploration complete! Visited {len(locations)} locations with {encounters} novel encounters.")
```

### Memory Management and Brain Persistence

```python
import pynlm
import os

def brain_lifespan_example():
    """Example: Save and load brain state"""
    print("Creating new brain...")
    brain1 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain1.initialize()
    
    # Run some learning
    for i in range(200):
        brain1.step(i)
    
    # Save brain state (checkpointing)
    save_file = "brain_checkpoint.pkl"
    if brain1.save(save_file):
        print(f"Brain saved to {save_file}")
    
    # Simulate time passing or shutdown
    print("Simulating interruption...")
    
    # Restore brain from checkpoint
    brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
    brain2.initialize()
    
    if brain2.load(save_file):
        print("Brain restored from checkpoint!")
        
        # Continue learning from where we left off
        for i in range(200, 400):
            brain2.step(i)
        
        # Verify continuity
        print(f"Original spikes: {brain1.getTotalSpikeCount()}")
        print(f"Restored spikes: {brain2.getTotalSpikeCount()}")
        
        # Clean up
        if os.path.exists(save_file):
            os.remove(save_file)
            print("Checkpoint cleaned up.")
    
    return brain2

# Run persistence example
persistent_brain = brain_lifespan_example()
```

### Performance Monitoring

```python
import pynlm
import time

def performance_benchmark():
    """Example: Performance monitoring and optimization"""
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 2000)
    config.set("region_count", 5)
    
    print("Starting performance benchmark...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Measure initialization time
    init_start = time.time()
    brain.initialize()
    init_time = time.time() - init_start
    print(f"Initialization time: {init_time:.3f} seconds")
    
    # Measure step performance
    step_times = []
    for step in range(100):
        step_start = time.time()
        brain.step(step)
        step_time = time.time() - step_start
        step_times.append(step_time)
    
    avg_step_time = sum(step_times) / len(step_times)
    min_step_time = min(step_times)
    max_step_time = max(step_times)
    
    print(f"Step performance (100 steps):")
    print(f"  Average: {avg_step_time:.6f} seconds ({1/avg_step_time:.0f} steps/sec)")
    print(f"  Min: {min_step_time:.6f} seconds")
    print(f"  Max: {max_step_time:.6f} seconds")
    print(f"  Neuron count: {brain.getTotalNeuronCount()}")
    print(f"  Synapse count: {brain.getTotalSynapseCount()}")
    
    return avg_step_time

# Run benchmark
performance_benchmark()
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
