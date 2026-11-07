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

| Make action | `agent.decodeMotorCommand()` |
        | Get neuromodulation state | `agent.getNeuromodulationState()` |

---

## Advanced Usage (NEW!)

### Advanced Agent Setup

This section shows how to use the new advanced convenience methods:

```python
import pynlm

# Advanced agent setup with learning configuration
config_dict = {
    "reward_modulation": True,
    "structural_plasticity": True,
    "development": True,
    "curiosity": True
}

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Setup advanced agent with learning
agent = pynlm.setupLearningAgent(brain, world, config_dict)

# Run experiment episode
spike_count, curiosity = agent.simulateEpisode(world, 100)
print(f"Episode completed: {spike_count} spikes, curiosity level: {curiosity:.3f}")
```

### Batch Operations for Performance

```python
import pynlm
import time

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Batch current injection for efficiency
injections = []
for i in range(100):
    injections.append((pynlm.NeuronId(i), 30.0))

start_time = time.time()
brain.injectCurrentBatch(injections)
for step in range(50):
    brain.step(step)
end_time = time.time()

print(f"Batch injection completed in {end_time - start_time:.3f} seconds")
```

### Experiment Configuration

```python
import pynlm

# Create configuration from dictionary
config_dict = {
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.1,
    "plasticity.stdp.enable": True,
   �ית
}

config = pynlm.createConfigFromDict(config_dict)
brain = pynlm.createBrain(config)
brain.initialize()

# Get comprehensive system statistics
stats = brain.getComprehensiveStats()
print(f"Neuron count: {stats[0]}, Synapse count: {stats[1]}, "
      f"Active neurons: {stats[2]}, Firing rate: {stats[4]:.2f}")
```

### Complete Experiment Setup

```python
import pynlm

def run_complex_experiment(num_steps=1000):
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent with all learning enabled
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run simulation
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 200 ==  handlebars():
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")
    
    # Get final statistics
    final_spikes = brain.getTotalSpikeCount()
    avg_firing = brain.getAverageFiringRate()
    curiosity = agent.getCuriosityLevel()
    
    return {
        "spikes": final_spikes,
        "firing_rate": avg_firing,
        "curiosity": curiosity
    }

# Run experiment
results = run_complex_experiment(1000)
print(f"Experiment results: {results}")
```

---

## Advanced Troubleshooting

### Performance Issues

**"Simulation is slow"**
- Use batch operations: `brain.injectCurrentBatch()` for multiple injections
- Reduce neuron count in config: `"brain.neuron_count": 500`
- Use fewer simulation steps

**"Memory usage is high"**
- Save brain state periodically: `brain.save("checkpoint.bin")`
- Use more efficient data structures (batch operations)
- Consider running in smaller regions

### Debugging Advanced Issues

**"Learning isn't working"**
- Check subsystem enable flags: `agent.enableRewardModulation(True)`
- Verify curiosity is enabled: `agent.enableCuriosity(True)`
- Check neuromodulation levels: `agent.getNeuromodulationState()`

**"Agent isn't exploring"**
- Enable curiosity: `agent.enableCuriosity(True)`
- Check novelty detection: `agent.getNoveltyLevel()`
- Adjust exploration parameters in config

---

## Next Steps (Enhanced)

When you're comfortable:

1. **For Advanced Users:**
   - Read `HOW_TO_USE.md` for comprehensive details
   - Explore Phase 6 integration features
   - Use `brain.getSystemStats()` for detailed diagnostics
   - Implement custom batch operations for your specific use case

2. **Research Applications:**
   - Use `createConfigFromDict()` for experimental parameter sets
   - Leverage `simulateEpisode()` for standardized experiments
   - Apply `injectCurrentBatch()` for high-throughput simulations

3. **Development & Testing:**
   - Use `setupLearningAgent()` for reproducible experimental setups
   - Monitor `getAdvancedStats()` for system health
   - Implement checkpointing with `brain.save()` for long simulations

That's it! You're now equipped to use both basic and advanced NLM features effectively.

**New Features Added:**
- Batch operations for performance
- Advanced agent configuration
- Comprehensive statistics gathering
- Flexible configuration loading
- Enhanced debugging tools
