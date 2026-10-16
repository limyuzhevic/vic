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

```python
import pynlm

# Advanced: Create brain for complex navigation task
brain = pynlm.createBrainForTask(
    pynlm.TaskType.ComplexNavigation,
    neuron_count=15000  # Larger brain for navigation
)

# Configure for research
pynlm.configureForResearch(brain)

# Enable advanced features
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
world.reset()
agent.initialize(world)

# Enable learning mechanisms
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)

# Run exploration simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Action {action}")
        print(f"  Stats: {brain.getFiringNeuronCount()} neurons firing")

print("Navigation simulation complete!")
```

### Advanced Example 2: Performance Monitoring

```python
import pynlm
import time

# Create performance-optimized brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
config.set("performance.optimization", True)

brain = pynlm.createBrain(config)
brain.initialize()

# Start performance profiling
brain.startPerformanceProfiler()

# Run computation-intensive task
start_time = time.time()
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        # Get performance statistics
        stats = brain.getPerformanceStats()
        elapsed = time.time() - start_time
        print(f"Step {step}:")
        print(f"  Neurons/sec: {stats.neuronsPerSecond:.0f}")
        print(f"  Spikes/sec: {stats.spikesPerSecond:.0f}")
        print(f"  Real-time factor: {stats.realTimeFactor:.2f}")
        print(f"  Memory usage: {stats.memoryUsageMB:.1f} MB")

brain.stopPerformanceProfiler()
print("Performance profiling complete!")
```

### Advanced Example 3: Batch Processing

```python
import pynlm
import time

# Create brain and world
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)

# Generate multiple parallel scenarios
num_scenarios = 10
steps_per_scenario = 200
all_actions = []

start_time = time.time()

for scenario in range(num_scenarios):
    print(f"Running scenario {scenario + 1}/{num_scenarios}")
    
    # Reset for each scenario
    world.reset()
    agent.reset()
    
    # Run batch of steps for this scenario
    scenario_actions = []
    for step in range(steps_per_scenario):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        scenario_actions.append(action)
    
    all_actions.extend(scenario_actions)

elapsed = time.time() - start_time
print(f"Batch processing complete! Processed {len(all_actions)} actions in {elapsed:.2f} seconds")
```

### Advanced Example 4: Configuration Validation and Comparison

```python
import pynlm

# Create two different configurations for comparison
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 1000)
config1.set("plasticity.stdp.enable", True)

config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 5000)
config2.set("plasticity.stdp.enable", False)

# Validate configurations
print("Validating configurations...")
valid1 = pynlm.validateConfig(config1)
valid2 = pynlm.validateConfig(config2)

print(f"Config 1 valid: {valid1}")
print(f"Config 2 valid: {valid2}")

# Compare configurations
print("\nComparing configurations...")
differences = pynlm.compareConfigs(config1, config2)
for diff in differences:
    print(f"  - {diff}")

# Get detailed differences
print("\nDetailed differences:")
detailed_diffs = pynlm.getConfigDifferences(config1, config2)
for key, diff in detailed_diffs.items():
    print(f"  Key: {key}")
    if diff["value1"] is not None and diff["value2"] is not None:
        print(f"    Config 1: {diff['value1']}")
        print(f"    Config 2: {diff['value2']}")
    elif diff["value1"] is not None:
        print(f"    Config 1: {diff['value1']}")
        print(f"    Config 2: <not set>")
    elif diff["value2"] is not None:
        print(f"    Config 1: <not set>")
        print(f"    Config 2: {diff['value2']}")

# Normalize config for optimal performance
print("\nNormalizing configuration...")
normalized_config = pynlm.normalizeConfig(config1)
print(f"Normalized neuron count: {normalized_config.get('brain.neuron_count', 'not set')}")
```

### Advanced Example 5: Debugging and Error Handling

```python
import pynlm

# Create brain with error handling
try:
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    print("Brain created successfully")
    
    brain.initialize()
    print("Brain initialized successfully")
    
    # Test error handling with invalid configuration
    try:
        invalid_config = None
        brain2 = pynlm.createBrain(invalid_config)
    except Exception as e:
        print(f"Caught expected error: {type(e).__name__}: {e}")
    
    # Debug neuron connections
    print("\nDebugging neuron connections...")
    test_neuron_id = None  # Would need to get actual neuron ID
    if test_neuron_id:
        connections = brain.debugNeuronConnections(test_neuron_id)
        print(f"Found {len(connections)} connections for neuron {test_neuron_id}")
    
    # Profile memory usage
    print("\nProfiling memory usage...")
    memory_profile = brain.profileMemoryUsage()
    print(f"Memory usage: {memory_profile.memoryUsageMB:.1f} MB")
    
except Exception as e:
    print(f"Error during simulation: {type(e).__name__}: {e}")
    raise
```

---

## Advanced Topics

### Configuration Validation
NLM provides configuration validation to help you catch errors early:

```python
# Check if your configuration is valid
if pynlm.validateConfig(config):
    print("Configuration is valid")
else:
    print("Configuration has issues - check the logs")
```

### Performance Monitoring
Monitor your brain's performance during simulation:

```python
# Get performance statistics
stats = brain.getPerformanceStats()
print(f"Processing: {stats.neuronsPerSecond:.0f} neurons/sec")
print(f"Memory: {stats.memoryUsageMB:.1f} MB")
```

### Batch Processing for Speed
Process multiple steps efficiently:

```python
# Batch process for better performance
steps = list(range(1000))
times = [i * 0.001 for i in steps]
brain.batchStep(steps, times)
```

### Smart Initialization
Create brains optimized for specific tasks:

```python
# Create brain for different task types
brain_simple = pynlm.createBrainForTask(pynlm.TaskType.SimpleControl, 500)
brain_complex = pynlm.createBrainForTask(pynlm.TaskType.ComplexNavigation, 15000)
brain_learning = pynlm.createBrainForTask(pynlm.TaskType.LearningAndMemory, 3000)
```

### Research Configuration
Set up brain for research-grade simulations:

```python
# Configure for advanced research
pynlm.configureForResearch(brain)
```

### Embedded Configuration
Optimize for embedded systems:

```python
# Configure for embedded systems
pynlm.configureForEmbedded(brain)
```

---

## Expert Features

When you're ready for advanced usage, explore:

1. **Custom Agent Behaviors**: Create agents with custom sensory processing and motor control
2. **Advanced Plasticity Rules**: Add custom learning mechanisms
3. **Research-Grade Configurations**: Optimize for scientific research
4. **Performance Tuning**: Fine-tune for specific hardware or use cases
5. **Debugging Tools**: Deep dive into brain internals and performance

For the complete expert documentation, see `HOW_TO_USE.md` and `docs/ARCHITECTURE.md`.

That's it! You're now ready for both basic and advanced NLM usage. The advanced examples above demonstrate the power of NLM's Python bindings for creating sophisticated neural simulations.
