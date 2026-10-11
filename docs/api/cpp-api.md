# NLM C++ API Documentation

**Complete reference for the C++ core API of the NLM neural simulation framework**

## Overview

This document provides comprehensive API reference for the C++ core API of NLM (Neural Learning Machine). The C++ API forms the foundation of the neural simulation engine, providing low-level access to neural computation, plasticity, neuromodulation, and cognitive systems.

## Core Architecture

### C++ Binding System

NLM's C++ API is a modern, header-only C++20 implementation that provides the complete neural simulation engine. Unlike the Python bindings, the C++ API offers direct access to all neural computation components without any abstraction layer.

```cpp
// Core architecture concept
#include "nlm/core/nlm.hpp"

// Create brain system
NLM::Brain brain(config);
brain.initialize();

// Neural computation
for (size_t step = 0; step < num_steps; ++step) {
    brain.step(step);
    // Neural dynamics happen here (native C++ engine)
}
```

## Module Structure

### Core Modules

The NLM C++ API is organized into the following modules:

#### Core Layer (`nlm/core/`)
- **Types**: Strongly-typed identifiers (NeuronId, SynapseId, etc.)
- **Config**: Configuration management system
- **Random**: Deterministic random number generator
- **Logger**: Structured logging system
- **SimulationClock**: Time management for simulation

#### Brain Layer (`nlm/brain/`)
- **Neuron**: Individual neuron representation (LIF dynamics)
- **Synapse**: Synaptic connection representation
- **NeuralPopulation**: Group of neurons with shared properties
- **NeuralRegion**: Brain region with multiple populations
- **Brain**: Central coordinator

#### Dynamics Layer (`nlm/dynamics/`)
- **NeuralDynamics**: Abstract base for neuron dynamics
- **SpikeSystem**: Event-driven spike processing

#### Plasticity Layer (`nlm/plasticity/`)
- **PlasticityRule**: Abstract base for plasticity rules
- **Hebbian**: Hebbian learning rule
- **STDP**: Spike-Timing-Dependent Plasticity
- **StructuralPlasticity**: Synapse formation/removal

#### Development Layer (`nlm/development/`)
- **DevelopmentSystem**: Orchestrates development
- **Synaptogenesis**: New synapse formation
- **Pruning**: Weak synapse removal
- **Maturation**: Adult property development

#### Neuromodulation Layer (`nlm/neuromodulation/`)
- **Neuromodulator**: Abstract base for neuromodulators
- **Dopamine**: Reward and reinforcement
- **Novelty**: Novelty detection
- **Curiosity**: Exploration motivation
- **PredictionError**: Error signals

#### Memory Layer (`nlm/memory/`)
- **WorkingMemory**: Persistent activity for temporary storage
- **EpisodicMemory**: Experience encoding in neural patterns
- **AssociativeMemory**: Experience-based associations
- **ConceptFormation**: Pattern discovery without labels
- **SpatialRepresentation**: Learned spatial relationships
- **TemporalRelation**: Before/after/sequence encoding

#### Prediction Layer (`nlm/prediction/`)
- **NeuralPrediction**: Temporal sequence learning
- **ActionConsequencePredictor**: Action effect prediction
- **PredictionErrorSignal**: Error computation for learning

#### Cognitive Layer (`nlm/cognition/`)
- **AttentionalSelection**: Competition-based attention
- **NeuralPlanner**: Predictive action selection
- **SelfModel**: Sensorimotor self-awareness
- **SocialLearning**: Observation and imitation

#### Sensory Layer (`nlm/sensory/`)
- **SensoryInput**: Base class for sensory data
- **Vision**: Visual input processing
- **Audio**: Audio input processing
- **InternalSignals**: Internal state signals

#### Motor Layer (`nlm/motor/`)
- **Action**: Action representation
- **MotorSystem**: Neural-to-action conversion

#### Environment Layer (`nlm/environment/`)
- **Environment**: Abstract environment interface
- **Observation**: Sensory observation wrapper
- **Action**: Action wrapper

#### Experiments Layer (`nlm/experiments/`)
- **Experiment**: Single experiment recording
- **ExperimentRunner**: Experiment execution
- **Metrics**: Data analysis
- **Phase4Experiment**: Phase 4 cognitive experiments

## Core Classes

### Brain Class (`nlm/brain/brain.hpp`)

The `Brain` class represents the central neural processing unit. It manages neurons, synapses, plasticity, neuromodulation, and cognitive systems.

#### Brain Class Definition

```cpp
class Brain {
public:
    // Constructors
    Brain() = default;
    Brain(const Config& config);
    
    // Core operations
    void initialize();
    void step(size_t step_number);
    void step(size_t step_number, double time);
    void reset();
    
    // State management
    void save(const std::string& filename) const;
    void load(const std::string& filename);
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getFiringNeuronCount() const;
    double getAverageFiringRate() const;
    double getExcitationInhibitionRatio() const;
    
    // Input/Output
    void receiveSensoryInput(const SensoryInput& input);
    Action produceAction();
    
    // Development
    void updateDevelopment(double timestep);
    DevelopmentStage getDevelopmentalStage() const;
    std::string getDevelopmentalStageName() const;
    
    // Regions
    NeuronRegionId addRegion(const std::string& name);
    NeuralRegion& getRegion(NeuronRegionId id);
    const NeuralRegion& getRegion(NeuronRegionId id) const;
    
    // Query
    bool isInitialized() const;
    bool isRunning() const;
};
```

#### Brain Class Methods

| Method | Description | Parameters | Returns |
|--------|-------------|------------|---------|
| `Brain(const Config& config)` | Constructor with configuration | `config: Config` | `Brain` |
| `initialize()` | Initialize brain state and systems | None | None |
| `step(size_t step_number)` | Process one simulation step | `step_number: size_t` | None |
| `step(size_t step_number, double time)` | Process step with timestamp | `step_number: size_t, time: double` | None |
| `reset()` | Reset brain to initial state | None | None |
| `save(const std::string& filename)` | Save brain state to file | `filename: std::string` | None |
| `load(const std::string& filename)` | Load brain state from file | `filename: std::string` | None |
| `receiveSensoryInput(const SensoryInput& input)` | Inject sensory data | `input: SensoryInput` | None |
| `produceAction()` | Get motor action from brain | None | `Action` |
| `getTotalNeuronCount()` | Get total number of neurons | None | `size_t` |
| `getTotalSynapseCount()` | Get total number of synapses | None | `size_t` |
| `getFiringNeuronCount()` | Get currently firing neurons | None | `size_t` |
| `getAverageFiringRate()` | Get average firing rate (Hz) | None | `double` |
| `getExcitationInhibitionRatio()` | Get E/I balance ratio | None | `double` |
| `updateDevelopment(double timestep)` | Update developmental stage | `timestep: double` | None |
| `getDevelopmentalStage()` | Get current developmental stage | None | `DevelopmentStage` |
| `getDevelopmentalStageName()` | Get developmental stage as string | None | `std::string` |
| `addRegion(const std::string& name)` | Add a new brain region | `name: std::string` | `NeuronRegionId` |
| `getRegion(NeuronRegionId id)` | Get brain region by ID | `id: NeuronRegionId` | `NeuralRegion&` |

### Neuron Class (`nlm/brain/neuron.hpp`)

The `Neuron` class represents individual neurons with Leaky Integrate-and-Fire dynamics and event-driven spike propagation.

#### Neuron Class Definition

```cpp
class Neuron {
public:
    // Constants
    static constexpr double DEFAULT_V_THRESH = -50.0;  // mV
    static constexpr double DEFAULT_V_REST = -70.0;   // mV
    static constexpr double DEFAULT_V_RESET = -75.0;  // mV
    static constexpr double DEFAULT_TAU_MEM = 20.0;   // ms
    static constexpr double DEFAULT_TAU_REF = 2.0;    // ms
    
    // Constructor
    Neuron(const NeuronConfig& config);
    
    // Core dynamics
    void update(double timestep);
    bool checkThreshold() const;
    void spike();
    void reset();
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
    
    // Properties
    double getVoltage() const;
    double getCurrent() const;
    double getThreshold() const;
    double getRestingPotential() const;
    double getResetPotential() const;
    double getMembraneTimeConstant() const;
    double getRefractoryPeriod() const;
    size_t getRefractoryRemaining() const;
    bool isSpiking() const;
    double getFiringRate() const;
    
    // Connectivity
    void addIncomingConnection(SynapseId synapse_id);
    void removeIncomingConnection(SynapseId synapse_id);
    void addOutgoingConnection(SynapseId synapse_id);
    void removeOutgoingConnection(SynapseId synapse_id);
    const std::vector<SynapseId>& getIncomingConnections() const;
    const std::vector<SynapseId>& getOutgoingConnections() const;
    
    // Identity
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
};
```

#### Neuron Constants

```cpp
enum class NeuronType {
    Excitatory,    // Glutamatergic neurons
    Inhibitory,    // GABAergic neurons
    Sensory,       // Sensory input neurons
    Motor,         // Motor output neurons
    Modulatory,    // Neuromodulatory neurons
    Internal       // Internal state neurons
};

struct NeuronConfig {
    NeuronId id;
    NeuronType type = NeuronType::Internal;
    double v_thresh = Neuron::DEFAULT_V_THRESH;  // mV
    double v_rest = Neuron::DEFAULT_V_REST;      // mV
    double v_reset = Neuron::DEFAULT_V_RESET;     // mV
    double tau_mem = Neuron::DEFAULT_TAU_MEM;     // ms
    double tau_ref = Neuron::DEFAULT_TAU_REF;     // ms
    std::vector<SynapseId> incoming_connections;
    std::vector<SynapseId> outgoing_connections;
};
```

### Synapse Class (`nlm/brain/synapse.hpp`)

The `Synapse` class represents synaptic connections between neurons with dynamic weights and delays.

#### Synapse Class Definition

```cpp
class Synapse {
public:
    // Constants
    static constexpr double DEFAULT_WEIGHT_MEAN = 0.5;
    static constexpr double DEFAULT_WEIGHT_STD = 0.1;
    static constexpr double DEFAULT_DELAY = 1.0;  // ms
    static constexpr double DEFAULT_CONDUCTANCE = 0.8;
    
    // Constructor
    Synapse(SynapseId id, NeuronId pre_id, NeuronId post_id);
    
    // Core dynamics
    void update(double timestep);
    void applyDelay();
    bool isDelayed() const;
    
    // State management
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
    
    // Plasticity
    void applySTDP(double pre_spike_time, double post_spike_time);
    void applyHebbian(double correlation);
    void applyRewardModulation(double reward_signal);
    
    // Properties
    SynapseId getId() const;
    NeuronId getPreNeuronId() const;
    NeuronId getPostNeuronId() const;
    double getWeight() const;
    void setWeight(double weight);
    double getDelay() const;
    void setDelay(double delay);
    double getConductance() const;
    void setConductance(double conductance);
    double getLastUpdateTime() const;
    bool isActive() const;
    void setActive(bool active);
    double getExcitationInhibition() const;
};
```

### NeuralPopulation Class (`nlm/brain/neural_population.hpp`)

The `NeuralPopulation` class represents groups of neurons with shared properties and collective dynamics.

#### NeuralPopulation Class Definition

```cpp
class NeuralPopulation {
public:
    // Constructor
    NeuralPopulation(NeuronPopulationId id, const std::string& name);
    
    // Neuron management
    NeuronId addNeuron(const NeuronConfig& config);
    void removeNeuron(NeuronId id);
    Neuron& getNeuron(NeuronId id);
    const Neuron& getNeuron(NeuronId id) const;
    size_t getNeuronCount() const;
    
    // Properties
    NeuronPopulationId getId() const;
    const std::string& getName() const;
    void setName(const std::string& name);
    NeuronType getNeuronType() const;
    void setNeuronType(NeuronType type);
    double getPopulationExcitement() const;
    double getPopulationInhibition() const;
    
    // Collective dynamics
    size_t getFiringCount() const;
    double getAverageFiringRate() const;
    double getTotalActivity() const;
    
    // Connectivity
    void addSynapse(const SynapseConfig& config);
    void removeSynapse(SynapseId id);
    Synapse& getSynapse(SynapseId id);
    const Synapse& getSynapse(SynapseId id) const;
    size_t getSynapseCount() const;
};
```

### NeuralRegion Class (`nlm/brain/neural_region.hpp`)

The `NeuralRegion` class represents brain regions containing multiple neural populations with regional properties and organization.

#### NeuralRegion Class Definition

```cpp
class NeuralRegion {
public:
    // Constructor
    NeuralRegion(NeuronRegionId id, const std::string& name);
    
    // Population management
    NeuronPopulationId addPopulation(const std::string& name);
    void removePopulation(NeuronPopulationId id);
    NeuralPopulation& getPopulation(NeuronPopulationId id);
    const NeuralPopulation& getPopulation(NeuronPopulationId id) const;
    size_t getPopulationCount() const;
    
    // Properties
    NeuronRegionId getId() const;
    const std::string& getName() const;
    void setName(const std::string& name);
    double getRegionalExcitement() const;
    double getRegionalInhibition() const;
    void setRegionalExcitement(double excitement);
    void setRegionalInhibition(double inhibition);
    
    // Collective properties
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getFiringNeuronCount() const;
    double getRegionalFiringRate() const;
};
```

### Config Class (`nlm/core/config.hpp`)

The `Config` class manages NLM system configuration parameters. It provides access to and modification of all brain, plasticity, neuromodulation, and development settings.

#### Config Class Definition

```cpp
class Config {
public:
    // Constructor
    Config();
    
    // Key management
    bool has(const std::string& key) const;
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    std::string getString(const std::string& key) const;
    int getInt(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(const std::string& prefix) const;
    
    // File I/O
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
    
    // Query
    bool isEmpty() const;
    size_t size() const;
};
```

#### Configuration Keys

##### Brain Configuration Keys

```cpp
// Brain configuration keys
namespace BrainConfigKeys {
    static constexpr const char* NEURON_COUNT = "brain.neuron_count";
    static constexpr const char* SYNAPSE_DENSITY = "brain.synapse_density";
    static constexpr const char* CONNECTION_PROBABILITY = "brain.connection_probability";
    static constexpr const char* INITIAL_WEIGHT_MEAN = "brain.initial_weight_mean";
    static constexpr const char* INITIAL_WEIGHT_STD = "brain.initial_weight_std";
    static constexpr const char* V_THRESH = "brain.v_thresh";
    static constexpr const char* V_REST = "brain.v_rest";
    static constexpr const char* V_RESET = "brain.v_reset";
    static constexpr const char* TAU_MEM = "brain.tau_mem";
    static constexpr const char* TAU_REF = "brain.tau_ref";
}
```

##### Plasticity Configuration Keys

```cpp
// Plasticity configuration keys
namespace PlasticityConfigKeys {
    static constexpr const char* STDP_ENABLE = "plasticity.stdp.enable";
    static constexpr const char* STDP_LEARNING_RATE = "plasticity.stdp.learning_rate";
    static constexpr const char* STDP_TAU_PLUS = "plasticity.stdp.tau_plus";
    static constexpr const char* STDP_TAU_MINUS = "plasticity.stdp.tau_minus";
    static constexpr const char* HEBBIAN_ENABLE = "plasticity.hebbian.enable";
    static constexpr const char* STRUCTURAL_ENABLE = "plasticity.structural.enable";
}
```

##### Neuromodulation Configuration Keys

```cpp
// Neuromodulation configuration keys
namespace NeuromodConfigKeys {
    static constexpr const char* DOPAMINE_SCALE = "neuromod.dopamine.scale";
    static constexpr const char* CURIOUSITY_ENABLE = "neuromod.curiosity.enable";
    static constexpr const char* NOVELTY_ENABLE = "neuromod.novelty.enable";
    static constexpr const char* PREDICTION_ERROR_SCALE = "neuromod.prediction_error.scale";
}
```

##### Development Configuration Keys

```cpp
// Development configuration keys
namespace DevelopmentConfigKeys {
    static constexpr const char* SYSTEM_ENABLE = "development.system.enable";
    static constexpr const char* PLASTICITY_RATE = "development.plasticity_rate";
    static constexpr const char* STAGE_DURATION = "development.stage_duration";
    static constexpr const char* CRITICAL_PERIOD = "development.critical_period";
}
```

### SpikeSystem Class (`nlm/dynamics/spike_system.hpp`)

The `SpikeSystem` class manages event-driven spike propagation and processing across the neural network.

#### SpikeSystem Class Definition

```cpp
class SpikeSystem {
public:
    // Constructor
    SpikeSystem();
    
    // Spike management
    void addSpike(NeuronId neuron_id, double time);
    void processSpikes(double current_time);
    void clearSpikes(double time_threshold);
    
    // Query
    size_t getSpikeCount() const;
    size_t getActiveSpikeCount() const;
    bool hasSpikes() const;
    std::vector<std::pair<NeuronId, double>> getSpikes() const;
    
    // Configuration
    void setMaxDelay(double max_delay);
    double getMaxDelay() const;
};
```

### PlasticityRule Class (`nlm/plasticity/plasticity_rule.hpp`)

The `PlasticityRule` class is the abstract base for all plasticity mechanisms in NLM.

#### PlasticityRule Class Definition

```cpp
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Core plasticity operation
    virtual void apply(NeuronId pre_id, NeuronId post_id, 
                      double pre_spike_time, double post_spike_time,
                      double weight) = 0;
    
    // Query
    virtual std::string getName() const = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};
```

### STDP Class (`nlm/plasticity/stdp.hpp`)

The `STDP` class implements Spike-Timing-Dependent Plasticity.

#### STDP Class Definition

```cpp
class STDP : public PlasticityRule {
public:
    // Constructor
    STDP(double learning_rate = 0.001, double tau_plus = 20.0, double tau_minus = 20.0);
    
    // PlasticityRule interface
    void apply(NeuronId pre_id, NeuronId post_id,
              double pre_spike_time, double post_spike_time,
              double weight) override;
    
    std::string getName() const override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // STDP-specific properties
    void setLearningRate(double rate);
    double getLearningRate() const;
    void setTimeConstants(double tau_plus, double tau_minus);
    void getTimeConstants(double& tau_plus, double& tau_minus) const;
};
```

### Hebbian Class (`nlm/plasticity/hebbian.hpp`)

The `Hebbian` class implements Hebbian learning rules.

#### Hebbian Class Definition

```cpp
class Hebbian : public PlasticityRule {
public:
    // Constructor
    Hebbian(double learning_rate = 0.001);
    
    // PlasticityRule interface
    void apply(NeuronId pre_id, NeuronId post_id,
              double pre_spike_time, double post_spike_time,
              double weight) override;
    
    std::string getName() const override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // Hebbian-specific properties
    void setLearningRate(double rate);
    double getLearningRate() const;
};
```

### Neuromodulator Class (`nlm/neuromodulation/neuromodulator.hpp`)

The `Neuromodulator` class is the abstract base for all neuromodulators in NLM.

#### Neuromodulator Class Definition

```cpp
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Core neuromodulation operation
    virtual void apply(NeuronId neuron_id, double signal_strength,
                      double weight, double time) = 0;
    
    // Query
    virtual std::string getName() const = 0;
    virtual bool isEnabled() const = 0;
    virtual void setEnabled(bool enabled) = 0;
};
```

### Dopamine Class (`nlm/neuromodulation/dopamine.hpp`)

The `Dopamine` class implements reward-based neuromodulation.

#### Dopamine Class Definition

```cpp
class Dopamine : public Neuromodulator {
public:
    // Constructor
    Dopamine(double scale = 1.0);
    
    // Neuromodulator interface
    void apply(NeuronId neuron_id, double signal_strength,
              double weight, double time) override;
    
    std::string getName() const override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // Dopamine-specific properties
    void setScale(double scale);
    double getScale() const;
};
```

### Novelty Class (`nlm/neuromodulation/novelty.hpp`)

The `Novelty` class implements novelty detection and exploration motivation.

#### Novelty Class Definition

```cpp
class Novelty : public Neuromodulator {
public:
    // Constructor
    Novelty(double threshold = 0.5, double scale = 1.0);
    
    // Neuromodulator interface
    void apply(NeuronId neuron_id, double signal_strength,
              double weight, double time) override;
    
    std::string getName() const override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // Novelty-specific properties
    void setThreshold(double threshold);
    double getThreshold() const;
    void setScale(double scale);
    double getScale() const;
    void updateNoveltySignal(double novelty_measure);
};
```

### Curiosity Class (`nlm/neuromodulation/curiosity.hpp`)

The `Curiosity` class implements exploration motivation and curiosity-driven behavior.

#### Curiosity Class Definition

```cpp
class Curiosity : public Neuromodulator {
public:
    // Constructor
    Curiosity(double exploration_rate = 0.1, double scale = 1.0);
    
    // Neuromodulator interface
    void apply(NeuronId neuron_id, double signal_strength,
              double weight, double time) override;
    
    std::string getName() const override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    
    // Curiosity-specific properties
    void setExplorationRate(double rate);
    double getExplorationRate() const;
    void setScale(double scale);
    double getScale() const;
    void updateExplorationMotivation(double environment_complexity);
};
```

### DevelopmentSystem Class (`nlm/development/development_system.hpp`)

The `DevelopmentSystem` class orchestrates developmental processes including synaptogenesis, pruning, and maturation.

#### DevelopmentSystem Class Definition

```cpp
class DevelopmentSystem {
public:
    // Constructor
    DevelopmentSystem();
    
    // Core development operations
    void update(double timestep);
    void triggerSynaptogenesis();
    void triggerPruning();
    void triggerMaturation();
    
    // Stage management
    void setDevelopmentalStage(DevelopmentStage stage);
    DevelopmentStage getDevelopmentalStage() const;
    std::string getDevelopmentalStageName() const;
    double getStageProgress() const;
    
    // Plasticity rate modulation
    void setPlasticityRate(double rate);
    double getPlasticityRate() const;
    
    // Critical period management
    bool isCriticalPeriod() const;
    double getCriticalPeriodRemaining() const;
    
    // Configuration
    void setStageDuration(DevelopmentStage stage, double duration);
    double getStageDuration(DevelopmentStage stage) const;
};
```

#### DevelopmentStage Enum

```cpp
enum class DevelopmentStage {
    Initial,           // Initial developmental state
    CriticalPeriod,    // Critical period for learning
    Maturation,        // Physical maturation
    Adult,             // Adult development
    Aging             // Aging development
};
```

### Action Class (`nlm/motor/action.hpp`)

The `Action` class represents neural motor commands and behavioral actions.

#### Action Class Definition

```cpp
class Action {
public:
    // Constructor
    Action();
    Action(ActionType type, double magnitude, const std::vector<double>& parameters);
    
    // Properties
    ActionType getType() const;
    void setType(ActionType type);
    
    double getMagnitude() const;
    void setMagnitude(double magnitude);
    
    const std::vector<double>& getParameters() const;
    void setParameters(const std::vector<double>& parameters);
    size_t getParameterCount() const;
    double getParameter(size_t index) const;
    void setParameter(size_t index, double value);
    
    // Query
    bool isValid() const;
    std::string toString() const;
    
    // Serialization
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### ActionType Enum

```cpp
enum class ActionType {
    MoveForward,       // Move forward
    MoveBackward,      // Move backward
    TurnLeft,          // Turn left
    TurnRight,         // Turn right
    MoveLeft,          // Move left
    MoveRight,         // Move right
    Jump,              // Jump
    Grab,              // Grab objects
    Release,           // Release objects
    Eat,               // Consume resources
    Sleep,             // Rest and recover
    Interact,          // Interact with environment
    Communicate,       // Send communication signals
    Explore,           // Explore environment
    Wait               // No action
};
```

### SensoryInput Class (`nlm/sensory/sensory_input.hpp`)

The `SensoryInput` class represents sensory data from the environment.

#### SensoryInput Class Definition

```cpp
class SensoryInput {
public:
    // Constructor
    SensoryInput();
    
    // Sensory modalities
    struct VisionData {
        double x, y;              // Position in visual field
        double intensity;         // Light intensity
        double color[3];          // RGB color values
        double object_id;         // ID of detected object
        double confidence;        // Detection confidence
    };
    
    struct TouchData {
        double x, y;              // Position of touch
        double pressure;          // Pressure magnitude
        double temperature;       // Temperature
        double pain_level;        // Pain level (0-1)
    };
    
    struct ProprioceptionData {
        double position_x, position_y;  // Body position
        double orientation;              // Body orientation (radians)
        double velocity_x, velocity_y;   // Velocity
        double angular_velocity;         // Angular velocity
    };
    
    struct InternalData {
        double energy;              // Current energy level
        double hunger;              // Hunger level (0-1)
        double thirst;              // Thirst level (0-1)
        double health;               // Health level (0-1)
        double mood;                 // Current mood (-1 to 1)
        std::vector<double> memory;  // Recent memory trace
    };
    
    // Setters
    void setVision(const VisionData& vision);
    void setTouch(const TouchData& touch);
    void setProprioception(const ProprioceptionData& proprio);
    void setInternal(const InternalData& internal);
    
    // Getters
    const VisionData& getVision() const;
    const TouchData& getTouch() const;
    const ProprioceptionData& getProprioception() const;
    const InternalData& getInternal() const;
    
    // Query
    bool hasVision() const;
    bool hasTouch() const;
    bool hasProprioception() const;
    bool hasInternal() const;
    
    // Utility
    void clear();
    std::string toString() const;
    
    // Serialization
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

### WorldObject Class (`nlm/world/world_object.hpp`)

The `WorldObject` class represents objects in the simulated environment.

#### WorldObject Class Definition

```cpp
class WorldObject {
public:
    // Constructor
    WorldObject();
    WorldObject(double x, double y, WorldObjectType type, double value = 0.0);
    
    // Properties
    double getX() const;
    void setX(double x);
    double getY() const;
    void setY(double y);
    WorldObjectType getType() const;
    void setType(WorldObjectType type);
    double getValue() const;
    void setValue(double value);
    double getSize() const;
    void setSize(double size);
    bool isActive() const;
    void setActive(bool active);
    
    // Query
    double getDistance(const WorldObject& other) const;
    double getDistance(double x, double y) const;
    bool intersects(const WorldObject& other) const;
    std::string toString() const;
    
    // Serialization
    void save(std::ostream& stream) const;
    void load(std::istream& stream);
};
```

#### WorldObjectType Enum

```cpp
enum class WorldObjectType {
    Empty,      // Empty space
    Resource,   // Consumable resources
    Hazard,     // Harmful objects
    Wall,       // Obstacles
    Marker,     // Location markers
    Predator,   // Threat objects
    Prey        // Prey objects
};
```

## Type Definitions

### Core Type Definitions

```cpp
typedef uint64_t NeuronId;
typedef uint64_t SynapseId;
typedef uint64_t NeuronPopulationId;
typedef uint64_t NeuronRegionId;
typedef uint64_t WorldObjectId;
typedef uint64_t ExperimentId;

typedef std::vector<NeuronId> NeuronIdList;
typedef std::vector<SynapseId> SynapseIdList;
typedef std::vector<NeuronPopulationId> NeuronPopulationIdList;
typedef std::vector<NeuronRegionId> NeuronRegionIdList;
```

### Configuration Type Definitions

```cpp
typedef std::variant<std::string, int, double, bool> ConfigValue;
typedef std::unordered_map<std::string, ConfigValue> ConfigMap;
typedef std::vector<std::string> ConfigKeyList;
```

## Configuration Functions

### Factory Functions

```cpp
namespace NLM {
    // Configuration creation
    Config createDefaultConfig();
    Config createAdvancedConfig();
    Config createSimpleConfig();
    Config createExperimentConfig();
    
    // Brain creation
    Brain createBrain(const Config& config);
    
    // Utility functions
    std::string getVersion();
    std::string getBuildInfo();
    void printSystemInfo();
}
```

## Example Usage

### Basic Brain Simulation

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>
#include <chrono>

int main() {
    // Create and initialize brain
    auto config = NLM::createDefaultConfig();
    auto brain = NLM::createBrain(config);
    brain.initialize();
    
    std::cout << "Brain initialized successfully!" << std::endl;
    std::cout << "Neurons: " << brain.getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain.getTotalSynapseCount() << std::endl;
    
    // Run simulation
    auto start_time = std::chrono::high_resolution_clock::now();
    for (size_t step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Log statistics
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << brain.getFiringNeuronCount() << " firing neurons, "
                      << brain.getAverageFiringRate() << " Hz average rate" << std::endl;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Simulation completed in " << duration.count() << " ms" << std::endl;
    
    return 0;
}
```

### Complete Agent Simulation

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>

void runCompleteSimulation(size_t num_steps = 1000) {
    // 1. Create configuration
    auto config = NLM::createDefaultConfig();
    
    // 2. Create brain and initialize
    auto brain = NLM::createBrain(config);
    brain.initialize();
    
    // 3. Create agent
    auto agent = NLM::createAgentBrain(brain);
    
    // 4. Create and configure world
    auto world = NLM::createSimpleWorld();
    world.configure(20, 20, 8, 8);
    world.reset();
    world.setAgentStart(10.0, 10.0);
    
    // 5. Initialize agent with world
    agent.initialize(world);
    
    // 6. Enable subsystems
    agent.enableRewardModulation(true);
    agent.enableStructuralPlasticity(true);
    agent.enableDevelopment(true);
    agent.enableCuriosity(true);
    
    // 7. Run simulation loop
    for (size_t step = 0; step < num_steps; ++step) {
        // Update world
        world.update(0.1);
        
        // Get sensory input
        auto percept = world.getSensoryPercept();
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Run brain step
        brain.step(step);
        
        // Decode action
        auto action = agent.decodeMotorCommand();
        
        // Apply action
        world.applyMotorCommand(action, world.getSimulationTime());
        
        // Apply reward modulation
        auto internal = percept.getInternal();
        double reward = internal.empty() ? 0.0 : internal[0];
        agent.applyRewardModulation(reward, 0.0);
        
        // Update development
        agent.updateDevelopment(0.1);
        
        // Print progress
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                      << brain.getFiringNeuronCount() << " firing neurons, "
                      << "Curiosity: " << agent.getCuriosityLevel() << ", "
                      << "Novelty: " << agent.getNoveltyLevel() << ", "
                      << "Dev Stage: " << agent.getDevelopmentalStageName() << std::endl;
        }
    }
}

int main() {
    runCompleteSimulation(1000);
    return 0;
}
```

### Advanced Configuration Example

```cpp
#include "nlm/core/nlm.hpp"
#include <fstream>

int main() {
    // Create advanced configuration
    auto config = NLM::createAdvancedConfig();
    
    // Customize specific parameters
    config.set("brain.neuron_count", 5000);
    config.set("brain.synapse_density", 0.15);
    config.set("plasticity.stdp.learning_rate", 0.002);
    config.set("plasticity.stdp.tau_plus", 25.0);
    config.set("plasticity.stdp.tau_minus", 25.0);
    config.set("neuromod.dopamine.scale", 1.5);
    config.set("neuromod.curiosity.enable", true);
    config.set("neuromod.novelty.enable", true);
    config.set("development.system.enable", true);
    config.set("development.plasticity_rate", 0.8);
    config.set("development.critical_period", 75.0);
    
    // Save configuration
    config.saveToFile("advanced_config.json");
    
    // Load configuration
    auto loaded_config = NLM::createDefaultConfig();
    loaded_config.loadFromFile("advanced_config.json");
    
    // Create brain with advanced configuration
    auto brain = NLM::createBrain(loaded_config);
    brain.initialize();
    
    std::cout << "Advanced brain created successfully!" << std::endl;
    
    return 0;
}
```

### World Simulation Example

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>

int main() {
    // Create and configure world
    auto world = NLM::createSimpleWorld();
    world.configure(30, 30, 10, 10);
    world.reset();
    world.setAgentStart(15.0, 15.0);
    world.setRandomSeed(12345);
    
    // Add interesting world objects
    world.addObject(NLM::WorldObject(5.0, 5.0, NLM::WorldObjectType::Resource, 2.0));
    world.addObject(NLM::WorldObject(25.0, 25.0, NLM::WorldObjectType::Resource, 2.0));
    world.addObject(NLM::WorldObject(15.0, 5.0, NLM::WorldObjectType::Hazard, -1.0));
    world.addObject(NLM::WorldObject(5.0, 15.0, NLM::WorldObjectType::Wall, 0.0));
    
    // Create brain
    auto config = NLM::createDefaultConfig();
    auto brain = NLM::createBrain(config);
    brain.initialize();
    
    // Create agent
    auto agent = NLM::createAgentBrain(brain);
    agent.initialize(world);
    
    // Run simulation
    auto world_start = world.getSimulationTime();
    while (world.getSimulationTime() - world_start < 10.0) {  // Run for 10 seconds
        world.update(0.1);
        
        auto percept = world.getSensoryPercept();
        agent.processSensoryInput(percept);
        
        brain.step(world.getSimulationTime());
        
        auto action = agent.decodeMotorCommand();
        world.applyMotorCommand(action, world.getSimulationTime());
        
        auto position = world.getAgentPosition();
        auto energy = world.getAgentEnergy();
        
        if (std::fmod(world.getSimulationTime(), 1.0) < 0.1) {
            std::cout << "Time " << world.getSimulationTime() << ": "
                      << "Position (" << position.x << ", " << position.y << "), "
                      << "Energy: " << energy << std::endl;
        }
    }
    
    std::cout << "World simulation completed!" << std::endl;
    
    return 0;
}
```

## Performance Optimization

### Multi-threaded Simulation

```cpp
#include "nlm/core/nlm.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadedBrainSimulator {
private:
    std::unique_ptr<NLM::Brain> brain_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool running_ = false;
    size_t current_step_ = 0;
    size_t max_steps_ = 0;
    std::vector<double> step_times_;
    
public:
    ThreadedBrainSimulator(const NLM::Config& config, size_t max_steps)
        : brain_(std::make_unique<NLM::Brain>(config)), max_steps_(max_steps) {}
    
    void start() {
        running_ = true;
        current_step_ = 0;
        step_times_.clear();
        
        std::thread(&ThreadedBrainSimulator::run, this).detach();
    }
    
    void stop() {
        std::unique_lock<std::mutex> lock(mutex_);
        running_ = false;
        cv_.notify_all();
    }
    
    bool isRunning() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return running_;
    }
    
    double getAverageStepTime() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (step_times_.empty()) return 0.0;
        double sum = 0.0;
        for (double time : step_times_) sum += time;
        return sum / step_times_.size();
    }
    
private:
    void run() {
        brain_->initialize();
        
        for (size_t step = 0; step < max_steps_ && isRunning(); ++step) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            {
                std::lock_guard<std::mutex> lock(mutex_);
                brain_->step(step);
                current_step_ = step;
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            
            std::lock_guard<std::mutex> lock(mutex_);
            step_times_.push_back(duration.count() / 1000.0);  // Convert to milliseconds
        }
    }
};
```

### Checkpoint and Recovery

```cpp
#include "nlm/core/nlm.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

class CheckpointManager {
private:
    std::unique_ptr<NLM::Brain> brain_;
    std::string checkpoint_dir_;
    
public:
    CheckpointManager(std::unique_ptr<NLM::Brain> brain, const std::string& checkpoint_dir)
        : brain_(std::move(brain)), checkpoint_dir_(checkpoint_dir) {
        std::filesystem::create_directories(checkpoint_dir_);
    }
    
    bool saveCheckpoint(size_t step) {
        std::string filename = checkpoint_dir_ + "/checkpoint_" + std::to_string(step) + ".bin";
        try {
            brain_->save(filename);
            std::cout << "Checkpoint saved: " << filename << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to save checkpoint: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool loadCheckpoint(size_t step) {
        std::string filename = checkpoint_dir_ + "/checkpoint_" + std::to_string(step) + ".bin";
        try {
            brain_->load(filename);
            std::cout << "Checkpoint loaded: " << filename << std::endl;
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Failed to load checkpoint: " << e.what() << std::endl;
            return false;
        }
    }
    
    NLM::Brain& getBrain() { return *brain_; }
};
```

## Error Handling

### Exception Types

```cpp
typedef std::runtime_error NLMBrainException;
typedef std::runtime_error NLMConfigException;
typedef std::runtime_error NLMWorldException;
typedef std::runtime_error NLMMemoryException;
```

### Error Handling Example

```cpp
#include "nlm/core/nlm.hpp"
#include <iostream>

int main() {
    try {
        // Create configuration
        auto config = NLM::createDefaultConfig();
        
        // Create brain
        auto brain = NLM::createBrain(config);
        brain.initialize();
        
        // Run simulation
        for (size_t step = 0; step < 1000; ++step) {
            brain.step(step);
        }
        
        std::cout << "Simulation completed successfully!" << std::endl;
        
    } catch (const NLMConfigException& e) {
        std::cerr << "Configuration error: " << e.what() << std::endl;
        return 1;
    } catch (const NLMBrainException& e) {
        std::cerr << "Brain error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
```

## Troubleshooting

### Common Issues and Solutions

#### "Brain is not responding" (No activity)

**Cause**: Brain may need time to "warm up" or may have insufficient input.

**Solution**:
1. Check that `brain.initialize()` was called before `brain.step()`
2. Increase simulation steps (50-100 steps for warm-up)
3. Ensure sensory input is connected to brain
4. Check that neurons have appropriate connections

#### "Memory issues with large brains"

**Cause**: Large brain configurations may exceed available memory.

**Solution**:
1. Use smaller `brain.neuron_count` (100-1000 for testing)
2. Reduce `brain.synapse_density` for sparser connections
3. Use `NLM::createSimpleConfig()` for resource-constrained environments
4. Monitor memory usage during simulation

#### "Non-deterministic behavior"

**Cause**: Lack of random seed for reproducible experiments.

**Solution**:
1. Set world random seed: `world.setRandomSeed(42)`
2. Use deterministic configurations
3. Enable reproducibility features in configuration

#### "Performance issues"

**Cause**: Inefficient simulation or overly complex configurations.

**Solution**:
1. Use `NLM::createSimpleConfig()` for testing
2. Reduce `brain.neuron_count` and `brain.synapse_density`
3. Use smaller simulation steps
4. Consider `createExperimentConfig()` for optimized performance

## Migration Guide

### From NLM v0.1.x to v0.2.0+

This guide covers migration from older NLM C++ API versions.

#### Configuration Changes

**v0.1.x**:
```cpp
// Old configuration style
Config config;
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);
```

**v0.2.0+**:
```cpp
// New configuration style
auto config = NLM::createDefaultConfig();
config.set("brain.neuron_count", 1000);
config.set("plasticity.stdp.enable", true);
```

#### API Changes

**v0.1.x**:
```cpp
// Old brain usage
Brain brain(config);
brain.initialize();
brain.runSimulation(100);
```

**v0.2.0+**:
```cpp
// New brain usage
auto brain = NLM::createBrain(config);
brain.initialize();
for (size_t step = 0; step < 100; ++step) {
    brain.step(step);
}
```

## Version Information

### Current API Version
v0.2.0

### Supported Standards
- C++20 (required)
- C++17 (minimum)

### Dependencies
- None (header-only library)
- Optional: Boost for additional functionality

### Build System
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

## References

### Related Documentation
- [NLM Python API](docs/api/python-api.md)
- [System Architecture](docs/architecture/architecture.md)
- [Configuration System](docs/api/configuration.md)
- [Performance Optimization](docs/best-practices/performance.md)
- [Tutorial: Getting Started](docs/tutorial/getting-started.md)

### Research Papers
- [Biological Fidelity Documentation](docs/SCIENCE.md)
- [Phase 6 Final Results](docs/PHASE6_FINAL_RESULTS.md)
- [Research Methods](docs/RESEARCH_METHODS.md)

## Changelog

### v0.2.0 (Current)
- Major API refactoring for better usability
- Added factory functions for configuration and brain creation
- Enhanced error handling and validation
- Improved documentation and examples

### v0.1.0
- Initial C++ API release
- Basic brain simulation capabilities
- Core plasticity mechanisms
- Simple world interaction

## Contact Information

### Issues and Feedback
- **GitHub Repository**: [nlm-project/nlm](https://github.com/nlm-project/nlm)
- **Documentation Issues**: Submit issues through repository
- **API Questions**: Check API reference documentation
- **Technical Support**: Consult system architecture documentation

### Support Channels
- **Documentation Hub**: Current documentation index
- **Quick Start Guide**: [easy_usage.md](easy_usage.md)
- **Technical Details**: [HOW_TO_USE.md](HOW_TO_USE.md)
- **Community Resources**: [SCIENCE.md](docs/SCIENCE.md)

---

*This documentation is continuously updated. Please report any issues or provide feedback to help improve the C++ API documentation.*

**Last Updated**: v0.2.0 - Comprehensive C++ API documentation enhancement**

---

## C++ API Quick Reference

### Core Functions

```cpp
// Configuration creation
NLM::Config createDefaultConfig();
NLM::Config createAdvancedConfig();
NLM::Config createSimpleConfig();
NLM::Config createExperimentConfig();

// Brain creation
NLM::Brain createBrain(const NLM::Config& config);

// Utility functions
std::string NLM::getVersion();
std::string NLM::getBuildInfo();
void NLM::printSystemInfo();
```

### Key Classes

#### Core Types
```cpp
typedef uint64_t NeuronId;
typedef uint64_t SynapseId;
typedef uint64_t NeuronPopulationId;
typedef uint64_t NeuronRegionId;
typedef uint64_t WorldObjectId;
```

#### Enumerations
```cpp
enum class NeuronType { Excitatory, Inhibitory, Sensory, Motor, Modulatory, Internal };
enum class DevelopmentStage { Initial, CriticalPeriod, Maturation, Adult, Aging };
enum class ActionType { MoveForward, MoveBackward, TurnLeft, TurnRight, Jump, Grab, Release, Eat, Sleep, Interact, Communicate, Explore, Wait };
enum class WorldObjectType { Empty, Resource, Hazard, Wall, Marker, Predator, Prey };
```

#### Brain Class Methods
```cpp
Brain(const Config& config);           // Constructor
void initialize();                      // Initialize brain
void step(size_t step_number);          // Process step
void step(size_t step_number, double time); // Process step with time
void reset();                           // Reset brain
void save(const std::string& filename); // Save state
void load(const std::string& filename); // Load state
```

#### Config Class Methods
```cpp
Config();                               // Constructor
bool has(const std::string& key);       // Check key exists
set(const std::string& key, const std::string& value); // Set string value
set(const std::string& key, int value); // Set int value
set(const std::string& key, double value); // Set double value
set(const std::string& key, bool value); // Set bool value
std::string getString(const std::string& key); // Get string
int getInt(const std::string& key);    // Get int
double getDouble(const std::string& key); // Get double
bool getBool(const std::string& key);  // Get bool
void saveToFile(const std::string& filename); // Save to file
void loadFromFile(const std::string& filename); // Load from file
```

#### Neuron Class Methods
```cpp
Neuron(const NeuronConfig& config);    // Constructor
void update(double timestep);           // Update dynamics
bool checkThreshold() const;            // Check if threshold crossed
void spike();                           // Process spike
void reset();                           // Reset neuron
double getVoltage() const;              // Get voltage
// ... (other methods)
```

### Usage Patterns

#### Pattern 1: Basic Simulation
```cpp
#include "nlm/core/nlm.hpp"

int main() {
    auto config = NLM::createDefaultConfig();
    auto brain = NLM::createBrain(config);
    brain.initialize();
    
    for (size_t step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    return 0;
}
```

#### Pattern 2: Configuration with File I/O
```cpp
#include "nlm/core/nlm.hpp"
#include <fstream>

int main() {
    auto config = NLM::createDefaultConfig();
    config.set("brain.neuron_count", 2000);
    
    config.saveToFile("config.json");
    
    auto new_config = NLM::createDefaultConfig();
    new_config.loadFromFile("config.json");
    
    auto brain = NLM::createBrain(new_config);
    brain.initialize();
    
    return 0;
}
```

This comprehensive C++ API documentation provides complete reference for NLM's C++ core API, including all core classes, configuration management, error handling, and usage examples. The API is designed for performance-critical applications with direct access to neural computation components.