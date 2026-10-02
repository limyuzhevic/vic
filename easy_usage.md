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

### Pattern 4: Advanced Operations

```python
# Batch simulation for efficiency
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run efficient batch simulation
stats = pynlm.run_batch_simulation(brain, world, agent, num_steps=1000, timestep=0.1)
spike_count, sim_time, curiosity, novelty = stats
print(f"Completed {sim_time}s simulation with {spike_count} spikes")
print(f"Final curiosity: {curiosity:.3f}, novelty: {novelty:.3f}")
```

### Pattern 5: Error Handling and Validation

```python
import pynlm

# Configuration validation before use
try:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    
    # This will raise an exception if validation fails
    pynlm.validate_configuration(config)
    print("Configuration is valid")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain initialized successfully")
    
except pynlm.RuntimeError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### Pattern 6: Complete Experiment Framework

```python
import pynlm

# Quick setup for experiments
def run_learning_experiment(num_steps=500):
    # Use the quick setup helper
    brain, world, agent = pynlm.quick_test_setup()
    
    # Run a complete learning episode
    results = pynlm.run_learning_episode(
        brain=brain,
        world=world, 
        agent=agent,
        max_steps=num_steps,
        timestep=0.1
    )
    
    steps_completed, total_reward, firing_rate, curiosity, dev_stage = results
    
    print(f"Learning episode completed:")
    print(f"  - {steps_completed} steps")
    print(f"  - Total reward: {total_reward:.2f}")
    print(f"  - Average firing rate: {firing_rate:.2f} Hz")
    print(f"  - Final curiosity: {curiosity:.3f}")
    print(f"  - Development stage: {dev_stage}")
    
    return results

# Run the experiment
experiment_results = run_learning_experiment(1000)
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

### New Advanced Functions

#### `pynlm.run_batch_simulation(brain, world, agent, num_steps, timestep, enable_learning=True)`
Efficient batch simulation that runs multiple steps and returns statistics.

#### `pynlm.validate_configuration(config)`
Validates configuration parameters and throws descriptive exceptions on errors.

#### `pynlm.run_experiment(brain, world, agent, num_steps, timestep)`
Runs a complete experiment and returns detailed statistics about the simulation.

#### `pynlm.quick_test_setup()`
Convenience function that creates a complete test setup with reasonable defaults.

#### `pynlm.run_learning_episode(brain, world, agent, max_steps, timestep)`
Runs a complete learning episode with automatic error handling and early stopping.

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
| Run batch sim | `pynlm.run_batch_simulation(...)` |
| Validate config | `pynlm.validate_configuration(config)` |
| Quick setup | `pynlm.quick_test_setup()` |

## Advanced Features Documentation

### Batch Simulation
`run_batch_simulation()` provides efficient batch processing for running multiple simulation steps. This is useful for:
- Running long simulations without manual intervention
- Collecting statistical data over time
- Testing agent behavior in stable environments

**Parameters:**
- `brain`: Brain instance
- `world`: World instance  
- `agent`: Agent brain interface
- `num_steps`: Number of steps to simulate
- `timestep`: Simulation time step
- `enable_learning`: Whether to enable learning features

**Returns:** Tuple of (spike_count, simulation_time, curiosity_level, novelty_level)

### Configuration Validation
`validate_configuration()` checks if a configuration is valid before use. This helps catch:
- Missing required parameters
- Invalid parameter values
- Type mismatches

**Example:**
```python
# This will throw an exception if validation fails
pynlm.validate_configuration(config)
```

### Experiment Framework
`run_experiment()` provides a complete framework for running experiments with automatic statistics collection. Useful for:
- Scientific experiments
- Performance benchmarking
- Research studies

### Quick Setup
`quick_test_setup()` creates a complete test environment with reasonable defaults. Perfect for:
- Getting started quickly
- Testing and development
- Educational purposes

### Learning Episode
`run_learning_episode()` runs a complete learning episode with automatic:
- Early stopping for safety
- Learning feature activation
- Result collection and reporting

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that brain is producing motor commands

**"Configuration validation failed"**
- Run `pynlm.validate_configuration(config)` to see specific errors
- Check documentation for required parameters

**"Batch simulation returned unexpected results"**
- Check that all input objects are not null
- Verify that timestep is positive
- Ensure brain, world, and agent are properly initialized

**"Learning episode terminated early"**
- Check agent health and position in world
- Verify environment has valid termination conditions
- Review world configuration for obstacles or hazards

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** - Technical reference and C++ integration guide
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into NLM architecture
3. **Experiment with different configurations!** - Try different parameters and settings
4. **Use advanced features like `run_batch_simulation()`** - Efficient batch processing for research
5. **Validate your configurations** - Catch errors early with `validate_configuration()`

That's it! You're now ready to use NLM's advanced features. The system combines simple ease-of-use with powerful capabilities for research and production use.

## Quick Start Summary

For beginners, start with:
```python
import pynlm

# Quick setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simple simulation
for i in range(100):
    brain.step(i)
    
print("Simulation complete!")
```

For advanced users, explore:
```python
import pynlm

# Full experiment with all features
brain, world, agent = pynlm.quick_test_setup()
results = pynlm.run_batch_simulation(brain, world, agent, 1000, 0.1)
pynlm.validate_configuration(brain.getConfig())
```
