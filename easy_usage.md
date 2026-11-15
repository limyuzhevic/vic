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

## Advanced Convenience Tools

For more complex simulations and better error handling, try the **NLM Convenience Module**:

The convenience module provides higher-level APIs for common operations:

### 1. NLMBuilder - Fluent Configuration

Create complete NLM systems with all desired features:

```python
from nlm_convenience import NLMBuilder

# Create system with all features enabled
brain_system = (NLMBuilder()
    .set_brain_size(2000, 2)
    .set_plasticity(True, True, True, True)
    .set_neuromodulation(True, True, True, True)
    .set_developmental(True)
    .set_world(20, 20, 8, 8)
    .build())

brain = brain_system.brain
agent = brain_system.agent
world = brain_system.world
```

### 2. SimulationRunner - Advanced Tracking

Run simulations with comprehensive metrics and statistics:

```python
from nlm_convenience import SimulationRunner

runner = SimulationRunner()
metrics = runner.run_episode(
    brain, world, agent,
    max_steps=2000,
    enable_learning=True
)

print(f"Total spikes: {metrics['total_spikes']}")
print(f"Average firing rate: {metrics['average_firing_rate']:.2f} Hz")
print(f"Total reward: {metrics['total_reward']:.2f}")
```

### 3. BrainProfiler - Brain Analysis

Analyze brain performance and dynamics:

```python
from nlm_convenience import BrainProfiler

profile = BrainProfiler.analyze_spike_patterns(brain, window_size=100)
print(f"Average firing rate: {profile['average_firing_rate']:.2f} Hz")
print(f"E/I ratio: {profile['excitatory_inhibition_ratio']:.2f}")
print(f"Burst count: {profile['burst_count']}")
```

### 4. NLMTools - Utility Functions

Import convenience tools and utilities:

```python
from nlm_convenience import NLMTools

# Save brain state
NLMTools.export_brain_state(brain, "checkpoint.bin")

# Benchmark configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
metrics = NLMTools.benchmark_configuration(config, steps=1000)
```

### Complete Example

```python
"""Advanced NLM simulation using convenience tools"""

import pynlm
from nlm_convenience import NLMBuilder, SimulationRunner

# Create complete system
brain_system = (NLMBuilder()
    .set_brain_size(3000, 2)
    .set_plasticity(True, True, True, True)
    .set_neuromodulation(True, True, True, True)
    .set_developmental(True)
    .set_world(30, 30, 12, 12)
    .build())

brain = brain_system.brain
agent = brain_system.agent
world = brain_system.world

# Run simulation with tracking
runner = SimulationRunner()
metrics = runner.run_episode(
    brain, world, agent,
    max_steps=1000,
    enable_learning=True
)

print(f"Simulation completed: {metrics['total_spikes']} spikes,")
print(f"{metrics['average_firing_rate']:.2f} Hz average firing rate")

# Save results
NLMTools.export_brain_state(brain, "simulation_final.bin")
```

## Next Steps

When you're comfortable:

1. **Explore HOW_TO_USE.md** - Read detailed documentation and examples
2. **Read docs/ARCHITECTURE.md** - Understand system architecture
3. **Try the convenience module** - Use the convenience tools for advanced simulations
4. **Experiment with configurations** - Test different brain and world settings
5. **Study the examples** - Review the complete examples in nlm_convenience.py

The convenience module makes it easier to create sophisticated NLM simulations while maintaining full access to the underlying power of the system. Use it for production simulations, research experiments, or when you want cleaner, more maintainable code.

That's it! You're now ready to use NLM with all its basic and advanced features.
