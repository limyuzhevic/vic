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

### Example 3: Advanced Agent with Custom Behaviors (Power User)

```python
import pynlm
import time

# 1. Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)  # Larger brain
config.set("brain.synapse_density", 0.05)
config.set("brain.v_thresh", -45.0)   # Lower threshold for higher firing
config.set("brain.tau_mem", 15.0)    # Slower decay

brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create agent with advanced features
agent = pynlm.createAgentBrain(brain)

# 3. Create world
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
world.reset()
world.setAgentStart(15.0, 15.0)

# 4. Initialize agent
agent.initialize(world)

# 5. Enable advanced learning systems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableCuriosity(True)             # Explore new things
agent.enableDevelopment(True)           # Brain matures over time
agent.enableStructuralPlasticity(True)  # Grow new connections

# 6. Configure personality traits
agent.setPersonality(0.7, 0.5, 0.8, 0.6)  # curiosity, boldness, exploration, learning

# 7. Set goals for goal-oriented behavior
agent.setGoalPosition(25.0, 25.0)        # Explore towards corner

# 8. Configure task priorities for multi-tasking
agent.setTaskPriority("exploration", 0.8)
agent.setTaskPriority("resource_collection", 0.6)
agent.setTaskPriority("avoidance", 0.9)

# 9. Enable advanced behaviors
agent.enableThreatDetection(True)
agent.enableSocialLearning(False)
agent.enableMultiTaskLearning(True)
agent.enableTransferLearning(True)
agent.enableHierarchicalRL(True)
agent.enableMetaCognition(True)

# 10. Setup performance monitoring
agent.enablePerformanceTracking(True)

# 11. Run advanced simulation
print("Starting advanced agent simulation...")
start_time = time.time()

for step in range(1000):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with advanced agent
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action with curiosity and exploration
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward with prediction
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    predicted = 0.0  # In real scenarios, this would be predicted reward
    agent.applyRewardModulation(reward, predicted)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress every 100 steps
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Position: ({world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f})")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Decision confidence: {agent.getDecisionConfidence():.3f}")
        print(f"  Exploration rate: {agent.getExplorationRate():.3f}")

end_time = time.time()
print(f"\nSimulation complete! Time: {end_time - start_time:.2f}s")
print(f"Total rewards: {agent.getTotalReward():.2f}")
print(f"Final behavior profile: {agent.getBehaviorProfile()}")

# 12. Save agent state for later
agent.saveState("advanced_agent_checkpoint.bin")
```

---

## Advanced Performance Optimization

### Performance-Optimized Agent Configuration

```python
import pynlm

# Create performance-optimized agent
config = pynlm.createDefaultConfig()

# Memory optimization
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.02)  # Fewer synapses for speed
config.set("brain.connection_probability", 0.03)

# Speed optimizations
config.set("brain.v_thresh", -50.0)    # Standard threshold
config.set("brain.tau_mem", 20.0)     # Standard decay
config.set("brain.tau_ref", 2.0)      # Standard refractory period

# Performance mode configuration
config.set("performance.mode", "throughput")  # "throughput" or "latency"
config.set("performance.optimization_level", 2)  # 0=basic, 1=intermediate, 2=advanced, 3=expert

brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Optimize for performance
agent.optimizeForThroughput()  # or agent.optimizeForLatency()
agent.enablePerformanceMonitoring(True)
```

### Custom Logging and Debugging

```python
import pynlm
import logging

# Configure advanced logging
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Setup performance monitoring
perf_monitor = pynlm.getPerformanceMonitor()

# Enable detailed logging
nlm_logger = pynlm.Logger.getGlobal()
nlm_logger.setLevel(nlm.LogLevel.Debug)
nlm_logger.enableAsyncLogging(True)

# Add custom filters
nlm_logger.addFilter(nlm.LevelFilter(nlm.LogLevel.Info))
nlm_logger.addFilter(nlm.PatternFilter("ERROR|CRITICAL"))

# Performance monitoring
print("Starting performance tracking...")

for step in range(500):
    world.update(0.1)
    
    # Track operation performance
    nlm.getPerformanceMonitor().startOperation("world_update")
    
    percept = world.getSensoryPercept()
    
    nlm.getPerformanceMonitor().endOperation("world_update")
    
    agent.processSensoryInput(percept)
    
    # Track brain step performance
    nlm.getPerformanceMonitor().startOperation("brain_step")
    brain.step(step)
    nlm.getPerformanceMonitor().endOperation("brain_step")
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        # Log performance statistics
        stats = nlm.getPerformanceMonitor().getStats()
        print(f"Step {step} Performance:")
        for op in stats:
            print(f"  {op.name}: {op.averageDuration:.2f}us avg, {op.callCount} calls")
        
        # Log brain stats
        print(f"    Brain: {brain.getFiringNeuronCount()} firing, {brain.getAverageFiringRate():.2f} Hz")
        print(f"    Agent: curiosity={agent.getCuriosityLevel():.3f}, novelty={agent.getNoveltyLevel():.3f}")
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

# Advanced features:
agent.setPersonality(0.5, 0.5, 0.5, 0.5)  # Custom personality
agent.setGoalPosition(10.0, 10.0)         # Goal-oriented behavior
agent.enableMultiTaskLearning(True)     # Multi-task learning
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

## Advanced Features for Power Users

### 1. Performance Optimization
- `agent.optimizeForThroughput()` - Maximum speed
- `agent.optimizeForLatency()` - Minimum delay
- `agent.enablePerformanceMonitoring(True)` - Track performance metrics

### 2. Custom Agent Behaviors
- `agent.setPersonality(curiosity, boldness, exploration, learning)` - Customize agent personality
- `agent.setGoalPosition(x, y)` - Goal-oriented behavior
- `agent.setTaskPriority(task, priority)` - Multi-task learning

### 3. Advanced Learning Systems
- `agent.enableTransferLearning(True)` - Learn from past experiences
- `agent.enableHierarchicalRL(True)` - Hierarchical reinforcement learning
- `agent.enableMetaCognition(True)` - Think about thinking

### 4. Enhanced Logging
- `nlm.Logger.getGlobal().enableAsyncLogging(True)` - High-performance logging
- `nlm.getPerformanceMonitor().startOperation("operation")` - Track performance
- `nlm.getPerformanceMonitor().endOperation("operation")` - End performance tracking

### 5. Configuration Management
- Custom brain configurations for specific tasks
- Performance tuning parameters
- Memory budget management

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that agent is enabled

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neuron activity

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that agent curiosity and exploration are enabled

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try the advanced examples in this guide
5. Explore the performance optimization features
6. Implement custom agent behaviors

That's it! You're now ready to use NLM at all levels, from beginner to power user!
