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

### Example 2: Enhanced Brain Simulation (NEW!)

```python
import pynlm

# One-line simulation with enhanced Pythonic API
stats = pynlm.runSimpleSimulation(1000)

print("Simulation complete!")
print("Activity statistics:", stats)
```

### Example 3: Complete Agent-World Simulation (NEW!)

```python
import pynlm

# Create and run a complete agent-world simulation
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(7.5, 7.5)

# Add some environment features
world.addObject(pynlm.WorldObject(5, 5, pynlm.WorldObjectType.Resource, 10.0, 0.5))

# Run simulation with learning
results = pynlm.runAgentWorldSimulation(world, 300)

print("Simulation complete!")
print(f"Total reward: {results['total_reward']:.2f}")
print(f"Final curiosity: {results['final_curiosity']:.3f}")
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

### Pattern 2: Get Brain Stats (ENHANCED!)

```python
# Enhanced brain statistics
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get comprehensive activity statistics
stats = brain.getActivityStats()
print("Neurons firing:", stats.get("firing_neuron_count", 0))
print("Total spikes:", stats.get("total_spike_count", 0))
print("Avg firing rate:", stats.get("average_firing_rate", 0.0))
print("Development stage:", stats.get("developmental_stage", 0))
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)              # Explore new things

# NEW: Enhanced agent with learning
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
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

## Enhanced Features (NEW!)

The updated NLM Python API now includes powerful enhanced features for easier usage:

### 🎯 One-Line Simulations

```python
# Simple simulation with one line
stats = pynlm.runSimpleSimulation(1000)

# Complete agent-world simulation with one line
results = pynlm.runAgentWorldSimulation(world, 500)
```

### 🧠 Phase 3 Style Learning Experiments

```python
# Demonstrate plasticity learning (NEW!)
results = pynlm.runPhase3StyleExperiment(1000)
print(f"Learning detected: {results['learning_detected']}")
print(f"Total spikes: {results['total_spikes']}")
```

### 🎮 Phase 4 Style Cognitive Experiments

```python
# Complete agent with cognitive functions (NEW!)
results = pynlm.runPhase4StyleExperiment(1000)
print(f"Episode score: {results['episode_score']:.2f}")
print(f"Final curiosity: {results['final_curiosity']:.3f}")
```

### ⚙️ Pythonic Configuration Management (ENHANCED!)

```python
# Enhanced configuration with dictionary support
config = pynlm.createDefaultConfig()

# Set multiple values at once
config.setMultiple({
    "brain.neuron_count": 1000,
    "brain.synapse_density": 0.15,
    "plasticity.stdp.enable": True,
    "neuromod.curiosity.enable": True
})

# Convert to dictionary for inspection
config_dict = config.toDict()
print("Configuration:", config_dict)
```

### 🔧 AgentBrain Enhancements (NEW!)

```python
# Enhanced agent with comprehensive methods
agent = pynlm.createAgentBrain(brain)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run episode with learning and tracking
reward = pynlm.runAgentEpisode(agent, world, 200)
```

### 📊 Comprehensive Monitoring (NEW!)

```python
# Get neuroscience-inspired metrics
metrics = agent.getNeuroscienceMetrics()
print(f"Curiosity level: {metrics['curiosity']:.3f}")
print(f"Novelty level: {metrics['novelty']:.3f}")
print(f"Prediction error: {metrics['prediction_error']:.4f}")
```

---

## Mini Projects (ENHANCED!)

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Enhanced Brain Watching a World (NEW!)

```python
import pynlm

# Setup with enhanced simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run enhanced simulation
stats = pynlm.runAgentWorldSimulation(world, 200)

print("Enhanced simulation complete!")
print(f"Total reward: {stats['total_reward']:.2f}")
print(f"Final curiosity: {stats['final_curiosity']:.3f}")
```

### Project 3: Complete Agent (ENHANCED!)

```python
import pynlm

# Setup with learning enabled
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run with learning tracking
results = pynlm.runAgentWorldSimulation(world, 300)

print("Enhanced agent simulation complete!")
print(f"Total reward: {results['total_reward']:.2f}")
print(f"Learning efficiency: {results['total_reward']/300:.4f}")
```

### Project 4: Learning Experiment (NEW!)

```python
import pynlm

# Run a Phase 3 style learning experiment
print("Running Phase 3 style learning experiment...")
results = pynlm.runPhase3StyleExperiment(500)

print("Learning Experiment Results:")
print(f"  Learning detected: {results['learning_detected']}")
print(f"  Total spikes: {results['total_spikes']}")
print(f"  Final firing rate: {results['final_firing_rate']:.4f}")
print(f"  Plasticity indicator: {results['final_eir'] > 0.5}")
```

---

## Quick Reference (ENHANCED!)

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Run simple simulation | `pynlm.runSimpleSimulation(1000)` |
| Run agent simulation | `pynlm.runAgentWorldSimulation(world, 500)` |
| Get activity stats | `brain.getActivityStats()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Get neuroscience metrics | `agent.getNeuroscienceMetrics()` |

---

## Enhanced Troubleshooting (NEW!)

**"My simulation isn't learning"**
- Did you enable the learning subsystems? `agent.enableRewardModulation(True)`
- Try running for more steps, especially with `runAgentWorldSimulation`
- Check that curiosity is enabled: `agent.enableCuriosity(True)`

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Enable the agent brain: `agent = pynlm.createAgentBrain(brain)`

**"Everything is 0"**
- Use enhanced simulations: `pynlm.runSimpleSimulation()` for better warm-up
- Make sure sensory input is connected through the agent
- Enable development for longer-term activity: `agent.enableDevelopment(True)`

**"My code is too verbose"**
- Use enhanced one-line simulations: `pynlm.runAgentWorldSimulation(world, 200)`
- Leverage built-in experiment helpers
- Use `pynlm.createCompleteTrainingSetup()` for advanced setups

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different enhanced configurations!

That's it! You're now ready to use NLM with the **Enhanced Python API** featuring powerful new shortcuts and experimental helpers.

## 🚀 What's New in Enhanced Python API

### ✅ Revolutionary New Features

1. **One-Line Simulations**: `pynlm.runAgentWorldSimulation(world, 500)` - complete agent loops in one line
2. **Phase 3/4 Experiment Helpers**: Built-in plasticity and cognitive experiments
3. **Pythonic Configuration**: Dictionary-based configuration and validation
4. **Enhanced Error Handling**: Better exceptions and validation feedback
5. **Neuroscience Metrics**: Advanced analysis tools for learning and development
6. **Training Setup**: Ready-to-use configurations for experimentation

### ✅ Enhanced Existing Features

1. **Better Documentation**: Comprehensive docstrings and examples
2. **Improved APIs**: More Pythonic method names and signatures
3. **Advanced Monitoring**: Real-time tracking and analysis
4. **Memory Management**: Safer garbage collection integration
5. **Type Safety**: Better runtime type checking and conversion

### ✅ Backward Compatibility

All existing code continues to work unchanged:

```python
# This still works exactly as before:
brain = pynlm.createBrain(config)
brain.initialize()
for step in range(100):
    brain.step(step)
```

But now you have powerful new tools for advanced experimentation!

## Installation and Usage

```bash
pip install -e .
```

```python
import pynlm

# Start with simple usage
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
```

Or use the enhanced features:

```python
# Jump right into advanced experiments
results = pynlm.runPhase4StyleExperiment(1000)
```

The enhanced Python API makes NLM accessible to beginners while providing powerful tools for advanced research and experimentation!
