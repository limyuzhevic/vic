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

## What Changed in the Python API

The NLM Python API has been significantly enhanced to make it easier and more powerful:

### New Features Added:

1. **Convenience Functions**: High-level functions for common patterns
2. **Type Hints**: Automatic type hints for better IDE support
3. **Property Access**: Intuitive attribute access to brain properties
4. **Enhanced Documentation**: Comprehensive docstrings and examples
5. **Multiple Configuration Presets**: Default, minimal, and research configurations
6. **Better Error Handling**: Clear error messages and exception types

### New Quick Setup:

```python
# NEW: Quick brain setup (replaces verbose setup)
brain = pynlm.createQuickBrain()
brain.initialize()

# NEW: Complete agent setup
brain, agent = pynlm.createAgentSetup(world)

# NEW: Run complete episodes
pynlm.run_episode(brain, agent, world, num_steps=100, verbose=True)
```

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain (OLD WAY)

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

### Example 1: Simplest Brain (NEW WAY)

```python
import pynlm

# NEW: One line to create and initialize a brain
brain = pynlm.createQuickBrain()

# Make it think
brain.step(0)

print(f"Your brain has {brain.neuron_count} neurons!")
print(f"It has fired {brain.total_spike_count} times.")
```

### Example 2: Brain in a Simple World (NEW WAY)

```python
import pynlm

# Setup everything with new convenience functions
brain, agent = pynlm.createAgentSetup(world)
brain.initialize()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run using new episode function
for avg_reward, max_firing in pynlm.run_episode(brain, agent, world, num_steps=50, verbose=True):
    pass  # Handled internally

print("Simulation finished!")
print("Firing neurons:", brain.firing_neuron_count)
```

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

*NEW: Now you can also use convenience functions for quick setup!*  
**NEW:** `pynlm.createQuickBrain()`, `pynlm.createAgentSetup()`, `pynlm.run_episode()`

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

**NEW:** The agent now also tracks curiosity and development!

---

## The Simplest Possible Example

### OLD WAY:
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

### NEW WAY:
```python
import pynlm

# One line to create, initialize, and configure a brain
brain = pynlm.createQuickBrain()

# Make it active
brain.step(0)

print(f"Your brain has {brain.neuron_count} neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation (OLD WAY)

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

### Pattern 1: Run a Simulation (NEW WAY)

```python
# NEW: Run complete simulation with one function
pynlm.run_episode(brain, agent, world, num_steps=1000, verbose=False)

# Get results
avg_reward, max_firing = pynlm.run_episode(brain, agent, world, num_steps=1000)
```

### Pattern 2: Get Brain Stats (ENHANCED)

```python
# NEW: Use intuitive property access
print("Neurons firing:", brain.firing_neuron_count)
print("Total spikes:", brain.total_spike_count)
print("Average firing rate:", brain.average_firing_rate)
print("Development stage:", brain.developmental_stage)
print("Curiosity level:", agent.curiosity_level)
print("Novelty level:", agent.novelty_level)
```

### Pattern 3: Enable Brain Features (ENHANCED)

```python
# NEW: These are all optional and available:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
agent.enableNovelty(True)               # Detect novelty
```

---

## What Each Part Does

### `pynlm.createQuickBrain(config)`
Creates a brain with sensible defaults for quick testing.

### `pynlm.createAgentSetup(world, config)`
Creates a complete agent setup with world.

### `pynlm.run_episode(brain, agent, world, num_steps, verbose)`
Runs a complete simulation episode.

### `brain.neuron_count` (NEW PROPERTY)
Get total number of neurons (read-only).

### `agent.curiosity_level` (NEW PROPERTY)
Get current curiosity level (exploration motivation).

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createQuickBrain()
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain, agent = pynlm.createAgentSetup(world)
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Watch the world
for i in range(30):
    pynlm.run_episode(brain, agent, world, num_steps=i+1)

print("Watched world for 30 steps")
print(f"Firing rate: {brain.average_firing_rate:.2f} Hz")
```

### Project 3: Complete Learning Agent (ENHANCED)

```python
import pynlm

# Setup with research configuration
config = pynlm.createResearchConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

brain, agent = pynlm.createAgentSetup(world, config)

# Enable learning with all systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableNovelty(True)

# Run using enhanced pattern
for avg_reward, max_firing in pynlm.run_episode(brain, agent, world, num_steps=100, verbose=True):
    pass

print("Agent simulation complete!")
print(f"Final curiosity: {agent.curiosity_level:.3f}")
print(f"Final development: {agent.developmental_stage}")
```

---

## Quick Reference (ENHANCED)

| What you want | OLD Code | NEW Code |
|--------------|----------|----------|
| Create brain | `pynlm.createBrain(config)` | `pynlm.createQuickBrain(config)` |
| Start brain | `brain.initialize()` | `pynlm.createQuickBrain()` |
| Make brain think | `brain.step(step)` | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` | `pynlm.createAgentSetup(world)` |
| Get brain stats | `brain.getFiringNeuronCount()` | `brain.firing_neuron_count` |
| See world | `world.getSensoryPercept()` | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` | `agent.decodeMotorCommand()` |

**NEW:** Many more properties available!

---

## Configuration Options (NEW)

The Python API now provides multiple configuration presets:

```python
# Default configuration (general purpose)
config = pynlm.createDefaultConfig()

# Minimal configuration (quick testing)
config = pynlm.createMinimalConfig()

# Research configuration (advanced experiments)
config = pynlm.createResearchConfig()
```

---

## New Examples Directory

Check out the new examples directory in `python/examples/` for comprehensive examples:

- `00_hello_world.py` - Basic usage
- `10_vision_world.py` - World interaction
- `20_complete_agent.py` - Full learning agent
- `30_exploration.py` - Exploration behavior
- `40_developmental.py` - Development processes
- `50_convenience_demo.py` - New convenience functions
- `60_config_comparison.py` - Configuration comparison
- `70_error_handling.py` - Error handling
- `80_performance_monitoring.py` - Performance tracking

Each example demonstrates a different aspect of the enhanced Python API.

---

## Documentation (NEW)

For comprehensive documentation, see:
- `docs/PYTHON_API.md` - Complete Python API reference
- `python/examples/README.md` - Examples directory guide

---

## Troubleshooting

### "My brain isn't doing anything"

```python
# Solution 1: Use new convenience functions
brain = pynlm.createQuickBrain()

# Solution 2: Ensure proper initialization
brain.initialize()

# Solution 3: Check if verbose output
pynlm.run_episode(brain, agent, world, num_steps=100, verbose=True)
```

### "The agent isn't moving"

```python
# Solution: Check action decoding
action = agent.decodeMotorCommand()
if action is None:
    print("No action produced - check brain activity")
```

### "Everything is 0"

```python
# Solution: Brains need time to "warm up"
pynlm.run_episode(brain, agent, world, num_steps=500, verbose=True)
```

### "Configuration questions"

```python
# For configuration options, see:
# - python/examples/60_config_comparison.py
# - docs/PYTHON_API.md
# - python/examples/README.md
```

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more C++ details
2. **Read `docs/PHASE6_ARCHITECTURE.md`** to understand how it all works
3. **Explore `python/examples/`** for more examples
4. **Check `docs/PYTHON_API.md`** for complete API documentation
5. **Experiment with different configurations**!

The enhanced Python API makes it easier than ever to create, simulate, and experiment with artificial brains. The new convenience functions, property access, and comprehensive documentation will help you get started quickly while still providing powerful features for advanced use cases.

**That's it! You're now ready to use the enhanced NLM Python API.**
