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

### `agent.decodeMotorCommand()`
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |

---

## Brain Systems Access

### Memory Systems

```python
# Access integrated memory systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Working memory - stores active neural traces
working_memory = brain.getWorkingMemory()
if working_memory:
    print(f"Working memory capacity: {working_memory.getCapacity()}")
    print(f"Active traces: {working_memory.getActiveTraces()}")

# Episodic memory - stores experiences
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    print(f"Max episodes: {episodic_memory.getMaxEpisodes()}")
    print(f"Stored episodes: {episodic_memory.getEpisodeCount()}")

# Associative memory - pattern associations
associative_memory = brain.getAssociativeMemory()
if associative_memory:
    print("Associative memory available for pattern matching")
```

### Prediction System

```python
# Access prediction system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

prediction_system = brain.getPredictionSystem()
if prediction_system:
    print("Prediction system for next-state prediction")
    # Predict next sensory state
    predicted = prediction_system.predictNextState()
    print(f"Predicted state: {predicted}")
```

### Cognition Systems

```python
# Access cognitive systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Neural planner for action planning
planner = brain.getPlanner()
if planner:
    print("Neural planner available for action sequences")

# Concept formation for pattern discovery
concept_formation = brain.getConceptFormation()
if concept_formation:
    print("Concept formation available for discovering patterns")

# Attention system for focus selection
attention = brain.getAttention()
if attention:
    print("Attention system available for competitive selection")
```

### Neuromodulation Systems

```python
# Access neuromodulation systems
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Dopamine system for reward modulation
dopamine = brain.getDopamine()
if dopamine:
    print(f"Dopamine level: {dopamine.getLevel():.3f}")
    print("Regulates neural excitability and plasticity")

# Curiosity system for exploration motivation
curiosity = brain.getCuriosity()
if curiosity:
    print(f"Curiosity level: {curiosity.getLevel():.3f}")
    print("Drives exploration of novel stimuli")

# Novelty detection system
novelty = brain.getNovelty()
if novelty:
    print("Novelty detection system available")

# Prediction error system
prediction_error = brain.getPredictionErrorSignal()
if prediction_error:
    print(f"Prediction error: {prediction_error.getError():.3f}")
```

### Development System

```python
# Access development system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

development_system = brain.getDevelopmentSystem()
if development_system:
    print(f"Developmental stage: {brain.getDevelopmentalStage().name}")
    
    # Development affects plasticity rates
    for stage in ["Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"]:
        brain.setDevelopmentalStage(getattr(pynlm, f"DevelopmentalStage.{stage}"))
        dev_stage = brain.getDevelopmentSystem()
        if dev_stage:
            print(f"{stage}: Plasticity rate = {dev_stage.getPlasticityRate():.3f}")
```

### Random Generator

```python
# Access random number generator
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

rng = brain.getRandomGenerator()
if rng:
    print("Random generator available")
    # Generate random numbers for experiments
    for i in range(5):
        random_val = rng.generateFloat()
        print(f"Random: {random_val:.3f}")
```

---

## Mini Projects with Integrated Systems

### Project 4: Memory-Enhanced Agent

```python
import pynlm

# Create agent with memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Connect memory to agent
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Memory-enhanced learning
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Monitor memory systems
    if step % 100 == 0:
        working_mem = brain.getWorkingMemory()
        episodic_mem = brain.getEpisodicMemory()
        curiosity = agent.getCuriosityLevel()
        print(f"Step {step}: WorkingMem={working_mem.getActiveTraces() if working_mem else 0}, "
              f"Episodes={episodic_mem.getEpisodeCount() if episodic_mem else 0}, "
              f"Curiosity={curiosity:.3f}")

print("Memory-enhanced agent simulation complete!")
```

### Project 5: Prediction-Based Agent

```python
import pynlm

# Create prediction-based agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Run with prediction system
prediction_system = brain.getPredictionSystem()
if prediction_system:
    print("Prediction system enabled - agent will learn expected outcomes")

for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Get prediction error to monitor learning
    prediction_error = agent.getPredictionError()
    if step % 100 == 0:
        print(f"Step {step}: Prediction error = {prediction_error:.3f}")

print("Prediction-based agent simulation complete!")
```

### Project 6: Developmental Agent

```python
import pynlm

# Create developmental agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=12, height=12, visionWidth=6, visionHeight=6)
world.reset()
agent.initialize(world)

# Enable development
agent.enableDevelopment(True)

# Monitor development progression
print("Starting developmental simulation...")
for step in range(400):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Monitor development stage
    if step % 100 == 0:
        dev_stage = brain.getDevelopmentalStage()
        firing_rate = brain.getAverageFiringRate()
        print(f"Step {step}: {dev_stage.name}, Firing={firing_rate:.3f}")

print("Developmental agent simulation complete!")
```

---

## Complete Brain API Reference

### Core Brain Methods
| Method | Description |
|--------|-------------|
| `brain.initialize()` | Initialize brain with configuration |
| `brain.step(step)` | Process one simulation step |
| `brain.step(step, time)` | Process step with timestamp |
| `brain.receiveSensoryInput(input)` | Inject sensory data into brain |
| `brain.injectCurrent(neuron, current)` | Inject current into specific neuron |
| `brain.injectCurrentToNeurons(type, current)` | Inject current to all neurons of type |
| `brain.produceAction()` | Generate motor action from neural activity |
| `brain.reset()` | Reset brain to initial state |
| `brain.save(filepath)` | Save brain state to file |
| `brain.load(filepath)` | Load brain state from file |

### Memory System Access
| Method | Description |
|--------|-------------|
| `brain.getWorkingMemory()` | Get working memory system |
| `brain.getEpisodicMemory()` | Get episodic memory system |
| `brain.getAssociativeMemory()` | Get associative memory system |

### Prediction and Cognition Systems
| Method | Description |
|--------|-------------|
| `brain.getPredictionSystem()` | Get prediction system |
| `brain.getPlanner()` | Get neural planner |
| `brain.getConceptFormation()` | Get concept formation system |
| `brain.getAttention()` | Get attention system |

### Development and Neuromodulation Systems
| Method | Description |
|--------|-------------|
| `brain.getDevelopmentSystem()` | Get development system |
| `brain.getDopamine()` | Get dopamine system |
| `brain.getCuriosity()` | Get curiosity system |
| `brain.getNovelty()` | Get novelty system |
| `brain.getPredictionErrorSignal()` | Get prediction error system |

### Agent Methods
| Method | Description |
|--------|-------------|
| `agent.processSensoryInput(percept)` | Process sensory percept |
| `agent.decodeMotorCommand()` | Decode brain activity into motor command |
| `agent.applyRewardModulation(reward, predicted)` | Apply reward-based learning |
| `agent.updateDevelopment(timestep)` | Update development system |
| `agent.getCuriosityLevel()` | Get curiosity level |
| `agent.getNoveltyLevel()` | Get novelty level |

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
| Get working memory | `brain.getWorkingMemory()` |
| Get episodic memory | `brain.getEpisodicMemory()` |
| Get prediction system | `brain.getPredictionSystem()` |
| Get planner | `brain.getPlanner()` |
| Get concept formation | `brain.getConceptFormation()` |
| Get attention | `brain.getAttention()` |
| Get development system | `brain.getDevelopmentSystem()` |
| Get dopamine | `brain.getDopamine()` |
| Get curiosity | `brain.getCuriosity()` |

---

## Advanced Usage

### Integrating Multiple Systems

```python
import pynlm

# Create integrated brain with all systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with all systems integrated
for step in range(1000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check all integrated systems
    if step % 200 == 0:
        print(f"=== Step {step} ===")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Working memory traces: {brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else 0}")
        print(f"  Stored episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
        print(f"  Prediction error: {agent.getPredictionError():.3f}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage().name}")

print("\n=== Integrated Brain Simulation Complete ===")
print("All brain systems (memory, prediction, cognition, neuromodulation, development) are working together!")
```

---

## Troubleshooting

**"My brain memory systems aren't working"**
- Check that you called `brain.initialize()` before accessing memory systems
- Ensure memory systems are properly integrated by checking `brain.getWorkingMemory() != None`

**"The prediction system isn't learning"**
- Make sure the prediction system exists: `if brain.getPredictionSystem():`
- Check that agent is providing sensory input to train predictions

**"Development isn't progressing"**
- Enable development: `agent.enableDevelopment(True)`
- Monitor development stage with `brain.getDevelopmentalStage()`

**"Everything is 0"**
- Brains need time to "warm up" - try more steps (1000+ for full development)
- Some neurons need input to fire - make sure sensory input is connected
- Check that memory systems have capacity: `brain.getWorkingMemory().setCapacity()`

---

## Next Steps

When you're comfortable with the integrated systems:

1. **Read `HOW_TO_USE.md`** for more advanced usage and configuration options
2. **Read `docs/ARCHITECTURE.md`** to understand how the integrated systems work together
3. **Experiment with different configurations** to see how development affects different brain systems
4. **Create your own experiments** combining multiple systems for complex behaviors
5. **Check the Phase 6 integration tests** in the C++ code for examples of fully integrated brain systems

The NLM brain now functions as a **coherent artificial brain** with all major systems integrated:

- **Working Memory**: Transient storage of active neural patterns
- **Episodic Memory**: Storage of experiences and events
- **Associative Memory**: Pattern associations and connections
- **Prediction System**: Forward models and error signals
- **Neural Planner**: Action sequence evaluation and planning
- **Concept Formation**: Pattern discovery and abstraction
- **Attention System**: Competitive selection and focus
- **Development System**: Age-based plasticity and maturation
- **Dopamine System**: Reward-based modulation of learning
- **Curiosity System**: Exploration motivation
- **Novelty Detection**: Surprise-based learning
- **Prediction Error**: Mismatch-based learning signals

This is the foundation for investigating emergence, learning, and adaptation in a brain-like system!

That's it! You're now ready to use NLM with all its integrated cognitive systems.

