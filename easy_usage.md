# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an **experimental computational brain project** designed to simulate how biological brains work using spiking neural networks.

Think of it like this:
- **Neurons** = Brain cells that send electrical signals
- **Synapses** = Connections between neurons that transmit signals
- **Learning** = Synaptic plasticity (strengthening/weakening connections based on activity)
- **Memory** = Neural memory systems that store experiences and patterns

NLM simulates **event-driven neural computation** using Leaky Integrate-and-Fire (LIF) neurons with:

- **Real neural dynamics** and spike timing
- **Multiple memory systems** (working, episodic, associative)
- **Advanced cognitive functions** (prediction, attention, planning)
- **Neuromodulation** (curiosity, novelty, reward processing)
- **Development** over time

That's it! NLM simulates the complete brain loop: sensory input → neural processing → memory → action selection.

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

# Advanced Examples (Memory Systems & Development)

## Example 4: Brain with Memory and Development

```python
import pynlm

# Create a default brain configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # More neurons for complex tasks
brain = pynlm.createBrain(config)
brain.initialize()

# Get access to memory systems and configure them
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

if working_memory:
    working_memory.setCapacity(brain.getTotalNeuronCount() // 10)
    
if episodic_memory:
    episodic_memory.setMaxEpisodes(5000)

# Enable advanced features
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)    # Learn from rewards
agent.enableDevelopment(True)         # Brain matures over time
agent.enableCuriosity(True)           # Explore new things
agent.enableStructuralPlasticity(True) # Grow new connections

# Simple world for testing
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)
agent.initialize(world)

# Run simulation with monitoring
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        if working_memory:
            print(f"  Memory traces active: {working_memory.getActiveTraces()}")
        if episodic_memory:
            print(f"  Episodes stored: {episodic_memory.getEpisodeCount()}")
        print(f"  Development stage: {agent.getDevelopmentalStage()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")

print("Advanced simulation complete!")
```

## Example 5: Using Prediction and Planning

```python
import pynlm

# Create brain with prediction capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Get access to prediction system
prediction_system = brain.getPredictionSystem()
planner = brain.getPlanner()

# Configure prediction system
if prediction_system:
    # Enable temporal sequence learning
    pass

if planner:
    planner.setPlanningDepth(3)  # Look 3 steps ahead

agent = pynlm.createAgentBrain(brain)
agent.enableDevelopment(True)

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

print("Running prediction and planning simulation...")
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        pred_error = agent.getPredictionError()
        curiosity = agent.getCuriosityLevel()
        if prediction_system:
            # Would check prediction accuracy here
            pass
        print(f"Step {step}: Prediction Error={pred_error:.3f}, Curiosity={curiosity:.3f}")

print("Prediction and planning simulation complete!")
```

## Example 6: Advanced Memory Management

```python
import pynlm

# Create brain with enhanced memory configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
brain = pynlm.createBrain(config)
brain.initialize()

# Configure all memory systems
if brain.getWorkingMemory():
    brain.getWorkingMemory().setCapacity(500)
    brain.getWorkingMemory().setDecayRate(0.95)  # Decay over time

if brain.getEpisodicMemory():
    brain.getEpisodicMemory().setMaxEpisodes(2000)

if brain.getAssociativeMemory():
    pass  # Would configure associative memory

# Create agent with full cognitive capabilities
agent = pynlm.createAgentBrain(brain)
agent.enableDevelopment(True)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Create a more complex world with dynamic elements
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.setMaxEnergy(1000.0)
world.setEnergyDecayRate(0.1)
world.reset()
agent.initialize(world)

print("Starting advanced memory management simulation...")
for step in range(2000):
    world.update(0.1)
    
    # Get sensory input and let agent process it
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Get action and apply to world
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Update development periodically
    agent.updateDevelopment(0.1)
    
    if step % 500 == 0:
        print(f"Step {step}:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Development stage: {agent.getDevelopmentalStage()}")
        print(f"  Neuromodulation level: {agent.getNeuromodulationLevel():.3f}")
        
        # Check memory stats
        working_mem = brain.getWorkingMemory()
        episodic_mem = brain.getEpisodicMemory()
        if working_mem:
            print(f"  Working memory active traces: {working_mem.getActiveTraces()}")
        if episodic_mem:
            print(f"  Episodic memory episodes: {episodic_mem.getEpisodeCount()}")

print("Advanced memory management simulation complete!")
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
