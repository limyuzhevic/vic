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

### NEW: Advanced Agent Setup

```python
import pynlm

# Create a complete agent setup with one line
brain, world, agent = pynlm.createAgentSetup(
    neuronCount=1000,
    worldSize=20,
    visionWidth=8,
    visionHeight=8,
    enableLearning=True
)

# Run simulation
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

# Get statistics
print("Brain stats:", pynlm.getBrainStats(brain))
print("Agent stats:", pynlm.getAgentStats(agent))
```

### NEW: Easy Serialization

```python
import pynlm

# Save your agent setup
brain, world, agent = pynlm.createAgentSetup()
pynlm.saveAgentToDirectory(brain, world, agent, "/tmp/my_agent")

# Later, load it back
loaded_brain, loaded_world, loaded_agent = pynlm.loadAgentFromDirectory("/tmp/my_agent")
```

### NEW: Debugging and Monitoring

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable verbose logging
pynlm.enableVerboseLogging()

# Print brain state
pynlm.printBrainState(brain)

# Run a few steps
for i in range(10):
    brain.step(i)

# Print final state
pynlm.printBrainState(brain, includeMemory=True)
```

### NEW: Advanced Agent Configuration

```python
import pynlm

# Create an advanced agent with custom configuration
brain, world, agent = pynlm.createAdvancedAgent(
    configName="research",
    worldConfig="large",
    enableAllFeatures=True
)

# Run a research experiment
pynlm.runExperiment(brain, world, agent, 1000, "research_study")
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

# NEW: Use convenience functions for comprehensive stats
stats = pynlm.getBrainStats(brain)
for key, value in stats.items():
    print(f"{key}: {value}")
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# NEW: Batch enable multiple features
pynlm.enableAllFeatures(agent, True)
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

## NEW: Advanced Features

### Configuration Management

```python
# Create and configure configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.15)

# Validate configuration
if pynlm.validateConfig(config):
    print("Configuration is valid!")

# Get configuration statistics
stats = pynlm.getConfigStats(config)
print(f"Total configuration entries: {stats['total_entries']}")
```

### Batch Operations

```python
# Set multiple configuration values at once
keyValues = [
    ("brain.neuron_count", "1000"),
    ("brain.synapse_density", "0.1"),
    ("plasticity.stdp.enable", "true")
]
pynlm.batchSetConfig(config, keyValues)
```

### Debugging Tools

```python
# Enable verbose logging
pynlm.enableVerboseLogging()

# Print detailed state
pynlm.printBrainState(brain)
pynlm.printAgentState(agent)

# Create a backup of current state
pynlm.backupState(brain, world, agent, "/tmp/backup")
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

### NEW: Complete Research Experiment

```python
import pynlm

# Setup advanced agent
brain, world, agent = pynlm.createAdvancedAgent(
    configName="experiment",
    worldConfig="medium",
    enableAllFeatures=True
)

# Run experiment
pynlm.runExperiment(brain, world, agent, 500, "learning_experiment")

# Save results
pynlm.saveAgentToDirectory(brain, world, agent, "/tmp/experiment_results")

print("Experiment completed and saved!")
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
| Get brain stats | `pynlm.getBrainStats(brain)` |
| Get agent stats | `pynlm.getAgentStats(agent)` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Save agent | `pynlm.saveAgentToDirectory(brain, world, agent, path)` |
| Load agent | `pynlm.loadAgentFromDirectory(path)` |

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
4. Try the new advanced features like `createAdvancedAgent()` and `runExperiment()`

That's it! You're now ready to use NLM with all its advanced features and capabilities.
