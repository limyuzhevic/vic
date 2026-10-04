# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

**NEW: Enhanced Features**

NLM now includes enhanced versions of the core brain system with advanced monitoring and error handling:

### Enhanced Brain (NLM Enhanced)

The EnhancedBrain class provides:
- **Performance monitoring**: Track execution times and memory usage
- **Error detection**: Automatic error detection and system health monitoring
- **Memory pools**: Enhanced memory management with leak detection
- **SIMD profiling**: Advanced SIMD vectorization performance analysis
- **System health monitoring**: Comprehensive system health status

### Advanced Configuration

Create an advanced configuration with optimized settings:

```python
import pynlm

# Create an advanced configuration
config = pynlm.createAdvancedConfig()

# Modify settings
config.set("brain.neuron_count", 10000)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)

# Create enhanced brain
brain = pynlm.createBrain(config)
```

### Performance Monitoring

The enhanced brain provides detailed performance monitoring:

```python
# Create enhanced brain with monitoring
brain = pynlm.createBrain(config)
brain.enableMonitoring(True)  # Enable performance monitoring

# Run simulation
for i in range(1000):
    brain.step(i)

# Get performance statistics
stats = brain.getPerformanceStats()
print(stats)

# Get system health
health = brain.getSystemHealth()
print("System status:", health.getStatusString())
```

### Memory Pool Statistics

Monitor memory pool usage:

```python
# Get memory pool statistics
memory_stats = brain.getMemoryPoolStats()
print(memory_stats)
```

### Context Manager

Use the enhanced brain with automatic cleanup:

```python
# Use enhanced brain with context manager
with pynlm.BrainContext(config) as brain:
    for i in range(100):
        brain.step(i)
    
    print(f"Simulation completed: {brain.getTotalSpikeCount()} spikes")
```

### Quick Comparison

| Feature | Basic Brain | Enhanced Brain |
|---------|-------------|----------------|
| Performance monitoring | ❌ | ✅ |
| Error detection | ❌ | ✅ |
| Memory pool monitoring | ❌ | ✅ |
| SIMD profiling | ❌ | ✅ |
| System health monitoring | ❌ | ✅ |

### Example: Complete Enhanced Simulation

```python
import pynlm

def runEnhancedSimulation(steps=1000):
    # 1. Create configuration
    config = pynlm.createAdvancedConfig()
    
    # 2. Create enhanced brain
    brain = pynlm.createBrain(config)
    brain.enableMonitoring(True)  # Enable monitoring
    brain.enableErrorDetection(True)  # Enable error detection
    
    # 3. Initialize brain
    brain.initialize()
    
    # 4. Run simulation with monitoring
    for step in range(steps):
        brain.step(step)
        
        # Optional: Check system health periodically
        if step % 100 == 0:
            health = brain.getSystemHealth()
            if not health.isHealthy():
                print(f"Warning: System health issue at step {step}")
    
    # 5. Print results
    print(f"=== Enhanced Simulation Results ===")
    print(f"Steps: {steps}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
    
    # 6. Print detailed performance report
    print(brain.getPerformanceStats())
    
    # 7. Print memory pool statistics
    print(brain.getMemoryPoolStats())
    
    return brain

# Run enhanced simulation
brain = runEnhancedSimulation(1000)
```

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

### NEW: Enhanced Agent

Create an enhanced agent with advanced capabilities:

```python
import pynlm

# Setup
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
agent.enableMonitoring(True)  # Enable monitoring

# Run simulation
for step in range(500):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Get motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Periodically check system health
    if step % 100 == 0:
        health = agent.getSystemHealth()
        if health.status != health.Status.HEALTHY:
            print(f"Agent health warning: {health.status}")
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

**NEW: Enhanced Statistics**

| What you want | Code |
|--------------|------|
| Performance stats | `brain.getPerformanceStats()` |
| Memory pool stats | `brain.getMemoryPoolStats()` |
| System health | `brain.getSystemHealth()` |
| SIMD profiling | `simd_profiler.printPerformanceReport()` |

---

## Next Steps

When you're comfortable:

1. Read `HOW_TO_USE.md` for more details on advanced usage
2. **NEW: Try Enhanced Brain features for production use**
3. Experiment with different configuration options!

**NEW: Enhanced Brain Features**

- Performance monitoring for production simulations
- Error detection and recovery
- Memory leak detection and prevention
- SIMD optimization analysis
- System health monitoring
- Comprehensive logging and statistics

That's it! You're now ready to use NLM Enhanced with all the advanced features and monitoring capabilities.

For the complete documentation of enhanced features, see the updated `HOW_TO_USE.md` file.

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

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
