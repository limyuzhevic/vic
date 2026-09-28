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

### Example 3: Advanced Agent with Learning

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
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"Novelty: {agent.getNoveltyLevel():.3f}")

print("Agent simulation complete!")
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

# Convenience methods
brain.runSimulationForSteps(500, world)    # Run simulation for steps
brain.runEpisode(world, maxSteps=1000)     # Run complete episode
brain.saveCheckpoint("checkpoint.bin")    # Save state to file
brain.loadCheckpoint("checkpoint.bin")    # Load state from file
stats = brain.getBrainStats()              # Get formatted stats
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

### `world.applyMotorCommand(motor_cmd, time)`
Applies the motor command to the world.

### `agent.applyRewardModulation(reward, predicted_reward)`
Helps the brain learn from rewards.

### `world.observe()`
Get sensory percept (alias for `getSensoryPercept()`).

### `world.applyAction(motor_cmd, time)`
Apply motor command (alias for `applyMotorCommand()`).

### `world.computeReward(brain)`
Compute reward based on agent state.

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
print("Total spikes:", brain.getTotalSpikeCount())
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
print("Neurons firing:", brain.getFiringNeuronCount())
```

### Project 3: Complete Agent with Learning

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
agent.enableDevelopment(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Agent simulation complete!")
```

### Project 4: Experiment Runner

```python
import pynlm

# Create experiment configuration
config = pynlm.ExperimentConfig.createTrainingConfig()
config.setNeuronCount(100)
config.setMaxSteps(1000)

# Validate configuration
if not config.validate():
    print(f"Config error: {config.validateAndGetError()}")
    exit(1)

# Create experiment metrics
metrics = pynlm.ExperimentMetrics()

# Create experiment runner
runner = pynlm.createExperimentRunner(config)

# Setup (would need actual brain, agent, world)
# runner.setup(config, brain, agent, world, environment)

print("Experiment setup complete!")
```

### Project 5: Visualization and Analysis

```python
import pynlm

# Create a brain
brain = pynlm.createQuickTestBrain()
brain.initialize()

# Create visualization
visualizer = pynlm.createVisualizer(brain, "PNG")

# Generate network visualization
network_data = visualizer.visualizeNetwork()
visualizer.saveVisualization("network.png")

print("Visualization saved!")
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
| Do action | `world.applyMotorCommand(action, time)` |
| Learn from rewards | `agent.applyRewardModulation(reward, predicted)` |
| Save brain | `brain.saveCheckpoint("file.bin")` |
| Load brain | `brain.loadCheckpoint("file.bin")` |
| Run experiment | `runner.runExperiment()` |
| Visualize | `pynlm.createVisualizer(brain)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that neurons have connections

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Ensure agent's motor neurons are connected

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Enable neuromodulation for better activity

**"ImportError: No module named 'pynlm'"**
```bash
pip install --force-reinstall .
```

**"Segmentation fault"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation
- Validate configuration parameters

**"Memory error"**
- Use smaller brain configuration
- Reduce simulation size
- Disable unnecessary plasticity mechanisms

---

## Next Steps

When you're comfortable:

1. Read `HOW_TO_USE.md` for more details
2. Try `EXPERIMENT_MANAGEMENT.md` for experiment framework
3. Explore `TRAINING_LOOP.md` for reinforcement learning
4. Check `VISUALIZATION_ANALYSIS.md` for visualization tools
5. Read `ADVANCED_FEATURES.md` for advanced use cases
6. Read `docs/ARCHITECTURE.md` to understand how it all works
7. Experiment with different configurations!

That's it! You're now ready to use NLM with all its advanced features.

---

*Version: 2.0 (Updated with complete implementation)*
*Last updated: 2026-09-28*

This updated version includes all the new functionality from the complete NLM implementation, including experiment management, training loops, visualization and analysis, and advanced convenience methods.