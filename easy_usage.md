# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**NEW IN PHASE 6: Enhanced Python Bindings!**

This guide has been updated with significant improvements to the Python bindings:
- ✅ Better error handling and input validation
- ✅ More Pythonic convenience methods
- ✅ Improved documentation and examples
- ✅ Comprehensive test suite included

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

### NEW: Pattern 4: Quick Statistical Analysis

```python
# Get brain statistics as a dictionary:
stats = pynlm.analyzeBrainStats(brain)
print(f"Total neurons: {stats['total_neurons']}")
print(f"Average firing rate: {stats['avg_firing_rate']:.3f}")
print(f"E/I ratio: {stats['e_to_i_ratio']:.3f}")
```

### NEW: Pattern 5: Configuration Presets

```python
# Create configuration optimized for different scenarios:
development_config = pynlm.createConfigForDevelopment()
adult_config = pynlm.createConfigForAdult()
```

---

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

### NEW: Convenience Methods

#### `pynlm.setupSimpleExperiment(neuronCount, worldSize)`
Quick setup for simple experiments with predefined brain/world/agent configuration.

#### `pynlm.runAgentSimulation(brain, world, numSteps, enableLearning)`
Run a complete agent simulation with brain, world, and agent integration.

#### `pynlm.analyzeBrainStats(brain)`
Get brain statistics as a Python dictionary for easy analysis.

#### `pynlm.createConfigForDevelopment()`
Create configuration optimized for developmental learning.

#### `pynlm.createConfigForAdult()`
Create configuration optimized for adult brain.

#### `pynlm.isReadyForSimulation(brain, world)`
Check if brain and world are ready for simulation.

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

### NEW: Project 4: Advanced Agent with Learning

```python
import pynlm

# Setup with development configuration
brain, world, agent = pynlm.setupSimpleExperiment(1000, 25)

# Create development configuration
dev_config = pynlm.createConfigForDevelopment()
brain = pynlm.createBrain(dev_config)
brain.initialize()

# Initialize agent with world
agent.initialize(world)

# Run advanced simulation with learning
result = pynlm.runAgentSimulation(brain, world, 200, True)

# Analyze results
stats = pynlm.analyzeBrainStats(brain)
print(f"Simulation Results:")
print(f"  Total Neurons: {stats['total_neurons']}")
print(f"  Total Synapses: {stats['total_synapses']}")
print(f"  Average Firing Rate: {stats['avg_firing_rate']:.3f}")
print(f"  Curiosity Level: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty Level: {agent.getNoveltyLevel():.3f}")
print(f"  Development Stage: {brain.getDevelopmentalStage()}")

pynlm.printBrainInfo(brain)
```

---

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

| What you want | New Code |
|--------------|----------|
| Complete agent simulation | `pynlm.runAgentSimulation(brain, world, steps, learning)` |
| Quick experiment setup | `pynlm.setupSimpleExperiment(neurons, world_size)` |
| Get brain stats as dict | `pynlm.analyzeBrainStats(brain)` |
| Development config | `pynlm.createConfigForDevelopment()` |
| Adult config | `pynlm.createConfigForAdult()` |
| Check simulation readiness | `pynlm.isReadyForSimulation(brain, world)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that you have at least some neurons configured

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that the agent has motor neurons configured

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that you have sufficient neurons configured

**"Input validation errors"**
- Invalid file paths, dimensions, or values now provide clear error messages
- Use the new convenience methods for safer API usage

**"Memory management issues"**
- Use `pynlm.runAgentSimulation()` or `pynlm.setupSimpleExperiment()` for safer API
- The convenience methods handle brain lifecycle management automatically
- For manual management, use `agent.getBrainShared()` to get a shared_ptr to brain

---

## Next Steps

When you're comfortable:

1. **Run the tests**: Execute `python -m pytest tests/test_pynlm.py` to verify all functionality

2. **Read `HOW_TO_USE.md` for more details** about advanced usage and API reference

3. **Read `docs/ARCHITECTURE.md` to understand how it all works** at the architectural level

4. **Experiment with different configurations** using the new configuration presets:
   - Development: `pynlm.createConfigForDevelopment()`
   - Adult: `pynlm.createConfigForAdult()`

5. **Try the new convenience methods** for easier setup and common workflows:
   - Quick experiment setup: `pynlm.setupSimpleExperiment()`
   - Complete agent simulation: `pynlm.runAgentSimulation()`
   - Statistical analysis: `pynlm.analyzeBrainStats()`

6. **Use the new error handling and validation** for more robust code

**NEW: Test Suite Included**

A comprehensive test suite is included in `tests/test_pynlm.py` that covers:
- Unit tests for individual classes
- Integration tests for common workflows
- Error handling tests for invalid inputs
- Memory management validation
- Performance verification

Run the tests to ensure everything works correctly:
```bash
python -m pytest tests/test_pynlm.py -v
```

That's it! You're now ready to use NLM with the enhanced Python bindings.

**NEW: Enhanced Python Bindings Summary**

The NLM Python bindings have been significantly improved in Phase 6:

1. **Better Error Handling**: Comprehensive input validation and clear error messages
2. **Memory Safety**: Improved pointer management and shared_ptr support
3. **Convenience Methods**: High-level APIs for common patterns
4. **Configuration Presets**: Ready-to-use configurations for different scenarios
5. **Statistical Analysis**: Built-in functions for brain analysis
6. **Test Coverage**: Comprehensive test suite for all functionality
7. **Documentation**: Enhanced examples and improved documentation

The Python bindings now provide a more robust, user-friendly, and feature-rich interface to the NLM neural simulation framework.

