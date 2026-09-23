# NLM Getting Started Guide

## Overview

Welcome to the NLM (Neural Learning Machine) getting started guide! This guide will help you quickly get started with NLM, whether you're new to neural simulation or looking to explore the new features we've added.

## What is NLM?

NLM is a brain-inspired spiking neural network simulator written in C++20. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms including STDP, Hebbian learning, and structural plasticity.

## Quick Start

### 1. Install NLM

#### Option A: Install from PyPI (Recommended)

```bash
# Install NLM Python bindings
pip install pynlm

# Verify installation
import pynlm
print(f"NLM version: {pynlm.__version__}")
```

#### Option B: Build from Source

```bash
# Clone NLM repository
git clone https://github.com/your-org/nlm.git
cd nlm

# Build with CMake
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Install Python bindings
cd ../python
pip install -e .
```

### 2. Run a Simple Simulation

#### Python Example (Recommended for beginners)

```python
import pynlm

print("=== NLM Quick Start Demo ===")

# 1. Create a brain with default configuration
print("\n1. Creating brain...")
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print(f"   Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"   Total synapses: {brain.getTotalSynapseCount()}")

# 2. Run a simple simulation
print("\n2. Running simulation...")
for step in range(10):
    brain.step(step)
    if step % 5 == 0:
        print(f"   Step {step}: {brain.getFiringNeuronCount()} firing neurons")

print("   ✓ Simple simulation completed!")

# 3. Create a complete agent-world simulation
print("\n3. Creating complete agent-world simulation...")

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

print("   ✓ Agent and world created!")

# 4. Run agent simulation
print("\n4. Running agent simulation...")
for step in range(50):
    # Update world
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell the agent
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides action
    action = agent.decodeMotorCommand()
    
    # Do action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 10 == 0:
        print(f"   Step {step}: "
              f"Neurons firing: {brain.getFiringNeuronCount()}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}")

print("   ✓ Agent simulation completed!")

# 5. Print final statistics
print("\n5. Final statistics:")
print(f"   Total simulation time: {world.getSimulationTime():.1f}")
print(f"   Total neurons: {brain.getTotalNeuronCount()}")
print(f"   Total synapses: {brain.getTotalSynapseCount()}")
print(f"   Current firing neurons: {brain.getFiringNeuronCount()}")
print(f"   Average firing rate: {brain.getAverageFiringRate():.2f}")
print(f"   Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"   Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"   Developmental stage: {brain.getDevelopmentalStage()}")

print("\n=== Quick Start Complete! ===")
print("\nNext steps:")
print("1. Read the comprehensive documentation in docs/")
print("2. Try the tutorial series in tutorials/")
print("3. Experiment with different configurations")
print("4. Explore the advanced features in the API documentation")
```

#### C++ Example (For advanced users)

```cpp
#include <iostream>
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"

int main() {
    std::cout << "=== NLM Quick Start Demo ===" << std::endl;
    
    // 1. Create a brain with default configuration
    std::cout << "\n1. Creating brain..." << std::endl;
    nlm::Config config;
    auto brain = nlm::createBrain(config);
    brain->initialize();
    
    std::cout << "   Brain created with " << brain->getTotalNeuronCount() << " neurons" << std::endl;
    std::cout << "   Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    
    // 2. Run a simple simulation
    std::cout << "\n2. Running simulation..." << std::endl;
    for (int step = 0; step < 10; ++step) {
        brain->step(step);
        if (step % 5 == 0) {
            std::cout << "   Step " << step << ": " 
                      << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    std::cout << "   ✓ Simple simulation completed!" << std::endl;
    
    // 3. Create a complete agent-world simulation
    std::cout << "\n3. Creating complete agent-world simulation..." << std::endl;
    
    auto world = nlm::createSimpleWorld();
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);
    
    auto agent = nlm::createAgentBrain(brain);
    agent->initialize(world);
    
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    std::cout << "   ✓ Agent and world created!" << std::endl;
    
    // 4. Run agent simulation
    std::cout << "\n4. Running agent simulation..." << std::endl;
    for (int step = 0; step < 50; ++step) {
        // Update world
        world->update(0.1);
        
        // Get what the agent sees
        auto percept = world->getSensoryPercept();
        
        // Tell the agent
        agent->processSensoryInput(percept);
        
        // Brain thinks
        brain->step(step);
        
        // Brain decides action
        auto action = agent->decodeMotorCommand();
        
        // Do action in world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (step % 10 == 0) {
            std::cout << "   Step " << step << ": "
                      << "Neurons firing: " << brain->getFiringNeuronCount() << ", "
                      << "Curiosity: " << agent->getCuriosityLevel() << ", "
                      << "Novelty: " << agent->getNoveltyLevel() << std::endl;
        }
    }
    
    std::cout << "   ✓ Agent simulation completed!" << std::endl;
    
    // 5. Print final statistics
    std::cout << "\n5. Final statistics:" << std::endl;
    std::cout << "   Total simulation time: " << world->getSimulationTime() << std::endl;
    std::cout << "   Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "   Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "   Current firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "   Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "   Curiosity level: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "   Novelty level: " << agent->getNoveltyLevel() << std::endl;
    std::cout << "   Developmental stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    
    std::cout << "\n=== Quick Start Complete! ===" << std::endl;
    std::cout << "\nNext steps:" << std::endl;
    std::cout << "1. Read the comprehensive documentation in docs/" << std::endl;
    std::cout << "2. Try the tutorial series in tutorials/" << std::endl;
    stdcout << "3. Experiment with different configurations" << std::endl;
    stdcout << "4. Explore the advanced features in the API documentation" << std::endl;
    
    return 0;
}
```

### 3. Create Your First Project

#### Python Project Example

```python
# my_neural_project.py
import pynlm
import numpy as np
import matplotlib.pyplot as plt

def create_custom_brain(neuron_count=500):
    """Create a custom brain with specific configuration."""
    config = pynlm.createDefaultConfig()
    
    # Customize configuration
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.learning_rate", 0.01)
    config.set("brain.enable_curiosity", True)
    config.set("brain.enable_structural_plasticity", True)
    config.set("brain.enable_development", True)
    
    return pynlm.createBrain(config)

def run_custom_simulation(brain, world, agent, steps=1000):
    """Run a custom simulation with data collection."""
    simulation_data = []
    
    for step in range(steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in agent
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect data
        if step % 10 == 0:
            simulation_data.append({
                'step': step,
                'simulation_time': world.getSimulationTime(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'neuromodulation': agent.getNeuromodulationLevel()
            })
    
    return simulation_data

def analyzeSimulation(data):
    """Analyze simulation data."""
    import pandas as pd
    
    df = pd.DataFrame(data)
    
    print("=== Simulation Analysis ===")
    print(f"Total steps: {len(df)}")
    print(f"Time range: {df['simulation_time'].min():.1f} - {df['simulation_time']:.1f}")
    print(f"Average firing neurons: {df['firing_neurons'].mean():.1f}")
    print(f"Max firing neurons: {df['firing_neurons'].max()}")
    print(f"Average curiosity: {df['curiosity'].mean():.3f}")
    print(f"Average novelty: {df['novelty'].mean():.3f}")
    
    # Plot results
    plt.figure(figsize=(15, 10))
    
    plt.subplot(3, 1, 1)
    plt.plot(df['simulation_time'], df['firing_neurons'])
    plt.title('Firing Neurons over Time')
    plt.ylabel('Firing Neurons')
    
    plt.subplot(3, 1, 2)
    plt.plot(df['simulation_time'], df['curiosity'])
    plt.title('Curiosity over Time')
    plt.ylabel('Curiosity')
    
    plt.subplot(3, 1, 3)
    plt.plot(df['simulation_time'], df['novelty'])
    plt.title('Novelty over Time')
    plt.ylabel('Novelty')
    plt.xlabel('Time')
    
    plt.tight_layout()
    plt.show()
    
    return df

# Main execution
if __name__ == "__main__":
    print("=== Creating Custom NLM Project ===")
    
    # Create components
    config = pynlm.createDefaultConfig()
    brain = create_custom_brain(1000)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Run simulation
    print("Running simulation...")
    simulation_data = run_custom_simulation(brain, world, agent, 5000)
    
    # Analyze results
    print("Analyzing results...")
    df = analyzeSimulation(simulation_data)
    
    print("\n=== Project Complete! ===")
    print("Your custom NLM project is ready for further exploration.")
```

## Exploring NLM Features

### 1. Try Different Configurations

```python
import pynlm

# Small, fast brain
config1 = pynlm.createDefaultConfig()
config1.set("brain.neuron_count", 100)
brain1 = pynlm.createBrain(config1)

# Large, complex brain
config2 = pynlm.createDefaultConfig()
config2.set("brain.neuron_count", 10000)
config2.set("brain.synapse_density", 0.2)
brain2 = pynlm.createBrain(config2)

# Compare performance
import time

start = time.time()
brain1.initialize()
for step in range(1000):
    brain1.step(step)
time1 = time.time() - start

start = time.time()
brain2.initialize()
for step in range(1000):
    brain2.step(step)
time2 = time.time() - start

print(f"Small brain: {time1:.3f}s ({1000/time1:.0f} steps/s)")
print(f"Large brain: {time2:.3f}s ({1000/time2:.0f} steps/s)")
```

### 2. Experiment with World Settings

```python
import pynlm

# Different world configurations
world_configs = [
    {"width": 10, "height": 10, "visionWidth": 4, "visionHeight": 4},
    {"width": 20, "height": 20, "visionWidth": 8, "visionHeight": 8},
    {"width": 30, "height": 30, "visionWidth": 12, "visionHeight": 12},
]

for i, config in enumerate(world_configs):
    world = pynlm.createSimpleWorld()
    world.configure(**config)
    world.reset()
    
    print(f"World {i+1}: {config['width']}x{config['height']}, "
          f"vision: {config['visionWidth']}x{config['visionHeight']}")
```

### 3. Test Learning Features

```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Test different learning combinations
learning_combinations = [
    {"reward": True, "curiosity": False, "development": False},
    {"reward": True, "curiosity": True, "development": False},
    {"reward": True, "curiosity": True, "development": True},
    {"reward": False, "curiosity": True, "development": False},
]

for i, combination in enumerate(learning_combinations):
    # Create fresh agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning features
    agent.enableRewardModulation(combination["reward"])
    agent.enableCuriosity(combination["curiosity"])
    agent.enableDevelopment(combination["development"])
    
    # Run simulation
    for step in range(100):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    print(f"Combination {i+1}: "
          f"Reward={combination['reward']}, "
          f"Curiosity={combination['curiosity']}, "
          f"Development={combination['development']}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
    print()
```

## Documentation Resources

### 1. Documentation Files

- **README.md**: Overview of the NLM project
- **easy_usage.md**: Beginner-friendly usage guide
- **docs/COMPREHENSIVE_DEVELOPMENT_GUIDE.md**: Complete development guide
- **docs/LOGGING_SYSTEM.md**: Documentation for modular logging system
- **docs/CONFIGURATION_SYSTEM.md**: Documentation for configuration system
- **docs/BATCH_PROCESSING.md**: Documentation for batch processing
- **docs/PERFORMANCE.md**: Documentation for performance profiling
- **docs/PYTHON_BINDINGS.md**: Documentation for enhanced Python bindings

### 2. Tutorial Series

The tutorial series covers all major topics:

1. **Tutorial 1: Basic Setup** - Creating your first NLM brain
2. **Tutorial 2: Advanced Configuration** - Schema-based configuration and factory presets
3. **Tutorial 3: Batch Processing** - Efficient batch processing and performance optimization
4. **Tutorial 4: Performance Profiling** - Using profiling tools and optimization
5. **Tutorial 5: Advanced Python Features** - Enhanced Python bindings and advanced usage
6. **Tutorial 6: Real-world Application** - Complete agent simulation and world interaction

### 3. API Documentation

Complete API documentation is available in the Python bindings documentation, including:
- Core classes (Brain, AgentBrain, Config, Logger)
- Performance classes (BatchBrain, PerformanceProfiler)
- Python bindings (pynlm module)
- Error handling and exceptions

## Troubleshooting

### Common Issues and Solutions

#### Issue 1: Import Error
**Problem**: `ImportError: No module named 'pynlm'`

**Solution**:
```bash
# Install NLM Python bindings
pip install pynlm

# Or install in development mode
pip install -e .
```

#### Issue 2: Brain Initialization Error
**Problem**: Brain fails to initialize or throws an exception.

**Solution**:
```python
import pynlm

# Create brain with default configuration
try:
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain initialized successfully!")
except Exception as e:
    print(f"Brain initialization failed: {e}")
```

#### Issue 3: Simulation Not Progressing
**Problem**: Simulation runs but doesn't seem to progress.

**Solution**:
```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Ensure all subsystems are enabled
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with verbose output
for step in range(10):
    world.update(0.1)
    percept = world.getSensoryPercept()
    print(f"Step {step}: Percept = {percept}")
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    print(f"  Action: {action}")
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Issue 4: Memory Issues
**Problem**: Simulation runs out of memory.

**Solution**:
```python
import pynlm

# Create brain with memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)  # Smaller brain
config.set("brain.synapse_density", 0.05)  # Lower synapse density

brain = pynlm.createBrain(config)
brain.initialize()

# Monitor memory usage
import psutil
import os

process = psutil.Process(os.getpid())
print(f"Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")

# Run simulation
for step in range(1000):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}: Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
```

## Getting Help

### 1. Documentation
- Read the comprehensive documentation in `docs/`
- Check the tutorial series in `tutorials/`
- Review API documentation in `docs/PYTHON_BINDINGS.md`

### 2. Examples
- Run the examples provided in this guide
- Explore the example scripts in the repository
- Look at the test files for usage patterns

### 3. Community
- Join the NLM community on GitHub
- Ask questions on Stack Overflow with `nlm` tag
- Report issues on the GitHub repository
- Check for updates and contributions

### 4. Support
- Check the troubleshooting guide in `docs/TROUBLESHOOTING.md`
- Consult the best practices guide in `docs/BEST_PRACTICES.md`
- Review the migration guide in `docs/MIGRATION_GUIDE.md`

## Next Steps

### 1. If You're New to NLM
- Read `README.md` for an overview of the project
- Try the quick start examples in this guide
- Read `easy_usage.md` for beginner-friendly guidance
- Start with `Tutorial 1: Basic Setup`

### 2. If You're Familiar with NLM
- Read `docs/COMPREHENSIVE_DEVELOPMENT_GUIDE.md` for advanced topics
- Try `Tutorial 3: Batch Processing` for performance optimization
- Explore `docs/PERFORMANCE.md` for profiling and optimization
- Experiment with advanced Python features

### 3. If You're an Advanced User
- Create your own tutorials and examples
- Contribute to the documentation
- Explore the batch processing and performance features
- Integrate NLM with your existing projects

## Welcome to NLM!

NLM is an exciting project that combines neuroscience, artificial intelligence, and software engineering. We're excited to have you join us on this journey!

Remember:
- Start simple and build up
- Read the documentation thoroughly
- Experiment and explore
- Ask questions when you're stuck
- Share your discoveries and contributions

Happy simulating!
