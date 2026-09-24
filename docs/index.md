# NLM Documentation

Welcome to the Neural Learning Machine (NLM) documentation. This project implements a biologically-inspired artificial neural network with real spiking neural computation, learning, and development.

## Overview

NLM (熙然) is a modular computational brain system designed to implement real neural computation with:

- **Leaky Integrate-and-Fire (LIF) neuron dynamics**
- **Event-driven spike propagation with synaptic delays**
- **Spike-Timing-Dependent Plasticity (STDP) and Hebbian learning**
- **Structural plasticity (synaptogenesis/pruning)**
- **Neuromodulation and developmental systems**
- **Working memory, episodic memory, and prediction systems**

## Quick Links

- [API Reference](api/README.md) - Complete class and method documentation
- [Architecture](architecture.md) - System design and component relationships
- [Algorithms](algorithms/README.md) - Mathematical formulations and learning rules
- [Getting Started](usage/getting-started.md) - Tutorial and examples
- [Configuration](config.md) - Configuration options and parameters
- [Development](development.md) - Development practices and migration guides

## Key Documentation Sections

### 1. API Reference
The complete API reference is organized by module:

- **Brain System**: [Neuron](api/brain/Neuron.md), [Synapse](api/brain/Synapse.md), [NeuralRegion](api/brain/NeuralRegion.md), [NeuralPopulation](api/brain/NeuralPopulation.md), [Brain](api/brain/Brain.md)
- **Neuromodulation**: [Neuromodulator](api/neuromodulation/Neuromodulator.md), [Dopamine](api/neuromodulation/Dopamine.md), [Curiosity](api/neuromodulation/Curiosity.md), [Novelty](api/neuromodulation/Novelty.md), [PredictionError](api/neuromodulation/PredictionError.md), [Reward](api/neuromodulation/Reward.md)
- **Agent Interface**: [AgentBrain](api/agent/AgentBrain.md), [AgentBody](api/agent/AgentBody.md)
- **Core Systems**: [Config](api/core/Config.md), [Random](api/core/Random.md), [Logger](api/core/Logger.md), [SimulationClock](api/core/SimulationClock.md)

### 2. Architecture Documentation
- [System Architecture](architecture.md) - Component relationships and dependencies
- [Phase Evolution](architecture/phases.md) - How the system evolved through phases
- [Data Flow Diagrams](architecture/data-flow.md) - Information flow between systems

### 3. Algorithm Documentation
- [LIF Dynamics](algorithms/LIF.md) - Mathematical formulation of neuron dynamics
- [STDP](algorithms/STDP.md) - Spike-timing dependent plasticity
- [Hebbian Learning](algorithms/Hebbian.md) - Activity-dependent plasticity
- [Neuromodulation](algorithms/Neuromodulation.md) - Reward and error signal processing
- [Memory Systems](algorithms/Memory.md) - Working memory and episodic memory

### 4. Usage Guides
- [Getting Started](usage/getting-started.md) - Installation and basic usage
- [Simulation Examples](usage/examples.md) - Common scenarios and demonstrations
- [Performance Tuning](usage/performance.md) - Optimization strategies
- [Advanced Usage](usage/advanced.md) - Complex configurations and patterns

### 5. Configuration Reference
- [Configuration Overview](config.md) - All configuration parameters
- [Default Configuration](config/default.md) - Default values and sources
- [Configuration Examples](config/examples.md) - Common configuration setups

### 6. Development Guides
- [Development Practices](development/practices.md) - Coding standards and patterns
- [Migration Guides](development/migration.md) - Upgrading between versions
- [Adding Components](development/adding-components.md) - How to extend NLM

### 7. Troubleshooting
- [Common Issues](troubleshooting/common.md) - Frequently encountered problems
- [Performance Troubleshooting](troubleshooting/performance.md) - Performance optimization
- [Debugging](troubleshooting/debugging.md) - Debugging techniques and tools

## Magic Numbers and Constants

This section documents all magic numbers and constants found in the codebase:

### AgentBrain.cpp (Source: /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_aa49398a-8a6c-4f35-9149-c77e0feef6ea/src/agent/AgentBrain.cpp)

- **Vision scaling**: `5.0f` - Converts vision percept values to current injection (lines 95, 113)
- **Touch scaling**: `8.0f` - Converts touch collision signals to current injection (line 104)
- **Internal signals**: `2.0f, 5.0f` - Centers and scales internal signal range (lines 113, 147)
- **Proprioception**: `2.0f, 3.0f` - Centers and scales proprioception signals (lines 122, 147)
- **Plasticity factor**: `0.5f + 0.5f * level` - Dopamine-modulated plasticity (line 271)

### Neuron.cpp (Source: /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_aa49398a-8a6c-4f35-9149-c77e0feef6ea/src/brain/Neuron.cpp)

- **Membrane capacitance**: `1.0f` nF - Neuronal membrane capacitance (line 22)
- **Time constant**: `20.0f` ms - Membrane time constant (line 23)
- **Max spike history**: `100` - Maximum recorded spikes per neuron (line 24)
- **Temperature scaling**: `1000.0f` - Converts timestep from seconds to milliseconds (line 223)

### Main.cpp (Source: /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_aa49398a-8a6c-4f35-9149-c77e0feef6ea/src/main.cpp)

- **Default timestep**: `0.001` - Simulation timestep in seconds (lines 371, 396)
- **Default neurons**: `500` - Number of neurons for testing (line 372)
- **Default regions**: `1` - Number of brain regions (line 373)
- **Default connection probability**: `0.15f` - Probability of forming synapses (line 374)
- **STDP weights**: `0.02f`, `0.015f` - Long-term potentiation and depression weights (lines 377-378)
- **STDP tau**: `20.0f` - STDP time constant (line 379)

### Neuromodulator.cpp (Source: /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_aa49398a-8a6c-4f35-9149-c77e0feef6ea/src/neuromodulation/Neuromodulator.cpp)

- **Plasticity factor**: `0.5f + 0.5f * level` - Dopamine-modulated plasticity (line 35)
- **Dopamine peak**: `1.0f` - Maximum dopamine level (line 13)
- **Decay rate**: `0.1f` - Natural dopamine decay rate (line 13)

## System Architecture

NLM is organized into a modular, layered architecture with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────────┐
│                        NLM Framework                            │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐          │
│  │   Core      │───▶│   Brain     │───▶│  Integration│          │
│  │  Services   │    │  Substrates │    │  Layer      │          │
│  └─────────────┘    └─────────────┘    └─────────────┘          │
│         │                      │                             │
│    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐       │
│    │   Sensory   │    │   Memory    │    │  Prediction │       │
│    │   Systems   │    │  Systems    │    │   Systems   │       │
│    └─────────────┘    └─────────────┘    └─────────────┘       │
│              │                      │                      │
│        ┌─────────────┐        ┌─────────────┐              │
│        │  Neuromod.  │        │  Development│              │
│        │  Systems    │        │   Systems   │              │
│        └─────────────┘        └─────────────┘              │
│                                                          │
│  ┌───────────────────────────────────────────────────────┐  │
│  │                    Phase 4 Cognition                  │  │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐   │  │
│  │  │Attention│  │Planning │  │Self-Model│  │Social   │   │  │
│  │  │         │  │         │  │         │  │Learning │   │  │
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘   │  │
│  └───────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

## Usage Examples

### Basic Brain Simulation
```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"

int main() {
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", 42);
    config->set("neuron_count", 500);
    
    // Initialize brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    return 0;
}
```

### Learning Experiment
```cpp
#include "agent/AgentBrain.hpp"
#include "environment/SimpleWorld.hpp"

void runLearningExperiment() {
    // Setup brain and world
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    SimpleWorld world(20, 20);
    AgentBrain agent(brain);
    agent.initialize(world);
    
    // Run learning episodes
    for (int episode = 0; episode < 100; ++episode) {
        // Reset agent
        agent.reset();
        
        // Episode loop
        while (!world.isCompleted()) {
            // Get sensory percept
            SensoryPercept percept = world.getSensoryPercept();
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Run brain step
            brain->step(episode * 1000);
            
            // Decode motor command
            MotorCommand command = agent.decodeMotorCommand();
            
            // Apply to world
            world.applyMotorCommand(command);
            
            // Get reward
            float reward = world.getReward();
            
            // Apply reward modulation
            agent.applyRewardModulation(reward, agent.getPredictionError());
        }
    }
}
```

## Quick Start

### 1. Installation

The NLM project uses CMake for build system. To build and install:

```bash
# Clone the repository
cd /path/to/nlm

# Configure with CMake
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. -DCMAKE_CXX_STANDARD=17

# Build
make -j$(nproc)

# Install (optional)
sudo make install
```

### 2. Running Examples

Several example programs are included:

```bash
# Basic neural computation test
./build/src/main

# Example with command-line arguments
./build/src/main --config my_config.cfg

# Run experiments
./build/src/experiments/ExperimentRunner
```

### 3. Configuration

NLM uses a flexible configuration system. Configuration files are YAML/INI format:

```yaml
# configs/default.cfg
random_seed: 42
simulation_timestep: 0.001
neuron_count: 500
region_count: 1
connection_probability: 0.15

stdp:
  ltp_weight: 0.02
  ltd_weight: 0.015
  tau: 20.0

structural_plasticity:
  synaptogenesis_rate: 0.0001
  pruning_rate: 0.00001
```

### 4. Documentation Structure

The complete documentation is organized as follows:

- **API Reference**: Complete method documentation with parameters and return values
- **Architecture**: System design and component relationships
- **Algorithms**: Mathematical formulations and learning rules
- **Usage**: Getting started and practical examples
- **Configuration**: All configuration parameters and their meanings
- **Development**: How to extend and modify NLM
- **Troubleshooting**: Common issues and solutions

## References

- [NLM Research Papers](docs/RESEARCH_METHODS.md)
- [Phase Evolution](docs/PHASE6_ARCHITECTURE.md)
- [Performance Characteristics](docs/PERFORMANCE.md)
- [Limitations and Future Work](docs/LIMITATIONS.md)