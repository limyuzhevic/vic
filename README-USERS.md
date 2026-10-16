# NLM User Guide

## Overview

This guide provides comprehensive information for users of the NLM (熙然) neural learning machine system. It covers installation, basic usage, configuration options, and getting started with the system.

## Quick Start

### 1. Installation

#### Linux/macOS
```bash
# Clone the repository
git clone https://github.com/neural-learning-machine/nlm.git
cd nlm

# Build
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

#### Windows
```bash
# Clone the repository
git clone https://github.com/neural-learning-machine/nlm.git
cd nlm

# Build using Visual Studio
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019"
cmake --build . --config Release
```

### 2. Running the System

#### Basic Usage
```bash
# Run Phase 6 demo (integration test)
cd build
./nlm_phase6_demo
```

#### Running Examples
```bash
# Build examples
make examples

# Run basic brain simulation
./examples/ex01_basic_brain

# Run memory systems example
./examples/ex02_memory_systems

# Run full integration example
./examples/ex07_integration
```

### 3. Configuration

#### Basic Configuration File
Create `config.json`:
```json
{
    "brain": {
        "maxNeurons": 1000,
        "learningRate": 0.01,
        "plasticityEnabled": true,
        "memory": {
            "workingMemory": {
                "enabled": true,
                "capacity": 500,
                "decayRate": 0.01
            },
            "episodicMemory": {
                "enabled": true,
                "maxEpisodes": 1000
            }
        }
    },
    "simulation": {
        "maxSteps": 10000,
        "timestep": 0.001
    }
}
```

#### Command Line Configuration
```bash
# Run with custom configuration
./nlm_simulation --config my_config.json

# Override specific settings
./nlm_simulation --brain.maxNeurons 2000 --brain.learningRate 0.005

# Enable specific features
./nlm_simulation --memory.workingMemory.enabled --neuromodulation.dopamine.enabled
```

## Basic Usage

### 1. Understanding NLM

NLM (熙然) is an experimental artificial developmental brain system that simulates brain-like computation through:

- **Spiking Neural Networks**: Event-driven neural computation
- **Developmental Processes**: Age-dependent learning and maturation
- **Multiple Memory Systems**: Working, episodic, and associative memory
- **Neuromodulation**: Reward-based and curiosity-driven learning
- **Predictive Processing**: Expectation-based inference and learning

### 2. Core Components

#### Brain System
The brain is the central coordinator of neural processing:

- **Neurons**: Basic spiking units with LIF dynamics
- **Regions**: Functional brain areas with connectivity
- **Populations**: Groups of neurons with shared properties
- **Synapses**: Communication channels between neurons

#### Memory Systems
Integrated memory systems for different types of information:

- **Working Memory**: Active, transient information storage
- **Episodic Memory**: Experience encoding and replay
- **Associative Memory**: Pattern associations and recall

#### Neuromodulation Systems
Systems that regulate motivation and learning:

- **Dopamine**: Reward prediction and reinforcement
- **Curiosity**: Exploration motivation and novelty detection
- **Novelty**: Sensory novelty detection for exploration

#### Agent Interface
Connects NLM to external environments:

- **AgentBrain**: Sensory transduction and motor decoding
- **SimpleWorld**: Basic world simulation interface

### 3. Running Your First Simulation

#### Basic Brain Simulation

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>

int main() {
    // Initialize logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::INFO);
    
    std::cout << "=== NLM Phase 6 Basic Simulation ===" << std::endl;
    
    // Create configuration
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
    
    std::cout << "Brain initialized successfully!" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": " 
                      << brain->getActiveNeuronCount() << " active neurons, "
                      << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
        }
    }
    
    std::cout << "=== Simulation Complete ===" << std::endl;
    return 0;
}
```

#### Memory Systems Simulation

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>
#include <vector>

int main() {
    // Configure with memory systems
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 500);
    config->set("brain.memory.workingMemory.enabled", true);
    config->set("brain.memory.episodicMemory.enabled", true);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Access memory systems
    auto workingMemory = brain->getWorkingMemory();
    auto episodicMemory = brain->getEpisodicMemory();
    
    // Store and retrieve experiences
    std::vector<float> experience = {0.1f, 0.5f, 0.8f, 0.3f, 0.7f};
    workingMemory->store(experience);
    
    auto retrieved = workingMemory->retrieve();
    std::cout << "Retrieved pattern size: " << retrieved.size() << std::endl;
    
    std::cout << "Memory systems simulation completed!" << std::endl;
    return 0;
}
```

#### Neuromodulation Simulation

```cpp
#include <nlm/Brain.hpp>
#include <nlm/Config.hpp>
#include <nlm/Logger.hpp>
#include <iostream>

int main() {
    // Configure neuromodulation
    auto config = std::make_shared<nlm::Config>();
    config->set("brain.maxNeurons", 300);
    config->set("brain.neuromodulation.dopamine.enabled", true);
    config->set("brain.neuromodulation.curiosity.enabled", true);
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Get neuromodulation systems
    auto dopamine = brain->getDopamine();
    auto curiosity = brain->getCuriosity();
    
    // Simulate reward-based learning
    float reward = 1.0f;
    float predictionError = 0.3f;
    
    // Apply reward modulation
    auto agent = std::make_shared<nlm::AgentBrain>(brain);
    agent->applyRewardModulation(reward, predictionError);
    
    std::cout << "Neuromodulation simulation completed!" << std::endl;
    std::cout << "Dopamine level: " << dopamine->getLevel() << std::endl;
    std::cout << "Curiosity level: " << curiosity->getLevel() << std::endl;
    
    return 0;
}
```

## Configuration Options

### 3.1 Brain Configuration

#### Core Settings
- **brain.maxNeurons**: Number of neurons in the brain
- **brain.learningRate**: Learning rate for plasticity
- **brain.regionCount**: Number of brain regions
- **brain.connectionProbability**: Probability of synaptic connections

#### Memory System Settings
- **brain.memory.workingMemory.enabled**: Enable working memory
- **brain.memory.workingMemory.capacity**: Working memory capacity
- **brain.memory.episodicMemory.enabled**: Enable episodic memory
- **brain.memory.episodicMemory.maxEpisodes**: Maximum episodic memory entries
- **brain.memory.associativeMemory.enabled**: Enable associative memory

#### Neuromodulation Settings
- **brain.neuromodulation.dopamine.enabled**: Enable dopamine system
- **brain.neuromodulation.dopamine.baselineLevel**: Baseline dopamine level
- **brain.neuromodulation.curiosity.enabled**: Enable curiosity system
- **brain.neuromodulation.novelty.enabled**: Enable novelty detection

#### Development Settings
- **brain.development.enabled**: Enable developmental processes
- **brain.development.plasticityDecayRate**: Rate of plasticity decay
- **brain.development.maturationRate**: Rate of neuronal maturation

### 3.2 Simulation Settings

- **simulation.maxSteps**: Maximum simulation steps
- **simulation.timestep**: Time step duration
- **simulation.checkpointInterval**: Interval for saving checkpoints

## Examples

### 4.1 Example Files

The NLM distribution includes several example files in the `examples/` directory:

#### ex01_basic_brain.cpp
Basic brain initialization and simulation.

#### ex02_memory_systems.cpp
Working with memory systems (working, episodic, associative).

#### ex03_neuromodulation.cpp
Neuromodulation and learning systems.

#### ex04_development.cpp
Brain development and aging processes.

#### ex05_prediction.cpp
Prediction system and error computation.

#### ex06_cognition.cpp
Cognitive mechanisms and planning.

#### ex07_integration.cpp
Complete system integration.

#### ex08_advanced_usage.cpp
Advanced usage patterns.

#### ex09_visualization.cpp
Visualization of brain activity.

#### ex10_performance.cpp
Performance testing and benchmarking.

### 4.2 Running Examples

```bash
# Build examples
make examples

# Run specific example
./examples/ex01_basic_brain
./examples/ex02_memory_systems
./examples/ex03_neuromodulation
# ... and so on
```

## Documentation

### 5.1 Documentation Resources

- **Quick Start**: This document (README.md)
- **Architecture**: docs/ARCHITECTURE.md
- **API Reference**: docs/API_REFERENCE.md
- **Tutorials**: docs/TUTORIALS.md
- **Best Practices**: docs/BEST_PRACTICES.md
- **Performance Tuning**: docs/PERFORMANCE_TUNING.md
- **Troubleshooting**: docs/TROUBLESHOOTING.md

### 5.2 Online Resources

- **Documentation**: https://docs.neural-learning-machine.org
- **Tutorials**: https://tutorials.neural-learning-machine.org
- **API Reference**: https://api.neural-learning-machine.org
- **Examples**: https://examples.neural-learning-machine.org
- **Community**: https://forum.neural-learning-machine.org

## Support

### 6.1 Getting Help

#### Community Support
- **GitHub Issues**: Report bugs and ask questions
- **Community Forum**: Discuss with other users
- **Documentation**: Browse comprehensive documentation

#### Technical Support
- **Email**: support@neural-learning-machine.org
- **GitHub**: Submit issues on GitHub
- **Slack/Discord**: Join the NLM community

### 6.2 Reporting Issues

#### Bug Reports
```bash
# Create a bug report
./nlm_simulation --bug-report

# Or submit on GitHub
# https://github.com/neural-learning-machine/nlm/issues
```

#### Feature Requests
```bash
# Request new features
./nlm_simulation --feature-request "Add X functionality"

# Or submit on GitHub
# https://github.com/neural-learning-machine/nlm/issues/new?template=feature_request.md
```

## Troubleshooting

### 7.1 Common Issues

#### Issue 1: "Failed to initialize brain"

**Symptoms:**
- Brain initialization returns false
- Error messages about configuration

**Solutions:**
```bash
# 1. Check configuration file
cat config.json | grep -E "(maxNeurons|learningRate)"

# 2. Validate configuration
./validate_config config.json

# 3. Check system resources
free -h
ulimit -a

# 4. Use default configuration
./nlm_simulation --use-defaults
```

#### Issue 2: "Out of memory"

**Symptoms:**
- Memory allocation failures
- System swapping
- Slow performance

**Solutions:**
```bash
# Reduce brain size
./nlm_simulation --brain.maxNeurons 500

# Disable memory-heavy features
./nlm_simulation --memory.episodicMemory.disabled

# Use memory-efficient configuration
./nlm_simulation --config memory_efficient.json
```

#### Issue 3: "Simulation step timeout"

**Symptoms:**
- Steps taking too long
- Real-time deadline violations

**Solutions:**
```bash
# Reduce network size
./nlm_simulation --brain.maxNeurons 1000

# Optimize configuration
./nlm_simulation --brain.learningRate 0.001

# Use performance-optimized build
./nlm_simulation --build optimized
```

### 7.2 Debug Logging

Enable detailed logging for debugging:
```cpp
// In your code
#include <nlm/Logger.hpp>

int main() {
    // Enable debug logging
    nlm::Logger::getInstance().setLevel(nlm::LogLevel::DEBUG);
    
    // Enable specific components
    nlm::Logger::getInstance().enableComponent(nlm::LogComponent::Brain);
    nlm::Logger::getInstance().enableComponent(nlm::LogComponent::Neuromodulation);
    
    // ... rest of program
}
```

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Contact

For questions, support, or collaboration:

- **Website**: https://neural-learning-machine.org
- **Documentation**: https://docs.neural-learning-machine.org
- **Community**: https://forum.neural-learning-machine.org
- **GitHub**: https://github.com/neural-learning-machine/nlm