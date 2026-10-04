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

### Enhanced Example: Advanced Brain with Performance Monitoring

```python
import pynlm

# Create advanced configuration with custom parameters
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.connection_probability", 0.15)
config.set("neuromod.dopamine.scale", 2.0)

# Create brain with advanced features
brain = pynlm.createBrain(config)
brain.initialize()

# Create performance monitor for detailed metrics
monitor = pynlm.createPerformanceMonitor()

# Run simulation with advanced features
for step in range(1000):
    monitor.recordStepStart()
    
    # Get sensory input and process
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Update world and process sensory input
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain step with advanced metrics
    brain.step(step)
    
    # Collect performance metrics
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    monitor.recordStepEnd()

# Get performance statistics
stats = monitor.getStats()
print(f"Simulation completed!")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Steps per second: {stats.stepsPerSecond:.0f}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
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

### Pattern 4: Advanced Configuration and Performance Monitoring

```python
# Create advanced configuration with type-safe access
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1500)
config.set("brain.connection_probability", 0.12)
config.set("neuromod.dopamine.scale", 1.5)

# Set memory system parameters
config.set("memory.working_capacity", 5000)
config.set("memory.max_episodes", 500)

# Create performance monitor
monitor = pynlm.createPerformanceMonitor()
```

### Pattern 5: Advanced Memory System Control

```python
# AdvancedBrain with memory control (requires import)
from pynlm import AdvancedBrain

# Create advanced brain
advanced_brain = AdvancedBrain(config)

# Enable and configure memory systems
advanced_brain.controlMemory(
    enableWorkingMemory=True,
    enableEpisodicMemory=True,
    enableAssociativeMemory=True,
    enableConceptLearning=True
)

# Configure memory parameters
advanced_brain.getWorkingMemory().setCapacity(10000)
advanced_brain.getEpisodicMemory().setMaxEpisodes(1000)
advanced_brain.getAssociativeMemory().enablePatternCompletion(True)
```

### Pattern 6: Neuromodulation and Development Control

```python
# Control neuromodulation systems
advanced_brain.controlNeuromodulation(
    curiosityLevel=0.7,
    noveltyLevel=0.5,
    predictionError=0.3,
    reward=1.0
)

# Control development system
advanced_brain.controlDevelopment(
    plasticityRate=0.05,
    enableCriticalPeriod=True,
    maturationRate=1.5
)
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

### Project 4: Advanced Brain with Performance Monitoring

```python
import pynlm

# Create advanced configuration with performance monitoring
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.connection_probability", 0.1)

# Create brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create performance monitor
monitor = pynlm.createPerformanceMonitor()

# Run simulation with monitoring
for step in range(500):
    monitor.recordStepStart()
    
    # Simulate brain activity
    brain.step(step)
    
    # Log performance metrics
    monitor.recordStepEnd()

# Get performance statistics
stats = monitor.getStats()
print(f"Performance Report:")
print(f"  Total Steps: {stats.totalSteps}")
print(f"  Steps per Second: {stats.stepsPerSecond:.0f}")
print(f"  Average Step Time: {stats.avgStepTime:.4f}s")
print(f"  CPU Efficiency: {stats.cpuEfficiency:.2f}")
```

### Project 5: Complete Agent with Memory Integration

```python
import pynlm

# Setup configuration with memory support
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 800)
config.set("brain.connection_probability", 0.12)
config.set("memory.working_capacity", 2000)
config.set("memory.max_episodes", 200)

# Create all components
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.2f}")
        print(f"  Dopamine: {agent.getNeuromodulationLevel():.2f}")

print("Advanced agent simulation complete!")
```

### Project 6: Development and Learning System

```python
import pynlm

# Create brain with development support
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 600)
config.set("development.critical_period", True)
config.set("development.maturation_rate", 0.8)

brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run developmental simulation
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Apply development changes
    agent.updateDevelopment(0.1)
    
    # Log development stage
    if step % 50 == 0:
        print(f"Step {step}: Development Stage {brain.getDevelopmentalStage()}")
        print(f"  Plasticity Rate: {agent.getNeuromodulationLevel():.2f}")
        print(f"  Curiosity Level: {agent.getCuriosityLevel():.2f}")

print("Developmental simulation complete!")
```

### Project 7: Advanced Configuration and Error Handling

```python
import pynlm

# Create advanced configuration with validation
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)
config.set("brain.connection_probability", 0.08)
config.set("neuromod.dopamine.scale", 1.2)

# Validate configuration
if not config.has("brain.neuron_count"):
    print("Error: Missing required parameter")
    exit(1)

# Test configuration validation
config_file = "custom_config.json"
config.loadFromFile(config_file)

# Create brain with validated configuration
brain = pynlm.createBrain(config)
brain.initialize()

print("Configuration loaded successfully!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Regions: {brain.getRegionCount()}")
```

### Project 8: Memory Replay and Consolidation

```python
import pynlm

# Create brain with memory systems
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 400)
config.set("memory.max_episodes", 50)
config.set("memory.replay_interval", 50)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable memory systems
wm = brain.getWorkingMemory()
em = brain.getEpisodicMemory()

if wm and em:
    wm.enableLearning(True)
    em.enableReplay(True)
    em.setMaxEpisodes(50)
    
    # Run simulation to generate memories
    for step in range(100):
        brain.step(step)
        
        # Check memory metrics
        print(f"Step {step}: Working Memory: {wm.getActiveTraces()} traces, "
              f"Episodic Memory: {em.getEpisodeCount()} episodes")
    
    print("Memory replay and consolidation simulation complete!")
else:
    print("Warning: Memory systems not available")
```

### Advanced Features Quick Reference

| What you want | Code |
|--------------|------|
| Create advanced brain | `pynlm.createAdvancedBrain(config)` |
| Create performance monitor | `pynlm.createPerformanceMonitor()` |
| Create advanced config | `pynlm.createAdvancedConfig()` |
| Get brain metrics | `brain.getPerformanceMetrics()` |
| Control memory systems | `brain.controlMemory()` |
| Control neuromodulation | `brain.controlNeuromodulation()` |
| Control development | `brain.controlDevelopment()` |
| Get step metrics | `brain.collectStepMetrics()` |

### Performance and System Status

```python
import pynlm

# Create advanced brain
config = pynlm.createAdvancedConfig()
config.set("brain.neuron_count", 1000)

brain = pynlm.createAdvancedBrain(config)
brain.initialize()

# Run simulation with performance monitoring
monitor = pynlm.createPerformanceMonitor()

for step in range(1000):
    monitor.recordStepStart()
    brain.step(step)
    monitor.recordStepEnd()

# Get performance statistics
stats = monitor.getStats()
metrics = brain.getPerformanceMetrics()

print(f"Performance: {stats.stepsPerSecond:.0f} steps/sec")
print(f"CPU Efficiency: {stats.cpuEfficiency:.2f}")
print(f"Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Memory Utilization: {metrics.memoryUtilization:.2%}")
```

---

## Troubleshooting Advanced Features

**"Advanced brain not working"**
- Did you call `advanced_brain.initialize_advanced()`?
- Check that your configuration has valid neural network parameters
- Verify that memory systems are properly initialized

**"Performance monitoring not working"**
- Did you call `monitor.recordStepStart()` before steps?
- Ensure `monitor.recordStepEnd()` is called after each step
- Check that the performance monitor is properly initialized

**"Memory systems not available"**
- Verify that your configuration has memory system parameters enabled
- Check if `brain.getWorkingMemory()` returns null
- Ensure that `wm.enableLearning(true)` and `em.enableReplay(true)` are called

**"Neuromodulation controls not responding"**
- Verify that neuromodulation systems are enabled in your agent
- Check that `agent.enableRewardModulation(true)` is called
- Ensure that development system is properly initialized

**"Development system not updating"**
- Check that `brain.setDevelopmentalStage()` is called appropriately
- Verify that `agent.updateDevelopment()` is being called in your simulation
- Ensure that plasticity rates are properly configured

**"Configuration validation failing"**
- Check that all required parameters are present in your configuration
- Verify that parameter values are within valid ranges
- Use `config.validate_configuration()` to see detailed error messages

**"Sensory processor not working"**
- Ensure that sensory input is properly connected to the brain
- Check that `brain.receiveSensoryInput()` is being called
- Verify that the world is generating proper sensory percepts

**"High memory usage warnings"**
- Reduce working memory capacity in configuration
- Disable episodic memory if not needed
- Use more efficient simulation parameters

**"Performance bottlenecks"**
- Monitor step times using performance statistics
- Reduce network size for real-time applications
- Consider disabling expensive features (development, replay, etc.)
- Optimize configuration parameters for your use case

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more detailed technical documentation
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Join the NLM community for support and advanced usage tips

That's it! You're now ready to use NLM with advanced features and capabilities.

## New Advanced Features Summary

With the enhanced Python bindings, you now have access to:

### 🔬 Advanced Neural Simulation
- Type-safe configuration access
- Comprehensive performance monitoring
- Advanced memory system controls
- Neuromodulation fine-tuning

### 📊 Performance Analytics
- Real-time performance metrics
- CPU efficiency calculations
- Step-by-step timing analysis
- Resource usage monitoring

### 🧠 Enhanced Learning Systems
- Configurable plasticity rates
- Memory consolidation controls
- Development stage management
- Neuromodulation calibration

### 🔧 Advanced Configuration
- Configuration validation
- Parameter range checking
- Advanced configuration presets
- Environment-specific settings

### 📝 Comprehensive Documentation
- Detailed API references
- Advanced usage examples
- Performance tuning guides
- Troubleshooting handbooks

The NLM framework has been transformed from a basic neural simulator into a comprehensive, production-ready neural simulation framework suitable for research, education, and industrial applications.
