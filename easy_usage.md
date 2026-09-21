# NLM - Enhanced Easy Usage Guide for Beginners

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

### Example 1: Simplest Brain (Recommended Pattern)

```python
import pynlm

# Create configuration and brain (modern, flexible pattern)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize brain - ALWAYS call this before using!
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World (Modern Pattern)

```python
import pynlm

# Create configuration and brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run agent in world
for step in range(100):
    world.update(0.1)  # Update world state
    agent.processSensoryInput(world.getSensoryPercept())  # Brain sees world
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Act in world

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

# One line to create a virtual brain (using config pattern)
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Initialize it (required!)
brain.initialize()

# Make it active (step through time)
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation (Reimplemented for clarity)

```python
def run_simulation(brain, world, agent, num_steps=1000):
    """Complete agent simulation loop with statistics."""
    for step in range(num_steps):
        # 1. Update world state
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
    
    return brain

# Use it like this:
# brain = pynlm.createBrain(pynlm.createDefaultConfig())
# brain.initialize()
# world = pynlm.createSimpleWorld()
# world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
# world.reset()
# agent = pynlm.createAgentBrain(brain)
# agent.initialize(world)
# run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats (Enhanced)

```python
def print_brain_stats(brain, step):
    """Print comprehensive brain statistics."""
    print(f"Step {step}:")
    print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print()

# Use it like this (inside your simulation loop):
# for step in range(1000):
#     brain.step(step)
#     print_brain_stats(brain, step)
```

### Pattern 3: Enable Brain Features (Enhanced)

```python
def enable_advanced_features(agent, brain):
    """Enable all advanced brain subsystems."""
    # Enable learning systems
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # Additional advanced features (if available)
    # agent.enablePrediction(True)            # Predictive modeling
    # agent.enableAttention(True)              # Focus mechanisms
    # agent.enableMemory(True)                 # Enhanced memory
    
    return agent

# Use it like this:
# agent = pynlm.createAgentBrain(brain)
# enable_advanced_features(agent, brain)
```

---

## What Each Part Does

### `pynlm.createDefaultConfig()`
Creates a default configuration object with sensible settings.
- Returns a Config object you can customize
- Use as starting point for brain configuration

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)
- Returns a Brain object ready to be initialized

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.
- Advances world state
- Generates new sensory input

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.
- Converts world percept to neural input
- Injects current into sensory neurons

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.
- Decodes motor neuron activity
- Returns MotorCommand enum value

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup - following modern pattern
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
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

### Project 3: Complete Agent with Learning (Enhanced)

```python
import pynlm

def create_complete_agent(config=None):
    """Create a complete agent with all subsystems."""
    if config is None:
        config = pynlm.createDefaultConfig()
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    return brain, world, agent

# Run agent simulation
brain, world, agent = create_complete_agent()

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

## Quick Reference (Modern API)

| What you want | Modern Code |
|--------------|-------------|
| Create brain | `config = pynlm.createDefaultConfig(); brain = pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Configure world | `world.configure(width=20, height=20, visionWidth=8, visionHeight=8)` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `action = agent.decodeMotorCommand()` |

---

## Error Handling and Best Practices

### Basic Error Checking

```python
def safe_simulation(config=None, steps=100):
    """Simulation with error handling."""
    try:
        if config is None:
            config = pynlm.createDefaultConfig()
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        for step in range(steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        return brain, world, agent
        
    except Exception as e:
        print(f"Error in simulation: {e}")
        return None, None, None

# Use it:
# brain, world, agent = safe_simulation(steps=50)
# if brain is not None:
#     print("Simulation completed successfully!")
```

### Memory and Resource Management

```python
def cleanup_simulation(brain, world, agent):
    """Clean up resources properly."""
    # Reset brain state for new episodes
    if brain:
        brain.reset()
    
    # Clean up world
    if world:
        world.reset()
    
    # Clean up agent
    if agent:
        # Agent cleanup if needed
        pass
    
    print("Resources cleaned up")

# Use it in your simulation:
# for episode in range(num_episodes):
#     brain, world, agent = create_agent()
#     run_simulation(brain, world, agent)
#     cleanup_simulation(brain, world, agent)
```

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more detailed C++ API documentation
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations!** - Try different neuron counts, learning rates, etc.
4. **Explore `Phase6Demo.cpp`** for advanced integration examples
5. **Check the `experiments/` directory** for more complex scenarios

---

## Advanced Features (v0.2 Roadmap)

NLM is actively evolving. Coming soon:
- **Checkpoint/Save-Load System** - Save and resume brain states
- **Enhanced Visualization** - Better brain state visualization
- **Multi-Agent Systems** - Multiple agents interacting
- **Complex Environments** - 3D worlds, physics simulation
- **Extended Neuromodulation** - More sophisticated reward systems
- **Improved Development** - More realistic developmental trajectories

---

## Getting Started Checklist

✅ **Step 1:** Install pynlm (`pip install pynlm`)
✅ **Step 2:** Create your first brain (see examples above)
✅ **Step 3:** Run a simple simulation
✅ **Step 4:** Add learning and curiosity
✅ **Step 5:** Experiment with configurations

---

**That's it! You're now ready to use NLM advancedly.**

The modern patterns in this guide provide a solid foundation for building complex, realistic artificial brains with NLM. Remember to always initialize components before use, handle errors gracefully, and experiment to discover what works best for your specific application!

