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

### Expert Features: Advanced Brain Configuration

For more advanced users, here are some advanced configuration options:

```python
import pynlm

# Create advanced configuration
config = pynlm.createDefaultConfig()

# Configure advanced parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15f)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 2.0f)
config.set("neuromod.curiosity.enable", True)
config.set("neuromod.novelty.enable", True)
config.set("development.initial_plasticity_rate", 0.8f)
config.set("memory.working_memory.capacity", 500)
config.set("memory.episodic_memory.max_episodes", 5000)

# Create brain with advanced configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

### Expert Feature: Custom Replay and Sleep Cycle

```python
import pynlm

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Custom simulation with sleep/rest cycle
for step in range(1000):
    # Regular simulation
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward
    reward = 0.1f  # Simple reward signal
    agent.applyRewardModulation(reward, 0.0f)
    
    # Enter sleep/rest cycle for consolidation (every 500 steps)
    if step % 500 == 0:
        print("Entering sleep/rest cycle for memory consolidation...")
        
        # Get best memories for replay
        workingMem = brain.getWorkingMemory()
        if workingMem:
            workingMem.consolidate()
            print("Working memory consolidated")
        
        episodicMem = brain.getEpisodicMemory()
        if episodicMem:
            # Replay important episodes
            episodes = episodicMem.getEpisodesForReplay(3)
            for episode in episodes:
                episodicMem.replayEpisode(episode)
            print(f"Replayed {len(episodes)} important episodes")
        
        # Enter resting state
        brain.setDevelopmentStage(pynlm.DevelopmentalStage.Maturation)
        print("Development stage: Maturation")
    
    # Log progress
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
```

### Expert Feature: Brain Checkpointing and Persistence

```python
import pynlm
import os

# Create and train brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate for a while
for step in range(500):
    # Add some sensory input
    if step % 100 == 0:
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
    
    brain.step(step)

# Save brain state (checkpointing)
checkpoint_dir = "./brain_checkpoints"
os.makedirs(checkpoint_dir, exist_ok=True)
checkpoint_file = os.path.join(checkpoint_dir, "trained_brain.bin")

if brain.save(checkpoint_file):
    print(f"Brain saved successfully to {checkpoint_file}")
else:
    print("Failed to save brain state")

# Later: Load saved brain
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.initialize()

if loaded_brain.load(checkpoint_file):
    print(f"Brain loaded successfully from {checkpoint_file}")
    print(f"Loaded brain has {loaded_brain.getTotalNeuronCount()} neurons")
    print(f"Loaded brain has {loaded_brain.getTotalSynapseCount()} synapses")
else:
    print("Failed to load brain state")
```

### Expert Feature: Advanced Planning and Prediction

```python
import pynlm

# Create brain with advanced planning
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("prediction.enable", True)
config.set("planning.depth", 5)

brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width(20, height=20, visionWidth=8, visionHeight=8))
world.reset()
agent.initialize(world)

# Run simulation with planning
for step in range(200):
    world.update(0.1)
    
    # Get current percept
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes with prediction
    brain.step(step)
    
    # Get predicted next state
    if brain.getPredictionSystem():
        prediction = brain.getPredictionSystem().predictNextState(percept)
    else:
        prediction = None
    
    # Plan action with prediction
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from prediction error if prediction system exists
    if brain.getPredictionSystem() and prediction:
        error = brain.getPredictionSystem().getPredictionError()
        if error > 0.1f:
            print(f"High prediction error at step {step}: {error:.3f}")
    
    print(f"Step {step}: Action={action}, Firing={brain.getFiringNeuronCount()}")
```

### Expert Feature: Memory Management and Analysis

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Simulate
for step in range(1000):
    # Simple behavior
    brain.step(step)
    
    if step % 50 == 0:
        # Analyze memory systems
        workingMem = brain.getWorkingMemory()
        episodicMem = brain.getEpisodicMemory()
        
        if workingMem:
            activity = workingMem.getMemoryActivity()
            activeTraces = workingMem.getActiveTraces()
            print(f"Step {step}: Working Memory - Activity: {activity:.3f}, Traces: {activeTraces}")
        
        if episodicMem:
            episodeCount = episodicMem.getEpisodeCount()
            avgReward = episodicMem.getAverageReward()
            print(f"Step {step}: Episodic Memory - Episodes: {episodeCount}, Avg Reward: {avgReward:.3f}")
        
        # Get neuromodulation levels
        dopamine = agent.getNeuromodulationLevel()
        curiosity = agent.getCuriosityLevel()
        novelty = agent.getNoveltyLevel()
        
        print(f"  Neuromodulation - Dopamine: {dopamine:.3f}, Curiosity: {curiosity:.3f}, Novelty: {novelty:.3f}")
        
        # Check development stage
        stage = brain.getDevelopmentalStage()
        print(f"  Development Stage: {stage}")
```

### Expert Feature: Custom Brain Development

```python
import pynlm

# Create brain with custom development
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Track development progress
for step in range(1500):
    brain.step(step)
    
    if step % 100 == 0:
        stage = brain.getDevelopmentalStage()
        print(f"Step {step}: Development Stage = {stage}")
        
        # Manually progress development
        if step >= 1000:
            brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Adult)
            print("Transitioned to Adult stage - reduced plasticity")
        
        # Check structural plasticity effects
        if brain.getStructuralPlasticity():
            print(f"  Synaptogenesis rate: {brain.getStructuralPlasticity().getSynaptogenesisRate():.6f}")
            print(f"  Pruning rate: {brain.getStructuralPlasticity().getPruningRate():.6f}")
```

### Expert Feature: Advanced Agent Brain Customization

```python
import pynlm

# Create brain and agent
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.setAgentStart(7.5, 7.5)
world.reset()
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Custom action selection strategy
def advanced_action_selection(agent, brain, world):
    # Get current percept
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(0)
    
    # Get raw motor command
    action = agent.decodeMotorCommand()
    
    # Apply advanced modifications based on context
    if agent.getCuriosityLevel() > 0.7:
        # High curiosity = more exploration
        action = pynlm.ActionType.Look
        print("High curiosity - looking around")
    elif agent.getNeuromodulationLevel() < 0.2:
        # Low neuromodulation = default/structured behavior
        action = pynlm.ActionType.Rest
        print("Low neuromodulation - resting")
    
    return action

# Run advanced simulation
for step in range(100):
    world.update(0.1)
    
    # Use custom action selection
    action = advanced_action_selection(agent, brain, world)
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward based on action success
    reward = 0.0f
    if action == pynlm.ActionType.Interact:
        reward = 1.0f  # Reward for interaction
    elif action == pynlm.ActionType.Eat:
        reward = 2.0f  # Higher reward for eating
    
    agent.applyRewardModulation(reward, 0.0f)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 20 == 0:
        print(f"Step {step}: Action={action}, Reward={reward}, Firing={brain.getFiringNeuronCount()}")
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
