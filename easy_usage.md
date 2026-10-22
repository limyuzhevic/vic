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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the BrainManager examples in the demo directory
5. Check out the BUILD_AND_TEST.md for a comprehensive demonstration

That's it! You're now ready to use NLM with the powerful new BrainManager interface!

### Quick BrainManager Reference

**Quick Start Code:**

```python
import pynlm

# Simple brain creation and simulation
brain = pynlm.BrainManager.createBrain(
    pynlm.BrainManager.createConfig(neuron_count=500, region_count=2)
)
pynlm.BrainManager.initializeBrain(brain)
pynlm.BrainManager.runSimpleSimulation(brain, 50)

print("✓ Brain simulation completed!")
```

**Advanced Configuration:**

```python
# Create custom config with BrainManager
config = pynlm.BrainManager.createConfig(
    neuron_count=1000,
    region_count=3,
    connection_probability=0.15,
    timestep=0.001,
    random_seed=42
)

# Save and load
pynlm.BrainManager.saveConfigToFile(config, "my_config.json")
loaded_config = pynlm.BrainManager.loadConfigFromFile("my_config.json")
```

**Enable Learning Features:**

```python
# Enable all learning and development features
pynlm.BrainManager.enableDevelopment(brain, True)
pynlm.BrainManager.enablePlasticity(brain, True)
pynlm.BrainManager.enableRewardModulation(brain, True)
pynlm.BrainManager.enableCuriosity(brain, True)
```

**Monitor and Save State:**

```python
# Get system statistics
stats = pynlm.BrainManager.getSystemStats(brain)
print(f"Neurons: {stats['neuron_count']}")
print(f"Firing rate: {stats['average_firing_rate']} Hz")

# Save for later use
pynlm.BrainManager.saveBrainState(brain, "brain_checkpoint.dat")
```

**Error Handling:**

```python
# BrainManager handles null objects safely
brain = pynlm.BrainManager.createBrain(some_config)
# Methods return safe defaults if brain is invalid
neuron_count = pynlm.BrainManager.getNeuronCount(brain)  # Safe even if null
```

**BrainManager Methods Summary:**

| Method | Purpose | Returns |
|--------|---------|---------|
| `createConfig()` | Create default configuration | Config |
| `loadConfigFromFile()` | Load config from file | Config |
| `saveConfigToFile()` | Save config to file | bool |
| `createBrain()` | Create brain with config | Brain |
| `initializeBrain()` | Initialize brain | bool |
| `runSimpleSimulation()` | Run simulation with optional input | void |
| `enableDevelopment()` | Enable development features | void |
| `enablePlasticity()` | Enable plasticity | void |
| `enableRewardModulation()` | Enable reward modulation | void |
| `enableCuriosity()` | Enable curiosity | void |
| `saveBrainState()` | Save brain state | bool |
| `loadBrainState()` | Load brain state | bool |
| `getSystemStats()` | Get system statistics | dict |
| `getBrainStatus()` | Get formatted status | string |
| `getNeuronCount()` | Get neuron count | size_t |
| `getSynapseCount()` | Get synapse count | size_t |
| `getAverageFiringRate()` | Get average firing rate | float |
| `getExcitationInhibitionRatio()` | Get E/I ratio | float |

**Key Benefits:**

- **Simplified API**: Easy-to-use high-level interface
- **Error Handling**: Graceful handling of null/invalid objects
- **Configuration Management**: Create, save, and load configurations
- **Development Features**: Enable learning, plasticity, reward, and curiosity
- **State Management**: Save and restore brain states
- **System Monitoring**: Get statistics and formatted status
- **Production Ready**: Robust and tested implementation
- **Backward Compatible**: Works with existing nlm framework
- **Well Documented**: Comprehensive examples and documentation

**Ready to use!** You can now use BrainManager for:

- Quick prototyping and testing
- Production simulations with learning
- Research experiments with configurable brains
- Educational demonstrations
- Integration with existing systems
- Custom simulation scenarios

Get started with BrainManager today! The framework is now more accessible, powerful, and user-friendly than ever before.
