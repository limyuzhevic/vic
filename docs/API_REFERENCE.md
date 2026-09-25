# NLM API Reference

## Overview

This document provides complete API documentation for the NLM system, organized by class hierarchy and functionality. It covers both the C++ core API and the Python bindings.

## Table of Contents

1. [Core Classes](#core-classes)
2. [Brain Layer](#brain-layer)
3. [Dynamics Layer](#dynamics-layer)
4. [Plasticity Layer](#plasticity-layer)
5. [Development Layer](#development-layer)
6. [Neuromodulation Layer](#neuromodulation-layer)
7. [Memory Systems](#memory-systems)
8. [Prediction Layer](#prediction-layer)
9. [Cognitive Layer](#cognitive-layer)
10. [Sensory/Motor Layers](#sensorymotor-layers)
11. [Environment Layer](#environment-layer)
12. [Experiment Framework](#experiment-framework)
13. [Python API Bindings](#python-api-bindings)

---

## Core Classes

### Types

#### NeuronId
```cpp
// Strong type for neuron identifiers
typedef uint32_t NeuronId;

// Special values
static constexpr NeuronId INVALID_NEURON_ID = UINT32_MAX;
static constexpr NeuronId NULL_NEURON_ID = 0;
```

#### SynapseId
```cpp
typedef uint32_t SynapseId;

static constexpr SynapseId INVALID_SYNAPSE_ID = UINT32_MAX;
```

#### NeuralPopulationId
```cpp
typedef uint32_t NeuralPopulationId;
```

#### NeuralRegionId
```cpp
typedef uint32_t NeuralRegionId;
```

### Core Types

#### SimulationClock
```cpp
class SimulationClock {
public:
    // Time management
    Timestep getCurrentStep() const;
    double getCurrentTime() const;
    double getTimeStep() const;
    
    // Time control
    void setTimeStep(double dt);
    void advance(double time_increment);
    
    // Timing utilities
    bool isNextStepReady(double target_time) const;
    double timeUntilNextStep() const;
    
    // State persistence
    void saveState(std::ostream& stream) const;
    void loadState(std::istream& stream);
};
```

#### Logger
```cpp
class Logger {
public:
    enum class Level {
        TRACE = 0,
        DEBUG = 1,
        INFO = 2,
        WARNING = 3,
        ERROR = 4,
        CRITICAL = 5
    };
    
    // Logging interface
    template<typename... Args>
    void trace(const char* format, Args&&... args);
    
    template<typename... Args>
    void debug(const char* format, Args&&... args);
    
    template<typename... Args>
    void info(const char* format, Args&&... args);
    
    template<typename... Args>
    void warning(const char* format, Args&&... args);
    
    template<typename... Args>
    void error(const char* format, Args&&... args);
    
    // Configuration
    void setLevel(Level level);
    void addHandler(std::unique_ptr<LogHandler> handler);
    
    // Utilities
    static Logger& getInstance();
    void flush();
};
```

#### Config
```cpp
class Config {
public:
    // Configuration values
    enum class ValueType {
        INTEGER,
        FLOAT,
        STRING,
        BOOLEAN,
        VECTOR_FLOAT,
        VECTOR_INT
    };
    
    // Query configuration
    bool has(const std::string& path) const;
    bool isSet(const std::string& path) const;
    
    // Get values
    int getInt(const std::string& path) const;
    float getFloat(const std::string& path) const;
    std::string getString(const std::string& path) const;
    bool getBool(const std::string& path) const;
    std::vector<float> getFloatVector(const std::string& path) const;
    std::vector<int> getIntVector(const std::string& path) const;
    
    // Set values
    void set(const std::string& path, int value);
    void set(const std::string& path, float value);
    void set(const std::string& path, const std::string& value);
    void set(const std::string& path, bool value);
    void set(const std::string& path, const std::vector<float>& values);
    void set(const std::string& path, const std::vector<int>& values);
    
    // List keys
    std::vector<std::string> getKeys(const std::string& prefix = "") const;
    std::vector<std::string> getAllKeys() const;
    
    // File I/O
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Default configuration
    static Config createDefaultConfig();
};
```

#### Random
```cpp
class Random {
public:
    // Seed management
    static void setSeed(uint64_t seed);
    static uint64_t getSeed();
    
    // Random number generation
    static uint32_t uniform32();
    static uint64_t uniform64();
    static float uniformFloat(float min = 0.0f, float max = 1.0f);
    static double uniformDouble(double min = 0.0, double max = 1.0);
    
    static int uniformInt(int min = 0, int max = 100);
    
    // Gaussian distribution
    static float gaussian(float mean = 0.0f, float stddev = 1.0f);
    
    // Important for NLM
    static float gaussianWeight(float stddev = 0.1f);
    
    // Determinism for experiments
    static void reset();  // Resets to initial state
};
```

## Brain Layer

### Brain
```cpp
class Brain {
public:
    // Brain construction
    Brain(const Config& config);
    virtual ~Brain();
    
    // Core simulation
    void initialize();
    void step(Timestep step_number);
    void step(Timestep step_number, double time);
    void reset();
    
    // State persistence
    void save(const std::string& filename) const;
    void load(const std::string& filename);
    
    // Neuron management
    NeuronId addNeuron(const NeuronConfig& config);
    NeuralPopulationId addPopulation(const NeuralPopulationConfig& config);
    NeuralRegionId addRegion(const NeuralRegionConfig& config);
    
    // Synapse management
    SynapseId createSynapse(NeuronId pre_neuron, NeuronId post_neuron,
                           const SynapseConfig& config);
    
    // Configuration
    void configure(const Config& config);
    Config getConfig() const;
    
    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getPopulationCount() const;
    size_t getRegionCount() const;
    
    // Dynamic statistics
    size_t getFiringNeuronCount() const;
    size_t getTotalSpikeCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // Memory integration
    void receiveWorkingMemory(const float* memory_pattern, size_t length);
    float getWorkingMemoryLoad() const;
    
    // Development
    DevelopmentalStage getDevelopmentalStage() const;
    double getDevelopmentalProgress() const;
    
    // Access to components
    NeuralRegion& getRegion(NeuralRegionId region_id);
    NeuralPopulation& getPopulation(NeuralPopulationId pop_id);
    Neuron& getNeuron(NeuronId neuron_id);
    Synapse& getSynapse(SynapseId synapse_id);
    
    // Neuromodulation interface
    void applyNeuromodulation(float level, NeuromodulatorType type);
    
    // Prediction interface
    void receivePrediction(const PredictionInput& prediction);
    float getPredictionError() const;
    
    // Planning interface
    void setPlanningGoal(const float* goal_state, size_t dimension);
    float getPlanningConfidence() const;
    
    // Experiment interface
    void registerExperiment(const std::string& name,
                           const ExperimentCallback& callback);
    void runExperiments();
};
```

### Neuron
```cpp
class Neuron {
public:
    // Neuron properties
    NeuronId getId() const;
    float getMembranePotential() const;
    float getLastSpikeTime() const;
    int getRefractoryRemaining() const;
    
    // State
    void setMembranePotential(float v);
    void setMembraneRestingPotential(float v_rest);
    void setThreshold(float threshold);
    
    // Dynamics
    void update(float dt, const float* inputs);
    void reset();
    
    // Type information
    NeuronType getType() const;
    void setType(NeuronType type);
    
    // Plasticity
    void updatePlasticity(float dt);
    
    // Energy/budget
    float getEnergyCost() const;
    void setEnergyBudget(float budget);
};
```

### Synapse
```cpp
class Synapse {
public:
    // Synapse properties
    SynapseId getId() const;
    NeuronId getPreNeuron() const;
    NeuronId getPostNeuron() const;
    float getWeight() const;
    float getDelay() const;
    
    // Transmission
    void updateWeight(float new_weight);
    void setDelay(float delay_steps);
    
    // Plasticity state
    float getEligibilityTrace() const;
    void updateEligibilityTrace(float dt);
    void resetEligibilityTrace();
    
    // Transmission properties
    float getConductance() const;
    float getReversalPotential() const;
    
    // State
    bool isActive() const;
    float getLastTransmissionTime() const;
    
    // Learning
    void applySTDP(float delta_w);
    void applyHebbian(float correlation);
};
```

### NeuralPopulation
```cpp
class NeuralPopulation {
public:
    // Population properties
    NeuralPopulationId getId() const;
    size_t getNeuronCount() const;
    NeuronType getNeuronType() const;
    float getGlobalBias() const;
    
    // Configuration
    void setNeuronType(NeuronType type);
    void setGlobalBias(float bias);
    void setMaxFiringRate(float rate);
    
    // Dynamics
    void update(float dt);
    void updateConnections(float dt);
    
    // Statistics
    size_t getFiringCount() const;
    float getAverageFiringRate() const;
    float getStandardDeviation() const;
    
    // Access neurons
    Neuron& getNeuron(size_t index);
    const Neuron& getNeuron(size_t index) const;
    
    // Connectivity
    void setConnectivityMatrix(const float* matrix, size_t size);
    float getEffectiveConnectivity(size_t pre_idx, size_t post_idx) const;
};
```

### NeuralRegion
```cpp
class NeuralRegion {
public:
    // Region properties
    NeuralRegionId getId() const;
    std::string getName() const;
    size_t getPopulationCount() const;
    
    // Population management
    NeuralPopulationId addPopulation(const NeuralPopulationConfig& config);
    NeuralPopulation& getPopulation(NeuralPopulationId pop_id);
    
    // Regional dynamics
    void update(float dt);
    void updateAllPopulations(float dt);
    
    // Regional statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalFiringCount() const;
    float getRegionalFiringRate() const;
    
    // Regional connections
    void establishRegionToRegionConnection(
        NeuralRegionId target_region,
        const ConnectionPattern& pattern,
        const ConnectionParams& params
    );
    
    // Regional plasticity
    void updateRegionalPlasticity(float dt);
    void applyRegionalDevelopment(float dt);
};
```

## Dynamics Layer

### NeuralDynamics (Abstract Base)
```cpp
class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron state
    virtual void update(Neuron& neuron, float dt, const float* inputs) = 0;
    
    // Initialize neuron
    virtual void initialize(Neuron& neuron) = 0;
    
    // Reset neuron
    virtual void reset(Neuron& neuron) = 0;
    
    // Get dynamics name
    virtual const char* getName() const = 0;
    
    // Clone
    virtual std::unique_ptr<NeuralDynamics> clone() const = 0;
};
```

### LIFNeuralDynamics
```cpp
class LIFNeuralDynamics : public NeuralDynamics {
public:
    // Configuration
    void setThreshold(float v_thresh);
    void setRestingPotential(float v_rest);
    void setResetPotential(float v_reset);
    void setTimeConstant(float tau_m);
    void setRefractoryPeriod(float tau_ref);
    
    // Update implementation
    void update(Neuron& neuron, float dt, const float* inputs) override;
    void initialize(Neuron& neuron) override;
    void reset(Neuron& neuron) override;
    
    // Name
    const char* getName() const override;
    
    // Clone
    std::unique_ptr<NeuralDynamics> clone() const override;
};
```

### SpikeSystem
```cpp
class SpikeSystem {
public:
    // Spike detection and processing
    void detectSpikes(const std::vector<Neuron*>& neurons);
    void processSpikes(const std::vector<Neuron*>& neurons);
    
    // Spike queue management
    size_t getSpikeCount() const;
    Timestep getLastSpikeTime() const;
    
    // Event-driven processing
    void scheduleSpikes(std::vector<NeuronId>& spike_ids,
                       std::vector<Timestep>& spike_times);
    
    // Recording
    void recordSpikes(const std::string& filename) const;
    
    // Integration with plasticity
    void notifyPlasticity(float dt);
};
```

## Plasticity Layer

### PlasticityRule (Abstract Base)
```cpp
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Apply plasticity
    virtual float apply(Neuron& pre_neuron, Neuron& post_neuron,
                       Synapse& synapse, float pre_spike_time,
                       float post_spike_time, float dt) = 0;
    
    // Reset state
    virtual void reset() = 0;
    
    // Configuration
    virtual void configure(const Config& config) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Clone
    virtual std::unique_ptr<PlasticityRule> clone() const = 0;
};
```

### STDP
```cpp
class STDP : public PlasticityRule {
public:
    // Configuration
    void configure(const Config& config) override;
    
    // Update rule implementation
    float apply(Neuron& pre_neuron, Neuron& post_neuron,
               Synapse& synapse, float pre_spike_time,
               float post_spike_time, float dt) override;
    
    // Reset
    void reset() override;
    
    // Properties
    const char* getName() const override;
    std::unique_ptr<PlasticityRule> clone() const override;
    
    // Access to internal state
    float getLearningRate() const;
    float getTimeConstantPlus() const;
    float getTimeConstantMinus() const;
    
    // Statistical information
    float getAverageWeightChange() const;
    size_t getUpdateCount() const;
};
```

### Hebbian
```cpp
class Hebbian : public PlasticityRule {
public:
    void configure(const Config& config) override;
    float apply(Neuron& pre_neuron, Neuron& post_neuron,
               Synapse& synapse, float pre_spike_time,
               float post_spike_time, float dt) override;
    void reset() override;
    const char* getName() const override;
    std::unique_ptr<PlasticityRule> clone() const override;
};
```

### StructuralPlasticity
```cpp
class StructuralPlasticity {
public:
    // Synapse formation
    bool attemptFormation(NeuronId pre_neuron, NeuronId post_neuron,
                          float pre_activity, float post_activity,
                          float dt);
    
    // Synapse elimination
    bool attemptPruning(Synapse& synapse, float weight,
                       float activity_level, float dt);
    
    // Configuration
    void configure(const Config& config);
    
    // Statistics
    size_t getFormationCount() const;
    size_t getPruningCount() const;
    float getFormationRate() const;
    float getPruningRate() const;
    
    // Developmental modulation
    void updateDevelopmentalModulation(float developmental_stage);
};
```

## Development Layer

### DevelopmentSystem
```cpp
class DevelopmentSystem {
public:
    // Stage management
    void update(DevelopmentStage& current_stage, float dt);
    bool isStageComplete() const;
    double getStageProgress() const;
    
    // Plasticity modulation
    float getPlasticityMultiplier(PlasticityType type) const;
    float getCriticalPeriodFactor() const;
    
    // Configuration
    void configure(const Config& config);
    
    // Event handling
    void onSynaptogenesisCompleted();
    void onPruningCompleted();
    void onMaturationCompleted();
};
```

### Synaptogenesis
```cpp
class Synaptogenesis {
public:
    // Formation algorithm
    bool attemptFormation(NeuronId pre_neuron, NeuronId post_neuron,
                          float pre_activity, float post_activity,
                          float dt);
    
    // Competition management
    void manageCompetition(const std::vector<NeuronId>& neurons);
    
    // Developmental scaling
    void updateScalingFactor(float developmental_stage);
};
```

### Pruning
```cpp
class Pruning {
public:
    // Pruning algorithm
    bool shouldPrune(Synapse& synapse, float weight,
                    float activity_level, float dt);
    
    // Activity-dependent pruning
    void updateActivityThresholds(float global_activity);
    
    // Developmental control
    void setCriticalPeriodActive(bool active);
    bool isCriticalPeriodActive() const;
};
```

### Maturation
```cpp
class Maturation {
public:
    // Property development
    void updateMaturationParameters(float dt);
    bool isMature() const;
    
    // Property changes
    float getDevelopmentalBias(NeuronType type) const;
    float getRefinementFactor() const;
    
    // Completion detection
    void onPlasticityComplete();
    void onAllMaturationComplete();
};
```

## Neuromodulation Layer

### Neuromodulator (Abstract Base)
```cpp
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Modulator properties
    virtual const char* getName() const = 0;
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    virtual NeuromodulatorType getType() const = 0;
    
    // Effects
    virtual float getPlasticityFactor() const = 0;
    virtual float getExcitabilityFactor() const = 0;
    virtual float getLearningRateFactor() const = 0;
    
    // State updates
    virtual void update(float dt) = 0;
    virtual void reset() = 0;
    
    // Signal generation
    virtual void onReward(float reward, float prediction_error) = 0;
    virtual void onNovelty(float novelty) = 0;
    virtual void onError(float error) = 0;
    
    // Integration
    virtual void integrateWithPlasticity(PlasticityRule& rule) = 0;
    virtual void integrateWithNeuronDynamics(NeuronDynamics& dynamics) = 0;
};
```

### Dopamine
```cpp
class Dopamine : public Neuromodulator {
public:
    // Signal generation
    void signalReward(float reward, float prediction_error) override;
    void signalPredictionError(float error) override;
    void signalNovelty(float novelty) override;
    
    // Properties
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    NeuromodulatorType getType() const override;
    
    // Factors
    float getPlasticityFactor() const override;
    float getExcitabilityFactor() const override;
    float getLearningRateFactor() const override;
    
    // Updates
    void update(float dt) override;
    void reset() override;
    
    // State access
    float getPredictionError() const;
    float getTemporalDifference() const;
};
```

### Curiosity
```cpp
class Curiosity : public Neuromodulator {
public:
    // Exploration motivation
    void setExplorationTarget(const float* target_state, size_t dimension);
    void updateExplorationDrive(float dt);
    
    // Novelty detection
    void onNoveltyObserved(float novelty_score);
    float getCuriosityLevel() const;
    
    // Properties
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    NeuromodulatorType getType() const override;
    
    // Factors
    float getPlasticityFactor() const override;
    float getExcitabilityFactor() const override;
    float getLearningRateFactor() const override;
    
    // Updates
    void update(float dt) override;
    void reset() override;
}; 
```

## Memory Systems

### Memory (Abstract Base)
```cpp
class Memory {
public:
    virtual ~Memory() = default;
    
    // Memory capacity
    virtual size_t getCapacity() const = 0;
    virtual float getCurrentLoad() const = 0;
    
    // Encoding
    virtual bool encode(const float* pattern, size_t length,
                      float importance = 1.0f) = 0;
    
    // Retrieval
    virtual bool retrieve(float* output_pattern, size_t length,
                         const float* cue_pattern = nullptr) = 0;
    
    // Updating
    virtual void update(float dt) = 0;
    
    // State management
    virtual void reset() = 0;
    virtual void consolidate() = 0;
    
    // Metadata
    virtual float getStability() const = 0;
    virtual float getInterference() const = 0;
    virtual std::string getDescription() const = 0;
};
```

### NeuralWorkingMemory
```cpp
class NeuralWorkingMemory : public Memory {
public:
    // Storage
    bool encode(const float* pattern, size_t length,
               float importance = 1.0f) override;
    
    // Retrieval (pattern completion)
    bool retrieve(float* output_pattern, size_t length,
                  const float* cue_pattern = nullptr) override;
    
    // Dynamics
    void update(float dt) override;
    
    // Properties
    size_t getCapacity() const override;
    float getCurrentLoad() const override;
    float getStability() const override;
    
    // Competition
    void applyLateralInhibition();
    size_t getWinnerIndex() const;
    
    // Integration
    void integrateWithNeuralActivity(const float* neural_activity,
                                    size_t neuron_count);
};
```

### NeuralEpisodicMemory
```cpp
class NeuralEpisodicMemory : public Memory {
public:
    // Encoding with temporal/spatial context
    bool encode(const float* pattern, size_t length,
               float importance = 1.0f,
               float temporal_context = 0.0f,
               float spatial_context = 0.0f) override;
    
    // Pattern completion retrieval
    bool retrieve(float* output_pattern, size_t length,
                  const float* cue_pattern = nullptr) override;
    
    // Replay mechanism
    void scheduleReplay(Timestep start_time, float replay_speed = 1.0f);
    
    // Consolidation
    void consolidate() override;
    
    // Properties
    size_t getCapacity() const override;
    float getCurrentLoad() const override;
    float getStability() const override;
    
    // Statistics
    size_t getReplayCount() const;
    float getReplayEfficiency() const;
};
```

### NeuralAssociativeMemory
```cpp
class NeuralAssociativeMemory : public Memory {
public:
    // Pattern association
    bool encode(const float* pattern, size_t length,
               float importance = 1.0f,
               const float* associated_pattern = nullptr) override;
    
    // Association retrieval
    bool retrieve(float* output_pattern, size_t length,
                  const float* cue_pattern = nullptr) override;
    
    // Hebbian strengthening
    void strengthenAssociation(const float* pattern1,
                              const float* pattern2,
                              float strength_increment);
    
    // Properties
    size_t getCapacity() const override;
    float getCurrentLoad() const override;
    float getStability() const override;
    
    // Statistics
    size_t getAssociationCount() const;
    float getAverageAssociationStrength() const;
};
```

### ConceptFormation
```cpp
class ConceptFormation : public Memory {
public:
    // Pattern discovery without labels
    bool encode(const float* pattern, size_t length,
               float importance = 1.0f) override;
    
    // Prototype-based retrieval
    bool retrieve(float* output_pattern, size_t length,
                  const float* cue_pattern = nullptr) override;
    
    // Concept stability tracking
    float getConceptStability(const float* prototype) const;
    void updateConceptStability(const float* pattern, float weight);
    
    // Properties
    size_t getCapacity() const override;
    float getCurrentLoad() const override;
    float getStability() const override;
    
    // Concept statistics
    size_t getConceptCount() const;
    float getAverageConceptStability() const;
};
```

## Prediction Layer

### NeuralPrediction
```cpp
class NeuralPrediction {
public:
    // Temporal sequence learning
    bool learnSequence(const float* sequence_patterns,
                      size_t sequence_length,
                      float temporal_spacing);
    
    // Prediction generation
    float predictNext(const float* current_state,
                     float* predicted_state,
                     size_t state_dimension);
    
    // Prediction error computation
    float computePredictionError(const float* predicted_state,
                                const float* actual_state,
                                size_t state_dimension);
    
    // Multi-step prediction
    void predictFuture(const float* current_state,
                      float* future_states,
                      size_t state_dimension,
                      size_t prediction_horizon);
    
    // Confidence tracking
    float getPredictionConfidence() const;
    void updateConfidence(float error);
    
    // Integration with learning
    void onPredictionError(float error, float learning_rate);
};
```

### ActionConsequencePredictor
```cpp
class ActionConsequencePredictor {
public:
    // Action-effect association
    bool learnActionEffect(const float* action_state,
                          const float* effect_state,
                          float consequence_value);
    
    // Effect prediction
    float predictEffect(const float* action_state,
                       float* effect_state,
                       size_t state_dimension);
    
    // Value learning
    float getExpectedValue(const float* action_state) const;
    void updateExpectedValue(const float* action_state,
                           float new_value,
                           float learning_rate);
    
    // Exploration/exploitation
    float getExplorationValue(const float* state) const;
};  
```

## Cognitive Layer

### AttentionalSelection
```cpp
class AttentionalSelection {
public:
    // Competitive attention
    size_t selectAttention(const float* input_signals,
                         size_t signal_count,
                         float* selected_signal = nullptr);
    
    // Lateral inhibition
    void applyLateralInhibition(float* signals, size_t count);
    
    // Top-down and bottom-up bias
    void setTopDownBias(const float* bias_vector, size_t bias_count);
    void setBottomUpSalience(const float* salience_vector, size_t salience_count);
    
    // Winner-take-more dynamics
    void applyWinnerTakeMore(float* signals, size_t count);
    
    // Statistics
    size_t getSelectionHistorySize() const;
    float getSelectionStability() const;
    
    // Integration
    void integrateWithSensoryInput(const float* sensory_input,
                                  size_t input_size);
    void integrateWithMemory(const float* memory_pattern,
                            float memory_strength);
};
```

### NeuralPlanner
```cpp
class NeuralPlanner {
public:
    // Multi-step action planning
    bool planActionSequence(const float* current_state,
                           std::vector<float*>& action_sequence,
                           std::vector<float>& sequence_values,
                           size_t max_horizon);
    
    // Action evaluation
    float evaluateAction(const float* action_state,
                        const float* expected_outcome) const;
    
    // Plan adaptation
    void updatePlanBasedOnOutcome(const float* action_sequence,
                                 float outcome_value,
                                 float learning_rate);
    
    // Plan hierarchy
    void createSubGoal(const float* subgoal_state,
                      float subgoal_value,
                      size_t parent_index = -1);
    
    // Properties
    size_t getPlanHorizon() const;
    float getPlanConfidence() const;
    
    // Integration
    void integrateWithPrediction(const NeuralPrediction& prediction);
    void integrateWithMemory(const Memory& working_memory);
};
```

### SelfModel
```cpp
class SelfModel {
public:
    // Body schema learning
    void learnBodySchema(const float* proprioceptive_input,
                        const float* visual_input,
                        float prediction_error);
    
    // Agency detection
    float detectSelfGeneratedChange(const float* sensory_input,
                                   const float* motor_command) const;
    
    // Capability representation
    void updateCapability(const float* action_result,
                         float success_value,
                         float learning_rate);
    
    // Self vs environment distinction
    float computeSelfOtherDistinction(const float* self_signal,
                                     const float* other_signal) const;
    
    // Properties
    float getSelfModelConfidence() const;
    size_t getCapabilityCount() const;
    
    // Integration
    void integrateWithSensoryProcessing(const float* sensory_stream);
    void integrateWithMotorOutput(const float* motor_stream);
};
```

### SocialLearning
```cpp
class SocialLearning {
public:
    // Observation learning
    void observeOtherAgent(const float* other_action,
                          const float* other_outcome,
                          float social_value);
    
    // Imitation capability
    bool imitateAction(const float* observed_action,
                      float* output_action,
                      float threshold = 0.8f) const;
    
    // Simple communication
    void learnCommunicationSignal(const float* action,
                                 const float* signal);
    
    // Social value computation
    float computeSocialValue(const float* other_outcome,
                           const float* self_outcome) const;
    
    // Properties
    float getSocialLearningRate() const;
    size_t getObservedActionCount() const;
    
    // Integration
    void integrateWithMirrorSystem(const float* visual_input);
    void integrateWithTheoryOfMind(const float* other_state);
};
```

## Sensory/Motor Layers

### SensoryInput (Abstract Base)
```cpp
class SensoryInput {
public:
    virtual ~SensoryInput() = default;
    
    // Data acquisition
    virtual void update(float dt) = 0;
    
    // Data access
    virtual const float* getData() const = 0;
    virtual size_t getDataSize() const = 0;
    
    // Data format
    virtual SensoryModality getModality() const = 0;
    virtual void setDataFormat(DataFormat format) = 0;
    
    // Transformation
    virtual void normalize(const float* target_mean,
                          const float* target_stddev) = 0;
    
    // Recording
    virtual void record(const std::string& filename) const = 0;
};
```

### Vision
```cpp
class Vision : public SensoryInput {
public:
    // Image processing
    void setImageDimensions(int width, int height);
    void setFieldOfView(float fov_x, float fov_y);
    void setResolution(int width, int height);
    
    // Feature extraction
    void enableEdgeDetection(bool enable);
    void enableColorProcessing(bool enable);
    void enableObjectDetection(bool enable);
    
    // Data access
    const float* getData() const override;
    size_t getDataSize() const override;
    
    // Format
    SensoryModality getModality() const override;
    
    // Statistics
    float getMeanBrightness() const;
    float getContrast() const;
    size_t getEdgeCount() const;
};
```

### Audio
```cpp
class Audio : public SensoryInput {
public:
    // Audio processing
    void setFrequencyRange(float min_freq, float max_freq);
    void setSampleRate(int sample_rate);
    void setNoiseThreshold(float threshold);
    
    // Feature extraction
    void enableSpectralAnalysis(bool enable);
    void enableTemporalPatternDetection(bool enable);
    
    // Data access
    const float* getData() const override;
    size_t getDataSize() const override;
    
    // Format
    SensoryModality getModality() const override;
    
    // Statistics
    float getSpectralCentroid() const;
    float getZeroCrossingRate() const;
    size_t getNoteCount() const;
};
```

### InternalSignals
```cpp
class InternalSignals : public SensoryInput {
public:
    // Signal types
    void enableEnergySignals(bool enable);
    void enableHomeostasisSignals(bool enable);
    void enableStressSignals(bool enable);
    
    // Data access
    const float* getData() const override;
    size_t getDataSize() const override;
    
    // Format
    SensoryModality getModality() const override;
    
    // Signal access
    float getEnergyLevel() const;
    float getStressLevel() const;
    float getHomeostasisScore() const;
};
```

### MotorSystem
```cpp
class MotorSystem {
public:
    // Action generation
    void generateAction(const float* neural_activity,
                       float* action_output,
                       size_t action_dimension);
    
    // Decoding from neural activity
    void setDecoderParameters(const float* weights,
                             const float* biases,
                             size_t layer_count);
    
    // Action scaling
    void setActionScale(float scale_factor);
    void setActionRange(float min_value, float max_value);
    
    // Constraints
    void setMaxJointTorque(float max_torque);
    void setVelocityLimits(const float* limits, size_t joint_count);
    
    // Statistics
    float getActionComplexity() const;
    float getMovementSmoothness() const;
    
    // Recording
    void recordAction(const std::string& filename) const;
};
```

### Action
```cpp
class Action {
public:
    // Action representation
    Action(const float* parameters, size_t parameter_count);
    
    // Action properties
    size_t getParameterCount() const;
    float getParameter(size_t index) const;
    float getMagnitude() const;
    float getDuration() const;
    
    // Action types
    ActionType getActionType() const;
    void setActionType(ActionType type);
    
    // Validation
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Serialization
    void serialize(std::ostream& stream) const;
    void deserialize(std::istream& stream);
};  
```

## Environment Layer

### Environment (Abstract Base)
```cpp
class Environment {
public:
    virtual ~Environment() = default;
    
    // Environment update
    virtual void update(float dt) = 0;
    
    // State access
    virtual const float* getState() const = 0;
    virtual size_t getStateSize() const = 0;
    
    // Sensory interface
    virtual const float* getSensoryPercept() const = 0;
    virtual size_t getSensoryPerceptSize() const = 0;
    
    // Action interface
    virtual void applyAction(const float* action, float dt) = 0;
    virtual ActionType getLastAppliedAction() const = 0;
    
    // Entity management
    virtual void addEntity(const Entity& entity) = 0;
    virtual void removeEntity(size_t entity_id) = 0;
    virtual const Entity& getEntity(size_t entity_id) const = 0;
    
    // Reward system
    virtual float getReward() const = 0;
    virtual float getPredictionError() const = 0;
    
    // Configuration
    virtual void configure(const std::string& config_file) = 0;
    virtual void reset() = 0;
};
```

### SimpleWorld
```cpp
class SimpleWorld : public Environment {
public:
    // World configuration
    void configure(int width, int height,
                   int vision_width = 8, int vision_height = 8);
    void setRandomSeed(uint64_t seed);
    void setMaxEnergy(float max_energy);
    
    // State
    const float* getState() const override;
    size_t getStateSize() const override;
    
    // Sensory percepts
    const float* getSensoryPercept() const override;
    size_t getSensoryPerceptSize() const override;
    
    // Action application
    void applyAction(const float* action, float dt) override;
    ActionType getLastAppliedAction() const override;
    
    // Agent management
    void setAgentStart(float x, float y);
    const float* getAgentPosition() const;
    float getAgentEnergy() const;
    
    // World objects
    void addObject(const WorldObject& object);
    void removeObject(float x, float y);
    bool isValidPosition(float x, float y) const;
    
    // Properties
    int getWidth() const;
    int getHeight() const;
    float getSimulationTime() const;
    
    // Rewards
    float getReward() const override;
    float getPredictionError() const override;
};
```

### Observation
```cpp
class Observation {
public:
    // Observation components
    struct Component {
        SensoryModality modality;
        float* data;
        size_t data_size;
        float importance;
    };
    
    // Observation management
    void addSensoryInput(SensoryModality modality,
                        const float* data, size_t size);
    void setTemporalContext(float time_step);
    
    // Data access
    const float* getPercept() const;
    size_t getPerceptSize() const;
    
    // Processing
    void normalize(const float* means, const float* stddevs,
                   size_t component_count);
    void applyFilter(const float* kernel, size_t kernel_size);
    
    // Statistics
    float getSignalToNoiseRatio() const;
    size_t getActiveComponents() const;
    
    // Recording
    void record(const std::string& filename) const;
};
```

## Experiment Framework

### Experiment
```cpp
class Experiment {
public:
    // Experiment definition
    Experiment(const std::string& name,
               const ExperimentProtocol& protocol);
    
    // Execution
    void run(Brain& brain, Environment& environment);
    bool isRunning() const;
    bool isComplete() const;
    
    // Data collection
    const std::vector<ExperimentData>& getResults() const;
    void recordData(const std::string& variable_name,
                   const std::vector<float>& data);
    void recordData(const std::string& variable_name,
                   const std::vector<float>& data,
                   const std::vector<float>& timestamps);
    
    // Analysis
    float getMetric(const std::string& metric_name) const;
    std::vector<float> getTimeSeries(const std::string& variable_name) const;
    
    // Configuration
    void configure(const Config& config);
    Config getConfig() const;
    
    // Callbacks
    void setProgressCallback(const ProgressCallback& callback);
    void setCompletionCallback(const CompletionCallback& callback);
};
```

### ExperimentRunner
```cpp
class ExperimentRunner {
public:
    // Experiment management
    void addExperiment(std::unique_ptr<Experiment> experiment);
    void removeExperiment(const std::string& name);
    Experiment& getExperiment(const std::string& name);
    
    // Batch execution
    void runAllExperiments(Brain& brain, Environment& environment);
    void runExperiments(const std::vector<std::string>& experiment_names,
                       Brain& brain, Environment& environment);
    
    // Experiment status
    bool isExperimentComplete(const std::string& name) const;
    float getExperimentProgress(const std::string& name) const;
    
    // Results management
    void saveResults(const std::string& directory) const;
    void loadResults(const std::string& directory);
    
    // Configuration
    void configure(const Config& config);
    
    // Utilities
    std::vector<std::string> getExperimentNames() const;
    size_t getRunningExperimentCount() const;
};
```

### Metrics
```cpp
class Metrics {
public:
    // Performance metrics
    struct PerformanceStats {
        float mean_firing_rate;
        float firing_rate_variance;
        float spike_count;
        float energy_efficiency;
        float plasticity_efficiency;
    };
    
    // Memory metrics
    struct MemoryStats {
        float working_memory_load;
        float working_memory_capacity;
        float episodic_memory_stability;
        float associative_memory_strength;
        float concept_coherence;
    };
    
    // Learning metrics
    struct LearningStats {
        float stdp_weight_change;
        float hebbian_weight_change;
        float learning_acceleration;
        float retention_after_training;
        float generalization_score;
    };
    
    // Get current metrics
    PerformanceStats getPerformanceStats() const;
    MemoryStats getMemoryStats() const;
    LearningStats getLearningStats() const;
    
    // Comparison metrics
    float compareToPreviousRun(const Metrics& previous) const;
    std::vector<std::string> getImprovementAreas() const;
    
    // Recording
    void record(const std::string& filename) const;
    void logProgress(const std::string& message);
};
```

## Python API Bindings

### Module Overview

The NLM Python bindings provide high-level access to the C++ engine through a simplified interface:

```python
import pynlm

# Check available functions
print(dir(pynlm))
```

### Core Functions

#### Configuration
```python
# Create configuration
config = pynlm.createDefaultConfig()

# Query values
if config.has("brain.neuron_count"):
    neurons = config.getInt("brain.neuron_count")

# Modify values
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)

# File operations
config.loadFromFile("my_config.json")
config.saveToFile("my_config.json")

# Validation
validation = config.validate()
if not validation.is_valid:
    print(validation.errors)
```

#### Brain Creation and Management
```python
# Create brain
brain = pynlm.createBrain(config)

# Initialize
brain.initialize()

# Simulation
brain.step(step_number)           # Single step
brain.step(step_number, time)     # Step with time
brain.reset()                      # Reset state

# Statistics
neurons = brain.getTotalNeuronCount()
synapses = brain.getTotalSynapseCount()
firing = brain.getFiringNeuronCount()
rate = brain.getAverageFiringRate()

# State management
brain.save("checkpoint.bin")
brain.load("checkpoint.bin")
```

#### World Creation and Management
```python
# Create world
world = pynlm.createSimpleWorld()

# Configure world
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.setRandomSeed(42)
world.setMaxEnergy(100.0)

# Control
world.reset()
world.update(timestep)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(action, current_time)

# Access state
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
```

#### AgentBrain Interface
```python
# Create agent
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Sensory processing
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)

# Motor decoding
action = agent.decodeMotorCommand()

# Neuromodulation
reward = world.getSensoryPercept().getInternal()[0]
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(dt)
stage = brain.getDevelopmentalStage()

# Subsystem control
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Statistics
curiosity = agent.getCuriosityLevel()
novelty = agent.getNoveltyLevel()
error = agent.getPredictionError()
```

#### Complete Example
```python
import pynlm

def run_agent_simulation(num_steps=1000):
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run the simulation
result = run_agent_simulation(1000)
```

### Enumerations

#### Neuron Types
```python
pynlm.NeuronType.Excitatory     # +15 to -50 mV, promotes firing
pynlm.NeuronType.Inhibitory    # -70 to +20 mV, suppresses firing
pynlm.NeuronType.Sensory        # -60 to -40 mV, receives input
pynlm.NeuronType.Motor          # -50 to +10 mV, generates output
pynlm.NeuronType.Modulatory     # -60 to -45 mV, regulates learning
pynlm.NeuronType.Internal      # -65 to -55 mV, maintains baseline
```

#### Developmental Stages
```python
pynlm.DevelopmentalStage.Initial         # Birth - primitive
pynlm.DevelopmentalStage.CriticalPeriod # Rapid learning phase
pynlm.DevelopmentalStage.Maturation      # Refinement phase
pynlm.DevelopmentalStage.Adult          # Stable adult
pynlm.DevelopmentalStage.Aging          # Declining plasticity
```

#### Action Types
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Sleep
pynlm.ActionType.Groom
pynlm.ActionType.Explore
pynlm.ActionType.Wait
```

#### WorldObject Types
```python
pynlm.WorldObjectType.Empty      # No object
pynlm.WorldObjectType.Resource   # Consumable resource
pynlm.WorldObjectType.Hazard     # Causes damage
pynlm.WorldObjectType.Wall       # Physical barrier
pynlm.WorldObjectType.Marker     # Target for agent
```

### Configuration Presets

```python
# Preset 1: Minimal brain (fast)
minimal_config = pynlm.createDefaultConfig()
minimal_config.set("brain.neuron_count", 500)
minimal_config.set("plasticity.stdp.enable", False)
minimal_config.set("plasticity.hebbian.enable", False)

# Preset 2: Learning brain (complex)
learning_config = pynlm.createDefaultConfig()
learning_config.set("brain.neuron_count", 2000)
learning_config.set("plasticity.stdp.enable", True)
learning_config.set("plasticity.hebbian.enable", True)
learning_config.set("plasticity.structural.enable", True)
learning_config.set("neuromod.dopamine.scale", 2.0)

# Preset 3: High plasticity (development)
development_config = pynlm.createDefaultConfig()
development_config.set("brain.neuron_count", 3000)
for plasticity_type in ["stdp", "hebbian", "structural"]:
    development_config.set(f"plasticity.{plasticity_type}.enable", True)
    development_config.set(f"plasticity.{plasticity_type}.learning_rate", 0.1)
```

### Key Functions Summary

| Function | Purpose | Return Type |
|----------|---------|-------------|
| `pynlm.createDefaultConfig()` | Create default configuration | `Config` |
| `pynlm.createBrain(config)` | Create neural brain | `Brain` |
| `pynlm.createAgentBrain(brain)` | Create brain-world interface | `AgentBrain` |
| `pynlm.createSimpleWorld()` | Create 2D simulation world | `SimpleWorld` |
| `brain.initialize()` | Initialize brain state | `None` |
| `brain.step(step)` | Run one simulation step | `None` |
| `brain.reset()` | Reset brain to initial state | `None` |
| `brain.save(filename)` | Save brain state | `None` |
| `brain.load(filename)` | Load brain state | `None` |
| `world.update(dt)` | Update world by time delta | `None` |
| `world.applyAction(action, time)` | Apply action to world | `None` |
| `agent.processSensoryInput(percept)` | Give input to brain | `None` |
| `agent.decodeMotorCommand()` | Get action from brain | `Action` |
| `agent.applyRewardModulation(reward, error)` | Apply learning signal | `None` |
| `agent.updateDevelopment(dt)` | Advance development | `None` |

## API Usage Examples

### Example 1: Simple Brain Simulation
```python
import pynlm
import time

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
start_time = time.time()
for step in range(1000):
    brain.step(step)
    
end_time = time.time()
print(f"Completed 1000 steps in {end_time - start_time:.2f} seconds")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

### Example 2: Learning in Environment
```python
import pynlm

def train_agent(num_episodes=10):
    # Setup agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=6, visionHeight=6)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # Training loop
    for episode in range(num_episodes):
        world.reset()
        agent.initialize(world)
        
        for step in range(500):
            # Get observation
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Make decision
            brain.step(step)
            action = agent.decodeMotorCommand()
            
            # Take action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = world.getReward()
            error = agent.getPredictionError()
            agent.applyRewardModulation(reward, error)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Print progress
        print(f"Episode {episode + 1}/{num_episodes}")
        print(f"  Final reward: {reward:.3f}")
        print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    return brain, world, agent

# Run training
trained_brain, trained_world, trained_agent = train_agent(10)
```

### Example 3: Memory System Testing
```python
import pynlm

def test_memory_systems():
    # Create brain with memory subsystems
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Test working memory
    print("Testing working memory...")
    working_memory_pattern = [0.1] * 100
    brain.receiveWorkingMemory(working_memory_pattern, len(working_memory_pattern))
    print(f"Working memory load: {brain.getWorkingMemoryLoad():.3f}")
    
    # Test episodic memory (requires world interaction)
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Run episode to create episodic memory
    for step in range(200):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Check memory stats
    metrics = brain.getMetrics()
    print(f"Episodic memory stability: {metrics.episodic_memory_stability:.3f}")
    print(f"Working memory capacity: {metrics.working_memory_capacity:.3f}")
    
    return brain, world, agent

# Test memory systems
brain, world, agent = test_memory_systems()
```

## API Version Information

```python
# Check NLM version
print(f"NLM Version: {pynlm.__version__}")

# Check build information
print(f"Build Info: {pynlm.__build_info__}")
print(f"Build Date: {pynlm.__build_date__}")

# Check dependencies
print(f"C++ Version: {pynlm.__cxx_version__}")
print(f"Python Version: {pynlm.__python_version__}")
```

This comprehensive API reference covers all NLM classes, methods, and interfaces. For the most current information, always check the generated documentation and API headers in the source code.