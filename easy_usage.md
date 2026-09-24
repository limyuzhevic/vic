# NLM - Easy Usage Guide for Beginners

**Updated for Phase 6: Final Integration**

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

**Phase 6 is the complete artificial brain** - all systems are now integrated and working together!

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (Phase 6: fully integrated brain)
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world (Phase 6: includes memory, neuromodulation, cognition)

---

## Quick Start (Copy & Paste) - Updated for Phase 6

### Example 1: Simplest Brain (Phase 6)

```python
import pynlm

# Step 1: Make a Phase 6 brain with integrated systems
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)  # Phase 6: larger brain
config.set("memory.working_memory_size", 1000)
config.set("memory.episodic_capacity", 100)
config.set("development.critical_period_duration", 100.0)

brain = pynlm.createBrain(config)

# Step 2: Turn it on (Phase 6: all subsystems initialize)
brain.initialize()

# Step 3: Make it think for 100 steps (Phase 6: memory updates, neuromodulation)
for i in range(100):
    brain.step(i)  # One step of thinking with full brain loop

print("Done! Your Phase 6 brain thought", brain.getTotalSpikeCount(), "times")
print("Memory traces:", brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0)
```

### Example 2: Complete Phase 6 Agent with Memory and Learning

```python
import pynlm

# Create everything with Phase 6 capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Phase 6: Enable all learning subsystems
agent.enableRewardModulation(True)      # Dopamine learning
agent.enableStructuralPlasticity(True)  # Brain growth
agent.enableDevelopment(True)           # Developmental stages
agent.enableCuriosity(True)             # Exploration drive

# Run for 50 steps (Phase 6: full integration demo)
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain processes with working memory
    brain.step(step)  # Phase 6: brain updates memory, prediction, attention
    action = agent.decodeMotorCommand()  # Brain decides action with neuromodulation
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    # Phase 6: Apply learning signals
    reward = 0.0  # Get reward from world
    agent.applyRewardModulation(reward, 0.0)
    agent.updateDevelopment(0.1)

print("Phase 6 simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Curiosity level:", agent.getCuriosityLevel())
print("Memory episodes:", brain.getEpisodicMemory() and brain.getEpisodicMemory().getEpisodeCount() or 0)
```

### Example 3: Phase 6 Learning Experiment

```python
import pynlm

# Create a learning environment with Phase 6 capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced Phase 6 features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run learning episode
for step in range(200):
    world.update(0.1)
    
    # Get sensory input (Phase 6: stores in working memory)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes with full Phase 6 integration
    brain.step(step)
    
    # Phase 6: Get action with planning and attention
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Phase 6: Learn from experience with prediction error
    reward = 0.0  # Simple reward
    agent.applyRewardModulation(reward, 0.0)
    
    # Phase 6: Development progresses
    agent.updateDevelopment(0.1)

print("Phase 6 learning complete!")
print(f"Development stage: {brain.getDevelopmentalStage()}")
print(f"Novelty level: {agent.getNoveltyLevel()}")
print(f"Prediction error: {agent.getPredictionError()}")
```

---

## Phase 6 Simple Analogy

| Real World | NLM Phase 6 Code |
|------------|------------------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` (with planning, attention) |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` (with dopamine, curiosity) |

---

## The Simplest Possible Example (Phase 6)

```python
import pynlm

# One line to create a Phase 6 virtual brain with all systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it (Phase 6: loads all integrated subsystems)
brain.initialize()

# Make it active (Phase 6: runs complete brain loop)
brain.step(0)

print("Your Phase 6 brain has", brain.getTotalNeuronCount(), "neurons!")
print("With working memory support, neuromodulation, and prediction systems.")
```

---

## Common Patterns - Updated for Phase 6

### Pattern 1: Run a Phase 6 Integration Test

```python
def run_phase6_integration(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees (Phase 6: processed into working memory)
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain (Phase 6: memory updates, prediction, attention)
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # 4. Get action from brain (Phase 6: planning with neuromodulation)
        motor_cmd = agent.decodeMotorCommand()
        
        # 5. Do action in world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # 6. Phase 6: Apply learning signals
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 7. Phase 6: Update development
        agent.updateDevelopment(0.1)

# Use it like this:
run_phase6_integration(brain, world, agent, 1000)
```

### Pattern 2: Phase 6 Brain Statistics

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())

# Phase 6: Memory statistics
if brain.getWorkingMemory():
    print("Working memory traces:", brain.getWorkingMemory().getActiveTraces())
if brain.getEpisodicMemory():
    print("Episodic episodes:", brain.getEpisodicMemory().getEpisodeCount())

# Phase 6: Neuromodulation statistics
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Prediction error:", agent.getPredictionError())
```

### Pattern 3: Enable Phase 6 Brain Features

```python
# These are all Phase 6 optional:
agent.enableRewardModulation(True)      # Dopamine-based learning
agent.enableStructuralPlasticity(True)  # Brain growth and pruning
agent.enableDevelopment(True)           # Developmental stages and maturation
agent.enableCuriosity(True)             # Exploration drive

# Phase 6: Additional statistics available
print("Neuromodulation level:", agent.getNeuromodulationLevel())
print("All Phase 6 integration systems are now connected!")
```

---

## What Each Part Does - Phase 6 Version

### `pynlm.createBrain(config)`
Creates a Phase 6 virtual brain with **ALL integrated systems**:
- Memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Prediction system
- Cognition (planning, attention, concepts)
- Development system
- Replay mechanism

### `brain.initialize()`
Starts up the Phase 6 brain with **complete subsystem initialization**.

### `brain.step(step_number)`
Makes the Phase 6 brain process one moment in time. The brain:
- Checks each neuron (LIF dynamics)
- Sends signals between connected neurons
- Updates working memory
- Applies neuromodulation effects
- Stores episodic memory
- Updates prediction system
- Applies plasticity rules (STDP, Hebbian)
- Applies structural plasticity
- Replays important memories
- Updates development
- Conserves memory

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the Phase 6 brain with working memory integration.

### `agent.decodeMotorCommand()`
Reads the Phase 6 brain activity to decide action, including planning and neuromodulation.

---

## Mini Projects - Phase 6 Edition

### Project 1: Phase 6 Silent Brain

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Phase 6 silent brain test done!")
print("Ready for sensory processing and learning.")
```

### Project 2: Phase 6 Brain Watching a World (Memory Integration)

```python
import pynlm

# Setup Phase 6 integrated brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps with Phase 6 memory
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)  # Phase 6: stores in episodic memory
    
print("Watched world for 30 steps with Phase 6 memory integration")
print("Firing rate:", brain.getAverageFiringRate())
print("Memory traces:", brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0)
```

### Project 3: Complete Phase 6 Agent (Full Integration Demo)

```python
import pynlm

# Setup Phase 6 integrated system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable ALL Phase 6 learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run Phase 6 simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)  # Phase 6: complete brain loop
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Development: {brain.getDevelopmentalStage()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Phase 6 agent simulation complete!")
print(f"Total episodes stored: {brain.getEpisodicMemory() and brain.getEpisodicMemory().getEpisodeCount() or 0}")
```

---

## Quick Reference - Phase 6

| What you want | Phase 6 Code |
|--------------|-------------|
| Create Phase 6 brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think (Phase 6) | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent (Phase 6) | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action (Phase 6) | `agent.decodeMotorCommand()` |

---

## Phase 6 Troubleshooting

**"My Phase 6 brain isn't showing learning"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if `agent.enableRewardModulation(True)` is called

**"The Phase 6 agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0 in Phase 6"**
- Brains need time to "warm up" - try more steps
- Phase 6 memory systems need input to store traces
- Make sure sensory input is connected

---

## Phase 6 Next Steps

When you're comfortable with Phase 6 integration:
1. Read `HOW_TO_USE.md` for more detailed Phase 6 configuration
2. Read `docs/ARCHITECTURE.md` to understand Phase 6 integration details
3. Read `docs/PHASE6_FEATURES.md` for complete feature list
4. Experiment with different Phase 6 configurations!

---

**You are now ready to use NLM Phase 6 - the complete artificial brain with all systems integrated!**

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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
