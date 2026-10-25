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

### Example 3: Enhanced Brain Simulation (NEW!)

```python
import pynlm

# Create an enhanced brain with Phase 4 configuration (emerging cognition)
brain = pynlm.createBrainFromPhase(4)
brain.initialize()

# Enable all learning systems
brain.enableAllPlasticity()
brain.enableAllNeuromodulation()

# Run simulation with monitoring
for step in range(200):
    brain.step(step)
    
    if step % 50 == 0:
        metrics = brain.getPerformanceMetrics()
        print(f"Step {step}: {metrics['average_firing_rate']:.3f} Hz, "
              f"{metrics['firing_neurons']} neurons firing")

print("Enhanced simulation complete!")
```

### Example 4: Complete Agent with Learning (NEW!)

```python
import pynlm

# Setup enhanced brain and agent
brain = pynlm.createBrainFromPhase(3)  # Phase 3: World interaction
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(7.0, 7.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all adaptive systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run learning episode
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.updateDevelopment(0.1)
    
    if step % 100 == 0:
        stats = agent.getComprehensiveStats()
        print(f"Step {step}: Curiosity={stats['curiosity_level']:.3f}, "
              f"Neuromod={stats['neuromodulation_level']:.3f}")

print("Learning simulation complete!")
```

### Example 5: Phase-Specific Development (NEW!)

```python
import pynlm

# Test different developmental phases
for phase in [2, 3, 4, 6]:
    print(f"\n=== Testing Phase {phase} ===")
    
    # Create brain for specific phase
    brain = pynlm.createBrainFromPhase(phase, scale=0.5)  # Smaller for testing
    brain.initialize()
    
    # Configure for Phase {phase} capabilities
    if phase >= 4:
        brain.enableAllPlasticity()
        brain.enableAllNeuromodulation()
    
    # Run brief simulation
    for step in range(50):
        brain.step(step)
    
    # Get and display results
    metrics = brain.getPerformanceMetrics()
    learning_stats = brain.getLearningStats()
    
    print(f"Neurons: {metrics['total_neurons']}")
    print(f"Average firing rate: {metrics['average_firing_rate']:.3f} Hz")
    print(f"Total spikes: {metrics['total_spikes']}")

print("\nAll phases tested successfully!")
```

### Example 6: Real-Time Monitoring (NEW!)

```python
import pynlm
import time

# Create and initialize brain
brain = pynlm.createBrainFromPhase(4)
brain.initialize()

print("Starting real-time monitoring...")
print("Press Ctrl+C to stop")

start_time = time.time()

while True:
    # Get current metrics
    metrics = brain.getPerformanceMetrics()
    memory_stats = brain.getMemoryStats()
    
    # Display real-time information
    elapsed = time.time() - start_time
    print(f"\rTime: {elapsed:6.1f}s | "
          f"Rate: {metrics['average_firing_rate']:6.3f} Hz | "
          f"Neurons: {metrics['firing_neurons']:4d} | "
          f"Spikes: {metrics['total_spikes']:6d} | "
          f"Dev: {brain.getDevelopmentalStage():2d}", end="")
    
    # Step simulation
    brain.step(static_cast<size_t>(elapsed * 100.0))  # 100 steps per second
    
    time.sleep(0.01)  # Small delay for display

print("\nMonitoring stopped!")
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

# New enhanced features:
brain.enableAllPlasticity()             # Enable all learning mechanisms
brain.enableAllNeuromodulation()        # Enable all adaptive systems
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

### Project 2: Brain Watching a World (NEW!)

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

### Project 3: Complete Agent (NEW!)

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

## Quick Reference (NEW!)

| Advanced Feature | Code |
|------------------|------|
| Create phase-specific brain | `pynlm.createBrainFromPhase(4)` |
| Get performance metrics | `brain.getPerformanceMetrics()` |
| Enable all learning | `brain.enableAllPlasticity()` |
| Enable all adaptive systems | `brain.enableAllNeuromodulation()` |
| Get comprehensive stats | `agent.getComprehensiveStats()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Use enhanced monitoring: `metrics = brain.getPerformanceMetrics()`

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify agent has motor capabilities enabled

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if learning systems are enabled

**"My simulation is slow"**
- Use enhanced features: `brain.enableAllPlasticity()` for optimized performance
- Monitor with: `metrics = brain.getPerformanceMetrics()`
- Consider Phase 4 for better performance optimization

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Read `EnhancedPythonAPIDocumentation.md` for advanced features!
4. Experiment with different configurations and phases!

That's it! You're now ready to use the enhanced NLM.

**New Features Available:**
- Phase-specific brain configurations
- Real-time performance monitoring
- Comprehensive learning system analysis
- Enhanced error handling and exception management
- Advanced simulation control and customization
- Better integration with Python tools and workflows
