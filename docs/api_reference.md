# NLM API Reference

## Core Classes

This section documents the main NLM classes and their public interfaces.

## Brain

### Overview

The `Brain` class is the central neural processing coordinator in NLM. It integrates all neural systems including neurons, synapses, memory, prediction, cognition, development, and neuromodulation.

### Class Definition

```cpp
class Brain {
public:
    // Construction and Destruction
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Copy operations (deleted)
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Move operations
    Brain(Brain&& other) noexcept;
    Brain& operator=(Brain&& other) noexcept;

    // Core Simulation
    bool initialize();
    void step(SimulationStep step);
    void step(SimulationStep step, Timestamp time);
    
    // Configuration and Status
    static void setGlobalLogger(std::shared_ptr<Logger> logger);
    void logStatus() const;
    
    // Brain Manipulation
    RegionId addRegion(const std::string& name);
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    
    // Access to Integrated Systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    DevelopmentSystem* getDevelopmentSystem();
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Plasticity Systems
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and Metrics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    float getExcitationInhibitionRatio() const;
    float getDevelopmentStage() const;
    void setDevelopmentStage(DevelopmentalStage stage);
    
    // Input/Output Processing
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Actions and Behavior
    std::unique_ptr<Action> produceAction();
    
    // Plasticity and Learning
    void applyNeuromodulation(const Neuromodulator& signal);
    void updatePlasticity();
    void develop();
    
    // State Management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Random Number Generation
    RandomGenerator* getRandomGenerator();

private:
    struct Impl;
    Impl* pImpl;
};

namespace nlm {
    using BrainHandle = std::shared_ptr<Brain>;
}
```

### Detailed Methods

#### Construction and Destruction

```cpp
// Constructor
// Parameters:
//   config: Shared pointer to configuration object
// Effects:
//   - Initializes random number generator
//   - Sets up integrated plasticity systems
//   - Configures neuromodulation systems
//   - Allocates memory for all integrated systems

// Destructor
// Effects:
//   - Cleans up all integrated systems
//   - Frees allocated memory
//   - Releases resources
```

#### Core Simulation

```cpp
bool Brain::initialize()

// Initializes the brain with current configuration
// Parameters:
//   None
// Returns:
//   true if initialization succeeded, false otherwise
// Effects:
//   - Creates brain regions with specified neuron counts
//   - Establishes synaptic connections
//   - Initializes all integrated systems (memory, prediction, cognition, etc.)
//   - Registers spike handlers for event-driven processing
//   - Configures checkpoint manager

void Brain::step(SimulationStep step)
void Brain::step(SimulationStep step, Timestamp time)

// Advances the brain simulation by one time step
// Parameters:
//   step: Current simulation step number
//   time: Current simulation time (optional, calculated if not provided)
// Effects:
//   - Processes delayed spike events
//   - Updates neuron membrane potentials (LIF dynamics)
//   - Detects and queues spike events
//   - Updates working memory
//   - Applies neuromodulation effects
//   - Applies plasticity rules (STDP, Hebbian)
//   - Updates episodic memory
//   - Updates prediction system
//   - Updates attention and concept formation
//   - Applies structural plasticity
//   - Replays important memories
//   - Applies development effects
//   - Manages checkpoints
```

#### Configuration and Status

```cpp
void Brain::logStatus() const

// Logs detailed information about current brain state
// Effects:
//   - Outputs statistics about regions, neurons, synapses
//   - Reports memory system status
//   - Shows neuromodulation levels
//   - Displays development stage
//   - Logs per-region details
```

#### Brain Manipulation

```cpp
RegionId Brain::addRegion(const std::string& name)

// Creates a new neural region
// Parameters:
//   name: Descriptive name for the region
// Returns:
//   Unique identifier for the new region
// Effects:
//   - Creates a NeuralRegion with specified ID and name
//   - Adds region to internal collection
//   - Returns region ID for future reference

NeuralRegion* Brain::getRegion(RegionId id)
NeuralRegion* Brain::getRegion(RegionId id) const

// Retrieves a specific region by ID
// Parameters:
//   id: Unique identifier of the region to retrieve
// Returns:
//   Pointer to the NeuralRegion, or nullptr if not found
// Effects:
//   - Searches through all regions for matching ID
//   - Returns pointer to region or nullptr
```

#### Access to Integrated Systems

```cpp
NeuralWorkingMemory* Brain::getWorkingMemory()

// Returns pointer to working memory system
// Returns:
//   Pointer to working memory, or nullptr if not initialized
// Effects:
//   - Returns internal pointer to working memory system
//   - No side effects

// Similar methods for all other integrated systems
```

#### Plasticity Systems

```cpp
STDP* Brain::getSTDP()
Hebbian* Brain::getHebbian()
StructuralPlasticity* Brain::getStructuralPlasticity()

// Returns pointers to plasticity system implementations
// Returns:
//   Pointers to respective plasticity systems
// Effects:
//   - Returns internal system pointers
//   - Allows direct configuration and access
```

#### Statistics and Metrics

```cpp
size_t Brain::getTotalNeuronCount() const
size_t Brain::getTotalSynapseCount() const

// Returns total counts of neurons and synapses across all regions
// Returns:
//   Total count of respective components
// Effects:
//   - Iterates through all regions and populations
//   - Accumulates counts from all components
//   - Returns summed totals

float Brain::getExcitationInhibitionRatio() const

// Calculates the E/I balance of the network
// Returns:
//   Excitation/Inhibition ratio, or infinity if no inhibition
// Effects:
//   - Iterates through all synapses
//   - Sums positive weights (excitatory)
//   - Sums absolute values of negative weights (inhibitory)
//   - Returns ratio or handles division by zero

DevelopmentalStage Brain::getDevelopmentStage() const
void Brain::setDevelopmentStage(DevelopmentalStage stage)

// Gets/sets the current developmental stage
// Returns/Sets:
//   Current developmental stage
// Effects:
//   - getDevelopmentStage: Returns internal stage
//   - setDevelopmentStage: Updates internal stage and affects plasticity
```

#### Input/Output Processing

```cpp
void Brain::receiveSensoryInput(const SensoryInput& input)

// Processes sensory input and injects into sensory neurons
// Parameters:
//   input: Sensory data containing vision, touch, internal signals
// Effects:
//   - Normalizes input values to appropriate ranges
//   - Distributes input across sensory neurons
//   - Stores relevant data in working memory
//   - Triggers neural processing

void Brain::injectCurrent(NeuronId neuron, MembranePotential current)

// Injects current into a specific neuron
// Parameters:
//   neuron: ID of neuron to receive current
//   current: Amount of current to inject (mV)
// Effects:
//   - Finds specified neuron in brain
//   - Injects current directly into neuron
//   - No return (current is consumed immediately)
```

#### Actions and Behavior

```cpp
std::unique_ptr<Action> Brain::produceAction()

// Selects and produces a motor action based on neural activity
// Returns:
//   Unique pointer to action, or Wait action if no activity
// Effects:
//   - Analyzes motor neuron group activity
//   - Selects action with highest activity level
//   - Creates and returns Action object
//   - Returns Wait action if activity below threshold
```

#### Plasticity and Learning

```cpp
void Brain::applyNeuromodulation(const Neuromodulator& signal)

// Applies neuromodulation effects to plasticity
// Parameters:
//   signal: Neuromodulator containing level and type
// Effects:
//   - Scales STDP learning rates based on signal level
//   - Modifies eligibility trace application
//   - Affects synaptic weight changes

void Brain::develop()

// Updates development system and structural plasticity
// Effects:
//   - Advances developmental stage if time permits
//   - Updates structural plasticity parameters
//   - Applies age-dependent plasticity changes
```

#### State Management

```cpp
void Brain::reset()

// Resets brain to initial state
// Effects:
//   - Resets simulation counters
//   - Resets neuron states
//   - Clears memory systems
//   - Resets neuromodulation states

bool Brain::save(const std::string& filepath) const
bool Brain::load(const std::string& filepath)

// Saves/loads complete brain state to/from file
// Parameters:
//   filepath: Path to checkpoint file
// Returns:
//   true if operation succeeded, false otherwise
// Effects:
//   - save: Serializes neuron states, synapse states, and metadata
//   - load: Deserializes and restores all state data
```

## Neuron

### Overview

The `Neuron` class represents a single biological neuron implementing the Leaky Integrate-and-Fire (LIF) model of neural dynamics. It handles membrane potential integration, spike generation, synaptic inputs, and plasticity.

### Class Definition

```cpp
class Neuron {
public:
    // Construction and Destruction
    explicit Neuron(NeuronId id);
    ~Neuron();
    
    // Copy operations (deleted)
    Neuron(const Neuron&) = delete;
    Neuron& operator=(const Neuron&) = delete;
    
    // Move operations
    Neuron(Neuron&& other) noexcept;
    Neuron& operator=(Neuron&& other) noexcept;

    // Identity and Classification
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State Access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane Potential Management
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold and Firing
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    void setRefractoryPeriod(uint32_t steps);
    void decrementRefractory();
    
    // Firing Rate
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    // LIF Parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    uint32_t getRefractoryPeriod() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    void setResetPotential(MembranePotential potential);
    
    // Spike Detection and History
    bool checkThreshold() const;
    float getLastSpikeTime() const;
    
    // LIF Step Function
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Synaptic Inputs
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void receiveModulatoryInput(MembranePotential amplitude);
    
    // External Inputs
    void injectCurrent(MembranePotential current);
    MembranePotential getTotalCurrent() const;
    void clearTotalCurrent();
    \n    // Spike History
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Synapse Management
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    const std::vector<SynapseHandle>& getIncomingSynapses() const;
    const std::vector<SynapseHandle>& getOutgoingSynapses() const;
    
    // Plasticity Flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Region/Population Membership
    void setRegionId(RegionId region);
    RegionId getRegionId() const;
    void setPopulationId(PopulationId population);
    PopulationId getPopulationId() const;
    
    // Simulation Step
    void step(Timestamp currentTime);
    
    // State Management
    void reset();
    void initializeRandom(RandomGenerator& rng);

private:
    struct Impl;
    Impl* pImpl;
};
```

### Detailed Methods

#### Construction and Destruction

```cpp
Neuron::Neuron(NeuronId id)

// Creates a new neuron with specified ID
// Parameters:
//   id: Unique identifier for the neuron
// Effects:
//   - Allocates implementation structure
//   - Sets neuron type to Internal by default
//   - Sets region and population IDs to invalid
//   - Initializes membrane potential to resting potential
//   - Initializes membrane conductance, threshold, etc.
```

#### State Access

```cpp
const NeuronState& Neuron::getState() const
NeuronState& Neuron::getState()

// Returns constant and non-constant references to neuron state
// Returns:
//   Constant reference: Read-only access
//   Non-constant reference: Read-write access
// Effects:
//   - Provides access to complete neuron state
//   - Allows inspection of membrane potential, threshold, etc.
```

#### Membrane Potential Management

```cpp
MembranePotential Neuron::getMembranePotential() const
void Neuron::setMembranePotential(MembranePotential potential)
void Neuron::addToMembranePotential(MembranePotential delta)

// Gets, sets, and modifies membrane potential
// Returns/Sets:
//   Current or new membrane potential (mV)
// Effects:
//   - getMembranePotential: Returns current potential
//   - setMembranePotential: Replaces potential
//   - addToMembranePotential: Modifies potential in place
```

#### Firing Control

```cpp
bool Neuron::isFiring() const
bool Neuron::isRefractory() const

// Checks firing state
// Returns:
//   true if neuron is currently firing, false otherwise
// true if neuron is in refractory period, false otherwise
// Effects:
//   - Reads firing state from neuron state
//   - Returns boolean state

void Neuron::setFiringState(FiringState state)
void Neuron::setRefractoryPeriod(uint32_t steps)
void Neuron::decrementRefractory()

// Manages refractory period
// Parameters:
//   state: New firing state to set
//   steps: Refractory period duration in simulation steps
// Effects:
//   - setFiringState: Updates internal firing state
//   - setRefractoryPeriod: Configures refractory duration
//   - decrementRefractory: Decrements remaining refractory time
```

#### LIF Dynamics

```cpp
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt)

// Performs one step of LIF dynamics
// Parameters:
//   currentTime: Current simulation time
//   dt: Time step duration (ms)
// Returns:
//   true if neuron fired this step, false otherwise
// Effects:
//   - Handles refractory period
//   - Updates membrane potential using LIF equation
//   - Applies spike-frequency adaptation
//   - Detects threshold crossing
//   - Generates spike if threshold reached
//   - Resets membrane potential on spike
//   - Enters refractory period
//   - Updates adaptation variable
```

#### Synaptic Inputs

```cpp
void Neuron::receiveExcitatoryInput(MembranePotential amplitude)
void Neuron::receiveInhibitoryInput(MembranePotential amplitude)

// Processes synaptic inputs
// Parameters:
//   amplitude: Synaptic input strength (mV)
// Effects:
//   - excitatory: Adds to membrane potential
//   - inhibitory: Subtracts from membrane potential
//   - Stored in synapticInput for LIF integration
```

#### External Inputs

```cpp
void Neuron::injectCurrent(MembranePotential current)
MembranePotential Neuron::getTotalCurrent() const
void Neuron::clearTotalCurrent()

// Manages external current injection
// Parameters:
//   current: External current to inject (mV)
// Returns:
//   Current total synaptic input (mV)
// Effects:
//   - injectCurrent: Adds to synaptic input
//   - getTotalCurrent: Returns accumulated input
//   - clearTotalCurrent: Resets input accumulation
```

#### Spike Management

```cpp
void Neuron::recordSpike(Timestamp timestamp)
const std::vector<Timestamp>& Neuron::getSpikeHistory() const
void Neuron::clearSpikeHistory()

// Manages spike history
// Effects:
//   - recordSpike: Appends timestamp to history
//   - getSpikeHistory: Returns reference to history
//   - clearSpikeHistory: Clears history vector
```

#### Synapse Management

```cpp
void Neuron::addIncomingSynapse(SynapseHandle handle)
void Neuron::addOutgoingSynapse(SynapseHandle handle)

// Manages synaptic connections
// Parameters:
//   handle: Synapse handle (ID or pointer)
// Effects:
//   - addIncomingSynapse: Adds to incoming synapse list
//   - addOutgoingSynapse: Adds to outgoing synapse list
```

#### Plasticity Control

```cpp
void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)

// Configures plasticity rules for this neuron
// Parameters:
//   hebbian: Enable Hebbian plasticity
//   stdp: Enable STDP plasticity
//   rewardModulated: Enable reward-modulated plasticity
// Effects:
//   - Sets plasticity flags accordingly
//   - Enables/disables specific plasticity mechanisms
```

#### State Management

```cpp
void Neuron::reset()
void Neuron::initializeRandom(RandomGenerator& rng)

// Resets or initializes neuron with random parameters
// Effects:
//   - reset: Returns to initial state (all defaults)
//   - initializeRandom: Sets biological parameters based on distributions
```

## Synapse

### Overview

The `Synapse` class represents a connection between two neurons that transmits electrical signals. It implements spike-timing-dependent plasticity (STDP) and Hebbian learning rules.

### Class Definition

```cpp
class Synapse {
public:
    // Construction and Destruction
    Synapse(NeuronId source, NeuronId destination);
    ~Synapse() = default;
    
    // Copy operations (deleted)
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    
    // Move operations
    Synapse(Synapse&& other) noexcept;
    Synapse& operator=(Synapse&& other) noexcept;

    // Identity and Properties
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    float getWeight() const;
    void setWeight(float weight);
    float getDelay() const;
    void setDelay(float delay);
    
    // Type and Classification
    SynapseType getType() const;
    void setType(SynapseType type);
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Plasticity Management
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    bool getPlasticityEnabled() const;
    
    // Spike History
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearSpikeHistory();
    
    // Eligibility Trace
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void addToEligibilityTrace(float delta);
    void decayEligibilityTrace(float factor);
    
    // Plasticity Application
    void applySTDP(const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  float timestep, float ltpWeight = 0.02f,
                  float ltdWeight = 0.015f, float tau = 20.0f);
    void applyHebbian(const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     float timestep, float learningRate = 0.01f);
    void applyRewardModulation(float reward, float predictionError,
                               float plasticityModifier = 1.0f);
    
    // State Management
    void step(Timestamp currentTime);
    void reset();
    void initializeRandom(RandomGenerator& rng);

private:
    struct Impl;
    Impl* pImpl;
};
```

### Detailed Methods

#### Construction and Destruction

```cpp
Synapse::Synapse(NeuronId source, NeuronId destination)

// Creates a new synapse between two neurons
// Parameters:
//   source: Neuron ID of presynaptic neuron
//   destination: Neuron ID of postsynaptic neuron
// Effects:
//   - Sets source and destination neuron IDs
//   - Initializes weight to default value
//   - Sets delay to default (1 simulation step)
//   - Sets type to Excitatory by default
//   - Enables both STDP and Hebbian plasticity
```

#### Identity and Properties

```cpp
NeuronId Synapse::getSourceNeuron() const
NeuronId Synapse::getDestinationNeuron() const

// Returns source and destination neuron IDs
// Returns:
//   Neuron ID of source or destination neuron
// Effects:
//   - Returns stored neuron IDs
//   - No side effects

float Synapse::getWeight() const
void Synapse::setWeight(float weight)

// Gets/sets synaptic weight
// Returns/Sets:
//   Current or new synaptic weight
// Effects:
//   - getWeight: Returns current weight
//   - setWeight: Updates weight with bounds checking

float Synapse::getDelay() const
void Synapse::setDelay(float delay)

// Gets/sets synaptic delay (in simulation steps)
// Returns/Sets:
//   Current or new delay
// Effects:
//   - getDelay: Returns stored delay
//   - setDelay: Updates delay with bounds checking
```

#### Type and Classification

```cpp
bool Synapse::isExcitatory() const
bool Synapse::isInhibitory() const

// Checks synapse type
// Returns:
//   true if synapse is excitatory, false if inhibitory
// true if synapse is inhibitory, false if excitatory
// Effects:
//   - Returns boolean based on synapse type
//   - Excitatory: weight > 0
//   - Inhibitory: weight < 0
```

#### Spike History

```cpp
void Synapse::recordPreSpike(Timestamp timestamp)
void Synapse::recordPostSpike(Timestamp timestamp)

// Records spike times for STDP
// Parameters:
//   timestamp: Time when spike occurred
// Effects:
//   - recordPreSpike: Appends to pre-synaptic spike history
//   - recordPostSpike: Appends to post-synaptic spike history
//   - Maintains maximum history size

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const
const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const

// Accesses spike history
// Returns:
//   References to pre-synaptic and post-synaptic spike history
// Effects:
//   - Returns read-only access to spike history
//   - History is used for STDP calculations
```

#### Eligibility Trace

```cpp
float Synapse::getEligibilityTrace() const
void Synapse::setEligibilityTrace(float trace)
void Synapse::addToEligibilityTrace(float delta)
void Synapse::decayEligibilityTrace(float factor)

// Manages eligibility trace for reward-modulated learning
// Returns/Sets:
//   Current eligibility trace value
//   New eligibility trace value
// Effects:
//   - getEligibilityTrace: Returns current trace
//   - setEligibilityTrace: Sets new trace value
//   - addToEligibilityTrace: Modifies trace
//   - decayEligibilityTrace: Decays trace by factor
```

#### Plasticity Application

```cpp
void Synapse::applySTDP(const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes,
                       float timestep, float ltpWeight, float ltdWeight,
                       float tau)

// Applies spike-timing dependent plasticity
// Parameters:
//   preSpikes: History of presynaptic spike times
//   postSpikes: History of postsynaptic spike times
//   timestep: Current simulation timestep
//   ltpWeight: Long-term potentiation weight (default: 0.02)
//   ltdWeight: Long-term depression weight (default: 0.015)
//   tau: Time constant for STDP traces (default: 20.0)
// Effects:
//   - Calculates weight changes based on spike timing
//   - Applies potentiation for pre-before-post spikes
//   - Applies depression for post-before-pre spikes
//   - Updates synaptic weight

void Synapse::applyHebbian(const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          float timestep, float learningRate)

// Applies Hebbian learning rule
// Parameters:
//   preSpikes: History of presynaptic spike times
//   postSpikes: History of postsynaptic spike times
//   timestep: Current simulation timestep
//   learningRate: Hebbian learning rate (default: 0.01)
// Effects:
//   - Calculates weight changes based on co-activity
//   - Strengthens synapses with correlated activity
//   - Updates synaptic weight

void Synapse::applyRewardModulation(float reward, float predictionError,
                                    float plasticityModifier)

// Applies reward-modulated plasticity
// Parameters:
//   reward: Received reward value
//   predictionError: Difference between expected and actual reward
//   plasticityModifier: Global plasticity modulation factor
// Effects:
//   - Scales eligibility trace by reward signal
//   - Updates synaptic weight based on reward prediction error
//   - Modulates learning rate based on neuromodulation
```

#### State Management

```cpp
void Synapse::step(Timestamp currentTime)

// Updates synapse state for one simulation step
// Parameters:
//   currentTime: Current simulation time
// Effects:
//   - Decays eligibility trace
//   - Updates any time-dependent state
//   - No weight changes in this simple version

void Synapse::reset()

// Resets synapse to initial state
// Effects:
//   - Resets weight to default
//   - Clears spike histories
//   - Resets eligibility trace

void Synapse::initializeRandom(RandomGenerator& rng)

// Initializes synapse with random biological parameters
// Parameters:
//   rng: Random number generator for reproducible results
// Effects:
//   - Sets random weight (typically 0.1-0.5 for excitation)
//   - Sets random delay (1-5 simulation steps)
//   - Sets random synapse type
//   - Resets all histories
```

## Core Systems

### Memory Systems

#### NeuralWorkingMemory
#### NeuralEpisodicMemory
#### NeuralAssociativeMemory

### Neuromodulation

#### Dopamine
#### Curiosity
#### Novelty
#### PredictionError

### Cognition

#### NeuralPlanner
#### ConceptFormation
#### AttentionalSelection

### Development

#### DevelopmentSystem
#### Synaptogenesis
#### Maturation
#### Pruning

### Environment

#### SimpleWorld
#### Observation
#### Action

### Performance

#### CheckpointSystem
#### EventQueue
#### MemoryPool
#### ParallelProcessing

This API reference provides detailed documentation for all NLM classes and their methods. For complete API documentation with parameter descriptions, return value details, and usage examples, see the generated documentation in the `docs/api_reference/` directory.

---

**API Reference Complete!** 📚

This reference covers all major NLM classes with detailed method signatures, parameters, return values, and effects. For the most up-to-date and complete API documentation, check the generated documentation files.

**Continue Exploring:**
- [Developer Guide](./developer_guide/) - Extending NLM
- [Examples](./examples/) - Working code examples
- [Architecture](docs/ARCHITECTURE.md) - System design and components