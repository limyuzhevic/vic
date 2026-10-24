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

### Example 2: Using New Convenience Methods

```python
import pynlm

# Create configuration and use new convenience methods
config = pynlm.createDefaultConfig()

# Set configuration using type-safe methods
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("neuromod.dopamine.scale", 1.5)

# Get configuration values using new convenience methods
neuron_count = config.getInt("brain.neuron_count")
synapse_density = config.getFloat("brain.synapse_density", 0.05)
dopamine_scale = config.getFloat("neuromod.dopamine.scale", 1.0)

print(f"Brain configured with {neuron_count} neurons")
print(f"Synapse density: {synapse_density}")
print(f"Dopamine scale: {dopamine_scale}")

# Create brain and use new Brain convenience methods
brain = pynlm.createBrain(config)
brain.initialize()

# Get neuron count by type
excitatory = brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)
inhibitory = brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory)
sensory = brain.getNeuronCountByType(pynlm.NeuronType.Sensory)

print(f"Neuron types - Excitatory: {excitatory}, Inhibitory: {inhibitory}, Sensory: {sensory}")

# Get comprehensive brain statistics
brain_stats = brain.getBrainStats()
print(f"Total spikes: {brain_stats['total_spikes']}")
print(f"Firing rate: {brain_stats['firing_rate']:.4f}")
```

### Example 2: Brain in a Simple World (Enhanced)

```python
import pynlm

# Create everything
config = pynlm.createDefaultConfig()

# Configure using new convenience methods
config.set("brain.neuron_count", 800)
config.set("neuromod.dopamine.scale", 1.2)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)

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

# Get comprehensive statistics using new convenience methods
brain_stats = brain.getBrainStats()
print(f"Brain statistics - Total spikes: {brain_stats['total_spikes']}, Firing rate: {brain_stats['firing_rate']:.4f}")

agent_stats = agent.getAgentStats()
print(f"Agent statistics - Curiosity: {agent_stats['curiosity']:.3f}, Novelty: {agent_stats['novelty']:.3f}")
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

# Using new convenience methods:
print("Excitatory neurons:", brain.getNeuronCountByType(pynlm.NeuronType.Excitatory))
print("Inhibitory neurons:", brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory))

# Get comprehensive brain statistics
brain_stats = brain.getBrainStats()
print("Comprehensive stats:")
for key, value in brain_stats.items():
    print(f"  {key}: {value}")
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

### Project 2: Brain Watching a World (Enhanced)

```python
import pynlm

# Setup with enhanced configuration
config = pynlm.createDefaultConfig()

# Configure using new convenience methods
config.set("brain.neuron_count", 600)
config.set("neuromod.curiosity.enable", True)

brain = pynlm.createBrain(config)
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

# Use new convenience methods to get statistics
brain_stats = brain.getBrainStats()
print("Brain statistics:")
for key, value in brain_stats.items():
    print(f"  {key}: {value}")

# Get neuron type analysis
print(f"Excitatory neurons: {brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)}")
print(f"Inhibitory neurons: {brain.getNeuronCountByType(pynlm.NeuronType.Inhibitory)}")

print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent (Enhanced)

```python
import pynlm

# Setup with enhanced configuration
config = pynlm.createDefaultConfig()

# Configure using new convenience methods
config.set("brain.neuron_count", 1000)
config.set("neuromod.dopamine.scale", 1.5)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning with agent convenience methods
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        # Use new convenience methods for statistics
        brain_stats = brain.getBrainStats()
        agent_stats = agent.getAgentStats()
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"Curiosity={agent_stats['curiosity']:.3f}, "
              f"Novelty={agent_stats['novelty']:.3f}")

print("Agent simulation complete!")

# Final comprehensive statistics using new convenience methods
print("\nFinal comprehensive statistics:")
brain_stats = brain.getBrainStats()
for key, value in brain_stats.items():
    if isinstance(value, float):
        print(f"  Brain {key}: {value:.4f}")
    else:
        print(f"  Brain {key}: {value}")

agent_stats = agent.getAgentStats()
for key, value in agent_stats.items():
    if isinstance(value, float):
        print(f"  Agent {key}: {value:.4f}")
    elif isinstance(value, dict):
        print(f"  Agent {key}:")
        for subkey, subvalue in value.items():
            print(f"    {subkey}: {subvalue}")
    else:
        print(f"  Agent {key}: {value}")
```

---

## Quick Reference

### New Convenience Methods

#### Config Class
| Method | Description | Example |
|--------|-------------|---------|
| `getInt(key, default)` | Get integer value safely | `config.getInt("brain.neuron_count", 1000)` |
| `getFloat(key, default)` | Get float value safely | `config.getFloat("brain.synapse_density", 0.1)` |
| `getBool(key, default)` | Get boolean value safely | `config.getBool("neuromod.curiosity.enable", True)` |
| `getString(key, default)` | Get string value safely | `config.getString("config.name", "default")` |

#### Brain Class
| Method | Description | Example |
|--------|-------------|---------|
| `getNeuronCountByType(type)` | Count neurons by type | `brain.getNeuronCountByType(pynlm.NeuronType.Excitatory)` |
| `getBrainStats()` | Get comprehensive brain statistics | `stats = brain.getBrainStats()` |

#### AgentBrain Class
| Method | Description | Example |
|--------|-------------|---------|
| `getAgentStats()` | Get comprehensive agent statistics | `stats = agent.getAgentStats()` |
| `getActionRecommendation(context)` | Get action recommendation based on brain state | `rec = agent.getActionRecommendation()` |
| `enableAllSystems()` | Enable all agent systems at once | `agent.enableAllSystems()` |
| `disableAllSystems()` | Disable all agent systems at once | `agent.disableAllSystems()` |

### Standard Methods (Unchanged)

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
