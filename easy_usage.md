# NLM - Enhanced Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**NEW!** The enhanced version now includes:

- **Method chaining** for easier configuration
- **Context managers** for automatic resource cleanup
- **High-level simulation utilities**
- **Enhanced vision processing**
- **Performance monitoring**
- **Advanced features for power users**

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

# Step 1: Make a brain (NEW! Enhanced configuration)
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World (NEW! Enhanced features)

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

### NEW! Complete Agent with Enhanced Configuration

```python
import pynlm

# Enhanced configuration with method chaining
config = pynlm.createConfig() \
    .withNeuronCount(1000) \
    .withLearningRate(0.001) \
    .withDevelopment(True) \
    .withCuriosity(True) \
    .withRewardModulation(True)

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

## NEW! High-Level Simulation Utilities

### Create a Foraging Environment

```python
import pynlm

# Create a foraging environment with one line of code
config, world, agent = nlm.SimulationUtils.createForagingEnvironment(
    worldWidth=20,
    worldHeight=20,
    visionWidth=8,
    visionHeight=8,
    numFoodResources=10
)

print(f"World size: {world.getWidth()} x {world.getHeight()}")
print(f"Vision size: {agent.getSensoryInputSize()}")
print(f"Food resources: {config.getOr('simulation.num_food_resources', 0)}")
```

### Run a Complete Episode

```python
# Create environment first
config, world, agent = nlm.SimulationUtils.createForagingEnvironment(
    worldWidth=15,
    worldHeight=15,
    visionWidth=6,
    visionHeight=6
)

# Run a complete episode
brain, agent = nlm.SimulationUtils.runEpisode(
    config, world, 100, enableLearning=True
)

print(f"Episode completed with {brain.getTotalSpikeCount()} total spikes")
```

### NEW! Enhanced Vision Processing

```python
import pynlm

# Create vision sensor
vision = nlm.Vision(8, 8, 3)

# Create test data
data = [0.1, 0.5, 0.9] * (8 * 8 * 3)
vision.setData(data)

# Convert to Python matrix for visualization
matrix = vision.to_python_matrix()
print(f"Matrix shape: {len(matrix)} x {len(matrix[0])}")

# Detect objects
objects = vision.detectObjects()
print(f"Objects detected: {objects['object_count']}")
print(f"Edges detected: {objects['edges_count']}")
```

### NEW! Performance Monitoring

```python
import pynlm

# Create environment and run simulation
config, world, agent = nlm.create_complete_environment()

# Create performance monitor
perf_monitor = nlm.PerformanceMonitor()
perf_monitor.start_monitoring()

# Run simulation with monitoring
for step in range(50):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain = agent.getBrain()
    brain.step(step)
    agent.decodeMotorCommand()
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    perf_monitor.add_metric("firing_rate", brain.getAverageFiringRate(), "Hz")

# Get performance summary
summary = perf_monitor.get_performance_summary()
print(f"Performance score: {summary['summary']['overall_score']:.2f}")
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

### NEW! Pattern 2: Get Brain Stats with Enhanced Configuration

```python
# Get brain stats with enhanced configuration
def run_enhanced_simulation(config, num_steps):
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    for step in range(num_steps):
        brain.step(step)
        
        # Get enhanced stats
        print(f"Step {step}:")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Firing: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Development: {brain.getDevelopmentalStage()}")
```

### NEW! Pattern 3: Enable Brain Features with Method Chaining

```python
# Create enhanced brain with method chaining
config = pynlm.createConfig() \
    .withNeuronCount(2000) \
    .withLearningRate(0.0005) \
    .withDevelopment(True) \
    .withCuriosity(True) \
    .withRewardModulation(True)

brain = pynlm.createBrain(config)
brain.initialize()

# Create environment
config, world, agent = nlm.create_complete_environment(world_width=25, world_height=25)

# Enable features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
```

---

## NEW! What Each Part Does

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

### NEW! Enhanced Configuration

### `pynlm.createConfig()`
Creates a configuration with method chaining support.

```python
# Method chaining
config = nlm.createConfig() \
    .withNeuronCount(1000) \
    .withLearningRate(0.001) \
    .withDevelopment(True)
```

### `pynlm.SimulationUtils.createForagingEnvironment()`
Creates a foraging environment with food resources and obstacles.

### `pynlm.Vision()`
Enhanced vision sensor with analysis capabilities.

### `pynlm.PerformanceMonitor()`
Performance monitoring for NLM simulations.

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

### NEW! Project 2: Brain Watching a World with Enhanced Features

```python
import pynlm

# Create enhanced environment
config, world, agent = nlm.create_complete_environment(
    world_width=20,
    world_height=20,
    vision_width=8,
    vision_height=8
)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Watch the world
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain = agent.getBrain()
    brain.step(i)
    
print("Watched world for 30 steps")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
```

### NEW! Project 3: Complete Agent with All Features

```python
import pynlm

# Setup with enhanced configuration
config = nlm.createConfig() \
    .withNeuronCount(1500) \
    .withLearningRate(0.001) \
    .withDevelopment(True)

brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run agent with performance monitoring
perf_monitor = nlm.PerformanceMonitor()
perf_monitor.start_monitoring()

for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    perf_monitor.add_metric("firing_rate", brain.getAverageFiringRate(), "Hz")
    
    if step % 20 == 0:
        print(f"Step {step}: Firing={brain.getFiringNeuronCount()}, Curiosity={agent.getCuriosityLevel():.3f}")

# Get performance summary
summary = perf_monitor.get_performance_summary()
print(f"\nPerformance: {summary['summary']['performance_rating']}")
print(f"Score: {summary['summary']['overall_score']:.2f}")
print("Agent simulation complete!")
```

---

## NEW! Quick Reference

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

**NEW! Enhanced Configuration:**
| What you want | Code |
|--------------|------|
| Create enhanced config | `pynlm.createConfig()` |
| Method chaining | `.withNeuronCount(1000)` |
| Create simulation config | `nlm.createConfig().createSimulationConfig()` |
| Create foraging env | `nlm.SimulationUtils.createForagingEnvironment()` |
| Enhanced vision | `nlm.Vision(width, height, channels)` |
| Performance monitor | `nlm.PerformanceMonitor()` |

---

## NEW! Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if enhanced configuration is working

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Enable learning subsystems: `agent.enableCuriosity(True)`

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check enhanced vision processing

**"Performance monitoring not working"**
- Ensure `perf_monitor.start_monitoring()` is called before simulation
- Check that metrics are being added correctly
- Verify configuration includes performance settings

---

## NEW! Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** - See the enhanced technical documentation
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into the enhanced architecture
3. **Experiment with different configurations!** - Try enhanced configuration options
4. **Explore examples in `examples/`** - See all the new example scripts
5. **Check the `IMPROVEMENTS.md` file** - Complete list of all enhancements
6. **Run the demonstration scripts** - See all features in action

**NEW! Additional Resources:**
- `demo_improved_bindings.py` - Complete demonstration of new features
- `test_pynlm.py` - Comprehensive test suite
- `examples/` - Additional example scripts
- `NLM_Enhanced_Python_Bindings_Documentation.md` - Complete documentation

That's it! You're now ready to use the enhanced NLM bindings with all the new features!
