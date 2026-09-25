# NLM Tutorial 1: First Steps with NLM

## Overview

Welcome to NLM! This tutorial will guide you through your first experience with the NLM (Neural Learning Machine) system. You'll learn how to:

1. Create a virtual brain
2. Initialize and run a simulation
3. Interact with a simple world environment
4. Observe neural learning and development

By the end of this tutorial, you'll have a working NLM brain that can learn from experience.

---

## Prerequisites

- Python 3.8 or higher
- NLM installed (`pip install .` or `pip install pynlm`)
- Basic understanding of neural networks and simulation concepts

## Setup

### Quick Installation (if needed)

If you haven't installed NLM yet:

```bash
# Install from source
pip install .

# Or install directly from this repository
pip install -e .
```

### Verify Installation

```python
import pynlm

print(f"NLM Version: {pynlm.__version__}")
print("✓ NLM installed successfully")
```

---

## Part 1: Creating Your First Brain

### Understanding the Components

Before we start, let's understand the main NLM components:

| Component | Purpose | Python API |
|-----------|---------|------------|
| **Brain** | Virtual spiking neural network | `pynlm.createBrain(config)` |
| **World** | Simulation environment | `pynlm.createSimpleWorld()` |
| **Agent** | Bridge between brain and world | `pynlm.createAgentBrain(brain)` |
| **Config** | Brain configuration | `pynlm.createDefaultConfig()` |

### Step 1: Create a Simple Brain

```python
import pynlm
import time

print("=== Creating Your First Brain ===")

# 1. Create configuration
config = pynlm.createDefaultConfig()

# 2. Customize brain size (optional)
config.set("brain.neuron_count", 500)

# 3. Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"✓ Brain initialized successfully")
```

**What just happened?**
- Created a configuration object
- Customized the number of neurons (500)
- Created a brain instance
- Initialized the brain (this sets up neurons, synapses, and initial state)

### Step 2: Test Basic Brain Function

```python
print("\n=== Testing Basic Brain Function ===")

# Run the brain for a few steps
print("Running brain simulation...")
start_time = time.time()

for step in range(100):
    brain.step(step)
    if step % 25 == 0:
        print(f"  Step {step}: {brain.getFiringNeuronCount()} neurons firing")

end_time = time.time()

print(f"✓ Simulation completed in {end_time - start_time:.2f} seconds")
print(f"✓ Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"✓ Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

**What you're seeing:**
- The brain is processing spikes through the neural network
- Each neuron follows Leaky Integrate-and-Fire dynamics
- Synapses are transmitting signals based on spike timing
- Learning mechanisms (STDP, Hebbian) are updating weights

---

## Part 2: Adding a World Environment

### Why Add a World?

The world gives your brain something to interact with. It provides:

1. **Sensory input** - What the agent can perceive
2. **Motor output** - How the agent can act
3. **Rewards/punishments** - Feedback for learning
4. **Challenges** - Problems to solve

### Step 3: Create a Simple World

```python
print("\n=== Adding a World Environment ===")

# 1. Create a simple 2D world
world = pynlm.createSimpleWorld()

# 2. Configure world parameters
world.configure(
    width=20,        # World width (cells)
    height=20,       # World height (cells)
    visionWidth=8,   # Agent vision width
    visionHeight=8   # Agent vision height
)

# 3. Reset world to initial state
world.reset()

print(f"✓ World created: {world.getWidth()}x{world.getHeight()} cells")
print(f"✓ Agent vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
print(f"✓ Agent starting position: {world.getAgentPosition()}")
```

### Step 4: Create an Agent

```python
print("\n=== Creating Agent Interface ===")

# 1. Create agent brain interface
agent = pynlm.createAgentBrain(brain)

# 2. Initialize agent with world
agent.initialize(world)

print("✓ Agent created and initialized")
print(f"✓ Agent connected to world")
```

### Step 5: Explore World Properties

```python
print("\n=== Exploring World Properties ===")

# Get sensory percept (what agent can see)
percept = world.getSensoryPercept()
print(f"✓ Sensory input received: {len(percept.getData())} elements")

# Get agent position and energy
position = world.getAgentPosition()
energy = world.getAgentEnergy()
print(f"✓ Agent position: {position}")
print(f"✓ Agent energy: {energy}")

# Check available actions
print(f"✓ Available actions: {agent.getAvailableActions()}")
```

---

## Part 3: Running a Complete Simulation

### The Simulation Loop

The core NLM simulation follows this pattern:

```
while simulation_running:
    1. Update world (time passes)
    2. Get sensory input (agent perceives environment)
    3. Process sensory input (brain receives information)
    4. Run brain step (neural computation)
    5. Decode motor command (brain decides action)
    6. Apply action to world (environment changes)
    7. Apply reward (brain learns from results)
    8. Update development (brain matures over time)
```

### Step 6: Run Your First Complete Simulation

```python
print("\n=== Complete Simulation Loop ===")

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation
num_steps = 200
print(f"Running {num_steps} simulation steps...")

for step in range(num_steps):
    # 1. Update world
    world.update(0.1)  # 0.1 time units per step
    
    # 2. Get sensory input
    percept = world.getSensoryPercept()
    
    # 3. Process sensory input in brain
    agent.processSensoryInput(percept)
    
    # 4. Run brain step (neural computation)
    brain.step(step)
    
    # 5. Decode motor command (brain decides action)
    action = agent.decodeMotorCommand()
    
    # 6. Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # 7. Apply reward (brain learns from results)
    reward = world.getReward()
    error = agent.getPredictionError()
    agent.applyRewardModulation(reward, error)
    
    # 8. Update development (brain matures over time)
    agent.updateDevelopment(0.1)
    
    # Print progress every 50 steps
    if step % 50 == 0:
        print(f"  Step {step}:")
        print(f"    Position: {world.getAgentPosition()}")
        print(f"    Energy: {world.getAgentEnergy():.1f}")
        print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"    Reward: {reward:.2f}")

print("✓ Simulation complete!")
```

### Step 7: Analyze Results

```python
print("\n=== Analyzing Results ===")

# Final statistics
final_position = world.getAgentPosition()
final_energy = world.getAgentEnergy()
total_spikes = brain.getTotalSpikeCount()
average_firing_rate = brain.getAverageFiringRate()
development_stage = brain.getDevelopmentalStage()

print("Final Simulation Statistics:")
print(f"  ✓ Agent position: {final_position}")
print(f"  ✓ Final energy: {final_energy:.1f}")
print(f"  ✓ Total spikes: {total_spikes}")
print(f"  ✓ Average firing rate: {average_firing_rate:.2f} Hz")
print(f"  ✓ Development stage: {development_stage.value}")

# Check learning progress
print(f"\nLearning Progress:")
print(f"  ✓ Reward modulation: {'Enabled' if agent.getRewardModulationEnabled() else 'Disabled'}")
print(f"  ✓ Curiosity system: {'Active' if agent.getCuriosityLevel() > 0 else 'Inactive'}")
print(f"  ✓ Development active: {'Yes' if brain.getDevelopmentalStage() != brain.getDevelopmentalStage().INITIAL else 'No'}")
```

---

## Part 4: Exploring Advanced Features

### Step 8: Enable More Learning Systems

```python
print("\n=== Exploring Advanced Learning Features ===")

# Create a brain with more advanced capabilities
advanced_config = pynlm.createLearningConfig()
advanced_config.set("plasticity.structural.enable", True)
advanced_config.set("memory.enable_working_memory", True)
advanced_config.set("memory.enable_episodic_memory", True)

# Create advanced brain
advanced_brain = pynlm.createBrain(advanced_config)
advanced_brain.initialize()

# Create advanced agent
advanced_world = pynlm.createSimpleWorld()
advanced_world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
advanced_agent = pynlm.createAgentBrain(advanced_brain)
advanced_agent.initialize(advanced_world)

# Enable all learning systems
advanced_agent.enableRewardModulation(True)
advanced_agent.enableCuriosity(True)
advanced_agent.enableDevelopment(True)
advanced_agent.enableStructuralPlasticity(True)

print("✓ Advanced brain configuration created")
print("✓ All learning systems enabled")
print("✓ Memory systems available")

# Run a shorter demonstration
short_steps = 100
for step in range(short_steps):
    advanced_world.update(0.1)
    percept = advanced_world.getSensoryPercept()
    advanced_agent.processSensoryInput(percept)
    advanced_brain.step(step)
    action = advanced_agent.decodeMotorCommand()
    advanced_world.applyMotorCommand(action, advanced_world.getSimulationTime())
    advanced_agent.applyRewardModulation(advanced_world.getReward(), advanced_agent.getPredictionError())
    advanced_agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        print(f"  Advanced simulation step {step}")

print("✓ Advanced simulation completed")
```

### Step 9: Experiment with Different Presets

```python
print("\n=== Experimenting with Different Presets ===")

# Test different presets
presets = [
    ("Minimal", pynlm.createMinimalConfig()),
    ("Learning", pynlm.createLearningConfig()),
    ("Development", pynlm.createDevelopmentConfig()),
    ("Curiosity", pynlm.createCuriosityConfig()),
]

for preset_name, preset_config in presets:
    print(f"\nTesting {preset_name} preset:")
    
    # Create brain with preset
    preset_brain = pynlm.createBrain(preset_config)
    preset_brain.initialize()
    
    # Quick test simulation
    for step in range(50):
        preset_brain.step(step)
    
    # Report results
    neurons = preset_brain.getTotalNeuronCount()
    spikes = preset_brain.getTotalSpikeCount()
    print(f"  ✓ Neurons: {neurons}")
    print(f"  ✓ Spikes: {spikes}")
```

---

## Part 5: Deep Dive Analysis

### Step 10: Analyze Neural Activity

```python
print("\n=== Deep Dive: Neural Activity Analysis ===")

# Collect activity data
activity_data = []
for step in range(100):
    brain.step(step)
    activity_data.append({
        'step': step,
        'firing_count': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage().value
    })

# Analyze patterns
initial_activity = activity_data[0]['firing_count']
final_activity = activity_data[-1]['firing_count']
activity_trend = (final_activity - initial_activity) / initial_activity if initial_activity > 0 else 0

print(f"Activity Analysis:")
print(f"  Initial firing count: {initial_activity}")
print(f"  Final firing count: {final_activity}")
print(f"  Activity trend: {'Increasing' if activity_trend > 0 else 'Decreasing' if activity_trend < 0 else 'Stable'} ({activity_trend:.1%})")

# Check for learning patterns
firing_rates = [data['firing_rate'] for data in activity_data]
avg_firing_rate = sum(firing_rates) / len(firing_rates)
print(f"Average firing rate: {avg_firing_rate:.2f} Hz")

if avg_firing_rate > 1.0:
    print("✓ Neural activity is sustained and learning")
elif avg_firing_rate < 0.1:
    print("⚠️  Low neural activity - brain may need stimulation")
else:
    print("✓ Moderate neural activity")
```

### Step 11: Visualize the Results

```python
print("\n=== Visualizing Results (Text-based) ===")

# Simple text visualization
print("Simulation Progress Visualization:")
print("(Each '.' represents one firing neuron)")

for i, data in enumerate(activity_data):
    if i % 10 == 0:  # Show every 10th step
        firing_count = data['firing_count']
        max_neurons = 50  # Maximum expected neurons
        bar_length = int((firing_count / max_neurons) * 20)
        bar = '.' * bar_length
        
        print(f"Step {data['step']:3d}: {bar} ({firing_count} neurons)")
```

---

## Part 6: Summary and Next Steps

### What You've Accomplished

✅ **Created a virtual brain** with 500 neurons
✅ **Initialized and ran simulations** for 200+ steps
✅ **Set up agent-world interaction** with sensory and motor systems
✅ **Enabled learning mechanisms** (reward, curiosity, development)
✅ **Observed neural activity** and firing patterns
✅ **Explored different configuration presets**

### Key Concepts Learned

1. **Brain Initialization**: `brain.initialize()` is essential before any simulation
2. **Simulation Loop**: The 8-step process for agent-environment interaction
3. **Learning Systems**: Reward modulation, curiosity, and development work together
4. **Configuration Management**: Different presets offer different capabilities
5. **Performance Considerations**: Balance between brain size and simulation speed

### Your Next Steps

Now that you understand the basics, you can:

1. **Read `HOW_TO_USE.md`** - Complete Python API reference
2. **Try `docs/EXAMPLES.md`** - More complex examples and use cases
3. **Check `docs/ARCHITECTURE.md`** - Deep dive into NLM architecture
4. **Explore `docs/ROADMAP.md`** - Current implementation status
5. **Start with `docs/tutorials/First Steps with NLM`** - Build on this foundation

### Common Next Projects

1. **Learning Tasks**: Create specific learning challenges
2. **Memory Experiments**: Test memory systems and recall
3. **Developmental Studies**: Track brain maturation over time
4. **Performance Testing**: Optimize brain configurations
5. **Research Experiments**: Design controlled scientific experiments

---

## Quick Reference Cheat Sheet

```python
# Essential NLM operations

# 1. Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)

# 3. Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Run simulation
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    agent.applyRewardModulation(world.getReward(), agent.getPredictionError())
    agent.updateDevelopment(0.1)

# 5. Check results
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Spikes: {brain.getTotalSpikeCount()}")
print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Common Mistakes to Avoid

1. **Forgetting `brain.initialize()`** - Always call this before simulation
2. **Using wrong step numbers** - Use sequential integers starting from 0
3. **Ignoring world updates** - Always call `world.update()` before getting sensory input
4. **Not applying actions** - Always call `world.applyMotorCommand()`
5. **Neglecting learning systems** - Enable reward, curiosity, and development for realistic behavior

---

## Welcome to NLM!

You've just completed your first NLM tutorial! You now have a working understanding of:

- Creating and initializing virtual brains
- Setting up agent-world interaction
- Running neural simulations
- Observing learning and development
- Analyzing neural activity

You're ready to explore more complex NLM applications and research projects. The NLM system offers endless possibilities for studying brain-like computation and learning.

**Next Tutorial:** Check out `docs/tutorials/Learning and Plasticity` to dive deeper into learning mechanisms.

**Documentation Index:** All documentation files are in the `docs/` directory, organized for easy navigation.

Happy simulating! 🚀

---

*Need help? Check the Troubleshooting Guide at `docs/TROUBLESHOOTING.md` or visit the NLM documentation website.*

---

**Tutorial 1 Complete!** You've successfully created your first NLM brain and run a complete simulation. Your foundation is solid for exploring more advanced NLM features and research applications.

**Score your understanding:** How many of the 8 simulation steps did you implement correctly? Which learning systems did you enable? What did you observe about neural activity?

Proceed to the next tutorial when you're ready to advance your NLM skills!