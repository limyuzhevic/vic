# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

**NEW in Phase 4-6:** Now you can create brains with memory, prediction, and advanced cognitive abilities!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns
- **Prediction** = The brain anticipating what will happen next
- **Cognition** = The brain thinking, planning, and making decisions

That's it! NLM simulates all of this with cutting-edge Phase 4-6 features!

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (now with memory and prediction!)
2. **World** - The environment the brain lives in (now with physics and hazards!)
3. **Agent** - The bridge connecting brain to world (now with advanced planning!)

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain (Basic)

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

### Example 2: Advanced Brain with Phase 4-6 Features

```python
import pynlm

# NEW: Create a brain with advanced capabilities
config = pynlm.createDefaultConfig()

# Enable advanced features
config.set("brain.enableWorkingMemory", True)
config.set("brain.enableEpisodicMemory", True)
config.set("brain.enablePrediction", True)
config.set("brain.enableCuriosity", True)
config.set("brain.enableDevelopment", True)

# Create advanced brain
brain = pynlm.createBrain(config)
brain.initialize()

# Simulate with advanced processing
for i in range(100):
    brain.step(i)  # Advanced brain with memory and prediction
    
    # Update working memory (Phase 4)
    brain.updateWorkingMemory()
    
    # Update episodic memory (Phase 4)
    brain.updateEpisodicMemory()
    
    # Update prediction (Phase 4)
    brain.updatePrediction()
    
    if i % 20 == 0:
        print(f"Step {i}: Brain with {brain.getFiringNeuronCount()} neurons")
        print(f"  Memory: {brain.getWorkingMemoryUsage()}")
        print(f"  Experience: {brain.getEpisodicMemoryCount()} episodes")
```

```

### Example 2: Brain in an Enhanced World with Phase 6

```python
import pynlm

# Create everything with Phase 6 enhanced capabilities
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# NEW: Create enhanced environment with physics and hazards
world = pynlm.createRealEnvironment()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable ALL advanced systems
agent.enableWorkingMemory(True)
agent.enableEpisodicMemory(True)
agent.enablePrediction(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run for 50 steps with enhanced capabilities
for step in range(50):
    world.update(0.1)  # Update world with physics
    
    percept = world.getSensoryPercept()  # What does the brain see?
    agent.processSensoryInput(percept)  # Brain processes it
    brain.step(step)  # Brain thinks (with memory and prediction)
    
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Memory: {brain.getWorkingMemoryUsage()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")

print("Enhanced simulation finished!")
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

---

## NEW: Common Patterns with Phase 4-6 Features

### Pattern 1: Run Advanced Simulation

```python
def run_advanced_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world with physics simulation
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain (with Phase 4 cognitive systems)
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks with memory and prediction
        brain.step(step)
        brain.updateWorkingMemory()
        brain.updateEpisodicMemory()
        brain.updatePrediction()
        
        # 5. Get action with advanced planning
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_advanced_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Enhanced Brain Stats (Phase 4-6)

```python
# Add these inside your loop for advanced monitoring:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Working memory usage:", brain.getWorkingMemoryUsage())
print("Episodic memory count:", brain.getEpisodicMemoryCount())
print("Prediction confidence:", agent.getPredictionConfidence())
print("Curiosity level:", agent.getCuriosityLevel())
print("Development stage:", brain.getDevelopmentalStage())
print("Environmental stress:", world.getEnvironmentalStress())
```

### Pattern 3: Enable All Advanced Features

```python
# These are the enhanced Phase 4-6 features you can enable:
agent.enableWorkingMemory(True)      # Phase 4: Remember things
agent.enableEpisodicMemory(True)     # Phase 4: Store experiences
agent.enablePrediction(True)         # Phase 4: Anticipate future
agent.enableCuriosity(True)          # Phase 6: Explore and learn
agent.enableDevelopment(True)       # Phase 6: Mature over time
agent.enableNeuromodulation(True)    # Phase 6: Reward and motivation
agent.enableStructuralPlasticity(True)  # Phase 6: Grow new connections
agent.enableAblationSystem(True)     # Phase 5: Research tool
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)`
Creates a virtual brain with **Phase 4-6 advanced capabilities**.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain with advanced cognitive systems.

### `brain.step(step_number)`
Makes the brain process one moment in time with **memory and prediction**.
- **Phase 4 Working Memory**: Maintains information across steps
- **Phase 4 Episodic Memory**: Stores experiences for later recall
- **Phase 4 Prediction**: Anticipates future states and actions

### `world.update(time)`
Updates the **enhanced world** with Phase 6 physics simulation.

### `agent.processSensoryInput(percept)`
Gives sensory information to the brain with **advanced cognitive processing**.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take with **Phase 4-6 planning**.

---

## NEW: Mini Projects (Phase 4-6)

### Project 1: Silent Advanced Brain (With Memory)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Enable advanced features
brain.enableWorkingMemory(True)
brain.enableEpisodicMemory(True)
brain.enablePrediction(True)

brain.initialize()

for i in range(10):
    brain.step(i)
    brain.updateWorkingMemory()
    brain.updateEpisodicMemory()
    brain.updatePrediction()

print("Advanced silent brain test done!")
```

### Project 2: Brain Watching an Enhanced World

```python
import pynlm

# Setup with Phase 6 enhanced world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createRealEnvironment()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced systems
agent.enableWorkingMemory(True)
agent.enableEpisodicMemory(True)
agent.enablePrediction(True)
agent.enableCuriosity(True)

# Watch the world with advanced capabilities
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    # Update advanced cognitive systems
    brain.updateWorkingMemory()
    brain.updateEpisodicMemory()
    brain.updatePrediction()
    
    if i % 10 == 0:
        print(f"Watched world: {brain.getWorkingMemoryUsage()} memory units")

print("Watched enhanced world with Phase 4-6 features!")
```

### NEW: Project 3: Complete Advanced Agent

```python
import pynlm

# Setup with ALL Phase 4-6 capabilities
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createRealEnvironment()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable ALL learning and cognitive systems
agent.enableWorkingMemory(True)
agent.enableEpisodicMemory(True)
agent.enablePrediction(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableNeuromodulation(True)
agent.enableStructuralPlasticity(True)

# Run agent with ALL capabilities
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Update advanced systems
    brain.updateWorkingMemory()
    brain.updateEpisodicMemory()
    brain.updatePrediction()
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: Advanced agent with all Phase 4-6 features")
        print(f"  Memory: {brain.getWorkingMemoryUsage()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
        print(f"  Development: {brain.getDevelopmentalStage()}")

print("Advanced agent simulation complete with ALL Phase 4-6 features!")
```
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

**"My brain isn't doing anything advanced"**
- Did you call `brain.initialize()`?
- Did you enable the advanced features? `brain.enableWorkingMemory(True)`
- Try increasing the number of steps

**"The agent isn't learning"**
- Did you call `world.applyMotorCommand()`?
- Did you enable curiosity and development? `agent.enableCuriosity(True)`
- Check that `world.update()` is being called

**"Everything is 0 (memory/prediction)"**
- Advanced systems need time to warm up - try more steps
- Make sure sensory input is connected to learning systems
- Enable all advanced features before running simulation

---

## NEW: Next Steps

When you're comfortable with the advanced features:

1. **Read `HOW_TO_USE.md`** for detailed Phase 4-6 implementation
2. **Read `docs/ARCHITECTURE.md`** to understand Phase 4-6 integration
3. **Read `docs/SCIENCE.md`** to understand the biological basis
4. **Experiment with different configurations** to discover Phase 4-6 capabilities
5. **Try Phase 6 experiments** for complete integration demos
6. **Use the advanced testing tools** to validate Phase 4-6 performance

---

That's it! You're now ready to use NLM with **ALL Phase 4-6 advanced features**.

**You now have access to:**
- Phase 4: Working memory, episodic memory, prediction, attention, planning, self-model, social learning
- Phase 5: Performance optimizations, memory pools, parallel processing, SIMD vectorization
- Phase 6: Enhanced motor system, real physics environment, comprehensive error handling
- Modern C++17 features and enhanced Python bindings
- Comprehensive test coverage and error handling

That's the power of NLM Phase 4-6! 🚀
