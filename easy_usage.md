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

### Example 1: Simplest Brain with Learning (NEW!)

```python
import pynlm

# Step 1: Make a brain with learning enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Enable neuromodulation for learning
agent = pynlm.createAgentBrain(brain)
agent.initialize()
agent.enableRewardModulation(True)  # Learning from rewards
agent.enableCuriosity(True)         # Exploration

# Step 3: Make it think for 100 steps with learning
for i in range(100):
    brain.step(i)  # One step of thinking with dopamine learning

print("Done! Your brain learned through experience")
```

### Example 2: Brain in a Simple World with Development (NEW!)

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

# Enable advanced features
agent.enableRewardModulation(True)     # Reward-based learning
agent.enableStructuralPlasticity(True) # Growing new connections
agent.enableDevelopment(True)          # Brain matures over time
agent.enableCuriosity(True)             # Explore novel things

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
print("Curiosity level:", agent.getCuriosityLevel())
print("Development stage:", brain.getDevelopmentStage())
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
| Your brain matures over time | `agent.updateDevelopment(dt)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain with learning
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation with Learning (UPDATED)

```python
def run_simulation_with_learning(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks with neuromodulation
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward modulation (learning)
        reward = world.getSensoryPercept().getEnergy() if world.getSensoryPercept() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Update development
        agent.updateDevelopment(0.1)

# Use it like this:
run_simulation_with_learning(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats with Advanced Features (UPDATED)

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Prediction error:", agent.getPredictionError())
```

### Pattern 3: Enable Advanced Learning Features (NEW!)

```python
# These are all optional but recommended for learning:
agent.enableRewardModulation(True)      # Learn from rewards using dopamine
agent.enableStructuralPlasticity(True)  # Grow new connections (synaptogenesis)
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enablePrediction(True)           # Forward modeling
```

---

## What Each Part Does (UPDATED)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses, equipped with neuromodulation systems.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron (LIF dynamics)
- Sends signals between connected neurons (spike propagation)
- Updates connections based on learning rules (STDP, Hebbian, structural)
- Applies neuromodulation (dopamine, curiosity, reward prediction error)
- Updates developmental stage

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain and updates brain state.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

### `agent.applyRewardModulation(reward, predicted)`
Applies reward prediction error to neuromodulation systems for learning.

### `agent.updateDevelopment(dt)`
Updates brain development for age-dependent plasticity.

### `agent.enableRewardModulation(True/False)`
Enable/disable reward-based learning with dopamine.

### `agent.enableCuriosity(True/False)`
Enable/disable curiosity-driven exploration behavior.

### `agent.enableDevelopment(True/False)`
Enable/disable developmental stages and age-dependent changes.

---

## Mini Projects

### Project 1: Silent Brain with Learning (NEW!)

```python
import pynlm

# Create brain with learning enabled
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Enable neuromodulation
agent = pynlm.createAgentBrain(brain)
agent.initialize()
agent.enableRewardModulation(True)

# Simulate with learning
for i in range(10):
    brain.step(i)

print("Brain learned through experience!")
```

### Project 2: Brain Watching a World with Development (UPDATED)

```python
import pynlm

# Setup with advanced features
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    # Get action and apply reward
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    reward = world.getSensoryPercept().getEnergy() if world.getSensoryPercept() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print("Curiosity:", agent.getCuriosityLevel())
print("Development stage:", brain.getDevelopmentStage())
```

### Project 3: Complete Agent with All Features (UPDATED)

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

# Enable ALL advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
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
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

print("Agent simulation complete!")
```

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain with learning | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Apply learning | `agent.applyRewardModulation(reward, predicted)` |

---

## Troubleshooting

**"My brain isn't learning"**
- Did you call `agent.enableRewardModulation(True)`?
- Make sure you're calling `agent.applyRewardModulation()` with reward values

**"The agent's isn't exploring"**
- Did you call `agent.enableCuriosity(True)`?

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Make sure reward modulation is enabled
- Development stages take time to mature

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM with all its advanced learning features.
