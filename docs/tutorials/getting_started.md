# NLM Getting Started Guide

## Overview

This tutorial provides a comprehensive introduction to the NLM (熙然) project. NLM is an experimental artificial developmental brain that learns from experience through interaction with an environment. The goal is to create a neural system that develops increasingly complex abilities through experience.

## Prerequisites

- **Basic programming knowledge** (C++ or Python preferred)
- **Understanding of neural networks** (optional)
- **Comfort with command-line interfaces** (for building from source)
- **Curiosity** - The project is research-oriented and encourages exploration

## Installation and Setup

### 1. Building from Source

NLM is primarily a C++ project, but includes Python bindings for easier interaction. Here's how to get started:

```bash
# Clone the repository
# (This assumes you already have the code)

# Create build directory and configure
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4

# Run the integration demo
./nlm_phase6_demo
```

### 2. Python Installation

For Python users, you can install NLM directly:

```bash
# Install via pip (if available)
pip install .

# Or install in development mode
pip install -e .

# Test the installation
python3 -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

### 3. Running Tests

After building, you can run the test suite:

```bash
# Run C++ tests
cd build
ctest --output-on-failure
```

## Your First Brain Simulation

### Method 1: C++ (Recommended for learning the system)

Create a new file `first_brain.cpp`:

```cpp
#include <iostream>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", 1000ULL);
    config->set("region_count", 2ULL);
    config->set("connection_probability", 0.1f);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    std::cout << "Brain created with " << brain->getTotalNeuronCount() 
              << " neurons\n";
    
    // Run simulation
    for (size_t step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 25 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " firing neurons\n";
        }
    }
    
    std::cout << "Simulation complete!\n";
    return 0;
}
```

Compile and run:

```bash
# In build directory
g++ ../first_brain.cpp -o first_brain ../src/brain/Brain.cpp ../src/core/Config/Config.cpp
./first_brain
```

### Method 2: Python (easier for experimentation)

Create a file `first_brain.py`:

```python
import pynlm
import time

# Create a simple brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("First brain simulation")
print("=======================")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation
for step in range(50):
    brain.step(step)
    
    if step % 10 == 0:
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        print(f"Step {step:3d}: {firing:4d} firing neurons, {rate:6.2f} Hz")

print("\nSimulation complete!")
```

Run it:

```bash
python3 first_brain.py
```

### Method 3: Complete Agent with World

For the full experience, create a complete agent-world system:

```python
import pynlm

def run_agent_episode():
    # 1. Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 3. Create agent
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # 4. Enable features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # 5. Run episode
    for step in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        if step % 20 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            print(f"Step {step}: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
    
    return brain, agent, world

# Run the episode
brain, agent, world = run_agent_episode()
print("Agent episode complete!")
```

## Understanding the Components

### Core Components

NLM consists of several key components:

1. **Brain** - The neural network
   - Neurons that process information
   - Synapses that connect neurons
   - Regions that organize neurons functionally

2. **World** - The environment
   - Grid-based environment with cells
   - Resources, hazards, and landmarks
   - Provides sensory input to agents

3. **Agent** - The interface
   - Processes sensory input
   - Generates motor commands
   - Modulates learning through neuromodulation

### Key Features

1. **Neuromodulation** - Learning signals
   - Dopamine for reward prediction error
   - Curiosity for exploration
   - Novelty detection for adaptation

2. **Memory Systems** (Phase 4+)
   - Working memory for temporary storage
   - Episodic memory for experiences
   - Associative memory for pattern relationships

3. **Prediction System** (Phase 4+)
   - Temporal sequence prediction
   - Action consequence prediction
   - Confidence and error signals

4. **Development** (Phase 6+)
   - Developmental stages affecting plasticity
   - Structural changes with age
   - Maturation of abilities

## Next Steps

### Explore More Examples

Run the existing demo programs:

```bash
# Integration demo (Phase 6)
./nlm_phase6_demo

# Phase 4 cognition demo
./nlm_phase4_demo

# Phase 3 world interaction demo
./nlm_phase3_demo
```

### Read the Documentation

- **HOW_TO_USE.md** - Complete Python API guide
- **easy_usage.md** - Beginner-friendly guide
- **docs/ARCHITECTURE.md** - System architecture
- **docs/SCIENCE.md** - Scientific background

### Try Experimental Features

The NLM project is research-oriented. Don't be afraid to experiment:

- Modify configurations in `configs/` directory
- Create your own experiment scenarios
- Test different neuron types and plasticity rules
- Explore the Python bindings for quick prototyping

## Troubleshooting Common Issues

### "Brain doesn't do anything"

**Problem**: Brain appears inactive with no neurons firing.

**Solutions**:
1. Ensure `brain.initialize()` is called before `brain.step()`
2. Try running more simulation steps (neurons need time to "warm up")
3. Check that sensory input is properly connected
4. Verify configuration parameters (neuron count, connection probability)

### "ImportError: No module named 'pynlm'"

**Problem**: Python bindings not installed.

**Solutions**:
1. Install Python bindings: `pip install -e .`
2. Ensure CMake finds Python
3. Check `pyproject.toml` configuration

### "Segmentation fault"

**Problem**: Crash during execution.

**Solutions**:
1. Use debug build: `cmake .. -DCMAKE_BUILD_TYPE=Debug`
2. Check for null pointers or invalid memory access
3. Verify all required components are initialized

## Community and Further Learning

### Resources

- **GitHub Repository**: Source code and examples
- **Phase 6 Documentation**: Complete system documentation
- **Experiment Examples**: Multiple demo programs
- **Python API Examples**: Interactive examples in `HOW_TO_USE.md`

### Contributing

NLM is an open research project. Contributions are welcome:

- Bug reports and fixes
- Documentation improvements
- New experiment designs
- Performance optimizations
- Python example additions

## Conclusion

You now have the basic tools to start working with NLM. Remember:

1. **Start simple**: Begin with basic examples before complex ones
2. **Experiment**: The project is designed for research and exploration
3. **Iterate**: Modify and improve based on your observations
4. **Document**: Share your findings and insights with the community

The NLM project aims to investigate whether cognitive abilities can emerge from neural dynamics without pre-programmed concepts or symbolic AI. Your contributions help advance this research!

---

*Next steps in your NLM journey:*
1. Run `HOW_TO_USE.md` for Python API details
2. Try one of the demo programs
3. Create your first experiment
4. Explore the Python bindings for rapid prototyping
5. Consider extending NLM for your specific research interests