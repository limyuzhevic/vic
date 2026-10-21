# NLM (Neural Learning Machine) - Quick Start Guide

## Overview

NLM (熙然, meaning "serene flow") is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms.

**Key Features:**
- High-performance spiking neural networks
- Event-driven simulation for efficiency
- Multiple plasticity rules (STDP, Hebbian, structural)
- Advanced memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Predictive planning and concept formation
- Developmental stages and learning
- Python bindings for easy use

---

## Part 1: Building the C++ Project

### Prerequisites

- **C++ Compiler**: GCC 10+, Clang 12+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **Python**: 3.8+ (for Python bindings)

### Quick Build

```bash
# Navigate to the project directory
cd /path/to/nlm

# Create a build directory
mkdir -p build
cd build

# Configure and build (Release mode for best performance)
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run basic tests
ctest --output-on-failure
```

### Build Outputs

After successful compilation, you'll have:

| Target | Type | Description |
|--------|------|-------------|
| `nlm` | Executable | Main simulation executable |
| `nlm_core` | Static Library | Core brain components |
| `nlm_agent` | Static Library | Agent system |
| `nlm_world` | Static Library | World simulation |
| `nlm_phase3_demo` | Executable | Phase 3 demonstration |
| `nlm_phase4_demo` | Executable | Phase 4 demonstration |

---

## Part 2: Using the Python API (Recommended for Beginners)

### Installation

```bash
# Install from source
pip install .

# For development (editable install)
pip install -e .
```

### Minimal Example

```python
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for i in range(100):
    brain.step(i)

print("Simulation complete!")
```

### Interactive Python Session

```python
>>> import pynlm
>>> config = pynlm.createDefaultConfig()
>>> config.set("brain.neuron_count", 500)
>>> config.set("brain.connection_probability", 0.1)
>>> brain = pynlm.createBrain(config)
>>> brain.initialize()
>>> brain.getTotalNeuronCount()
500
>>> brain.getTotalSynapseCount()
50
```

---

## Part 3: C++ Basic Usage

### Simple C++ Program

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000);
    config->set("connection_probability", 0.1f);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    std::cout << "Simulation complete. Firing neurons: " 
              << brain->getFiringNeuronCount() << std::endl;
    
    return 0;
}
```

---

## Part 4: Configuration Files

### JSON Configuration

Create `config.json`:

```json
{
    "brain": {
        "neuron_count": 1000,
        "connection_probability": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001
        }
    }
}
```

Load it in your program:

```python
import pynlm
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")
```

### Command Line Arguments

```bash
./nlm --config=config.json --neuron_count=2000 --connection_probability=0.15
```

```cpp
// Command line usage
./nlm --neuron_count=1000 --simulation_timestep=0.001
```

---

## Part 5: Key API Reference

### Brain Class

```python
# Creation
brain = pynlm.createBrain(config)

# Initialization
brain.initialize()

# Simulation
brain.step(step_number)                    # With default timestep
brain.step(step_number, time)              # With custom time
brain.reset()                              # Reset brain state

# Input/Output
brain.receiveSensoryInput(sensory_input)   # Inject sensory data
action = brain.produceAction()            # Get motor action

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()      # Currently firing
brain.getAverageFiringRate()      # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance

# State
brain.save("checkpoint.bin")       # Save state
brain.load("checkpoint.bin")       # Load state
```

### Config Class

```python
# Create
config = pynlm.createDefaultConfig()

# Query
if config.has("brain.neuron_count"):
    keys = config.getKeys()

# Modify
config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

### AgentBrain Class

```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()
```

---

## Part 6: Working with Worlds

### Simple 2D World Example

```python
import pynlm

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
agent = pynlm.createAgentBrain(brain)

# Initialize agent with world
agent.initialize(world)

# Run simulation
for step in range(500):
    world.update(timestep=0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Run brain step
    brain.step(step)
    
    # Get motor command
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply to world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Agent at ({world.getAgentBody().getX()}, {world.getAgentBody().getY()})")
```

---

## Part 7: Development Stages

NLM supports developmental stages that affect learning and plasticity:

```python
import pynlm

# Developmental stages
stages = [
    pynlm.DevelopmentalStage.Initial,      # High plasticity
    pynlm.DevelopmentalStage.CriticalPeriod, # Moderate plasticity
    pynlm.DevelopmentalStage.Maturation,   # Low plasticity
    pynlm.DevelopmentalStage.Adult,       # Stable
    pynlm.DevelopmentalStage.Aging        # Declining plasticity
]

# Set development stage
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
```

---

## Part 8: Common Patterns

### Training a Brain

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.enable", True)
config.set("neuromod.dopamine.scale", 1.0)

brain = pynlm.createBrain(config)
brain.initialize()

# Training loop
for episode in range(10):
    # Reset and initialize
    brain.reset()
    brain.initialize()
    
    # Run episode
    for step in range(1000):
        # Apply training signals
        if step % 100 == 0:
            # Reward at specific times
            brain.applyNeuromodulation(pynlm.DopamineLevel::High)
        
        brain.step(step)
        
        # Log progress
        if step % 100 == 0:
            print(f"Episode {episode}, Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Learning rate: {brain.getSTDP()->getLTPWeight()}")
```

### Saving and Loading Checkpoints

```python
import pynlm

# Create and train brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Train for a while
for step in range(500):
    brain.step(step)

# Save checkpoint
brain.save("training_checkpoint_500.bin")

# Later, resume training
brain.load("training_checkpoint_500.bin")

# Continue training
for step in range(500, 1000):
    brain.step(step)
    
    if step % 100 == 0:
        brain.save(f"checkpoint_{step}.bin")
```

---

## Part 9: Troubleshooting

### Common Issues and Solutions

**"Cannot find module 'pynlm'"**
```bash
# Install correctly
pip install --force-reinstall .
```

**"Segmentation fault during simulation"**
```python
# Ensure proper initialization
brain = pynlm.createBrain(config)
brain.initialize()  # ALWAYS call this before stepping

# Reset before re-running
brain.reset()
brain.initialize()
```

**"CMake cannot find Python"**
```bash
pip install scikit-build-core pybind11
cmake .. -DPython_EXECUTABLE=$(which python)
```

**Build Issues**
```bash
# Check CMake version
cmake --version
# Ensure >= 3.16

# Check compiler support
g++ --version  # Should show C++20 support
```

---

## Part 10: Advanced Usage

For more advanced topics, see the full documentation:

- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture overview
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions

### Performance Tips

```python
import pynlm

# Optimized configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)        # Adjust based on memory
config.set("simulation.timestep", 0.0001)    # Smaller steps for accuracy
config.set("plasticity.stdp.enable", False)  # Disable if not needed

# Run optimized simulation
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(10000):
    brain.step(step)
```

---

## Getting Help

**Documentation:** https://nlm.readthedocs.io
**Repository:** https://github.com/nlm-project/nlm
**Issues:** https://github.com/nlm-project/nlm/issues

**Quick Examples:** Look for example scripts in the `examples/` directory or in the Python module.

---

*Last updated: Phase 6 - Final Integration*

This guide covers the essential NLM functionality. For specific use cases, refer to the detailed documentation or examples in the repository.
