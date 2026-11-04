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

### Project 5: Enhanced Reward System

```python
import pynlm

# Create enhanced reward system
reward = pynlm.createReward()

# Get observation from world
observation = world.getSensoryPercept()

# Compute environmental reward (requires implementation in SimpleWorld)
env_reward = reward.computeEnvironmentalReward(observation)
print(f"Environmental reward: {env_reward}")

# Compute integrated reward with neuromodulation
neuromodulation_level = agent.getNeuromodulationLevel()
integrated_reward = reward.computeIntegratedReward(observation, neuromodulation_level)
print(f"Integrated reward: {integrated_reward}")

# Use reward in learning
agent.applyRewardModulation(integrated_reward, 0.0)
```

### Project 6: Advanced Learning with Plasticity Rules

```python
import pynlm

# Create brain and plasticity rules
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create and configure plasticity rules
stdp = pynlm.createSTDP()
stdp.configure(ltpWeight=0.015, ltdWeight=0.018, tau=35.0)

hebbian = pynlm.createHebbian()
hebbian.setLearningRate(0.015)

# Run simulation with enhanced plasticity
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Project 7: Neuromodulation Integration

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access neuromodulation system
neuromodulator = brain.getNeuromodulator()

# Get neuromodulators
dopamine = neuromodulator.getDopamine()
curiosity = neuromodulator.getCuriosity()
novelty = neuromodulator.getNovelty()

# Modify neuromodulation levels
dopamine.setLevel(0.5)
curiosity.setLevel(0.3)
novelty.setLevel(0.2)

# Signal rewards
neuromodulator.signalReward(0.8)
neuromodulator.signalRewardPredictionError(0.1)
```

### Project 8: Memory Systems

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

# Store and recall memories
working_memory.addItem("observation", 0.8)
working_memory.addItem("action", 0.9, priority=1)

episode = {"sensory": "vision_data", "action": "move_forward", "reward": 0.5}
episodic_memory.storeEpisode(episode)

# Use memory for learning
recall = episodic_memory.recallEpisode(0)
```

### Project 9: Development System

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access development system
development = brain.getDevelopmentSystem()

# Update developmental stage
for step in range(200):
    development.updateStage(0.1)  # Update per timestep
    current_stage = development.getCurrentStage()
    
    # Set developmental stage based on simulation time
    if step == 50:
        development.setStage(pynlm.DevelopmentalStage.Maturation)
    elif step == 100:
        development.setStage(pynlm.DevelopmentalStage.Adult)
    elif step == 150:
        development.setStage(pynlm.DevelopmentalStage.Aging)
```

---

## Advanced Features Overview

### Enhanced Neural Dynamics

The NLM neural dynamics have been significantly improved:

1. **Optimized Spike Rate Control**: 
   - Higher neuron threshold (-52mV vs -55mV)
   - Extended refractory period (15-25 steps vs 9 steps)
   - Slower membrane time constant (50-75ms vs 20ms)
   - Stronger leak conductance (15nS vs 10nS)

2. **Spike-Frequency Adaptation**:
   - Post-spike hyperpolarization
   - Adaptation variable tracking
   - Dynamic refractory period extension

### Enhanced Learning Mechanisms

1. **Improved STDP**: 
   - Pattern selectivity analysis
   - Temporal consistency bonuses
   - Longer integration window (35ms vs 20ms)
   - Weight-dependent soft bounds

2. **Enhanced Hebbian Learning**:
   - Exploration-exploitation balance
   - Pattern consistency rewards
   - Underrepresented synapse bonuses

3. **Comprehensive Neuromodulation**:
   - Reward-modulated plasticity
   - Curiosity-driven exploration
   - Novelty detection
   - Prediction error signaling

### Improved Reward System

1. **Environmental Reward Computation**:
   - Vision-based feature analysis
   - Internal signal stability rewards
   - Audio-based activity monitoring
   - Active perception incentives

2. **Integrated Neuromodulatory Rewards**:
   - Dopamine-modulated reward sensitivity
   - Baseline reward persistence
   - History-based reward trends

### Complete Memory Systems

1. **Working Memory**:
   - Persistent activity competition
   - Priority-based item management
   - Decay and consolidation mechanisms

2. **Episodic Memory**:
   - Experience encoding
   - Replay and consolidation
   - Sleep/rest cycle integration

### Python Bindings Enhancement

The Python bindings now include:

- **Reward system** for reinforcement learning
- **STDP and Hebbian plasticity rules**
- **Neuromodulation system** (Dopamine, Curiosity, Novelty)
- **Memory systems** (Working Memory, Episodic Memory)
- **Prediction system** for forward models
- **Development system** for lifespan learning
- **Enhanced agent interface** with advanced learning features

### CLI Improvements (Coming Soon)

Phase 6 demo executables now include:
- `nlm_demo_simple` - Basic simulation with enhanced reward
- `nlm_demo_advanced` - Full integration with all systems
- `nlm_demo_neuromodulation` - Neuromodulation-focused learning
- `nlm_demo_development` - Developmental stage learning
- `nlm_demo_memory` - Memory system integration

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more detailed C++ usage
2. Read `docs/ARCHITECTURE.md` to understand the system design
3. Experiment with different configurations!
4. Explore the new Python bindings for enhanced learning features
5. Try the CLI demo executables for advanced usage

That's it! You're now ready to use NLM.
