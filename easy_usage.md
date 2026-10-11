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

### Example 2: Brain in a Simple World (NEW!)

```python
import pynlm

# Create everything with enhanced configuration
brain = pynlm.createBrain(pynlm.createPhase6IntegratedConfig())
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

### Example 3: Using Configuration Presets (NEW!)

```python
import pynlm

# Choose the perfect preset for your use case
# Options: "minimal", "balanced", "advanced", "high_performance", "phase6_integrated"

# Minimal configuration for quick testing
brain = pynlm.createBrain(pynlm.createMinimalConfig())
brain.initialize()

# Balanced configuration for general use (recommended for most users)
brain = pynlm.createBrain(pynlm.createBalancedConfig())
brain.initialize()

# Advanced configuration with enhanced features
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# High-performance configuration for speed
brain = pynlm.createBrain(pynlm.createHighPerformanceConfig())
brain.initialize()

# Phase 6 integrated configuration with all features
brain = pynlm.createBrain(pynlm.createPhase6IntegratedConfig())
brain.initialize()
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

# One line to create a virtual brain with best defaults
brain = pynlm.createBrain(pynlm.createPhase6IntegratedConfig())

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

### Pattern 2: Get Brain Stats (ENHANCED!)

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
print("Neuromodulation level:", agent.getNeuromodulationLevel())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Prediction error:", agent.getPredictionError())
```

### Pattern 3: Enable Brain Features (IMPROVED!)

```python
# These are all optional with better configuration:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# You can also configure features at creation time:
config = pynlm.createPhase6IntegratedConfig()
agent.enableRewardModulation(config.getBoolOr("enable_reward_modulation", True))
agent.enableCuriosity(config.getBoolOr("enable_curiosity", True))
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createPhase6IntegratedConfig()` for simple setup)

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

### NEW: Enhanced Configuration Methods

```python
# Create configuration with specific presets
config = pynlm.createMinimalConfig()      # Quick testing
config = pynlm.createBalancedConfig()    # Recommended for most users
config = pynlm.createAdvancedConfig()    # Enhanced features
config = pynlm.createHighPerformanceConfig()  # Optimized for speed
config = pynlm.createPhase6IntegratedConfig()  # All features enabled

# Get available presets
presets = pynlm.getPresetNames()
print("Available presets:", presets)

# Validate configuration
validation = pynlm.validateConfig(config)
if validation.valid:
    print("Configuration is valid!")
else:
    print("Configuration errors:", validation.errorMessage)
```

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createPhase6IntegratedConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World (NEW ENHANCED!)

```python
import pynlm

# Setup with Phase 6 integration
brain = pynlm.createBrain(pynlm.createPhase6IntegratedConfig())
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
print("Memory traces:", brain.getWorkingMemory().getActiveTraces() if brain.getWorkingMemory() else "N/A")
```

### Project 3: Complete Agent with Learning (ENHANCED!)

```python
import pynlm

# Setup with advanced features
config = pynlm.createAdvancedConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning based on config
agent.enableRewardModulation(config.getBoolOr("enable_reward_modulation", True))
agent.enableCuriosity(config.getBoolOr("enable_curiosity", True))

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.2f}")
        print(f"  Curiosity: {agent.getCuriosityLevel():.2f}")

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
| Get advanced stats | `agent.getNeuromodulationLevel()`, `agent.getCuriosityLevel()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

| Configuration Presets | Code |
|-------------------|------|
| Minimal config | `pynlm.createMinimalConfig()` |
| Balanced config | `pynlm.createBalancedConfig()` |
| Advanced config | `pynlm.createAdvancedConfig()` |
| High-performance config | `pynlm.createHighPerformanceConfig()` |
| Phase 6 integrated config | `pynlm.createPhase6IntegratedConfig()` |
| Get presets list | `pynlm.getPresetNames()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if neuromodulation levels are active

**"The agent isn't learning"**
- Did you call `agent.applyRewardModulation()`?
- Check if curiosity is enabled
- Verify development is enabled

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if working memory is working

**"Configuration validation failed"**
- Use `pynlm.validateConfig(config)` to see errors
- Check the error message and fix invalid values

---

## Next Steps

When you're comfortable:

1. **Experiment with different configuration presets** - Try each preset to see which works best for your needs

2. **Read `HOW_TO_USE.md` for more details** - Contains advanced technical information

3. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Deep dive into the brain architecture

4. **Check `docs/PHASE6_FINAL_AUDIT.md`** - Latest audit of Phase 6 integration

5. **Try the Phase 6 demo** - Run the comprehensive integration test with `./nlm_phase6_demo`

6. **Build with Python bindings** - Install the Python package `pynlm` and explore the Python API

That's it! You're now ready to use NLM with all the enhanced features and improvements.

**NEW: Phase 6 Integration Benefits**
- ✓ Memory systems (working, episodic, semantic, procedural, associative) all integrated
- ✓ Neuromodulation (dopamine, curiosity, novelty, prediction error) fully connected
- ✓ Prediction system integrated with learning
- ✓ Development system affects plasticity rates
- ✓ Checkpoint save/load working
- ✓ Replay and consolidation functional
- ✓ Complete brain loop functions coherently