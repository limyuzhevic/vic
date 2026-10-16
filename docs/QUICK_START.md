# NLM Quick Start Guide

## Overview

This guide provides rapid introduction to the NLM (熙然) neural learning machine system. It covers installation, basic setup, and getting started with your first neural simulation in minutes.

## Prerequisites

### System Requirements

- **Operating Systems**: Linux, macOS, Windows
- **Compiler**: C++17 compatible (GCC 9+, Clang 8+, MSVC 2019+)
- **Build System**: CMake 3.15+
- **Python**: 3.7+ (for documentation generation)

### Recommended Hardware
- **Minimum**: 2-core processor, 4GB RAM
- **Recommended**: 8-core processor, 16GB RAM
- **High-Performance**: Many-core processor, 64GB+ RAM or GPU

## Quick Installation

### Step 1: Clone Repository
```bash
# Clone the NLM repository
git clone https://github.com/neural-learning-machine/nlm.git
cd nlm
```

### Step 2: Configure Build
```bash
# Create build directory
mkdir build
cd build

# Configure with default settings (Release build)
cmake .. -DCMAKE_BUILD_TYPE=Release
```

### Step 3: Build
```bash
# Build with parallel jobs
make -j$(nproc)

# Or use Ninja for faster builds
# cmake .. -G "Ninja"
# ninja
```

### Step 4: Install Examples
```bash
# Build examples for demonstration
make examples
```

## Your First NLM Simulation

### Method 1: Command Line Interface
```bash
# Run Phase 6 integration demo
cd build
./nlm_phase6_demo
```

### Method 2: C++ Programming
Create a file called `first_brain.cpp`:

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>

int main() {
    // Initialize logging for debugging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== NLM Phase 6 First Brain ===" << std::endl;
    
    // Create and configure brain
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 100);
    config->set("brain.learningRate", 0.01);
    config->set("brain.simulation.maxSteps", 1000);
    
    // Create and initialize brain
    auto brain = std::make_shared<nlm::Brain>(config);
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "✓ Brain initialized successfully!" << std::endl;
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    
    // Run simulation loop
    std::cout << "\nRunning simulation..." << std::endl;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 20 == 0) {
            std::cout << "  Step " << step << ": "
                     << brain->getActiveNeuronCount() << " active, "
                     << brain->getFiringNeuronCount() << " firing, "
                     << brain->getAverageFiringRate() << " Hz" << std::endl;
        }
    }
    
    // Save brain state
    brain->save("first_brain_checkpoint.brain");
    std::cout << "\n✓ Brain state saved to 'first_brain_checkpoint.brain'" << std::endl;
    
    std::cout << "\n=== Simulation Complete ===" << std::endl;
    return 0;
}
```

Compile and run:
```bash
# Compile
cd build
g++ -std=c++17 first_brain.cpp -o first_brain -lnlm

# Run
./first_brain
```

### Method 3: Using Existing Examples
```bash
# Run basic brain simulation
cd build
./examples/ex01_basic_brain

# Run memory systems example
./examples/ex02_memory_systems

# Run full integration example
./examples/ex07_integration
```

## Understanding NLM Components

### Core Brain Systems

1. **Neural Core**: Processing engine with neurons and synapses
2. **Memory Systems**: Working, episodic, and associative memory
3. **Neuromodulation**: Reward, curiosity, and novelty systems
4. **Prediction**: Expectation-based learning
5. **Cognition**: Planning and decision-making
6. **Development**: Age-dependent plasticity

### Key Configuration Parameters

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| `brain.maxNeurons` | Number of neurons | 1000 | 100-100000 |
| `brain.learningRate` | Plasticity learning rate | 0.01 | 0.001-0.1 |
| `brain.regionCount` | Number of brain regions | 3 | 1-10 |
| `brain.connectionProbability` | Synaptic connection probability | 0.1 | 0.01-0.5 |

### Common Configuration Templates

#### Basic Configuration
```json
{
    "brain": {
        "maxNeurons": 500,
        "learningRate": 0.01,
        "plasticityEnabled": true
    },
    "simulation": {
        "maxSteps": 10000,
        "timestep": 0.001
    }
}
```

#### Advanced Configuration
```json
{
    "brain": {
        "maxNeurons": 2000,
        "learningRate": 0.005,
        "plasticityEnabled": true,
        "memory": {
            "workingMemory": {
                "enabled": true,
                "capacity": 1000,
                "decayRate": 0.01
            },
            "episodicMemory": {
                "enabled": true,
                "maxEpisodes": 5000,
                "replayWindow": 1000
            }
        },
        "neuromodulation": {
            "dopamine": {
                "enabled": true,
                "baselineLevel": 0.1
            },
            "curiosity": {
                "enabled": true,
                "baselineLevel": 0.1
            }
        },
        "development": {
            "enabled": true,
            "plasticityDecayRate": 0.02
        }
    }
}
```

## Running Advanced Examples

### 1. Memory Systems Example
```cpp
// ex02_memory_systems.cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>
#include <vector>

int main() {
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 500);
    config->set("brain.memory.workingMemory.enabled", true);
    config->set("brain.memory.episodicMemory.enabled", true);
    config->set("brain.memory.associativeMemory.enabled", true);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Demonstrate memory systems
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    auto associativeMemory = brain->getAssociativeMemory();
    
    // Store and retrieve patterns
    std::vector<float> experience = {0.1f, 0.5f, 0.8f, 0.3f, 0.7f};
    workingMemory->store(experience);
    
    auto retrieved = workingMemory->retrieve();
    
    std::cout << "Memory systems demonstration complete!" << std::endl;
    return 0;
}
```

### 2. Neuromodulation Example
```cpp
// ex03_neuromodulation.cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>

int main() {
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 300);
    config->set("brain.neuromodulation.dopamine.enabled", true);
    config->set("brain.neuromodulation.curiosity.enabled", true);
    config->set("brain.neuromodulation.novelty.enabled", true);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Access neuromodulation systems
    auto dopamine = brain->getDopamine();
    auto curiosity = brain->getCuriosity();
    auto novelty = brain->getNovelty();
    
    // Simulate reward learning
    float reward = 1.0f;
    float predictionError = 0.3f;
    
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->applyRewardModulation(reward, predictionError);
    
    std::cout << "Neuromodulation demonstration complete!" << std::endl;
    std::cout << "Dopamine level: " << dopamine->getLevel() << std::endl;
    std::cout << "Curiosity level: " << curiosity->getLevel() << std::endl;
    
    return 0;
}
```

## Learning Path

### For Complete Beginners
1. **Start with Command Line**: `./nlm_phase6_demo` - No coding required
2. **Explore Examples**: `./examples/ex01_basic_brain` - Understand the structure
3. **Try Basic Simulation**: Your first C++ program with NLM

### For Intermediate Users
1. **Experiment with Configuration**: Modify JSON configs
2. **Build Your Own Examples**: Create custom simulations
3. **Access Memory Systems**: Work with working, episodic, and associative memory

### For Advanced Users
1. **Neuromodulation Research**: Study reward and curiosity systems
2. **Developmental Studies**: Explore age-dependent plasticity
3. **Performance Optimization**: Fine-tune for large-scale simulations

## Troubleshooting

### Common Issues

#### Issue 1: "Failed to initialize brain"
**Symptoms**: Brain initialization returns false, configuration errors
**Solutions**:
```bash
# 1. Validate configuration
./nlm_simulation --validate-config config.json

# 2. Check system resources
free -h
ulimit -a

# 3. Use simplified configuration
./nlm_simulation --use-defaults
```

#### Issue 2: "Out of memory"
**Symptoms**: Memory allocation failures, slow performance
**Solutions**:
```bash
# Reduce brain size
./nlm_simulation --brain.maxNeurons 500

# Disable memory-heavy features
./nlm_simulation --memory.episodicMemory.disabled

# Use memory-efficient build
./nlm_simulation --build memory-efficient
```

#### Issue 3: "Simulation step timeout"
**Symptoms**: Steps taking too long, real-time deadline violations
**Solutions**:
```bash
# Reduce network size
./nlm_simulation --brain.maxNeurons 1000

# Optimize configuration
./nlm_simulation --brain.learningRate 0.001

# Use performance-optimized build
./nlm_simulation --build optimized
```

## Next Steps

1. **Read Documentation**: Explore `docs/` for comprehensive guides
2. **Join Community**: Connect with other NLM users and researchers
3. **Contribute**: Help improve documentation and examples
4. **Research**: Use NLM for neural learning machine research

## Resources

### Online Resources
- **Documentation**: https://docs.neural-learning-machine.org
- **API Reference**: https://api.neural-learning-machine.org
- **Tutorials**: https://tutorials.neural-learning-machine.org
- **Examples**: https://examples.neural-learning-machine.org
- **Community**: https://forum.neural-learning-machine.org

### Local Resources
- **Documentation**: `docs/` directory in repository
- **Examples**: `examples/` directory
- **Source Code**: `src/` directory
- **Tests**: `tests/` directory

## Conclusion

You've successfully set up and run your first NLM neural simulation! The NLM system provides a powerful platform for studying brain-like computation through experimental artificial developmental brain systems. The integration of multiple cognitive systems enables emergent behaviors from simple neural components.

**Key Takeaways**:
- NLM uses spiking neural networks with LIF dynamics
- Multiple memory, neuromodulation, and developmental systems are integrated
- Configuration-based control allows flexible experimentation
- Phase 6 represents complete system integration

**Next Milestones**:
1. Master configuration management
2. Explore memory systems integration
3. Study neuromodulation effects
4. Investigate developmental trajectories

Happy neural simulation! 🎯