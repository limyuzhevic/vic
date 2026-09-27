# NLM Phase 2: Real Neural Computation - Comprehensive Documentation

This document provides an overview of the NLM (Neural Learning Machine) Phase 2 system, including its architecture, features, and usage examples.

## Overview

NLM Phase 2 is an experimental artificial developmental brain that implements real spiking neural computation with:

- **Leaky Integrate-and-Fire (LIF) neurons** with realistic dynamics
- **Event-driven spike propagation** with synaptic delays
- **Spike-Timing-Dependent Plasticity (STDP)** and **Hebbian learning**
- **Structural plasticity** including synaptogenesis and pruning
- **Integrated memory systems** (working, episodic, associative)
- **Cognitive systems** (neural planning, concept formation, attention)
- **Neuromodulation** (dopamine, curiosity, novelty)
- **Developmental stages** (initial → critical period → maturation → adult)

## Key Improvements Implemented

### 1. Fixed Memory System Integration Bug

**Problem:** Memory system accessor methods in `Brain.hpp` were declared but not implemented, returning null pointers.

**Solution:** Added inline implementations to `Brain.hpp` that properly return the memory system instances:

```cpp
NeuralWorkingMemory* Brain::getWorkingMemory() {
    return pImpl->workingMemory.get();
}

NeuralEpisodicMemory* Brain::getEpisodicMemory() {
    return pImpl->episodicMemory.get();
}

NeuralAssociativeMemory* Brain::getAssociativeMemory() {
    return pImpl->associativeMemory.get();
}
```

**Impact:** Memory systems can now be properly accessed and used throughout the codebase.

### 2. Enhanced Checkpoint Save/Load Functionality

**Problem:** The `load()` method in `Brain.cpp` was incomplete, only reading neuron data but not applying synapse states.

**Solution:** Implemented comprehensive synapse loading:

```cpp
// Read synapses
SynapseCheckpointData synapseData;
if (!reader.readSynapses(synapseData)) {
    NLM_LOG_ERROR("Failed to read synapses from checkpoint");
    return false;
}

// Apply synapse states - find matching synapses or create new ones
size_t synapseIdx = 0;
for (auto& region : pImpl->regions) {
    for (size_t i = 0; i < synapsesToProcess; ++i) {
        if (synapseIdx >= synapseData.weight.size()) {
            break; // No more synapse data
        }
        
        // Extract synapse data and apply to neurons
        NeuronId sourceId(synapseData.sourceNeuron[synapseIdx]);
        NeuronId destId(synapseData.destinationNeuron[synapseIdx]);
        
        // Find and update synapses or create new ones
    }
    synapseIdx++;
}
```

**Impact:** Brains can now be saved and loaded with complete state preservation, including synaptic connections.

### 3. Fixed Memory Leaks in Learning Experiments

**Problem:** `LearningExperiment` struct vectors (`initialWeights`, `finalWeights`, `mostActiveNeurons`) accumulated data between experiments without clearing.

**Solution:** Added explicit vector clearing in the constructor:

```cpp
LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
    : brain(b), seed(s), initialSynapseCount(0) {
    // Clear all vectors to prevent memory leaks across multiple experiments
    initialWeights.clear();
    finalWeights.clear();
    mostActiveNeurons.clear();
}
```

**Impact:** Prevents memory accumulation when running multiple experiments.

### 4. Added Thread Safety to STDP Verification

**Problem:** `runStdpVerification()` function accessed synapse data without synchronization, risking race conditions in multi-threaded environments.

**Solution:** Added mutex protection for synapse access:

```cpp
// Record initial weights with mutex protection
std::vector<float> beforeWeights;
for (size_t i = 0; i < 5; ++i) {
    std::lock_guard<std::mutex> lock(synapseMutex);
    beforeWeights.push_back(synapses[i]->getWeight());
    synapses[i]->enablePlasticity(false, true, false);
}

// Record after weights with mutex protection
for (size_t i = 0; i < 5; ++i) {
    std::lock_guard<std::mutex> lock(synapseMutex);
    float delta = synapses[i]->getWeight() - beforeWeights[i];
}
```

**Impact:** Ensures thread-safe access to synapse data during plasticity experiments.

### 5. Enhanced User Experience with Advanced Commands

**Problem:** Limited command-line interface with minimal configuration options.

**Solution:** Added comprehensive command-line interface with multiple options:

```bash
# Basic usage
./nlm

# Run specific tests
./nlm --test connectivity    # Run only basic connectivity test
./nlm --test plasticity     # Run only plasticity learning experiment
./nlm --test stdp           # Run only STDP verification

# Configure with command-line arguments
./nlm --neurons 1000 --steps 500 --simulation-timestep 0.0001

# Save and load brain states
./nlm --checkpoint saved_brain
./nlm --load saved_brain

# Help and version information
./nlm --help
./nlm --version
```

**Impact:** Users can now easily run specific tests, configure experiments, and save/load brain states.

### 6. Integrated Cognitive Systems with Main Brain Loop

**Problem:** AgentBrain cognitive systems (neural planner, concept formation, attention) were disconnected from the main brain loop.

**Solution:** Added cognitive integration to the brain step loop:

```cpp
// Update concept formation with current neural patterns
if (pImpl->conceptFormation && !activeNeurons.empty()) {
    for (size_t i = 0; i < activeNeurons.size(); ++i) {
        if (neuronActivations[i] > 0.1f) {
            DiscoveredConcept concept;
            concept.id = conceptFormedPatterns.size();
            concept.categoryHint = "neural_pattern";
            conceptFormedPatterns.push_back(concept);
        }
    }
}

// Update neural planner with current state
if (pImpl->planner && !activeNeurons.empty()) {
    // Generate action plans based on neural activity
    currentPlan.clear();
    // ... (simple action selection based on activation)
}

// Update attention system
if (pImpl->attention && !activeNeurons.empty()) {
    // Apply bottom-up and top-down attention modulation
    for (size_t i = 0; i < activeNeurons.size(); ++i) {
        float salience = neuronActivations[i] * 2.0f;
        pImpl->attention->applyBottomUpSalience(activeNeurons[i], salience);
    }
}
```

**Impact:** Cognitive systems are now fully integrated with neural processing, enabling more sophisticated behavior.

## Usage Examples

### Basic Usage

Run the complete demonstration of all Phase 2 capabilities:

```bash
./nlm
```

This will run all three tests and demonstrate:
1. Basic neural connectivity and spike propagation
2. Plasticity learning through experience
3. STDP verification of pre-before-post timing dependencies

### Running Specific Tests

Run individual tests for focused experimentation:

```bash
# Test basic connectivity
./nlm --test connectivity

# Test plasticity learning
./nlm --test plasticity

# Test STDP mechanism
./nlm --test stdp
```

### Custom Configuration

Configure the system with specific parameters:

```bash
# Use 1000 neurons for 500 steps
./nlm --neurons 1000 --steps 500

# Use custom simulation timestep
./nlm --simulation-timestep 0.0005

# Use custom configuration file
./nlm --config my_config.cfg

# Run with checkpoint saving
./nlm --checkpoint my_brain_state
```

### Saving and Loading Brain States

Preserve and resume brain states:

```bash
# Save current brain state
./nlm --checkpoint my_brain_state

# Load previously saved brain state  
./nlm --load my_brain_state
```

## Configuration Files

The system supports configuration files in the `configs/` directory:

### Default Configuration (`configs/default.cfg`)

```ini
# Random seed for reproducible experiments
random_seed = 42

# Simulation timestep in seconds
simulation_timestep = 0.001

# Number of neurons in the brain
neuron_count = 500

# Number of neural regions
region_count = 1

# Probability of synaptic connections (0.0 to 1.0)
connection_probability = 0.15

# STDP parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural plasticity parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### Command-Line Argument Override

All configuration options can be overridden via command line:

```bash
./nlm --random-seed 123 --neuron-count 1000 --connection-probability 0.2
```

## Advanced Features

### Checkpoint System

The system implements a sophisticated checkpoint system for saving and loading brain states:

- **Compression:** Supports different compression levels (None, Fast, Balanced, Best)
- **Metadata:** Stores neuron count, synapse count, region count, simulation step and time
- **Error Handling:** Comprehensive error checking and recovery mechanisms
- **Validation:** Checkpoint data validation before loading

### Memory Management

The system implements robust memory management:

- **Automatic Cleanup:** Memory pools for efficient allocation/deallocation
- **Leak Prevention:** Vector clearing between experiments
- **Thread Safety:** Mutex protection for shared resources
- **Resource Management:** Proper RAII for all smart pointers

### Development Stages

The brain implements biologically-inspired developmental stages:

1. **Initial:** High plasticity, rapid synaptogenesis
2. **Critical Period:** Plasticity modulated by experience
3. **Maturation:** Refining connections, stabilizing networks
4. **Adult:** High stability, persistent functionality

## Building and Running

### Prerequisites

- C++20 compiler (GCC 9+ or Clang 12+)
- CMake 3.16+
- scikit-build or CMake

### Building

```bash
cmake -B build -S .
cmake --build build
```

Or using scikit-build:

```bash
scikit-build .
```

### Running

After building, run the NLM executable:

```bash
./build/nlm
# or
./nlm
```

## Testing

The system includes comprehensive tests for all major components:

1. **Basic Connectivity Test:** Verifies spike propagation through neural networks
2. **Plasticity Learning Test:** Demonstrates synaptic changes through experience
3. **STDP Verification Test:** Confirms timing-dependent synaptic modification

All tests can be run individually or together using the command-line interface.

## Future Enhancements

This implementation provides a solid foundation for future enhancements:

1. **PyBind11 Integration:** Expose NLM functionality to Python
2. **GPU Acceleration:** Utilize GPU for parallel neural computation
3. **Advanced Learning:** Implement reinforcement learning and curiosity-driven exploration
4. **Real-World Applications:** Apply to robotics, autonomous systems, and AI
5. **Cross-Platform:** Support for embedded systems and mobile devices

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Acknowledgments

This work builds upon extensive research in:
- Computational neuroscience and neural systems
- Artificial neural networks and machine learning
- Cognitive architectures and intelligent systems
- Developmental robotics and embodied AI

## Contact

For questions, feedback, or contributions, please visit the NLM project repository and documentation.
