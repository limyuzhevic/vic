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
print("Excitatory/Inhibitory ratio:", brain.getExcitationInhibitionRatio())
```

### Pattern 3: Advanced Configuration

```python
# Create specialized configurations for different experiment types
if experiment_type == "learning":
    config = pynlm.createExperimentConfig("learning")
elif experiment_type == "structural":
    config = pynlm.createExperimentConfig("structural")
elif experiment_type == "cognitive":
    config = pynlm.createExperimentConfig("cognitive")
else:
    config = pynlm.createDefaultConfig()

brain = pynlm.createBrain(config)
brain.initialize()
```

### Pattern 4: Enable Advanced Features

```python
# These are all optional and available for advanced usage:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enablePredictionError(True)       # Learn from prediction errors
agent.enableNoveltyDetection(True)      # Detect surprising events
```

---

## What Each Part Does

### `pynlm.createDefaultConfig()`
Creates a basic configuration with sensible default settings for simple experiments.

### `pynlm.createExperimentConfig(experimentType)`
Creates a specialized configuration optimized for different types of experiments:
- "learning": Optimized for synaptic learning and plasticity experiments
- "structural": Optimized for structural plasticity and connectivity changes
- "cognitive": Optimized for cognitive processing and concept formation

### `pynlm.createConfigWithValues(values)`
Creates a custom configuration with specific key-value pairs.

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules
- Stores experiences in memory systems
- Applies neuromodulation effects
- Updates developmental state

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### `agent.enableFeature()`
Enables advanced brain features:
- `enableRewardModulation()`: Learn from rewards
- `enableStructuralPlasticity()`: Grow new connections
- `enableDevelopment()`: Brain matures over time
- `enableCuriosity()`: Explore new things
- `enablePredictionError()`: Learn from prediction errors
- `enableNoveltyDetection()`: Detect surprising events

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

### Project 2: Learning Experiment with Specialized Config

```python
import pynlm

# Create a specialized configuration for a learning experiment
config = pynlm.createExperimentConfig("learning")
brain = pynlm.createBrain(config)
brain.initialize()

# Run learning experiment
for i in range(200):
    brain.step(i)

print(f"Learning experiment: {brain.getTotalSpikeCount()} spikes")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
```

### Project 3: Advanced Agent with Full Control

```python
import pynlm

# Create specialized configurations
config = pynlm.createExperimentConfig("cognitive")

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run complete agent simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

    if step % 25 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"Stats: E/I ratio={brain.getExcitationInhibitionRatio():.2f}, " +
              f"Dev stage={brain.getDevelopmentalStage()}, " +
              f"Curiosity={agent.getCuriosityLevel():.2f}")

print("Complete agent simulation finished!")
```
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
| Create default config | `pynlm.createDefaultConfig()` |
| Create experiment config | `pynlm.createExperimentConfig("type")` |
| Enable learning | `agent.enableRewardModulation(True)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Make sure sensory input is connected (use `world.update()` and `agent.processSensoryInput()`)

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that motor neurons are properly configured

**"Advanced configuration issues"**
- Experiment types: "learning", "structural", "cognitive" - make sure you use the correct string
- Custom configuration: keys must match valid configuration parameter names

**"Memory system issues"**
- Working memory has limited capacity - check `brain.getWorkingMemory()`
- Episodic memory has maximum episode limits - check `brain.getEpisodicMemory()`
- Memory consolidation requires time and proper neuromodulation

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more technical details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations using `createExperimentConfig()`
4. Try the advanced agent features with `agent.enableFeature()`
5. Read `docs/PHASE6_FINAL_AUDIT.md` for details about the latest improvements
6. Read `docs/SCIENCE.md` for the scientific background and mathematical foundations

**New Features to Explore:**
- Advanced configuration for learning, structural, and cognitive experiments
- Novelty detection and curiosity-driven exploration
- Memory consolidation and replay systems
- Developmental stages and plasticity modulation
- Reward-modulated learning and prediction errors
- Inter-region connectivity and communication

That's it! You're now ready to use NLM with both basic and advanced functionality.
