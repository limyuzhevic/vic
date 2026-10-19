# NLM (Neural Learning Machine) - Getting Started Guide

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

**Key Features:**
- Brain-inspired spiking neural network simulation
- Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- Multiple plasticity mechanisms (STDP, Hebbian, structural)
- Developmental learning and adaptation
- Curiosity-driven exploration

---

## Prerequisites

Before you begin, make sure you have:

### System Requirements
- **Operating System**: Linux, macOS, or Windows
- **Memory**: At least 2 GB RAM for basic simulations
- **Disk Space**: 100 MB for source code and build directory

### Software Dependencies
- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)
- **Build Tools**: make, ninja, or your preferred build system

---

## Quick Start (3-Minute Setup)

### Option 1: Install from Pre-built Binary (Recommended for Users)

```bash
# This option requires pre-built binaries, typically available through package managers
# Check your package manager for nlm or pynlm packages

# Example for Linux with package manager:
# sudo apt install nlm pynlm  # (if available)

# If available as Python package:
pip install pynlm
```

### Option 2: Build from Source (Recommended for Developers)

```bash
# Clone the repository
 git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build C++ components
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
pip install .
```

---

## Your First Brain Simulation

Let's create and run your first neural brain simulation in Python:

### Step 1: Basic Brain Setup

```python
import pynlm

# Create a default configuration
config = pynlm.createDefaultConfig()

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
```

### Step 2: Run a Simple Simulation

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

print(f"Simulation completed. Total spikes: {brain.getTotalSpikeCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
```

### Step 3: Create a Complete Agent System

```python
import pynlm

# 1. Setup brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create agent interface
agent = pynlm.createAgentBrain(brain)

# 3. Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# 4. Initialize agent with world
agent.initialize(world)

# 5. Enable learning capabilities
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# 6. Run simulation loop
for step in range(500):
    world.update(0.1)  # Update world
    
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain processes input
    
    brain.step(step)  # Brain thinks
    
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Agent simulation completed!")
print(f"Firing rate: {brain.getAverageFiringRate():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

---

## Learning Path

### For Beginners

1. **Basic Operations**: [BasicBrainSimulation.py](#)
2. **Agent Environment**: [AgentWorldExample.py](#)
3. **Understanding Brain Stats**: [BrainStatistics.py](#)

### For Intermediate Users

1. **Learning Systems**: [LearningBrain.py](#)
2. **World Interaction**: [WorldExploration.py](#)
3. **Configuration Tuning**: [ConfigExamples.py](#)

### For Advanced Users

1. **Custom Architectures**: [AdvancedBrainDesign.py](#)
2. **Performance Optimization**: [PerformanceBenchmarks.py](#)
3. **Research Experiments**: [ResearchExperiments.py](#)

---

## Next Steps

Once you're comfortable with the basics:

1. **Read the Documentation**: 
   - `docs/ARCHITECTURE.md` - System architecture overview
   - `docs/SCIENCE.md` - Scientific background
   - `docs/EXPERIMENTS.md` - Experiment descriptions

2. **Explore Examples**:
   - `examples/` directory contains working code samples
   - Phase 6 demo executables for full integration testing

3. **Customize and Extend**:
   - Modify configuration files in `configs/`
   - Add new world types in `src/world/`
   - Extend neural mechanisms in `src/brain/`

4. **Contributing**:
   - Report issues on GitHub
   - Submit pull requests with tests
   - Improve documentation with examples

---

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of simulation steps
- Check that the brain has enough neurons (default: 1000)

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent's motor neurons are connected

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters (neuron count, connection probability)

### Need More Help?
- Check the `HOW_TO_USE.md` for detailed examples
- Visit the NLM project documentation
- Join the NLM community for support
