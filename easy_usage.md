# NLM - Easy Usage Guide for Beginners

**An Experimental Artificial Developmental Brain**

## Quick Start

Welcome to NLM (Neural Learning Machine)! This is a brain-inspired spiking neural network simulator that learns from experience.

### What Makes NLM Special?

✅ **Real LIF Neuron Dynamics** - Leaky Integrate-and-Fire neurons with biological accuracy  
✅ **Full Integration** - All cognitive systems: Memory, Neuromodulation, Prediction, Cognition, Development  
✅ **Event-Driven Processing** - Spike propagation with synaptic delays  
✅ **Multiple Plasticity Rules** - STDP, Hebbian, and structural plasticity  
✅ **Development Stages** - Brain matures through developmental phases  
✅ **Python API** - Easy-to-use Python interface for rapid prototyping  

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

### Example 2: Enhanced Brain with Learning

```python
import pynlm

# Step 1: Make a brain with Phase 6 configuration (best for learning)
brain = pynlm.createBrain(pynlm.createPhase6Config())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think and learn for 500 steps
for i in range(500):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
print("Development stage:", brain.getDevelopmentalStage())
print("Learning achieved:", brain.getTotalSpikeCount() > 0)
```

### Example 3: Complete Agent with World Interaction

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create enhanced learning agent with all capabilities
agent = pynlm.createLearningAgent(brain, world,
                                 enableRewardModulation=True,
                                 enableStructuralPlasticity=True,
                                 enableDevelopment=True,
                                 enableCuriosity=True)

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
print("Novelty level:", agent.getNoveltyLevel())
```

### The Simplest Possible Example

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

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createLearningAgent(brain, world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent with Enhanced Learning

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createPhase6Config())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createLearningAgent(brain, world)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

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
print("Curiosity:", agent.getCuriosityLevel())
print("Novelty:", agent.getNoveltyLevel())
```

### Pattern 3: Enable Advanced Learning Systems

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Phase 6 includes all of the above by default
```

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createPhase6Config()` for best learning)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules
- Updates developmental stage

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try using `createPhase6Config()` for better learning
- Make sure the agent is enabled with all learning systems

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure `agent.initialize(world)` was called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Use `createPhase6Config()` for better initial conditions

**"No learning happening"**
- Make sure all plasticity systems are enabled
- Use `createLearningAgent()` helper function
- Check that reward signals are being generated

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Try `easy_usage_enhanced.md` for comprehensive examples
4. Experiment with different configurations!

That's it! You're now ready to use NLM.

