# NLM - Basic Examples

**Date:** 2026-10-13
**Audience:** Beginners and users new to NLM
**Level:** Basic

---

## What You'll Learn

This section provides **basic, easy-to-follow examples** for getting started with NLM. These examples demonstrate:

1. **Creating and running simple simulations**
2. **Using the core NLM API**
3. **Working with brains, worlds, and agents**
4. **Basic analysis and debugging**
5. **Real-world use cases**

## Example 1: Silent Brain (Just Neurons)

### What It Does
This is the simplest possible example - creating a virtual brain and making it "think" for a few steps.

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it (this is REQUIRED!)
brain.initialize()

# Make it active (one step of thinking)
brain.step(0)

# See the results
print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("Total spikes so far:", brain.getTotalSpikeCount())
```

### Step-by-Step Explanation

#### Step 1: Import the Library
```python
import pynlm
```

#### Step 2: Create the Brain
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
```

**What happens:**
- `createDefaultConfig()`: Creates a brain with sensible defaults
- `createBrain(config)`: Creates a brain instance with those settings

#### Step 3: Initialize the Brain
```python
brain.initialize()
```

**Why it's important:** Every brain needs to be initialized before use. This:
- Allocates memory for neurons and synapses
- Sets up internal data structures
- Initializes neural states

#### Step 4: Run the Simulation
```python
brain.step(0)
```

**What happens:**
- Each neuron checks its membrane potential
- Spikes are generated when threshold is reached
- Signals are sent to connected neurons
- Synaptic weights may be updated (plasticity)

#### Step 5: Get Results
```python
print("Total neurons:", brain.getTotalNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
```

**Key metrics to know:**
- `getTotalNeuronCount()`: Total number of neurons in the brain
- `getTotalSynapseCount()`: Total number of connections
- `getTotalSpikeCount()`: Total spikes generated so far

## Example 2: Brain Watching a World

### What It Does
This example shows how a brain interacts with a simulated world - a complete agent loop.

```python
import pynlm

# Setup (this is where we create everything)
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)  # Move world forward in time
    agent.processSensoryInput(world.getSensoryPercept())  # Brain sees the world
    brain.step(i)  # Brain thinks about what it sees
    
    # Get what the brain decides to do
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())  # Do the action

print("Watched world for 30 steps")
print("Average firing rate:", brain.getAverageFiringRate())
print("Neurons currently firing:", brain.getFiringNeuronCount())
```

### Detailed Breakdown

#### 1. Creating and Configuring the World
```python
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
```

**Key parameters:**
- `width` and `height`: Size of the 2D world
- `visionWidth` and `visionHeight`: How much the agent can see
- `reset()`: Put the world in its initial state

#### 2. Creating the Agent
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

**What the agent does:**
- Connects brain to world
- Provides interface for sensory input and motor output
- Handles reward signals
- Manages curiosity and exploration

#### 3. The Core Simulation Loop
```python
for i in range(30):
    world.update(0.1)  # 1. Update world
    agent.processSensoryInput(world.getSensoryPercept())  # 2. Process what it sees
    brain.step(i)  # 3. Brain thinks
    action = agent.decodeMotorCommand()  # 4. Brain decides
    world.applyMotorCommand(action, world.getSimulationTime())  # 5. Do the action
```

**This is the complete agent loop:**
1. **World update**: Environment changes, time passes
2. **Sensory processing**: Agent gets information about environment
3. **Brain processing**: Neural network processes information
4. **Motor decoding**: Neural network produces action
5. **Action application**: Action affects the world

#### 4. Monitoring and Statistics
```python
print("Average firing rate:", brain.getAverageFiringRate())
print("Neurons currently firing:", brain.getFiringNeuronCount())
```

**Useful statistics:**
- `getAverageFiringRate()`: Average spikes per neuron per second
- `getFiringNeuronCount()`: Number of neurons currently active

## Example 3: Complete Agent with Learning

### What It Does
This example shows a complete learning agent with all the features enabled.

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
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # 7. Run simulation loop
    for step in range(num_steps):
        world.update(0.1)
        
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward from world feedback
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
    
    print("Agent simulation complete!")
    return agent, brain, world

# Run the simulation
agent, brain, world = run_agent_simulation(1000)
```

### Key Features Explained

#### 1. Enabling Learning Subsystems
```python
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

**What each system does:**
- **Reward modulation**: Adjusts learning based on rewards
- **Structural plasticity**: Creates/destroys synapses based on activity
- **Development**: Changes brain properties over time
- **Curiosity**: Drives exploration of novel stimuli

#### 2. The Complete Agent Loop
```python
for step in range(num_steps):
    world.update(0.1)
    
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    agent.updateDevelopment(0.1)
```

**This demonstrates:**
- **Sensory processing**: Brain receives information from world
- **Motor control**: Brain produces actions based on its state
- **Reward learning**: Brain learns from world feedback
- **Development**: Brain changes over time

## Example 4: Mini Projects

### Mini Project 1: Silent Brain

**Goal:** Create a simple brain and run it for a few steps.

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

**Learning:** The absolute minimum to get a brain working.

### Mini Project 2: Brain Watching a World

**Goal:** Make a brain interact with a world.

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

**Learning:** Complete sensory-motor loop.

### Mini Project 3: Complete Agent

**Goal:** Build a learning agent with all features.

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

**Learning:** Full agent behavior with learning.

## Example 5: Quick Reference Guide

### Cheat Sheet: Essential Commands

| What you want | Command |
|--------------|--------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

### Common Patterns

#### Pattern 1: Run a Simulation
```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        
        agent.processSensoryInput(percept)
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

run_simulation(brain, world, agent, 1000)
```

#### Pattern 2: Get Brain Stats
```python
# Add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

#### Pattern 3: Enable Brain Features
```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

## Example 6: Simple Analogy

### Real World vs. NLM Code

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns | `agent.applyRewardModulation(reward, predicted)` |

### Step-by-Step Agent Behavior

1. **Perception:** `percept = world.getSensoryPercept()`
   - Get what the agent sees

2. **Processing:** `agent.processSensoryInput(percept)`
   - Brain processes the sensory information

3. **Decision:** `action = agent.decodeMotorCommand()`
   - Brain decides what to do

4. **Action:** `world.applyMotorCommand(action, time)`
   - Agent does something in the world

5. **Learning:** `agent.applyRewardModulation(reward, predicted)`
   - Brain learns from the results

## Example 7: Getting Started Checklist

### For Complete Beginners

**✅ If you can do this, you're ready:**

```python
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run for a few steps
for i in range(10):
    brain.step(i)

print("Done!")
```

### For Intermediate Users

**✅ If you can do this, you're progressing:**

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Simulation complete!")
```

### For Advanced Users

**✅ If you can do this, you're an expert:**

```python
import pynlm
from pynlm.advanced_api import (
    AdvancedAPI,
    SimulationRunner,
    SimpleAgent,
    createBrainProfiler,
    createNetworkConnectivityAnalyzer
)

# Setup advanced simulation
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create analysis tools
brain_profiler = createBrainProfiler(brain)
network_analyzer = createNetworkConnectivityAnalyzer(brain)

# Run simulation with advanced features
sim_runner = SimulationRunner(num_steps=1000, dt=0.1)
agent = SimpleAgent(brain, pynlm.createSimpleWorld(), "agent_1")
sim_runner.addAgent(agent)

brain_profiler.start()
sim_runner.start()

for step in range(1000):
    sim_runner.step()

brain_profiler.stop()
sim_runner.stop()

# Get comprehensive analysis
print(brain_profiler.getProfileSummary())
print(network_analyzer.getConnectivitySummary())
```

## Example 8: Troubleshooting Common Issues

### Issue 1: "My brain isn't doing anything"

**Problem:** Brain produces no activity

**Solution:**
```python
# Did you call initialize?
brain.initialize()  # ✅ Always call this!

# Try more steps
for step in range(1000):
    brain.step(step)  # ✅ Need more steps

# Check configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # ✅ Ensure reasonable size
```

### Issue 2: "The agent isn't moving"

**Problem:** Agent produces no actions

**Solution:**
```python
# Did you call the core loop?
for step in range(100):
    world.update(0.1)  # ✅ World must update
    agent.processSensoryInput(world.getSensoryPercept())  # ✅ Process input
    brain.step(step)  # ✅ Brain must think
    action = agent.decodeMotorCommand()  # ✅ Get action
    world.applyMotorCommand(action, world.getSimulationTime())  # ✅ Apply action
```

### Issue 3: "Everything is 0"

**Problem:** All metrics are zero

**Solution:**
```python
# Brains need time to "warm up"
for step in range(1000):
    brain.step(step)  # ✅ More steps needed

# Some neurons need input to fire
agent.processSensoryInput(percept)  # ✅ Provide sensory input
```

## Example 9: What's Next?

### After You've Learned the Basics

1. **Read `HOW_TO_USE.md`** - More detailed instructions
2. **Read `docs/ARCHITECTURE.md`** - Understand how it all works
3. **Experiment with configurations** - Try different settings
4. **Look at examples in `examples_comprehensive.py`** - See advanced usage

### When You're Ready for Advanced Features

1. **Explore `ADVANCED_API_DOCUMENTATION.md`** - Complete API documentation
2. **Try `examples_comprehensive.py`** - 9 comprehensive examples
3. **Use `examples_advanced.md`** - Advanced usage patterns
4. **Check `examples_analysis.md`** - Analysis and debugging tools
5. **Use `examples_integration.md`** - Integration examples
6. **Try `examples_profiling.md`** - Performance profiling

## Quick Reference: Common Commands

### Essential Commands

```python
# Import
import pynlm

# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Initialize
brain.initialize()
agent.initialize(world)

# Core loop
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Key Statistics

```python
# Brain statistics
brain.getTotalNeuronCount()      # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()     # Currently firing
brain.getAverageFiringRate()     # Average firing rate
brain.getTotalSpikeCount()       # Total spikes
brain.getExcitationInhibitionRatio()  # E/I balance

# Agent statistics
agent.getCuriosityLevel()        # Curiosity drive
agent.getNoveltyLevel()          # Novelty detection
agent.getPredictionError()       # Prediction accuracy
agent.getNeuromodulationLevel()  # Neuromodulation

# World statistics
world.getWidth()                 # World width
world.getHeight()                # World height
world.getSimulationTime()        # Current simulation time
```

### Debug Commands

```python
# Check if components are working
assert brain is not None
assert world is not None
assert agent is not None

# Check initialization
assert brain.getTotalNeuronCount() > 0
assert brain.getTotalSynapseCount() > 0

# Check activity
assert brain.getFiringNeuronCount() >= 0
assert brain.getTotalSpikeCount() >= 0
```

## Summary

This guide provides **basic examples** for getting started with NLM:

### ✅ What You'll Learn
1. **Creating simple brains** - The absolute minimum to get started
2. **Complete agent loops** - How brains interact with worlds
3. **Real-world analogies** - Easy ways to understand the concepts
4. **Common patterns** - Reusable code templates
5. **Troubleshooting** - Solutions to common problems
6. **Progressions** - Clear learning path from beginner to expert

### ✅ Quick Start

**For beginners:**
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Done!")
```

**For intermediate users:**
```python
# See Example 2 or 3 for complete agent simulation
```

**For advanced users:**
```python
# See Example 8 for advanced features
```

### ✅ Resources

- **Main documentation**: `HOW_TO_USE.md`, `easy_usage.md`
- **Architecture**: `docs/ARCHITECTURE.md`
- **Advanced API**: `ADVANCED_API_DOCUMENTATION.md`
- **Examples**: `examples_comprehensive.py`
- **Code**: `src/` directory for implementation details

You now have everything you need to start using NLM! 🎉

---

**Last Updated:** 2026-10-13
**Status:** Production Ready
**Next:** Explore `HOW_TO_USE.md` for more details or `ADVANCED_API_DOCUMENTATION.md` for advanced features