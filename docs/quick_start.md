# Quick Start Guide

Get up and running with NLM in 15 minutes! This guide is designed for absolute beginners with no prior experience in neural networks or simulation frameworks.

## Prerequisites

- Python 3.8+ installed
- Basic programming knowledge (variables, loops, functions)
- No prior neural network experience required!

## What You'll Learn

By the end of this guide, you'll be able to:

✅ Create a virtual brain that thinks and learns
✅ Run simulations with a simple environment
✅ Analyze brain activity and performance
✅ Enable learning features like curiosity and reward modulation

## Installation

### Install the Python Bindings

If you haven't installed NLM yet, follow these simple steps:

```bash
# Install the package
pip install pynlm

# Or install in development mode (recommended for learning)
pip install -e .
```

### Verify Installation

```python
import pynlm

print("NLM Version:", pynlm.__version__)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
print("Brain created successfully!")
print(f"Your brain has {brain.getTotalNeuronCount()} neurons")
```

## The 3 Core Concepts

Before we start, let's understand the three main things you'll work with:

### 1. Brain 🧠
- A virtual brain with neurons that can fire and learn
- Contains hundreds to millions of interconnected neurons
- Can process information and make decisions

### 2. World 🌍
- A simulation environment where the brain operates
- Provides sensory input (what the brain can see, hear, feel)
- Provides rewards and feedback for learning

### 3. Agent 🤖
- The bridge between the brain and the world
- Takes sensory input from the world
- Sends commands back to the world
- Applies learning signals (rewards, curiosity)

## Your First Brain 🚀

Let's create your very first brain!

```python
import pynlm

print("=== Creating Your First Brain ===")

# Step 1: Create a configuration (settings for your brain)
config = pynlm.createDefaultConfig()

# Step 2: Create the brain with those settings
brain = pynlm.createBrain(config)

# Step 3: Initialize the brain (start thinking)
brain.initialize()

# Step 4: Make the brain think for a few moments
print("\nMaking your brain think...")
for step in range(10):
    brain.step(step)
    if step % 3 == 0:  # Print every 3 steps
        print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"\n✅ Success! Your brain thought for 10 steps")
print(f"📊 Total neurons: {brain.getTotalNeuronCount()}")
print(f"⚡ Total spikes: {brain.getTotalSpikeCount()}")
print(f"📈 Average firing rate: {brain.getAverageFiringRate():.2f}")
```

## Running a Complete Simulation 🎮

Now let's create a complete simulation with a brain, world, and agent!

```python
import pynlm

print("\n=== Complete Simulation Example ===")

# 1. Create the brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create the world (your brain's environment)
world = pynlm.createSimpleWorld()
world.configure(
    width=20,           # World is 20x20 units
    height=20,
    visionWidth=8,      # Agent sees 8x8 area at a time
    visionHeight=8
)
world.reset()

# 3. Create the agent (bridge between brain and world)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Enable learning features (optional but recommended)
agent.enableRewardModulation(True)  # Learn from rewards
agent.enableCuriosity(True)         # Explore new things

# 5. Run the simulation!
print("\nRunning simulation...")
for step in range(50):
    # Update the world
    world.update(0.1)  # 0.1 seconds per step
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the brain what it sees
    agent.processSensoryInput(percept)
    
    # Let the brain think
    brain.step(step)
    
    # Get the brain's decision
    action = agent.decodeMotorCommand()
    
    # Do something in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Show progress
    if step % 10 == 0:
        print(f"  Step {step}:")
        print(f"    🧠 Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"    🎲 Curiosity: {agent.getCuriosityLevel():.2f}")
        print(f"    📍 Agent position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")

print(f"\n✅ Simulation complete!")
print(f"🎯 Final curiosity level: {agent.getCuriosityLevel():.2f}")
print(f"📊 Average firing rate: {brain.getAverageFiringRate():.2f}")
```

## Mini Project: Silent Brain 🧘

Let's create a simpler example without a world, just to understand brain operation:

```python
import pynlm

print("\n=== Mini Project: Silent Brain ===")

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print("Your silent brain is thinking...")
for i in range(5):
    brain.step(i)
    print(f"  Step {i}: Brain is active with {brain.getFiringNeuronCount()} firing neurons")

print("\n🎯 Silent brain test complete!")
print(f"📈 Your brain generated {brain.getTotalSpikeCount()} electrical signals!")
```

## Mini Project: Brain Watching 🌍

Let's watch a world with a brain but no agent:

```python
import pynlm

print("\n=== Mini Project: Brain Watching World ===")

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

# Just watch, don't act
print("Watching world for 10 steps...")
for i in range(10):
    world.update(0.1)
    # Pass world percept directly to brain (bypassing agent)
    percept = world.getSensoryPercept()
    brain.receiveSensoryInput(percept)  # Direct sensory input
    brain.step(i)
    
    if i % 3 == 0:
        print(f"  Step {i}: Watching... {brain.getFiringNeuronCount()} neurons firing")

print(f"\n👁️ Watched world for 10 steps")
print(f"🧠 Firing rate: {brain.getAverageFiringRate():.2f}")
```

## Mini Project: Complete Agent 🎯

Now let's create a learning agent that explores and improves:

```python
import pynlm

print("\n=== Mini Project: Complete Learning Agent ===")

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)   # Learn from rewards
agent.enableCuriosity(True)          # Explore new things
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)         # Brain matures over time

# Run the learning agent
print("Running learning agent for 30 steps...")
for step in range(30):
    world.update(0.1)
    
    # Agent sees the world
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Agent decides what to do
    action = agent.decodeMotorCommand()
    
    # Agent does the action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Brain learns from the result
    agent.applyRewardModulation(1.0, 0.0)  # Simple reward
    
    # Show learning progress
    if step % 5 == 0:
        print(f"  Step {step}:")
        print(f"    🎯 Agent moved to ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        print(f"    🧠 {brain.getFiringNeuronCount()} neurons firing")
        print(f"    🎲 Curiosity: {agent.getCuriosityLevel():.2f}")

print(f"\n🎯 Agent learning complete!")
print(f"📈 Final curiosity: {agent.getCuriosityLevel():.2f}")
print(f"⚡ Total spikes generated: {brain.getTotalSpikeCount()}")
```

## Get Brain Statistics 📊

At any point, you can get detailed information about your brain:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Collect statistics
print("=== Brain Statistics ===")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Currently firing: {brain.getFiringNeuronCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Get comprehensive stats
stats = pynlm.getBrainStats(brain)
print(f"\n=== Advanced Stats ===")
for key, value in stats.items():
    if isinstance(value, (int, float)):
        print(f"{key}: {value:.3f}")
    else:
        print(f"{key}: {value}")
```

## Quick Reference Cards 🃏

### Essential Functions

| What you want to do | Code | Description |
|-------------------|------|-------------|
| Create brain | `pynlm.createBrain(pynlm.createDefaultConfig())` | Make a new brain |
| Start brain | `brain.initialize()` | Wake up the brain |
| Make brain think | `brain.step(step)` | One moment of thinking |
| Create world | `pynlm.createSimpleWorld()` | Make a simulation environment |
| Create agent | `pynlm.createAgentBrain(brain)` | Bridge brain to world |
| See world | `world.getSensoryPercept()` | Get sensory input |
| Get brain stats | `brain.getFiringNeuronCount()` | Check brain activity |
| Make action | `agent.decodeMotorCommand()` | Get brain decision |

### Common Problems & Solutions 🔧

**Problem:** "My brain isn't doing anything"
```python
# Solution: Make sure you call initialize()
brain.initialize()
```

**Problem:** "The agent isn't moving"
```python
# Solution: Make sure world is updating
world.update(0.1)
agent.processSensoryInput(world.getSensoryPercept())
```

**Problem:** "Everything is 0"
```python
# Solution: Brains need time to "warm up"
for step in range(100):  # More steps
    brain.step(step)
```

## Your First Complete Project ✅

Here's your first complete NLM project that you can save and reuse:

```python
"""
NLM First Project: Simple Agent Learning

This is your first complete NLM project!
Save this as 'first_project.py' and run it.
"""

import pynlm

def run_simple_agent(num_steps=100):
    """Run a simple agent simulation."""
    print(f"Starting simple agent simulation for {num_steps} steps...")
    
    # 1. Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # 3. Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # 5. Run simulation
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        agent.applyRewardModulation(1.0, 0.0)  # Simple reward
        
        if step % 20 == 0:
            print(f"Step {step}: Agent at ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
    
    # 6. Show results
    print(f"\nSimulation complete!")
    print(f"Total reward: {agent.getRewardValue()}")
    print(f"Final curiosity: {agent.getCuriosityLevel():.2f}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    return brain, world, agent

if __name__ == "__main__":
    brain, world, agent = run_simple_agent(100)
    print("\n🎉 Your first NLM project is complete!")
```

## Next Steps 📚

Now that you're up and running:

1. **Try the other mini projects** to understand different components
2. **Read the Basic Concepts** guide to understand what's happening under the hood
3. **Explore the Python API Reference** for all available functions
4. **Try building your own agent** with custom behaviors
5. **Check out the advanced examples** for more complex scenarios

You've successfully completed the Quick Start Guide! You're now ready to:

- ✅ Create and run NLM simulations
- ✅ Understand the basic components (brain, world, agent)
- ✅ Enable learning and observation
- ✅ Get useful statistics and insights

Happy brain-building! 🧠✨
