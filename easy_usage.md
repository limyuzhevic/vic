# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## New Pythonic Interface Features

The NLM Python API now includes **Pythonic enhancements** that make working with neural simulations more intuitive and efficient:

### Context Managers
Use `with` statements for automatic cleanup and state management

### Convenience Methods
Shorter, more Pythonic method names that match typical usage patterns

### Batch Operations
Process multiple neurons or sensory inputs efficiently in single calls

### State Management
Save and restore brain states easily

### Clone and Copy Operations
Create copies of your brains and agent interfaces

## Enhanced Quick Start Examples

### Example 1: Simplest Brain with Context Manager

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Use context manager for automatic cleanup with "with" statement
with brain as brain_ctx:
    # Access the brain through the context wrapper
    ctx = brain_ctx.getSimulationState()
    print("Initial state:", ctx)

    # Make it think for 100 steps
    for i in range(100):
        brain_ctx.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Agent with Pythonic Convenience Methods

```python
import pynlm

# Create everything using modern Pythonic API
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning using convenience methods
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run for 50 steps using Pythonic names
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processInput(percept)  # Pythonic name: processInput
    brain.stepWithTime(step, 0.0)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

### Example 3: Batch Operations for Efficiency

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Batch neuron current injection
injections = []
for i in range(100):
    injections.append((pynlm.createNeuronId(i), 5.0))

brain.injectCurrentBatch(injections)

# Batch sensory input processing
percepts = []
for i in range(50):
    percept = pynlm.SensoryPercept()
    # Add some vision data
    vision_data = [0.1 * j for j in range(256)]
    percept.setVision(vision_data)
    percepts.append(percept)

agent.processInputsBatch(percepts)

print("Batch operations completed!")
```

### Example 4: Clone and Copy Operations

```python
import pynlm

# Create original brain and agent
original_brain = pynlm.createBrain(pynlm.createDefaultConfig())
original_brain.initialize()

original_agent = pynlm.createAgentBrain(original_brain)

# Clone the agent (creates new brain with same configuration)
cloned_agent = original_agent.clone()
cloned_agent.initialize(world)  # Must re-initialize in a world

# Copy settings from original to another agent
target_agent = pynlm.createAgentBrain(pynlm.createBrain(pynlm.createDefaultConfig()))
target_agent.copySettings(original_agent)

print("Clone and copy operations completed!")
```

### Example 5: State Management and Validation

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Get complete state as dictionary
agent_state = agent.getAgentState()
print("Current agent state:", agent_state)

# Validate state and check for issues
is_valid, error_msg = agent.validateState()
if not is_valid:
    print("WARNING: State validation failed:", error_msg)
else:
    print("State is valid and healthy")

print("State management completed!")
```

### Example 6: Enhanced Development Tracking

```python
import pynlm

# Create brain with development enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable development
agent.enableDevelopment(True)

# Run simulation with development tracking
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processInput(percept)
    brain.stepWithTime(step, 0.0)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

    # Print development progress
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
        print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
        print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")

# Check for developmental milestones
if brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.Adult:
    print("Brain has reached adulthood - plasticity is reduced")
elif brain.getDevelopmentalStage() == pynlm.DevelopmentalStage.CriticalPeriod:
    print("Brain in critical period - high plasticity, rapid learning")
```

---

---

## Enhanced Quick Reference

### Quick Start with New Pythonic Interface

```python
import pynlm

# Create brain with context manager support
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run enhanced simulation
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processInput(percept)  # Pythonic name
        brain.stepWithTime(step, 0.0)  # Enhanced with timestamp
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
```

### Performance-Enhanced Simulation

```python
import pynlm

def run_high_performance_simulation():
    """Run simulation with all performance enhancements."""
    
    # Create brain with all optimizations
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Generate all data upfront for batch processing
    percepts = []
    for i in range(1000):
        percept = pynlm.SensoryPercept()
        vision_data = [0.1 * j for j in range(256)]
        percept.setVision(vision_data)
        percepts.append(percept)
    
    # Run with batch operations for maximum efficiency
    for step in range(0, 1000, 50):  # Process in chunks
        # Process chunk of percepts
        agent.processInputsBatch(percepts[step:step+50])
        
        # Update development in batches
        agent.updateDevelopmentsBatch([0.1] * 50)
        
        # Update world
        for _ in range(50):
            world.update(0.1)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Print progress
        if step % 250 == 0:
            print(f"Progress: {step}/1000 steps")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("High-performance simulation complete!")

run_high_performance_simulation()
```

### State Management for Reproducibility

```python
import pynlm

def reproducible_experiment():
    """Run experiment with full state management."""
    
    # Create and initialize experiment
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableDevelopment(True)
    
    # Save initial state for reproducibility
    initial_state = agent.getAgentState()
    
    # Run experiment
    for step in range(500):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processInput(percept)
        brain.stepWithTime(step, 0.0)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = agent.getNeuromodulationLevel()
        agent.applyRewardModulation(reward, 0.0)
    
    # Validate final state
    is_valid, error_msg = agent.validateState()
    if not is_valid:
        print(f"WARNING: Final state validation failed: {error_msg}")
    
    # Save final state for later analysis
    final_state = agent.getAgentState()
    
    # Print summary
    print(f"Experiment complete!")
    print(f"Initial curiosity: {initial_state.get('curiosity', 0):.3f}")
    print(f"Final curiosity: {final_state.get('curiosity', 0):.3f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")

reproducible_experiment()
```

### Quick Reference of New Pythonic Methods

| Category | Old Method | New Pythonic Method | Description |
|----------|------------|---------------------|-------------|
| **Input Processing** | `agent.processSensoryInput(percept)` | `agent.processInput(percept)` | More Pythonic name |
| **Motor Decoding** | `agent.decodeMotorCommand()` | `agent.decodeMotorCommand()` | Already Pythonic |
| **Reward Modulation** | `agent.applyRewardModulation(reward, predicted)` | `agent.applyRewardModulation(reward, predicted)` | Already Pythonic |
| **Development** | `agent.updateDevelopment(timestep)` | `agent.updateDevelopment(timestep)` | Already Pythonic |
| **Brain Steps** | `brain.step(step)` | `brain.stepWithTime(step, 0.0)` | Enhanced with timestamp |
| **Statistics** | `brain.getFiringNeuronCount()` | `brain.getFiringNeuronCount()` | Same name |
| **Configuration** | `agent.enableRewardModulation(True)` | `agent.enableRewardModulation(True)` | Already Pythonic |
| **State Management** | N/A | `agent.getAgentState()`, `agent.setAgentState()` | New feature |
| **Validation** | N/A | `agent.validateState()` | New feature |
| **Batch Operations** | N/A | `brain.injectCurrentBatch()`, `agent.processInputsBatch()` | New feature |
| **Context Managers** | N/A | `with brain as brain_ctx:` | New feature |
| **Cloning** | N/A | `agent.clone()`, `agent.copySettings()` | New feature |

---

## The 3 Things You Need to Know (Updated)

1. **Brain** - The virtual brain that thinks (now with context managers and batch operations)
2. **World** - The environment the brain lives in (with enhanced state management)
3. **Agent** - The bridge connecting brain to world (now with enhanced convenience methods)

---

## Enhanced Quick Start (Copy & Paste)

### Example 1: Simplest Brain with Context Manager

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Use context manager for enhanced state management
with brain as brain_ctx:
    # Access enhanced context methods
    state = brain_ctx.getSimulationState()
    print("Enhanced state:", state)

    # Make it think for 100 steps
    for i in range(100):
        brain_ctx.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World (Pythonic)

```python
import pynlm

# Create everything with Pythonic names
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps with Pythonic method names
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processInput(percept)  # Pythonic name
    brain.stepWithTime(step, 0.0)  # Enhanced step
    action = agent.decodeMotorCommand()  # Still Pythonic
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

### Example 3: Complete Agent with All Enhancements

```python
import pynlm

# Setup with modern Pythonic patterns
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run enhanced agent simulation
for step in range(100):
    world.update(0.1)
    agent.processInput(world.getSensoryPercept())
    brain.stepWithTime(step, 0.0)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Enhanced agent simulation complete!")
```

---

## Simple Analogy (Enhanced)

| Real World | Enhanced NLM Code |
|------------|-------------------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processInput(percept)` | Pythonic name |
| You decide to act | `action = agent.decodeMotorCommand()` | Still Pythonic |
| You do the action | `world.applyMotorCommand(action, time)` | Unchanged |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` | Still Pythonic |

---

## The Simplest Possible Example (Enhanced)

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active with enhanced step method
brain.stepWithTime(0, 0.0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Enhanced Patterns

### Pattern 1: Enhanced Run Simulation

```python
import pynlm

def run_enhanced_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain (using Pythonic name)
        agent.processInput(percept)
        
        # 4. Brain thinks (using enhanced method)
        brain.stepWithTime(step, 0.0)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_enhanced_simulation(brain, world, agent, 1000)
```

### Pattern 2: Enhanced Brain Stats

```python
import pynlm

# Create brain with enhanced statistics
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Add enhanced statistics to your simulation
for step in range(100):
    world.update(0.1)
    agent.processInput(world.getSensoryPercept())
    brain.stepWithTime(step, 0.0)
    
    # Enhanced statistics (more concise)
    print(f"Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    print(f"Avg firing rate: {brain.getAverageFiringRate()}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
```

### Pattern 3: Enhanced Enable Brain Features

```python
import pynlm

# These are all optional and already Pythonic:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# New Pythonic convenience methods available:
agent.enable_reward_modulation(True)      # Alternative Pythonic name
agent.enable_curiosity(True)             # Alternative Pythonic name
agent.enable_development(True)           # Alternative Pythonic name
```

---

## What Each Part Does (Enhanced)

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses. Now supports context managers and batch operations!

### Enhanced `brain.initialize()`
Starts up the brain. Always call this before using the brain! Now supports advanced state management.

### Enhanced `brain.stepWithTime(step_number, time)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules
- Supports timestamped steps for precise temporal control

### `world.update(time)`
Updates the virtual world by `time` seconds. Enhanced with better state management.

### Enhanced `agent.processInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain. Now with Pythonic naming and batch support.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take. Already Pythonic.

---

## Mini Projects (Enhanced)

### Project 1: Silent Brain (Just Neurons) - Enhanced

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.stepWithTime(i, 0.0)

print("Enhanced silent brain test done!")
```

### Project 2: Brain Watching a World - Enhanced

```python
import pynlm

# Setup with enhanced Pythonic patterns
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps with enhanced methods
for i in range(30):
    world.update(0.1)
    agent.processInput(world.getSensoryPercept())  # Pythonic name
    brain.stepWithTime(i, 0.0)  # Enhanced method
    
print("Enhanced world watching complete!")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Enhanced Agent

```python
import pynlm

# Setup with all modern enhancements
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run enhanced agent with Pythonic methods
for step in range(100):
    world.update(0.1)
    agent.processInput(world.getSensoryPercept())
    brain.stepWithTime(step, 0.0)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Enhanced agent simulation complete!")
```

---

## Quick Reference (Enhanced)

| What you want | Enhanced Code |
|--------------|---------------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think (enhanced) | `brain.stepWithTime(step, time)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats (unchanged) | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action (unchanged) | `agent.decodeMotorCommand()` |
| Get simulation state (new) | `brain.getSimulationState()` |
| Process inputs in batch (new) | `agent.processInputsBatch(percepts)` |

---

## Enhanced Troubleshooting

### **"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try using enhanced step method: `brain.stepWithTime(step, 0.0)`
- Try increasing the number of steps

### **"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Use enhanced agent: `agent.processInput(percept)`

### **"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Use enhanced statistics: `brain.getAllNeuronsSummary()`
- Some neurons need input to fire - make sure sensory input is connected

### **"State validation failed"**
- Use validation: `agent.validateState()`
- Handle specific errors based on error message
- Enable disabled features based on validation feedback

### **"Context manager not working"**
- Use correct context manager syntax: `with brain as brain_ctx:`
- Ensure you're calling methods on the context wrapper
- Initialize brain before using context manager

---

## Next Steps (Enhanced)

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for comprehensive documentation of all Pythonic methods
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations** using the new Pythonic interface
4. **Explore batch operations** for improved performance
5. **Master state management** for reproducible experiments
6. **Use context managers** for automatic resource management
7. **Implement error handling** with validation
8. **Create clones and copies** for comparative experiments

That's it! You're now ready to use **enhanced NLM** with the **new Pythonic interface**!

## Key Benefits of the Enhanced Pythonic Interface

- **More intuitive**: Method names match Python conventions
- **More efficient**: Batch operations for better performance
- **More powerful**: Context managers for automatic resource management
- **More robust**: State management and validation for error handling
- **More flexible**: Clone and copy operations for experimental control
- **More maintainable**: Cleaner API that's easier to understand and use

Embrace the enhanced Pythonic interface and build more sophisticated and robust NLM applications!

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
