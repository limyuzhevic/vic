# NLM Documentation

**Neural Learning Machine Documentation**

## Overview

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain. This document provides comprehensive information about using, configuring, and extending the NLM system.

## Table of Contents

1. [Quick Start Guide](#quick-start-guide)
2. [Configuration Management](#configuration-management)
3. [Core API Reference](#core-api-reference)
4. [Brain System Architecture](#brain-system-architecture)
5. [Memory Systems](#memory-systems)
6. [Plasticity Mechanisms](#plasticity-mechanisms)
7. [Neuromodulation](#neuromodulation)
8. [Development System](#development-system)
9. [Prediction and Cognition](#prediction-and-cognition)
10. [Memory Integration](#memory-integration)
11. [System Administration](#system-administration)
12. [Advanced Configuration](#advanced-configuration)
13. [Benchmarking and Performance](#benchmarking-and-performance)
14. [Python Bindings](#python-bindings)
15. [Contributing](#contributing)

## Quick Start Guide

### Installation

```bash
# Clone the repository
mkdir nlm-project
cd nlm-project

# Build the system
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### Basic Usage

```bash
# Run Phase 2 demo (basic neural computation)
./nlm

# Run Phase 6 integration demo
./nlm_phase6_demo

# Run benchmark tests
./nlm_benchmark
```

### Example Configuration

Create a configuration file `my_config.cfg`:

```
# Example NLM configuration file
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1000
region_count = 2
connection_probability = 0.15

# Plasticity settings
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_time_constant = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001

# Neuromodulation settings
dopamine_baseline = 0.1
reward_discount_factor = 0.99

# Logging settings
log_level = INFO
log_to_file = true
log_filename = nlm_run.log

# Environment settings
environment_name = GridWorld
environment_width = 20
environment_height = 20
```

### Running with Custom Configuration

```bash
./nlm --config my_config.cfg
```

## Configuration Management

NLM uses a flexible configuration system that supports multiple file formats and types.

### Supported Data Types

- **Basic types**: `int`, `int64_t`, `double`, `bool`, `std::string`
- **Array types**: `std::vector<int>`, `std::vector<double>`, `std::vector<std::string>`
- **Complex types**: Support for future nested objects

### Configuration File Formats

#### 1. Plain Text Format (default)

```
# Simple key=value format
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
connection_probability = 0.1
```

#### 2. JSON Format (future)

```json
{
    "random_seed": 12345,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.15,
    "plasticity": {
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015
    }
}
```

### Command Line Configuration

```bash
# Command line overrides configuration file
./nlm --config file.cfg --random_seed=99999 --neuron_count=2000

# Individual parameters
./nlm --neuron_count=2000 --region_count=4 --connection_probability=0.2
```

### Configuration Sources and Priority

1. **Default values** - Built-in defaults
2. **Configuration file** - `configs/default.cfg` or specified file
3. **Command line arguments** - Highest priority
4. **Runtime changes** - Can be modified during execution

### Configuration Management Functions

```cpp
// Get configuration values
auto seed = config.get<int64_t>("random_seed");
auto neurons = config.get<int64_t>("neuron_count");
auto timestep = config.get<double>("simulation_timestep");

// Get with default values
auto defaultSeed = config.getOr<int64_t>("random_seed", 42);

// Set configuration values
config.set("random_seed", 12345LL, ConfigSource::Runtime);
config.set("simulation_timestep", 0.001, ConfigSource::Runtime);
config.set("neuron_count", 1000LL, ConfigSource::Runtime);

// Check if configuration exists
if (config.has("custom_parameter")) {
    auto value = config.get<double>("custom_parameter");
}

// Get all configuration keys
auto keys = config.getKeys();

// Clear configuration
config.clear();

// Get configuration summary
std::string summary = config.summary();
```

## Core API Reference

### Basic Types

```cpp
namespace nlm {
    // Neuron identifiers
    using NeuronId = uint64_t;
    using SynapseId = uint64_t;
    using RegionId = uint64_t;
    using PopulationId = uint64_t;
    
    // Data types
    using SimulationStep = uint64_t;
    using Timestamp = double;
    using TimestepDuration = double;
    using SynapticWeight = float;
    using MembranePotential = float;
    using FiringRate = float;
    using Delay = uint32_t;
    
    // Neural properties
    enum class NeuronType { Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal };
    enum class SynapseType { Excitatory, Inhibitory, Modulatory, Electrical, GapJunction };
    enum class DevelopmentalStage { Initial, CriticalPeriod, Maturation, Adult, Aging };
    
    // State flags
    struct PlasticityFlags {
        bool hebbian;
        bool stdp;
        bool reward_modulated;
        bool structural;
        bool eligible;
    };
}
```

### Neuron Class

```cpp
class Neuron {
public:
    // Constructors and identity
    Neuron(NeuronId id);
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State management
    const NeuronState& getState() const;
    NeuronState& getState();
    void reset();
    void initializeRandom(RandomGenerator& rng);
    
    // Membrane potential dynamics
    void setMembranePotential(MembranePotential potential);
    MembranePotential getMembranePotential() const;
    void addToMembranePotential(MembranePotential delta);
    void injectCurrent(MembranePotential current);
    
    // Threshold and firing
    void setThreshold(MembranePotential threshold);
    MembranePotential getThreshold() const;
    bool checkThreshold() const;
    bool isFiring() const;
    bool isRefractory() const;
    
    // LIF dynamics
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    float getLastSpikeTime() const;
    
    // Synaptic input
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // Plasticity
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Input/output
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Connectivity
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Memory membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Statistics
    float getFiringRate() const;
    void setFiringRate(FiringRate rate);
    float getTotalCurrent() const;
    void clearTotalCurrent();
};
```

### Synapse Class

```cpp
class Synapse {
public:
    // Constructors
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    // Identity
    SynapseId getId() const;
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight management
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Plasticity
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Activity history
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Reward-modulated learning
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Dynamics
    void step(Timestamp currentTime);
    void reset();
    void initializeRandom(RandomGenerator& rng);
};
```

### Brain Class

```cpp
class Brain {
public:
    // Constructor
    explicit Brain(std::shared_ptr<Config> config);
    
    // Initialization
    bool initialize();
    void reset();
    
    // Main simulation loop
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // I/O
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Output generation
    std::unique_ptr<Action> produceAction();
    
    // Neuromodulation
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Plasticity
    void updatePlasticity();
    
    // Development
    void develop();
    
    // Memory access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction systems
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Development
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    float getAverageFiringRate() const;
    
    // Status
    void logStatus() const;
    
    // Checkpointing
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
};
```

## Brain System Architecture

### Regional Organization

NLM organizes neurons into regions and populations:

```cpp
// Create regions
RegionId region1 = brain->addRegion("Cortex");
RegionId region2 = brain->addRegion("Thalamus");

// Create populations
PopulationId sensory = region->addPopulation(100, NeuronType::Sensory);
PopulationId motor = region->addPopulation(50, NeuronType::Motor);
```

### Connectivity Patterns

#### Random Connectivity

The brain initializes random connectivity based on connection probability:

```cpp
// For each region, create random synapses
for (auto& region : brain->getRegions()) {
    region->initializeRandomConnectivity(*rng, connectionProbability, 0.2f, 0.1f);
}
```

#### Inter-region Connections

Long-range connections between regions:

```cpp
brain->addInterRegionConnection(RegionId(1), RegionId(2), 0.5f, 2);
brain->addInterRegionConnection(RegionId(2), RegionId(1), -0.3f, 1);
```

### Neural Dynamics

#### LIF Neuron Model

Each neuron implements Leaky Integrate-and-Fire dynamics:

```cpp
dV/dt = (V_rest - V)/tau + I/C
V_new = V + dt * 1000 * ((V_rest - V)/tau + I/C)
```

#### Spike Propagation

Spikes propagate through synapses with delays:

```cpp
// When a neuron fires:
// 1. Schedule outgoing spike events
// 2. Record post-synaptic spike for plasticity
// 3. For each outgoing synapse:
//    - Create delayed spike event
//    - Record pre-synaptic spike for STDP
//    - Store activation in working memory
```

## Memory Systems

### Working Memory

Manages active neural representations:

```cpp
// Store neuron activation
workingMemory->storeToNeuron(neuronId, activationValue);

// Get currently active neurons
auto activeNeurons = workingMemory->getMemoryNeurons();

// Get activation strengths
auto activations = workingMemory->getMemoryActivations();

// Update decay over time
workingMemory->update(timestep);
```

### Episodic Memory

Stores experience episodes:

```cpp
// Create and store episodes
episodes->storeEpisode(episode);

// Get episodes for replay
auto replayEpisodes = episodicMemory->getEpisodesForReplay(5);

// Replay important episodes (consolidation)
episodicMemory->replayEpisode(episode);

// Memory consolidation
episodicMemory->consolidate(0.3f);
```

### Associative Memory

Forms pattern associations:

```cpp
// Learn associations
associativeMemory->learnAssociation(patternA, patternB, strength);

// Retrieve associations
auto associations = associativeMemory->getAssociations(pattern);
```

## Plasticity Mechanisms

### Spike-Timing-Dependent Plasticity (STDP)

```cpp
// Configure STDP parameters
float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
float tau = config->getOr<float>("stdp_tau", 20.0f);
stdp->configure(ltpWeight, ltdWeight, tau);
```

### Hebbian Learning

```cpp
// Enable Hebbian plasticity
synapse->enablePlasticity(true, false, false);
```

### Structural Plasticity

```cpp
// Synaptogenesis (new synapse formation)
structuralPlasticity->setSynaptogenesisRate(0.0001f);

// Pruning (synapse elimination)
structuralPlasticity->setPruningRate(0.00001f);
```

## Neuromodulation

### Dopamine System

```cpp
// Apply dopamine effects
dopamine->update(timestep);
float dopamineLevel = dopamine->getLevel();

// Dopamine affects excitability
for (auto* neuron : allNeurons) {
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        neuron->injectCurrent(excitabilityMod);
    }
}
```

### Curiosity System

```cpp
// Update curiosity based on prediction error
curiosity->update(noveltyLevel, predictionError, timestep);

// Apply curiosity effects on exploration
if (curiosity->drivesExploration()) {
    // Increase exploration behavior
    explorationBias += curiosity->getLevel() * 0.1f;
}
```

### Reward Prediction Error

```cpp
// Update prediction error
predictionError->update(actualReward, predictedReward);
float error = predictionError->getError();

// Use error to modulate learning
plasticityMod = 1.0f + error * 0.5f;
```

## Development System

### Developmental Stages

```cpp
enum class DevelopmentalStage {
    Initial,        // High plasticity, rapid learning
    CriticalPeriod, // Moderate plasticity, specialized learning
    Maturation,     // Low plasticity, stabilization
    Adult,          // Stable, minimal change
    Aging           // Declining function
};
```

### Developmental Plasticity

```cpp
// Development affects plasticity rates based on stage
developmentSystem->update(brain, *rng, timestep * 1000);

// Example: Critical period reduces plasticity rate
DevelopmentalStage stage = brain->getDevelopmentalStage();
float plasticityMod = 1.0f;

if (stage == DevelopmentalStage::CriticalPeriod) {
    plasticityMod = 0.8f;
} else if (stage == DevelopmentalStage::Adult) {
    plasticityMod = 0.2f;
}
```

### Synaptogenesis and Pruning

```cpp
// Structural plasticity occurs periodically
if (currentStep % 1000 == 0) {
    developmentSystem->update(brain, *rng, timestep * 1000);
}

// Affects synaptic growth rates
sp->setSynaptogenesisRate(0.0001f * plasticityMod);
sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
```

## Prediction and Cognition

### Neural Prediction System

```cpp
// Prediction system runs alongside neural processing
predictionSystem->update(sensoryInput);

// Compute prediction errors
float predictionError = predictionSystem->getError();

// Use prediction error for learning
predictionError->update(actualInput, prediction);
```

### Neural Planner

```cpp
// Initialize planner
planner->initialize(brain);
planner->setPlanningDepth(5);

// Plan actions based on current state
auto plans = planner->planActions(currentState);
```

### Concept Formation

```cpp
// Concept formation processes patterns
conceptFormation->initialize(brain);

// Form concepts from neural activity
conceptFormation->processNeuralActivity(brain);

// Retrieve formed concepts
auto concepts = conceptFormation->getConcepts();
```

### Attentional Selection

```cpp
// Update attention system
attention->update(timestep);

// Process competition between representations
std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
attention->processCompetition(competitors);

// Get attended focus
NeuronId attended = attention->getAttendedNeuron();
```

## Memory Integration

### Memory-Neural Loop

```cpp
// Memory affects neural processing
// 1. Working memory provides activation patterns
// 2. Episodic memory provides context
// 3. Associative memory provides associations
// 4. All feed back into neural dynamics

// Example: Memory guides attention
if (workingMemory->hasActiveMemories()) {
    attention->focusOnMemoryNeurons(workingMemory->getMemoryNeurons());
}
```

### Replay and Consolidation

```cpp
// Memory replay during rest/consolidation
if (currentStep % replayInterval == 0) {
    auto episodesToReplay = episodicMemory->getEpisodesForReplay(3);
    for (const auto* episode : episodesToReplay) {
        episodicMemory->replayEpisode(episode);
    }
}

// Memory consolidation
if (currentStep % consolidationInterval == 0) {
    episodicMemory->consolidate(0.3f);
}
```

## System Administration

### Running Different Demos

```bash
# Phase 2: Basic neural computation demo
./nlm

# Phase 6: Full integration demo
./nlm_phase6_demo

# Phase 3: World interaction demo
./nlm_phase3_demo

# Phase 4: Cognitive development demo
./nlm_phase4_demo

# Phase 5: Performance optimizations demo
./nlm_phase5_demo

# Benchmark: Performance measurements
./nlm_benchmark
```

### Configuration Management

```bash
# List available configuration files
ls configs/

# View configuration file
less configs/default.cfg

# Create custom configuration
cp configs/default.cfg my_config.cfg
# Edit my_config.cfg

# Run with custom configuration
./nlm --config my_config.cfg

# Override specific parameters
./nlm --config my_config.cfg --neuron_count=2000 --simulation_timestep=0.0005
```

### Log Management

```bash
# View logs
less nlm.log

# Configure logging levels
# log_level = DEBUG
# log_level = INFO
# log_level = WARNING
# log_level = ERROR

# Log to file
# log_to_file = true
# log_filename = nlm_run.log
```

### Performance Monitoring

```bash
# Check system resources
./nlm_benchmark

# Monitor performance during run
top -p $(pgrep nlm)
```

### Checkpoint Management

```bash
# Save brain state
# During execution, call brain->save("checkpoint.nxml")

# Load brain state
# brain->load("checkpoint.nxml")

# Checkpoint frequency
# Configure in config file: checkpoint_interval = 1000
```

## Advanced Configuration

### Performance Parameters

```cpp
// High-performance configuration
config->set("neuron_count", 5000LL, ConfigSource::Runtime);
config->set("region_count", 5, ConfigSource::Runtime);
config->set("connection_probability", 0.05f, ConfigSource::Runtime);
config->set("simulation_timestep", 0.0005, ConfigSource::Runtime);
config->set("replay_interval", 500, ConfigSource::Runtime);
config->set("consolidation_interval", 5000, ConfigSource::Runtime);
```

### Memory-Intensive Configuration

```cpp
// Memory-optimized configuration
config->set("neuron_count", 2000, ConfigSource::Runtime);
config->set("working_memory_capacity", 200, ConfigSource::Runtime);
config->set("max_episodes", 100, ConfigSource::Runtime);
```

### Learning-Intensive Configuration

```cpp
// Intensive learning configuration
config->set("stdp_ltp_weight", 0.05f, ConfigSource::Runtime);
config->set("stdp_ltd_weight", 0.04f, ConfigSource::Runtime);
config->set("synaptogenesis_rate", 0.001f, ConfigSource::Runtime);
config->set("pruning_rate", 0.0005, ConfigSource::Runtime);
```

## Benchmarking and Performance

### Benchmark Metrics

```cpp
// Benchmark execution time
auto start = std::chrono::high_resolution_clock::now();
// Run simulation
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

// Benchmark spike processing
// Count spikes before and after
// duration = simulation time / spike count = time per spike

// Benchmark memory usage
// Track active neurons, synapses, memory usage
```

### Performance Optimization

#### SIMD Vectorization

```cpp
// Neural operations can use SIMD for better performance
// Vector operations on membrane potentials
// Parallel processing of neurons
```

#### Parallel Processing

```cpp
// Multiple regions can be processed in parallel
// Spike propagation across regions
// Memory updates
```

#### Memory Pools

```cpp
// Reuse memory allocations for better performance
// Neuron pools, synapse pools
// Event queues for spike delivery
```

### Performance Testing

```bash
# Run benchmark suite
./nlm_benchmark

# Measure performance with different configurations
cmake -DTEST_PERFORMANCE=ON
make
./nlm_benchmark --detailed
```

## Python Bindings

NLM provides Python bindings for integration with Python applications:

### Installation

```bash
# Build with Python bindings
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make

# Install Python module
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create brain with configuration
config = pynlm.Config()
config.set("neuron_count", 1000)
config.set("simulation_timestep", 0.001)

brain = pynlm.Brain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    # Get output actions
    action = brain.produceAction()
    if action:
        print(f"Action: {action.type}")

# Save brain state
brain.save("brain_checkpoint.pkl")
```

### Advanced Python Features

```python
# Access memory systems
working_memory = brain.getWorkingMemory()
e episodes = working_memory.getMemoryEpisodes()

# Access neuromodulation
novelty = brain.getNovelty()
novelty_level = novelty.getLevel()

# Access prediction systems
prediction_error = brain.getPredictionErrorSignal()
error = prediction_error.getError()
```

### Performance with Python

```python
# Vectorized operations for better performance
import numpy as np

# Batch processing
neuron_ids = np.arange(1000)
activations = brain.getNeurons(neuron_ids)

# Efficient memory access
memory_patterns = working_memory.getAllMemoryPatterns()
```

## Contributing

### Code Quality Guidelines

#### Coding Standards

- **C++20**: Use modern C++20 features
- **Memory safety**: Use smart pointers, avoid raw pointers
- **Error handling**: Use exceptions for error handling
- **Documentation**: Use Doxygen-style comments
- **Testing**: Write comprehensive unit tests

#### Performance Considerations

- **Cache efficiency**: Optimize memory access patterns
- **Parallelization**: Use multi-threading where appropriate
- **Vectorization**: Use SIMD instructions for numerical operations
- **Memory management**: Reuse memory allocations, use pools

#### Testing

```cpp
// Unit tests in tests/ directory
// Integration tests in experiments/
// Performance benchmarks in benchmarks/
```

### Development Workflow

1. **Feature development**: Create feature branch
2. **Code review**: Follow existing code style
3. **Testing**: Write tests before implementing features
4. **Documentation**: Update documentation
5. **Integration**: Test with existing systems
6. **Performance**: Optimize critical paths
7. **Release**: Create pull request with documentation

### Adding New Features

#### Plasticity Rules

1. Create new plasticity rule class
2. Implement update method
3. Integrate with existing plasticity system
4. Add configuration parameters
5. Write tests
6. Update documentation

#### Memory Systems

1. Design memory structure
2. Implement storage/retrieval
3. Integrate with neural processing
4. Add configuration options
5. Write tests
6. Update documentation

#### Neuromodulation

1. Design neuromodulator behavior
2. Implement integration with plasticity
3. Add configuration parameters
4. Write tests
5. Update documentation

### Research and Development

NLM is a research project. When adding new features:

1. **Document assumptions**: Clearly state theoretical basis
2. **Document limitations**: Be honest about capabilities and constraints
3. **Document implications**: Explain how features affect other systems
4. **Document trade-offs**: Explain performance vs. complexity trade-offs
5. **Peer review**: Have multiple people review changes
6. **Reproducibility**: Ensure results can be reproduced

### License

MIT License. See LICENSE file for details.

## Support and Contact

### Issues and Bug Reports

For bug reports and feature requests, please:
1. Check existing issues
2. Search for similar problems
3. Provide detailed reproduction steps
4. Include configuration and environment information
5. Provide expected vs. actual behavior

### Community

- **Documentation**: https://nlm.readthedocs.io
- **Code**: https://github.com/nlm-project/nlm
- **Discussions**: https://github.com/nlm-project/nlm/discussions
- **Issues**: https://github.com/nlm-project/nlm/issues

### Acknowledgments

This project builds on decades of research in computational neuroscience, artificial intelligence, and machine learning. Special thanks to:

- The NLM research team
- Contributors who improved the system
- Users who tested and provided feedback
- Open source projects that NLM depends on

## Version History

### Version 0.1.0 (2026-11-08)

- Initial release
- Core neural computation systems
- Basic memory integration
- Configuration system

### Version 0.2.0 (2026-12-01)

- Advanced plasticity rules
- Complete memory systems
- Neuromodulation integration
- Development system

### Version 0.3.0 (2027-01-15)

- Full prediction and cognition
- Advanced attention mechanisms
- Replay and consolidation
- Performance optimizations

### Future Versions

- **Phase 7**: Social learning and communication
- **Phase 8**: Meta-cognition and self-modeling
- **Phase 9**: Consciousness emergence research
- **Phase 10**: Practical applications

## Acknowledgments

This documentation was generated from the NLM source code and represents the current state of the project as of 2026-11-08. For the most recent information, please check the official documentation.

---

**Documentation maintained by the NLM project team**
**Last updated: 2026-11-08**

For questions or comments, please visit the NLM project repository.