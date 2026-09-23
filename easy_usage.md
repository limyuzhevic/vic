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

## Configuration Guide

### Creating and Using Configurations

NLM provides a flexible configuration system that allows you to:

1. **Use default configurations**
   ```python
   import pynlm
   
   # Create a default configuration
   config = pynlm.createDefaultConfig()
   
   # Modify specific parameters
   config.set("brain.neuron_count", 1000)
   config.set("plasticity.stdp.learning_rate", 0.001)
   ```

2. **Load from file**
   ```python
   # Create configuration and load from JSON/YAML file
   config = pynlm.createDefaultConfig()
   config.loadFromFile("my_config.json")
   ```

3. **Validate configurations**
   ```python
   # Check if configuration is valid
   if config.isValid() {
       print("Configuration is valid!")
   } else {
       print("Configuration has validation errors")
       print(config.validate())
   }
   ```

4. **Set validation rules**
   ```python
   # Define validation rules for specific parameters
   config.setValidator("brain.neuron_count", 
       lambda key, value: ValidationResult.OutOfRange if (value < 100 || value > 10000) else ValidationResult.Success)
   ```

5. **Save configurations**
   ```python
   # Save configuration to file
   config.saveToFile("my_config.json")
   ```

### Configuration Examples

#### Example 1: Custom Learning Configuration
```python
import pynlm

# Create configuration for focused learning
config = pynlm.createDefaultConfig()

# Set parameters for high plasticity
config.set("plasticity.hebbian.enable", true)
config.set("plasticity.stdp.enable", true)
config.set("plasticity.stdp.learning_rate", 0.05)

# Configure for faster learning
config.set("brain.synapse_density", 0.2)
config.set("development.synaptogenesis_rate", 0.001)

# Enable neuromodulation for reward-based learning
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", true)

# Save custom configuration
config.saveToFile("learning_config.json")
```

#### Example 2: Development-Focused Configuration
```python
import pynlm

# Start with defaults
config = pynlm.createDefaultConfig()

# Configure for developmental learning
config.set("development.enable", true)
config.set("development.stage", pynlm.DevelopmentalStage.CriticalPeriod)
config.set("development.synaptogenesis_rate", 0.002)
config.set("development.pruning_rate", 0.0005)

# Set longer critical period for more development
config.set("development.critical_period_end", 200.0)

# Create brain with this config
brain = pynlm.createBrain(config)
brain.initialize()
```

#### Example 3: Performance-Optimized Configuration
```python
import pynlm

# Create performance configuration
config = pynlm.createDefaultConfig()

# Optimize for speed
config.set("brain.neuron_count", 200)  # Fewer neurons for speed
config.set("simulation.timestep", 0.01)  # Larger timestep
config.set("plasticity.enable", false)  # No plasticity for pure simulation

# Still keep basic functionality
config.set("brain.connection_probability", 0.1)
config.set("log_level", pynlm.LogLevel.Warning)  # Reduce logging
```

### Configuration Parameters Reference

Here are some common configuration parameters you can use:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 500 | Number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.stdp.enable` | bool | true | Enable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.01 | STDP learning rate |
| `development.enable` | bool | true | Enable developmental processes |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `log_level` | string | "INFO" | Logging verbosity level |

### Quick Configuration Tips

1. **For research:** Use smaller neuron counts and enable all plasticity mechanisms
2. **For demos:** Use default configuration or create lightweight configs
3. **For production:** Optimize for performance with fewer neurons and limited plasticity
4. **For development:** Enable developmental stages and curiosity for exploration

---

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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
