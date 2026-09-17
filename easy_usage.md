# NLM Phase 2: Real Neural Computation - Easy Usage Guide

This document provides examples of using the NLM Phase 2 neural simulation system.

## What is NLM Phase 2?

NLM Phase 2 implements **real spiking neural computation** with:

- **LIF neurons** (Leaky Integrate-and-Fire)
- **Event-driven spike propagation** with delays
- **STDP and Hebbian plasticity**
- **Structural plasticity** (synaptogenesis/pruning)
- **Sensory processing** (vision, audio, internal signals)

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks with real LIF neurons
2. **World** - The environment the brain interacts with
3. **Agent** - The bridge connecting brain to world

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain (Phase 2)

```python
import pynlm

# Step 1: Make a brain (Phase 2 with real neurons)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on (initialize with LIF neuron dynamics)
brain.initialize()

# Step 3: Make it think for 100 steps (LIF dynamics + plasticity)
for i in range(100):
    brain.step(i)  # One step of LIF integration

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World (Phase 2)

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
    brain.step(step)  # Brain thinks with LIF dynamics
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

### Example 3: Phase 2 Learning (Hebbian + STDP)

```python
import pynlm

# Setup with plasticity enabled
config = pynlm.createDefaultConfig()
config.set("stdp_ltp_weight", 0.02f)  # LTP strength
config.set("stdp_ltd_weight", 0.018f) # LTD strength

brain = pynlm.createBrain(config)
brain.initialize()

# Enable plasticity (Phase 2 feature)
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)

# Run learning simulation
for step in range(200):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)  # LIF dynamics + STDP plasticity

print("Learning complete!")
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
```

## Simple Analogy

| Real World | NLM Phase 2 Code |
|------------|------------------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain with real neurons
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it (LIF neuron setup)
brain.initialize()

# Make it active (LIF integration)
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "real neurons!")
```

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain (LIF dynamics)
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks (LIF + plasticity)
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
```

### Pattern 3: Enable Phase 2 Features

```python
# These are all Phase 2 enabled features:
agent.enableRewardModulation(True)      # Learn from rewards (reward-modulated STDP)
agent.enableStructuralPlasticity(True)  # Grow new connections (synaptogenesis)
agent.enableDevelopment(True)           # Brain matures over time (maturation/pruning)
agent.enableCuriosity(True)             # Explore new things (novelty detection)
```

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with **real LIF neurons** and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain with **biologically realistic neuron parameters** (-70mV resting, -55mV threshold, etc.). Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time:
- **LIF dynamics**: Membrane potential integration with leak
- **Spike detection**: Check for threshold crossing
- **Plasticity**: STDP and Hebbian learning updates

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain, which affects **LIF neuron inputs**.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take based on **neural firing rates**.

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

### Project 3: Complete Phase 2 Agent

```python
import pynlm

# Setup with Phase 2 learning
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable ALL Phase 2 features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Phase 2 agent simulation complete!")
```

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain (real LIF neurons) | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think (LIF dynamics) | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps (LIF neurons need time to fire)

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called before action

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how Phase 2 systems work
3. Experiment with different configurations!

That's it! You're now ready to use NLM Phase 2 with **real spiking neural computation**.

## Note

This is Phase 2 of NLM: Real Neural Computation. Phase 6 (`nlm_phase6_demo`) includes additional systems:
- Working memory
- Episodic memory
- Neuromodulation
- Prediction
- Development
- Checkpointing
- Replay

For the complete integrated system, run `nlm_phase6_demo`.

## Resources

- **Documentation**: `docs/` directory contains detailed technical documentation
- **Phase 2 Demo**: `src/main.cpp` shows complete Phase 2 implementation
- **Phase 6 Demo**: `src/experiments/Phase6Demo.cpp` shows full integration
- **API Reference**: Generated from Doxygen comments in source code
- **Examples**: More examples in `examples/` directory