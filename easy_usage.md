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

## Advanced Python Features

### Configuration Management

```python
import pynlm

# Create configuration from dictionary
config_dict = {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity.stdp.enable": True,
    "plasticity.hebbian.enable": True,
    "neuromod.curiosity.enable": True
}
config = pynlm.createConfig(config_dict)

# Create simple configuration
brain = pynlm.createBrain(pynlm.createSimpleConfig(500, 1))

# Create demo configuration for learning examples
demo_config = pynlm.createDemoConfig()
brain = pynlm.createBrain(demo_config)
```

### Enhanced Simulation Features

```python
import pynlm

# Create brain and agent together
brain, agent = pynlm.createBrainAndAgent({
    "neuron_count": 800,
    "plasticity.stdp.enable": True,
    "neuromod.curiosity.enable": True
})

# Create world and agent in one go
config = pynlm.createDemoConfig()
brain = pynlm.createBrain(config)
world = pynlm.createWorld(20, 20, 8, 8)
agent = pynlm.createAgent(brain, world)

# Run simulation using convenience method
brain, agent = pynlm.createBrainAndAgent(config)
world = pynlm.createWorld(15, 15, 6, 6)
brain, agent = pynlm.createBrainAndAgent(config)
agent.initialize(world)

# Run complete episode
brain, agent = pynlm.createBrainAndAgent(config)
world = pynlm.createWorld(10, 10, 4, 4)
agent.initialize(world)

# Use new convenience methods
brain.runSimulation(1000)  # Run complete simulation
brain.setRandomSeed(42)    # For reproducible results
brain.logStatus()          # Get detailed brain information
```

### Brain Monitoring and Information

```python
import pynlm

config = pynlm.createDemoConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enhanced statistics and monitoring
stats = {
    "total_neurons": brain.getTotalNeuronCount(),
    "total_synapses": brain.getTotalSynapseCount(),
    "active_neurons": brain.getActiveNeuronCount(),
    "firing_neurons": brain.getFiringNeuronCount(),
    "average_firing_rate": brain.getAverageFiringRate(),
    "excitation_inhibition_ratio": brain.getExcitationInhibitionRatio(),
    "total_spikes": brain.getTotalSpikeCount(),
    "developmental_stage": brain.getDevelopmentalStage(),
    "region_count": brain.getRegionCount()
}

# Get brain information
brain.logStatus()

# Get neurons by type
excitatory_neurons = brain.getNeuronsByType(pynlm.NeuronType.Excitatory)
motor_neurons = brain.getNeuronsByType(pynlm.NeuronType.Motor)

# Get firing neurons (convenience method)
firing_neurons = brain.getFiringNeurons()
```

### Agent Interaction

```python
import pynlm

# Create complete agent system
config = pynlm.createDemoConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createWorld(15, 15, 6, 6)
agent = pynlm.createAgent(brain, world)

# Agent thinks based on sensory input
for step in range(100):
    world.update(0.1)
    
    # Process visual input
    vision_data = {
        "width": 6, "height": 6, "channels": 3,
        "data": [0.1, 0.2, 0.3, 0.4, 0.5, 0.6,
                 0.7, 0.8, 0.9, 1.0, 1.1, 1.2,
                 1.3, 1.4, 1.5, 1.6, 1.7, 1.8,
                 1.9, 2.0, 2.1, 2.2, 2.3, 2.4,
                 2.5, 2.6, 2.7, 2.8, 2.9, 3.0,
                 3.1, 3.2, 3.3, 3.4, 3.5, 3.6]
    }
    agent.processVision(vision_data)
    
    # Agent thinks and acts
    agent.think()
    action = agent.act()
    
    # World executes action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Agent learns from experience
    agent.learn(reward=1.0, predicted_reward=0.8)

# Disable features when not needed
agent.disableRewardModulation()
agent.disableCuriosity()

# Check agent status
if agent.isDevelopmentEnabled():
    print("Development is enabled")
if agent.isCuriosityEnabled():
    print("Curiosity is enabled")
```

### Advanced Simulation and Analysis

```python
import pynlm

config = pynlm.createDemoConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createWorld(10, 10, 4, 4)
agent = pynlm.createAgent(brain, world)

# Run multiple episodes and get rewards
brain, agent = pynlm.createBrainAndAgent(config)
world = pynlm.createWorld(8, 8, 4, 4)
agent.initialize(world)
rewards = pynlm.runMultipleEpisodes(brain, world, 10, 200)
print("Rewards per episode:", rewards)

# Performance measurement
brain, agent = pynlm.createBrainAndAgent(config)
world = pynlm.createWorld(6, 6, 3, 3)
agent.initialize(world)
performance = pynlm.measurePerformance(brain, 1000)
print("Performance metrics:", performance)

# Statistical analysis
brain, agent = pynlm.createBrainAndAgent(config)
world = pynlm.createWorld(5, 5, 3, 3)
agent.initialize(world)
statistics = pynlm.runStatisticalAnalysis(brain, 100, 500)
print("Statistical analysis:", statistics)
```

### Module Information

```python
import pynlm

# Get version information
print(pynlm.getVersion())

# Get module information
info = pynlm.getModuleInfo()
print("Name:", info["name"])
print("Version:", info["version"])
print("Description:", info["description"])
print("License:", info["license"])
```

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
