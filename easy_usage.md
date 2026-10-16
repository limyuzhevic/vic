# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

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

### Enhanced Quick Start

Here's an enhanced version of the basic examples with more features:

```python
import pynlm
import numpy as np

print("=" * 60)
print("NLM Enhanced Quick Start")
print("=" * 60)

# Create an advanced brain with learning enabled
print("\n1. Creating advanced brain with learning systems...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure brain with typical values
brain.getConfig().set("brain.neuron_count", 1000)
brain.getConfig().set("brain.synapse_density", 0.1)
brain.getConfig().set("brain.development.enabled", True)
brain.getConfig().set("brain.plasticity.enabled", True)

print(f"   ✓ Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"   ✓ Development enabled: {brain.getConfig().get('brain.development.enabled')}")

# Create world and agent
print("\n2. Setting up world and agent...")
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enable_all_systems()

print(f"   ✓ Created world: {world.getWidth()}x{world.getHeight()} units")
print(f"   ✓ Agent with {agent.getSensoryInputSize()} sensory inputs")
print(f"   ✓ All learning systems enabled")

# Run simulation with tracking
print("\n3. Running simulation with tracking...")
simulation_stats = {
    'steps': 0,
    'firing_rates': [],
    'rewards': [],
    'curiosity_levels': [],
    'actions_taken': 0
}

for step in range(50):
    # Update world
    world.update(0.1)
    
    # Get and process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    if step % 10 == 0:
        agent.applyRewardModulation(0.5, 0.3)
    
    # Track statistics
    simulation_stats['steps'] += 1
    simulation_stats['firing_rates'].append(brain.getAverageFiringRate())
    simulation_stats['rewards'].append(0.5 if step % 10 == 0 else 0.0)
    simulation_stats['curiosity_levels'].append(agent.getCuriosityLevel())
    
    if action != pynlm.MotorCommand.Wait:
        simulation_stats['actions_taken'] += 1
    
    if step % 10 == 0:
        print(f"   Step {step}: "
              f"Firing={brain.getAverageFiringRate():.3f}Hz, "
              f"Curiosity={agent.getCuriosityLevel():.3f}, "
              f"Actions={simulation_stats['actions_taken']}")

# Print final statistics
print("\n4. Simulation Results:")
print(f"   Total simulation steps: {simulation_stats['steps']}")
print(f"   Average firing rate: {np.mean(simulation_stats['firing_rates']):.3f} Hz")
print(f"   Total actions taken: {simulation_stats['actions_taken']}")
print(f"   Average curiosity: {np.mean(simulation_stats['curiosity_levels']):.3f}")
print(f"   Total spikes: {brain.getTotalSpikeCount()}")
print(f"   Current developmental stage: {brain.getDevelopmentalStage()}")

print("\n5. Testing development system...")
# Test developmental stages
stages = [pynlm.DevelopmentalStage.Initial, pynlm.DevelopmentalStage.CriticalPeriod,
         pynlm.DevelopmentalStage.Maturation, pynlm.DevelopmentalStage.Adult]

for stage in stages:
    brain.setDevelopmentalStage(stage)
    print(f"   Set developmental stage: {stage}")

print("\n" + "=" * 60)
print("Enhanced quick start completed successfully!")
print("You can now use NLM with advanced features like:")
print("  • Learning systems (reward, curiosity, development)")
print("  • Memory systems (working, episodic)")
print("  • Prediction and planning modules")
print("  • Neuromodulation")
print("  • Performance monitoring")
print("  • Advanced configuration options")
print("=" * 60)
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

## Quick Start Summary

### 1. Install
```bash
pip install pynlm
```

### 2. Basic Usage
```python
import pynlm

# Create and initialize
brain = pynlm.createBrain()
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print("Done!")
```

### 3. Complete Agent
```python
import pynlm

# Setup
brain = pynlm.createBrain()
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### 4. Advanced Features
```python
# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

You're ready to explore the full power of NLM!
