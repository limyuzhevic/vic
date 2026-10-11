# NLM Quick Start Guide

## Quick Start for New Users

This document provides a rapid introduction to NLM (Neural Learning Machine) for new users. It covers the essentials needed to get started with the system quickly and effectively.

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project that simulates neural systems with the goal of understanding how cognition can emerge from neural dynamics. Key characteristics:

- **NOT** a transformer or LLM
- **NOT** a chatbot or deep learning model
- **NOT** a pretrained AI system
- **IS** a brain-inspired architecture with neurons and synapses
- **IS** designed to develop and adapt over time through interaction with an environment
- **IS** a research platform for investigating computational brain principles

## System Overview

NLM implements a closed-loop brain system with the following architecture:

```
WORLD → SENSORY INPUT → NEURAL PROCESSING → INTERNAL STATE
          ↓                      ↓                ↓
        MEMORY / PREDICTION ← NEUROMODULATION ← ACTION SELECTION
                              ↓
                         MOTOR OUTPUT → WORLD
```

## Installation and Setup

### Prerequisites

1. **Install NLM**

```bash
# Clone the repository
cd /path/to

# Configure build options
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    -DNLM_ENABLE_TESTS=ON \
    -DNLM_ENABLE_PYTHON_BINDINGS=ON

# Build all components
make -C build -j$(nproc)

# Build Python bindings
pip install -e .
```

2. **Set up development environment**

```bash
# Create conda environment
conda create -n nlm python=3.8
conda activate nlm

# Install Python dependencies
pip install pybind11 scikit-build-core numpy pytest
```

### Verify Installation

```bash
# Test C++ installation
./build/bin/nlm_phase4_demo

# Test Python installation
python3 -c "import pynlm; print(pynlm.__version__)"
```

## Basic Usage

### 1. Creating a Brain

**Python API (Recommended for beginners):**

```python
import pynlm

# Create brain with default configuration
brain = pynlm.create_brain(pynlm.create_default_config())
brain.initialize()

print(f"Created brain with {brain.get_total_neuron_count()} neurons")
print(f"Total synapses: {brain.get_total_synapse_count()}")
```

**C++ API (For advanced users):**

```cpp
#include "nlm/brain/Brain.hpp"
#include "nlm/core/Config/Config.hpp"

using namespace nlm;

int main() {
    // Create configuration
    auto config = createConfiguration();
    
    // Add basic configuration
    auto builder = config->getBuilder();
    builder->addInt("nlm.brain.neuron_count", 1000, "Number of neurons")
           ->addDouble("nlm.brain.synapse_density", 0.1, "Synapse density");
    
    auto finalConfig = builder->build();
    
    // Create and initialize brain
    Brain brain(finalConfig);
    brain.initialize();
    
    std::cout << "Created brain with " 
              << brain.getTotalNeuronCount() << " neurons" << std::endl;
    
    return 0;
}
```

### 2. Running a Simulation

**Python Example:**

```python
import pynlm

# Create brain and world
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
brain.initialize()

agent = pynlm.create_agent_brain(brain)
world = pynlm.create_simple_world()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()

# Enable key systems
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
agent.enable_development(True)

# Run simulation loop
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory input from world
    percept = world.get_sensory_percept()
    
    # Process sensory input in brain
    agent.process_sensory_input(percept)
    
    # Run brain step
    brain.step(step)
    
    # Decode motor command from brain activity
    motor_cmd = agent.decode_motor_command()
    
    # Apply action to world
    world.apply_action(motor_cmd, world.get_simulation_time())
    
    # Print progress
    if step % 20 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
        print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
        print(f"  Novelty: {agent.get_novelty_level():.3f}")
        print(f"  Dev Stage: {brain.get_developmental_stage()}")
```

**C++ Example:**

```cpp
#include "nlm/brain/Brain.hpp"
#include "nlm/agent/AgentBrain.hpp"
#include "nlm/world/SimpleWorld.hpp"

using namespace nlm;

int main() {
    // Create configuration
    auto config = createConfiguration();
    
    // Create brain, agent, and world
    auto brain = createBrain(config);
    auto agent = createAgentBrain(brain);
    auto world = createSimpleWorld();
    
    // Configure world
    world->configure(20, 20, 8, 8);
    world->reset();
    
    // Enable subsystems
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    // Run simulation
    for (int step = 0; step < 100; ++step) {
        // Update world
        world->update(0.1f);
        
        // Process sensory input
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Run brain step
        brain->step(step);
        
        // Decode and apply action
        auto motorCmd = agent->decodeMotorCommand();
        world->applyAction(motorCmd, step * 0.1f);
        
        // Print progress
        if (step % 20 == 0) {
            std::cout << "Step " << step << ":" << std::endl;
            std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
            std::cout << "  Curiosity: " << agent->getCuriosityLevel() << std::endl;
            std::cout << "  Novelty: " << agent->getNoveltyLevel() << std::endl;
            std::cout << "  Dev Stage: " << brain->getDevelopmentalStage() << std::endl;
        }
    }
    
    return 0;
}
```

### 3. Advanced Configuration

**Create custom configuration with JSON/YAML:**

```python
import pynlm
import json

# Create configuration with custom parameters
config_dict = {
    "nlm": {
        "brain": {
            "neuron_count": 5000,
            "synapse_density": 0.05,
            "learning_rate": 0.001
        },
        "plasticity": {
            "stdp": {
                "enabled": True,
                "learning_rate": 0.001
            }
        }
    },
    "phase6": {
        "enable_checkpointing": True,
        "enable_replay": True,
        "enable_memory_systems": True
    }
}

# Create configuration from dictionary
config = pynlm.create_config_from_dict(config_dict)
brain = pynlm.create_brain(config)
brain.initialize()
```

**Save configuration to file:**

```cpp
#include "nlm/core/Config/Config.hpp"

auto config = createConfiguration();

// Add configuration
auto builder = config->getBuilder();
builder->addInt("nlm.brain.neuron_count", 2000, "Number of neurons");
builder->addDouble("nlm.brain.learning_rate", 0.001, "Learning rate");

auto finalConfig = builder->build();

// Save to file
finalConfig->saveToFile("my_config.json");
```

## Key Components and APIs

### Core Classes

#### Brain Class
```python
# Main brain interface
brain = pynlm.create_brain(config)
brain.initialize()
brain.step(step_number)           # Run one simulation step
brain.reset()                      # Reset brain state
brain.receive_sensory_input(input) # Inject sensory data
action = brain.produce_action()    # Get motor action

# Statistics
brain.get_total_neuron_count()      # Total neurons
brain.get_total_synapse_count()     # Total synapses
brain.get_firing_neuron_count()     # Currently firing
brain.get_average_firing_rate()     # Average firing rate
```

#### Config Class
```python
# Configuration management
config = pynlm.create_default_config()
config.set("brain.neuron_count", 1000)
config.get_or("brain.neuron_count", 1000)
config.has("brain.neuron_count")
config.load_from_file("config.json")
config.save_to_file("config.json")
```

#### AgentBrain Class
```python
# Agent-brain interface
agent = pynlm.create_agent_brain(brain)
agent.initialize(world)
agent.process_sensory_input(percept)
motor_cmd = agent.decode_motor_command()
agent.apply_reward_modulation(reward, predicted_reward)
```

#### SimpleWorld Class
```python
# Environment interface
world = pynlm.create_simple_world()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
world.update(timestep)

percept = world.get_sensory_percept()
world.apply_action(motor_cmd, current_time)
```

### Enumerations

```python
# Neuron types
pynlm.NeuronType.EXCITATORY
pynlm.NeuronType.INHIBITORY
pynlm.NeuronType.SENSORY
pynlm.NeuronType.MOTOR

# Developmental stages
pynlm.DevelopmentalStage.INITIAL
pynlm.DevelopmentalStage.CRITICAL_PERIOD
pynlm.DevelopmentalStage.MATURATION
pynlm.DevelopmentalStage.ADULT

# Action types
pynlm.ActionType.MOVE_FORWARD
pynlm.ActionType.TURN_LEFT
pynlm.ActionType.INTERACT
pynlm.ActionType.EAT
```

## Example Scripts

### Simple Neural Network

```python
import pynlm

def run_simple_simulation():
    """Run a simple neural network simulation."""
    # Create brain
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Run simulation
    for i in range(100):
        brain.step(i)
    
    print("Simulation complete!")
    print(f"Total spikes: {brain.get_total_spike_count()}")

if __name__ == "__main__":
    run_simple_simulation()
```

### Environment Interaction

```python
import pynlm

def run_agent_simulation():
    """Run a complete agent simulation with environment interaction."""
    # Setup
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    agent = pynlm.create_agent_brain(brain)
    world = pynlm.create_simple_world()
    world.configure(width=10, height=10, vision_width=8, vision_height=8)
    world.reset()
    
    agent.initialize(world)
    
    # Run episode
    for step in range(500):
        world.update(0.1)
        agent.process_sensory_input(world.get_sensory_percept())
        brain.step(step)
        world.apply_action(agent.decode_motor_command(), world.get_simulation_time())
    
    print("Agent simulation complete!")

if __name__ == "__main__":
    run_agent_simulation()
```

### Memory System Integration

```python
import pynlm

def run_memory_integration():
    """Demonstrate memory system integration."""
    # Enable memory systems
    config = pynlm.create_default_config()
    config.set("phase6.enable_working_memory", True)
    config.set("phase6.enable_episodic_memory", True)
    
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Store experiences over time
    for step in range(100):
        # Create sensory input
        input_data = {
            "vision": [0.1, 0.2, 0.3, 0.4, 0.5],
            "touch": [0.0, 0.5, 0.0],
            "internal": [0.8, 0.1]
        }
        
        # Convert to neural format
        sensory_input = pynlm.create_sensory_input(input_data)
        
        # Store in working memory
        if brain.get_working_memory():
            brain.get_working_memory().store_experience(step, sensory_input)
        
        # Store in episodic memory
        if brain.get_episodic_memory():
            brain.get_episodic_memory().store_experience(step, sensory_input)
        
        # Process in brain
        brain.step(step)
    
    # Test memory retrieval
    if brain.get_working_memory():
        experience = brain.get_working_memory().retrieve_experience(50)
        print(f"Retrieved experience from step 50")
    
    if brain.get_episodic_memory():
        episode = brain.get_episodic_memory().recall_episode(25)
        print(f"Recalled episode around step 25")
    
    print("Memory integration complete!")

if __name__ == "__main__":
    run_memory_integration()
```

## Troubleshooting

### Common Issues and Solutions

#### ImportError: No module named 'pynlm'

**Problem**: Python package not installed.

**Solution**:
```bash
pip install -e .
```

#### ModuleNotFoundError: No module named 'nlm'

**Problem**: C++ headers not found.

**Solution**:
```bash
# Build with Python bindings
cmake -S . -B build -DPYTHON_EXECUTABLE=$(which python3)
make -C build -j$(nproc)
```

#### Segmentation fault in C++ executable

**Problem**: Invalid configuration or missing initialization.

**Solution**:
```bash
# Check for memory debugging
valgrind ./build/bin/nlm_phase4_demo

# Use debug build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
make -C build
./build/bin/nlm_phase4_demo
```

#### Configuration file not found

**Problem**: Configuration file path incorrect.

**Solution**:
```cpp
// Use absolute paths or relative to current directory
config->loadFromFile("./config.json");
config->loadFromFile("../configs/default.cfg");
```

### Getting Help

#### Online Resources

- **Documentation**: https://github.com/your-repo/nlm/docs
- **API Reference**: Generated during build
- **Examples**: `examples/` directory
- **Tutorials**: Available in documentation

#### Community Support

- **GitHub Issues**: Report bugs and request features
- **Discussions**: Community questions and discussions
- **Contributing Guide**: How to contribute to the project

## Advanced Usage

### Performance Optimization

```python
# Enable performance optimizations
config.set("phase6.enable_optimization", True)
config.set("performance.enable_vectorization", True)
config.set("performance.enable_parallel", True)

# Run performance benchmark
brain.run_performance_benchmark(neuron_count=10000, steps=1000)
```

### Debugging

```python
# Enable debugging
import os
os.environ["NLM_DEBUG"] = "1"

# Set log level
brain.set_log_level("DEBUG")

# Enable verbose output
brain.enable_verbose_logging(True)
```

### Extension Development

```cpp
// Extend NLM with custom components
#include "nlm/core/Component.hpp"

class CustomNeuron : public NeuralInterface {
public:
    void step(double timestep) override {
        // Custom neuron dynamics
    }
    
    void connect(NeuralInterface* other) override {
        // Custom connectivity
    }
};

// Register custom component
void registerCustomComponent() {
    NeuralFactory::registerComponent("CustomNeuron", []() {
        return std::make_unique<CustomNeuron>();
    });
}
```

## Quick Start Summary

### For Researchers

1. Clone the repository
2. Build with CMake
3. Run example simulations
4. Explore documentation

### For Developers

1. Set up development environment
2. Explore API documentation
3. Write tests and examples
4. Contribute to development

### For Users

1. Install NLM
2. Run example scripts
3. Customize configurations
4. Explore interactive demos

## Further Reading

### Documentation

- **Architecture.md**: System architecture and design
- **Configuration.md**: Configuration system usage
- **ErrorHandling.md**: Error handling and robustness
- **Performance.md**: Performance optimization guide
- **Phase6Guide.md**: Phase 6 integration guide
- **Development.md**: Development and contribution guidelines

### Code Examples

- **examples/simple/**: Simple neural network examples
- **examples/agent/**: Agent-environment interaction examples
- **examples/memory/**: Memory system integration examples
- **examples/performance/**: Performance optimization examples

### Scientific Background

- **SCIENCE.md**: Scientific background and biological inspiration
- **EXPERIMENTS.md**: Experiment descriptions and results
- **LIMITATIONS.md**: Current limitations and future directions

## Conclusion

NLM provides a powerful platform for experimenting with neural computation and cognitive emergence. This quick start guide provides the essentials to begin working with the system. For more detailed information, refer to the comprehensive documentation available in the `docs/` directory.

The NLM project continues to evolve, and new features are regularly added. Keep the documentation updated and explore the examples to fully leverage the capabilities of this experimental neural system.