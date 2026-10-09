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

## Advanced Python API Features

NLM now provides advanced features for power users and research applications:

### 1. Enhanced Configuration

The Python API now supports type-safe configuration methods:

```python
import pynlm

config = pynlm.createDefaultConfig()

# Set configuration values with type safety
config.set("brain.neuron_count", 1500)      # int64_t
config.set("brain.synapse_density", 0.2f)   # float
config.set("brain.connection_probability", 0.1)
config.set("plasticity.stdp.enable", True) # bool
config.set("random_seed", 42)              # int64_t

# Get configuration values
neuron_count = config.getInt("brain.neuron_count", 1000)
connection_prob = config.getFloat("brain.connection_probability", 0.1)
enable_stdp = config.getBool("plasticity.stdp.enable", True)

# Configuration validation
errors = config.validate()
if errors:
    print("Configuration errors:", errors)
```

### 2. Advanced Brain Operations

Brain operations now include performance monitoring and advanced statistics:

```python
import pynlm
import time

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation with performance monitoring
start_time = time.time()
for step in range(1000):
    brain.step(step)
    
    # Optional: Monitor progress
    if step % 200 == 0:
        stats = brain.getBrainStats()
        print(f"Step {step}: {stats['firingNeuronCount']} firing, "
              f"{stats['averageFiringRate']:.2f} avg rate")

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")

# Get comprehensive statistics
brain_stats = brain.getBrainStats()
print("Comprehensive statistics:", brain_stats)

# Get performance profile
profile = brain.getPerformanceProfile(500, 0.001)
print("Performance profile:", profile)
```

### 3. Enhanced Agent Brain

AgentBrain now includes advanced neuromodulation and learning features:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all advanced features
agent.enableAdvancedNeuromodulation()

# Run simulation with detailed tracking
for step in range(500):
    world.update(0.1)
    
    # Process sensory input with validation
    percept = world.getSensoryPercept()
    if percept:
        agent.processSensoryInput(percept)
    
    # Run brain
    brain.step(step)
    
    # Decode motor command
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward with action context
    action_result = world.getActionResult()
    if action_result:
        agent.applyAdvancedRewardModulation(action_result)
    
    # Update development
    agent.updateDevelopmentWithAdaptiveTimestep()
    
    # Get behavioral profile
    if step % 100 == 0:
        profile = agent.getBehavioralProfile()
        print(f"Step {step}: Curiosity={profile['curiosityLevel']:.2f}, "
              f"Novelty={profile['noveltyLevel']:.2f}, "
              f"Learning={profile['isLearning']}, "
              f"Exploring={profile['isExploring']}")
```

### 4. Performance Benchmarking

Built-in performance benchmarking tools:

```python
import pynlm

def benchmark_brain_performance():
    """Benchmark brain performance across different configurations."""
    
    configs = [
        {"neurons": 500, "steps": 1000},
        {"neurons": 1000, "steps": 500},
        {"neurons": 2000, "steps": 200},
        {"neurons": 5000, "steps": 100},
    ]
    
    print("=== NLM Performance Benchmark ===\n")
    
    results = []
    
    for config in configs:
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.getConfig().set("neuron_count", config["neurons"])
        brain.getConfig().set("random_seed", 42)
        
        start_time = time.time()
        brain.initialize()
        
        for step in range(config["steps"]):
            brain.step(step)
        
        end_time = time.time()
        total_time = end_time - start_time
        
        brain_stats = brain.getBrainStats()
        
        result = {
            "neurons": config["neurons"],
            "steps": config["steps"],
            "time": total_time,
            "steps_per_sec": config["steps"] / total_time,
            "spikes_per_step": brain_stats.get("totalSpikeCount", 0) / config["steps"],
            "final_firing_rate": brain_stats.get("averageFiringRate", 0.0)
        }
        
        results.append(result)
        
        print(f"{config['neurons']:>6} neurons: {result['steps_per_sec']:>8.1f} steps/sec, "
              f"{result['spikes_per_step']:>6.2f} spikes/step, "
              f"{result['final_firing_rate']:>6.2f} final rate")
    
    return results

# Memory usage profiling
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get memory usage estimate
memory_usage = brain.getMemoryUsage()
print(f"Memory usage: {memory_usage['totalMemoryKB']} KB total")
print(f"  Neurons: {memory_usage['neuronMemoryKB']} KB")
print(f"  Synapses: {memory_usage['synapseMemoryKB']} KB")
```

### 5. Checkpoint and State Management

Advanced checkpoint and state management:

```python
import pynlm
import os

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation
for step in range(100):
    brain.step(step)

# Save brain state with validation
checkpoint_file = "my_brain_checkpoint.bin"
if brain.save(checkpoint_file):
    print(f"Brain saved successfully to {checkpoint_file}")
else:
    print("Failed to save brain state")

# Load with validation
if brain.load(checkpoint_file):
    print(f"Brain loaded successfully from {checkpoint_file}")
    print(f"Loaded brain has {brain.getTotalNeuronCount()} neurons")
else:
    print("Failed to load brain state")

# Create sub-brains for parallel processing
sub_brain1 = brain.createSubBrain("sub1", 500)
sub_brain2 = brain.createSubBrain("sub2", 500)

print(f"Created sub-brains: {sub_brain1.getTotalNeuronCount()} + {sub_brain2.getTotalNeuronCount()} neurons")
```

### 6. Simulation Setup Types

Different simulation setup types for various use cases:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()

# Setup type registry
setup_types = pynlm.getAvailableSetups()

# Create different setups
default_setup = pynlm.createDefaultSetup(brain, world)
advanced_setup = pynlm.createEnhancedSetup(brain, world)
experimental_setup = pynlm.createExperimentalSetup(brain, world, environmentType="controlled")

print("Setup types available:", setup_types)
print(f"Created {len([default_setup, advanced_setup, experimental_setup])} different simulation setups")
```

### 7. Research and Development Features

Tools for research and development:

```python
import pynlm

# Research setup for controlled experiments
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
research_setup = pynlm.createResearchSetup(brain, world)

# Development setup for skill acquisition
development_setup = pynlm.createDevelopmentSetup(brain, world, skillAcquisition=True)

# Export brain data for analysis
analysis_data = brain.exportNeurons("brain_analysis.json")
print(f"Exported brain analysis: {analysis_data}")

# Import brain data (placeholder)
import_result = brain.importNeurons("brain_analysis.json")
print(f"Import result: {import_result}")
```

---

## Next Steps

After mastering the basics, explore these advanced topics:

1. **Advanced API Features**: Use the enhanced configuration, performance monitoring, and advanced agent features
2. **Research Applications**: Create experimental setups with controlled conditions
3. **Development Scenarios**: Use development setups for skill acquisition and learning studies
4. **Performance Optimization**: Use benchmarking tools and memory profiling
5. **State Management**: Implement checkpoint systems for long-running simulations
6. **Parallel Processing**: Create sub-brains for parallel computation
7. **Data Analysis**: Export and import brain states for analysis

NLM provides a comprehensive platform for artificial neural simulation, from simple demonstrations to complex research applications.

---

## Additional Resources

- **HOW_TO_USE.md**: Detailed installation and configuration guide
- **docs/ARCHITECTURE.md**: System architecture and component overview
- **docs/EXPERIMENTS.md**: Experiment descriptions and protocols
- **pyproject.toml**: Project configuration and dependencies

Enjoy exploring the fascinating world of artificial neural systems with NLM!

