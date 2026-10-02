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

### Example 1 (Enhanced with Better API)

```python
import enhanced_pynlm as pynlm

# Step 1: Create enhanced brain with Pythonic API
brain_manager = pynlm.create_default_brain()

# Step 2: Initialize (Pythonic chainable API)
brain_manager.initialize()

# Step 3: Run multiple steps with statistics
result = brain_manager.run_batch(100)
print(f"Done! Your brain thought {brain_manager.brain.getTotalSpikeCount()} times")
print(f"Processed {result.processed_count}/{result.total_count} steps")
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

### Example 2 (Enhanced with Progress Tracking)

```python
import enhanced_pynlm as pynlm

# Create enhanced brain and world managers
brain_manager = pynlm.create_default_brain()
world_manager = pynlm.create_default_world(width=10, height=10, 
                                          vision_width=8, vision_height=8)

# Connect brain to world
brain_manager.initialize(num_steps=10)  # Warm up brain
world_manager.initialize(brain_manager.brain)

# Progress callback for tracking
def progress_callback(progress, processed, total, elapsed, message):
    if processed % 10 == 0:  # Update every 10 steps
        print(f"Progress: {progress:.1f}% ({processed}/{total}) - {message}")

callback = pynlm.ProgressCallback(
    update_interval=0.5,
    on_progress=progress_callback
)
brain_manager.add_progress_callback(callback)

# Run complete episode
result = world_manager.run_episode(
    num_steps=50,
    progress_callback=callback,
    timeout=10.0  # 10 second timeout
)

print(f"Episode completed: {result['steps_completed']} steps")
print(f"Success: {result['success']}")
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

### Project 1 (Enhanced with Statistics)

```python
import enhanced_pynlm as pynlm
import time

brain_manager = pynlm.create_default_brain()

# Initialize and run with timing
start_time = time.time()
brain_manager.initialize(num_steps=5)  # Warm up
run_start = time.time()

# Run simulation with statistics
for i in range(10):
    brain_manager.step(i)
    
    if i % 5 == 0:  # Print every 5 steps
        stats = brain_manager.get_statistics()
        print(f"Step {i}:")
        print(f"  Neurons: {stats['neurons']['total']}")
        print(f"  Firing: {stats['neurons']['firing']}")
        print(f"  Firing Rate: {stats['neurons']['average_firing_rate']:.2f}")

run_time = time.time() - run_start
print(f"\nSimulation completed in {run_time:.2f} seconds")
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

### Project 2 (Enhanced with Progress and Data Export)

```python
import enhanced_pynlm as pynlm
import numpy as np

# Setup with enhanced API
brain_manager = pynlm.create_default_brain()
world_manager = pynlm.create_default_world(
    width=20, height=20, 
    vision_width=8, vision_height=8
)

brain_manager.initialize(num_steps=5)  # Warm up
world_manager.initialize(brain_manager.brain)

# Setup data collection
collected_percepts = []
collected_stats = []

# Progress callback
def store_data_callback(progress, processed, total, elapsed, message):
    if processed > 0:
        # Store brain statistics every 5 steps
        if processed % 5 == 0:
            stats = brain_manager.get_statistics()
            collected_stats.append(stats)

# Run episode with data collection
callback = pynlm.ProgressCallback(on_progress=store_data_callback)
brain_manager.add_progress_callback(callback)

result = world_manager.run_episode(
    num_steps=30,
    progress_callback=callback
)

# Convert to NumPy for analysis
if collected_stats:
    firing_rates = [s['neurons']['average_firing_rate'] for s in collected_stats]
    np_firing_rates = np.array(firing_rates)
    
    print(f"Firing rate statistics:")
    print(f"  Mean: {np_firing_rates.mean():.3f}")
    print(f"  Std: {np_firing_rates.std():.3f}")
    print(f"  Min: {np_firing_rates.min():.3f}")
    print(f"  Max: {np_firing_rates.max():.3f}")

print(f"Watched world for 30 steps")
print(f"Firing rate: {brain_manager.average_firing_rate:.3f}")
```

### Project 3: Complete Agent with Learning

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

### Project 3 (Enhanced with Full Feature Set)

```python
import enhanced_pynlm as pynlm
import numpy as np

# Setup with all features
config = pynlm.create_default_config()
brain_manager = pynlm.create_default_brain(config)
world_manager = pynlm.create_default_world(
    width=15, height=15, 
    vision_width=8, vision_height=8
)

brain_manager.initialize(num_steps=10)  # Warm up brain
world_manager.initialize(brain_manager.brain)

# Enable all learning subsystems
brain_manager.brain.enableRewardModulation(True)
brain_manager.brain.enableStructuralPlasticity(True)
brain_manager.brain.enableDevelopment(True)
brain_manager.brain.enableCuriosity(True)

# Create progress callbacks
progress_updates = []
def progress_callback(progress, processed, total, elapsed, message):
    progress_updates.append({
        'step': processed,
        'progress': progress,
        'elapsed': elapsed,
        'message': message
    })
    
    # Print every 20 steps
    if processed % 20 == 0:
        print(f"Step {processed}/{total} ({progress:.1f}%): "
              f"{brain_manager.firing_neuron_count} neurons firing, "
              f"curiosity: {brain_manager.get_curiosity_level():.3f}")

callback = pynlm.ProgressCallback(
    update_interval=0.5,
    on_progress=progress_callback
)
brain_manager.add_progress_callback(callback)

# Run enhanced simulation with batch processing
print("Starting enhanced agent simulation...")
for step in range(100):
    # Update world
    world_manager.update(0.1)
    
    # Get and process sensory input
    percept = world_manager.get_sensory_percept()
    if world_manager.agent:
        world_manager.agent.process_sensory_input(percept)
    
    # Run brain step
    brain_manager.step(step)
    
    # Decode motor command
    if world_manager.agent:
        action = world_manager.agent.decode_motor_command()
        
        # Apply action
        world_manager.apply_motor_command(action, world_manager.get_simulation_time())
        
        # Apply reward modulation
        reward = (percept.get_internal()[0] 
                 if percept.get_internal() and percept.get_internal()[0] 
                 else 0.0)
        if world_manager.agent:
            world_manager.agent.apply_reward_modulation(reward, 0.0)
    
    # Update development
    if world_manager.agent:
        world_manager.agent.update_development(0.1)

print("\nEnhanced agent simulation complete!")

# Analyze results
if progress_updates:
    steps = [p['step'] for p in progress_updates]
    progress = [p['progress'] for p in progress_updates]
    
    print(f"\nSimulation Summary:")
    print(f"  Total steps: {max(steps)}")
    print(f"  Average progress: {np.mean(progress):.1f}%")
    print(f"  Final curiosity: {brain_manager.get_curiosity_level():.3f}")
    print(f"  Final novelty: {brain_manager.get_novelty_level():.3f}")
    print(f"  Prediction error: {brain_manager.get_prediction_error():.3f}")

# Save brain checkpoint for later use
checkpoint_file = "brain_checkpoint.pkl"
try:
    brain_manager.save_checkpoint(checkpoint_file)
    print(f"Brain saved to {checkpoint_file}")
except Exception as e:
    print(f"Warning: Could not save checkpoint: {e}")
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time.

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

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

1. **Read `HOW_TO_USE.md` for more details**
2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
3. **Experiment with different configurations!**

That's it! You're now ready to use NLM.

## What Makes NLM Enhanced

Compared to the basic `pynlm` bindings, the enhanced version provides:

- **Pythonic API**: Method chaining, properties, context managers
- **Batch Processing**: Run multiple steps efficiently with progress tracking
- **NumPy Integration**: Convert brain data to NumPy arrays for analysis
- **Progress Callbacks**: Track long-running operations with customizable callbacks
- **Error Handling**: Custom exceptions for specific error conditions
- **Configuration Management**: Centralized configuration and validation
- **Visualization Integration**: Export data for plotting and analysis
- **Plugin System**: Extend functionality with custom plugins
- **Checkpoint Management**: Save and load brain states
- **Performance Monitoring**: Track and analyze simulation performance

Compared to the basic `pynlm` bindings, the enhanced version provides:

- **Pythonic API**: Method chaining, properties, context managers
- **Batch Processing**: Run multiple steps efficiently with progress tracking
- **NumPy Integration**: Convert brain data to NumPy arrays for analysis
- **Progress Callbacks**: Track long-running operations with customizable callbacks
- **Error Handling**: Custom exceptions for specific error conditions
- **Configuration Management**: Centralized configuration and validation
- **Visualization Integration**: Export data for plotting and analysis
- **Plugin System**: Extend functionality with custom plugins
- **Checkpoint Management**: Save and load brain states
- **Performance Monitoring**: Track and analyze simulation performance