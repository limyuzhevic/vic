# NLM C++ API Documentation

## Overview

The NLM (Neural Learning Machine) C++ API provides a complete interface for neural simulation with spiking neural networks, event-driven dynamics, and integrated memory systems.

## Core Classes

### Brain
The central neural simulation brain class implementing Leaky Integrate-and-Fire (LIF) dynamics and complete integrated brain systems.

#### Key Methods

```cpp
// Creation and initialization
Brain(std::shared_ptr<Config> config);  // Constructor with config
bool initialize();                    // Initialize brain from configuration

// Main simulation loop
void step(SimulationStep currentStep);           // Step with timestep
void step(SimulationStep currentStep, Timestamp currentTime); // Step with timestamp

// I/O handling
void receiveSensoryInput(const SensoryInput& input);           // Inject sensory input
void injectCurrent(NeuronId neuron, MembranePotential current); // Inject to specific neuron
void injectCurrentToNeurons(NeuronType type, MembranePotential current); // Inject to neuron type

// Action production
std::unique_ptr<Action> produceAction();                      // Get motor action

// State management
void reset();                          // Reset all state
bool save(const std::string& filepath) const;  // Save to checkpoint
bool load(const std::string& filepath);       // Load from checkpoint

// Region management
RegionId addRegion(const std::string& name = "");     // Add neural region
NeuralRegion* getRegion(RegionId id);               // Get region by ID
size_t getRegionCount() const;                      // Get region count

// Statistics and monitoring
float getExcitationInhibitionRatio() const;         // E/I balance
float getAverageFiringRate() const;                  // Average firing rate
size_t getTotalNeuronCount() const;                  // Total neurons
size_t getTotalSynapseCount() const;                 // Total synapses
size_t getTotalSpikeCount() const;                   // Total spikes

// Memory system access (Phase 6 integrated systems)
NeuralWorkingMemory* getWorkingMemory();            // Working memory
NeuralEpisodicMemory* getEpisodicMemory();          // Episodic memory
NeuralAssociativeMemory* getAssociativeMemory();     // Associative memory
PredictionSystem* getPredictionSystem();            // Prediction system

// Cognition systems (Phase 6 integrated)
NeuralPlanner* getPlanner();                         // Neural planner
ConceptFormation* getConceptFormation();             // Concept formation
AttentionalSelection* getAttention();                // Attention system
DevelopmentSystem* getDevelopmentSystem();           // Development system

// Neuromodulation systems (Phase 6 integrated)
Dopamine* getDopamine();                             // Dopamine (reward)
Curiosity* getCuriosity();                           // Curiosity (exploration)
Novelty* getNovelty();                               // Novelty detection
PredictionError* getPredictionErrorSignal();         // Prediction error

// Plasticity systems (Phase 2)
SpikeSystem* getSpikeSystem();                       // Spike dynamics
STDP* getSTDP();                                     // Spike-timing dependent plasticity
Hebbian* getHebbian();                               // Hebbian learning
StructuralPlasticity* getStructuralPlasticity();     // Structural changes

// Configuration and random access
std::shared_ptr<const Config> getConfig() const;      // Get configuration
RandomGenerator* getRandomGenerator();               // Random number generator

// Status and logging
void logStatus() const;                              // Log brain status
```

### Config
Flexible configuration system supporting nested sections, validation, and multiple sources.

#### Key Methods

```cpp
// Creation and loading
Config();                                           // Default constructor
bool loadFromFile(const std::string& filepath);     // Load from file
bool loadFromArgs(int argc, char** argv);            // Load from command line
bool saveToFile(const std::string& filepath) const; // Save to file

// Value access with section support
std::optional<T> get(const std::string& section, const std::string& key);     // Get with section
template<typename T> T getOr(const std::string& section, const std::string& key, const T& defaultValue); // Get with default

// Value setting with section support
template<typename T> void set(const std::string& section, const std::string& key, const T& value); // Set with section

// Key existence and management
bool has(const std::string& section, const std::string& key);     // Check existence
void remove(const std::string& section, const std::string& key);  // Remove key
std::vector<std::string> getKeys(const std::string& section = ""); // Get keys (section-specific)
std::vector<std::string> getSections() const;                     // Get all sections

// Validation and schema
bool validateParameter(const ConfigParameter& param);              // Validate single parameter
bool validateAllParameters(const std::vector<ConfigParameter>& parameters); // Validate all parameters
void setDefaultConfigFile(const std::string& filepath);           // Set default config file
std::string getDefaultConfigFile() const;                        // Get default config file

// Diagnostics
std::vector<std::string> getErrors() const;                       // Get error messages
void clearErrors();                                               // Clear error messages
std::string summary() const;                                      // Get configuration summary
```

### NeuralRegion
Represents a brain region containing multiple neuron populations and local connectivity.

#### Key Methods

```cpp
// Creation and identity
NeuralRegion(RegionId id);                                    // Constructor
NeuralRegion(RegionId id, const std::string& name);           // Constructor with name
RegionId getId() const;                                       // Get region ID
const std::string& getName() const;                          // Get region name
void setName(const std::string& name);                        // Set region name

// Population management
PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal); // Add population
NeuralPopulation* getPopulation(PopulationId id);             // Get population
size_t getPopulationCount() const;                           // Get population count

// Neuron access
std::vector<Neuron*> getAllNeurons();                         // Get all neurons
std::vector<const Neuron*> getAllNeurons() const;            // Get all neurons (const)

// Statistics
size_t getTotalNeuronCount() const;                          // Total neurons
size_t getActiveNeuronCount() const;                         // Active neurons
size_t getFiringNeuronCount() const;                         // Firing neurons
float getAverageFiringRate() const;                          // Average firing rate
float getAverageSynapticWeight() const;                       // Average synaptic weight
float getSynapticDensity() const;                             // Synaptic density (connectivity)

// Connectivity
std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);       // Get outgoing synapses
std::vector<Synapse*> getSynapsesTo(NeuronId neuron);         // Get incoming synapses

// Simulation
void step(Timestamp currentTime);                           // Step all components
void reset();                                               // Reset all state
void initializeRandomConnectivity(RandomGenerator& rng,
                                  float connectionProbability,
                                  float meanWeight,
                                  float weightVariance);     // Initialize random connectivity
```

### NeuralPopulation
Group of neurons with shared properties and statistics.

#### Key Methods

```cpp
// Creation and identity
NeuralPopulation(PopulationId id, size_t size);               // Constructor
PopulationId getId() const;                                   // Get population ID
size_t getSize() const;                                       // Get population size
bool isEmpty() const;                                        // Check if empty

// Neuron access
Neuron* getNeuron(NeuronIndex index);                        // Get neuron by index
const Neuron* getNeuron(NeuronIndex index) const;            // Get neuron (const)
void addNeuron(Neuron* neuron);                               // Add neuron
const std::vector<Neuron*>& getNeurons() const;              // Get all neurons

// Population type
NeuronType getNeuronType() const;                           // Get neuron type
void setNeuronType(NeuronType type);                         // Set neuron type

// Statistics
float getActivityLevel() const;                              // Activity level (0-1)
float getAverageFiringRate() const;                          // Average firing rate
size_t getActiveCount() const;                               // Active neuron count
size_t getFiringCount() const;                               // Firing neuron count

// Simulation
void initializeRandom(RandomGenerator& rng);                  // Initialize neurons randomly
void step(Timestamp currentTime);                           // Step all neurons
void reset();                                               // Reset all neurons

// Statistics recording
void recordActivity();                                       // Record current activity
float getMeanMembranePotential() const;                      // Mean membrane potential
float getVarianceMembranePotential() const;                  // Variance of membrane potential
```

### Neuron
Single neuron implementing Leaky Integrate-and-Fire (LIF) dynamics.

#### Key Methods

```cpp
// Creation and identity
Neuron(NeuronId id);                                         // Constructor
NeuronId getId() const;                                       // Get neuron ID

// State access
NeuronState getState() const;                               // Get complete state
NeuronState& getState();                                     // Get complete state (non-const)

// Membrane potential
MembranePotential getMembranePotential() const;             // Get membrane potential
void setMembranePotential(MembranePotential potential);      // Set membrane potential
void addToMembranePotential(MembranePotential delta);        // Add to membrane potential

// Threshold and state
MembranePotential getThreshold() const;                      // Get firing threshold
void setThreshold(MembranePotential threshold);              // Set firing threshold
bool checkThreshold() const;                                  // Check if threshold exceeded
bool isFiring() const;                                       // Check if neuron is firing
bool isRefractory() const;                                    // Check if in refractory period
void setFiringState(FiringState state);                      // Set firing state
void setRefractoryPeriod(uint32_t steps);                    // Set refractory period
void decrementRefractory();                                  // Decrement refractory counter

// LIF parameters
void setLeakConductance(MembranePotential conductance);      // Set leak conductance
MembranePotential getLeakConductance() const;                // Get leak conductance
uint32_t getRefractoryPeriod() const;                        // Get refractory period
void setRestingPotential(MembranePotential potential);       // Set resting potential
MembranePotential getRestingPotential() const;               // Get resting potential
void setResetPotential(MembranePotential potential);         // Set reset potential
MembranePotential getResetPotential() const;                 // Get reset potential

// Firing and spikes
float getLastSpikeTime() const;                             // Get last spike time
bool stepLIF(Timestamp currentTime, TimestepDuration dt);    // Perform LIF step, return true if fired
void recordSpike(Timestamp timestamp);                      // Record spike for plasticity
const std::vector<Timestamp>& getSpikeHistory() const;      // Get spike history
void clearSpikeHistory();                                    // Clear spike history

// Current and inputs
void receiveExcitatoryInput(MembranePotential amplitude);  // Receive excitatory input
void receiveInhibitoryInput(MembranePotential amplitude);    // Receive inhibitory input
void receiveModulatoryInput(MembranePotential amplitude);    // Receive modulatory input
void injectCurrent(MembranePotential current);              // Inject external current
MembranePotential getTotalCurrent() const;                  // Get total injected current
void clearTotalCurrent();                                    // Clear injected current

// Plasticity
const PlasticityFlags& getPlasticityFlags() const;          // Get plasticity flags
PlasticityFlags& getPlasticityFlags();                       // Get plasticity flags (non-const)
void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated); // Enable plasticity

// Membership
void setRegionId(RegionId region);                          // Set region membership
RegionId getRegionId() const;                               // Get region ID
void setPopulationId(PopulationId population);               // Set population membership
PopulationId getPopulationId() const;                       // Get population ID

// Simulation
void step(Timestamp currentTime);                          // Update neuron for one step
void reset();                                               // Reset to initial state
void initializeRandom(RandomGenerator& rng);                 // Initialize with random parameters
```

### Synapse
Neural connection between neurons with dynamic properties.

#### Key Methods

```cpp
// Creation and identity
Synapse(NeuronId source, NeuronId destination);           // Constructor
SynapseId getId() const;                                 // Get synapse ID
NeuronId getSourceNeuron() const;                        // Get source neuron ID
NeuronId getDestinationNeuron() const;                   // Get destination neuron ID

// Weight and properties
float getWeight() const;                                 // Get synaptic weight
void setWeight(float weight);                           // Set synaptic weight
Delay getDelay() const;                                 // Get transmission delay
void setDelay(Delay delay);                             // Set transmission delay

// Type and modulation
SynapseType getType() const;                            // Get synapse type
void setType(SynapseType type);                         // Set synapse type
PlasticityFlags getPlasticityFlags() const;             // Get plasticity flags
void setPlasticityFlags(const PlasticityFlags& flags);   // Set plasticity flags

// State and history
float getEligibilityTrace() const;                      // Get eligibility trace
void setEligibilityTrace(float trace);                  // Set eligibility trace

// Plasticity history
const std::vector<Timestamp>& getPreSpikeHistory() const;   // Get presynaptic spike history
const std::vector<Timestamp>& getPostSpikeHistory() const;  // Get postsynaptic spike history
void recordPreSpike(Timestamp timestamp);               // Record presynaptic spike
void recordPostSpike(Timestamp timestamp);              // Record postsynaptic spike
void clearSpikeHistory();                               // Clear spike history

// Simulation
void step(Timestamp currentTime);                       // Update synapse state
void reset();                                           // Reset state
void initializeRandom(RandomGenerator& rng);             // Initialize random properties

// Plasticity updates
void updateSTDP(const SpikeSystem& spikeSystem,         // Update STDP
               const std::vector<Timestamp>& preSpikes,
               const std::vector<Timestamp>& postSpikes,
               TimestepDuration dt);
void updateHebbian(const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt);                  // Update Hebbian
```

### AgentBrain
Interface connecting NLM brain to world simulation for agent behavior.

#### Key Methods

```cpp
// Creation
AgentBrain(std::shared_ptr<Brain> brain);               // Constructor

// Initialization
void initialize(SimpleWorld& world);                    // Initialize with world

// Sensory processing
void processSensoryInput(const SensoryPercept& percept); // Process sensory input

// Motor decoding
Action decodeMotorCommand();                            // Decode motor command

// Neuromodulation
void applyRewardModulation(float reward, float predictedReward); // Apply reward modulation

// Development
void updateDevelopment(float timestep);                // Update development

// Subsystem enable/disable (Phase 6 features)
void enableRewardModulation(bool enable);              // Enable reward modulation
void enableStructuralPlasticity(bool enable);          // Enable structural plasticity
void enableDevelopment(bool enable);                   // Enable development
void enableCuriosity(bool enable);                     // Enable curiosity

// Subsystem status
bool isRewardModulationEnabled() const;                // Check reward modulation
bool isStructuralPlasticityEnabled() const;            // Check structural plasticity
bool isDevelopmentEnabled() const;                     // Check development
bool isCuriosityEnabled() const;                        // Check curiosity

// State getters
float getCuriosityLevel() const;                        // Get curiosity level
float getNoveltyLevel() const;                          // Get novelty level
float getPredictionError() const;                       // Get prediction error
float getNeuromodulationLevel() const;                  // Get neuromodulation level
DevelopmentalStage getDevelopmentalStage() const;     // Get developmental stage

// World interaction
Action decodeMotorCommand();                            // Decode motor action
```

## Configuration System

The configuration system supports nested sections with a clean, type-safe API:

### Configuration File Format

```ini
[general]
random_seed = 42
simulation_timestep = 0.001

brain.neuron_count = 1000
brain.region_count = 2
brain.connection_probability = 0.1

[plasticity]
plasticity_learning_rate = 0.01
stdp_ltp_weight = 0.01
stdp_ltd_weight = 0.012

[development]
synaptogenesis_rate = 0.001
pruning_rate = 0.0001

[neuromodulation]
dopamine_baseline = 0.1

[environment]
environment_name = GridWorld
environment_width = 10
environment_height = 10
```

### Parameter Validation

```cpp
// Define parameter schema
std::vector<ConfigParameter> parameters = {
    ConfigParameter("", "brain.neuron_count", "Number of neurons", "1000", true,
                   [](const ConfigValue& v) { return std::get<int>(v) > 0; }),
    ConfigParameter("plasticity", "stdp_ltp_weight", "STDP LTP weight", "0.01"),
    ConfigParameter("development", "synaptogenesis_rate", "Synaptogenesis rate", "0.001")
};

// Validate configuration
Config config;
config.loadFromFile("config.ini");
if (config.validateAllParameters(parameters)) {
    // Configuration is valid
} else {
    // Show errors
    for (const auto& error : config.getErrors()) {
        std::cout << "Error: " << error << std::endl;
    }
}
```

## Advanced Usage Patterns

### 1. Creating a Complete Agent Simulation

```cpp
// 1. Create configuration
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 2000);
config->set("brain.region_count", 3);
config->set("plasticity.stdp.enable", true);

// 2. Create brain and initialize
auto brain = std::make_shared<Brain>(config);
brain->initialize();

// 3. Create world
auto world = std::make_shared<SimpleWorld>();
world->configure(30, 30, 8, 8);
world->reset();
world->setAgentStart(15.0, 15.0);

// 4. Create agent interface
auto agent = std::make_shared<AgentBrain>(brain);
agent->initialize(*world);

// 5. Enable learning systems
agent->enableRewardModulation(true);
agent->enableCuriosity(true);
agent->enableDevelopment(true);
agent->enableStructuralPlasticity(true);

// 6. Run simulation loop
for (size_t step = 0; step < 1000; ++step) {
    world->update(0.1);
    
    auto percept = world->getSensoryPercept();
    agent->processSensoryInput(percept);
    
    brain->step(step);
    
    auto action = agent->decodeMotorCommand();
    world->applyMotorCommand(action, world->getSimulationTime());
    
    // Optional: Apply reward modulation
    // agent->applyRewardModulation(reward, predictedReward);
}
```

### 2. Brain State Persistence (Checkpointing)

```cpp
// Save brain state (checkpoint)
brain->save("checkpoint_phase5.bin");

// Later, load brain state
if (brain->load("checkpoint_phase5.bin")) {
    std::cout << "Brain loaded successfully" << std::endl;
} else {
    std::cout << "Failed to load brain" << std::endl;
}
```

### 3. Custom Configuration Loading

```cpp
// Command line interface support
int main(int argc, char** argv) {
    auto config = std::make_shared<Config>();
    
    // Load from default config file
    config->setDefaultConfigFile("configs/default.cfg");
    config->loadFromFile(config->getDefaultConfigFile());
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    // Create brain with merged configuration
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    return 0;
}
```

### 4. Region-based Brain Organization

```cpp
// Create brain with multiple regions
auto config = std::make_shared<Config>();
config->set("brain.neuron_count", 1000);
config->set("brain.region_count", 4);

auto brain = std::make_shared<Brain>(config);
brain->initialize();

// Access different regions
auto regions = brain->getRegions();
for (const auto& region : regions) {
    std::cout << "Region " << region->getName() << ": " 
              << region->getTotalNeuronCount() << " neurons" << std::endl;
    
    // Get specific neuron types
    auto populations = region->getAllPopulations();
    for (const auto& pop : populations) {
        std::cout << "  Population type: " 
                  << static_cast<int>(pop->getNeuronType()) << std::endl;
    }
}
```

### 5. Plasticity Control

```cpp
// Access and modify plasticity systems
auto stdp = brain->getSTDP();
if (stdp) {
    // Configure STDP parameters
    stdp->configure(0.01f, 0.012f, 20.0f);
}

auto hebbian = brain->getHebbian();
if (hebbian) {
    // Enable/disable Hebbian learning
    hebbian->enable(true);
}

auto structural = brain->getStructuralPlasticity();
if (structural) {
    // Set developmental rates
    structural->setSynaptogenesisRate(0.0001f);
    structural->setPruningRate(0.00001f);
}
```

## Building and Installation

### Build from Source

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
make -j$(nproc)

# Run tests (if built)
ctest --output-on-failure
```

### Python Package

```bash
# Install build dependencies
pip install pybind11 scikit-build-core pytest numpy

# Build and install Python bindings
pip install .

# Or install in development mode
pip install -e .
```

## Testing and Validation

The NLM project includes comprehensive tests for all major components:

1. **Unit Tests**: Test individual classes and systems
2. **Integration Tests**: Test complete brain systems
3. **Performance Tests**: Benchmark key operations
4. **Phase Tests**: Verify specific phase implementations

### Running Tests

```bash
# Run all tests
cd build
ctest

# Run specific test suite
./nlm_test --filter "plasticity"
```

## Performance Considerations

### Memory Management

- The NLM framework uses RAII (Resource Acquisition Is Initialization) for automatic memory management
- Smart pointers (`std::shared_ptr`, `std::unique_ptr`) manage object lifetimes
- Event-driven architecture minimizes unnecessary computation

### Optimization Features

1. **Memory Pools**: Reduce allocation overhead
2. **Event Queues**: Efficient spike event delivery
3. **Parallel Processing**: SIMD vectorization for neuron operations
4. **Sparse Connectivity**: Efficient synaptic connection management

## Troubleshooting

### Common Issues and Solutions

**Issue**: Brain initialization fails with configuration errors
**Solution**:
- Check configuration file syntax
- Validate parameter types and ranges
- Ensure required parameters are present

**Issue**: Simulation runs slowly
**Solution**:
- Enable performance optimizations
- Reduce neuron count for debugging
- Profile with `perf` or `gprof`

**Issue**: Python bindings not found
**Solution**:
- Ensure Python package is installed: `pip install .`
- Check Python path: `python -c "import pynlm; print(pynlm.__version__)"`

## Future Extensions

The NLM framework is designed for extensibility:

1. **New Neuron Models**: Hodgkin-Huxley, Morris-Lecar, etc.
2. **New Synapse Types**: NMDA, GABA_B, electrical synapses
3. **Advanced Memory Systems**: Working memory with capacity limits
4. **Complex Environments**: 3D worlds, continuous spaces
5. **Multi-agent Systems**: Multiple interacting agents
6. **Hardware Acceleration**: GPU support for large-scale simulations

## License

MIT License - See LICENSE file for details.

---

*This documentation is part of the NLM (熙然) research project. The framework is intended for scientific research and educational purposes.*
