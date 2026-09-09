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

### NEW: Enhanced Example with Configuration

```python
import pynlm

# Step 1: Create brain with Pythonic configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Pythonic way to set values
config.set("brain.connection_probability", 0.1)

# Apply preset configurations (NEW FEATURE)
pynlm.apply_default_brain_settings(config)

brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 3: Run complete simulation using utility function (NEW FEATURE)
# This is the high-level way to run simulations
pynlm.run_simulation(brain, world, agent, 500)

print("✓ Enhanced simulation completed!")
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

### Pattern 4: Pythonic Configuration (NEW)

```python
# NEW: Use Pythonic configuration interface
config = pynlm.createDefaultConfig()

# Check if a key exists
if config.has("brain.neuron_count"):
    print(f"Current neuron count: {config.getInt('brain.neuron_count')}")

# Set values with type safety
config.set("brain.neuron_count", 3000)
config.set("performance.enable_simd", True)

# Get values with helpful error messages
try:
    count = config.getInt("brain.neuron_count")
    print(f"Neurons: {count}")
except pynlm.ConfigKeyError as e:
    print(f"Configuration error: {e}")

# Get value or default
max_neurons = config.getOr("brain.neuron_count", 1000)
print(f"Max neurons: {max_neurons}")
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

### NEW: Configuration Helpers

#### `pynlm.apply_default_brain_settings(config)`
Apply standard brain configuration settings.

#### `pynlm.apply_performance_settings(config)`
Enable performance optimizations like SIMD.

#### `pynlm.apply_development_settings(config)`
Enable developmental features and critical periods.

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

### Project 3: Complete Agent (ENHANCED)

```python
import pynlm

# Setup with Pythonic configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)

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

# Run agent using high-level utility
brain = pynlm.run_simulation(brain, world, agent, 200)
    
print("Agent simulation complete!")
```

### Project 4: Configuration Examples (NEW)

```python
import pynlm

# Example 1: Create simple config
config = pynlm.create_simple_config(neuron_count=5000, connection_prob=0.05)
brain = pynlm.createBrain(config)

# Example 2: Create brain from file
brain = pynlm.create_brain_from_config_file("my_config.cfg")

# Example 3: Run with error handling
try:
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30)
    # ... rest of simulation
except pynlm.ConfigError as e:
    print(f"Configuration error: {e}")
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

| Configuration | Code |
|---------------|------|
| Pythonic config | `config.set("key", value)` |
| Get with error handling | `config.getInt("key")` |
| Apply presets | `pynlm.apply_default_brain_settings(config)` |
| High-level simulation | `pynlm.run_simulation(...)` |
| Create from file | `pynlm.create_brain_from_config_file("file.cfg")` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Configuration error"**
- Use try-catch with `pynlm.ConfigError` for better error messages
- Check that configuration keys exist with `config.has("key")`

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"Pythonic config not working"**
- Make sure you're using the right method names: `getInt()`, `getDouble()`, `getString()`, `getBool()`
- Check type compatibility: `config.getInt("key")` expects an integer value

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
3. **Experiment with different configurations!**
4. **Try the enhanced Pythonic interface features**
5. **Use high-level utility functions for complex simulations**
6. **Explore configuration presets and error handling**

---

## Quick Start Guide

### For Absolute Beginners:
```python
# One line to get started
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(10):
    brain.step(i)
print("Done!")
```

### For Intermediate Users:
```python
# Use Pythonic configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)

# Use presets
pynlm.apply_default_brain_settings(config)

# Run simulation
brain = pynlm.run_simulation(brain, world, agent, 500)
```

### For Advanced Users:
```python
# Use high-level utilities
brain = pynlm.create_brain_from_config_file("advanced_config.cfg")
brain = pynlm.run_simulation(brain, world, agent, 1000)

# Handle errors gracefully
with try-catch for configuration and runtime errors:
    # Your simulation code
```

That's it! You're now ready to use NLM with the enhanced Python bindings.

The new Pythonic interface makes it easier to:
- **Configure brains** with type-safe methods
- **Handle errors** with descriptive exception messages
- **Use presets** for common configurations
- **Run simulations** with high-level utilities
- **Extend functionality** with improved API discoverability

