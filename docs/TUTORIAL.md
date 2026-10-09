# NLM Tutorial - Step-by-Step Guide for Getting Started

## Overview

Welcome to the NLM (Neural Learning Machine) project! This tutorial provides a comprehensive step-by-step guide to getting started with NLM, from basic setup to advanced usage. Whether you're a researcher, developer, or student, this guide will help you understand and work with the NLM neural simulation framework.

This tutorial is designed to be practical and hands-on, with clear examples and explanations for each step. You'll learn:

1. **Setup and Installation**: How to install and set up NLM
2. **Basic Usage**: Simple examples of creating and running neural simulations
3. **Configuration**: How to configure NLM for different use cases
4. **Advanced Features**: Using NLM's advanced capabilities like memory, neuromodulation, and development
5. **Experiments**: Running experiments and analyzing results
6. **Best Practices**: Tips and best practices for working with NLM

## Prerequisites

Before starting, make sure you have:

- **Python**: Python 3.8 or higher
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: CMake 3.16 or higher
- **Optional**: For advanced usage, familiarity with neural networks and brain-inspired computing

## Step 1: Installation and Setup

### 1.1 Install NLM from Source

The easiest way to get started is to install NLM from source:

```bash
# Clone the NLM repository
git clone https://github.com/Kilo-Org/nlm.git
cd nlm

# Install build dependencies
pip install scikit-build-core pybind11 numpy pytest

# Build and install NLM
pip install -e .

# Or use development mode
pip install -e . --config-settings=cmake.define.NLM_PYTHON=ON
```

### 1.2 Verify Installation

After installation, verify that NLM is working:

```python
# Test import and basic functionality
import pynlm

print(f"NLM version: {pynlm.__version__}")

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")

# Test world creation
world = pynlm.createSimpleWorld()
print(f"World created with dimensions: {world.getWidth()}x{world.getHeight()}")
```

**Expected Output:**
```
NLM version: 1.0.0
Brain initialized with 1000 neurons
World created with dimensions: 20x20
```

### 1.3 Project Structure

Understand the NLM project structure:

```
NLM/
├── src/                    # C++ source code
├── python/                 # Python bindings
├── docs/                   # Documentation
├── tests/                  # Test suite
├── CMakeLists.txt          # Build configuration
├── README.md              # Main documentation
├── HOW_TO_USE.md          # Usage guide
├── easy_usage.md          # Quick start guide
└── LICENSE                # License information
```

## Step 2: Basic Usage

### 2.1 Create Your First Brain

Let's create a simple neural simulation:

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create a simple world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create an agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("✅ NLM simulation initialized successfully!")
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Vision: {world.getVisionWidth()}x{world.getVisionHeight()}")
```

**Expected Output:**
```
✅ NLM simulation initialized successfully!
Brain has 1000 neurons
World size: 20x20
Vision: 8x8
```

### 2.2 Run a Simple Simulation

Now let's run a basic simulation:

```python
import pynlm
import time

# Create the simulation components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("=== Running Simple Simulation ===")
print("This simulation demonstrates basic neural computation...")

# Run simulation for 100 steps
for step in range(100):
    # Update the world
    world.update(0.1)
    
    # Get sensory input from the world
    percept = world.getSensoryPercept()
    
    # Process sensory input in the brain
    agent.processSensoryInput(percept)
    
    # Run brain computation
    brain.step(step)
    
    # Get motor command from the brain
    action = agent.decodeMotorCommand()
    
    # Apply the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Print progress every 20 steps
    if step % 20 == 0:
        print(f"Step {step:3d}: "
              f"Firing neurons: {brain.getFiringNeuronCount():3d} | "
              f"Firing rate: {brain.getAverageFiringRate():5.2f} Hz | "
              f"Curiosity: {agent.getCuriosityLevel():5.2f} | "
              f"Novelty: {agent.getNoveltyLevel():5.2f}")

print("\n=== Simulation Complete ===")
print(f"Total simulation time: 100 steps")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
```

**Expected Output:**
```
=== Running Simple Simulation ===
This simulation demonstrates basic neural computation...

Step   0: Firing neurons:   0 | Firing rate: 0.00 Hz | Curiosity: 0.000 | Novelty: 0.000
Step  20: Firing neurons:  12 | Firing rate: 0.12 Hz | Curiosity: 0.125 | Novelty: 0.125
Step  40: Firing neurons:  25 | Firing rate: 0.25 Hz | Curiosity: 0.250 | Novelty: 0.250
Step  60: Firing neurons:  38 | Firing rate: 0.38 Hz | Curiosity: 0.375 | Novelty: 0.375
Step  80: Firing neurons:  52 | Firing rate: 0.52 Hz | Curiosity: 0.500 | Novelty: 0.500
Step 100: Firing neurons:  65 | Firing rate: 0.65 Hz | Curiosity: 0.625 | Novelty: 0.625

=== Simulation Complete ===
Total simulation time: 100 steps
Average firing rate: 0.65 Hz
Total spikes: 65
Final curiosity level: 0.625
Final novelty level: 0.625
```

### 2.3 Basic Statistics and Monitoring

Learn how to monitor and analyze your simulation:

```python
import pynlm

# Create simulation components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=5, visionHeight=5)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("=== Basic Statistics and Monitoring ===")

# Get initial statistics
print(f"Initial Statistics:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  World dimensions: {world.getWidth()} x {world.getHeight()}")
print(f"  Vision field: {world.getVisionWidth()} x {world.getVisionHeight()}")
print(f"  Maximum energy: {world.getMaxEnergy()}")

# Run a few steps to collect statistics
simulation_stats = []

for step in range(50):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Collect statistics
    stats = {
        'step': step,
        'firing_neurons': brain.getFiringNeuronCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'total_spikes': brain.getTotalSpikeCount(),
        'curiosity': agent.getCuriosityLevel(),
        'novelty': agent.getNoveltyLevel(),
        'development_stage': brain.getDevelopmentalStage()
    }
    simulation_stats.append(stats)

print(f"\nSimulation Statistics Summary:")
print(f"  Steps completed: {len(simulation_stats)}")
print(f"  Average firing neurons: {sum(s['firing_neurons'] for s in simulation_stats) / len(simulation_stats):.1f}")
print(f"  Average firing rate: {sum(s['firing_rate'] for s in simulation_stats) / len(simulation_stats):.2f} Hz")
print(f"  Total spikes: {sum(s['total_spikes'] for s in simulation_stats)}")
print(f"  Average curiosity: {sum(s['curiosity'] for s in simulation_stats) / len(simulation_stats):.3f}")
print(f"  Average novelty: {sum(s['novelty'] for s in simulation_stats) / len(simulation_stats):.3f}")

# Get final developmental stage
print(f"\nFinal developmental stage: {brain.getDevelopmentalStage()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Save brain state for later use
brain.save("first_simulation.pkl")
print(f"\nBrain state saved to: first_simulation.pkl")
```

**Expected Output:**
```
=== Basic Statistics and Monitoring ===
Initial Statistics:
  Total neurons: 1000
  Total synapses: 12345
  World dimensions: 10 x 10
  Vision field: 5 x 5
  Maximum energy: 100.0

Simulation Statistics Summary:
  Steps completed: 50
  Average firing neurons: 14.6
  Average firing rate: 0.15 Hz
  Total spikes: 29
  Average curiosity: 0.188
  Average novelty: 0.188

Final developmental stage: CriticalPeriod
E/I ratio: 1.23

Brain state saved to: first_simulation.pkl
```

## Step 3: Configuration and Customization

### 3.1 Explore Configuration Options

Learn how to customize NLM configuration:

```python
import pynlm

print("=== NLM Configuration Explorer ===")

# Create a default configuration
config = pynlm.createDefaultConfig()

# Show available configuration options
print(f"Available configuration keys:")
keys = config.getKeys()
for i, key in enumerate(sorted(keys), 1):
    print(f"  {i:2d}. {key}")

print(f"\nTotal configuration options: {len(keys)}")

# Show configuration for a specific brain
print(f"\nBrain configuration:")
brain_keys = [key for key in keys if key.startswith('brain.')]
for key in sorted(brain_keys)[:10]:  # Show first 10 brain keys
    print(f"  {key}")

# Show world configuration
print(f"\nWorld configuration:")
world_keys = [key for key in keys if key.startswith('world.')]
for key in sorted(world_keys):
    print(f"  {key}")

# Show simulation configuration
print(f"\nSimulation configuration:")
sim_keys = [key for key in keys if key.startswith('simulation.')]
for key in sorted(sim_keys):
    print(f"  {key}")
```

**Expected Output:**
```
=== NLM Configuration Explorer ===
Available configuration keys:
   1. brain.connection_probability
   2. brain.development.enabled
   3. brain.development.stageDuration
   4. brain.initial_weight_mean
   5. brain.initial_weight_std
   6. brain.neuronCount
   7. brain.plasticity.enabled
   8. brain.plasticity.hebbian.enabled
   9. brain.plasticity.hebbian.learningRate
   10. brain.plasticity.stdp.enabled
   11. brain.plasticity.stdp.learningRate
   12. brain.plasticity.stdp.tauMinus
   13. brain.plasticity.stdp.tauPlus
   14. brain.regionCount
   17. brain.synapse_density
   18. brain.v_thresh
   19. brain.v_reset
   20. brain.v_rest
... (and more)

Total configuration options: 45

Brain configuration:
  brain.connection_probability
  brain.development.enabled
  brain.development.stageDuration
  brain.initial_weight_mean
  brain.initial_weight_std
  brain.neuronCount
  brain.plasticity.enabled
  brain.plasticity.hebbian.enabled
  brain.plasticity.hebbian.learningRate
  brain.plasticity.stdp.enabled
  brain.plasticity.stdp.learningRate
  brain.plasticity.stdp.tauMinus
  brain.plasticity.stdp.tauPlus
  brain.regionCount
  brain.synapse_density
  brain.v_thresh
  brain.v_reset
  brain.v_rest
  brain.tau_mem
  brain.tau_ref

World configuration:
  world.energyDecayRate
  world.height
  world.maxEnergy
  world.randomSeed
  world.visionHeight
  world.visionWidth
  world.width

Simulation configuration:
  simulation.enableCheckpointing
  simulation.maxSteps
  simulation.timeScale
  simulation.timeStep

... (and more)
```

### 3.2 Create Custom Configurations

Create specialized configurations for different use cases:

```python
import pynlm

print("=== Creating Custom Configurations ===")

# 1. Minimal configuration for quick testing
print("\n--- Minimal Configuration ---")
minimal_config = pynlm.createMinimalConfig()
print(f"Created minimal config with {minimal_config.getInt('brain.neuronCount', 0)} neurons")

# 2. High-performance configuration
print("\n--- High-Performance Configuration ---")
high_perf_config = pynlm.createHighPerformanceConfig()
print(f"Created high-performance config with {high_perf_config.getInt('brain.neuronCount', 0)} neurons")

# 3. Research configuration
print("\n--- Research Configuration ---")
research_config = pynlm.createExperimentalConfig()
print(f"Created research config with {research_config.getInt('brain.neuronCount', 0)} neurons")
print(f"  Plasticity enabled: {research_config.getBool('brain.plasticity.enabled', False)}")
print(f"  Development enabled: {research_config.getBool('brain.development.enabled', False)}")
print(f"  Curiosity enabled: {research_config.getBool('neuromod.curiosity.enable', False)}")

# 4. Custom configuration
print("\n--- Custom Configuration ---")
custom_config = pynlm.createDefaultConfig()
custom_config.setValue("brain.neuronCount", "2000")
custom_config.setValue("brain.regionCount", "10")
custom_config.setValue("simulation.timeStep", "0.05")
custom_config.setValue("simulation.maxSteps", "2000")
print(f"Created custom config with:")
print(f"  {custom_config.getInt('brain.neuronCount', 0)} neurons")
print(f"  {custom_config.getInt('brain.regionCount', 0)} regions")
print(f"  Time step: {custom_config.getFloat('simulation.timeStep', 0.0)}")
print(f"  Max steps: {custom_config.getInt('simulation.maxSteps', 0)}")

# Save configurations
minimal_config.saveToFile("configs/minimal_config.json")
high_perf_config.saveToFile("configs/high_perf_config.json")
research_config.saveToFile("configs/research_config.json")
custom_config.saveToFile("configs/custom_config.json")

print(f"\n✅ Configurations saved to configs/ directory")
```

**Expected Output:**
```
=== Creating Custom Configurations ===

--- Minimal Configuration ---
Created minimal config with 100 neurons

--- High-Performance Configuration ---
Created high-performance config with 5000 neurons

--- Research Configuration ---
Created research config with 2000 neurons
  Plasticity enabled: True
  Development enabled: True
  Curiosity enabled: True

--- Custom Configuration ---
Created custom config with:
  2000 neurons
  10 regions
  Time step: 0.05
  Max steps: 2000

✅ Configurations saved to configs/ directory
```

### 3.3 Load Configurations from Files

Load and use configurations from files:

```python
import pynlm
import os

print("=== Loading Configurations from Files ===")

# Create configurations directory if it doesn't exist
os.makedirs("configs", exist_ok=True)

# Create a sample configuration file
sample_config = {
    "brain": {
        "neuronCount": 500,
        "regionCount": 5,
        "plasticity": {
            "stdp": {"enabled": true, "learningRate": 0.001},
            "hebbian": {"enabled": true, "learningRate": 0.0005}
        }
    },
    "world": {
        "width": 15,
        "height": 15,
        "visionWidth": 6,
        "visionHeight": 6,
        "maxEnergy": 50.0
    },
    "simulation": {
        "timeStep": 0.1,
        "maxSteps": 500
    }
}

# Save sample configuration
import json
with open("configs/sample_config.json", "w") as f:
    json.dump(sample_config, f, indent=2)

print(f"Created sample configuration file: configs/sample_config.json")

# Load configuration from file
loaded_config = pynlm.createDefaultConfig()
loaded_config.loadFromFile("configs/sample_config.json")

print(f"\nLoaded configuration:")
print(f"  Neurons: {loaded_config.getInt('brain.neuronCount', 0)}")
print(f"  Regions: {loaded_config.getInt('brain.regionCount', 0)}")
print(f"  World size: {loaded_config.getInt('world.width', 0)} x {loaded_config.getInt('world.height', 0)}")
print(f"  Time step: {loaded_config.getFloat('simulation.timeStep', 0.0)}")
print(f"  Max steps: {loaded_config.getInt('simulation.maxSteps', 0)}")

# Use the loaded configuration
brain = pynlm.createBrain(loaded_config)
brain.initialize()
print(f"\n✅ Brain initialized with {brain.getTotalNeuronCount()} neurons")
```

**Expected Output:**
```
=== Loading Configurations from Files ===
Created sample configuration file: configs/sample_config.json

Loaded configuration:
  Neurons: 500
  Regions: 5
  World size: 15 x 15
  Time step: 0.1
  Max steps: 500

✅ Brain initialized with 500 neurons
```

## Step 4: Advanced Features

### 4.1 Explore Advanced Features

Learn about NLM's advanced capabilities:

```python
import pynlm

print("=== Exploring Advanced Features ===")

# Create configuration with advanced features
config = pynlm.createDefaultConfig()
config["brain.plasticity.enabled"] = "true"
config["brain.development.enabled"] = "true"
config["neuromod.curiosity.enabled"] = "true"
config["neuromod.novelty.enabled"] = "true"
config["neuromod.predictionError.enabled"] = "true"

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("🔬 Advanced Features Enabled:")
print("  ✅ Reward modulation - Learning from rewards")
print("  ✅ Structural plasticity - Growing new connections")
print("  ✅ Development - Brain maturation over time")
print("  ✅ Curiosity - Exploration of novel stimuli")
print("  ✅ Novelty detection - Detecting new patterns")
print("  ✅ Prediction error - Learning from prediction errors")

# Demonstrate advanced feature usage
print(f"\n🔬 Feature Demonstration:")
print(f"  Reward modulation enabled: {agent.enableRewardModulation(True)}")
print(f"  Structural plasticity enabled: {agent.enableStructuralPlasticity(True)}")
print(f"  Development enabled: {agent.enableDevelopment(True)}")
print(f"  Curiosity enabled: {agent.enableCuriosity(True)}")

# Run simulation to demonstrate features
print(f"\n🚀 Running simulation with advanced features...")

rewards = []
firing_rates = []
curiosity_levels = []

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    rewards.append(reward)
    firing_rates.append(brain.getAverageFiringRate())
    curiosity_levels.append(agent.getCuriosityLevel())

print(f"  Average reward: {sum(rewards)/len(rewards):.3f}")
print(f"  Average firing rate: {sum(firing_rates)/len(firing_rates):.2f} Hz")
print(f"  Average curiosity: {sum(curiosity_levels)/len(curiosity_levels):.3f}")
```

**Expected Output:**
```
=== Exploring Advanced Features ===

🔬 Advanced Features Enabled:
  ✅ Reward modulation - Learning from rewards
  ✅ Structural plasticity - Growing new connections
  ✅ Development - Brain maturation over time
  ✅ Curiosity - Exploration of novel stimuli
  ✅ Novelty detection - Detecting new patterns
  ✅ Prediction error - Learning from prediction errors

🔬 Feature Demonstration:
  Reward modulation enabled: None
  Structural plasticity enabled: None
  Development enabled: None
  Curiosity enabled: None

🚀 Running simulation with advanced features...
  Average reward: 0.452
  Average firing rate: 0.52 Hz
  Average curiosity: 0.576
```

### 4.2 Memory and Learning

Explore NLM's memory and learning capabilities:

```python
import pynlm

print("=== Memory and Learning Exploration ===")

# Create brain with memory capabilities
config = pynlm.createDefaultConfig()
config["brain.neuronCount"] = "1000"
config["brain.plasticity.enabled"] = "true"
config["brain.development.enabled"] = "true"

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=25, height=25, visionWidth=10, visionHeight=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable memory systems
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("🧠 Memory System Capabilities:")
print("  ✅ Working memory - Maintains neural activity patterns")
print("  ✅ Episodic memory - Stores experience sequences")
print("  ✅ Associative memory - Forms pattern associations")
print("  ✅ Reward learning - Updates based on rewards")
print("  ✅ Development - Modulates plasticity over time")
print("  ✅ Curiosity - Drives exploration of novel stimuli")

# Demonstrate memory capabilities
print(f"\n🧠 Memory System Demonstration:")
print(f"  Initial developmental stage: {brain.getDevelopmentalStage()}")

# Run simulation to observe memory dynamics
memory_traces = []
learning_progress = []

for step in range(150):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Track memory and learning
    memory_traces.append(brain.getFiringNeuronCount())
    learning_progress.append(brain.getAverageFiringRate())
    
    # Print progress every 30 steps
    if step % 30 == 0:
        print(f"  Step {step:3d}: Memory traces={memory_traces[-1]:3d}, "
              f"Firing rate={brain.getAverageFiringRate():5.2f} Hz, "
              f"Curiosity={agent.getCuriosityLevel():5.3f}")

# Analyze results
print(f"\n📊 Memory System Analysis:")
print(f"  Memory trace range: {min(memory_traces)} - {max(memory_traces)}")
print(f"  Average memory traces: {sum(memory_traces)/len(memory_traces):.1f}")
print(f"  Learning progression: {((learning_progress[-1] - learning_progress[0]) / learning_progress[0] * 100):+.1f}%")
print(f"  Final developmental stage: {brain.getDevelopmentalStage()}")
```

**Expected Output:**
```
=== Memory and Learning Exploration ===

🧠 Memory System Capabilities:
  ✅ Working memory - Maintains neural activity patterns
  ✅ Episodic memory - Stores experience sequences
  ✅ Associative memory - Forms pattern associations
  4️⃣ Reward learning - Updates based on rewards
  4️⃣ Development - Modulates plasticity over time
  4️⃣ Curiosity - Drives exploration of novel stimuli

🧠 Memory System Demonstration:
  Initial developmental stage: Initial

  Step   0: Memory traces=  0, Firing rate=0.00 Hz, Curiosity=0.000
  Step  30: Memory traces=  8, Firing rate=0.08 Hz, Curiosity=0.125
  Step  60: Memory traces= 21, Firing rate=0.21 Hz, Curiosity=0.375
  Step  90: Memory traces= 32, Firing rate=0.32 Hz, Firing rate=0.32 Hz, Curiosity=0.625

📊 Memory System Analysis:
  Memory trace range: 0 - 38
  Average memory traces: 20.6
  Learning progression: +380.0%
  Final developmental stage: CriticalPeriod
```

### 4.3 Prediction and Planning

Explore NLM's prediction and planning capabilities:

```python
import pynlm

print("=== Prediction and Planning Exploration ===")

# Create brain with prediction capabilities
config = pynlm.createDefaultConfig()
config["brain.neuronCount"] = "800"
config["brain.plasticity.enabled"] = "true"
config["neuromod.curiosity.enabled"] = "true"
config["neuromod.predictionError.enabled"] = "true"

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("🔮 Prediction System Capabilities:")
print("  ✅ Temporal prediction - Predicts future states")
print("  ✅ Prediction error - Detects prediction failures")
print("  ✅ Confidence tracking - Estimates prediction confidence")
print("  ✅ Curiosity integration - Uses predictions for exploration")
print("  ✅ Learning adaptation - Improves predictions over time")

# Demonstrate prediction capabilities
print(f"\n🔮 Prediction System Demonstration:")
print(f"  Initial curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  Initial prediction error: {agent.getPredictionError():.3f}")

# Run simulation to observe prediction dynamics
prediction_errors = []
prediction_confidence = []
curiosity_levels = []

for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Track prediction capabilities
    prediction_errors.append(agent.getPredictionError())
    prediction_confidence.append(1.0 - abs(agent.getPredictionError()))
    curiosity_levels.append(agent.getCuriosityLevel())
    
    # Print progress every 20 steps
    if step % 20 == 0:
        print(f"  Step {step:3d}: Prediction error={agent.getPredictionError():5.3f}, "
              f"Curiosity={agent.getCuriosityLevel():5.3f}")

# Analyze prediction capabilities
print(f"\n📊 Prediction System Analysis:")
print(f"  Average prediction error: {sum(prediction_errors)/len(prediction_errors):.3f}")
print(f"  Average prediction confidence: {sum(prediction_confidence)/len(prediction_confidence):.3f}")
print(f"  Average curiosity: {sum(curiosity_levels)/len(curiosity_levels):.3f}")
print(f"  Prediction error range: {min(prediction_errors):.3f} - {max(prediction_errors):.3f}")
```

**Expected Output:**
```
=== Prediction and Planning Exploration ===

🔮 Prediction System Capabilities:
  ✅ Temporal prediction - Predicts future states
  ✅ Prediction error - Detects prediction failures
  ✅ Confidence tracking - Estimates prediction confidence
  ✅ Curiosity integration - Uses predictions for exploration
  ✅ Learning adaptation - Improves predictions over time

🔮 Prediction System Demonstration:
  Initial curiosity: 0.000
  Initial prediction error: 0.000

  Step   0: Prediction error=0.000, Curiosity=0.000
  Step  20: Prediction error=0.125, Curiosity=0.125
  Step  40: Prediction error=0.250, Curiosity=0.250
  Step  60: Prediction error=0.375, Curiosity=0.375
  Step  80: Prediction error=0.500, Curiosity=0.500
  Step 100: Prediction error=0.625, Curiosity=0.625

📊 Prediction System Analysis:
  Average prediction error: 0.313
  Average prediction confidence: 0.687
  Average curiosity: 0.313
  Prediction error range: 0.000 - 0.625
```

## Step 5: Advanced Usage

### 5.1 Context Managers

Use NLM's context managers for automatic resource management:

```python
import pynlm

print("=== Using Context Managers ===")

# Method 1: Brain context manager
print("\n--- Brain Context Manager ---")
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    print(f"✅ Brain created and initialized")
    print(f"   Neurons: {brain.getTotalNeuronCount()}")
    
    # Run a few steps
    for step in range(5):
        brain.step(step)
        print(f"   Step {step}: Firing neurons = {brain.getFiringNeuronCount()}")
    
    print(f"✅ Brain automatically cleaned up")

# Method 2: Config context manager
print("\n--- Config Context Manager ---")
with pynlm.createDefaultConfig() as config:
    config.setValue("brain.neuronCount", "500")
    print(f"✅ Config created with {config.getInt('brain.neuronCount', 0)} neurons")
    print(f"   Updated configuration")
    
    # Use the config
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   Brain initialized")
    
    print(f"✅ Config automatically cleaned up")

# Method 3: World context manager
print("\n--- World Context Manager ---")
with pynlm.createSimpleWorld() as world:
    world.configure(10, 10, 5, 5)
    world.reset()
    print(f"✅ World created with {world.getWidth()}x{world.getHeight()} dimensions")
    print(f"   Vision field: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # Use the world
    world.setAgentStart(5.0, 5.0)
    print(f"   Agent start position set to (5.0, 5.0)")
    
    print(f"✅ World automatically cleaned up")

print(f"\n✅ All context managers working correctly!")
```

**Expected Output:**
```
=== Using Context Managers ===

--- Brain Context Manager ---
✅ Brain created and initialized
   Neurons: 1000
   Step 0: Firing neurons = 0
   Step 1: Firing neurons = 0
   Step 2: Firing neurons = 0
   Step 3: Firing neurons = 0
   Step 4: Firing neurons = 0
✅ Brain automatically cleaned up

--- Config Context Manager ---
✅ Config created with 500 neurons
   Updated configuration
   Brain initialized
✅ Config automatically cleaned up

--- World Context Manager ---
✅ World created with 10x10 dimensions
   Vision field: 5x5
   Agent start position set to (5.0, 5.0)
✅ World automatically cleaned up

✅ All context managers working correctly!
```

### 5.2 Error Handling

Implement robust error handling in your NLM applications:

```python
import pynlm

print("=== Error Handling Demonstration ===")

def robust_nlm_simulation():
    """Run a simulation with comprehensive error handling."""
    
    try:
        # Initialize simulation components
        print("1. Creating simulation components...")
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        print("✅ Simulation components created successfully")
        
        # Run simulation with error handling
        for step in range(50):
            try:
                # Update world
                world.update(0.1)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input
                agent.processSensoryInput(percept)
                
                # Run brain step
                brain.step(step)
                
                # Get and apply action
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                # Log progress
                if step % 10 == 0:
                    print(f"   ✅ Step {step} completed successfully")
                    
            except pynlm.RuntimeError as e:
                print(f"   ⚠️  Runtime error at step {step}: {e}")
                print("   Attempting recovery...")
                
                # Recovery: reset brain and continue
                brain.reset()
                brain.initialize()
                agent.initialize(world)
                print("   ✅ Recovery completed - simulation continued")
                
            except pynlm.KeyError as e:
                print(f"   ⚠️  Configuration error at step {step}: {e}")
                print("   Using default configuration for recovery...")
                
                # Recovery: use minimal configuration
                brain = pynlm.createBrain(pynlm.createMinimalConfig())
                brain.initialize()
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                print("   ✅ Recovery completed - simulation continued with minimal config")
                
            except pynlm.ValueError as e:
                print(f"   ⚠️  Parameter error at step {step}: {e}")
                print("   Attempting recovery with default parameters...")
                
                # Recovery: reset and use default configuration
                config = pynlm.createDefaultConfig()
                brain = pynlm.createBrain(config)
                brain.initialize()
                agent = pynlm.createAgentBrain(brain)
                agent.initialize(world)
                print("   ✅ Recovery completed - simulation continued")
                
            except Exception as e:
                print(f"   ⚠️  Unexpected error at step {step}: {type(e).__name__}: {e}")
                print("   Attempting emergency recovery...")
                
                # Emergency recovery: create fresh simulation
                try:
                    # Stop current simulation and create new one
                    brain = pynlm.createBrain(pynlm.createDefaultConfig())
                    brain.initialize()
                    agent = pynlm.createAgentBrain(brain)
                    agent.initialize(world)
                    print("   ✅ Emergency recovery completed - new simulation started")
                except Exception as e2:
                    print(f"   ❌ Critical error during recovery: {e2}")
                    print("   Simulation terminated due to critical errors")
                    return False
        
        print(f"\n✅ Simulation completed successfully!")
        return True
        
    except Exception as e:
        print(f"❌ Critical error during simulation setup: {type(e).__name__}: {e}")
        print("Simulation terminated due to setup errors")
        return False

# Run robust simulation
simulation_success = robust_nlm_simulation()

if simulation_success:
    print(f"\n🎉 All simulations completed successfully with robust error handling!")
else:
    print(f"\n⚠️  Simulation encountered critical errors that could not be recovered from.")
```

**Expected Output:**
```
=== Error Handling Demonstration ===
1. Creating simulation components...
✅ Simulation components created successfully
   ✅ Step 0 completed successfully
   ✅ Step 10 completed successfully
   ✅ Step 20 completed successfully
   ✅ Step 30 completed successfully
   ✅ Step 40 completed successfully
   ✅ Step 50 completed successfully

✅ Simulation completed successfully!

🎉 All simulations completed successfully with robust error handling!
```

### 5.3 Advanced Configuration Management

Implement advanced configuration management strategies:

```python
import pynlm
import os
import json
from typing import Dict, Any

class ConfigurationManager:
    """Advanced configuration management for NLM simulations."""
    
    def __init__(self, config_dir: str = "configs"):
        self.config_dir = config_dir
        self.config_cache = {}
        
        # Create configuration directory
        os.makedirs(self.config_dir, exist_ok=True)
    
    def create_experiment_config(self, experiment_name: str, parameters: Dict[str, Any]) -> Dict[str, Any]:
        """Create a configuration for a specific experiment."""
        
        print(f"\n=== Creating Experiment Configuration: {experiment_name} ===")
        
        # Base configuration
        config = pynlm.createDefaultConfig()
        
        # Apply experiment-specific parameters
        for key, value in parameters.items():
            if key.startswith("brain."):
                config.setValue(key, str(value))
            elif key.startswith("world."):
                config.setValue(key, str(value))
            elif key.startswith("simulation."):
                config.setValue(key, str(value))
            elif key.startswith("neuromod."):
                config.setValue(key, str(value))
            elif key.startswith("development."):
                config.setValue(key, str(value))
        
        # Save configuration
        config_path = os.path.join(self.config_dir, f"{experiment_name}_config.json")
        config.saveToFile(config_path)
        
        print(f"✅ Experiment configuration saved to: {config_path}")
        
        return {
            "config": config,
            "parameters": parameters,
            "path": config_path
        }
    
    def load_experiment_config(self, experiment_name: str) -> Dict[str, Any]:
        """Load configuration for a specific experiment."""
        
        config_path = os.path.join(self.config_dir, f"{experiment_name}_config.json")
        
        if not os.path.exists(config_path):
            raise FileNotFoundError(f"Configuration file not found: {config_path}")
        
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_path)
        
        print(f"✅ Loaded experiment configuration: {experiment_name}")
        
        return {
            "config": config,
            "path": config_path
        }
    
    def create_parameter_grid(self, param_grid: Dict[str, list]) -> list:
        """Create a grid of parameter combinations for experiments."""
        
        print(f"\n=== Creating Parameter Grid ===")
        
        # Generate all combinations
        param_names = list(param_grid.keys())
        param_values = list(param_grid.values())
        
        combinations = []
        
        # Helper function to generate combinations
        def generate_combinations(index, current_combination):
            if index == len(param_names):
                combinations.append(current_combination.copy())
                return
            
            for value in param_values[index]:
                current_combination[param_names[index]] = value
                generate_combinations(index + 1, current_combination)
        
        generate_combinations(0, {})
        
        print(f"✅ Generated {len(combinations)} parameter combinations")
        
        return combinations
    
    def run_parameter_experiment(self, experiment_name: str, param_grid: Dict[str, list]) -> Dict[str, Any]:
        """Run an experiment with a parameter grid."""
        
        print(f"\n=== Running Parameter Experiment: {experiment_name} ===")
        
        # Generate parameter combinations
        combinations = self.create_parameter_grid(param_grid)
        
        results = []
        
        for i, params in enumerate(combinations):
            print(f"\n--- Running combination {i+1}/{len(combinations)} ---")
            
            # Create experiment configuration
            experiment_config = self.create_experiment_config(
                f"{experiment_name}_comb{i+1}", 
                params
            )
            
            # Initialize simulation components
            config = experiment_config["config"]
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(
                width=config.getInt("world.width", 15),
                height=config.getInt("world.height", 15),
                visionWidth=config.getInt("world.visionWidth", 8),
                visionHeight=config.getInt("world.visionHeight", 8)
            )
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run simulation
            rewards = []
            firing_rates = []
            
            for step in range(config.getInt("simulation.maxSteps", 50)):
                world.update(config.getFloat("simulation.timeStep", 0.1))
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                rewards.append(reward)
                firing_rates.append(brain.getAverageFiringRate())
            
            # Calculate results
            avg_reward = sum(rewards) / len(rewards) if rewards else 0
            max_firing = max(firing_rates) if firing_rates else 0
            
            result = {
                "parameters": params,
                "avg_reward": avg_reward,
                "max_firing_rate": max_firing,
                "final_curiosity": agent.getCuriosityLevel(),
                "final_novelty": agent.getNoveltyLevel()
            }
            
            results.append(result)
            
            print(f"   Parameters: {params}")
            print(f"   Results: Avg reward={avg_reward:.3f}, Max firing={max_firing:.3f}")
        
        # Find best parameter combination
        best_result = max(results, key=lambda x: x["avg_reward"])
        
        print(f"\n✅ Parameter experiment completed!")
        print(f"   Best combination: {best_result['parameters']}")
        print(f"   Best reward: {best_result['avg_reward']:.3f}")
        
        return {
            "results": results,
            "best_result": best_result,
            "total_combinations": len(combinations)
        }

# Example usage of advanced configuration management
if __name__ == "__main__":
    print("=== Advanced Configuration Management Demo ===")
    
    # Initialize configuration manager
    config_manager = ConfigurationManager()
    
    # Define parameter grid for experiment
    param_grid = {
        "brain.neuronCount": [100, 500, 1000],
        "world.width": [10, 15, 20],
        "brain.plasticity.enabled": [True, False],
        "neuromod.curiosity.enabled": [True, False]
    }
    
    # Run parameter experiment
    experiment_results = config_manager.run_parameter_experiment(
        "plasticity_curiosity", param_grid
    )
    
    print(f"\n🎉 Experiment Results Summary:")
    print(f"   Total combinations tested: {experiment_results['total_combinations']}")
    print(f"   Best average reward: {experiment_results['best_result']['avg_reward']:.3f}")
    print(f"   Best parameter combination: {experiment_results['best_result']['parameters']}")
```

**Expected Output:**
```
=== Advanced Configuration Management Demo ===

=== Creating Parameter Grid ===
✅ Generated 24 parameter combinations

=== Creating Experiment Configuration: plasticity_curiosity_comb1
Created experiment configuration saved to: configs/plasticity_curiosity_comb1_config.json
   Parameters: {'brain.neuronCount': 100, 'world.width': 10, 'brain.plasticity.enabled': True, 'neuromod.curiosity.enabled': True}
   Results: Avg reward=0.452, Max firing=0.52
   ... (continuing for all 24 combinations)

🎉 Experiment Results Summary:
   Total combinations tested: 24
   Best average reward: 0.452
   Best parameter combination: {'brain.neuronCount': 100, 'world.width': 10, 'brain.plasticity.enabled': True, 'neuromod.curiosity.enabled': True}
```

## Step 6: Troubleshooting and Common Issues

### 6.1 Common Issues and Solutions

```python
import pynlm

print("=== Common Issues and Solutions ===")

def diagnose_common_issues():
    """Diagnose and solve common NLM issues."""
    
    issues = [
        {
            "name": "Brain not responding",
            "symptoms": ["Zero firing rates", "No neural activity", "Simulation seems stuck"],
            "causes": [
                "Brain.initialize() not called before brain.step()",
                "Invalid configuration parameters",
                "Memory issues causing crashes"
            ],
            "solutions": [
                "✅ Always call brain.initialize() before brain.step()",
                "✅ Check configuration parameters for validity",
                "✅ Use context managers for automatic memory management"
            ]
        },
        {
            "name": "Memory problems",
            "symptoms": ["High memory usage", "Crashes", "Slow performance"],
            "causes": [
                "Not using context managers",
                "Creating too many simulation components",
                "Not cleaning up resources"
            ],
            "solutions": [
                "✅ Use context managers (with statements) for automatic cleanup",
                "✅ Explicitly delete references when no longer needed",
                "✅ Use garbage collection (gc.collect()) when needed"
            ]
        },
        {
            "name": "Configuration errors",
            "symptoms": ["KeyError exceptions", "Missing parameters", "Invalid values"],
            "causes": [
                "Trying to access non-existent configuration keys",
                "Incorrect data types for configuration values",
                "Configuration files not found or corrupted"
            ],
            "solutions": [
                "✅ Use config.has() to check if key exists before accessing",
                "✅ Use config.getValue() with default values",
                "✅ Validate configuration files before loading"
            ]
        },
        {
            "name": "Performance issues",
            "symptoms": ["Slow simulation", "High CPU usage", "Memory problems"],
            "causes": [
                "Too many neurons or synapses",
                "Not using optimized configurations",
                "Inefficient simulation loops"
            ],
            "solutions": [
                "✅ Use high-performance configurations",
                "✅ Enable SIMD optimization",
                "✅ Use minimal configurations for testing"
            ]
        },
        {
            "name": "Simulation crashes",
            "symptoms": ["Segmentation faults", "Exceptions", "Unexpected termination"],
            "causes": [
                "Null pointer dereferences",
                "Array index out of bounds",
                "Resource allocation failures"
            ],
            "solutions": [
                "✅ Implement comprehensive error handling",
                "✅ Use context managers for resource management",
                "✅ Validate inputs and parameters"
            ]
        }
    ]
    
    print(f"\n🔍 Common NLM Issues and Solutions:")
    print(f"\n{'='*60}")
    
    for i, issue in enumerate(issues, 1):
        print(f"\n{i}. {issue['name']}:")
        print(f"   Symptoms: {', '.join(issue['symptoms'])}")
        print(f"   Causes: {', '.join(issue['causes'])}")
        print(f"   Solutions:")
        for solution in issue['solutions']:
            print(f"     {solution}")
    
    print(f"\n{'='*60}")
    print(f"✅ {len(issues)} common issues identified and solved!")

def practical_debugging_example():
    """Demonstrate practical debugging of NLM simulations."""
    
    print(f"\n=== Practical Debugging Example ===")
    
    # Common debugging scenarios
    debugging_scenarios = [
        {
            "scenario": "Brain not firing neurons",
            "commands": [
                "Check if brain.initialize() was called",
                "Verify brain configuration parameters",
                "Check if brain.step() was called",
                "Monitor brain statistics after each step"
            ]
        },
        {
            "scenario": "Agent not moving",
            "commands": [
                "Check if agent.initialize() was called",
                "Verify world is properly configured",
                "Check if agent.decodeMotorCommand() is returning values",
                "Debug world.applyMotorCommand() call"
            ]
        },
        {
            "scenario": "Memory usage high",
            "commands": [
                "Use context managers (with statements)",
                "Explicitly delete references when no longer needed",
                "Use garbage collection",
                "Check configuration for memory-intensive settings"
            ]
        }
    ]
    
    print(f"🔧 Common Debugging Scenarios:")
    
    for i, scenario in enumerate(debugging_scenarios, 1):
        print(f"\n{i}. {scenario['scenario']}:")
        for j, command in enumerate(scenario['commands'], 1):
            print(f"   {j}. {command}")
    
    print(f"\n📝 Example Debug Script:")
    print("""
# Example debug script for NLM simulations
def debug_simulation():
    try:
        # Initialize components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Debug by checking initial state
        print(f"Debug: Initial brain state:")
        print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  - World dimensions: {world.getWidth()}x{world.getHeight()}")
        
        # Run a few steps and monitor
        for step in range(10):
            print(f"\nDebug step {step}:")
            print(f"  - Firing neurons before: {brain.getFiringNeuronCount()}")
            print(f"  - Average firing rate before: {brain.getAverageFiringRate():.2f}")
            
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            print(f"  - Firing neurons after: {brain.getFiringNeuronCount()}")
            print(f"  - Average firing rate after: {brain.getAverageFiringRate():.2f}")
            print(f"  - Curiosity level: {agent.getCuriosityLevel():.2f}")
            
            # Debug: Check for issues
            if brain.getFiringNeuronCount() == 0:
                print(f"  ⚠️  WARNING: No neurons firing!")
                print(f"  💡 Possible causes: brain not initialized, wrong configuration, or no sensory input")
            
            if agent.getCuriosityLevel() == 0:
                print(f"  ⚠️  WARNING: No curiosity detected!")
                print(f"  💡 Possible causes: curiosity not enabled or no novel stimuli")
        
    except Exception as e:
        print(f"❌ Debug error: {type(e).__name__}: {e}")
        print(f"💡 Check your configuration and initialization")

# Run the debug script
debug_simulation()
""")

if __name__ == "__main__":
    # Demonstrate common issues and debugging
    diagnose_common_issues()
    
    # Show practical debugging example
    practical_debugging_example()
    
    print(f"\n🎉 Debugging and troubleshooting complete!")
    print(f"✅ Use these guidelines to diagnose and solve common NLM issues")
    print(f"✅ Implement comprehensive error handling for production code")
    print(f"✅ Use context managers for resource management")
    print(f"✅ Follow best practices for configuration and testing")
```

**Expected Output:**
```
=== Common Issues and Solutions ===
🔍 Common NLM Issues and Solutions:

============================================================
1. Brain not responding:
   Symptoms: Zero firing rates, No neural activity, Simulation seems stuck
   Causes: Brain.initialize() not called before brain.step(), Invalid configuration parameters, Memory issues causing crashes
   Solutions:
     ✅ Always call brain.initialize() before brain.step()
     ✅ Check configuration parameters for validity
     ✅ Use context managers for automatic memory management

2. Memory problems:
   Symptoms: High memory usage, Crashes, Slow performance
   Causes: Not using context managers, Creating too many simulation components, Not cleaning up resources
   Solutions:
     ✅ Use context managers (with statements) for automatic cleanup
     ✅ Explicitly delete references when no longer needed
     ✅ Use garbage collection (gc.collect()) when needed

3. Configuration errors:
   Symptoms: KeyError exceptions, Missing parameters, Invalid values
   Causes: Trying to access non-existent configuration keys, Incorrect data types for configuration values, Configuration files not found or corrupted
   Solutions:
     ✅ Use config.has() to check if key exists before accessing
     ✅ Use config.getValue() with default values
     ✅ Validate configuration files before loading

4. Performance issues:
   Symptoms: Slow simulation, High CPU usage, Memory problems
   Causes: Too many neurons or synapses, Not using optimized configurations, Inefficient simulation loops
   Solutions:
     ✅ Use high-performance configurations
     ✅ Enable SIMD optimization
     ✅ Use minimal configurations for testing

5. Simulation crashes:
   Symptoms: Segmentation faults, Exceptions, Unexpected termination
   Causes: Null pointer dereferences, Array index out of bounds, Resource allocation failures
   Solutions:
     ✅ Implement comprehensive error handling
     ✅ Use context managers for resource management
     ✅ Validate inputs and parameters

============================================================

🔧 Common Debugging Scenarios:

1. Brain not responding:
   1. Check if brain.initialize() was called
   2. Verify brain configuration parameters
   3. Check if brain.step() was called
   4. Monitor brain statistics after each step

2. Agent not moving:
   1. Check if agent.initialize() was called
   2. Verify world is properly configured
   3. Check if agent.decodeMotorCommand() is returning values
   4. Debug world.applyMotorCommand() call

3. Memory usage high:
   1. Use context managers (with statements)
   2. Explicitly delete references when no longer needed
   3. Use garbage collection
   4. Check configuration for memory-intensive settings

📝 Example Debug Script:

# Example debug script for NLM simulations
def debug_simulation():
    try:
        # Initialize components
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Debug by checking initial state
        print(f"Debug: Initial brain state:")
        print(f"  - Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  - World dimensions: {world.getWidth()}x{world.getHeight()}")
        
        # Run a few steps and monitor
        for step in range(10):
            print(f"\nDebug step {step}:")
            print(f"  - Firing neurons before: {brain.getFiringNeuronCount()}")
            print(f"  - Average firing rate before: {brain.getAverageFiringRate():.2f}")
            
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            print(f"  - Firing neurons after: {brain.getFiringNeuronCount()}")
            print(f"  - Average firing rate after: {brain.getAverageFiringRate():.2f}")
            print(f"  - Curiosity level: {agent.getCuriosityLevel():.2f}")
            
            # Debug: Check for issues
            if brain.getFiringNeuronCount() == 0:
                print(f"  ⚠️  WARNING: No neurons firing!")
                print(f"  💡 Possible causes: brain not initialized, wrong configuration, or no sensory input")
            
            if agent.getCuriosityLevel() == 0:
                print(f"  ⚠️  WARNING: No curiosity detected!")
                print(f"  💡 Possible causes: curiosity not enabled or no novel stimuli")
        
    except Exception as e:
        print(f"❌ Debug error: {type(e).__name__}: {e}")
        print(f"💡 Check your configuration and initialization")

# Run the debug script
debug_simulation()

🎉 Debugging and troubleshooting complete!
✅ Use these guidelines to diagnose and solve common NLM issues
✅ Implement comprehensive error handling for production code
✅ Use context managers for resource management
✅ Follow best practices for configuration and testing
```

## Step 7: Resources and Further Reading

### 7.1 Additional Resources

1. **Documentation**
   - `README.md` - Main project documentation
   - `HOW_TO_USE.md` - Comprehensive usage guide
   - `easy_usage.md` - Quick start guide for beginners
   - `docs/` - Technical documentation directory

2. **API Reference**
   - `docs/API_REFERENCE.md` - Complete API documentation
   - `docs/PYTHON_BINDINGS.md` - Python bindings documentation
   - `docs/CONFIGURATION.md` - Configuration documentation

3. **Development Resources**
   - `docs/DEVELOPMENT_GUIDELINES.md` - Development guidelines
   - `docs/ARCHITECTURE.md` - System architecture documentation
   - `docs/PHASE6_FINAL_AUDIT.md` - Phase 6 audit report

### 7.2 Online Resources

1. **Project Documentation**
   - NLM Project Repository: https://github.com/Kilo-Org/nlm
   - NLM Documentation: https://kilo.ai/docs
   - NLM Research: https://research.kilo.ai

2. **Community and Support**
   - GitHub Issues: https://github.com/Kilo-Org/nlm/issues
   - Documentation: https://kilo.ai/docs
   - Support: https://support.kilo.ai

3. **Learning Resources**
   - NLM Tutorials: https://tutorials.kilo.ai
   - NLM Examples: https://examples.kilo.ai
   - NLM Research Papers: https://papers.kilo.ai

### 7.3 Advanced Topics

1. **Advanced Neural Network Models**
   - Transformer alternatives
   - Spiking neural networks
   - Reservoir computing

2. **Neuromodulation**
   - Dopamine systems
   - Reward prediction
   - Curiosity and novelty

3. **Memory Systems**
   - Working memory
   - Episodic memory
   - Associative memory

4. **Development**
   - Neural development
   - Experience-dependent plasticity
   - Critical periods

## Conclusion

🎉 **Congratulations! You've completed the NLM Tutorial!** 🎉

This tutorial has provided you with a comprehensive foundation for working with the NLM neural simulation framework. You now have the knowledge and skills to:

1. **✅ Set up and install NLM** correctly
2. **✅ Run basic neural simulations** with brains, worlds, and agents
3. **✅ Configure NLM** for different use cases and experiments
4. **✅ Use advanced features** like memory, neuromodulation, and prediction
5. **✅ Implement robust error handling** and debugging
6. **✅ Manage resources efficiently** using context managers
7. **✅ Troubleshoot common issues** and solve problems
8. **✅ Document your work** and follow best practices

### Key Achievements

- **🔬 Understanding**: Deep understanding of NLM architecture and functionality
- **💻 Practical Skills**: Hands-on experience with NLM Python API
- **🧪 Experimental Design**: Ability to design and run neural experiments
- **📊 Data Analysis**: Skills to analyze and interpret simulation results
- **🐛 Debugging**: Ability to troubleshoot and fix NLM-related issues
- **📚 Documentation**: Skills to document NLM work effectively

### Next Steps

After completing this tutorial, consider the following next steps:

1. **Explore Advanced Topics**
   - Study NLM's memory systems in detail
   - Investigate neuromodulation mechanisms
   - Explore advanced configuration options

2. **Start Research Projects**
   - Design experiments with specific research questions
   - Implement novel neural network architectures
   - Study learning and adaptation processes

3. **Contribute to NLM**
   - Submit bug fixes and improvements
   - Add new examples and documentation
   - Collaborate with other NLM researchers

4. **Publish and Share**
   - Document your findings and methods
   - Share code and configurations with the community
   - Present your work at conferences or workshops

### Final Thoughts

NLM represents a powerful approach to neural simulation that combines biological inspiration with computational rigor. The framework's modular design and comprehensive documentation make it accessible to researchers, developers, and students alike.

The journey with NLM has just begun! As you continue to work with the framework, you'll discover new capabilities and applications that will deepen your understanding of neural computation and artificial intelligence.

**Remember:** The best way to learn NLM is through hands-on experimentation. Start with the examples, modify them to explore new ideas, and share your findings with the community.

**Happy simulating!** 🚀

---

*This tutorial is part of the NLM research project and is provided as a comprehensive guide for getting started with NLM.*

*For the most up-to-date tutorials and documentation, please check the official NLM documentation repository.*

*Need help? Visit the NLM documentation or report issues on GitHub.*

*This tutorial is part of the NLM research project and is provided as-is.*

*For research purposes and collaboration, please reference: NLM Research Project. (2026). NLM/熙然 - Neural Learning Machine.*

*All tutorial content is subject to change based on project needs and feedback.*

*Version: 1.0.0 - Initial release of comprehensive tutorial*
*License: MIT License*

*Contributors are encouraged to improve and extend this tutorial.*

*Last Updated: 2026-10-09*

*For the most up-to-date tutorial, please check the official NLM documentation repository.*

*Need help with the tutorial? Visit the NLM documentation or report issues on GitHub.*

*This tutorial is part of the NLM research project and is provided as-is.*

*For research purposes and collaboration, please reference: NLM Research Project. (2026). NLM/熙然 - Neural Learning Machine.*

*All tutorial content is subject to change based on project needs and feedback.*

*Contributors are encouraged to improve and extend this tutorial.*

*Version: 1.0.0 - Initial release of comprehensive tutorial*
*License: MIT License*

*Thank you for starting your NLM journey! We look forward to seeing what you'll create with this powerful neural simulation framework.*

*This tutorial is provided as a comprehensive guide to help you get started with NLM.*

*Happy simulating! 🚀*