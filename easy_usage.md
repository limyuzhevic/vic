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

## Version Information

**NLM Python Bindings Version:** `1.0.0`
**Author:** NLM Development Team
**Description:** Python bindings for Neural Learning Machine simulation framework

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

## NEW! Pythonic Configuration Interface

The NLM Python bindings now feature a Python dictionary-like interface for the Config class, making configuration management more intuitive and Pythonic:

```python
import pynlm

# Create and configure brain with Pythonic interface
config = pynlm.createDefaultConfig()

# Method 1: Traditional approach (still works)
config.set("brain.neuron_count", 1000)

# Method 2: Python dictionary-like interface (NEW!)
config["brain.neuron_count"] = 1000
config["plasticity.stdp.learning_rate"] = 0.001
config["brain.v_thresh"] = -50.0

# Method 3: Type-specific getters (NEW!)
neuron_count = config.get_int("brain.neuron_count", 1000)
learning_rate = config.get_float("plasticity.stdp.learning_rate", 0.001)
threshold = config.get_int("brain.v_thresh", -50)

# Method 4: Safe access with defaults
spike_count = config.get_or("brain.spike_threshold", 10)
print(f"Spike threshold: {spike_count}")

# Method 5: Update multiple settings at once
config.update({
    "brain.synapse_density": 0.1,
    "plasticity.hebbian.enable": True,
    "neuromod.dopamine.scale": 1.0
})

# Method 6: Check if configuration exists
if "brain.neuron_count" in config:
    print("Brain configuration available")

# Method 7: Get all configuration keys
keys = config.getKeys()
print(f"Available keys: {keys}")

# Method 8: String representation
print(repr(config))
```

### Configuration Type-Specific Methods

```python
# Get with type-specific convenience methods
config["brain.neuron_count"] = 500  # Python int
config["brain.v_thresh"] = -55.0   # Python float
config["brain.is_active"] = True   # Python bool
config["brain.names"] = ["cortex", "hippocampus"]  # Python list

# The Config class automatically converts Python types
print(config["brain.neuron_count"])     # 500 (int)
print(config["brain.v_thresh"])        # -55.0 (float)
print(config["brain.is_active"])       # True (bool)
print(config["brain.names"])           # ["cortex", "hippocampus"] (list)
```

---

## NEW! Enhanced Sensory Processing

The SensoryPercept class now supports batch processing and dictionary conversion:

```python
import pynlm

# Create world and get sensory percept
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Get sensory percept (always a SensoryPercept object)
percept = world.getSensoryPercept()

# NEW: Convert to Python dictionary for easy manipulation
percept_dict = percept.toDict()
print("Percept as dictionary:", percept_dict)

# NEW: Update percept from dictionary
new_percept = pynlm.SensoryPercept()
new_percept.fromDict({
    "vision": [0.1, 0.2, 0.3, 0.4],
    "touch": [0.0, 0.5],
    "internal": [0.8, 0.9],
    "timestamp": 123.45
})

# NEW: Batch process multiple percepts
percept1 = pynlm.SensoryPercept()
percept1.setVision([0.1, 0.2, 0.3])

percept2 = pynlm.SensoryPercept()
percept2.setVision([0.4, 0.5, 0.6])

percept3 = pynlm.SensoryPercept()
percept3.setVision([0.7, 0.8, 0.9])

# Batch update multiple percepts
all_percepts = [percept1, percept2, percept3]
for i, percept in enumerate(all_percepts):
    percept.batchUpdate({"vision": [v * (i + 1) for v in percept.getVision()]})
    print(f"Percept {i+1} scaled:", percept.getVision())

# NEW: Scale vision data (common operation)
percept.scaleVision(1.5)  # Scale all vision values by 1.5x

# NEW: Get sensor data as convenient tuple
vision_data = percept.getSensorData("vision")
print("Vision data tuple:", vision_data)  # ("vision", data, width, height)

# NEW: Check if percept is empty
if percept.isEmpty():
    print("Percept has no data")
```

---

## NEW! Enhanced World Interface

The SimpleWorld class now provides more Pythonic methods for agent interaction:

```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# NEW: Get agent position as convenient tuple
position = world.getPosition()
print(f"Agent position: x={position[0]}, y={position[1]}")

# NEW: Get agent velocity as tuple
velocity = world.getVelocity()
print(f"Agent velocity: vx={velocity[0]}, vy={velocity[1]}")

# NEW: Get complete world state as dictionary
state = world.getState()
print("World state:", state)

# NEW: Set world state from dictionary
new_state = {
    "x": 15.0,
    "y": 15.0,
    "orientation": 45.0,
    "health": 100.0,
    "energy": 80.0
}
world.setState(new_state)
print("Agent moved to new position")

# NEW: Get world bounds as tuple
bounds = world.getBounds()
print(f"World bounds: minX={bounds[0]}, minY={bounds[1]}, maxX={bounds[2]}, maxY={bounds[3]}")

# NEW: Check if agent is alive
if world.isActive():
    print("Agent is alive and active")
```

---

## NEW! Enhanced Brain Interface

The Brain class now provides comprehensive Pythonic methods for brain manipulation and statistics:

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# NEW: Get comprehensive brain statistics as dictionary
brain_stats = brain.getStats()
print("Brain statistics:", brain_stats)

# NEW: Get neuron-specific statistics
neuron_stats = brain.getNeuronStats()
print("Neuron stats:", neuron_stats)

# NEW: Get region names
region_names = brain.getRegionNames()
print("Region IDs:", region_names)

# NEW: Inject currents to multiple neurons using dictionary
currents = {
    "1": 1.5,  # neuron ID 1
    "10": 2.0,  # neuron ID 10
    "100": 0.5  # neuron ID 100
}
brain.injectCurrentBatch(currents)

# NEW: Inject currents to neuron types
neuron_currents = {
    "Excitatory": 1.0,
    "Inhibitory": -0.5,
    "Sensory": 0.8
}
brain.injectCurrentToTypeBatch(neuron_currents)

# NEW: Check if brain is initialized
if brain.isInitialized():
    print("Brain is ready for simulation")

# NEW: Save brain with automatic timestamp
brain.saveWithTimestamp("my_brain_checkpoint")

# NEW: Chain multiple operations (method chaining)
brain.stepWithTime(0, 0.0)  # Returns self for chaining
brain.stepWithTime(1, 0.1)  # Can chain multiple steps
brain.saveWithTimestamp("auto_saved")

# Create agent with enhanced interface
agent = pynlm.createAgentBrain(brain)

# NEW: Get comprehensive agent state
agent_state = agent.getState()
print("Agent state:", agent_state)

# NEW: Enable all features at once
agent.enableAllFeatures()

# NEW: Get list of enabled features
enabled_features = agent.getEnabledFeatures()
print("Enabled features:", enabled_features)

# NEW: Process multiple sensory percepts at once
percept_list = [percept1, percept2, percept3]  # List of SensoryPercept objects
agent.processSensoryBatch(percept_list)

# NEW: Update agent with timestep (returns self for chaining)
agent.updateWithTime(0.1).updateWithTime(0.2)

# NEW: Check if agent is actively learning
if agent.isActive():
    print("Agent is learning effectively")
```

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

### Pattern 2: Get Brain Stats (ENHANCED)

```python
# Enhanced brain statistics collection inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())

# NEW: Get comprehensive stats
brain_stats = brain.getStats()
print("Full brain stats:", brain_stats)

# NEW: Get agent status
agent_status = agent.getStatus()
print("Agent status:", agent_status)
```

### Pattern 3: Enable Brain Features (ENHANCED)

```python
# Enhanced brain feature management:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# NEW: Enable all features at once
agent.enableAllFeatures()

# NEW: Disable all features
agent.disableAllFeatures()

# NEW: Check which features are enabled
enabled_features = agent.getEnabledFeatures()
print("Currently enabled:", enabled_features)
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

### `brain.step(step_number, time)`
**NEW!** Enhanced step method that accepts a timestamp for time-synchronized simulation.

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

### Project 2: Brain Watching a World (ENHANCED)

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

# NEW: Enhanced monitoring with batch processing
percept_list = []
for i in range(30):
    world.update(0.1)
    percept = world.getSensoryPercept()
    percept_list.append(percept)
    agent.processSensoryInput(percept)
    brain.step(i)

# Process all percepts in batch
agent.processSensoryBatch(percept_list)
print("Processed all percepts efficiently!")
```

### Project 3: Complete Agent (ENHANCED)

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

# NEW: Enhanced feature configuration
agent.enableAllFeatures()  # Enable reward, structural plasticity, development, curiosity

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
        # NEW: Enhanced monitoring
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
        
        # Get comprehensive stats
        stats = brain.getStats()
        print(f"  Brain stats: {len(stats)} categories")

print("Agent simulation complete!")
```

---

## Quick Reference (ENHANCED)

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Make brain think with time | `brain.step(step, time)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| Get comprehensive brain stats | `brain.getStats()` |
| Get agent status | `agent.getStatus()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Python config access | `config["key"] = value` |
| Batch process percepts | `agent.processSensoryBatch(percepts)` |
| Get agent state | `agent.getState()` |

---

## Error Handling (IMPROVED)

The NLM Python bindings now provide better error messages and Python-specific exceptions:

```python
import pynlm

try:
    # Access non-existent configuration key
    value = config["nonexistent.key"]
except KeyError as e:
    print(f"Configuration key error: {e}")
except ValueError as e:
    print(f"Value error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")

# All exceptions are mapped to appropriate Python exceptions
# with clear, informative error messages
```

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Configuration key not found"**
- Use `config.get_or()` for safe access with defaults
- Use `if "key" in config:` to check existence

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

**"Batch processing error"**
- Ensure all items in the list are the correct type
- Check that sensory percepts have valid data

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more detailed C++ documentation
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations!** - The new Pythonic interfaces make experimentation much easier
4. **Try the enhanced examples** that demonstrate the new batch processing and dictionary interfaces
5. **Use the comprehensive stats methods** to monitor your brain's performance

The NLM Python bindings now provide both simple beginner-friendly interfaces and advanced Pythonic features for experienced users. You're now ready to use NLM with confidence!

---

## Advanced Usage Tips

### 1. **Configuration Management**
```python
# Create a configuration factory function
def create_experiment_config(experiment_type="standard"):
    config = pynlm.createDefaultConfig()
    
    if experiment_type == "standard":
        config.update({
            "brain.neuron_count": 1000,
            "brain.synapse_density": 0.1,
            "plasticity.stdp.enable": True
        })
    elif experiment_type == "exploration":
        config.update({
            "brain.neuron_count": 500,
            "plasticity.curiosity.enable": True,
            "neuromod.novelty.enable": True
        })
    
    return config

# Use it
config = create_experiment_config("exploration")
brain = pynlm.createBrain(config)
```

### 2. **Simulation Monitoring**
```python
class SimulationMonitor:
    def __init__(self):
        self.stats_history = []
    
    def record(self, step, brain, agent):
        self.stats_history.append({
            "step": step,
            "neurons_firing": brain.getFiringNeuronCount(),
            "total_spikes": brain.getTotalSpikeCount(),
            "curiosity": agent.getCuriosityLevel(),
            "novelty": agent.getNoveltyLevel(),
            "dev_stage": brain.getDevelopmentalStage()
        })
    
    def get_summary(self):
        if not self.stats_history:
            return "No data recorded"
        
        latest = self.stats_history[-1]
        return f"Step {latest['step']}: {latest['neurons_firing']} firing neurons, " \
               f"curiosity={latest['curiosity']:.3f}, dev_stage={latest['dev_stage']}"

# Use the monitor
monitor = SimulationMonitor()

for step in range(1000):
    # Your simulation loop here...
    monitor.record(step, brain, agent)
    
print(monitor.get_summary())
```

### 3. **Efficient Batch Processing**
```python
def run_batch_simulation(brain, world, agent, num_episodes, steps_per_episode):
    """Run multiple simulation episodes efficiently using batch processing"""
    for episode in range(num_episodes):
        # Reset environment and agent for new episode
        world.reset()
        agent.reset()
        
        # Collect all percepts for the episode
        episode_percepts = []
        
        for step in range(steps_per_episode):
            world.update(0.1)
            percept = world.getSensoryPercept()
            episode_percepts.append(percept)
            agent.processSensoryInput(percept)
            brain.step(step)
            world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Process all percepts in batch for better efficiency
        agent.processSensoryBatch(episode_percepts)
        
        # Get comprehensive stats
        brain_stats = brain.getStats()
        agent_stats = agent.getStatus()
        
        print(f"Episode {episode}: {brain_stats['neurons']} neurons, "
              f"curiosity={agent_stats['curiosity']:.3f}")
    
    return brain, agent, world

# Run multiple episodes
brain, agent, world = run_batch_simulation(brain, world, agent, 5, 100)
```

These advanced patterns showcase how the new Pythonic interfaces enable more efficient and powerful simulations while maintaining readability and ease of use.

---

## New Features Coming Soon

The enhanced Python bindings will soon include:

- **Advanced serialization**: JSON-based brain saving/loading
- **Neural network visualization**: Built-in plotting and visualization tools
- **Multi-environment support**: Complex world configurations and environments
- **Advanced learning algorithms**: Custom plasticity rules and learning mechanisms
- **Parallel processing**: Multi-core and GPU acceleration for large-scale simulations

These features will continue to make NLM more powerful and accessible to Python users!

---

That's it! You're now ready to use NLM with both the simple beginner interfaces and the advanced Pythonic features. The enhanced bindings provide the best of both worlds: easy-to-use methods for beginners and powerful, Pythonic interfaces for advanced users.

Happy brain simulation! 🧠✨