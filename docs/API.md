# NLM - C++ API Documentation

**An Experimental Artificial Developmental Brain**

This document describes the C++ API for NLM (熙然), a neural simulation framework for artificial brains that learn from experience.

## Overview

NLM implements a complete brain system with:
- **Real spiking neural dynamics** (LIF neurons, event-driven computation)
- **Integrated memory systems** (working memory, episodic memory, associative memory)
- **Neuromodulation** (dopamine, curiosity, novelty detection)
- **Cognitive systems** (neural planning, concept formation, attention)
- **Developmental processes** (structural plasticity, maturation)
- **Learning mechanisms** (STDP, Hebbian plasticity)

## Basic Usage Examples

### Creating and Running a Brain

```cpp
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    // Create configuration
    auto config = std::make_shared<nlm::Config>();
    
    // Set configuration values
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("simulation_timestep", 0.001);
    
    // Create brain with configuration
    auto brain = std::make_shared<nlm::Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        std::cerr << "Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    // Run simulation steps
    for (size_t step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    
    // Check results
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    
    return 0;
}
```

### Running Specific Experiments

```cpp
#include "main.cpp"  // Contains experiment runner functions

int main(int argc, char** argv) {
    // Parse command line arguments
    std::string experiment = "all";  // "basic", "plasticity", "stdp", or "all"
    size_t steps = 1000;
    
    // Create and configure brain
    auto config = std::make_shared<nlm::Config>();
    // ... configure brain ...
    
    auto brain = std::make_shared<nlm::Brain>(config);
    brain->initialize();
    
    // Run specific experiment
    runExperiment(brain, experiment, steps);
    
    return 0;
}
```

### Using Configuration Files

#### Simple Format (.cfg, .txt)
```
# NLM Configuration File
neuron_count = 500
region_count = 1
connection_probability = 0.1
simulation_timestep = 0.001
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
enable_hebbian = true
enable_stdp = true
```

#### JSON Format (.json, .cfg)
```json
{
    "neuron_count": 500,
    "region_count": 1,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "enable_hebbian": true,
    "enable_stdp": true
}
```

#### Command Line Arguments
```bash
./nlm --experiment plasticity --steps 2000 --neurons 1000
./nlm --config my_config.cfg --experiment stdp
./nlm --neuron_count 2000 --region_count 3 --help
```

## Core API Classes

### nlm::Brain
The main brain class that coordinates all neural systems.

**Constructor**
```cpp
Brain(std::shared_ptr<Config> config);
```

**Methods**
- `bool initialize()` - Initialize brain with neurons, regions, and systems
- `void step(SimulationStep step)` - Perform one simulation step
- `void step(SimulationStep step, Timestamp currentTime)` - Perform step with specific time
- `void receiveSensoryInput(const SensoryInput& input)` - Inject sensory input
- `void injectCurrent(NeuronId neuron, MembranePotential current)` - Inject current into specific neuron
- `void injectCurrentToNeurons(NeuronType type, MembranePotential current)` - Inject into all neurons of type

**Access Methods**
- `SpikeSystem* getSpikeSystem()` - Access spike system
- `STDP* getSTDP()` - Access STDP plasticity
- `Hebbian* getHebbian()` - Access Hebbian plasticity
- `StructuralPlasticity* getStructuralPlasticity()` - Access structural plasticity
- `size_t getTotalNeuronCount() const` - Get total neuron count
- `size_t getTotalSynapseCount() const` - Get total synapse count
- `size_t getTotalSpikeCount() const` - Get total spike count
- `float getExcitationInhibitionRatio() const` - Get E/I ratio
- `void logStatus() const` - Log brain status and statistics

**Memory System Access**
- `NeuralWorkingMemory* getWorkingMemory()` - Access working memory
- `NeuralEpisodicMemory* getEpisodicMemory()` - Access episodic memory
- `NeuralAssociativeMemory* getAssociativeMemory()` - Access associative memory

**Cognition System Access**
- `NeuralPlanner* getPlanner()` - Access neural planner
- `ConceptFormation* getConceptFormation()` - Access concept formation
- `AttentionalSelection* getAttention()` - Access attention system

**Neuromodulation Access**
- `Dopamine* getDopamine()` - Access dopamine system
- `Curiosity* getCuriosity()` - Access curiosity system
- `Novelty* getNovelty()` - Access novelty detection
- `PredictionError* getPredictionErrorSignal()` - Access prediction error

### nlm::Config
Configuration management with JSON/YAML and simple file support.

**Methods**
- `Config()` - Default constructor
- `bool loadFromFile(const std::string& filepath)` - Load from file (auto-detects format)
- `bool loadFromArgs(int argc, char** argv)` - Load from command line
- `bool saveToFile(const std::string& filepath) const` - Save to file
- `template<typename T> std::optional<T> get(const std::string& key) const` - Get typed value
- `template<typename T> T getOr(const std::string& key, const T& defaultValue) const` - Get with default
- `void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime)` - Set value
- `void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime)` - Set string
- `bool has(const std::string& key) const` - Check if key exists
- `std::vector<std::string> getKeys() const` - Get all keys
- `std::string summary() const` - Get configuration summary

### nlm::Neuron
Individual neuron implementing LIF dynamics.

**Methods**
- `bool stepLIF(Timestamp currentTime, TimestepDuration dt)` - LIF step, returns true if fired
- `void recordSpike(Timestamp timestamp)` - Record spike for plasticity
- `const std::vector<Timestamp>& getSpikeHistory() const` - Get spike history
- `void receiveExcitatoryInput(MembranePotential amplitude)` - Receive excitatory input
- `void receiveInhibitoryInput(MembranePotential amplitude)` - Receive inhibitory input
- `void injectCurrent(MembranePotential current)` - Inject external current
- `void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)` - Enable plasticity rules

### nlm::Synapse
Synaptic connection with transmission delays and plasticity.

**Methods**
- `void recordPreSpike(Timestamp timestamp)` - Record pre-synaptic spike
- `void recordPostSpike(Timestamp timestamp)` - Record post-synaptic spike
- `float getEligibilityTrace() const` - Get reward eligibility trace
- `void decayEligibilityTrace(float decayRate)` - Decay eligibility trace

## Experiments

### Basic Connectivity Test
```cpp
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
Tests basic neural connectivity by injecting current into sensory neurons.
```

### Plasticity Learning Experiment
```cpp
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
Demonstrates measurable synaptic changes through experience with Hebbian and STDP.
```

### STDP Verification
```cpp
void runStdpVerification(std::shared_ptr<Brain> brain);
Verifies STDP rule by creating correlated pre->post activity to trigger potentiation.
```

## Configuration Parameters

### Core Parameters
- `neuron_count` - Number of neurons (default: 1000)
- `region_count` - Number of neural regions (default: 1)
- `connection_probability` - Probability of synaptic connections (default: 0.1)
- `simulation_timestep` - Simulation timestep in seconds (default: 0.001)

### STDP Parameters
- `stdp_ltp_weight` - STDP LTP weight (default: 0.02)
- `stdp_ltd_weight` - STDP LTD weight (default: 0.015)
- `stdp_tau` - STDP time constant (default: 20.0)

### Plasticity Parameters
- `synaptogenesis_rate` - Rate of new synapse formation (default: 0.0001)
- `pruning_rate` - Rate of synapse elimination (default: 0.00001)

### Memory Parameters
- `replay_interval` - Memory replay interval (default: 100)
- `consolidation_interval` - Memory consolidation interval (default: 1000)

### Development Parameters
- `development_stage` - Current developmental stage (0=Initial, 1=CriticalPeriod, 2=Maturation, 3=Adult)

## Command Line Interface

### Available Options
```bash
--help                    Show help message
--config <file>          Load configuration file
--experiment <name>      Run experiment (basic, plasticity, stdp, all)
--steps <N>              Number of simulation steps
--neurons <N>            Number of neurons (overrides config)
--regions <N>            Number of regions (overrides config)
```

### Example Usage
```bash
# Run comprehensive demo
./nlm_phase2_demo

# Run specific experiment with custom settings
./nlm --experiment plasticity --steps 5000 --neurons 2000

# Load custom configuration
./nlm --config my_config.json --experiment stdp

# Show help
./nlm --help
```

## Building and Running

### Build Instructions
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Running Tests
```bash
# Run all unit tests
./nlm_tests

# Run integration demo
./nlm_phase6_demo

# Run Phase 2 demo
./nlm_phase2_demo
```

## Advanced Features

### Checkpoints and Persistence
- `bool save(const std::string& filepath) const` - Save brain state
- `bool load(const std::string& filepath)` - Load brain state
- Checkpoint system supports compression and metadata tracking

### Error Handling
- All public methods include comprehensive error checking
- Config system validates keys and values
- Graceful degradation for invalid inputs

### Performance Features
- SIMD vectorization for neural computations
- Memory pools for efficient allocation
- Event-driven spike propagation for efficiency

## Research Features

### Data Collection
- Detailed spike recording for analysis
- Memory trace tracking
- Neuromodulator level monitoring

### Extensibility
- Plugin architecture for new plasticity rules
- Configurable learning algorithms
- Modular system design

## Best Practices

### Memory Management
- Use smart pointers for all object creation
- Brain, Config, and other objects are designed for move semantics
- Avoid raw pointers except for internal implementation details

### Error Handling
- Check return values of all public methods
- Use exception-safe code patterns
- Implement proper cleanup on errors

### Configuration
- Use configuration files for experiment parameters
- Override configuration via command line for quick testing
- Validate configuration before use

## Migration Notes

### From Previous Versions
- Configuration format now supports both simple key=value and JSON formats
- Command-line interface simplified and enhanced
- Some internal API changes for improved type safety

### API Stability
- Core APIs remain stable across versions
- New features added through optional parameters
- Backward compatibility maintained for public interfaces

## Conclusion

NLM provides a complete, integrated brain simulation system with advanced neural dynamics, learning, memory, and cognitive capabilities. The C++ API offers comprehensive control over all aspects of the simulated brain while maintaining ease of use for basic applications and extensibility for research.

The framework is designed for both practical applications and research investigations into brain-like computation and artificial intelligence.