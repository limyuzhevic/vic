# NLM Tutorial: Getting Started with the Neural Learning Machine

## Overview

Welcome to the NLM (熙然) tutorial! This guide will help you get started with NLM, covering everything from installation to running your first experiment.

**Prerequisites:**
- Basic understanding of C++ programming
- Interest in neural computation and artificial brains
- Access to a Linux/macOS environment (Windows with WSL)

**Time Investment:** 30-60 minutes for basic setup and first experiment

## Table of Contents

1. [Installation and Setup](#1-installation-and-setup)
2. [Understanding NLM Architecture](#2-understanding-nlm-architecture)
3. [Running Basic Experiments](#3-running-basic-experiments)
4. [Exploring NLM Features](#4-exploring-nlm-features)
5. [Writing Custom Experiments](#5-writing-custom-experiments)
6. [Troubleshooting](#6-troubleshooting)

## 1. Installation and Setup

### 1.1 Clone NLM Repository

First, clone the NLM repository to your local machine:

```bash
git clone https://github.com/your-username/nlm.git
cd nlm
```

### 1.2 Build NLM

NLM uses CMake for building. Follow these steps:

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

**If you need specific options:**

```bash
# Debug build
cmake .. -DCMAKE_BUILD_TYPE=Debug

# With CUDA support (if available)
cmake .. -DCMAKE_ENABLE_CUDA=ON

# Custom installation prefix
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local/nlm
```

### 1.3 Verify Installation

After building, verify that NLM works:

```bash
# Test the basic demo
./nlm

# Test Phase 3 demo (world interaction)
./nlm_phase3_demo

# Test Phase 4 demo (cognition)
./nlm_phase4_demo
```

If you encounter any issues, check the [troubleshooting section](#6-troubleshooting).

## 2. Understanding NLM Architecture

### 2.1 Core Components

NLM is organized into several key layers:

#### The Brain (Central Controller)

The Brain is the main component that coordinates all neural operations. Think of it as the central processing unit of an artificial brain.

#### Neural Systems

1. **Sensory System:** Processes input from the environment
2. **Motor System:** Converts neural activity into actions
3. **Memory Systems:** Working memory, episodic memory, associative memory
4. **Prediction System:** Predicts future states and actions
5. **Neuromodulation System:** Reward, curiosity, novelty detection
6. **Cognition System:** Attention, planning, concept formation
7. **Development System:** Neural maturation and adaptation

#### Key Features

- **Spiking Neural Dynamics:** Uses realistic integrate-and-fire neurons
- **Event-Driven Processing:** Efficient spike-based computation
- **Plasticity:** STDP, Hebbian learning, structural changes
- **Working Memory:** Persistent activity for temporary information
- **Replay:** Memory consolidation through replay
- **Development:** Stage-based neural maturation

### 2.2 Data Flow

```
ENVIRONMENT
    ↓
SENSORY INPUT (Vision, Audio, Proprioception)
    ↓
NLM BRAIN
    ├── Neural Processing (LIF dynamics, spikes)
    ├── Memory Systems (Working, Episodic, Associative)
    ├── Prediction System
    ├── Neuromodulation (Reward, Curiosity)
    └── Action Selection
    ↓
MOTORS → ENVIRONMENT
    ↓
REWARD / ERROR / SURPRISE
    ↓
NEUROMODULATION → PLASTICITY
    ↓
BRAIN CHANGES → FUTURE BEHAVIOR
```

### 2.3 Running Modes

#### Phase 1: Core Neural Computation
Focuses on basic neural dynamics and plasticity:
- Integrate-and-fire neurons
- Spike timing-dependent plasticity (STDP)
- Event-driven spike propagation
- Structural plasticity (synaptogenesis/pruning)

#### Phase 2: World Interaction
Adds environmental interaction:
- Sensory input and motor output
- Reward prediction error
- Developmental stages
- Novelty and curiosity

#### Phase 3: Cognitive Emergence
Introduces higher-level cognition:
- Neural prediction system
- Working memory
- Episodic memory
- Concept formation
- Attention (competitive, not Transformer)
- Predictive planning
- Self-model

#### Phase 4: Advanced Cognition
Full cognitive capabilities:
- Social learning
- Continual learning
- Complex decision making

#### Phase 5: Performance Optimizations
- Memory pools and event queues
- SIMD vectorization
- Parallel processing
- Checkpoint system

#### Phase 6: Integration
- All systems integrated into coherent brain
- Comprehensive integration experiment

## 3. Running Basic Experiments

### 3.1 Simple Demo (Phase 1)

Run the basic neural computation demo:

```bash
# Simple neuron dynamics demo
./nlm --simulation.max_steps 1000 --brain.neuron_count 100
```

This runs a basic experiment with 100 neurons performing spiking computations.

### 3.2 World Interaction Demo (Phase 3)

Run the world interaction demo:

```bash
# Basic world interaction
./nlm_phase3_demo --world.simple --steps 500

# With sensory and motor components
./nlm_phase3_demo --world.simple --sensory.enabled --motor.enabled --steps 1000
```

### 3.3 Cognitive Demo (Phase 4)

Run the cognitive emergence demo:

```bash
# Basic cognitive demo
./nlm_phase4_demo --cognition.enabled --memory.enabled --prediction.enabled

# With attention and planning
./nlm_phase4_demo --cognition.attention.enabled --cognition.planning.enabled
```

### 3.4 Integration Demo (Phase 6)

Run the comprehensive integration demo:

```bash
# Full integration test
./nlm_phase6_demo --integration.test --validate.all

# With detailed logging
./nlm_phase6_demo --log.level info --log.file integration.log
```

## 4. Exploring NLM Features

### 4.1 Neural Dynamics Exploration

Explore different neuron dynamics:

```bash
# LIF (Leaky Integrate-and-Fire) dynamics
./nlm --brain.dynamics LIF --simulation.max_steps 2000

# Hodgkin-Huxley dynamics (if available)
./nlm --brain.dynamics HH --simulation.max_steps 2000

# FitzHugh-Nagumo dynamics (if available)
./nlm --brain.dynamics FHN --simulation.max_steps 2000
```

### 4.2 Plasticity Exploration

Explore different learning rules:

```bash
# STDP (Spike-Timing-Dependent Plasticity)
./nlm --brain.plasticity STDP --simulation.max_steps 2000

# Hebbian learning
./nlm --brain.plasticity Hebbian --simulation.max_steps 2000

# Reward-modulated plasticity
./nlm --brain.plasticity RewardModulated --simulation.max_steps 2000
```

### 4.3 Memory Exploration

Explore different memory systems:

```bash
# Working memory demo
./nlm --memory.working.enabled --simulation.max_steps 2000

# Episodic memory demo
./nlm --memory.episodic.enabled --simulation.max_steps 2000

# Associative memory demo
./nlm --memory.associative.enabled --simulation.max_steps 2000
```

### 4.4 Neuromodulation Exploration

Explore different neuromodulators:

```bash
# Dopamine (reward) system
./nlm --neuromodulation.dopamine.enabled --simulation.max_steps 2000

# Curiosity system
./nlm --neuromodulation.curiosity.enabled --simulation.max_steps 2000

# Novelty system
./nlm --neuromodulation.novelty.enabled --simulation.max_steps 2000
```

### 4.5 Development Exploration

Explore developmental processes:

```bash
# Basic development
./nlm --development.enabled --development.stages 3 --simulation.max_steps 3000

# Advanced development with plasticity decay
./nlm --development.enabled --development.stages 5 \
      --development.plasticity_decay_rate 0.95 \
      --simulation.max_steps 5000
```

## 5. Writing Custom Experiments

### 5.1 Basic Experiment Template

Create a custom experiment:

```cpp
#include <nlm/brain/Brain.hpp>
#include <nlm/core/Config.hpp>
#include <nlm/environment/SimpleWorld.hpp>
#include <nlm/agent/AgentBrain.hpp>

class MyExperiment {
public:
    MyExperiment() {
        // Initialize configuration
        config_ = std::make_shared<nlm::Config>();
        config_->loadFromFile("config.json");
        
        // Create brain
        brain_ = std::make_shared<nlm::Brain>(config_);
        brain_->initialize();
        
        // Create world
        world_ = std::make_shared<nlm::SimpleWorld>(config_);
        
        // Create agent
        agent_ = std::make_shared<nlm::AgentBrain>(brain_, world_);
    }
    
    void run(int steps) {
        for (nlm::SimulationStep step = 0; step < steps; ++step) {
            // Get percept from world
            auto percept = world_->getPercept();
            
            // Process sensory input
            agent_->processSensoryInput(percept);
            
            // Step the brain
            brain_->step(step);
            
            // Get motor command
            auto action = agent_->getMotorCommand();
            
            // Apply action to world
            world_->applyAction(action);
            
            // Log progress
            if (step % 100 == 0) {
                brain_->logStatus();
            }
        }
    }
    
private:
    std::shared_ptr<nlm::Config> config_;
    std::shared_ptr<nlm::Brain> brain_;
    std::shared_ptr<nlm::SimpleWorld> world_;
    std::shared_ptr<nlm::AgentBrain> agent_;
};

int main() {
    MyExperiment experiment;
    experiment.run(10000);
    return 0;
}
```

### 5.2 Configuration File for Experiment

Create a configuration file for your experiment:

```json
{
  "nlm": {
    "simulation": {
      "max_steps": 10000,
      "dt": 0.001,
      "time_unit": "ms"
    },
    "brain": {
      "neuron_count": 500,
      "region_count": 8,
      "plasticity_enabled": true,
      "development_enabled": true,
      "prediction_enabled": true
    },
    "memory": {
      "working_memory_capacity": 100,
      "working_memory_decay_rate": 0.01,
      "episodic_memory_enabled": true,
      "episodic_memory_capacity": 2000
    },
    "sensory": {
      "vision_enabled": true,
      "audio_enabled": true,
      "feature_dimensions": 64
    },
    "motor": {
      "action_types": ["move", "grasp", "rotate"],
      "exploration_rate": 0.1
    },
    "neuromodulation": {
      "dopamine_enabled": true,
      "curiosity_enabled": true,
      "novelty_enabled": true,
      "prediction_error_enabled": true
    },
    "development": {
      "stages": 4,
      "plasticity_decay_rate": 0.97
    }
  }
}
```

### 5.3 Python Bindings Example

Use NLM with Python bindings:

```python
import nlm

# Create configuration
config = nlm.Config()
config.set("nlm.simulation.max_steps", 5000)
config.set("nlm.brain.neuron_count", 200)
config.set("nlm.memory.working_memory_capacity", 50)

# Create brain
brain = nlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    # Get sensory input
    percept = get_sensor_input()  # Your sensor function
    brain.receiveSensoryInput(percept)
    
    # Step brain
    brain.step(step)
    
    # Get motor output
    action = brain.produceAction()
    apply_motor_action(action)  # Your motor function
    
    if step % 100 == 0:
        brain.logStatus()
```

## 6. Troubleshooting

### 6.1 Common Issues

#### Build Errors

**Issue:** CMake configuration errors
**Solution:**
```bash
# Clean and rebuild
cd build
rm -rf *
cmake ..
make -j4
```

**Issue:** Missing dependencies
**Solution:** Install required packages:
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential cmake libomp-dev

# macOS
brew install cmake
```

#### Runtime Errors

**Issue:** Configuration file not found
**Solution:** Use absolute path or place config file in working directory

**Issue:** Permission denied
**Solution:** Check file permissions and run with appropriate privileges

#### Performance Issues

**Issue:** Slow simulation
**Solution:**
- Increase optimization level: `-DCMAKE_BUILD_TYPE=Release`
- Use fewer neurons: `--brain.neuron_count 100`
- Reduce simulation steps
- Enable SIMD: `-DCMAKE_ENABLE_SSE=ON`

### 6.2 Debugging

#### Enable Debug Logging

```bash
./nlm --log.level debug --log.file debug.log
```

#### Memory Debugging

```bash
./nlm --memory.debug.enabled --simulation.max_steps 100
```

#### Configuration Debugging

```bash
./nlm --config.debug.enabled --config.validate.all
```

### 6.3 Getting Help

#### Online Resources

- **Documentation:** https://nlm.ai/docs
- **GitHub Repository:** https://github.com/your-username/nlm
- **Issues:** https://github.com/your-username/nlm/issues

#### Local Documentation

```bash
# Show help information
cd build
./nlm --help

# Show version information
./nlm --version
```

#### Community Support

Join the NLM community:

- **Discord Server:** [https://discord.gg/nlm](https://discord.gg/nlm)
- **Forum:** [https://forum.nlm.ai](https://forum.nlm.ai)
- **Twitter:** [@NLM_Research](https://twitter.com/NLM_Research)

## 7. Next Steps

### 7.1 Advanced Topics

- **Understanding Neural Dynamics:** Study the LIF equation and neuron models
- **Plasticity Rules:** Explore STDP, Hebbian, and reward-modulated learning
- **Memory Systems:** Investigate working memory persistence and replay
- **Neuromodulation:** Understand dopamine, curiosity, and novelty
- **Development:** Learn about stage-based neural maturation

### 7.2 Research Applications

- **Cognitive Modeling:** Model human cognitive processes
- **AI Neuroscience:** Test hypotheses about brain function
- **Robot Learning:** Create learning robots with neural brains
- **Computational Neuroscience:** Study neural algorithms

### 7.3 Contributing

- **Report Issues:** Found a bug? Report it on GitHub
- **Contribute Code:** Add new features or fix existing issues
- **Write Documentation:** Improve tutorials and examples
- **Run Experiments:** Share your experimental results

## 8. Conclusion

You now have a complete introduction to NLM! This tutorial covered:

1. **Installation and Setup:** Building NLM on your system
2. **Architecture Understanding:** How NLM works as an artificial brain
3. **Basic Experiments:** Running different types of demos
4. **Feature Exploration:** Trying out different neural components
5. **Custom Experiments:** Writing your own experiments

### Key Takeaways

- NLM is a modular, biologically-inspired neural system
- It supports multiple learning mechanisms and memory systems
- It's designed for research and experimentation
- It's extensible and community-driven

### Recommended Next Steps

1. **Run the integration demo:** `./nlm_phase6_demo`
2. **Explore the configuration options:** Check `docs/CONFIGURATION.md`
3. **Read the architecture guide:** Review `docs/ARCHITECTURE.md`
4. **Start your own experiment:** Create a custom experiment
5. **Join the community:** Connect with other NLM users

Enjoy exploring NLM and building your own neural systems!

---

*This tutorial is part of the NLM documentation suite. For more advanced topics, check the API reference and other documentation files.*
