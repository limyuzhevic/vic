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

### `agent.applyRewardModulation(reward, predicted_reward)`
Applies learning based on reward prediction errors.

### `agent.updateDevelopment(timestep)`
Updates the developmental stage of the brain.

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

### Project 4: Working Memory Example

```python
import pynlm

# Create brain with memory support
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Check working memory
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Active traces: {working_memory.getActiveTraces()}")

# Run simulation to fill working memory
for step in range(500):
    brain.step(step)

# Check memory statistics
if working_memory:
    print(f"Working memory after simulation: {working_memory.getActiveTraces()} active traces")
```

### Project 5: Neuromodulation Example

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable neuromodulation
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation with learning
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Check neuromodulation levels
    dopamine = agent.getNeuromodulationLevel()
    curiosity = agent.getCuriosityLevel()
    novelty = agent.getNoveltyLevel()
    
    if step % 50 == 0:
        print(f"Step {step}: Dopamine={dopamine:.3f}, Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")

print("Neuromodulation simulation complete!")
```

### Project 6: Configuration Example

```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Query existing configuration options
print("Configuration options:")
for key in config.getKeys():
    print(f"  {key}")

# Modify configuration parameters
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.05)
config.set("plasticity.stdp.enable", True)

# Create brain with modified configuration
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Configured brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
```

### Project 7: Development Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable development
agent.enableDevelopment(True)

# Run long-term simulation to observe development
for step in range(5000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    agent.updateDevelopment(0.1)
    
    # Monitor developmental stage
    if step % 1000 == 0:
        stage = brain.getDevelopmentalStage()
        print(f"Step {step}: Developmental stage = {stage}")

print("Developmental simulation complete!")
```

### Project 8: Memory Replay Example

```python
import pynlm

# Create brain with episodic memory
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation to create episodic memories
for step in range(1000):
    brain.step(step)

# Access episodic memory
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    episode_count = episodic_memory.getEpisodeCount()
    print(f"Created {episode_count} episodic memories")
    
    # Get episodes for replay
    episodes_to_replay = episodic_memory.getEpisodesForReplay(5)
    print(f"Available for replay: {len(episodes_to_replay)} episodes")

# Save brain state
brain.save("nlm_checkpoint.bin")
print("Brain state saved to checkpoint")
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
| Apply reward learning | `agent.applyRewardModulation(reward, predicted)` |
| Update development | `agent.updateDevelopment(timestep)` |
| Enable features | `agent.enableRewardModulation(True)` |

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

**"My agent isn't learning"**
- Did you enable reward modulation with `agent.enableRewardModulation(True)`?
- Check that you have reward signals from the world

**"Memory systems aren't working"**
- Check that `brain.getWorkingMemory()` and `brain.getEpisodicMemory()` return valid objects
- Ensure you run enough simulation steps for memory formation

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more technical details about building from source
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations!**
4. **Try the Phase 6 demo** to see all systems working together:
   ```bash
   ./nlm_phase6_demo
   ```

5. **Look at the benchmark code** to understand performance characteristics
6. **Check out the experiment framework** for running scientific studies

---

## Python API Quick Reference

### Core Functions

- `pynlm.createDefaultConfig()` - Create default configuration
- `pynlm.createBrain(config)` - Create neural brain
- `pynlm.createAgentBrain(brain)` - Create agent interface
- `pynlm.createSimpleWorld()` - Create 2D simulation world

### Configuration Keys

Common configuration options include:
- `brain.neuron_count` - Total number of neurons
- `brain.synapse_density` - Synapse connectivity density
- `brain.connection_probability` - Probability of connection between neurons
- `plasticity.stdp.enable` - Enable STDP learning
- `plasticity.hebbian.enable` - Enable Hebbian learning
- `neuromod.dopamine.scale` - Dopamine modulation scale

### Memory Systems

- `brain.getWorkingMemory()` - Get working memory system
- `brain.getEpisodicMemory()` - Get episodic memory system
- `brain.getAssociativeMemory()` - Get associative memory system

### Development Stages

- `pynlm.DevelopmentalStage.Initial` - Initial stage
- `pynlm.DevelopmentalStage.CriticalPeriod` - Critical period
- `pynlm.DevelopmentalStage.Maturation` - Maturation stage
- `pynlm.DevelopmentalStage.Adult` - Adult stage

### Action Types

- `pynlm.ActionType.MoveForward` - Move forward
- `pynlm.ActionType.MoveBackward` - Move backward
- `pynlm.ActionType.TurnLeft` - Turn left
- `pynlm.ActionType.TurnRight` - Turn right
- `pynlm.ActionType.Interact` - Interact with object
- `pynlm.ActionType.Wait` - Do nothing

That's it! You're now ready to use NLM. The system is powerful but the basic concepts are simple: create a brain, put it in a world, give it sensory input, and let it learn through interaction.
