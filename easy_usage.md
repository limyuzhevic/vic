# NLM - Easy Usage Guide for Beginners and Advanced Users

This guide covers everything you need to know to use the NLM (Neural Learning Machine) system. Whether you're a beginner or an advanced user, this guide will help you get the most out of NLM.

---

## Quick Overview

NLM is a brain-inspired spiking neural network simulator written in C++20 with Python bindings. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, multiple plasticity mechanisms (STDP, Hebbian, structural), and integrated memory, prediction, cognition, and neuromodulation systems.

## Part 1: Quick Start for Beginners

### The 3 Core Components

1. **Brain** - The virtual brain that thinks, learns, and stores memories
2. **World** - The environment the brain lives in (sensory inputs, objects, goals)
3. **Agent** - The bridge connecting brain to world (sensory processing, motor decoding)

### Basic Workflow

```python
import pynlm

# Step 1: Create everything
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Step 2: Run simulation
for step in range(1000):
    world.update(0.1)                           # Update environment
    percept = world.getSensoryPercept()        # What brain sees
    agent.processSensoryInput(percept)         # Brain processes
    brain.step(step)                            # Brain thinks
    action = agent.decodeMotorCommand()        # Brain decides
    world.applyMotorCommand(action, world.getSimulationTime())  # Execute
    
# Step 3: Check results
print(f"Simulation done: {brain.getTotalSpikeCount()} spikes")
print(f"Neurons firing: {brain.getFiringNeuronCount()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

## Part 2: Configuration and Customization

### Brain Configuration

```python
# Basic configuration
config = pynlm.createDefaultConfig()

# Custom settings
config.set("brain.neuron_count", 1000)
config.set("brain.synapse_density", 0.1)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)
config.set("neuromod.curiosity.enable", True)
```

### World Configuration

```python
world = pynlm.createSimpleWorld()

# Configure world dimensions and agent
world.configure(
    width=20,           # World width in units
    height=20,          # World height in units
    visionWidth=8,      # Vision width pixels
    visionHeight=8      # Vision height pixels
)

# Set starting position
world.setAgentStart(10.0, 10.0)

# Environment settings
world.setMaxEnergy(100.0)
world.setEnergyDecayRate(0.1)
world.setRandomSeed(42)  # For reproducibility
```

## Part 3: Enabling Advanced Features

### Reward Learning

```python
agent.enableRewardModulation(True)      # Learn from rewards
```

### Structural Plasticity

```python
agent.enableStructuralPlasticity(True)  # Grow new connections, prune old ones
```

### Development

```python
agent.enableDevelopment(True)           # Brain matures over time
```

### Curiosity-Driven Exploration

```python
agent.enableCuriosity(True)             # Explore new things based on novelty
```

## Part 4: Real-World Examples

### Example 1: Navigation Task

```python
def run_navigation_task(steps=500):
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Run navigation simulation
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Show progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    return brain
```

### Example 2: Learning Experience Storage

```python
def run_experience_learning(steps=1000):
    # Create agent with memory systems
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Track learning progress
    learning_history = []
    
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Record learning metrics
        if step % 50 == 0:
            learning_history.append({
                'step': step,
                'spikes': brain.getTotalSpikeCount(),
                'firing': brain.getFiringNeuronCount(),
                'reward': reward,
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel()
            })
    
    return brain, learning_history
```

## Part 5: Advanced Features for Expert Users

### 5.1 Direct Neural Control

```python
# Inject specific current into neurons
brain.injectCurrent(neuron_id, 50.0)  # Strong excitation
brain.injectCurrentToNeurons(NeuronType.Sensory, 10.0)  # All sensory neurons
```

### 5.2 Checkpoint Management

```python
# Save brain state (checkpointing)
brain.save("my_checkpoint.bin")

# Load brain state
brain.load("my_checkpoint.bin")
```

### 5.3 Custom Neural Operations

```python
# Add new neural regions
region_id = brain.addRegion("cortex")

# Get brain statistics
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getAverageFiringRate()
brain.getExcitationInhibitionRatio()

# Get developmental stage
brain.getDevelopmentalStage()
```

### 5.4 Performance Monitoring

```python
# Monitor simulation performance
start_time = time.time()
for step in range(10000):
    brain.step(step)
    # ... your simulation loop ...

end_time = time.time()
print(f"Simulation took {end_time - start_time:.2f} seconds")
```

## Part 6: Debugging and Troubleshooting

### Common Issues and Solutions

**Issue: "My brain isn't doing anything"
```python
# Solution:
# 1. Check initialization
brain.initialize()  # Always call this first!

# 2. Ensure sensory input
agent.processSensoryInput(world.getSensoryPercept())

# 3. Run more steps
for step in range(1000):  # More steps than 100
    brain.step(step)
```

**Issue: "The agent isn't moving"
```python
# Solution:
# 1. Call world.update() to advance time
world.update(0.1)

# 2. Process sensory input so brain has information
agent.processSensoryInput(world.getSensoryPercept())

# 3. Decode motor command and apply it
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, world.getSimulationTime())
```

**Issue: "Everything is 0"
```python
# Solution:
# 1. Brains need time to "warm up" - try more steps
for step in range(1000):  # Warm-up period
    brain.step(step)

# 2. Ensure sensory input is connected
agent.processSensoryInput(world.getSensoryPercept())

# 3. Check neural parameters
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
```

## Part 7: Command-Line Tools

### nlm (Phase 2 Demo)

```bash
# Run neural computation demo
./nlm                 # Phase 2 demo in src/main.cpp
./nlm_phase3_demo    # Phase 3 demo
./nlm_phase4_demo    # Phase 4 demo
./nlm_phase6_demo    # Phase 6 integration test
./nlm_benchmark      # Performance benchmark
```

### Building from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake (Release build for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build all targets
make -j4

# Run tests (if built)
ctest --output-on-failure
```

### Python Library Installation

```bash
# Install from source (recommended for development)
pip install pybind11 scikit-build-core pytest numpy
pip install .

# Or install in development mode
pip install -e .
```

## Part 8: Quick Reference Guide

### Core Classes and Their Purpose

| Class | Purpose | Typical Usage |
|-------|---------|---------------|
| **Brain** | Central neural processor | `pynlm.createBrain(config)` |
| **Config** | Configuration management | `pynlm.createDefaultConfig()` |
| **AgentBrain** | Brain-world interface | `pynlm.createAgentBrain(brain)` |
| **SimpleWorld** | 2D simulation environment | `pynlm.createSimpleWorld()` |
| **SensoryInput** | Sensory data (vision, touch, etc.) | `world.getSensoryPercept()` |
| **Action** | Motor output representation | `agent.decodeMotorCommand()` |

### Key Methods

#### Brain Methods
- `createBrain(config)` - Create brain with configuration
- `initialize()` - Initialize brain (always call this!)
- `step(step)` - Run one simulation step
- `reset()` - Reset brain state
- `save(filepath)` - Save brain state
- `load(filepath)` - Load brain state
- `getTotalNeuronCount()` - Get total neurons
- `getTotalSynapseCount()` - Get total synapses
- `getAverageFiringRate()` - Get average firing rate

#### Config Methods
- `createDefaultConfig()` - Create default config
- `set(key, value)` - Set configuration value
- `has(key)` - Check if key exists
- `loadFromFile(filepath)` - Load from JSON file
- `saveToFile(filepath)` - Save to JSON file

#### World Methods
- `createSimpleWorld()` - Create 2D world
- `configure(width, height, visionWidth, visionHeight)` - Set dimensions
- `update(timestep)` - Update simulation time
- `getSensoryPercept()` - Get agent's sensory input
- `getAgentBody()` - Get agent state
- `applyMotorCommand(cmd, time)` - Execute action

#### Agent Methods
- `createAgentBrain(brain)` - Create agent-brain interface
- `initialize(world)` - Initialize with world
- `processSensoryInput(percept)` - Process sensory data
- `decodeMotorCommand()` - Get motor action
- `applyRewardModulation(reward, predicted)` - Apply learning
- `updateDevelopment(timestep)` - Update development

## Part 9: Best Practices

### 1. Always Initialize

```python
# DO: Always initialize before using
brain.initialize()

# DON'T: Skip initialization (causes crashes)
brain.step(0)  # Crashes!
```

### 2. Use Configuration

```python
# DO: Use custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
brain = pynlm.createBrain(config)

# DON'T: Hardcode defaults
brain = pynlm.createBrain()  # Uses hardcoded defaults
```

### 3. Monitor Progress

```python
# DO: Log progress regularly
for step in range(1000):
    # ... your simulation ...
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# DON'T: Let simulations run silently
for step in range(1000000):
    brain.step(step)  # No feedback
```

### 4. Use Appropriate Simulation Length

```python
# DO: Use appropriate step counts
for step in range(1000):   # For quick tests
    brain.step(step)

for step in range(100000): # For comprehensive learning
    brain.step(step)
```

## Part 10: When to Use NLM

### Use NLM For:
- **Neural simulation research** - Study spiking neural dynamics
- **Brain-inspired AI** - Implement brain-like learning and cognition
- **Developmental psychology** - Simulate brain development
- **Neuromodulation studies** - Explore reward, curiosity, novelty
- **Memory research** - Investigate working and episodic memory
- **Cognition research** - Study attention, planning, decision-making

### Don't Use NLM For:
- **High-frequency trading** - Not designed for speed-critical applications
- **Large-scale deep learning** - Slower than modern frameworks
- **Real-time control systems** - Not optimized for real-time performance
- **Image classification** - Not designed for traditional ML tasks

---

## Acknowledgements

This guide is based on the comprehensive NLM documentation and the extensive work done across all 6 phases of the Neural Learning Machine project.

For the full documentation and source code, visit: https://github.com/nlm-project/nlm

For technical support and community discussions, join our Discord: https://discord.gg/nlm
