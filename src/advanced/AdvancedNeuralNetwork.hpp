#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

namespace nlm {

// Advanced Neural Network Features

// Multi-layer neural network construction
class AdvancedNeuralNetwork {
public:
    // Layer types for different neural representations
    enum class LayerType {
        Input,
        Hidden,
        Output,
        Recurrent,
        Attention,
        Memory
    };
    
    // Neuron type registry for pluggable neuron implementations
    class NeuronFactory {
    public:
        virtual ~NeuronFactory() = default;
        virtual std::unique_ptr<class Neuron> createNeuron(NeuronId id) = 0;
        virtual std::string getTypeName() const = 0;
    };
    
    // Synapse type registry for pluggable synapse implementations  
    class SynapseFactory {
    public:
        virtual ~SynapseFactory() = default;
        virtual std::unique_ptr<class Synapse> createSynapse(SynapseId id, NeuronId source, NeuronId destination) = 0;
        virtual std::string getTypeName() const = 0;
    };
    
    // Layer in multi-layer network
    struct Layer {
        std::string name;
        LayerType type;
        size_t neuronCount;
        std::string neuronType;  // Custom neuron type name
        
        // Connectivity parameters
        float connectionProbability;
        std::string connectivityPattern;  // "random", "grid", "small_world", "scale_free"
        
        // External input/output connections
        bool hasExternalInputs;
        bool hasExternalOutputs;
        
        Layer() : neuronCount(0), connectionProbability(0.1f),
                 hasExternalInputs(false), hasExternalOutputs(false) {}
    };
    
    AdvancedNeuralNetwork();
    ~AdvancedNeuralNetwork();
    
    // Network construction methods
    void addLayer(const Layer& layer);
    void connectLayers();
    void configureConnectivity(const std::string& pattern, float probability);
    
    // Neuron/synapse factory management
    void registerNeuronType(const std::string& typeName, std::unique_ptr<NeuronFactory> factory);
    void registerSynapseType(const std::string& typeName, std::unique_ptr<SynapseFactory> factory);
    
    // Multi-modal processing
    void addExternalInput(NeuronId neuronId, const std::vector<float>& inputData);
    void addExternalOutput(NeuronId neuronId, std::function<void(std::vector<float>)> outputCallback);
    
    // Network operations
    void forwardPropagate(const std::vector<float>& input);
    std::vector<float> getLayerOutput(size_t layerIndex) const;
    std::vector<float> getNetworkOutput() const;
    
    // Modular architecture - get neurons and synapses by type
    std::vector<Neuron*> getNeuronsByType(NeuronType type) const;
    std::vector<Synapse*> getSynapsesByType(SynapseType type) const;
    
    // Network statistics
    size_t getLayerCount() const;
    size_t getTotalNeuronCount() const;
    float getNetworkActivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Hierarchical Memory System
class HierarchicalMemorySystem {
public:
    // Memory levels
    enum class MemoryLevel {
        ShortTerm,      // Working memory - < 1 minute retention
        Intermediate,   // Episodic memory - 1 hour to 1 day
        LongTerm        // Semantic memory - days to years
    };
    
    // Memory consolidation policies
    enum class ConsolidationPolicy {
        TimeBased,      // Consolidate after time threshold
        StrengthBased,  // Consolidate when memory strength exceeds threshold
        RewardBased,    // Consolidate when associated with high reward
        MetaLearning    // Meta-learning about optimal consolidation
    };
    
    // Forgetting mechanisms
    enum class ForgettingPolicy {
        ExponentialDecay,
        LinearDecay,
        PriorityBased,
        MetaplasticDecay  // Meta-plasticity affects decay rate
    };
    
    struct MemoryTrace {
        std::vector<float> pattern;      // Neural pattern
        float strength;                   // Memory activation strength
        MemoryLevel level;                // Current memory level
        float timestamp;                  // Creation time
        float lastAccess;                 // Last access time
        size_t accessCount;               // How often accessed
        
        // Transfer learning metadata
        std::vector<float> taskContext;  // Task domain information
        float transferPotential;         // How well memory can transfer
        
        // Meta-learning metadata
        float predictionError;           // Prediction error during consolidation
        float learningRate;               // Adaptive learning rate
        
        MemoryTrace() : strength(0.0f), timestamp(0.0f), lastAccess(0.0f),
                       accessCount(0), transferPotential(0.0f), predictionError(0.0f),
                       learningRate(0.1f) {}
    };
    
    HierarchicalMemorySystem();
    ~HierarchicalMemorySystem();
    
    // Memory operations
    void store(const std::vector<float>& pattern, MemoryLevel level);
    std::vector<float> retrieve(const std::vector<float>& queryPattern, 
                               MemoryLevel minLevel = MemoryLevel::ShortTerm) const;
    
    // Hierarchical operations
    void promoteToNextLevel(size_t traceIndex);
    void consolidateToLongTerm(MemoryLevel sourceLevel, 
                               ConsolidationPolicy policy = ConsolidationPolicy::StrengthBased);
    
    // Forgetting mechanisms
    void forget(ForgettingPolicy policy, float threshold = 0.5f);
    void updateForgettingRates();  // Meta-plasticity affects forgetting
    
    // Transfer learning
    void transferToTask(const std::vector<float>& pattern, 
                       const std::vector<float>& taskContext);
    std::vector<float> retrieveForTask(const std::vector<float>& taskContext) const;
    
    // Meta-learning
    void updateMetaLearningParameters(const std::vector<float>& predictionErrors);
    float getMetaLearningRate(MemoryLevel level) const;
    void adaptConsolidationPolicy();  // Meta-learning about optimal consolidation
    
    // Memory statistics
    size_t getMemoryTraceCount(MemoryLevel level) const;
    float getTotalMemoryStrength(MemoryLevel level) const;
    float getRetentionRate(MemoryLevel level) const;
    
    // Integration with brain
    void setBrain(Brain* brain);
    void step(float timestep);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Goal-based Planning with Hierarchical Task Decomposition
class HierarchicalTaskDecomposition {
public:
    // Task types for different levels of abstraction
    enum class TaskType {
        Primitive,      // Atomic action
        Composite,      // Sequence of primitives
        Subgoal,        // High-level goal
        Strategy,       // Multi-step plan
        Meta            // Self-improvement goal
    };
    
    // Task execution status
    struct Task {
        std::string name;
        TaskType type;
        std::vector<float> goalState;      // Desired state
        float rewardValue;                  // Expected reward
        float complexity;                    // Task difficulty
        float estimatedDuration;            // Time to complete
        
        // Hierarchy
        std::vector<size_t> childTaskIndices;
        size_t parentTaskIndex;              // -1 if root task
        
        // Execution state
        bool isCompleted;
        bool isActive;
        float progress;                      // 0.0 to 1.0
        float accumulatedReward;             // Actual reward received
        
        // Social learning metadata
        std::vector<size_t> observedByAgents;  // Which agents have observed this
        std::vector<float> imitationSuccessRate;
        
        Task() : rewardValue(0.5f), complexity(1.0f), estimatedDuration(1.0f),
                parentTaskIndex(-1), isCompleted(false), isActive(false),
                progress(0.0f), accumulatedReward(0.0f) {}
    };
    
    HierarchicalTaskDecomposition();
    ~HierarchicalTaskDecomposition();
    
    // Task management
    size_t addTask(const Task& task, size_t parentIndex = -1);
    void setCurrentGoal(size_t taskIndex);
    void executeActiveTasks(const std::vector<float>& currentState);
    
    // Hierarchical decomposition
    void decomposeTask(size_t taskIndex, size_t decompositionLevel);
    std::vector<size_t> getTaskHierarchy(size_t taskIndex) const;
    std::vector<size_t> getExecutableTasks(size_t taskIndex) const;
    
    // Planning
    std::vector<size_t> planTaskSequence(size_t taskIndex,
                                         const std::vector<float>& startState);
    float evaluateTaskSequence(const std::vector<size_t>& taskSequence,
                              const std::vector<float>& startState) const;
    
    // Social learning and imitation
    void observeOtherAgentTask(size_t agentId, size_t taskIndex);
    void learnFromObservation(size_t observingAgentId, size_t taskIndex,
                             const std::vector<float>& successIndicators);
    std::vector<size_t> getImitationCandidates(const std::vector<float>& currentState) const;
    
    // Meta-cognition
    void monitorTaskExecution(size_t taskIndex, const std::vector<float>& actualOutcome);
    float assessMetaCognitiveState() const;  // Self-monitoring confidence
    void optimizeTaskExecution(const std::vector<float>& feedback);
    void improveTaskStrategies();  // Self-improvement
    
    // Transfer of skills
    void transferSkill(size_t sourceTaskIndex, size_t targetTaskIndex);
    std::vector<size_t> findTransferableSkills(size_t sourceTaskIndex) const;
    
    // Integration with brain
    void setBrain(Brain* brain);
    void step(float timestep);
    
    // API for external agents
    const Task& getTask(size_t index) const;
    std::vector<size_t> getRootTasks() const;
    size_t getCurrentGoalTask() const { return currentGoalTask_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    size_t currentGoalTask_;
};

} // namespace nlm
