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

### Example 1: Simplest Brain (Enhanced)

```python
import pynlm

# Step 1: Make a brain (using enhanced preset)
brain = pynlm.createBrainFromPreset("standard")

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Brain in a Simple World (Enhanced)

```python
import pynlm

# Create everything with enhanced agent
brain = pynlm.createBrainFromPreset("standard")
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

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

### Example 3: Ultra-Simple One-Liner

```python
import pynlm

# One line to create a virtual brain (with preset)
brain = pynlm.createBrainFromPreset("small")

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
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

## Common Patterns

### Pattern 1: Run a Simulation (Enhanced)

```python
from pynlm_enhanced import run_simulation_loop, SimulationMonitor

def run_simulation(brain, world, agent, num_steps):
    # Create monitor for performance tracking
    monitor = SimulationMonitor(brain, agent, world)
    
    # Run enhanced simulation loop with monitoring
    success = run_simulation_loop(brain, world, agent, num_steps, monitor=monitor)
    
    if success:
        print(f"Simulation completed with {monitor.get_summary()['total_steps']} steps")
    
    return monitor

# Use it like this:
# monitor = run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats (Enhanced)

```python
from pynlm_enhanced import analyze_brain_state

# Just add these inside your loop:
analysis = analyze_brain_state(brain, include_detailed=True)
print("Neurons firing:", analysis['firing_neurons'])
print("Total spikes:", analysis['total_spikes'])
print("Avg firing rate:", analysis['average_firing_rate'])
print("Development stage:", analysis['developmental_stage'])
print("Regions:", analysis['regions_count'])

if 'detailed' in analysis:
    print("Active regions:", len(analysis['detailed']['active_regions']))
```

### Pattern 3: Enable Brain Features (Enhanced)

```python
from pynlm_enhanced import create_agent_with_features

# Create agent with all features enabled
agent = create_agent_with_features(
    brain, world,
    enable_reward=True,      # Learn from rewards
    enable_curiosity=True,   # Explore new things
    enable_development=True, # Brain matures over time
    enable_plasticity=True   # Grow new connections
)

# Use agent with world
agent.initialize(world)
```

---

## Enhanced Configuration Options

### Preset-Based Configuration

Instead of struggling with configuration details, use presets:

```python
import pynlm

# Quick presets - no configuration headaches
brain = pynlm.createBrainFromPreset("small")      # For quick testing
brain = pynlm.createBrainFromPreset("standard")   # Balanced performance  
brain = pynlm.createBrainFromPreset("large")     # Detailed simulations
brain = pynlm.createBrainFromPreset("research")   # Advanced research

# Or create configuration from presets
config = pynlm.createConfigFromPreset("large")
```

### What Each Part Does (Enhanced)

### `pynlm.createConfigFromPreset(preset_name, **kwargs)`
Creates configuration from named presets with optional customization.

### `pynlm.createBrainFromPreset(preset_name, **kwargs)`
Creates brain using preset configurations instantly.

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

## Available Presets

| Preset | Neuron Count | Description |
|--------|-------------|-------------|
| `standard` | 1,000 | Balanced configuration for general use |
| `large` | 5,000 | Detailed simulations with more neurons |
| `small` | 100 | Quick testing with minimal resources |
| `research` | 10,000 | Advanced research configuration |

### Enhanced Usage Examples

```python
import pynlm
from pynlm_enhanced import (
    createConfigFromPreset, 
    createBrainFromPreset, 
    run_simulation_loop,
    SimulationMonitor,
    analyze_brain_state
)

# 1. Create configuration with custom parameters
config = createConfigFromPreset(
    "large",
    neuron_count=3000,  # Override preset
    plasticity_hebbian_enable=False  # Disable Hebbian learning
)

# 2. Create brain with preset
brain = createBrainFromPreset("research")
brain.initialize()

# 3. Run simulation with monitoring
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

monitor = SimulationMonitor(brain, agent, world)
success = run_simulation_loop(
    brain, world, agent, 
    steps=2000, timestep=0.001,
    monitor=monitor
)

# 4. Analyze results
analysis = analyze_brain_state(brain)
print(f"Simulation completed with {analysis['average_firing_rate']:.2f} avg firing rate")
```

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrainFromPreset("small")
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup with enhanced agent
brain = pynlm.createBrainFromPreset("standard")
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableCuriosity(True)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Learning Agent

```python
import pynlm
from pynlm_enhanced import create_agent_with_features

# Setup with learning capabilities
brain = pynlm.createBrainFromPreset("large")
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = create_agent_with_features(brain, world)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"Novelty: {agent.getNoveltyLevel():.3f}")

print("Agent simulation complete!")
```

---

## Quick Reference

### What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrainFromPreset("standard")` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `analyze_brain_state(brain)` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Enhanced Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Use `analyze_brain_state(brain)` to check if brain is working

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Enable agent's motor capabilities with `agent.enableCuriosity(True)`

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Use `create_agent_with_features(brain, world)` to enable all capabilities

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details** - This has advanced C++ build instructions and configuration options
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into the neural architecture
3. **Experiment with different configurations!** - Try different presets and see what happens
4. **Try the `pynlm_enhanced` module** - Use convenience functions and monitoring tools
5. **Create your own experiments** - The enhanced API makes it easier to run complex simulations

That's it! You're now ready to use the enhanced NLM with all its new features and improved usability.

---

**New Features Added:**
- Configuration presets for easy setup
- Enhanced agent with feature management
- Performance monitoring tools
- Brain state analysis utilities
- Simulation loop helpers
- Better error handling
- Backward compatibility with old-style usage