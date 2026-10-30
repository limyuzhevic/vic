# NLM Getting Started Guide

Welcome to NLM (Neural Learning Machine)! This guide will help you get started with using NLM to create and experiment with artificial developmental brains.

## What You Need

- NLM installed (see below for installation)
- Basic C++/Python knowledge
- Curiosity and enthusiasm!

## Quick Start: 3 Example Projects

### Project 1: Silent Brain (Just Neurons)

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

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
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

### Project 3: Complete Agent with Learning

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
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

## Using the Command Line

NLM provides several command-line tools:

### Basic Demo
```bash
./nlm_demo
```
Shows a simple brain creating, thinking, and learning.

### Phase 2 Advanced Demo
```bash
./nlm_phase2_demo
```
Demonstrates advanced neural computation with LIF dynamics, STDP, and Hebbian learning.

### Phase 4 Experiment
```bash
./nlm_phase4_demo
```
Runs a complete experimental session with cognitive capabilities.

### Unit Tests
```bash
./nlm_test
```
Runs the full test suite to verify all components work correctly.

### Phase 6 Integration Test
```bash
./nlm_phase6_demo
```
Comprehensive integration test showing all brain systems working together.

## Configuration Files

NLM supports configuration files. Create a `config.json` file:

```json
{
  "brain": {
    "neuron_count": 500,
    "region_count": 2,
    "connection_probability": 0.15,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015
  },
  "neuromodulation": {
    "dopamine_scale": 1.0,
    "curiosity_enable": true,
    "novelty_enable": true
  },
  "development": {
    "enable_development": true,
    "enable_maturation": true
  }
}
```

Load it with:
```bash
./nlm_demo --config config.json
```

## Building from Source

### Prerequisites
- CMake 3.16+
- C++20 compiler
- Python 3.8+
- pybind11
- scikit-build-core

### Build Steps

```bash
# Create build directory
mkdir build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build (use all cores)
make -j$(nproc)

# Install (optional)
make install
```

### Python Installation

For Python bindings, use pip:

```bash
# Install from source
pip install .

# Or install in development mode
pip install -e .

# Install specific version
pip install dist/nlm-*.whl
```

## Understanding NLM Components

### Brain
- **What**: The virtual brain with neurons and synapses
- **How**: Uses Leaky Integrate-and-Fire (LIF) neuron dynamics
- **Why**: Simulates realistic neural computation

### World
- **What**: The environment the brain lives in
- **How**: 2D grid with objects, resources, hazards
- **Why**: Provides sensory input and motor output

### Agent
- **What**: Bridge connecting brain to world
- **How**: Processes sensory input, decodes motor commands
- **Why**: Enables brain-environment interaction

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
```

### Pattern 2: Get Brain Statistics

```python
# Inside your simulation loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

### Build Issues

**"CMake cannot find Python:"**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**"ImportError: No module named 'pynlm':"**
```bash
pip install --force-reinstall .
```

**"Segmentation faults:"**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

## Next Steps

When you're comfortable:

1. **Read the documentation**: `docs/ARCHITECTURE.md` explains how it all works
2. **Experiment with configurations**: Try different neuron counts, connection probabilities, etc.
3. **Explore the demos**: Run `./nlm_phase2_demo` and `./nlm_phase4_demo` to see advanced features
4. **Try the experiments**: `./nlm_phase6_demo` shows the complete integrated brain
5. **Look at the source code**: The demo files are great for learning NLM patterns
6. **Create your own**: Use the patterns and components to build your own neural experiments

## Learning Resources

- **HOW_TO_USE.md**: Detailed guide to using NLM
- **easy_usage.md**: Beginner-friendly overview
- **docs/ARCHITECTURE.md**: System architecture
- **docs/SCIENCE.md**: Scientific background
- **docs/EXPERIMENTS.md**: Experiment descriptions
- **README.md**: Overview and build instructions

## Key Classes and Methods

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

## Neuron Types

| Type | Description |
|------|-------------|
| `Excitatory` | Increases firing probability of target neurons |
| `Inhibitory` | Decreases firing probability of target neurons |
| `Sensory` | Receives input from environment |
| `Motor` | Controls output to environment |
| `Modulatory` | Affects learning and plasticity |
| `Internal` | Internal processing neurons |

## Developmental Stages

| Stage | Description |
|-------|-------------|
| `Initial` | Newborn brain, high plasticity |
| `CriticalPeriod` | Rapid learning phase |
| `Maturation` | Refining connections |
| `Adult` | Stable, efficient operation |
| `Aging` | Gradual degradation |

You're now ready to use NLM! Happy experimenting! 🚀
