# NLM C++ API Reference

## Overview

The NLM C++ API provides **complete access to the Phase 6 integrated artificial brain** through the NLM library. This API enables users to create, configure, and run simulations of brain-inspired cognitive systems with full access to all memory, cognition, and neuromodulation systems.

> **Key Feature:** The C++ API provides **unified access** to all Phase 6 cognitive systems including memory, prediction, planning, attention, and neuromodulation.

## API Structure

```cpp
NLM/
├── include/nlm/                     # Core API headers
│   ├── config/                     # Configuration classes
│   ├── core/                       # Core brain classes
│   ├── brain/                      # Brain management classes
│   ├── agent/                      # Agent system classes
│   ├── memory/                     # Memory system classes
│   ├── cognition/                  # Cognitive system classes
│   ├── neuromodulation/             # Neuromodulation system classes
│   ├── prediction/                  # Prediction system classes
│   ├── development/                 # Development system classes
│   └── performance/                 # Performance monitoring classes
├── src/                             # Implementation files
└── examples/                        # Example programs
```

## Core Classes and Headers

### Configuration Classes

#### nlm/config/Config.hpp

```cpp
namespace nlm {

class Config {
public:
    // Constructors
    Config();
    Config(const std::string& filename);
    
    // Configuration operations
    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);
    
    // Configuration retrieval
    std::string get(const std::string& key) const;
    int getInt(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    
    // Configuration validation
    bool validate() const;
    bool load(const std::string& filename);
    bool save(const std::string& filename) const;
    
    // Configuration inspection
    std::vector<std::string> getKeys() const;
    bool hasKey(const std::string& key) const;
    std::string toString() const;
};

} // namespace nlm
```

#### nlm/config/Phase6Config.hpp

```cpp
namespace nlm {

class Phase6Config : public Config {
public:
    // Constructors
    Phase6Config();
    Phase6Config(const std::string& filename);
    
    // Phase 6 configuration operations
    void enableMemorySystems(bool enable = true);
    void enableCognitiveSystems(bool enable = true);
    void enableNeuromodulation(bool enable = true);
    void enablePredictionSystems(bool enable = true);
    void enableDevelopment(bool enable = true);
    
    // Memory system configuration
    void setWorkingMemoryCapacity(int capacity);
    void setEpisodicMemoryCapacity(int capacity);
    void setAssociativeMemoryStrength(double strength);
    
    // Cognitive system configuration
    void setAttentionEnabled(bool enabled);
    void setPlanningEnabled(bool enabled);
    void setSelfModelEnabled(bool enabled);
    void setSocialLearningEnabled(bool enabled);
    void setConceptFormationEnabled(bool enabled);
    
    // Neuromodulation configuration
    void setDopamineEnabled(bool enabled);
    void setCuriosityEnabled(bool enabled);
    void setNoveltyEnabled(bool enabled);
    void setPredictionErrorEnabled(bool enabled);
    
    // Prediction system configuration
    void setTemporalPredictionEnabled(bool enabled);
    void setActionConsequencePredictionEnabled(bool enabled);
    
    // Development configuration
    void setDevelopmentStagesEnabled(bool enabled);
    void setAgeDependentPlasticity(bool enabled);
    
    // Helper methods
    bool isPhase6Integrated() const;
    std::string getPhase6Status() const;
};

} // namespace nlm
```

#### nlm/config/AdvancedConfig.hpp

```cpp
namespace nlm {

class AdvancedConfig : public Config {
public:
    // Constructors
    AdvancedConfig();
    AdvancedConfig(const std::string& filename);
    
    // Performance optimization
    void setPerformanceMode(PerformanceMode mode);
    void enableParallelProcessing(bool enable = true);
    void enableGPUAcceleration(bool enable = true);
    void setThreadCount(int count);
    
    // Memory optimization
    void enableMemoryCache(bool enable = true);
    void enableMemoryCompression(bool enable = true);
    void setMemoryEvictionPolicy(MemoryEvictionPolicy policy);
    void setMemoryPoolSize(size_t size);
    
    // Neuromodulation settings
    void setCuriosityStrength(double strength);
    void setNoveltyThreshold(double threshold);
    void setDopamineGatingStrength(double strength);
    void setNeuromodulationUpdateInterval(int interval);
    
    // Cognitive settings
    void setPlanningHorizon(int horizon);
    void setAttentionFocusStrength(double strength);
    void setSelfModelUpdateFrequency(int frequency);
    
    // Simulation settings
    void setMaxSteps(int maxSteps);
    void setCheckpointInterval(int interval);
    void setSimulationTimestep(double timestep);
    void setLoggingLevel(LogLevel level);
    
    // Helper methods
    PerformanceMode getPerformanceMode() const;
    bool isGPUEnabled() const;
    MemoryEvictionPolicy getMemoryEvictionPolicy() const;
};

enum class PerformanceMode {
    BALANCED,
    MAXIMUM,
    FAST,
    POWER_SAVE
};

enum class MemoryEvictionPolicy {
    FIRST_IN_FIRST_OUT,
    LEAST_RECENTLY_USED,
    LEAST_FREQUENTLY_USED,
    ADAPTIVE
};

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    OFF
};

} // namespace nlm
```

### Core Classes

#### nlm/core/Brain.hpp

```cpp
namespace nlm {

class Brain {
public:
    // Constructors
    Brain();
    Brain(const Config& config);
    
    // Brain lifecycle management
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    
    // Simulation control
    void step(int step);
    void step(int step, Timestamp timestamp);
    int getCurrentStep() const;
    Timestamp getCurrentTime() const;
    
    // Brain state access
    int getNeuronCount() const;
    int getSynapseCount() const;
    double getTotalSpikeCount() const;
    std::string getMemoryStats() const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // Brain systems access
    WorkingMemory* getWorkingMemory();
    EpisodicMemory* getEpisodicMemory();
    SemanticMemory* getSemanticMemory();
    ProceduralMemory* getProceduralMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    Attention* getAttention();
    SelfModel* getSelfModel();
    SocialLearner* getSocialLearner();
    Neuromodulation* getNeuromodulation();
    Development* getDevelopment();
    
    // Statistics and monitoring
    Statistics getStatistics() const;
    std::string getDebugInfo() const;
    
    // Persistence
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);
    
    // Error handling
    void setErrorHandler(std::function<void(const std::exception&)>& handler);
    std::vector<std::exception> getErrors() const;
};

} // namespace nlm
```

#### nlm/brain/Brain.hpp

```cpp
namespace nlm {

class Brain {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    // Constructors
    Brain();
    Brain(const Config& config);
    
    // Deleted copy constructor and assignment operator
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Move constructor and assignment operator
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Destructor
    ~Brain();
    
    // Brain lifecycle management
    bool initialize();
    void shutdown();
    bool isInitialized() const;
    
    // Simulation control
    void step(int step);
    void step(int step, Timestamp timestamp);
    int getCurrentStep() const;
    Timestamp getCurrentTime() const;
    
    // Brain state access
    int getNeuronCount() const;
    int getSynapseCount() const;
    double getTotalSpikeCount() const;
    std::string getMemoryStats() const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // Brain systems access
    WorkingMemory* getWorkingMemory();
    EpisodicMemory* getEpisodicMemory();
    SemanticMemory* getSemanticMemory();
    ProceduralMemory* getProceduralMemory();
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    Attention* getAttention();
    SelfModel* getSelfModel();
    SocialLearner* getSocialLearner();
    Neuromodulation* getNeuromodulation();
    Development* getDevelopment();
    
    // Statistics and monitoring
    Statistics getStatistics() const;
    std::string getDebugInfo() const;
    
    // Persistence
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);
    
    // Error handling
    void setErrorHandler(std::function<void(const std::exception&)>& handler);
    std::vector<std::exception> getErrors() const;
};

} // namespace nlm
```

#### nlm/agent/AgentBrain.hpp

```cpp
namespace nlm {

class AgentBrain {
public:
    // Constructors
    AgentBrain(Brain& brain);
    AgentBrain(Brain& brain, const Config& agentConfig);
    
    // Agent lifecycle management
    bool initialize(World& world);
    void shutdown();
    bool isInitialized() const;
    
    // World interaction
    void setWorld(World& world);
    World& getWorld();
    
    // Sensory processing
    SensorData processSensoryInput(const Percept& percept);
    SensorData processSensoryInput(const Percept& percept, Timestamp timestamp);
    
    // Action selection
    Action decodeMotorCommand();
    Action decodeMotorCommand(Timestamp timestamp);
    Action decodeMotorCommand(const Action& preferredAction);
    
    // Agent state access
    bool isEpisodeComplete() const;
    int getStepCount() const;
    double getTotalReward() const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // Cognitive system integration
    void enableCognitiveSystems(bool enable = true);
    void setCognitiveSystemWeights(const std::map<std::string, double>& weights);
    std::map<std::string, double> getCognitiveSystemWeights() const;
    
    // Performance monitoring
    Statistics getPerformanceStatistics() const;
    void setPerformanceMonitor(std::shared_ptr<PerformanceMonitor> monitor);
};

} // namespace nlm
```

#### nlm/memory/WorkingMemory.hpp

```cpp
namespace nlm {

class WorkingMemory {
public:
    // Constructors
    WorkingMemory();
    WorkingMemory(const Config& config);
    
    // Memory operations
    bool store(const std::string& key, const std::string& value, double strength = 1.0);
    bool store(const std::string& key, double value, double strength = 1.0);
    bool store(const std::string& key, const Vector3D& value, double strength = 1.0);
    bool store(const std::string& key, const std::string& value, const std::string& context, double strength = 1.0);
    
    std::string retrieve(const std::string& key, double threshold = 0.5) const;
    double retrieveDouble(const std::string& key, double threshold = 0.5) const;
    Vector3D retrieveVector(const std::string& key, double threshold = 0.5) const;
    std::pair<std::string, std::string> retrieveWithContext(const std::string& key, double threshold = 0.5) const;
    
    // Memory management
    bool remove(const std::string& key);
    bool clear();
    bool isStored(const std::string& key) const;
    
    // Memory statistics
    int getEntryCount() const;
    int getTotalItems() const;
    double getAverageStrength() const;
    double getMemoryCapacity() const;
    
    // Memory properties
    bool isFull() const;
    double getRemainingCapacity() const;
    std::vector<std::string> getKeys() const;
    
    // Memory dynamics
    void updateStrength(const std::string& key, double newStrength);
    void decayStrength(double decayRate);
    void applyCompetition();
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

} // namespace nlm
```

#### nlm/memory/EpisodicMemory.hpp

```cpp
namespace nlm {

class EpisodicMemory {
public:
    // Constructors
    EpisodicMemory();
    EpisodicMemory(const Config& config);
    
    // Memory storage
    bool store(const Episode& episode);
    bool store(const std::string& input, const std::string& output, 
               double reward, Timestamp timestamp,
               const std::string& context = "");
    
    // Memory retrieval
    Episode retrieve(const std::string& input) const;
    std::vector<Episode> retrieveSimilar(const std::string& input, int maxResults = 10) const;
    std::vector<Episode> retrieveByTime(Timestamp start, Timestamp end) const;
    std::vector<Episode> retrieveByReward(double minReward) const;
    
    // Memory management
    bool remove(const std::string& episodeId);
    bool clear();
    bool isStored(const std::string& episodeId) const;
    
    // Memory statistics
    int getEntryCount() const;
    int getTotalEpisodes() const;
    double getAverageReward() const;
    double getMemoryCapacity() const;
    
    // Memory properties
    bool isFull() const;
    double getRemainingCapacity() const;
    std::vector<std::string> getEpisodeIds() const;
    
    // Memory consolidation
    void consolidate();
    void replay(const std::string& episodeId);
    void replayByTime(Timestamp start, Timestamp end);
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

struct Episode {
    std::string id;
    Timestamp timestamp;
    std::string input;
    std::string output;
    double reward;
    std::string context;
    double prediction_error;
    std::map<std::string, double> metadata;
};

} // namespace nlm
```

#### nlm/memory/AssociativeMemory.hpp

```cpp
namespace nlm {

class AssociativeMemory {
public:
    // Constructors
    AssociativeMemory();
    AssociativeMemory(const Config& config);
    
    // Association formation
    bool associate(const std::string& stimulus, const std::string& response);
    bool associate(const std::string& stimulus, double stimulusStrength, 
                   const std::string& response, double responseStrength);
    
    // Prediction
    std::string predict(const std::string& stimulus) const;
    double predictStrength(const std::string& stimulus) const;
    std::vector<std::pair<std::string, double>> predictMultiple(const std::string& stimulus, int maxResults = 5) const;
    
    // Association management
    bool removeAssociation(const std::string& stimulus);
    bool clear();
    bool isAssociated(const std::string& stimulus) const;
    
    // Association statistics
    int getAssociationCount() const;
    int getTotalAssociations() const;
    double getAverageStrength() const;
    double getAssociability() const;
    
    // Pattern completion
    std::string completePattern(const std::string& partialStimulus) const;
    bool isPatternComplete(const std::string& stimulus) const;
    
    // Hebbian learning
    void applyHebbianLearning(const std::string& stimulus, const std::string& response, double learningRate = 0.01);
    void applySTDP(const std::string& preNeurons, const std::string& postNeurons, double stdpWeight = 0.1);
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

} // namespace nlm
```

#### nlm/cognition/NeuralPlanner.hpp

```cpp
namespace nlm {

class NeuralPlanner {
public:
    // Constructors
    NeuralPlanner();
    NeuralPlanner(const Config& config);
    
    // Planning
    std::vector<std::string> planAction(const std::string& goal, 
                                        const std::vector<std::string>& availableActions,
                                        int horizon = 5);
    std::vector<Action> planActionSequence(const std::string& goal,
                                           const std::vector<std::string>& availableActions,
                                           int horizon = 5);
    double evaluatePlan(const std::vector<std::string>& plan,
                       const World& world) const;
    
    // Planning parameters
    void setPlanningHorizon(int horizon);
    void setActionCostFunction(std::function<double(const std::string&, const World&)> costFunction);
    void setGoalEvaluator(std::function<double(const std::string&, const World&)> evaluator);
    void setExplorationRate(double rate);
    
    // Planning statistics
    int getPlanCount() const;
    double getAveragePlanQuality() const;
    double getPlanningEfficiency() const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

} // namespace nlm
```

#### nlm/cognition/Attention.hpp

```cpp
namespace nlm {

class Attention {
public:
    // Constructors
    Attention();
    Attention(const Config& config);
    
    // Attentional selection
    std::string selectAttention(const std::vector<std::string>& inputs);
    std::string selectAttention(const std::vector<std::pair<std::string, double>>& inputs);
    double selectAttentionalWeight(const std::string& input, const std::vector<std::pair<std::string, double>>& allInputs);
    
    // Attention modulation
    void setPriority(const std::string& input, double priority);
    void updateAttention(const std::vector<double>& salienceValues);
    void applyCompetition(const std::vector<std::string>& inputs);
    void applyTopDownModulation(const std::string& goal, double strength);
    
    // Attention statistics
    double getAttentionFocus(const std::string& input) const;
    std::string getAttendedInput() const;
    double getAttentionDistraction() const;
    int getAttentionalResourcesUsed() const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

} // namespace nlm
```

#### nlm/cognition/SelfModel.hpp

```cpp
namespace nlm {

class SelfModel {
public:
    // Constructors
    SelfModel();
    SelfModel(const Config& config);
    
    // Self-model operations
    void updateInternalState(const SensorData& sensorData);
    void predictActionConsequences(const Action& action);
    double getActionOutcomePrediction(const Action& action) const;
    std::string getActionBelief(const Action& action) const;
    
    // Agency attribution
    bool isSelfGenerated(const Event& event) const;
    std::string getAgencyAttribution(const Event& event) const;
    double getAgencyConfidence(const Event& event) const;
    
    // Self-model statistics
    int getInternalStateSize() const;
    double getModelConsistency() const;
    double getPredictionAccuracy() const;
    std::map<std::string, double> getSelfModelMetrics() const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

} // namespace nlm
```

#### nlm/neuromodulation/Neuromodulation.hpp

```cpp
namespace nlm {

class Neuromodulation {
public:
    // Constructors
    Neuromodulation();
    Neuromodulation(const Config& config);
    
    // Neuromodulator access
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionError();
    Serotonin* getSerotonin();
    Norepinephrine* getNorepinephrine();
    Acetylcholine* getAcetylcholine();
    
    // Neuromodulation integration
    void updateNeuromodulation(Timestamp timestamp);
    void applyNeuromodulationToPlasticity(Plasticity& plasticity);
    void applyNeuromodulationToCognition(Cognition& cognition);
    void applyNeuromodulationToMemory(Memory& memory);
    
    // Neuromodulation statistics
    double getDopamineLevel() const;
    double getCuriosityLevel() const;
    double getNoveltyLevel() const;
    double getPredictionErrorLevel() const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

} // namespace nlm
```

#### nlm/neuromodulation/Dopamine.hpp

```cpp
namespace nlm {

class Dopamine {
public:
    // Constructors
    Dopamine();
    Dopamine(const Config& config);
    
    // Reward processing
    void processReward(double reward, double predictionError);
    double getDopamineLevel() const;
    double getRewardPredictionError() const;
    
    // Plasticity modulation
    double modulatePlasticity(double baselinePlasticity, const SynapticConnection& synapse) const;
    void applyRewardModulation(SynapticSystem& synapticSystem);
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

} // namespace nlm
```

#### nlm/neuromodulation/Curiosity.hpp

```cpp
namespace nlm {

class Curiosity {
public:
    // Constructors
    Curiosity();
    Curiosity(const Config& config);
    
    // Exploration motivation
    double calculateCuriosity(const Environment& environment, 
                             const AgentState& agentState);
    bool shouldExplore(double curiosityThreshold = 0.5) const;
    std::string generateExplorationGoal(const Environment& environment) const;
    
    // Novelty detection
    double detectNovelty(const Percept& percept, Timestamp timestamp) const;
    bool isNovel(const Percept& percept, double noveltyThreshold = 0.5) const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

} // namespace nlm
```

#### nlm/prediction/PredictionSystem.hpp

```cpp
namespace nlm {

class PredictionSystem {
public:
    // Constructors
    PredictionSystem();
    PredictionSystem(const Config& config);
    
    // Temporal prediction
    void trainSequence(const std::vector<int>& sequence);
    int predictNext() const;
    std::vector<int> predictSequence(int length) const;
    double getPredictionConfidence() const;
    
    // Action-consequence prediction
    void trainActionConsequence(const Action& action, const WorldState& worldState);
    double predictActionOutcome(const Action& action, const WorldState& worldState) const;
    Action predictBestAction(const std::vector<Action>& availableActions, const WorldState& worldState) const;
    
    // Prediction error
    double calculatePredictionError(const std::vector<int>& actual, const std::vector<int>& predicted) const;
    void updatePrediction(double predictionError);
    double getPredictionErrorLevel() const;
    
    // Prediction statistics
    int getTrainingSamples() const;
    double getPredictionAccuracy() const;
    double getTemporalPredictionAccuracy() const;
    double getActionConsequencePredictionAccuracy() const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

} // namespace nlm
```

#### nlm/development/Development.hpp

```cpp
namespace nlm {

class Development {
public:
    // Constructors
    Development();
    Development(const Config& config);
    
    // Developmental stages
    void updateDevelopmentStage(Timestamp timestamp);
    DevelopmentStage getCurrentStage() const;
    bool isDevelopmentStageComplete(DevelopmentStage stage) const;
    
    // Plasticity modulation
    double getPlasticityModifier(PlasticityType type) const;
    void applyDevelopmentalPlasticity(SynapticSystem& synapticSystem);
    void applyDevelopmentalMaturation(Brain& brain);
    
    // Maturation
    void mature() const;
    bool isMatured() const;
    double getMaturationLevel() const;
    
    // Critical periods
    bool isCriticalPeriod(PlasticityType type) const;
    Timestamp getCriticalPeriodEnd(PlasticityType type) const;
    void extendCriticalPeriod(PlasticityType type, double extensionFactor = 1.0);
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

enum class DevelopmentStage {
    EMBRYONIC,
    NEONATAL,
    JUVENILE,
    ADOLESCENT,
    ADULT
};

enum class PlasticityType {
    SYNAPTAGENESIS,
    PRUNING,
    STDP,
    HEBBIAN,
    HOMEOSTATIC
};

} // namespace nlm
```

#### nlm/performance/PerformanceMonitor.hpp

```cpp
namespace nlm {

class PerformanceMonitor {
public:
    // Constructors
    PerformanceMonitor();
    PerformanceMonitor(const Config& config);
    
    // Performance monitoring
    void start();
    void stop();
    bool isRunning() const;
    
    void recordStep(int step);
    void recordSpike(int neuronId, Timestamp timestamp);
    void recordMemoryAccess(int memoryId, bool hit);
    
    // Statistics
    PerformanceMetrics getMetrics() const;
    double getOperationsPerSecond() const;
    double getAverageStepTime() const;
    double getSpikeRate() const;
    double getMemoryUsage() const;
    double getCPUUtilization() const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

struct PerformanceMetrics {
    double total_time;
    int total_steps;
    double operations_per_second;
    double average_step_time;
    double spike_rate;
    double memory_usage_mb;
    double cpu_utilization;
    double memory_efficiency;
    double stability;
    double prediction_accuracy;
    std::map<std::string, double> custom_metrics;
};

} // namespace nlm
```

### Supporting Classes and Structures

#### nlm/core/Statistics.hpp

```cpp
namespace nlm {

struct Statistics {
    // Simulation statistics
    int total_steps;
    double simulation_time;
    double average_step_time;
    
    // Neural statistics
    int total_neurons;
    int total_synapses;
    double total_spikes;
    double average_firing_rate;
    
    // Memory statistics
    int working_memory_entries;
    int episodic_memory_entries;
    int associative_memory_associations;
    
    // Cognitive statistics
    int planning_steps_generated;
    int attention_selections_made;
    int predictions_made;
    int concepts_formed;
    
    // Neuromodulation statistics
    double dopamine_level;
    double curiosity_level;
    double novelty_level;
    double prediction_error_level;
    
    // Performance statistics
    double memory_efficiency;
    double stability;
    double prediction_accuracy;
    double learning_rate;
};

} // namespace nlm
```

#### nlm/world/World.hpp

```cpp
namespace nlm {

class World {
public:
    // Constructors
    World();
    World(int width, int height);
    
    // World management
    void reset();
    void update(double deltaTime);
    bool isEpisodeComplete() const;
    
    // Sensory processing
    Percept getSensoryPercept() const;
    Percept getSensoryPercept(SensorType type) const;
    
    // Action application
    void applyMotorCommand(const Action& action, Timestamp timestamp);
    bool applyAction(const std::string& action, Timestamp timestamp);
    
    // World state access
    int getWidth() const;
    int getHeight() const;
    WorldState getWorldState() const;
    
    // Reward computation
    double computeReward(const Agent& agent) const;
    double computeReward(const std::string& action, const Agent& agent) const;
    
    // Configuration access
    const Config& getConfig() const;
    void setConfig(const Config& config);
    
    // State management
    std::string serialize() const;
    bool deserialize(const std::string& data);
};

} // namespace nlm
```

#### nlm/core/Types.hpp

```cpp
namespace nlm {

// Core type definitions
using NeuronId = uint32_t;
using SynapseId = uint32_t;
using RegionId = uint32_t;
using PopulationId = uint32_t;
using ActionId = uint32_t;
using Step = int64_t;
using Timestamp = double;
using Weight = double;
using Strength = double;
using Probability = double;
using Rate = double;
using Time = double;

// Standard types
using Vector3D = std::array<double, 3>;
using Matrix3x3 = std::array<std::array<double, 3>, 3>;
using SensorData = std::vector<double>;
using Percept = std::map<std::string, double>;
using Action = std::map<std::string, double>;
using Event = std::map<std::string, std::string>;
using WorldState = std::map<std::string, double>;

// Enumerations
enum class SensorType {
    VISION,
    TOUCH,
    PROPRIOCEPTION,
    INTERIOR,
    AUDIO,
    TACTILE
};

enum class MotorCommandType {
    MOVE,
    GRASP,
    EXPLORE,
    COMMUNICATE,
    STAY,
    NONE
};

} // namespace nlm
```

### Utility Classes

#### nlm/core/Exception.hpp

```cpp
namespace nlm {

class NLMException : public std::exception {
public:
    enum class Type {
        GENERIC,
        CONFIG,
        BRAIN,
        MEMORY,
        WORLD,
        PREDICTION,
        NEUROMODULATION,
        DEVELOPMENT
    };
    
    NLMException(Type type, const std::string& message);
    NLMException(Type type, const std::string& message, const std::string& errorCode);
    
    const char* what() const noexcept override;
    
    Type getType() const;
    const std::string& getMessage() const;
    const std::string& getErrorCode() const;
    
    template<typename T>
    T getData() const;
    
    template<typename T>
    void setData(const T& data);
    
private:
    Type type;
    std::string message;
    std::string errorCode;
    std::any data;
};

// Specialized exception classes
class ConfigError : public NLMException {
public:
    ConfigError(const std::string& message) : NLMException(Type::CONFIG, message) {}
};

class BrainError : public NLMException {
public:
    BrainError(const std::string& message) : NLMException(Type::BRAIN, message) {}
};

class MemoryError : public NLMException {
public:
    MemoryError(const std::string& message) : NLMException(Type::MEMORY, message) {}
};

class WorldError : public NLMException {
public:
    WorldError(const std::string& message) : NLMException(Type::WORLD, message) {}
};

class PredictionError : public NLMException {
public:
    PredictionError(const std::string& message) : NLMException(Type::PREDICTION, message) {}
};

class NeuromodulationError : public NLMException {
public:
    NeuromodulationError(const std::string& message) : NLMException(Type::NEUROMODULATION, message) {}
};

class DevelopmentError : public NLMException {
public:
    DevelopmentError(const std::string& message) : NLMException(Type::DEVELOPMENT, message) {}
};

} // namespace nlm
```

## API Usage Examples

### Basic C++ Usage
```cpp
#include <nlm/core/Brain.hpp>
#include <nlm/config/Phase6Config.hpp>

int main() {
    // Create Phase 6 configuration
    nlm::Phase6Config config;
    config.enableMemorySystems();
    config.enableCognitiveSystems();
    config.enableNeuromodulation();
    
    // Create brain
    nlm::Brain brain(config);
    brain.initialize();
    
    // Run simulation
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
    }
    
    // Shutdown
    brain.shutdown();
    return 0;
}
```

### Advanced C++ Usage
```cpp
#include <nlm/core/Brain.hpp>
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/world/World.hpp>
#include <nlm/config/AdvancedConfig.hpp>

int main() {
    // Create advanced configuration
    nlm::AdvancedConfig config;
    config.setPerformanceMode(nlm::PerformanceMode::MAXIMUM);
    config.enableGPUAcceleration(true);
    config.setMemoryCache(true);
    
    // Create brain, world, and agent
    nlm::Brain brain(config);
    nlm::World world(20, 20);
    nlm::AgentBrain agent(brain);
    
    // Initialize systems
    brain.initialize();
    agent.initialize(world);
    
    // Run episode
    for (int step = 0; step < 500; ++step) {
        // Process sensory input
        auto percept = world.getSensoryPercept();
        auto sensorData = agent.processSensoryInput(percept);
        
        // Brain processing
        brain.step(step);
        
        // Action selection
        auto action = agent.decodeMotorCommand();
        
        // Apply action
        world.applyMotorCommand(action, world.getCurrentTime());
        
        // Check termination
        if (world.isEpisodeComplete()) {
            world.reset();
            agent.initialize(world);
        }
    }
    
    // Shutdown
    agent.shutdown();
    brain.shutdown();
    return 0;
}
```

## API Integration

### Integration with Python Bindings
```cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <nlm/core/Brain.hpp>
#include <nlm/config/Phase6Config.hpp>

namespace py = pybind11;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = "NLM Python API bindings";
    
    // Configuration classes
    py::class_<nlm::Config>(m, "Config")
        .def(py::init<>())
        .def("set", &nlm::Config::set)
        .def("get", &nlm::Config::get)
        .def("validate", &nlm::Config::validate);
    
    py::class_<nlm::Phase6Config, nlm::Config>(m, "Phase6Config")
        .def(py::init<>())
        .def("enable_memory_systems", &nlm::Phase6Config::enableMemorySystems)
        .def("enable_cognitive_systems", &nlm::Phase6Config::enableCognitiveSystems);
    
    // Brain class
    py::class_<nlm::Brain>(m, "Brain")
        .def(py::init<>())
        .def(py::init<const nlm::Config&>())
        .def("initialize", &nlm::Brain::initialize)
        .def("shutdown", &nlm::Brain::shutdown)
        .def("step", &nlm::Brain::step)
        .def("get_current_step", &nlm::Brain::getCurrentStep)
        .def("get_total_spike_count", &nlm::Brain::getTotalSpikeCount)
        .def("get_working_memory", &nlm::Brain::getWorkingMemory)
        .def("get_prediction_system", &nlm::Brain::getPredictionSystem);
    
    // Working memory
    py::class_<nlm::WorkingMemory>(m, "WorkingMemory")
        .def("store", &nlm::WorkingMemory::store)
        .def("retrieve", &nlm::WorkingMemory::retrieve)
        .def("get_entry_count", &nlm::WorkingMemory::getEntryCount);
    
    // Memory class
    py::class_<nlm::EpisodicMemory>(m, "EpisodicMemory")
        .def("store", &nlm::EpisodicMemory::store)
        .def("retrieve", &nlm::EpisodicMemory::retrieve);
    
    // Prediction system
    py::class_<nlm::PredictionSystem>(m, "PredictionSystem")
        .def("train_sequence", &nlm::PredictionSystem::trainSequence)
        .def("predict_next", &nlm::PredictionSystem::predictNext);
    
    // Exception classes
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const nlm::NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}
```

## API Documentation Summary

### Core Classes
- **nlm::Config**: Base configuration class
- **nlm::Phase6Config**: Phase 6 specific configuration
- **nlm::AdvancedConfig**: Advanced configuration with optimization settings
- **nlm::Brain**: Main brain simulation class
- **nlm::AgentBrain**: Agent integration class
- **nlm::WorkingMemory**: Working memory system
- **nlm::EpisodicMemory**: Episodic memory system
- **nlm::AssociativeMemory**: Associative memory system
- **nlm::NeuralPlanner**: Planning system
- **nlm::Attention**: Attention system
- **nlm::SelfModel**: Self-model system
- **nlm::Neuromodulation**: Neuromodulation system
- **nlm::Dopamine**: Dopamine system
- **nlm::Curiosity**: Curiosity system
- **nlm::Novelty**: Novelty detection
- **nlm::PredictionSystem**: Prediction system
- **nlm::Development**: Development system
- **nlm::PerformanceMonitor**: Performance monitoring

### Key Headers
- **include/nlm/config/Config.hpp**: Configuration base class
- **include/nlm/config/Phase6Config.hpp**: Phase 6 configuration
- **include/nlm/config/AdvancedConfig.hpp**: Advanced configuration
- **include/nlm/core/Brain.hpp**: Brain core class
- **include/nlm/agent/AgentBrain.hpp**: Agent integration
- **include/nlm/memory/WorkingMemory.hpp**: Working memory
- **include/nlm/memory/EpisodicMemory.hpp**: Episodic memory
- **include/nlm/memory/AssociativeMemory.hpp**: Associative memory
- **include/nlm/cognition/NeuralPlanner.hpp**: Planning system
- **include/nlm/cognition/Attention.hpp**: Attention system
- **include/nlm/cognition/SelfModel.hpp**: Self-model system
- **include/nlm/neuromodulation/Neuromodulation.hpp**: Neuromodulation system
- **include/nlm/neuromodulation/Dopamine.hpp**: Dopamine system
- **include/nlm/neuromodulation/Curiosity.hpp**: Curiosity system
- **include/nlm/prediction/PredictionSystem.hpp**: Prediction system
- **include/nlm/development/Development.hpp**: Development system
- **include/nlm/performance/PerformanceMonitor.hpp**: Performance monitoring

### Key Functions
- **nlm::createBrain()**: Create brain instance
- **nlm::createAgentBrain()**: Create agent with brain integration
- **nlm::createWorld()**: Create world environment
- **nlm::createDefaultConfig()**: Create default configuration
- **nlm::createPhase6Config()**: Create Phase 6 configuration
- **nlm::createAdvancedConfig()**: Create advanced configuration

## API Testing and Validation

### Running C++ Tests
```bash
# Run all tests
make test

# Run specific test suites
cd tests
./test_phase6_integration
./test_phase6_memory
./test_phase6_prediction
./test_full_integration

# Run performance tests
./test_performance_benchmark
```

### Test Coverage
The NLM C++ API includes comprehensive test coverage:
- **Unit tests**: Individual component testing
- **Integration tests**: System-wide functionality verification
- **Performance tests**: Efficiency and scalability assessment
- **Stress tests**: Robustness and reliability testing
- **Compatibility tests**: Cross-version and cross-platform validation

## API Documentation Generation

### Generating C++ Documentation
```bash
# Generate API documentation
doxygen Doxyfile

# Generate comprehensive documentation
pandoc docs/api/cpp.md -o docs/api/cpp.html
```

### Documentation Sources
- **Source code**: Inline comments and documentation
- **Headers**: Class and function documentation
- **Examples**: Complete working examples
- **Tests**: Comprehensive test coverage
- **Reference**: Complete API documentation