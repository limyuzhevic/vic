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

### Example 2: Enhanced Brain with Research Features

```python
import pynlm

# Create an enhanced brain with research capabilities
brain = pynlm.createResearchBrain(500, True)

# Create performance profiler
profiler = pynlm.PerformanceProfiler()

# Run simulation with profiling
profiler.start()
for step in range(100):
    brain.step(step)
    profiler.step()
profiler.stop()

# Analyze performance
metrics = profiler.getMetrics()
print(f"Average step time: {profiler.getAverageStepTime():.3f}ms")
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
```

### Example 2b: Brain in a Simple World (Enhanced)

```python
import pynlm

# Create everything with enhanced configuration
config = pynlm.createEnhancedConfig()
config.setNeuronCount(200)
config.setConnectionProbability(0.1)

brain = pynlm.createBrain(config.getInternalConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable research features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run for 50 steps with profiling
profiler = pynlm.PerformanceProfiler()
profiler.start()

for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action
    
    # Apply reward modulation
    reward = percept.getInternal()[0] if percept.getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)

profiler.stop()

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())

# Analyze performance
metrics = profiler.getMetrics()
print(f"Performance: {profiler.getAverageStepTime():.3f}ms average step time")
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

### Pattern 1: Run a Simulation with Profiling

```python
def run_profiled_simulation(brain, world, agent, num_steps):
    import pynlm
    
    # Start profiling
    profiler = pynlm.PerformanceProfiler()
    profiler.start()
    
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
        
        # 7. Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)

    profiler.stop()
    
    # Return results and performance metrics
    return {
        'brain': brain,
        'profiler': profiler,
        'metrics': profiler.getMetrics()
    }

# Use it like this:
result = run_profiled_simulation(brain, world, agent, 1000)
print(f"Performance: {result['profiler'].getAverageStepTime():.3f}ms per step")
```

### Pattern 2: Get Brain Stats (Enhanced)

```python
import pynlm

# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())

# Enhanced analytics with research utilities
brain_stats = brain.analyzeNeuralActivity()
print("Enhanced stats:")
for key, value in brain_stats.items():
    print(f"  {key}: {value}")
```

### Pattern 3: Enable Brain Features (Research Edition)

```python
import pynlm

# These are all optional and research-focused:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Advanced neuromodulation research
if hasattr(brain, 'getDopamine'):
    brain.getDopamine().signalReward(1.0)
    
if hasattr(brain, 'getCuriosity'):
    brain.getCuriosity().setExplorationRate(0.5)
    
if hasattr(brain, 'getNovelty'):
    brain.getNovelty().setSensitivity(0.3)
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

### Enhanced Features:
- `pynlm.createResearchBrain(neuron_count, enable_plasticity)`
  Creates a brain with research-oriented settings

- `pynlm.createEnhancedConfig()`
  Creates configuration with validation and schema

- `pynlm.PerformanceProfiler()`
  Performance profiling for research optimization

---

## Mini Projects

### Project 1: Silent Brain with Profiling (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run with performance profiling
profiler = pynlm.PerformanceProfiler()
profiler.start()

for i in range(10):
    brain.step(i)
    profiler.step()

profiler.stop()

print("Silent brain test done!")
print(f"Performance: {profiler.getAverageStepTime():.3f}ms per step")
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
```

### Project 2: Brain Watching a World (Enhanced)

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

# Enable research features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Watch the world for 30 steps with profiling
profiler = pynlm.PerformanceProfiler()
profiler.start()

for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
    # Apply reward modulation for learning
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)

profiler.stop()

print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print(f"Performance: {profiler.getAverageStepTime():.3f}ms per step")
```

### Project 3: Complete Agent with Research Features

```python
import pynlm

# Setup
config = pynlm.createEnhancedConfig()
config.setNeuronCount(500)
config.setConnectionProbability(0.1)
brain = pynlm.createBrain(config.getInternalConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable research capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Start profiling
profiler = pynlm.PerformanceProfiler()
profiler.start()

# Run agent with performance tracking
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Performance: {profiler.getAverageStepTime():.3f}ms per step")
        print(f"  Firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development: {brain.getDevelopmentalStage()}")

profiler.stop()

print("Agent simulation complete!")

# Final performance report
metrics = profiler.getMetrics()
print(f"\n=== Performance Report ===")
print(f"Total time: {metrics.totalTimeMs:.2f}ms")
print(f"Average step time: {profiler.getAverageStepTime():.3f}ms")
print(f"Min step time: {profiler.getMinStepTime():.3f}ms")
print(f"Max step time: {profiler.getMaxStepTime():.3f}ms")
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

### Enhanced Quick Reference

| What you want | Code |
|--------------|------|
| Create research brain | `pynlm.createResearchBrain(neuron_count, plasticity)` |
| Create enhanced config | `pynlm.createEnhancedConfig()` |
| Start profiling | `profiler = pynlm.PerformanceProfiler()` |
| Analyze performance | `brain.analyzeNeuralActivity()` |
| Store experience | `brain.storeExperience(...)` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Enable profiling to see performance: `profiler = pynlm.PerformanceProfiler()`

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that `agent.decodeMotorCommand()` is returning valid actions

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Use profiling to identify performance bottlenecks

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
   - Covers building the C++ project from source
   - Detailed Python API documentation
   - Configuration options and examples

2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
   - System architecture overview
   - Component interconnections
   - Research design patterns

3. **Experiment with different configurations!**
   - Try different neuron counts
   - Adjust plasticity parameters
   - Test neuromodulation effects

4. **Use the enhanced Python API for research**
   - `pynlm.createResearchBrain()` for optimized research setup
   - `pynlm.PerformanceProfiler()` for performance analysis
   - `brain.analyzeNeuralActivity()` for detailed statistics
   - `config = pynlm.createEnhancedConfig()` for validated configuration

That's it! You're now ready to use NLM with advanced research capabilities.

---

## Advanced Research Features (Requires `pynlm_enhanced`)

The enhanced Python bindings (`pynlm_enhanced`) provide additional research capabilities:

- **Performance profiling** for simulation optimization
- **Memory analysis** for studying memory systems
- **Neuromodulation control** for studying brain chemicals
- **Configuration validation** for ensuring valid parameter combinations
- **Research utilities** for batch experiments and statistical analysis
- **Development stage control** for studying brain maturation

To use enhanced features, import from `pynlm_enhanced` instead of `pynlm`.

Example:
```python
from pynlm_enhanced import *

# Use enhanced features
brain = createResearchBrain(1000, True)
profiler = PerformanceProfiler()
# ... rest of code
```
