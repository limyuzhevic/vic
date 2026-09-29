# NLM API Reference

This document provides a complete API reference for the NLM (Neural Learning Machine) framework, covering all public headers and their functions as of Phase 6 (FINAL INTEGRATION).

## Overview

NLM is an experimental computational brain project implementing Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation, synaptic delays, and multiple plasticity mechanisms.

## Core Headers Reference

### 1. Core Types (`src/core/Types/Types.hpp`)

**NeuronId**
```cpp
struct NeuronId {
    uint64_t value;
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr uint64_t index() const { return value; }
};
```

**Description:** Unique identifier for neurons with integer value and comparison operators.

**Usage Example:**
```cpp
nlm::NeuronId neuron1(1);
nlm::NeuronId neuron2(2);
if (neuron1 == neuron2) { /* false */ }
```

---

**SynapseId**
```cpp
struct SynapseId {
    uint64_t value;
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr uint64_t index() const { return value; }
};
```

**Description:** Unique identifier for synapses.

**Usage Example:**
```cpp
nlm::SynapseId syn1(1);
brain->addInterRegionConnection(region1, region2, 0.5f, 5, syn1);
```

---

**NeuronType (enum)**
```cpp
enum class NeuronType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};
```

**Description:** Neuron classification affecting connectivity and behavior.

**Usage Example:**
```cpp
neuron->setType(nlm::NeuronType::Sensory);
```

---

**PlasticityFlags**
```cpp
struct PlasticityFlags {
    bool hebbian : 1;
    bool stdp : 1;
    bool reward_modulated : 1;
    bool structural : 1;
    bool eligible : 1;
    constexpr PlasticityFlags() : hebbian(false), stdp(false),
                                  reward_modulated(false), structural(false),
                                  eligible(false) {}
};
```

**Description:** Bitfield flags for enabled plasticity mechanisms.

**Usage Example:**
```cpp
neuron->enablePlasticity(true, true, false);  // Hebbian + STDP
```

---

### 2. Configuration (`src/core/Config/Config.hpp`)

**Config Class**
```cpp
class Config {
public:
    Config();
    ~Config();
    
    // File I/O
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    
    // Value access
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Value setting
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Query
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    
    // Utility
    void remove(const std::string& key);
    void clear();
    std::string summary() const;
};
```

**Description:** Type-safe configuration management with JSON support and runtime persistence.

**Configuration Sources:**
- `ConfigSource::Default`: Default configuration values
- `ConfigSource::File`: Loaded from configuration file
- `ConfigSource::CommandLine`: From command line arguments
- `ConfigSource::Runtime`: Set at runtime

**Usage Examples:**

**Example 1: Basic Configuration**
```cpp
#include "src/core/Config/Config.hpp"

int main() {
    auto config = std::make_shared<nlm::Config>();
    
    // Load from JSON file
    if (!config->loadFromFile("brain_config.json")) {
        std::cerr << "Failed to load configuration" << std::endl;
        return 1;
    }
    
    // Get values with defaults
    int neuronCount = config->getOr<int>("neuron_count", 1000);
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    
    // Set values
    config->set("simulation_duration", 1000.0);
    config->set("debug_mode", false);
    
    // Save configuration
    config->saveToFile("updated_config.json");
    
    return 0;
}
```

**Example 2: Configuration with Validation**
```cpp
auto config = std::make_shared<nlm::Config>();

// Set required parameters
config->set("neuron_count", 500);
config->set("simulation_timestep", 0.001f);

// Validate configuration
if (!config->has("neuron_count") || config->getOr<int>("neuron_count", 0) <= 0) {
    throw nlm::ValidationException("Neuron count must be positive");
}

// Create brain with validated config
nlm::Brain brain(config);
brain.initialize();
```

---

### 3. Brain Architecture (`src/brain/Brain.hpp`)

**Brain Class**
```cpp
class Brain {
public:
    // Constructor
    explicit Brain(std::shared_ptr<Config> config);
    
    // Move semantics
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialization
    bool initialize();
    
    // Core simulation
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Input processing
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Output generation
    std::unique_ptr<class Action> produceAction();
    
    // Plasticity and development
    void updatePlasticity();
    void develop();
    
    // State management
    void reset();
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // System access
    NeuralRegion* addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system
    PredictionSystem* getPredictionSystem();
    
    // Cognition systems
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Configuration and utilities
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    void logStatus() const;
    
    // Memory leak detection
    static size_t getActiveInstanceCount();
    bool validateAndCheck() const;
};
```

**Description:** Central brain coordinator integrating all neural systems with real spiking dynamics.

**Key Methods:**

**Brain::initialize()**
```cpp
bool initialize();
```

**Description:** Initializes all internal systems including memory, prediction, cognition, neuromodulation, and plasticity mechanisms.

**Parameters:** None

**Returns:** True if initialization succeeded, false otherwise

**Throws:** ValidationException if configuration is invalid

**Brain::step()**
```cpp
void step(SimulationStep currentStep);
void step(SimulationStep currentStep, Timestamp currentTime);
```

**Description:** Performs one simulation step updating neural dynamics, plasticity, development, and integrated systems.

**Parameters:**
- `currentStep`: Current simulation step number
- `currentTime`: Optional timestamp for precise timing (second overload)

**Brain::receiveSensoryInput()**
```cpp
void receiveSensoryInput(const class SensoryInput& input);
```

**Description:** Injects sensory input into sensory neurons representing external stimuli.

**Parameters:**
- `input`: Sensory input data containing patterns and intensities

**Brain::produceAction()**
```cpp
std::unique_ptr<class Action> produceAction();
```

**Description:** Generates motor output based on motor neuron activity.

**Returns:** Unique pointer to Action object. May be null if no action is generated.

**Usage Example:**
```cpp
auto action = brain->produceAction();
if (action) {
    std::cout << "Action type: " << action->getType() << std::endl;
    std::cout << "Action value: " << action->getValue() << std::endl;
}
```

**Brain::save() and load()**
```cpp
bool save(const std::string& filepath) const;
bool load(const std::string& filepath);
```

**Description:** Checkpoint brain state to file or restore from checkpoint.

**Parameters:**
- `filepath`: Path to checkpoint file

**Returns:** True if operation succeeded, false otherwise

---

### 4. Neuron System (`src/brain/Neuron.hpp`)

**Neuron Class**
```cpp
class Neuron {
public:
    // Constructor
    explicit Neuron(NeuronId id);
    
    // Identity and type
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // State access
    const NeuronState& getState() const;
    NeuronState& getState();
    
    // Membrane potential
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    // Threshold management
    MembranePotential getThreshold() const;
    void setThreshold(MembranePotential threshold);
    
    // Firing state
    bool isFiring() const;
    bool isRefractory() const;
    void setFiringState(FiringState state);
    
    // LIF neuron parameters
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
    void setRestingPotential(MembranePotential potential);
    MembranePotential getRestingPotential() const;
    
    // Spike detection and LIF dynamics
    bool checkThreshold() const;
    bool stepLIF(Timestamp currentTime, TimestepDuration dt);
    
    // Input and output
    void receiveExcitatoryInput(MembranePotential amplitude);
    void receiveInhibitoryInput(MembranePotential amplitude);
    void injectCurrent(MembranePotential current);
    
    // Plasticity
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Memory
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    
    // Connectivity
    void addIncomingSynapse(SynapseHandle handle);
    void addOutgoingSynapse(SynapseHandle handle);
    
    // Lifecycle
    void step(Timestamp currentTime);
    void reset();
    void initializeRandom(RandomGenerator& rng);
};
```

**Description:** Implements Leaky Integrate-and-Fire (LIF) neuron model with realistic dynamics.

**Usage Example:**
```cpp
#include "src/brain/Neuron.hpp"
#include "src/core/Types/Types.hpp"

int main() {
    // Create neuron
    nlm::Neuron neuron(nlm::NeuronId(1));
    neuron.setType(nlm::NeuronType::Excitatory);
    
    // Configure neuron parameters
    neuron.setRestingPotential(-70.0f);
    neuron.setThreshold(-55.0f);
    neuron.setResetPotential(-70.0f);
    neuron.setLeakConductance(10.0f);
    neuron.setRefractoryPeriod(5);
    
    // Simulate neural dynamics
    float timestep = 0.001f;
    float time = 0.0f;
    
    for (int step = 0; step < 1000; ++step) {
        time += timestep;
        
        // Inject external current
        if (step < 100) {
            neuron.injectCurrent(150.0f);
        }
        
        // Update neuron state
        if (neuron.stepLIF(time, timestep)) {
            std::cout << "Neuron fired at time " << time << " ms" << std::endl;
        }
        
        // Check if neuron is firing
        if (neuron.isFiring()) {
            std::cout << "Neuron is currently firing" << std::endl;
        }
        
        // Log membrane potential
        float potential = neuron.getMembranePotential();
        std::cout << "Step " << step << ": Membrane potential = " 
                  << potential << " mV" << std::endl;
    }
    
    return 0;
}
```

---

### 5. Synapse System (`src/brain/Synapse.hpp`)

**Synapse Class**
```cpp
class Synapse {
public:
    // Constructor
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
    
    // Excitation/Inhibition
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Plasticity
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Learning traces
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Spike history for STDP
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Dynamics
    void step(Timestamp currentTime);
    void reset();
    void initializeRandom(RandomGenerator& rng);
};
```

**Description:** Implements synaptic connections with realistic dynamics, delays, and plasticity support.

**Usage Example:**
```cpp
#include "src/brain/Synapse.hpp"

int main() {
    // Create synapse from neuron 1 to neuron 2
    nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
    
    // Configure synapse
    synapse.setType(nlm::SynapseType::Excitatory);
    synapse.setWeight(0.5f);
    synapse.setDelay(5);
    
    // Check synapse properties
    std::cout << "Synapse is excitatory: " << synapse.isExcitatory() << std::endl;
    std::cout << "Current weight: " << synapse.getWeight() << std::endl;
    std::cout << "Delay: " << synapse.getDelay() << " ms" << std::endl;
    
    // Update synapse
    synapse.step(0.001f);
    
    return 0;
}
```

---

### 6. Plasticity Rules

#### PlasticityRule Base Class (`src/plasticity/PlasticityRule.hpp`)

```cpp
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    virtual void update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Rule control
    bool isEnabled() const;
    void setEnabled(bool enabled);
};
```

**STDP Implementation (`src/plasticity/STDP.hpp`)**

```cpp
class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);
    float getLTPWeight() const;
    void setLTDWeight(float weight);
    float getLTDWeight() const;
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
};
```

**Hebbian Implementation (`src/plasticity/Hebbian.hpp`)**

```cpp
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
};
```

**StructuralPlasticity (`src/plasticity/StructuralPlasticity.hpp`)**

```cpp
class StructuralPlasticity {
public:
    StructuralPlasticity();
    ~StructuralPlasticity();
    
    // Create new synapse
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    
    // Remove synapse
    bool removeSynapse(Brain* brain, SynapseId synapse);
    
    // Create new neuron
    NeuronId createNeuron(Brain* brain, NeuronType type);
    
    // Remove neuron
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Update structural plasticity
    void update(Brain* brain, class RandomGenerator& rng);
    
    // Get rates
    float getSynaptogenesisRate() const;
    void setSynaptogenesisRate(float rate);
    float getPruningRate() const;
    void setPruningRate(float rate);
};
```

---

### 7. Neuromodulation System

#### Neuromodulator Base Class (`src/neuromodulation/Neuromodulator.hpp`)

```cpp
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
};
```

**Dopamine (`src/neuromodulation/Dopamine.hpp`)**

```cpp
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward-based learning
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
};
```

**Curiosity (`src/neuromodulation/Curiosity.hpp`)**

```cpp
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level
    float getLevel() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
};
```

**Novelty (`src/neuromodulation/Novelty.hpp`)**

```cpp
class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Compute novelty from sensory input
    float computeNovelty(const std::vector<float>& sensoryInput) const;
    
    // Get novelty level
    float getLevel() const;
    
    // Update with new sensory input
    void update(const std::vector<float>& sensoryInput, TimestepDuration dt);
};
```

---

### 8. Memory Systems

#### NeuralWorkingMemory (`src/memory/NeuralWorkingMemory.hpp`)

```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();
    
    // Store active neural patterns
    void storePattern(const std::vector<float>& pattern, float confidence);
    
    // Retrieve patterns
    std::vector<float> retrievePattern(size_t index) const;
    
    // Pattern competition and selection
    size_t selectPattern() const;
    
    // Clear memory
    void clear();
    
    // Get statistics
    size_t getPatternCount() const;
    float getTotalConfidence() const;
};
```

#### NeuralEpisodicMemory (`src/memory/NeuralEpisodicMemory.hpp`)

```cpp
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    ~NeuralEpisodicMemory();
    
    // Store experience
    void storeExperience(SimulationStep step, const class Observation& input,
                         const class Action& action, float reward);
    
    // Retrieve experience
    bool retrieveExperience(SimulationStep step, class Observation& input,
                           class Action& action, float& reward) const;
    
    // Replay experiences
    void replay(SimulationStep startStep, SimulationStep endStep);
    
    // Get experience count
    size_t getExperienceCount() const;
};
```

#### NeuralAssociativeMemory (`src/memory/NeuralAssociativeMemory.hpp`)

```cpp
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    ~NeuralAssociativeMemory();
    
    // Store association
    void storeAssociation(const std::vector<float>& key,
                         const std::vector<float>& value);
    
    // Retrieve association
    bool retrieveAssociation(const std::vector<float>& key,
                            std::vector<float>& value) const;
    
    // Remove association
    void removeAssociation(const std::vector<float>& key);
    
    // Get statistics
    size_t getAssociationCount() const;
};
```

---

### 9. Prediction System

#### NeuralPrediction (`src/prediction/NeuralPrediction.hpp`)

```cpp
class NeuralPrediction {
public:
    NeuralPrediction();
    ~NeuralPrediction();
    
    // Generate prediction for sensory input
    std::vector<float> predict(const std::vector<float>& sensoryInput) const;
    
    // Update prediction based on actual input
    void update(const std::vector<float>& sensoryInput);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset prediction
    void reset();
};
```

#### PredictionSystem (`src/prediction/PredictionSystem.hpp`)

```cpp
class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Add prediction network
    void addPrediction(const std::string& name, NeuralPrediction* prediction);
    
    // Get prediction by name
    NeuralPrediction* getPrediction(const std::string& name);
    
    // Process prediction for input
    std::vector<float> process(const std::vector<float>& sensoryInput);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get prediction confidence
    float getPredictionConfidence() const;
};
```

---

### 10. Cognition Systems

#### NeuralPlanner (`src/cognition/NeuralPlanner.hpp`)

```cpp
class NeuralPlanner {
public:
    NeuralPlanner();
    ~NeuralPlanner();
    
    // Plan action sequence
    std::vector<class Action> plan(const class Observation& currentState,
                                 size_t maxSteps) const;
    
    // Evaluate action sequence
    float evaluateSequence(const std::vector<class Action>& sequence) const;
    
    // Set planning parameters
    void setMaxSteps(size_t maxSteps);
    void setExplorationRate(float rate);
};
```

#### ConceptFormation (`src/cognition/ConceptFormation.hpp`)

```cpp
class ConceptFormation {
public:
    ConceptFormation();
    ~ConceptFormation();
    
    // Learn concept from patterns
    void learnConcept(const std::vector<float>& pattern, const std::string& conceptName);
    
    // Classify pattern into concepts
    std::string classifyPattern(const std::vector<float>& pattern) const;
    
    // Get concept statistics
    size_t getConceptCount() const;
    float getConceptStrength(const std::string& conceptName) const;
};
```

#### AttentionalSelection (`src/cognition/AttentionalSelection.hpp`)

```cpp
class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Select attention focus
    void selectFocus(const std::vector<float>& sensoryInput);
    
    // Get attentional focus
    std::vector<float> getFocus() const;
    
    // Get attentional strength
    float getAttentionalStrength() const;
};
```

---

### 11. Development System

#### DevelopmentSystem (`src/development/DevelopmentSystem.hpp`)

```cpp
class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Update developmental stage
    void update(float dt);
    
    // Get current developmental stage
    DevelopmentalStage getStage() const;
    
    // Check if in critical period
    bool isCriticalPeriod() const;
    
    // Get developmental age
    float getAge() const;
};
```

---

### 12. Dynamics System

#### SpikeSystem (`src/dynamics/SpikeSystem.hpp`)

```cpp
class SpikeSystem {
public:
    SpikeSystem();
    ~SpikeSystem();
    
    // Process spike events
    void processSpikes(Timestamp currentTime);
    
    // Add spike event
    void addSpike(NeuronId neuron, Timestamp timestamp);
    
    // Clear spike events
    void clearSpikes();
    
    // Get spike count
    size_t getSpikeCount() const;
    
    // Enable/disable visualization
    void enableVisualization(bool enabled);
};
```

---

### 13. Motor System

#### Action (`src/motor/Action.hpp`)

```cpp
class Action {
public:
    Action();
    ~Action();
    
    // Get action type
    const std::string& getType() const;
    void setType(const std::string& type);
    
    // Get action value
    float getValue() const;
    void setValue(float value);
    
    // Get action parameters
    const std::vector<float>& getParameters() const;
    void setParameters(const std::vector<float>& parameters);
};
```

#### MotorSystem (`src/motor/MotorSystem.hpp`)

```cpp
class MotorSystem {
public:
    MotorSystem();
    ~MotorSystem();
    
    // Generate action from neural activity
    std::unique_ptr<Action> generateAction(const class NeuralRegion& motorRegion) const;
    
    // Execute action in world
    void executeAction(const Action& action, class World* world) const;
    
    // Get motor statistics
    size_t getActionCount() const;
    float getSuccessRate() const;
};
```

---

### 14. Environment System

#### SimpleWorld (`src/world/SimpleWorld.hpp`)

```cpp
class SimpleWorld {
public:
    SimpleWorld();
    ~SimpleWorld();
    
    // Configure world dimensions
    void configure(int width, int height, int visionWidth, int visionHeight);
    
    // Update world state
    float update(float dt);
    
    // Get sensory percept
    class SensoryPercept getSensoryPercept() const;
    
    // Apply motor command
    void applyMotorCommand(const class Action& action, Timestamp time);
    
    // Get agent body
    const class AgentBody& getAgentBody() const;
    
    // World properties
    int getWidth() const;
    int getHeight() const;
    float getSimulationTime() const;
};
```

## Error Handling

NLM provides a comprehensive exception hierarchy:

- `NLMException`: Base exception class
- `ValidationException`: Configuration validation failures
- `NullPointerException`: Null pointer dereferences
- `ConfigurationException`: Configuration file errors
- `FileIOError`: File I/O operations
- `PlasticityException`: Plasticity rule errors

All public methods throw exceptions for invalid parameters or runtime errors.

## Thread Safety

The NLM framework is generally thread-safe for concurrent reads. For concurrent writes, proper synchronization is required. The `Brain` class uses internal mutexes for thread-safe operations.

## Performance Considerations

1. **Use const references** for parameter passing
2. **Reserve memory** for containers that will be resized
3. **Use move semantics** for temporary objects
4. **Minimize locking** in multi-threaded contexts
5. **Use SIMD** where appropriate for numerical operations
6. **Profile regularly** to identify bottlenecks

## License

MIT License. See LICENSE file for details.
