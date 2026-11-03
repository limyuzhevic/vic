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

### Project 3: Complete Agent with Advanced Features

```python
import pynlm

# Setup with advanced configuration
config = pynlm.createAdvancedConfig()  # New: Pre-configured advanced settings
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

print("Agent simulation complete!")
```

---

## Configuration Helpers for Different Scenarios

### Quick Configuration Setup
```python
# Quick setup with specific neuron/region counts
config = pynlm.quickCreateConfig(neuronCount=2000, regionCount=8)
brain = pynlm.createBrain(config)

# Advanced configuration for research
research_config = pynlm.createResearchConfig()

# Configuration optimized for exploration
exploration_config = pynlm.createExplorationConfig()

# Configuration optimized for exploitation
exploitation_config = pynlm.createExploitationConfig()

# Configuration optimized for visualization
viz_config = pynlm.createVisualizationConfig()
```

### Configuration Validation
```python
# Validate your configuration before using it
is_valid, errors = pynlm.validateConfig(config)
if not is_valid:
    print("Configuration errors:", errors)
    # Fix configuration issues...
```

### Configuration Merging
```python
# Merge two configurations, with override taking precedence
merged_config = pynlm.mergeConfigs(base_config, override_config)
brain = pynlm.createBrain(merged_config)
```

### Configuration Statistics
```python
# Get statistics about your configuration
stats = pynlm.getConfigStats(config)
print("Total keys:", stats["total_keys"])
print("Configuration efficiency:", (stats["int_values"] + stats["float_values"]) * 100 / stats["total_keys"], "%")
```

---

## Advanced Python API Features

### Enhanced AgentBrain Methods
```python
# Advanced AgentBrain methods
agent = pynlm.createAgentBrain(brain)

# Get internal firing rates (advanced debugging)
internal_rates = agent.getInternalFiringRates()

# Get self-model confidence (advanced cognition)
confidence = agent.getSelfModelConfidence()

# Encode text as sensory patterns (Phase 6 feature)
text_pattern = agent.encodeText("hello world")

# Get concept count (advanced learning)
concept_count = agent.getConceptCount()

# Get complete agent body state
body = agent.getAgentBody()

# Set agent starting position
agent.setAgentStart(10.0, 10.0)

# Comprehensive debug information
agent.printDebugInfo()

# Run benchmark episode
avg_reward, steps = agent.benchmarkEpisode(world, 1000)

# Export/import brain state
agent.exportBrainState("my_brain.bin")
agent.importBrainState("saved_brain.bin")
```

### Brain Configuration
```python
# Get brain configuration
config = agent.getConfig()
config.set("neuron_count", 5000)
config.set("plasticity.stdp.enable", True)
```

---

## Debugging and Performance Monitoring

### Basic Debugging
```python
import pynlm

# Create debugger
debugger = pynlm.createDebugger()

# Configure debugger
debugger.startBenchmark("my_simulation")
debugger.recordSystemInfo("Linux", "GCC 11.0", 8, 16 * 1024 * 1024 * 1024)
debugger.recordConfigSummary({"neuron_count": 2000, "region_count": 8})
debugger.recordMemoryInfo(100, 150, 50, 80)

# Run simulation
for step in range(100):
    # ... your simulation code ...
    debugger.recordStep(step, spike_count, firing_rate, active_neurons, "MoveForward")
    # ... rest of simulation ...

debugger.recordPerformanceSnapshot(metrics)

# Generate comprehensive debug report
debug_report = debugger.generateDebugReport()
print(debug_report)

# Stop benchmark
debugger.stopBenchmark()
```

### Performance Profiling
```python
# Create performance profiler
profiler = pynlm.createPerformanceProfiler()

# Profile operations
profiler.start("brain_initialization")
# ... initialize brain ...
profiler.stop("brain_initialization")

profiler.start("simulation_step")
# ... run simulation step ...
profiler.stop("simulation_step")

# Get performance report
profile_report = profiler.getPerformanceReport()
print(profile_report)
```

### Memory Tracking
```python
# Create memory tracker
memory_tracker = pynlm.createMemoryTracker()

# Track memory allocations
memory_tracker.recordAllocation(1024, "brain_allocation")
memory_tracker.recordDeallocation(512, "brain_allocation")

# Get memory report
memory_report = memory_tracker.getMemoryReport()
print(memory_report)
```

---

## Examples of Different Simulation Scenarios

### 1. Learning and Exploration
```python
# Create exploration-optimized configuration
config = pynlm.createExplorationConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable curiosity-driven exploration
agent = pynlm.createAgentBrain(brain)
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)
```

### 2. Skill Exploitation
```python
# Create exploitation-optimized configuration
config = pynlm.createExploitationConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Focus on refining learned behaviors
agent = pynlm.createAgentBrain(brain)
agent.enableCuriosity(False)  # Reduce exploration
agent.enableDevelopment(True)  # Continue maturation
```

### 3. Research Setup
```python
# Create research-grade configuration
config = pynlm.createResearchConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable all plasticity mechanisms
agent = pynlm.createAgentBrain(brain)
agent.enableStructuralPlasticity(True)
agent.enableDevelopmentalPlasticity(True)
agent.enableSocialLearning(True)
```

### 4. Visualization and Debugging
```python
# Create visualization-optimized configuration
config = pynlm.createVisualizationConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable performance tracking
agent = pynlm.createAgentBrain(brain)
agent.enablePerformanceTracking(True)
```

---

## Troubleshooting Advanced Features

### "My simulation is too slow"
```python
# Check performance metrics
metrics = agent.getPerformanceMetrics()
print("Steps per second:", metrics.stepsPerSecond)
print("Efficiency:", metrics.efficiency)

# Optimize configuration
config = pynlm.createScalableConfig(numRegions=16, connectivity=0.12)
```

### "Memory usage is high"
```python
# Check memory usage
memory_report = memory_tracker.getMemoryReport()
print(memory_report)

# Profile memory allocations
profiler = pynlm.createMemoryProfiler()
profiler.start("memory_intensive_operation")
# ... run operation ...
profiler.stop("memory_intensive_operation")
```

### "Debugging information is too verbose"
```python
# Limit debug information
debugger.setVerbosityLevel(2)  # 0=quiet, 1=summary, 2=detailed

# Filter debug messages
debugger.setLogFilter(["simulation", "performance"])
```

---

## Quick Reference (Extended)

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

**NEW ADVANCED FEATURES:**

| Advanced feature | Method |
|-----------------|--------|
| Configuration helpers | `pynlm.quickCreateConfig()`, `pynlm.createExplorationConfig()`, etc. |
| Debug configuration | `pynlm.validateConfig()`, `pynlm.mergeConfigs()`, `pynlm.getConfigStats()` |
| Enhanced AgentBrain | `agent.getInternalFiringRates()`, `agent.encodeText()`, `agent.benchmarkEpisode()` |
| Brain configuration | `agent.getConfig()`, `agent.setAgentStart()` |
| Comprehensive debugging | `pynlm.createDebugger()` |
| Performance profiling | `pynlm.createPerformanceProfiler()` |
| Memory tracking | `pynlm.createMemoryTracker()` |

---

## Next Steps

When you're comfortable:
1. **Read `HOW_TO_USE.md`** for more technical details
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations** - each scenario has its own optimized settings!
4. **Use debugging tools** to optimize your simulations
5. **Create your own configuration helpers** for specialized scenarios!

That's it! You're now ready to use NLM with both basic and advanced features.
