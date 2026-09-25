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

### Example 4: Using New Convenience Methods

```python
import pynlm

# Create brain with enhanced convenience methods
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Configure world using new convenience methods
world = pynlm.createSimpleWorld()
config.configureWorld(100, 100, 16, 16)  # New method!

# Add objects using enhanced world setup
world.reset()

# Get enhanced brain statistics
stats = brain.getConfig().getSystemStats()  # New method!
print("Brain stats:", stats)

# Use enhanced convenience methods
brain.getConfig().setFloat("learning_rate", 0.5)  # New method!
brain.getConfig().setInt("max_steps", 1000)     # New method!

for step in range(100):
    brain.step(step)

print("Enhanced convenience methods working!")
```

### Example 5: Advanced Features

```python
import pynlm

# Use factory functions for quick starts
brain = pynlm.quickStartExperiment()  # New helper function!
brain.initialize()

# Enable all features
agent = pynlm.createAgentBrain(brain)
agent.enableAllFeatures()  # New method!

# Create world with objects
world = pynlm.createStandardTestWorld()  # New world setup!
world.configure(50, 50, 8, 8)

agent.initialize(world)

# Run with enhanced world methods
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print("Advanced features working!")
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

| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## New Convenience Methods Reference

### Config Class

| New Method | Description |
|------------|-------------|
| `config.configureWorld(width, height, visionWidth, visionHeight)` | Configure world dimensions |
| `config.getSystemStats()` | Get configuration statistics |
| `config.getFloat(key, default)` | Get config value as float |
| `config.getInt(key, default)` | Get config value as int |
| `config.setFloat(key, value)` | Set config value as float |
| `config.setInt(key, value)` | Set config value as int |
| `config.setString(key, value)` | Set config value as string |

### Factory Functions

| New Method | Description |
|------------|-------------|
| `pynlm.quickStartExperiment()` | Quick start with default experiment setup |
| `pynlm.resetConfigToDefaults(config)` | Reset config to Phase 2 defaults |
| `pynlm.normalizeConfigForExperiment(config, experimentType)` | Normalize for specific experiment |
| `pynlm.createWorldWithObjects(width, height, objects)` | Create world with predefined objects |
| `pynlm.createStandardTestWorld()` | Create standard test environment |

### AgentBrain Class

| New Method | Description |
|------------|-------------|
| `agent.enableAllFeatures()` | Enable all neuromodulation features |
| `agent.disableAllFeatures()` | Disable all neuromodulation features |
| `agent.getNeuromodulationStatus()` | Get current neuromodulation status |

### World Class

| New Method | Description |
|------------|-------------|
| `world.simulateEpisode(duration, enableLogging)` | Run complete episode simulation |

### Utility Functions

| New Method | Description |
|------------|-------------|
| Any additional utility functions added |

---

## Troubleshooting

**"New convenience methods aren't working"**
- Make sure you're using the latest version of pynlm
- Check that the methods exist by looking at the New Convenience Methods Reference above
- Verify your Python environment has the bindings installed

**"Some examples show new features but others don't"**
- The examples with `*New*` methods in their names include the enhanced features
- Regular examples show the basic functionality

**"I'm getting errors with the factory functions"**
- Make sure you import pynlm correctly
- Factory functions provide defaults but you can still customize afterwards

---

## What's New in This Version

This version includes:

1. **Enhanced Python Bindings**: Added `getFloat()`, `getInt()`, `setFloat()`, `setInt()`, `setString()` convenience methods to the Config class
2. **Factory Functions**: Added `quickStartExperiment()`, `resetConfigToDefaults()`, `normalizeConfigForExperiment()` for common setup patterns
3. **World Helpers**: Added `createWorldWithObjects()`, `createStandardTestWorld()` for easier world setup
4. **AgentBrain Enhancements**: Added `enableAllFeatures()`, `disableAllFeatures()`, `getNeuromodulationStatus()` methods
5. **Utility Functions**: Added global convenience functions for common operations
6. **Improved Documentation**: Enhanced this guide with examples of all new features

These new features make it much easier to get started with NLM and implement common patterns without writing repetitive setup code.
