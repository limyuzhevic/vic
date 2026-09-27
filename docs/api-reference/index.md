# API Reference

## Overview

The NLM (熙然) API provides comprehensive access to brain simulation capabilities through a well-defined C++ interface. This reference documents all public classes, methods, and types available for use in brain simulation experiments.

## API Organization

The NLM API is organized into logical layers:

```
nlm/
├── core/                 # Core types and utilities
├── brain/                # Neural computation
├── dynamics/             # Neural dynamics
├── plasticity/           # Learning rules
├── development/          # Developmental processes
├── neuromodulation/       # Biological modulators
├── memory/               # Memory systems
├── prediction/           # Predictive computations
├── cognition/            # Cognitive mechanisms
├── sensory/              # Sensory processing
├── motor/                # Motor control
├── environment/          # Environment simulation
├── experiments/          # Experiment framework
└── visualization/        # Visualization tools
```

## Core Types and Utilities

### nlm::Types

The `nlm::Types` namespace provides strongly-typed identifiers for all neural components:

#### NeuronId

```cpp
using NeuronId = uint64_t;
```

**Description:** Unique identifier for neurons. Ranges from 1 to 2^64-1.

#### SynapseId

```cpp
using SynapseId = uint64_t;
```

**Description:** Unique identifier for synapses.

#### RegionId

```cpp
using RegionId = uint32_t;
```

**Description:** Unique identifier for brain regions.

#### PopulationId

```cpp
using PopulationId = uint32_t;
```

**Description:** Unique identifier for neuron populations.

#### Delay

```cpp
using Delay = uint16_t;
```

**Description:** Synaptic transmission delay in milliseconds (0-65535 ms).

#### MembranePotential

```cpp
using MembranePotential = float;
```

**Description:** Membrane potential in millivolts (-100 to +100 mV).

#### FiringRate

```cpp
using FiringRate = float;
```

**Description:** Neuronal firing rate in spikes per second (0 to infinity).

#### SimulationStep

```cpp
typedef uint64_t SimulationStep;
```

**Description:** Simulation step counter.

#### Timestamp

```cpp
typedef double Timestamp;
```

**Description:** Absolute simulation time in seconds.

#### PlasticityFlags

```cpp
enum class PlasticityFlags : uint8_t {
    NONE = 0,
    LTP = 1 << 0,
    LTD = 1 << 1,
    STRUCTURAL = 1 << 2,
    REWARD_MODULATED = 1 << 3
};
```

**Description:** Flags for plasticity rule types.

#### NeuronType

```cpp
enum class NeuronType : uint8_t {
    EXCITATORY = 0,
    INHIBITORY = 1,
    SENSORY = 2,
    MOTOR = 3,
    INTERNEURON = 4,
    INTERNEURON = 5
};
```

**Description:** Functional classification of neurons.

#### NeuronState

```cpp
struct NeuronState {
    MembranePotential membranePotential;
    MembranePotential threshold;
    FiringState firingState;
    uint32_t refractoryRemaining;
    FiringRate firingRate;
    MembranePotential leakConductance;
};
```

**Description:** Complete state of a neuron.

#### FiringState

```cpp
enum class FiringState : uint8_t {
    RESTING = 0,
    DEPOLARIZING = 1,
    FIRING = 2,
    REFRACTORY = 3
};
```

**Description:** Current firing state of a neuron.

### nlm::Config

```cpp
class Config {
public:
    template<typename T>
    bool get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, T defaultValue) const;
    
    template<typename T>
    void set(const std::string& key, const T& value);
};
```

**Description:** Configuration management system for simulation parameters.

#### Methods

| Method | Description |
|--------|-------------|
| `get<T>(key)` | Get parameter value by key |
| `getOr<T>(key, default)` | Get parameter or return default |
| `set<T>(key, value)` | Set parameter value |

#### Configuration Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `simulation.timestep` | float | 0.001 | Simulation time step |
| `simulation.steps` | int | 1000 | Number of simulation steps |
| `simulation.random_seed` | uint64_t | 42 | Random seed for reproducibility |
| `brain.neuron_count` | int | 1024 | Total neuron count |
| `brain.region_count` | int | 4 | Number of brain regions |
| `memory.working_memory.capacity` | int | 256 | Working memory capacity |

### nlm::Logger

```cpp
class Logger {
public:
    enum class Level {
        TRACE, DEBUG, INFO, WARN, ERROR, FATAL
    };
    
    static void initialize(const Config& config);
    static void shutdown();
    
    template<typename... Args>
    static void log(Level level, const std::string& format, const Args&... args);
};
```

**Description:** Structured logging system for simulation debugging and monitoring.

### nlm::RandomGenerator

```cpp
class RandomGenerator {
public:
    RandomGenerator(uint64_t seed);
    uint64_t next();
    float uniformFloat(float min, float max);
    int uniformInt(int min, int max);
    float gaussianFloat(float mean, float stddev);
};
```

**Description:** Deterministic random number generator for reproducible experiments.

### nlm::SimulationClock

```cpp
class SimulationClock {
public:
    SimulationClock();
    void reset();
    Timestamp now() const;
    SimulationStep step() const;
    void advance(Timestamp delta);
};
```

**Description:** Time management for simulation control.

## Brain Layer API

### nlm::Neuron

```cpp
class Neuron {
public:
    explicit Neuron(NeuronId id);
    NeuronId getId() const;
    NeuronType getType() const;
    void setType(NeuronType type);
    
    MembranePotential getMembranePotential() const;
    void setMembranePotential(MembranePotential potential);
    void addToMembranePotential(MembranePotential delta);
    
    void setThreshold(MembranePotential threshold);
    MembranePotential getThreshold() const;
    
    FiringState getFiringState() const;
    void setFiringState(FiringState state);
    
    bool shouldFire() const;
    void handleSpike();
    
    uint32_t getRefractoryPeriod() const;
    void setRefractoryPeriod(uint32_t steps);
    
    void decrementRefractory();
    bool isRefractory() const;
    
    FiringRate getFiringRate() const;
    void setFiringRate(FiringRate rate);
    
    void setLeakConductance(MembranePotential conductance);
    MembranePotential getLeakConductance() const;
};
```

**Description:** Individual neuron representation with LIF dynamics.

### nlm::Synapse

```cpp
class Synapse {
public:
    Synapse(SynapseId id, NeuronId preNeuron, NeuronId postNeuron);
    
    SynapseId getId() const;
    NeuronId getPreNeuronId() const;
    NeuronId getPostNeuronId() const;
    
    float getWeight() const;
    void setWeight(float weight);
    
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    PlasticityFlags getPlasticityFlags() const;
    void setPlasticityFlags(PlasticityFlags flags);
    
    float getLastPostSpikeTime() const;
    void recordPostSpike(Timestamp time);
    
    float calculateUpdate(double timeDifference, PlasticityRule* rule);
};
```

**Description:** Synaptic connection with spike-timing-dependent plasticity.

### nlm::NeuralPopulation

```cpp
class NeuralPopulation {
public:
    NeuralPopulation(PopulationId id, NeuronType type);
    
    PopulationId getId() const;
    NeuronType getType() const;
    
    size_t getNeuronCount() const;
    void addNeuron(std::shared_ptr<Neuron> neuron);
    std::vector<Neuron*> getNeurons() const;
    
    void setProperties(float threshold, float leakConductance);
    float getAverageMembranePotential() const;
    size_t getFiringCount() const;
    
    void injectCurrent(MembranePotential current);
    std::vector<FiringRate> getFiringRates() const;
};
```

**Description:** Group of neurons with shared properties and dynamics.

### nlm::NeuralRegion

```cpp
class NeuralRegion {
public:
    NeuralRegion(RegionId id);
    
    RegionId getId() const;
    size_t getPopulationCount() const;
    
    void addPopulation(std::unique_ptr<NeuralPopulation> population);
    NeuralPopulation* getPopulation(PopulationId id);
    std::vector<NeuralPopulation*> getPopulations() const;
    
    void injectCurrentToPopulation(PopulationId id, MembranePotential current);
    std::vector<FiringRate> getRegionFiringRates() const;
    
    size_t getTotalNeuronCount() const;
    std::vector<Neuron*> getAllNeurons() const;
};
```

**Description:** Brain region containing multiple neuron populations.

### nlm::Brain

```cpp
class Brain {
public:
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    std::unique_ptr<Action> produceAction();
    
    void applyNeuromodulation(const Dopamine& signal);
    void applyNeuromodulation(const Novelty& signal);
    void applyNeuromodulation(const Curiosity& signal);
    void applyNeuromodulation(const PredictionError& signal);
    
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getSemanticMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getNeuralPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttentionalSelection();
    
    DevelopmentSystem* getDevelopmentSystem();
    
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
};
```

**Description:** Central coordinator of the neural system with integrated memory, prediction, cognition, and neuromodulation systems.

## Dynamics Layer API

### nlm::NeuralDynamics

```cpp
class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    virtual void integrate(const NeuralPopulation& population, Timestamp dt) = 0;
    virtual bool shouldFire(const Neuron& neuron) const = 0;
    virtual void handleSpike(Neuron& neuron) = 0;
    virtual std::string getModelName() const = 0;
};
```

**Description:** Abstract base class for neuron dynamics models.

### nlm::SpikeSystem

```cpp
class SpikeSystem {
public:
    SpikeSystem();
    
    void addSpikeEvent(Timestamp time, NeuronId neuronId);
    bool processEvents(Timestamp currentTime, std::vector<Neuron*>& neurons);
    void clearEvents();
    size_t getEventCount() const;
    Timestamp getNextEventTime() const;
    
    void setMaxEvents(size_t maxEvents);
    size_t getMaxEvents() const;
};
```

**Description:** Event-driven spike processing system for efficient neural simulation.

## Plasticity Layer API

### nlm::PlasticityRule

```cpp
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    virtual float calculateWeightChange(Synapse& synapse, double timeDifference) = 0;
    virtual std::string getRuleName() const = 0;
    virtual PlasticityFlags getPlasticityFlag() const = 0;
};
```

**Description:** Abstract base class for plasticity rules.

### nlm::STDP

```cpp
class STDP : public PlasticityRule {
public:
    STDP();
    
    void configure(float ltpWeight, float ltdWeight, float tau);
    
    float calculateWeightChange(Synapse& synapse, double timeDifference) override;
    std::string getRuleName() const override;
    PlasticityFlags getPlasticityFlag() const override;
    
    void setPostSpikeTime(NeuronId neuronId, Timestamp time);
    float calculatePostTraceContribution(NeuronId neuronId, Timestamp currentTime);
    float calculatePreTraceContribution(NeuronId neuronId, Timestamp currentTime);
};
```

**Description:** Spike-Timing-Dependent Plasticity implementation.

### nlm::Hebbian

```cpp
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    
    void configure(float learningRate);
    
    float calculateWeightChange(Synapse& synapse, double timeDifference) override;
    std::string getRuleName() const override;
    PlasticityFlags getPlasticityFlag() const override;
    
    void recordCoActivation(NeuronId preNeuron, NeuronId postNeuron);
    float calculateCoActivationStrength(NeuronId preNeuron, NeuronId postNeuron);
};
```

**Description:** Hebbian learning rule implementation.

### nlm::StructuralPlasticity

```cpp
class StructuralPlasticity : public PlasticityRule {
public:
    StructuralPlasticity();
    
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    
    float calculateWeightChange(Synapse& synapse, double timeDifference) override;
    std::string getRuleName() const override;
    PlasticityFlags getPlasticityFlag() const override;
    
    void processSynaptogenesis(const std::vector<Neuron*>& neurons);
    void processPruning(const std::vector<Neuron*>& neurons);
    size_t getCreatedSynapseCount() const;
    size_t getRemovedSynapseCount() const;
};
```

**Description:** Structural plasticity for synapse formation and removal.

## Development Layer API

### nlm::DevelopmentSystem

```cpp
class DevelopmentSystem {
public:
    DevelopmentSystem();
    
    void update(DevelopmentalStage stage, double age);
    float getPlasticityFactor(DevelopmentalStage stage, double age) const;
    DevelopmentalStage getCurrentStage(double age) const;
    
    void setPlasticitySchedule(const PlasticitySchedule& schedule);
    const PlasticitySchedule& getPlasticitySchedule() const;
    
    bool shouldTriggerMaturation(double age) const;
    MaturationStage getMaturationStage(double age) const;
};
```

**Description:** Orchestrates developmental stages and maturation processes.

## Neuromodulation Layer API

### nlm::Dopamine

```cpp
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    
    void configure(float strength, float rewardDelay);
    
    float computeSignal(const PredictionError& error) const;
    void applyToPlasticity(PlasticityRule& rule, float weight);
    
    std::string getModulatorName() const override;
    PlasticityFlags getPlasticityFlag() const override;
};
```

**Description:** Dopamine neuromodulation for reward processing and reinforcement learning.

### nlm::Curiosity

```cpp
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    
    void configure(float threshold, float explorationBonus);
    
    float computeSignal(const Novelty& novelty, const PredictionError& error) const;
    void applyToActionSelection(const std::vector<Neuron*>& neurons);
    
    std::string getModulatorName() const override;
    PlasticityFlags getPlasticityFlag() const override;
};
```

**Description:** Curiosity neuromodulation for exploration behavior.

### nlm::Novelty

```cpp
class Novelty : public Neuromodulator {
public:
    Novelty();
    
    void configure(float sensitivity, float decayRate);
    
    float computeSignal(const std::vector<float>& currentInput) const;
    void update(const std::vector<float>& currentInput);
    
    std::string getModulatorName() const override;
    PlasticityFlags getPlasticityFlag() const override;
};
```

**Description:** Novelty detection for change-based learning.

### nlm::PredictionError

```cpp
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    
    void configure(float learningRate, float confidenceThreshold);
    
    float computeSignal(const std::vector<float>& expected, 
                       const std::vector<float>& actual) const;
    std::vector<float> getPrediction(const std::vector<float>& input) const;
    
    std::string getModulatorName() const override;
    PlasticityFlags getPlasticityFlag() const override;
};
```

**Description:** Prediction error computation for predictive coding.

## Memory Layer API

### nlm::NeuralWorkingMemory

```cpp
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    
    void addItem(const NeuralPattern& item, float importance);
    std::vector<NeuralPattern> getCurrentItems() const;
    void clearItems();
    
    size_t getCapacity() const;
    void setCapacity(size_t capacity);
    
    float getTotalActivation() const;
    std::vector<NeuralPattern> getTopKItems(size_t k) const;
    
    void updateItemImportance(size_t index, float newImportance);
    void consolidateItems();
};
```

**Description:** Persistent activity system for temporary information storage.

### nlm::NeuralEpisodicMemory

```cpp
class NeuralEpisodicMemory {
public:
    NeuralEpisodicMemory();
    
    void storeExperience(const NeuralPattern& pattern, Timestamp timestamp);
    std::vector<NeuralPattern> retrieveExperience(Timestamp startTime, Timestamp endTime) const;
    
    void setReplayInterval(size_t steps);
    void triggerReplay();
    
    void setConsolidationThreshold(float threshold);
    void triggerConsolidation();
    
    size_t getExperienceCount() const;
    float getStorageEfficiency() const;
};
```

**Description:** Experience storage with pattern completion and replay capabilities.

### nlm::NeuralAssociativeMemory

```cpp
class NeuralAssociativeMemory {
public:
    NeuralAssociativeMemory();
    
    void storeAssociation(const NeuralPattern& key, const NeuralPattern& value);
    NeuralPattern retrieveAssociation(const NeuralPattern& key) const;
    
    float getAssociationStrength(const NeuralPattern& key) const;
    void updateAssociation(const NeuralPattern& key, const NeuralPattern& newValue);
    
    std::vector<NeuralPattern> getAllKeys() const;
    void clearAssociations();
};
```

**Description:** Experience-based pattern associations for memory recall.

## Prediction Layer API

### nlm::PredictionSystem

```cpp
class PredictionSystem {
public:
    PredictionSystem();
    
    void learnSequence(const std::vector<NeuralPattern>& sequence);
    std::vector<NeuralPattern> predictFuture(size_t steps) const;
    float computePredictionError(const NeuralPattern& expected, 
                               const NeuralPattern& actual) const;
    
    void setPredictionHorizon(size_t steps);
    size_t getPredictionHorizon() const;
    
    float getPredictionAccuracy() const;
    void reset();
};
```

**Description:** Temporal sequence learning and prediction system.

## Cognitive Layer API

### nlm::NeuralPlanner

```cpp
class NeuralPlanner {
public:
    NeuralPlanner();
    
    std::vector<NeuralPattern> evaluateActionSequences(const std::vector<NeuralPattern>& actions) const;
    NeuralPattern selectBestAction(const std::vector<NeuralPattern>& candidates) const;
    
    void setPlanningDepth(size_t depth);
    size_t getPlanningDepth() const;
    
    float getPlanningConfidence(const NeuralPattern& action) const;
    void updateActionQuality(const NeuralPattern& action, float quality);
};
```

**Description:** Predictive action selection using learned sequences.

### nlm::ConceptFormation

```cpp
class ConceptFormation {
public:
    ConceptFormation();
    
    void processPattern(const NeuralPattern& pattern);
    std::vector<NeuralPattern> getCurrentConcepts() const;
    
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    float calculateConceptStability(const NeuralPattern& concept) const;
    std::vector<NeuralPattern> getSimilarConcepts(const NeuralPattern& pattern) const;
};
```

**Description:** Pattern discovery and prototype formation.

### nlm::AttentionalSelection

```cpp
class AttentionalSelection {
public:
    AttentionalSelection();
    
    NeuralPattern selectAttention(const std::vector<NeuralPattern>& inputs, 
                                 const NeuralPattern& topDownBias) const;
    
    void setCompetitionStrength(float strength);
    float getCompetitionStrength() const;
    
    std::vector<float> getAttentionWeights(const std::vector<NeuralPattern>& inputs) const;
    void updateAttention(const NeuralPattern& attendedPattern, float strength);
};
```

**Description:** Competitive attention selection mechanism.

## Sensory Layer API

### nlm::SensoryInput

```cpp
class SensoryInput {
public:
    virtual ~SensoryInput() = default;
    
    virtual std::vector<float> process(const Observation& observation) const = 0;
    virtual std::string getInputType() const = 0;
};
```

**Description:** Abstract base class for sensory input processing.

### nlm::Vision

```cpp
class Vision : public SensoryInput {
public:
    Vision();
    
    std::vector<float> process(const Observation& observation) const override;
    std::string getInputType() const override;
    
    void setVisualField(float fieldOfView);
    void setResolution(int width, int height);
    
    std::vector<float> extractFeatures(const cv::Mat& image) const;
};
```

**Description:** Visual input processing system.

### nlm::Audio

```cpp
class Audio : public SensoryInput {
public:
    Audio();
    
    std::vector<float> process(const Observation& observation) const override;
    std::string getInputType() const override;
    
    void setFrequencyRange(float min, float max);
    void setGainControl(bool enabled);
    
    std::vector<float> extractFeatures(const std::vector<float>& audioSignal) const;
};
```

**Description:** Audio input processing system.

### nlm::InternalSignals

```cpp
class InternalSignals : public SensoryInput {
public:
    InternalSignals();
    
    std::vector<float> process(const Observation& observation) const override;
    std::string getInputType() const override;
    
    void addInternalState(const std::string& name, float value);
    float getInternalState(const std::string& name) const;
    
    std::vector<std::string> getAvailableStates() const;
};
```

**Description:** Internal state signals for self-monitoring.

## Motor Layer API

### nlm::Action

```cpp
class Action {
public:
    Action();
    
    void addMotorCommand(const std::string& command, float parameter);
    std::vector<std::pair<std::string, float>> getCommands() const;
    
    void setActionDuration(float duration);
    float getActionDuration() const;
    
    std::string toString() const;
    bool equals(const Action& other) const;
};
```

**Description:** Motor action representation.

### nlm::MotorSystem

```cpp
class MotorSystem {
public:
    MotorSystem();
    
    std::unique_ptr<Action> decodeMotorCommand(const std::vector<Neuron*>& motorNeurons) const;
    
    void setActionSelectionThreshold(float threshold);
    float getActionSelectionThreshold() const;
    
    std::vector<std::unique_ptr<Action>> getPossibleActions() const;
    void addActionCandidate(const Action& action, float probability);
};
```

**Description:** Neural-to-action conversion system.

## Environment Layer API

### nlm::Environment

```cpp
class Environment {
public:
    virtual ~Environment() = default;
    
    virtual void reset() = 0;
    virtual bool isCompleted() const = 0;
    virtual float getReward() const = 0;
    virtual std::vector<float> getObservation() const = 0;
    
    virtual void applyAction(const Action& action) = 0;
    virtual Timestamp getCurrentTime() const = 0;
    
    virtual std::string getEnvironmentType() const = 0;
    virtual std::string getDescription() const = 0;
};
```

**Description:** Abstract environment interface.

### nlm::SimpleWorld

```cpp
class SimpleWorld : public Environment {
public:
    SimpleWorld();
    
    void reset() override;
    bool isCompleted() const override;
    float getReward() const override;
    std::vector<float> getObservation() const override;
    
    void applyAction(const Action& action) override;
    Timestamp getCurrentTime() const override;
    
    std::string getEnvironmentType() const override;
    std::string getDescription() const override;
    
    void setGoalCompletion(float goal);
    float getGoalProgress() const;
    
    std::vector<std::string> getAvailableObjects() const;
    float getObjectDistance(const std::string& object) const;
};
```

**Description:** Simple 2D environment for testing and experimentation.

## Experiment Layer API

### nlm::Experiment

```cpp
class Experiment {
public:
    explicit Experiment(const Config& config);
    virtual ~Experiment() = default;
    
    virtual bool setup() = 0;
    virtual void runStep(SimulationStep step) = 0;
    virtual void analyze() = 0;
    virtual void cleanup() = 0;
    
    const Config& getConfig() const;
    std::string getName() const;
    
    virtual std::string getDescription() const = 0;
    virtual std::string getVersion() const = 0;
};
```

**Description:** Abstract base class for experiments.

### nlm::ExperimentRunner

```cpp
class ExperimentRunner {
public:
    ExperimentRunner();
    
    bool loadExperiment(const std::string& name, const Config& config);
    bool runExperiment(const std::string& name, size_t steps);
    bool saveResults(const std::string& outputFile) const;
    
    void setOutputFormat(ExportFormat format);
    ExportFormat getOutputFormat() const;
    
    std::vector<std::string> getAvailableExperiments() const;
    ExperimentInfo getExperimentInfo(const std::string& name) const;
};
```

**Description:** Manages experiment execution and result storage.

### nlm::Metrics

```cpp
class Metrics {
public:
    Metrics();
    
    void recordSpikeCount(size_t count);
    void recordMemoryUsage(float memory);
    void recordPerformance(float performance);
    void recordPlasticityEvents(size_t count);
    void recordNeuromodulationSignals(size_t count);
    
    float getAverageSpikeCount() const;
    float getAverageMemoryUsage() const;
    float getAveragePerformance() const;
    float getPlasticityRate() const;
    float getNeuromodulationRate() const;
    
    void reset();
    void exportMetrics(std::ostream& output, MetricFormat format);
};
```

**Description:** Data collection and analysis for experiment metrics.

## Visualization Layer API

### nlm::VisualizationInterface

```cpp
class VisualizationInterface {
public:
    virtual ~VisualizationInterface() = default;
    
    virtual void initialize(const Config& config) = 0;
    virtual void update(const std::vector<Neuron*>& neurons) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;
    
    virtual bool isEnabled() const = 0;
    virtual std::string getBackend() const = 0;
};
```

**Description:** Abstract base class for visualization backends.

## Plugin System API

### nlm::PluginInterface

```cpp
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    virtual std::string name() const = 0;
    virtual std::string version() const = 0;
    
    virtual void initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    
    virtual std::vector<std::string> getSupportedActions() const = 0;
    virtual bool executeAction(const std::string& action, 
                              const std::map<std::string, std::string>& parameters) = 0;
};
```

**Description:** Abstract interface for NLM plugins.

## Type Aliases

The following type aliases are available for convenience:

```cpp
namespace nlm {
    using NeuronId = uint64_t;
    using SynapseId = uint64_t;
    using RegionId = uint32_t;
    using PopulationId = uint32_t;
    using Delay = uint16_t;
    using MembranePotential = float;
    using FiringRate = float;
    using SimulationStep = uint64_t;
    using Timestamp = double;
    
    enum class PlasticityFlags : uint8_t { ... };
    enum class NeuronType : uint8_t { ... };
    enum class FiringState : uint8_t { ... };
    enum class DevelopmentalStage { ... };
    enum class ExportFormat { ... };
    enum class MetricFormat { ... };
}
```

## Error Handling

### nlm::NLMException

```cpp
class NLMException : public std::runtime_error {
public:
    explicit NLMException(NLMError error, const std::string& message);
    NLMError error() const;
    
private:
    NLMError error_;
};
```

**Description:** Base exception class for all NLM errors.

## Constants

### nlm::Constants

```cpp
namespace nlm::Constants {
    // Neural parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float TIME_CONSTANT = 20.0f;
    static constexpr float RESTING_POTENTIAL = -70.0f;
    static constexpr float THRESHOLD = -55.0f;
    
    // Simulation parameters
    static constexpr Timestamp DEFAULT_TIMESTEP = 0.001;
    static constexpr SimulationStep DEFAULT_MAX_STEPS = 10000;
    
    // Memory parameters
    static constexpr size_t DEFAULT_WORKING_MEMORY_CAPACITY = 256;
    static constexpr size_t DEFAULT_EPISODIC_MEMORY_SIZE = 1024;
    
    // Development parameters
    static constexpr double INITIAL_PLASTICITY = 1.0;
    static constexpr double ADULT_PLASTICITY = 0.1;
}
```

## Building the API Reference

The API reference is automatically generated from Doxygen-style comments in the source code. To regenerate this documentation:

```bash
# Install Doxygen and graphviz
cmake .. -DENABLE_DOXYGEN=ON
make docs

# Or manually
doxygen Doxyfile
```

## API Versioning

NLM follows semantic versioning for the API:

- **MAJOR version**: Backward incompatible changes
- **MINOR version**: New functionality, backward compatible
- **PATCH version**: Bug fixes, documentation updates

API changes are only made between MAJOR versions to ensure backward compatibility for existing users.

## License

NLM API Reference is licensed under the MIT License.

## Copyright

© 2026 NLM Development Team. All rights reserved.