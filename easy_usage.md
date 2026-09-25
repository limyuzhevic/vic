# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## New Features in Phase 6 (Integrated Brain)

NLM Phase 6 includes **comprehensive integration** of all brain systems:

### 🧠 **Memory Systems**
- **Working Memory**: Short-term memory that maintains active information
- **Episodic Memory**: Long-term memory that stores experiences and events
- **Associative Memory**: Pattern-based memory for making connections

### 🎯 **Cognitive Systems**
- **Concept Formation**: Discovers patterns and forms abstract concepts
- **Neural Planner**: Plans multi-step actions and predicts outcomes
- **Attentional Selection**: Selects what to focus on using competitive dynamics

### ⚡ **Neuromodulation Systems**
- **Dopamine**: Reward signaling and reinforcement learning
- **Curiosity**: Exploration motivation and novel behavior
- **Novelty**: Detects new patterns and events
- **Acetylcholine (ACh)**: Attention and memory consolidation
- **Norepinephrine (NE)**: Arousal and signal detection
- **Serotonin (5-HT)**: Mood regulation and impulse control

### 🔮 **Prediction System**
- **Temporal Prediction**: Learns sequences and predicts what comes next
- **Action Consequences**: Predicts outcomes of actions
- **Confidence Tracking**: Estimates reliability of predictions

### 🌱 **Development System**
- **Plasticity Modifiers**: Changes learning rates during development
- **Maturation**: Brain ages and adjusts capabilities
- **Stage Transitions**: Different developmental phases with different learning properties

## Key Benefits

### Before Phase 6:
- **Disconnected systems**: All brain components existed but didn't interact
- **Limited functionality**: Basic neural computation without memory or cognition
- **Stub implementations**: Many TODO PHASE 2 comments

### After Phase 6:
- **Fully integrated**: All systems work together in a coherent brain loop
- **Rich capabilities**: Learning, memory, prediction, and cognition
- **Robust implementation**: All TODO/PLACEHOLDER comments resolved
- **Real artificial brain**: Functions as a complete simulated neural system

## Usage Examples

### Simple Brain (Still Works)
```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it think
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

### Integrated Brain (New Capabilities)
```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent and world for full integration
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
agent.initialize(world)

# Enable all cognitive capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation with memory, prediction, and cognition
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check cognitive capabilities
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Memory episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
        print(f"  Prediction error: {brain.getPredictionSystem().getPredictionError() if brain.getPredictionSystem() else 0}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
```

## What Changed

### **Memory Integration**
- **Working Memory**: Now stores active neural patterns with competition
- **Episodic Memory**: Now records experiences with temporal and reward information
- **Memory Consolidation**: Automatically consolidates important memories

### **Prediction Integration** 
- **Sequence Learning**: Now learns temporal patterns from experience
- **Action Consequences**: Now predicts outcomes of actions
- **Prediction Error**: Now modulates learning based on prediction accuracy

### **Cognitive Integration**
- **Concept Formation**: Now processes patterns to form abstract concepts
- **Neural Planner**: Now uses predictions to select optimal actions
- **Attentional Selection**: Now applies competitive selection to focus resources

### **Neuromodulation Integration**
- **Dopamine**: Now affects neural excitability, working memory, and decision making
- **Acetylcholine**: Now enhances attention and memory encoding
- **Norepinephrine**: Now increases arousal and signal detection
- **Serotonin**: Now regulates impulsivity and emotional state

## Verification

The Phase 6 integration has been thoroughly tested:

```bash
# Run the Phase 6 integration experiment
./nlm_phase6_demo

# Or run through the Python API
python -c "
import pynlm
result = pynlm.runPhase6Integration()
print('Integration Results:', result)
print('Memory systems:', 'Yes' if result.memoryWorkingMemoryIntegrated and result.memoryEpisodicMemoryIntegrated else 'No')
print('Prediction:', 'Yes' if result.predictionIntegrated else 'No')
print('Neuromodulation:', 'Yes' if result.neuromodulationIntegrated else 'No')
print('Development:', 'Yes' if result.developmentIntegrated else 'No')
"
```

## Backward Compatibility

All existing code continues to work:

```python
# Old examples still work unchanged
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
for i in range(100):
    brain.step(i)
```

New capabilities are available through the enhanced AgentBrain interface and additional getter methods.

## New API Methods

For advanced usage, the following methods are now available:

```python
# Get integrated systems
brain.getWorkingMemory()           # NeuralWorkingMemory
brain.getEpisodicMemory()          # NeuralEpisodicMemory  
brain.getPredictionSystem()        # PredictionSystem
brain.getPlanner()                # NeuralPlanner
brain.getConceptFormation()        # ConceptFormation
brain.getAttention()              # AttentionalSelection
brain.getDevelopmentSystem()       # DevelopmentSystem
brain.getDopamine()               # Dopamine
brain.getACh()                    # Acetylcholine
brain.getNE()                     # Norepinephrine
brain.get5HT()                    # Serotonin

# Get current sensory observation
brain.getCurrentSensoryObservation()  # For prediction system

# Enhanced agent capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

## Summary

Phase 6 represents a **quantum leap** in NLM capabilities:

- **Before**: Disconnected components that didn't work together
- **After**: Fully integrated artificial brain that learns, remembers, predicts, and plans

The NLM project has evolved from a **neural simulator** to a **complete artificial brain** with all the components needed for sophisticated cognitive behavior.

That's it! You're now ready to use the fully integrated NLM Phase 6 brain.

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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
