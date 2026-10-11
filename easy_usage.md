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

### Example 3: Using Advanced Features

```python
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Performance monitoring
monitor = pynlm.create_performance_monitor()
monitor.start()

# Run simulation with monitoring
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record metrics
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Get performance statistics
stats = monitor.stop()
print(f"Simulation stats: {stats}")

print("Agent simulation complete!")
```

### Example 4: Modular API Usage

```python
import pynlm
from pynlm.core.config import Config
from pynlm.core.types import NeuronType, ActionType
from pynlm.agent import AgentBrain
from pynlm.world import SimpleWorld
from pynlm.sensory import Vision
from pynlm.motor import Action

# Modular imports for better organization
config = Config()
config.set("brain.neuron_count", 500)

brain = pynlm.createBrain(config)
brain.initialize()

# Configure specific brain parameters
brain.getConfig().set("brain.v_thresh", -52.0)

# Create world with custom configuration
world = SimpleWorld()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.reset()
world.setMaxEnergy(150.0)

# Create agent
agent = AgentBrain(brain)
agent.initialize(world)

# Enable subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

print("Modular API setup complete!")
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

### Pattern 4: Performance Monitoring

```python
# Enable performance monitoring
import pynlm
from pynlm.advanced import create_performance_monitor

monitor = create_performance_monitor()
monitor.start()

# Run simulation
for step in range(1000):
    # ... your simulation code ...
    
    if step % 100 == 0:
        monitor.record_step_time(some_timing_function())

# Get performance statistics
stats = monitor.stop()
print(f"Average step time: {stats['avg_step_time']*1000:.2f}ms")
print(f"Steps per second: {stats['steps_per_second']:.1f}")
```

### Pattern 5: Experiment Management

```python
# Run multiple experiments
import pynlm
from pynlm.advanced import create_experiment_manager

experiment_manager = create_experiment_manager()

# Create experiment
exp_id = experiment_manager.create_experiment(
    name="Test Experiment",
    brain=brain,
    world=world,
    agent=agent,
    config={"record_metrics": True, "brain_config": brain.getConfig()}
)

# Run experiment
results = experiment_manager.run_experiment(exp_id, num_steps=1000)
print(f"Experiment completed in {results['duration']:.2f}s")
print(f"Total spikes: {results['metrics']['brain_metrics'][0]['total_spikes']}")
```

---

## What Each Part Does

### Core Classes

| Function | Purpose |
|----------|---------|
| `pynlm.createBrain(config)` | Creates a virtual brain with neurons and synapses |
| `brain.initialize()` | Starts up the brain. Always call this before using the brain! |
| `brain.step(step_number)` | Makes the brain process one moment in time |
| `pynlm.createSimpleWorld()` | Creates a simple 2D world |
| `pynlm.createAgentBrain(brain)` | Creates an agent brain interface |
| `brain.getFiringNeuronCount()` | Gets count of currently firing neurons |
| `world.getSensoryPercept()` | Gets what the agent sees |
| `agent.decodeMotorCommand()` | Gets action from brain's motor neurons |

### Advanced Features

| Feature | Benefit |
|---------|---------|
| `PerformanceMonitor` | Track simulation performance |
| `AdvancedBrainFeatures` | Enhanced brain analysis capabilities |
| `AdvancedWorldFeatures` | Custom sensor/actuator support |
| `ExperimentManager` | Run and manage experiments |

### Configuration

| Function | Purpose |
|----------|---------|
| `pynlm.createDefaultConfig()` | Create default configuration |
| `brain.getConfig()` | Get brain configuration |
| `config.set(key, value)` | Set configuration value |

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
from pynlm.advanced import create_performance_monitor

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning and monitoring
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

monitor = create_performance_monitor()
monitor.start()

# Run agent with performance tracking
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Get performance stats
stats = monitor.stop()
print(f"Simulation completed in {stats['total_time']:.2f}s")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Steps per second: {stats['steps_per_second']:.1f}")

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
| Performance monitor | `create_performance_monitor()` |
| Experiment manager | `create_experiment_manager()` |
| Advanced features | `AdvancedBrainFeatures`, `AdvancedWorldFeatures` |

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

**"Performance monitoring not working"**
- Make sure `monitor.start()` is called before simulation
- Check that `monitor.record_step_time()` is called each step

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more details on the old API
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Explore `python/pynlm/`** for the new modular API structure
4. **Try the advanced features** in `python/pynlm/advanced.py`
5. **Experiment with different configurations!**

That's it! You're now ready to use NLM with the improved modular Python API!
