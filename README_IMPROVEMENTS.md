# NLM Documentation

## Overview

This is the NLM (熙然, "serene flow") project - an experimental artificial developmental brain system. This documentation provides an overview of the codebase structure and improvements made.

## Current Phase

Phase 2: Real Neural Computation - Implements real spiking neural computation with:
- Leaky Integrate-and-Fire (LIF) neurons
- Event-driven spike propagation with synaptic delays
- STDP and Hebbian plasticity
- Structural plasticity (synaptogenesis/pruning)

## Codebase Structure

### Core Components

#### 1. Brain System (`src/brain/`)
- `Brain.hpp` - Central brain coordinator
- `Brain.cpp` - Brain implementation
- `Neuron.hpp` - LIF neuron implementation
- `Neuron.cpp` - Neuron dynamics and plasticity
- `Synapse.hpp` - Synapse model with STDP
- `Synapse.cpp` - Synaptic dynamics and plasticity
- `NeuralRegion.hpp` - Brain region manager
- `NeuralRegion.cpp` - Region implementation
- `NeuralPopulation.hpp` - Neural population manager

#### 2. Plasticity System (`src/plasticity/`)
- `STDP.hpp` - Spike-Timing-Dependent Plasticity
- `Hebbian.hpp` - Hebbian learning rule
- `PlasticityRule.hpp` - Base plasticity rule

#### 3. Memory Systems (`src/memory/`)
- `Memory.hpp` - Base memory system
- `NeuralWorkingMemory.hpp` - Working memory
- `NeuralEpisodicMemory.hpp` - Episodic memory
- `NeuralAssociativeMemory.hpp` - Associative memory

#### 4. Neuromodulation (`src/neuromodulation/`)
- `Dopamine.hpp` - Reward and reinforcement
- `Curiosity.hpp` - Exploration motivation
- `Novelty.hpp` - Novelty detection
- `PredictionError.hpp` - Prediction error signaling

#### 5. Prediction System (`src/prediction/`)
- `PredictionSystem.hpp` - Prediction and error computation

#### 6. Cognition Systems (`src/cognition/`)
- `NeuralPlanner.hpp` - Action sequence evaluation
- `ConceptFormation.hpp` - Pattern discovery
- `Attention.hpp` - Competitive selection

#### 7. Development (`src/development/`)
- `DevelopmentSystem.hpp` - Developmental system
- `Synaptogenesis.hpp` - Synapse formation
- `Pruning.hpp` - Synapse elimination
- `Maturation.hpp` - Neural maturation

#### 8. Sensory and Motor (`src/sensory/`, `src/motor/`)
- Vision, Audio, Internal signals
- Action selection

#### 9. Core Utilities (`src/core/`)
- Configuration management
- Random number generation
- Logging system
- Type definitions

#### 10. Environment (`src/environment/`)
- World simulation
- Observation management

#### 11. Experiments (`src/experiments/`)
- Phase 2 demo
- Experiment framework
- Metrics collection

## Key Improvements Made

### 1. Enhanced Error Handling
- Added comprehensive logging to config file loading
- Improved error messages for invalid configuration values
- Added validation for population IDs in NeuralRegion
- Enhanced error reporting throughout the codebase

### 2. Performance Optimizations
- Optimized LIF neuron dynamics by pre-calculating inverse constants
- Reduced redundant calculations in membrane potential updates
- Improved adaptation variable decay calculations
- Optimized synaptic input handling

### 3. Better Code Organization
- Extracted LearningExperiment class from main.cpp for better reusability
- Improved separation of concerns in main.cpp
- Added forward declarations for better compilation times
- Better encapsulation of internal implementation details

### 4. Enhanced Type Safety
- Added ConfigKey class for better configuration management
- Improved type safety in configuration system
- Better parameter validation in multiple components

### 5. Improved Documentation
- Added comprehensive documentation to key headers
- Improved comments throughout the codebase
- Added clear explanations of complex algorithms
- Documented the purpose and usage of major components

## Building and Running

### Prerequisites
- CMake 3.10 or higher
- C++17 compatible compiler

### Build Instructions
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running
```bash
./nlm_phase2_demo
```

## Configuration

The system uses a simple key=value configuration file format:
- `configs/default.cfg` - Default configuration
- Command-line arguments override configuration file values
- Runtime configuration can be modified programmatically

## Learning and Usage

### Understanding the Brain
1. Study the LIF neuron implementation in Neuron.cpp
2. Understand STDP and Hebbian plasticity rules
3. Explore the neuromodulation systems (dopamine, curiosity, novelty)
4. Review the prediction system and cognitive architectures

### Contributing
1. Add new plasticity rules in the plasticity directory
2. Implement new neuromodulators in neuromodulation/
3. Extend memory systems in memory/
4. Improve sensory and motor systems
5. Add new experiment types in experiments/

## Future Directions

Phase 2 successfully implemented the core neural computation. Future phases will:
- Implement real JSON/YAML configuration parsing
- Add advanced visualization capabilities
- Implement reward prediction error learning
- Extend structural plasticity mechanisms
- Add social learning capabilities
- Implement more sophisticated cognitive architectures

## License

MIT License - See LICENSE file for details

## Authors

Research project - See docs/ for scientific background
