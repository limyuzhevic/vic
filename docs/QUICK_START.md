# QUICK_START.md

# NLM Quick Start Guide

## 5-Minute Introduction

Welcome to NLM (熙然) — an experimental artificial developmental brain. This quick start guide will get you up and running with the NLM brain simulation in under 5 minutes.

## Prerequisites

- **C++ compiler**: GCC 10+ or Clang 12+
- **CMake**: Version 3.16+
- **Python 3.8+** (for Python bindings)
- **100MB+ free disk space**

## Step 1: Build from Source

### Recommended Build Command

```bash
# Create build directory and build
cd /path/to/nlm
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Verify Build Success

```bash
# Run Phase 6 demo (integration test)
./nlm_phase6_demo
```

You should see output like:
```
Phase 6 Integration Demo Starting...
Phase 6 Brain Components:
  ✓ Memory Systems
  ✓ Neuromodulation
  ✓ Prediction System
  ✓ Cognition Systems
  ✓ Development Integration
  ✓ Replay and Consolidation
  ✓ Reward System
Phase 6 Demo Complete
```

## Step 2: Install Python Bindings

### From Source (Recommended)

```bash
# Install build dependencies
pip install pybind11 scikit-build-core numpy pytest

# Build and install in development mode
pip install -e .

# Verify installation
python -c "import pynlm; print('NLM Python module loaded successfully')"
```

## Step 3: Run Basic Example

### Python Introduction

```python
# File: examples/python_intro.py

import pynlm

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"✓ Simulation complete: {i+1} steps")
print(f"✓ Average neural activity: {brain.getAverageActivity():.3f}")
print(f"✓ Memory usage: {len(brain.getActiveMemories())} active memories")
```

```bash
python examples/python_intro.py
```

### C++ Basic Example

```cpp
// File: examples/basic_example.cpp

#include <iostream>
#include <memory>
#include <vector>

#include "src/brain/Brain.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"

int main() {
    std::cout << "NLM Basic Example" << std::endl;
    std::cout << "===================" << std::endl << std::endl;

    // Create default configuration
    auto config = std::make_shared<pynlm::Config>();
    config->brain.tau_ref = 20.0f;
    config->brain.v_reset = -65.0f;
    config->brain.v_thresh = -50.0f;
    config->plasticity.enable_stdp = true;
    config->plasticity.enable_hebbian = true;
    config->neuromod.dopamine.enable = true;

    // Create and initialize brain
    auto brain = std::make_shared<pynlm::Brain>(config);
    brain->initialize();

    std::cout << "Brain initialized successfully" << std::endl;
    std::cout << "Number of neurons: " << brain->getNumNeurons() << std::endl;
    std::cout << "Default configuration loaded" << std::endl << std::endl;

    // Run simulation
    std::cout << "Running simulation..." << std::endl;
    for (int step = 0; step < 100; ++step) {
        brain->step(step);
        if (step % 25 == 0) {
            std::cout << "  Step " << step << ": " 
                     << brain->getAverageActivity() << " avg activity" << std::endl;
        }
    }

    std::cout << "Simulation complete!" << std::endl;
    std::cout << "Total steps: 100" << std::endl;

    return 0;
}
```

```bash
# Build and run
cd build
g++ -std=c++20 -I../include -I../src examples/basic_example.cpp libnlm_core.a -o basic_example
./basic_example
```

## Step 4: Explore Phase 6 Integration

### Advanced Integration Example

```python
# File: examples/advanced_integration.py

import pynlm
import numpy as np

# Create a more complex brain setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent with world interaction
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

print("=== Phase 6 Integration Demo ===")
print(f"World size: {world.getWidth()}x{world.getHeight()}")
print(f"Agent initialized: {agent.getMemoryUsage()} memory slots")

# Run an episode
episode_steps = 0
for _ in range(500):
    world.update(0.1)
    
    # Get sensory input and process
    sensory = world.getSensoryPercept()
    agent.processSensoryInput(sensory)
    brain.step(episode_steps)
    
    # Apply motor command
    motor = agent.decodeMotorCommand()
    world.applyMotorCommand(motor, world.getSimulationTime())
    
    episode_steps += 1

print(f"Episode complete: {episode_steps} steps")
print(f"Final brain state: {brain.getAverageActivity():.3f} avg activity")
print(f"Final memory count: {len(brain.getActiveMemories())} active memories")
print("✓ Phase 6 integration successful!")
```

```bash
python examples/advanced_integration.py
```

## Step 5: Neuromodulation Demo

```python
# File: examples/neuromodulation_demo.py

import pynlm

# Create brain with enhanced neuromodulation
config = pynlm.createDefaultConfig()
config->neuromod.dopamine.scale = 2.0f
config->neuromod.curiosity.enable = true
config->neuromod.novelty.enable = true
config->neuromod.prediction.error_sensitivity = 1.5f

brain = pynlm.createBrain(config)
brain.initialize()

print("=== Neuromodulation Demo ===")
print("Brain initialized with enhanced neuromodulation")

# Track neuromodulator activity over time
for step in range(200):
    brain.step(step)
    
    if step % 50 == 0:
        neuromod = brain.getNeuromodulators()
        print(f"Step {step}:")
        print(f"  Dopamine: {neurom.dopamine:.3f}")
        print(f"  Curiosity: {neurom.curiosity:.3f}")
        print(f"  Novelty: {neurom.novelty:.3f}")
        print(f"  Prediction Error: {neurom.prediction_error:.3f}")

print("✓ Neuromodulation demo complete!")
```

```bash
python examples/neuromodulation_demo.py
```

## Step 6: Memory System Demo

```python
# File: examples/memory_system_demo.py

import pynlm

# Create brain focused on memory systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== Memory System Demo ===")
print("Brain initialized with all memory systems")

# Demonstrate memory systems
for step in range(100):
    brain.step(step)
    
    # Get memory statistics
    working_mem = brain.getWorkingMemory()
    episodic_mem = brain.getEpisodicMemory()
    associative_mem = brain.getAssociativeMemory()
    
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Working Memory: {working_mem.size()} entries, "
              f"{working_mem.getActiveCount()} active")
        print(f"  Episodic Memory: {episodic_mem.getCurrentSequence().size()} "
              f"experiences stored")
        print(f"  Associative Memory: {associative_mem.getNumPatterns()} "
              f"patterns learned")

print("✓ Memory system demo complete!")
```

```bash
python examples/memory_system_demo.py
```

## Step 7: Development System Demo

```python
# File: examples/development_demo.py

import pynlm

# Create developing brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("=== Development System Demo ===")
print("Brain initialized in developmental mode")

# Track development across multiple stages
for stage in range(3):
    print(f"\n--- Developmental Stage {stage + 1} ---")
    
    for step in range(50):
        brain.step(stage * 50 + step)
        
        if step % 10 == 0:
            dev_stats = brain.getDevelopmentStats()
            plasticity = brain.getPlasticityStats()
            
            print(f"  Step {step}:")
            print(f"    Neural excitability: {dev_stats.neural_excitability:.3f}")
            print(f"    Plasticity rate: {plasticity.plasticity_rate:.3f}")
            print(f"    Structural connections: {plasticity.structural_connections}")

print("✓ Development system demo complete!")
```

```bash
python examples/development_demo.py
```

## Quick Summary

You now have:

✅ **Built and run NLM successfully**
✅ **Installed Python bindings**
✅ **Run all example demonstrations**

### What's Next?

- Explore the full API documentation in `docs/CPP_API_REFERENCE.md`
- Read advanced usage in `docs/ADVANCED_TOPICS.md`
- Check out Phase 6 integration details in `docs/PHASE6_INTEGRATION_EXAMPLES.md`
- Study the Python API guide in `docs/PYTHON_BINDING_GUIDE.md`

### Get Help

- **Documentation**: See `docs/README.md` for complete guide
- **Issues**: Check the GitHub repository for common problems
- **Community**: Join the NLM community for support

---

*Created for NLM Phase 6 Integration - Last Updated $(date +%Y-%m-%d)*