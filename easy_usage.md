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

## Advanced Features for Experienced Users

When you're ready to go beyond the basics, NLM offers many advanced features:

### Advanced Configuration

```python
# Create a brain with custom settings
config = pynlm.createDefaultConfig()
config.set("neuron_count", 5000)  # Larger brain
config.set("simulation_timestep", 0.0005)  # Faster simulation

brain = pynlm.createBrain(config)
brain.initialize()
```

### Advanced Agent Control

```python
# Create agent with enhanced capabilities
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)           # Enhanced exploration
agent.enableStructuralPlasticity(True)  # Brain growth
agent.enableDevelopment(True)           # Brain maturation

# Fine-tune advanced parameters
agent.setExplorationWeight(0.8)      # More exploration
agent.setLearningRate(0.05)           # Faster learning
agent.setNoveltyThreshold(0.3)        # More sensitive to novelty
agent.setMemoryRetention(0.9)         # Better memory retention
agent.setDopamineGain(2.0)            # Stronger reward signals
agent.setCuriosityGain(1.5)           # Stronger curiosity signals
agent.setDevelopmentSpeed(1.2)        # Accelerated development
agent.setSensoryGain(2.0)              # Enhanced sensory processing
agent.setMotorThreshold(0.2)           # More responsive movement
```

### Performance Monitoring

```python
# Monitor your agent's performance
for step in range(1000):
    # Run simulation
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check performance metrics
    if step % 100 == 0:
        print("=== Performance Metrics ===")
        print("Activity Level:", brain.getFiringNeuronCount())
        print("Average Firing Rate:", brain.getAverageFiringRate())
        print("Development Stage:", brain.getDevelopmentalStage())
        print("Neuromodulation Level:", agent.getNeuromodulationLevel())
        print("Curiosity Level:", agent.getCuriosityLevel())
        print("Novelty Level:", agent.getNoveltyLevel())
        print("Prediction Error:", agent.getPredictionError())
        print("Total Memory Usage:", agent.getTotalMemoryUsage())
        print("Exploration Ratio:", agent.getExplorationRatio())
        
        # Debug info
        debug_info = agent.getDebugInfo()
        print(debug_info)
```

### Development Control

```python
# Force specific development stages for testing
agent.forceDevelopmentStage(DevelopmentalStage::CriticalPeriod)
brain.step(0)

# Check how development affects plasticity
print("Development Stage:", brain.getDevelopmentalStage())
print("Plasticity Modifier:", agent.plasticityModifier_)
```

### Debugging and Visualization

```python
# Enable debug visualization
agent.enableDebugVisualization(True)
agent.setVisualizationScale(2.0)

# Get detailed debug information
debug_info = agent.getDebugInfo()
print(debug_info)
```

### Memory Management

```python
# Control memory persistence
agent.enableMemoryPersistence(True)
agent.setMemoryRetention(0.85)  # 85% memory retention

# Monitor memory usage
for step in range(500):
    # ... simulation code ...
    print(f"Memory Usage: {agent.getTotalMemoryUsage()} bytes")
```

### Fine-tuning Example

```python
# Complete advanced agent setup
def create_advanced_agent():
    # Configuration
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 3000)
    config.set("region_count", 3)
    config.set("simulation_timestep", 0.001)
    
    # Brain and agent creation
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableMemoryPersistence(True)
    
    # Fine-tune parameters for specific task
    agent.setExplorationWeight(0.7)      # Balance exploration vs. exploitation
    agent.setLearningRate(0.02)           # Fast but stable learning
    agent.setNoveltyThreshold(0.4)        # Moderate novelty sensitivity
    agent.setDopamineGain(1.5)            # Strong reward signals
    agent.setCuriosityGain(1.2)           # Moderate curiosity
    agent.setDevelopmentSpeed(1.0)        # Normal development pace
    agent.setSensoryGain(1.8)              # Enhanced sensory processing
    agent.setMotorThreshold(0.3)           # Responsive movement
    agent.setVisualizationScale(1.5)      # Good visualization scale
    
    return brain, world, agent

# Use the advanced agent
brain, world, agent = create_advanced_agent()

# Run extended training
for step in range(2000):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 500 == 0:
        print(f"Step {step}: Performance metrics collected")
```

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

## Advanced Reference

| Advanced Feature | Code |
|-----------------|------|
| Set exploration weight | `agent.setExplorationWeight(0.8)` |
| Set learning rate | `agent.setLearningRate(0.05)` |
| Set novelty threshold | `agent.setNoveltyThreshold(0.3)` |
| Enable memory persistence | `agent.enableMemoryPersistence(True)` |
| Set memory retention | `agent.setMemoryRetention(0.9)` |
| Set dopamine gain | `agent.setDopamineGain(2.0)` |
| Set curiosity gain | `agent.setCuriosityGain(1.5)` |
| Set development speed | `agent.setDevelopmentSpeed(1.2)` |
| Set sensory gain | `agent.setSensoryGain(2.0)` |
| Set motor threshold | `agent.setMotorThreshold(0.2)` |
| Enable debug visualization | `agent.enableDebugVisualization(True)` |
| Get debug info | `agent.getDebugInfo()` |

---

## Next Steps

When you're comfortable:

1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the Phase 6 demo: `./nlm_phase6_demo`

## Troubleshooting

**"My advanced agent isn't working as expected"**
- Check your parameter settings (review the advanced feature examples)
- Ensure `agent.enableCuriosity(True)` is called for exploration
- Try reducing `setExplorationWeight()` for more focused behavior
- Check `setNoveltyThreshold()` for novelty sensitivity

**"Performance monitoring isn't showing data"**
- Make sure `agent.getDebugInfo()` is called periodically
- Verify that `brain.getFiringNeuronCount()` and related methods are working
- Check that your advanced features are properly enabled

**"Memory usage is too high"**
- Reduce `agent.setMemoryRetention()` to 0.5 or lower
- Disable `agent.enableMemoryPersistence()` if not needed
- Monitor `agent.getTotalMemoryUsage()` to track memory consumption

That's it! You're now ready to use NLM at both basic and advanced levels.

**Note**: The advanced features provide much more control over your neural network simulation, allowing for fine-tuned behavior and performance optimization. Use them to create agents with specific characteristics and behaviors for your particular use case.
