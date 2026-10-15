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

## Advanced Configuration (New Feature!)

The NLM now includes advanced configuration with validation and performance profiles!

### Example: Performance Profile Configuration

```python
import pynlm

# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Apply performance profile
config.applyPerformanceProfile("balanced")  # Options: "fast", "balanced", "deep"

# Create and initialize brain
brain = pynlm.createBrain(config.getConfig())
brain.initialize()

# Check configuration
print("Configuration summary:")
print(config.getProfileSummary())
```

### Example: Batch Configuration Setting

```python
import pynlm

# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Batch set multiple settings at once
settings = [
    {"brain.neuron_count", 5000},
    {"plasticity.stdp.enable", True},
    {"plasticity.hebbian.enable", True},
    {"neuromod.curiosity.enable", True},
    {"neuromod.novelty.enable", False},
]

config.batchSet(settings)

# Verify settings
print("Brain neuron count:", config.getValidated("brain.neuron_count"))
print("STDP enabled:", config.getValidated("plasticity.stdp.enable"))
```

### Example: Validation and Error Handling

```python
import pynlm

# Create advanced configuration
config = pynlm.createAdvancedConfig()

# Set values with automatic validation
config.setValidated("brain.neuron_count", 1000)  # Valid range: 100-100000
config.setValidated("plasticity.stdp.learning_rate", 0.001)  # Valid range: 0.0-0.1

# Try to set invalid value (will raise exception)
try:
    config.setValidated("brain.neuron_count", -50)  # Invalid!
except ValueError as e:
    print("Validation error caught:", str(e))

# Use validated getter
neuron_count = config.getValidated("brain.neuron_count", 500)
print("Neurons:", neuron_count)
```

---

## Advanced Agent Operations (New Feature!)

### Example: Run Complete Episodes

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run a complete episode
print("Running episode...")
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("Episode complete!")
```

### Example: Training and Evaluation

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure training
num_episodes = 10
steps_per_episode = 500

print("Training agent...")
for episode in range(num_episodes):
    # Run episode
    for step in range(steps_per_episode):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress
    print(f"Episode {episode + 1}/{num_episodes} complete")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")

print("Training complete!")
```

---

## Performance Monitoring (New Feature!)

### Example: Benchmark and Monitor

```python
import pynlm
import time

# Create a benchmark brain
brain = pynlm.benchmarkPerformance("balanced")
brain.initialize()

print("Benchmarking performance...")
steps = 1000

start_time = time.time()
for i in range(steps):
    brain.step(i)
end_time = time.time()

# Get performance metrics
metrics = brain.getPerformanceMetrics()
print(f"Performance metrics:")
print(f"  Steps per second: {steps / (end_time - start_time):.0f}")
print(f"  Average firing rate: {metrics.averageFiringRate:.2f} Hz")
print(f"  Memory usage: {metrics.totalMemoryUsage / 1024 / 1024:.2f} MB")
print(f"  Plasticity update time: {metrics.plasticityUpdateTime * 1000:.2f} ms")

# Get system health
health = brain.getSystemHealth()
print(f"System health: {'Healthy' if health.isHealthy else 'Issues detected'}")
if not health.isHealthy:
    for warning in health.warnings:
        print(f"  Warning: {warning}")
    for error in health.errors:
        print(f"  Error: {error}")
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

### Project 3: Complete Agent with Advanced Features

```python
import pynlm

# Setup
config = pynlm.createAdvancedConfig()
config.applyPerformanceProfile("balanced")

brain = pynlm.createBrain(config.getConfig())
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

# Run agent with monitoring
print("Running advanced agent simulation...")
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

print("Advanced agent simulation complete!")
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

**"Configuration validation error"**
- Use `config.setValidated()` instead of `config.set()`
- Check the error message for valid range

**"Performance issues"**
- Use performance profiles: `applyPerformanceProfile("fast")` or `applyPerformanceProfile("deep")`
- Monitor performance with `brain.getPerformanceMetrics()`

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the advanced examples in this guide
5. Check out the performance monitoring and validation features

That's it! You're now ready to use NLM with advanced features and capabilities.
