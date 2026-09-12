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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details about the improved API
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Read `docs/PROGRESSION_TUTORIAL.md` for structured learning paths
4. Experiment with different configurations!

## New Improved Python API

The NLM Python library has been improved with new factory functions, context managers, and better error handling. Here's what's new:

### New Factory Functions

Instead of creating components separately, use factory functions:

```python
# Method 1: Simple agent (pre-configured)
brain, world, agent = pynlm.createSimpleAgent(width=100, height=100)

# Method 2: Default settings
brain, world, agent = pynlm.createDefaultAgent()

# Method 3: Training agent (with learning enabled)
brain, world, agent = pynlm.createTrainingAgent(width=200, height=200)

# Method 4: Challenge agent (with obstacles)
brain, world, agent = pynlm.createChallengeAgent(width=300, height=300)

# Method 5: Custom experiment (from config file)
brain, world, agent = pynlm.createExperimentAgent(width=400, height=400, config_file="config.json")
```

### Context Managers

Automatic resource management with context managers:

```python
# Create agent with automatic cleanup
with pynlm.createSimpleAgent() as (brain, world, agent):
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

# Brain context manager
with pynlm.BrainContextManager(brain) as brain_ctx:
    for i in range(100):
        brain_ctx.step(i)

# World context manager  
with pynlm.SimpleWorldContextManager(world) as world_ctx:
    for step in range(100):
        world_ctx.update(0.1)
```

### Error Handling

Robust simulation with comprehensive error handling:

```python
def run_simulation_with_error_handling(width=100, height=100, num_steps=1000):
    brain = world = agent = None
    
    try:
        # Create agent using factory function
        brain, world, agent = pynlm.createSimpleAgent(width, height)
        
        # Run simulation loop
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Error checking
            if brain.getFiringNeuronCount() == 0 and step > 100:
                print("Warning: No neurons firing - possible initialization issue")
                
    except pynlm.NLMException as e:
        print(f"NLM error: {e}")
        raise
    except Exception as e:
        print(f"Unexpected error: {e}")
        raise
    finally:
        # Cleanup
        if agent:
            agent.reset()
        if world:
            world.reset()
        print("Simulation completed")

# Run robust simulation
try:
    run_simulation_with_error_handling()
except Exception as e:
    print(f"Simulation failed: {e}")
```

### Advanced Configuration

Create custom configurations:

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Customize brain parameters
config.set("brain.neuron_count", 2000)
config.set("brain.v_thresh", -45.0)
config.set("plasticity.stdp.enable", True)

# Customize world
config.set("world.max_energy", 200.0)
config.set("world.width", 300.0)

# Save to file
config.saveToFile("my_config.json")

# Load from file
new_config = pynlm.Config()
new_config.loadFromFile("my_config.json")

# Use custom config
brain = pynlm.createBrain(new_config)
```

## Quick Reference

### Creating Agents (New API)

| What you want | Old Way | New Way |
|---------------|---------|----------|
| Simple agent | `brain = pynlm.createBrain(...); agent = pynlm.createAgentBrain(brain)` | `brain, world, agent = pynlm.createSimpleAgent()` |
| Training agent | Manual setup with all features enabled | `brain, world, agent = pynlm.createTrainingAgent()` |
| Challenge agent | Manual setup with obstacles | `brain, world, agent = pynlm.createChallengeAgent()` |

### Context Management

| Need | Old Way | New Way |
|------|---------|----------|
| Resource cleanup | Manual reset calls | `with pynlm.createSimpleAgent():` |
| Batch operations | Multiple function calls | Single context manager |

### Error Handling

| Error type | Old behavior | New behavior |
|------------|--------------|--------------|
| NLMException | Generic exception | Specific NLM errors |
| ValueError | Generic exception | Type checking with clear messages |
| RuntimeError | Generic exception | Initialization state tracking |

## Upgrading from Old to New API

### Step 1: Replace Manual Creation with Factory Functions

**Old:**
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(100, 100, 20, 20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

**New:**
```python
brain, world, agent = pynlm.createSimpleAgent(width=100, height=100, vision_width=20, vision_height=20)
```

### Step 2: Add Context Management

**Old:**
```python
brain = pynlm.createBrain(...)
# ... use brain
brain.reset()  # Manual cleanup
```

**New:**
```python
with pynlm.BrainContextManager(brain) as brain_ctx:
    # ... use brain
    # Automatic cleanup on exit
```

### Step 3: Enable Advanced Features

**Old:**
```python
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)
```

**New:**
```python
brain, world, agent = pynlm.createTrainingAgent()  # All features enabled
# OR with simple agent (features not enabled by default)
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)  # ... etc
```

## Summary

The new improved Python API provides:

1. **Factory Functions**: One-line agent creation with sensible defaults
2. **Context Managers**: Automatic resource management and cleanup
3. **Better Error Handling**: Specific error types with meaningful messages
4. **Flexible Configuration**: Custom configuration with file I/O
5. **Learning Path Tutorial**: Structured progression from beginner to advanced

The documentation has been reorganized into a learning progression system (`docs/PROGRESSION_TUTORIAL.md`) that guides users through:
- **Level 1**: Foundations (silent brain, agent in world)
- **Level 2**: Core Patterns (simulation loops, monitoring)
- **Level 3**: Factory Functions and Context Managers
- **Level 4**: Advanced Features and Custom Experiments

That's it! You're now ready to use NLM with the improved Python API.
