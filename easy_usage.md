# NLM - Easy Usage Guide for Beginners and Advanced Users

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

### Example 2: Advanced Brain with Development

```python
import pynlm

# Create a balanced configuration with development
config = pynlm.createBalancedConfig(neuronCount=1000, enableDevelopment=True)
brain = pynlm.createBrain(config)
brain.initialize()

# Set developmental stage (critical for learning)
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Run some steps
for i in range(500):
    brain.step(i)
    if i % 100 == 0:
        print(f"Step {i}: Firing rate = {brain.getAverageFiringRate():.3f}")
        print(f"Development stage: {brain.getDevelopmentStageName()}")
        print(f"Progress: {brain.getDevelopmentalProgress():.1f}%")

print("Advanced development completed!")
```

### Example 3: Complete Advanced Environment

```python
import pynlm

# Create a complete advanced environment
brain, world, agent = pynlm.createCompleteEnvironment(
    configType="balanced",
    worldWidth=20,
    worldHeight=20,
    visionWidth=8,
    visionHeight=8
)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run advanced simulation with statistics
result = pynlm.runAdvancedSimulation(
    brain, world, agent,
    numSteps=1000,
    recordMetrics=True,
    callback=lambda step, firing_rate, reward: 
        print(f"Step {step}: FR={firing_rate:.3f}, Reward={reward:.3f}")
)

print(f"=== SIMULATION COMPLETE ===")
print(f"Total Reward: {result.totalReward:.3f}")
print(f"Average Firing Rate: {result.avgFiringRate:.3f}")
print(f"Curiosity Level: {result.curiosityLevel:.3f}")
print(f"Final Neurons: {result.finalNeuronCount}")
print(f"Final Synapses: {result.finalSynapseCount}")
```

### Example 4: World with Random Objects

```python
import pynlm

# Create world with random objects
world = pynlm.createAdvancedWorld(
    width=20,
    height=20,
    visionWidth=8,
    visionHeight=8,
    energyDecayRate=0.01,
    maxEnergy=100.0,
    numObjects=15  # Add 15 random objects
)

# Add some special objects
print("Adding special objects...")
for obj in world.getObjects():
    if obj.type == pynlm.WorldObjectType.Resource:
        print(f"Resource at ({obj.x:.1f}, {obj.y:.1f}) value={obj.value}")
    elif obj.type == pynlm.WorldObjectType.Hazard:
        print(f"Hazard at ({obj.x:.1f}, {obj.y:.1f}) value={obj.value}")
```

---

## Advanced Features for Power Users

### Configuration Types

The `createBalancedConfig()` function provides several configuration presets:

```python
# Default balanced configuration
config = pynlm.createBalancedConfig()

# High plasticity (more learning, more connections)
config = pynlm.createBalancedConfig(neuronCount=1500, enableDevelopment=True)

# Sparse (fewer neurons, less plasticity)
config = pynlm.createBalancedConfig(neuronCount=500, connectionProbability=0.05)

# Custom configuration
config = pynlm.createBalancedConfig(
    neuronCount=2000,
    connectionProbability=0.2,
    enablePlasticity=True,
    enableDevelopment=False
)
```

### World Creation Options

```python
# Basic world
world = pynlm.createSimpleWorld()

# Advanced world with energy decay and max energy
world = pynlm.createAdvancedWorld(
    width=30,
    height=30,
    visionWidth=10,
    visionHeight=10,
    energyDecayRate=0.02,
    maxEnergy=200.0
)

# World with many objects
world = pynlm.createAdvancedWorld(20, 20, 8, 8, numObjects=50)
```

### Development System

```python
# Monitor development progress
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
stage_name = brain.getDevelopmentStageName()
progress = brain.getDevelopmentalProgress()

print(f"Stage: {stage_name}, Progress: {progress:.1f}%")

# Development stages:
# - Initial: Brain just created
# - CriticalPeriod: Maximum learning ability
# - Maturation: Developing specialized functions
# - Adult: Stable performance
# - Aging: Gradual decline
```

### Neuromodulation Control

```python
# Control neuromodulation levels
brain.setDopamineLevel(1.0)      # Reward processing
brain.setCuriosityLevel(0.8)     # Exploration motivation
brain.setNoveltyLevel(0.6)       # Novelty detection
brain.setPredictionErrorLevel(0.1) # Prediction error

# Monitor current levels
dopamine = brain.getDopamineLevel()
curiosity = brain.getCuriosityLevel()
novelty = brain.getNoveltyLevel()
error = brain.getPredictionError()

print(f"Dopamine: {dopamine:.2f}, Curiosity: {curiosity:.2f}, Novelty: {novelty:.2f}, Error: {error:.2f}")
```

### Brain State Management

```python
# Save brain state
brain.saveState("my_brain_checkpoint.bin")

# Load brain state
brain.loadState("my_brain_checkpoint.bin")

# Validate state
if brain.validateState():
    print("Brain state is valid")
else:
    print("Brain state validation failed!")

# Get brain hash
brain_hash = brain.getBrainHash()
print(f"Brain hash: {brain_hash}")
```

### Advanced Statistics

```python
# Get comprehensive statistics
stats = brain.getStatistics()

print("=== BRAIN STATISTICS ===")
print(f"Neurons: {stats.totalNeurons}")
print(f"Synapses: {stats.totalSynapses}")
print(f"Active Neurons: {stats.activeNeurons}")
print(f"Firing Neurons: {stats.firingNeurons}")
print(f"Average Firing Rate: {stats.avgFiringRate:.3f}")
print(f"Excitatory/Inhibition Ratio: {stats.excitationInhibitionRatio:.3f}")
print(f"Working Memory Traces: {stats.workingMemoryTraces}")
print(f"Episodic Memory Episodes: {stats.episodicMemoryEpisodes}")
print(f"Concept Count: {stats.conceptCount}")
print(f"Curiosity Level: {stats.curiosityLevel:.3f}")
print(f"Novelty Level: {stats.noveltyLevel:.3f}")
print(f"Dopamine Level: {stats.dopamineLevel:.3f}")
print(f"Prediction Error: {stats.predictionError:.3f}")
print(f"Developmental Stage: {stats.developmentalStage}")
print(f"Developmental Progress: {stats.developmentalProgress:.1f}%")
```

### Agent Configuration

```python
# Create advanced agent with specific features
agent = pynlm.createAdvancedAgent(
    brain,
    world,
    enableRewardModulation=True,
    enableCuriosity=True,
    enableDevelopment=True,
    enableStructuralPlasticity=True
)
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
        
        # Optional: Record statistics
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentStageName())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
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

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createBalancedConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createBalancedConfig())
brain.initialize()
world = pynlm.createAdvancedWorld(20, 20, 8, 8, numObjects=10)
agent = pynlm.createAdvancedAgent(brain, world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent with Statistics

```python
import pynlm

# Setup
config = pynlm.createBalancedConfig(enableDevelopment=True)
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createAdvancedWorld(15, 15, 8, 8, numObjects=5)
agent = pynlm.createAdvancedAgent(brain, world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent with statistics
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}")

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
| Create advanced world | `pynlm.createAdvancedWorld(width, height, visionWidth, visionHeight, numObjects)` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Create advanced agent | `pynlm.createAdvancedAgent(brain, world, **features)` |
| Get brain stats | `brain.getStatistics()` |
| Create balanced config | `pynlm.createBalancedConfig()` |
| Get development stage | `brain.getDevelopmentStageName()` |
| Set developmental stage | `brain.setDevelopmentalStage(stage)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if neuromodulation is enabled

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that `agent.decodeMotorCommand()` is returning an action

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check development stage for proper maturation

**"Statistics not working"**
- Ensure you're calling `brain.getStatistics()` after the brain has run steps
- Check that development is enabled for proper statistics collection

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details on advanced features
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Use `NLM_IMPROVEMENTS_SUMMARY.md` for complete API reference

That's it! You're now ready to use NLM with both basic and advanced capabilities.

---

## Version Information

**Enhanced Version**: 2.0
- Added 20+ advanced features for power users
- Improved performance with O(1) configuration lookups
- Enhanced memory management with modern C++ patterns
- Comprehensive statistics collection
- Advanced development system
- Neuromodulation control
- World generation utilities
- Complete simulation framework

**Previous Version**: 1.0
- Basic functionality for beginners
- Simple world and agent interfaces
- Basic configuration options