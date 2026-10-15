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

---

## Advanced Usage Guide

Now that you've mastered the basics, let's explore the advanced features available in NLM Phase 6!

### 1. Using Advanced Brain Systems

**Working Memory Integration**:
```python
import pynlm

# Create a complete agent setup
brain, world, agent = pynlm.createSimpleAgent()

# Enable advanced brain systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True) 
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Access advanced brain systems
wm = brain.getWorkingMemory()
em = brain.getEpisodicMemory()
ap = brain.getAssociativeMemory()
pred = brain.getPredictionSystem()
planner = brain.getPlanner()
concept = brain.getConceptFormation()
attention = brain.getAttention()

print("Working memory capacity:", wm.getCapacity())
print("Available brain systems:", {
    'working_memory': wm is not None,
    'episodic_memory': em is not None,
    'associative_memory': ap is not None,
    'prediction': pred is not None,
    'planner': planner is not None,
    'concepts': concept is not None,
    'attention': attention is not None,
})
```

**Memory-Enhanced Brain Function**:
```python
# Use working memory to store and retrieve patterns
pattern = [0.1, 0.5, 0.8, 0.3, 0.9]
wm.store(pattern, strength=1.5)
retrieved = wm.retrieve()

# Store episodic memories
agent = pynlm.createAgentBrain(brain)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
brain.step(step)

# Episodic memory item
from pynlm import ActionType
episode = {
    'timestamp': step,
    'sensory_state': [0.1, 0.2, 0.3],
    'action': ActionType.MoveForward,
    'reward': 1.0,
    'position_x': world.getAgentBody().x,
    'position_y': world.getAgentBody().y,
}
em.storeEpisode(episode)
```

### 2. Performance Monitoring and Debugging

**Real-time Performance Monitoring**:
```python
import pynlm
import time

# Create demo brain with monitoring
brain = pynlm.createDemoBrain()

# Run simulation with progress tracking
start_time = time.time()
spike_counts, firing_rates = pynlm.simulateWithPerformanceMonitoring(
    brain, 
    1000,
    progressCallback=lambda step, rate, duration: print(
        f"Step {step}: {rate:.2f} firing rate, {duration:.3f}s"
    )
)

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f}s")
print(f"Final firing rate: {firing_rates[-1]:.2f}")
print(f"Total spikes: {spike_counts[-1]}")
```

**Comprehensive Brain Statistics**:
```python
# Get detailed statistics
stats = pynlm.getBrainStatistics(brain)
print("=== BRAIN STATISTICS ===")
for key, value in stats.items():
    print(f"{key}: {value}")

# Capture debug snapshot
snapshot = pynlm.captureBrainSnapshot(brain)
print("=== SNAPSHOT ===")
for key, value in snapshot.items():
    print(f"{key}: {value}")
```

**Error Recovery and Simulation Safety**:
```python
# Run simulation with error handling and recovery
brain = pynlm.createDemoBrain()
steps_completed, errors = pynlm.runSimulationWithRecovery(
    brain,
    numSteps=500,
    timestep=0.1,
    maxErrors=3
)

print(f"Successfully completed {len(steps_completed)} steps")
if errors:
    print("Errors encountered:")
    for error in errors:
        print(f"  - {error}")
```

### 3. Batch Operations and Efficient Processing

**Batch Sensory Processing**:
```python
import pynlm

# Create multiple sensory inputs
sensor_inputs = []
for i in range(10):
    input = pynlm.SensoryInput()
    input.setData([i * 0.1, i * 0.2, i * 0.3])
    sensor_inputs.append(input)

# Process all inputs efficiently
brain = pynlm.createDemoBrain()
pynlm.processMultipleSensoryInputs(brain, sensor_inputs)
print(f"Processed {len(sensor_inputs)} sensory inputs")
```

**Configuration Management**:
```python
import pynlm
import json

# Create brain from configuration file
brain = pynlm.createBrainFromFile("my_config.json")

# Or save current brain state for later use
brain.save("checkpoint_state.bin")

# Load saved state
loaded_brain = pynlm.loadBrainState("checkpoint_state.bin")
```

### 4. Advanced Agent Behaviors

**Complete Agent with All Learning Systems**:
```python
def advanced_agent_simulation(num_steps=1000):
    """Run an advanced agent simulation with all learning systems enabled."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    
    # Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable ALL subsystems for maximum learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation loop with monitoring
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Get reward from internal signals
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Prediction error: {agent.getPredictionError():.3f}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
    
    print("Advanced agent simulation complete!")

# Run the simulation
advanced_agent_simulation(1000)
```

### 5. Utility Functions and Helper Tools

**Quick Brain Setup Utilities**:
```python
# Create pre-configured brains for different use cases
brain1 = pynlm.createDemoBrain()  # Standard demo configuration
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())  # Minimal setup
brain3 = pynlm.createBrain(config)  # Custom configuration
```

**Agent Helper Functions**:
```python
# Quick agent setup helper
def createAgentWithAllFeatures(neuron_count=500):
    """Create an agent with all features enabled."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    return brain, world, agent
```

### 6. Best Practices and Tips

**For Performance**:
- Use batch operations for processing multiple inputs
- Monitor performance with the built-in tools
- Use checkpoints for long-running simulations

**For Debugging**:
- Use `getBrainStatistics()` for comprehensive status
- Use `captureBrainSnapshot()` for debugging
- Enable error recovery with `runSimulationWithRecovery()`

**For Advanced Features**:
- Access all brain systems via `brain.getWorkingMemory()`, `brain.getEpisodicMemory()`, etc.
- Use the demo helper functions for rapid prototyping
- Combine multiple systems for sophisticated agent behavior

### 7. Example: Complete Advanced Brain System Integration

Here's a complete example showing all advanced features working together:
```python
import pynlm
import time

print("=== ADVANCED NLM BRAIN SYSTEM DEMONSTRATION ===\n")

# 1. Create brain with all advanced systems
print("1. Creating advanced brain with all systems...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create agent with world
print("2. Setting up agent environment...")
world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(12.5, 12.5)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 3. Enable all learning systems
print("3. Enabling all learning systems...")
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# 4. Demonstrate advanced system access
print("4. Accessing advanced brain systems...")
systems = {
    'Working Memory': brain.getWorkingMemory(),
    'Episodic Memory': brain.getEpisodicMemory(),
    'Associative Memory': brain.getAssociativeMemory(),
    'Prediction System': brain.getPredictionSystem(),
    'Neural Planner': brain.getPlanner(),
    'Concept Formation': brain.getConceptFormation(),
    'Attentional Selection': brain.getAttention(),
    'Development System': brain.getDevelopmentSystem(),
    'Dopamine System': brain.getDopamine(),
    'Curiosity System': brain.getCuriosity(),
    'Novelty System': brain.getNovelty(),
}

print("Available systems:")
for name, system in systems.items():
    print(f"  ✓ {name}: {'YES' if system else 'NO'}")

# 5. Run simulation with monitoring
print("\n5. Running advanced simulation with monitoring...")
start_time = time.time()

spike_counts, firing_rates = pynlm.simulateWithPerformanceMonitoring(
    brain,
    500,
    progressCallback=lambda step, rate, duration: None  # Silent mode
)

end_time = time.time()

# 6. Get comprehensive statistics
print("\n6. Simulation Results:")
stats = pynlm.getBrainStatistics(brain)
print(f"  Duration: {end_time - start_time:.2f}s")
print(f"  Total spikes: {stats.get('total_spikes', 'N/A')}")
print(f"  Final firing rate: {stats.get('average_firing_rate', 'N/A')}")
print(f"  Active neurons: {stats.get('active_neurons', 'N/A')}")

# 7. Demonstrate advanced functionality
print("\n7. Demonstrating advanced functionality:")
wm = brain.getWorkingMemory()
if wm:
    print(f"  Working memory traces: {wm.getActiveTraces()}")

em = brain.getEpisodicMemory()
if em:
    print(f"  Episodic memory episodes: {em.getEpisodeCount()}")

ap = brain.getAssociativeMemory()
if ap:
    print(f"  Associative memory associations: {ap.getAssociationCount()}")

print("\n=== DEMONSTRATION COMPLETE ===")
print("The NLM Phase 6 brain successfully integrates all advanced systems!")
```

These examples showcase the enhanced Python API for NLM Phase 6, including all the advanced brain systems, performance monitoring tools, batch operations, and utility functions I've added to make the neural learning machine more powerful and easier to use for advanced applications.

---

## Next Steps

When you're comfortable with these advanced features:
1. **Read `HOW_TO_USE.md`** - For detailed C++ build and installation instructions
2. **Read `docs/ARCHITECTURE.md`** - To understand the complete system architecture
3. **Read `docs/PHASE6_FINAL_AUDIT.md`** - To see the complete Phase 6 integration results
4. **Experiment with different configurations** - Try various brain sizes, connection densities, and plasticity settings!

The NLM Phase 6 system is now a fully integrated artificial brain with memory, prediction, cognition, and neuromodulation systems working together seamlessly. You're ready to build sophisticated neural agents and explore the limits of artificial brain architectures!

Enjoy experimenting with the enhanced NLM Python API!
