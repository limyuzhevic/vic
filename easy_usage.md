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

---

## Advanced Usage Examples

### Advanced Example 1: Performance Monitoring

```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create performance monitor
monitor = pynlm.createPerformanceMonitor()

# Run with monitoring
monitor_result = pynlm.runSimulationWithMonitoring(brain, monitor, 500)

print(f"Completed {monitor_result.stepsCompleted} steps")
print(f"Total spikes: {monitor_result.totalSpikes}")
print(f"Average CPU time per step: {monitor_result.avgCPUTimePerStep:.4f} seconds")
print(f"Final firing rate: {monitor_result.finalFiringRate:.2f}")
print(f"Max spikes in a single step: {monitor_result.maxSpikesInStep}")
```

### Advanced Example 2: Batch Processing

```python
import pynlm

def run_batch_training(num_episodes=100):
    """Run multiple training episodes with batch processing"""
    
    # Configuration for batch training
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", "500")
    config.set("plasticity.stdp.enable", "true")
    config.set("neuromod.curiosity.enable", "true")
    
    batch_results = []
    
    for episode in range(num_episodes):
        print(f"Running episode {episode + 1}/{num_episodes}")
        
        # Create new brain for each episode
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent with learning enabled
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run batch simulation
        result = pynlm.runBatchSimulation(
            brain, world, agent, 
            numSteps=200,
            enableNeuromodulation=True,
            enableDevelopment=True,
            enableCuriosity=True
        )
        
        batch_results.append(result)
        
        # Log results
        if episode % 10 == 0:
            print(f"  Episode {episode + 1}: {result.stepsCompleted} steps, "
                  f"{result.totalSpikes} spikes, avg firing rate: {result.finalFiringRate:.2f}")
    
    # Analyze batch results
    print(f"\nBatch Training Complete!")
    print(f"Average steps per episode: {sum(r.stepsCompleted for r in batch_results) / len(batch_results):.1f}")
    print(f"Average total spikes per episode: {sum(r.totalSpikes for r in batch_results) / len(batch_results):.1f}")
    print(f"Average final firing rate: {sum(r.finalFiringRate for r in batch_results) / len(batch_results):.2f}")
    
    return batch_results

# Run batch training
results = run_batch_training(50)
```

### Advanced Example 3: Configuration Management

```python
import pynlm
import json
import os

# Create optimized configuration
config = pynlm.createOptimizedConfig("balanced")

# Export configuration to JSON
config.saveToFile("my_config.json")

# Load configuration from file
loaded_config = pynlm.loadConfigFromJSON("my_config.json")

# Load configuration from dictionary
dict_config = {
    "brain.neuron_count": "1000",
    "brain.synapse_density": "0.15",
    "neuromod.curiosity.enable": "true",
    "plasticity.stdp.enable": "true"
}

from_dict_config = pynlm.createConfigFromDict(dict_config)

# Convert to dictionary for easy analysis
config_dict = pynlm.configToDict(config)
print("Configuration parameters:", len(config_dict))
for key, value in config_dict.items():
    print(f"  {key}: {value}")

# Create brain with loaded configuration
brain = pynlm.createBrain(loaded_config)
brain.initialize()
```

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM with all its advanced features.

