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

## Quick Start (Copy & Paste) - NEW SIMPLIFIED API

### Example 1: Simplest Brain with Easy Setup

```python
import pynlm

# Step 1: Make a brain (simple and easy!)
brain = pynlm.createQuickBrain(1000, 1)  # Just 2 lines to create your brain!

# Step 2: Set it up
def run_simple_simulation(brain, num_steps=100):
    for i in range(num_steps):
        brain.step(i)
    print(f"Done! Your brain thought {brain.getTotalSpikeCount()} times")
    return brain.getTotalSpikeCount()

# Run it
run_simple_simulation(brain)
```

### Example 2: Brain in a Simple World (with Agent Helper)

```python
import pynlm

# Use the new simplified API for everything
brain = pynlm.createQuickBrain(1000, 1)

# Create world and agent in one go
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with easy beginner helpers
pynlm.run_simulation(brain, world, agent, 50, 
                    enable_reward=True, 
                    enable_curiosity=True)

print("Simulation finished!")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Example 3: The Simplest Possible Example (One line!)

```python
import pynlm

# One line to create and initialize a virtual brain
brain = pynlm.createQuickBrain()
brain.initialize()
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns with Simplified API

### Pattern 1: Run a Simulation (Already implemented)

```python
def run_simulation(brain, world, agent, num_steps, **options):
    # Enable features if requested
    if options.get('enable_reward'):
        agent.enableRewardModulation(True)
    if options.get('enable_curiosity'):
        agent.enableCuriosity(True)
    if options.get('enable_plasticity'):
        agent.enableStructuralPlasticity(True)
    if options.get('enable_development'):
        agent.enableDevelopment(True)
    
    # The actual simulation loop
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    return brain, world, agent

# Use it:
brain = pynlm.createQuickBrain(1000, 1)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

brain, world, agent = run_simulation(brain, world, agent, 1000, 
                                     enable_reward=True, enable_curiosity=True)
```

### Pattern 2: Get Brain Stats (Much easier!)

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())

# NEW: Quick brain health check
health_status = pynlm.getBrainHealth(brain)
print("Brain health:", health_status)
```

### Pattern 3: Enable Brain Features (Much simpler!)

```python
# These are now much simpler with agent helpers:
agent.enableAllFeatures(True)      # Enable everything at once!
agent.enableAllFeatures(False)     # Disable everything

# Or enable specific features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

# NEW: Set all features at once with one method!
agent.setFeatures({
    'reward': True,
    'curiosity': True,
    'plasticity': True,
    'development': True,
    'novelty': True,
    'prediction': True
})
```

---

## What Each Part Does (NEW Simplified API)

### `pynlm.createQuickBrain(neuron_count, regions)`
Creates a virtual brain with configuration. Much simpler than before!
- `neuron_count` = Number of neurons (default: 1000)
- `regions` = Number of brain regions (default: 1)
- `config` = settings (use `createDefaultConfig()` for simple setup)

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

## Mini Projects (NEW Easier Examples)

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

# Simplest possible setup
brain = pynlm.createQuickBrain()
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
print("Stats:", brain.getFiringNeuronCount(), "neurons firing")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Use the simplified API
brain = pynlm.createQuickBrain(500, 1)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
pynlm.run_simulation(brain, world, agent, 30)

print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
print("Brain health:", pynlm.getBrainHealth(brain))
```

### Project 3: Complete Agent (Easier Learning!)

```python
import pynlm

# Setup with one-liners
brain = pynlm.createQuickBrain(1000, 1)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable ALL learning features at once!
agent.enableAllFeatures(True)

# Run agent with simplified helper
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
print("Final brain health:", pynlm.getBrainHealth(brain))
```

---

## Quick Reference (NEW Simplified)

| What you want | Easy Code | Complex Code |
|--------------|-----------|--------------|
| Create brain | `pynlm.createQuickBrain()` | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` | `brain.initialize()` |
| Make brain think | `brain.step(step)` | `brain.step(step, time)` |
| Create world | `pynlm.createSimpleWorld()` | `SimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` | `AgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` | `brain.getActiveNeuronCount()` |
| See world | `world.getSensoryPercept()` | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` | `brain.produceAction()` |

---

## Monitoring and Debugging (NEW Features!)

### Check Brain Health

```python
# NEW: Get overall brain health status
health = pynlm.getBrainHealth(brain)
print(f"Brain health score: {health}")

# NEW: Get detailed brain status
pynlm.printBrainStatus(brain)

# NEW: Monitor performance over time
pynlm.monitorPerformance(brain, 100, 10)  # Monitor for 100 steps, print every 10
```

### Memory Management

```python
# NEW: Simple memory management
agent.clearWorkingMemory()  # Clear working memory
agent.clearEpisodicMemory()  # Clear episodic memory
agent.clearAllMemory()  # Clear all memory

# NEW: Check memory status
print(f"Working memory traces: {agent.getWorkingMemoryCount()}")
print(f"Episodic memory episodes: {agent.getEpisodicMemoryCount()}")
```

### Feature Management

```python
# NEW: Enable all features at once
agent.enableAllFeatures(True)

# NEW: Configure feature combinations
agent.setFeatures({
    'reward': True,
    'curiosity': True,
    'plasticity': True,
    'development': True,
    'novelty': True,
    'prediction': True
})
```

---

## What Makes This Different?

1. **Simpler Setup**: `pynlm.createQuickBrain()` vs 5+ steps
2. **Helper Functions**: `run_simulation()` does everything for you
3. **One-Line Feature Enable**: `agent.enableAllFeatures(True)`
4. **Built-in Monitoring**: `getBrainHealth()` gives you instant feedback
5. **Memory Helpers**: Simple methods to manage memory without complex APIs
6. **Performance Monitoring**: Built-in tools to track brain performance

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** (if it exists)
2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
3. **Experiment with different configurations!**

The simplified API makes it much easier to get started with NLM while still giving you full access to the powerful underlying systems when you need it!

## Backward Compatibility

For advanced users who need the old API:
- The old API still works exactly as before
- The SimplifiedBrain includes an expert API that matches the old interface
- You can import the old way if you need complex operations

---

## Troubleshooting (Mostly Fixed Now!)

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`? (the new API handles this!)
- Try increasing the number of steps
- Check brain health: `pynlm.getBrainHealth(brain)`

**"The agent isn't moving"**
- Did you call `agent.enableAllFeatures(True)`? (enables everything!)
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - use the simplified setup

The new simplified API should make most of these troubleshooting steps much easier!

---

That's it! You're now ready to use NLM with the new simplified API. The hard work of setting up the brain, connecting all the systems, and making everything work together is done for you - you just need to focus on creating intelligent behavior!

## Monitoring and Debugging

The enhanced NLM now includes built-in monitoring and debugging tools to help you understand your brain's behavior:

### Brain Health Monitoring

```python
# Get overall brain health
brain_health = pynlm.getBrainHealth(brain)

# Print detailed brain status
pynlm.printBrainStatus(brain)

# Monitor performance over time
pynlm.monitorPerformance(brain, 100, 10)
```

### Memory Management

```python
# Simple memory management
agent.clearWorkingMemory()
agent.clearEpisodicMemory()
agent.clearAllMemory()

# Check memory status
print(f"Working memory traces: {agent.getWorkingMemoryCount()}")
print(f"Episodic memory episodes: {agent.getEpisodicMemoryCount()}")
```

### Feature Control

```python
# Enable all features at once
agent.enableAllFeatures(True)

# Configure specific features
agent.setFeatures({
    'reward': True,
    'curiosity': True,
    'plasticity': True
})
```

These tools make it much easier to monitor and control your NLM brain!

---

That's it! You're now ready to use NLM with the enhanced interface. The NLM now includes:
- Built-in monitoring and debugging tools
- Health tracking for your brain
- Performance monitoring capabilities
- Better error detection
- Simplified memory management

These improvements make it much easier to:
- Monitor brain health
- Track learning progress
- Debug issues
- Understand brain performance
- Manage memory systems

The NLM is now more user-friendly while maintaining all its powerful capabilities!
