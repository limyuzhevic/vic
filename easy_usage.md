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

### Example 3: Advanced Brain Setup with Configuration

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Create and initialize brain (using enhanced factory)
brain = pynlm.create_brain_with_config_file("configs/default.cfg")
# Or use the enhanced factory that includes validation:
# brain = pynlm.create_brain_with_config_file("configs/my_config.cfg")

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
agent = pynlm.createAgentBrain(brain)

print(f"Created brain with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
```

### Example 4: Complete Learning Simulation

```python
import pynlm

# Setup with learning capabilities
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
agent = pynlm.createAgentBrain(brain)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

world.reset()
agent.initialize(world)

# Run learning simulation
for step in range(1000):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks and produces action
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    # Execute action and get reward
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from reward (reward from world internal signals)
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Reward: {reward:.3f}")

print("Learning simulation complete!")
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

## Advanced Usage Examples

### Example 5: Brain State Management and Persistence

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# Save brain state (checkpointing)
brain.save("checkpoint1.bin")
print("Saved brain state to checkpoint1.bin")

# Run some simulation steps
for step in range(500):
    brain.step(step)

# Load brain state (continue from checkpoint)
brain.load("checkpoint1.bin")
print("Loaded brain state from checkpoint1.bin")

# Verify brain is functional
print(f"Brain has {brain.getTotalSpikeCount()} spikes")
print(f"Brain has {brain.getFiringNeuronCount()} firing neurons")
```

### Example 6: Benchmarking and Performance Testing

```python
import pynlm
import time

# Create a larger brain for meaningful benchmarks
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2500)
brain = pynlm.createBrain(config)
brain.initialize()

# Run benchmark (returns ms_per_step, steps, total_ms)
results = pynlm.benchmark_brain(brain, 1000, 100)
ms_per_step, steps, total_ms = results

print(f"Benchmark completed:")
print(f"  Steps: {steps}")
print(f"  Total time: {total_ms} ms")
print(f"  Average per step: {ms_per_step:.3f} ms")
print(f"  Performance: {1000/ms_per_step:.1f} steps/sec")
```

### Example 7: Configuration Validation and Help

```python
import pynlm

# Validate a configuration file
if pynlm.validate_config_file("configs/default.cfg"):
    print("Configuration file is valid")
else:
    print("Configuration file has errors")

# Get available configuration keys
default_keys = pynlm.list_default_config_keys()
print(f"Available configuration keys: {len(default_keys)} keys")
print("Sample keys:", default_keys[:5])

# Get configuration schema (for documentation/help)
schema = pynlm.get_config_schema()
print("Configuration schema includes:")
for key in schema:
    print(f"  - {key}")
```

### Example 8: Error Handling and Robust Simulation

```python
import pynlm

# Robust simulation with error handling
try:
    # Create brain with error handling
    brain = pynlm.create_brain_with_file("configs/default.cfg")
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
    agent = pynlm.createAgentBrain(brain)
    
    world.reset()
    agent.initialize(world)
    
    # Run simulation with error handling
    for step in range(1000):
        try:
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        except RuntimeError as e:
            print(f"Error at step {step}: {e}")
            # Continue with next step instead of crashing
            continue
        except Exception as e:
            print(f"Unexpected error at step {step}: {e}")
            break
            
    print("Simulation completed successfully!")
    
except Exception as e:
    print(f"Failed to run simulation: {e}")
```

### Example 9: Comparative Analysis

```python
import pynlm

# Create brains with different configurations for comparison
print("Creating brains with different configurations...")

# Brain A: Standard configuration
config_a = pynlm.createDefaultConfig()
brain_a = pynlm.createBrain(config_a)
brain_a.initialize()

# Brain B: Larger, with learning enabled
config_b = pynlm.createDefaultConfig()
config_b.set("brain.neuron_count", 5000)
config_b.set("plasticity.stdp.enable", True)
config_b.set("plasticity.hebbian.enable", True)

# Use enhanced factory for brain B with validation
brain_b = pynlm.create_brain_with_config(config_b)

# Compare brain properties
print(f"\nBrain A (Standard):")
print(f"  Neurons: {brain_a.getTotalNeuronCount()}")
print(f"  Synapses: {brain_a.getTotalSynapseCount()}")
print(f"  Firing rate: {brain_a.getAverageFiringRate():.3f}")

print(f"\nBrain B (Enhanced with learning):")
print(f"  Neurons: {brain_b.getTotalNeuronCount()}")
print(f"  Synapses: {brain_b.getTotalSynapseCount()}")
print(f"  Firing rate: {brain_b.getAverageFiringRate():.3f}")
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
