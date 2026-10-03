# NLM (熙然) - Neural Learning Machine API Documentation

NLM is an experimental artificial developmental brain system implementing real spiking neural computation with event-driven dynamics and integrated cognitive systems.

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) API. NLM implements a modular computational brain with the following major components:

- **Core Layer**: Basic data types, configuration, random number generation, logging, and time management
- **Brain Layer**: Individual neurons, synapses, neural populations, regions, and the central brain coordinator
- **Dynamics Layer**: Neural dynamics, spike systems, and event-driven computation
- **Plasticity Layer**: STDP, Hebbian, and structural plasticity rules
- **Development Layer**: Synaptogenesis, pruning, and maturation
- **Neuromodulation Layer**: Dopamine, novelty, curiosity, and prediction error systems
- **Memory Layer**: Working memory, episodic memory, and associative memory
- **Prediction Layer**: Temporal sequence learning and action consequence prediction
- **Cognitive Layer**: Attention, planning, concept formation, and self-model
- **Sensory/Motor Layer**: Input/output processing and environment interaction

## Key Design Principles

1. **No ML Frameworks**: NLM implements its own neural computation from scratch
2. **No Transformer Architecture**: Uses spiking dynamics and competitive attention
3. **Biological Inspiration**: Designed to be incrementally more biologically accurate
4. **Modularity**: Clear interfaces between components
5. **Determinism**: Reproducible experiments via explicit random seeds
6. **Performance**: Data-oriented design for large-scale simulation
7. **Extensibility**: Clear patterns for adding new components
8. **No Predefined Knowledge**: All learning from experience

## Core Layer

### Config Class (`core/Config/Config.hpp`)

Configuration management system supporting JSON and legacy formats.

**Constructor**
```cpp
Config();
```

**Methods**
```cpp
bool loadFromFile(const std::string& filepath);     // Load from JSON or .cfg file
bool loadFromArgs(int argc, char** argv);          // Load from command line
bool saveToFile(const std::string& filepath) const; // Save to JSON
void set(const std::string& key, const ConfigValue& value, ConfigSource source);
template<typename T> std::optional<T> get(const std::string& key) const;
template<typename T> T getOr(const std::string& key, const T& defaultValue) const;
bool has(const std::string& key) const;
void remove(const std::string& key);
std::vector<std::string> getKeys() const;
std::string summary() const;
```

**Configuration Types**
- `ConfigSource`: Default, File, CommandLine, Runtime
- `ConfigValue`: Variant type supporting int, int64_t, double, bool, std::string, and vectors

### RandomGenerator Class (`core/Random/Random.hpp`)

Deterministic random number generator with multiple distributions.

**Constructor**
```cpp
RandomGenerator(uint64_t seed);
```

**Methods**
```cpp
uint64_t getSeed() const;
void setSeed(uint64_t seed);
int uniformInt(int min, int max);
double uniformReal(double min, double max);
float uniformReal(float min, float max);
double gaussian(double mean, double variance);
bool bernoulli(double probability);
```

### SimulationClock Class (`core/SimulationClock/SimulationClock.hpp`)

Time management for simulation with millisecond precision.

**Constructor**
```cpp
SimulationClock(TimestepDuration timestep);
```

**Methods**
```cpp
void step();                                    // Advance time by one timestep
double getCurrentTime() const;                  // Get current simulation time
SimulationStep getCurrentStep() const;          // Get current step number
TimestepDuration getTimestep() const;            // Get timestep duration
void setTimestep(TimestepDuration timestep);
```

## Brain Layer

### Brain Class (`brain/Brain.hpp`)

Central coordinator of the neural system implementing real spiking neural computation.

**Constructor**
```cpp
Brain(std::shared_ptr<Config> config);
```

**Main Methods**
```cpp
bool initialize();                              // Initialize brain with configuration
void step(SimulationStep currentStep);           // Single simulation step
void step(SimulationStep currentStep, Timestamp currentTime); // Step with explicit time
void receiveSensoryInput(const SensoryInput& input);     // Process sensory input
void injectCurrent(NeuronId neuron, MembranePotential current); // Inject current into specific neuron
void injectCurrentToNeurons(NeuronType type, MembranePotential current); // Inject into all neurons of type

// Accessors
SpikeSystem* getSpikeSystem();                  // Get spike system
STDP* getSTDP();                                // Get STDP plasticity system
Hebbian* getHebbian();                          // Get Hebbian plasticity system
StructuralPlasticity* getStructuralPlasticity();  // Get structural plasticity

// Statistics and queries
size_t getTotalNeuronCount() const;
size_t getTotalSynapseCount() const;
size_t getTotalSpikeCount() const;
float getExcitationInhibitionRatio() const;
```

### Neuron Class (`brain/Neuron.hpp`)

LIF (Leaky Integrate-and-Fire) neuron implementation with full biophysical properties.

**Constructor**
```cpp
Neuron(NeuronId id, NeuronType type);
```

**Core Methods**
```cpp
void stepLIF(Timestamp currentTime, TimestepDuration dt);      // Update neuron dynamics
void injectCurrent(MembranePotential current);                   // Inject current
void receiveExcitatoryInput(MembranePotential input);           // Receive excitatory input
void receiveInhibitoryInput(MembranePotential input);           // Receive inhibitory input
void fire();                                                     // Handle spike emission
```

**State Accessors**
```cpp
MembranePotential getMembranePotential() const;
MembranePotential getRestingPotential() const;
MembranePotential getThreshold() const;
float getMembranePotentialDecay() const;
FiringState getFiringState() const;
bool isFiring() const;
Timestamp getLastSpikeTime() const;
```

### Synapse Class (`brain/Synapse.hpp`)

Synaptic connection with delays, short-term plasticity, and spike-timing-dependent plasticity.

**Constructor**
```cpp
Synapse(SynapseId id, NeuronId source, NeuronId destination);
```

**Core Methods**
```cpp
void step(Timestamp currentTime);                    // Update synaptic dynamics
void recordPreSpike(Timestamp timestamp);             // Record presynaptic spike
void recordPostSpike(Timestamp timestamp);            // Record postsynaptic spike
void addToWeight(SynapticWeight delta);               // Modify synaptic weight
void setDelay(Delay delay);                           // Set transmission delay
```

**Plasticity Integration**
```cpp
void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
void setEligibilityTrace(float trace);              // For reward-modulated learning
float getEligibilityTrace() const;
```

## Plasticity Layer

### STDP Class (`plasticity/STDP.hpp`)

Spike-Timing-Dependent Plasticity implementing the classic pre-before-post potentiation and post-before-pre depression.

**Constructor**
```cpp
STDP();
```

**Core Methods**
```cpp
void update(Synapse* synapse,
            const std::vector<Timestamp>& preSpikes,
            const std::vector<Timestamp>& postSpikes,
            TimestepDuration dt);
void applyWeightChange(Synapse* synapse, SynapticWeight delta);
```

**Configuration**
```cpp
void configure(float ltpWeight, float ltdWeight, float tau);  // Set STDP parameters
void setLTPWeight(float weight);                              // Long-term potentiation
void setLTDWeight(float weight);                              // Long-term depression
void setTimeConstant(float tau);                              // Time window width
```

**Accessors**
```cpp
float getLTPWeight() const;
float getLTDWeight() const;
float getTimeConstant() const;
const char* getName() const;
```

### Hebbian Class (`plasticity/Hebbian.hpp`)

Hebbian learning implementing "neurons that fire together, wire together" with correlation-based learning.

**Constructor**
```cpp
Hebbian();
```

**Core Methods**
```cpp
void update(Synapse* synapse,
            const std::vector<Timestamp>& preSpikes,
            const std::vector<Timestamp>& postSpikes,
            TimestepDuration dt);
void applyWeightChange(Synapse* synapse, SynapticWeight delta);
```

**Configuration**
```cpp
void setLearningRate(float rate);
void setMaxWeight(float maxWeight);
```

**Accessors**
```cpp
float getLearningRate() const;
float getMaxWeight() const;
const char* getName() const;
```

### StructuralPlasticity Class (`plasticity/StructuralPlasticity.hpp`)

Synaptogenesis and pruning system implementing activity-dependent structural changes.

**Core Methods**
```cpp
void setSynaptogenesisRate(float rate);
void setPruningRate(float rate);
void update(Brain* brain, RandomGenerator& rng);
```

## Memory Systems

### NeuralWorkingMemory Class (`memory/NeuralWorkingMemory.hpp`)

Persistent activity system for temporary storage of recent information.

**Core Methods**
```cpp
void initialize(Brain* brain);
void storeToNeuron(NeuronId neuronId, float activation);
void update(TimestepDuration dt);
std::vector<NeuronId> getMemoryNeurons() const;
size_t getActiveTraces() const;
```

### NeuralEpisodicMemory Class (`memory/NeuralEpisodicMemory.hpp`)

Experience storage system encoding episodes as neural patterns with temporal indexing.

**Core Methods**
```cpp
void initialize(Brain* brain);
void storeEpisode(const EpisodicMemoryItem& episode);
const EpisodicMemoryItem* getEpisode(size_t index) const;
std::vector<const EpisodicMemoryItem*> getEpisodesForReplay(size_t maxCount) const;
void replayEpisode(const EpisodicMemoryItem* episode);
size_t getEpisodeCount() const;
void consolidate(float threshold);
```

### NeuralAssociativeMemory Class (`memory/NeuralAssociativeMemory.hpp`)

Pattern association system forming connections between co-occurring experiences.

**Core Methods**
```cpp
void initialize(Brain* brain);
void associate(const std::vector<NeuronId>& pattern1, const std::vector<NeuronId>& pattern2);
std::vector<NeuronId> retrieve(const std::vector<NeuronId>& pattern) const;
```

## Neuromodulation Systems

### Dopamine Class (`neuromodulation/Dopamine.hpp`)

Reward and reinforcement system modulating neural excitability and learning rates.

**Core Methods**
```cpp
void initialize(Brain* brain);
void update(TimestepDuration dt);
float getLevel() const;                            // Current dopamine level
float getPlasticityFactor() const;                  // Modulation of learning rates
void setLevel(float level);
```

### Curiosity Class (`neuromodulation/Curiosity.hpp`)

Exploration motivation system driving search for novel information.

**Core Methods**
```cpp
void initialize(Brain* brain);
void update(TimestepDuration dt);
float getDrive() const;                            // Curiosity drive level
bool shouldExplore() const;                         // Decision to explore
```

### Novelty Class (`neuromodulation/Novelty.hpp`)

Novelty detection system identifying new or unexpected patterns.

**Core Methods**
```cpp
void initialize(Brain* brain);
void update(TimestepDuration dt);
float getNovelty() const;                          // Novelty signal strength
void setReferencePattern(const std::vector<float>& pattern);
```

## Cognitive Systems

### NeuralPlanner Class (`cognition/NeuralPlanner.hpp`)

Action planning system using predictions to select optimal behaviors.

**Core Methods**
```cpp
void initialize(Brain* brain);
void setPlanningDepth(size_t depth);
std::unique_ptr<Action> planAction();               // Generate action plan
```

### ConceptFormation Class (`cognition/ConceptFormation.hpp`)

Pattern discovery system forming categories without predefined labels.

**Core Methods**
```cpp
void initialize(Brain* brain);
void processPattern(const std::vector<float>& pattern);
std::vector<std::string> getConcepts() const;
float getConceptStability(const std::string& concept) const;
```

### AttentionalSelection Class (`cognition/AttentionalSelection.hpp`)

Competitive attention system allocating processing resources to salient stimuli.

**Core Methods**
```cpp
void initialize(Brain* brain);
void setInhibitionStrength(float strength);
void setExcitationStrength(float strength);
void update(TimestepDuration dt);
void processCompetition(const std::vector<NeuronId>& competitors);
NeuronId getSelectedTarget() const;
```

## Key Data Types

### Type Definitions (`core/Types/Types.hpp`)

Strongly-typed identifiers for neural system components.

**Neuron Types**
```cpp
enum class NeuronType { Sensory, Internal, Motor };
```

**Identification**
```cpp
using NeuronId = uint64_t;
using SynapseId = uint64_t;
using RegionId = uint64_t;
using PopulationId = uint64_t;
using SimulationStep = uint64_t;
using Timestamp = double;
using MembranePotential = float;
using SynapticWeight = float;
using Delay = uint8_t;
```

**State**
```cpp
struct NeuronState {
    MembranePotential membranePotential;
    MembranePotential restingPotential;
    MembranePotential threshold;
    MembranePotential resetPotential;
    MembranePotential leakConductance;
    FiringState firingState;
    float refractoryRemaining;
    float refractoryPeriod;
    Timestamp lastSpikeTime;
};
```

**Flag Structures**
```cpp
struct PlasticityFlags {
    bool hebbian;
    bool stdp;
    bool reward_modulated;
};
```

## Building and Running

### Dependencies
NLM requires a modern C++20 compiler and standard library. No external dependencies beyond the standard library.

### Building with CMake
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Running the Default Simulation
```bash
./nlm
```

### Running with Custom Configuration
```bash
./nlm --config my_config.json --neuron_count 2000 --random_seed 12345
```

### Configuration File Formats

**JSON Format (recommended)**
```json
{
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "simulation_timestep": 0.001,
    "random_seed": 42,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015,
    "stdp_tau": 20.0,
    "synaptogenesis_rate": 0.0001,
    "pruning_rate": 0.00001,
    "_description": "Phase 2 default configuration"
}
```

**Simple Format (.cfg)**
```
neuron_count = 1000
region_count = 2
connection_probability = 0.1
simulation_timestep = 0.001
random_seed = 42
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
# Default configuration for NLM Phase 2
```

### Command Line Options
```
--help                    Show help message and exit
--verbose                 Enable verbose logging
--config <file>           Load configuration from file
--neuron_count <n>        Set number of neurons
--region_count <n>         Set number of brain regions
--connection_probability <p> Set connection probability
--simulation_timestep <t> Set simulation timestep in seconds
--random_seed <seed>      Set random seed for reproducible experiments
```

### Running Tests
```bash
cd build
ctest
```

## Performance Considerations

### Large-Scale Simulations
- Use JSON configuration for better parameter management
- Consider `--neuron_count` values up to 1,000,000 for large networks
- Enable `--verbose` mode for performance profiling
- Monitor memory usage with `--help` to see system requirements

### Memory Optimization
- The system uses approximately 50 bytes per neuron
- For 1M neurons: ~50MB for neuron data, plus synaptic connections
- Synaptic connections are sparse (typically 10-20% connectivity)

### Computational Complexity
- Neuron updates: O(N) where N = neuron count
- Synapse processing: O(E) where E = synapse count  
- Spike processing: O(S) where S = spike count in window
- Overall complexity is linear with system size for all major operations

## Extensibility

### Adding New Plasticity Rules
1. Create new class inheriting from `PlasticityRule`
2. Implement `update()` and `applyWeightChange()` methods
3. Register with Brain using existing plasticity system

### Adding New Neuron Dynamics
1. Create new class inheriting from `NeuralDynamics`
2. Implement update rules for membrane potential
3. Register with Brain's neuron factory

### Adding New Memory Systems
1. Create new class inheriting from memory base interface
2. Implement storage/retrieval methods
3. Integrate with Brain's memory management system

### Adding New Sensory Modalities
1. Create new class inheriting from `SensoryInput`
2. Implement pattern extraction from world state
3. Register with Brain's sensory processing

## Acknowledgements

This project represents years of research in computational neuroscience and artificial intelligence. The NLM system builds upon decades of work in spiking neural networks, plasticity rules, and cognitive architectures.

## License

MIT License - see LICENSE file for details.
