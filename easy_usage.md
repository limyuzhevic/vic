# NLM - Easy Usage Guide for Beginners (Enhanced)

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know (Enhanced)

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste) (Enhanced)

### Example 1: Simplest Brain (Enhanced)

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

### Example 2: Brain in a Simple World (Enhanced)

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

# Enable learning (NEW!)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    # Print statistics (NEW!)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, Curiosity: {agent.getCuriosityLevel():.3f}")

print("Simulation finished!")
```

---

## Simple Analogy (Enhanced)

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example (Enhanced)

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
print("Total spikes:", brain.getTotalSpikeCount())
```

---

## Common Patterns (Enhanced)

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

### Pattern 2: Get Brain Stats (Enhanced)

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("Curiosity:", agent.getCuriosityLevel())
print("Novelty:", agent.getNoveltyLevel())
print("Prediction error:", agent.getPredictionError())
```

### Pattern 3: Enable Brain Features (Enhanced - NEW!)

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enableNovelty(True)               # Detect interesting patterns
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)` (Enhanced)
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)
- Returns a `Brain` object with all neural systems

### `brain.initialize()` (Enhanced)
Starts up the brain. Always call this before using the brain!
- Initializes all neural components (memory, prediction, cognition, neuromodulation)
- Sets up synaptic connectivity
- Configures plasticity rules

### `brain.step(step_number)` (Enhanced)
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules
- Updates memory systems
- Applies neuromodulation effects
- Updates developmental state

### `world.update(time)` (Enhanced)
Updates the virtual world by `time` seconds.
- Advances world simulation
- Updates agent position and state
- Calculates rewards and penalties

### `agent.processSensoryInput(percept)` (Enhanced)
Gives sensory information (vision, touch, etc.) to the brain.
- Injects sensory data into sensory neurons
- Stores in working memory
- Updates prediction systems

### `agent.decodeMotorCommand()` (Enhanced)
Reads the brain's motor neurons to decide what action to take.
- Selects action based on motor neuron activity
- Maps neural activity to motor commands
- Returns `Action` object

### `pynlm.createSimpleWorld()` (Enhanced - NEW!)
Factory function to create a simple 2D world.
- Creates world with configurable dimensions
- Provides sensory inputs (vision, audio, touch)
- Manages agent position and movement
- Handles actions and results

### `pynlm.createAgentBrain(brain)` (Enhanced - NEW!)
Factory function to create an agent brain interface.
- Bridges brain and world
- Processes sensory inputs
- Decodes motor commands
- Manages neuromodulation
- Tracks learning and development

---

## Mini Projects (Enhanced)

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Project 2: Brain Watching a World (Enhanced)

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
agent.enableCuriosity(True)  # Add learning

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print("Curiosity:", agent.getCuriosityLevel())
```

### Project 3: Complete Agent (Enhanced - NEW!)

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

# Enable learning (NEW!)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableNovelty(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = agent.getCuriosityLevel()  # Simplified for example
    agent.applyRewardModulation(reward, 0.0)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
print("Final curiosity:", agent.getCuriosityLevel())
print("Final novelty:", agent.getNoveltyLevel())
```

---

## Quick Reference (Enhanced)

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Make brain learn | `agent.enableRewardModulation(True)` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Get stats | `brain.getFiringNeuronCount()`, etc. |

---

## Troubleshooting (Enhanced)

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try enabling learning: `agent.enableCuriosity(True)`
- Check if the world is providing sensory input

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Make sure the agent has energy: `agent.getCuriosityLevel()`

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Enable curiosity for exploration: `agent.enableCuriosity(True)`

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM with all the enhanced features.

---

## What's New in This Enhanced Version

1. **Rich Factory API**: New factory functions like `pynlm.createSimpleWorld()`
2. **Learning Features**: Built-in curiosity, novelty, and reward systems
3. **Enhanced Statistics**: More detailed brain and agent statistics
4. **Better Examples**: More comprehensive examples with learning enabled
5. **Input Validation**: Better error handling and user guidance
6. **Memory Systems**: Access to working, episodic, and associative memory
7. **Neuromodulation**: Direct access to curiosity, novelty, and reward systems

The enhanced version makes it much easier to get started with NLM and explore its full capabilities!
