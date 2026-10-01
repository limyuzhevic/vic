#pragma once

#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/environment/Environment.hpp"
#include "../src/experiments/Experiment.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace nlm {

// Multi-agent configuration
struct MultiAgentConfig {
    // Agent population
    struct AgentTemplate {
        std::string name;
        std::string type;  // "sensorimotor", "cognitive", "reactive", "architect"
        float sensitivity;
        float learningRate;
        float explorationRate;
        std::map<std::string, float> parameters;
        
        AgentTemplate() : sensitivity(1.0f), learningRate(0.01f), explorationRate(0.1f) {}
    };
    
    std::vector<AgentTemplate> agentTemplates;
    size_t populationSize;
    
    // Coordination mechanisms
    struct CoordinationMechanism {
        std::string name;
        std::string type;  // "hierarchical", "swarm", "consensus", "auction"
        float strength;
        std::map<std::string, float> parameters;
        bool isEnabled;
        
        CoordinationMechanism() : strength(1.0f), isEnabled(true) {}
    };
    
    std::vector<CoordinationMechanism> coordinationMechanisms;
    
    // Custom environments
    struct EnvironmentTemplate {
        std::string name;
        std::string type;  // "continuous", "discrete", "partiallyObservable"
        std::map<std::string, float> parameters;
        bool isDynamicallyChanging;
        
        EnvironmentTemplate() : isDynamicallyChanging(false) {}
    };
    
    std::vector<EnvironmentTemplate> environmentTemplates;
    
    // Advanced experiment control
    struct ExperimentControl {
        bool enableStepControl;
        bool enableConditionControl;
        bool enableRewardControl;
        bool enableNoiseControl;
        float noiseLevel;
        float stepDuration;
        float simulationSpeed;
        
        ExperimentControl() : enableStepControl(false), enableConditionControl(false),
                            enableRewardControl(false), enableNoiseControl(false),
                            noiseLevel(0.01f), stepDuration(0.1f), simulationSpeed(1.0f) {}
    };
    
    ExperimentControl experimentControl;
    
    // Parallel processing configuration
    struct ParallelProcessingConfig {
        bool enableParallel;
        size_t numThreads;
        std::string schedulingPolicy;  // "static", "dynamic", "workstealing"
        float loadBalancingThreshold;
        std::string memoryAllocationStrategy;  // "pool", "manual", "automatic"
        
        ParallelProcessingConfig() : enableParallel(false), numThreads(std::thread::hardware_concurrency()),
                                    loadBalancingThreshold(0.8f) {}
    };
    
    ParallelProcessingConfig parallelConfig;
    
    // Memory-efficient simulation modes
    enum class MemoryMode {
        Standard,
        Compressed,
        Sparse,
        Streaming,
        OnDemand
    };
    
    MemoryMode memoryMode;
    float compressionRatio;
    bool enableCheckpointing;
    bool enableLazyLoading;
    
    // Simulation statistics
    struct SimulationStats {
        size_t totalSteps;
        float averageStepTime;
        size_t activeAgents;
        float averageFitness;
        std::map<std::string, float> populationStats;
        std::map<std::string, float> environmentStats;
        float memoryUsage;
        float cpuUsage;
    };
    
    SimulationStats stats;
};

// Agent state for multi-agent systems
struct AgentState {
    AgentId id;
    std::string name;
    std::string type;
    float health;
    float energy;
    float experience;
    float fitness;
    std::map<std::string, float> attributes;
    std::map<std::string, float> memory;
    std::vector<std::string> behaviors;
    
    AgentState() : id(0), health(1.0f), energy(1.0f), experience(0.0f), fitness(0.0f) {}
};

// Event for multi-agent coordination
struct CoordinationEvent {
    enum class Type {
        Message,
        Action,
        Observation,
        Decision,
        Learning,
        Migration,
        Conflict
    };
    
    Type type;
    Timestamp timestamp;
    AgentId sourceAgent;
    AgentId targetAgent;
    std::string message;
    std::map<std::string, float> data;
    float priority;
    
    CoordinationEvent() : timestamp(0.0), sourceAgent(0), targetAgent(0), priority(0.0f) {}
};

// Advanced Simulation Engine
class AdvancedSimulation {
public:
    AdvancedSimulation();
    ~AdvancedSimulation();
    
    // Disable copying, enable moving
    AdvancedSimulation(const AdvancedSimulation&) = delete;
    AdvancedSimulation& operator=(const AdvancedSimulation&) = delete;
    AdvancedSimulation(AdvancedSimulation&&) noexcept = default;
    AdvancedSimulation& operator=(AdvancedSimulation&&) noexcept = default;
    
    // Configuration
    void configure(const MultiAgentConfig& config);
    bool loadEnvironment(const std::string& envName);
    bool loadAgentTemplate(const std::string& templateName);
    
    // Multi-agent system management
    AgentId createAgent(const std::string& templateName, RegionId region = RegionId(1));
    bool removeAgent(AgentId agentId);
    bool activateAgent(AgentId agentId);
    bool deactivateAgent(AgentId agentId);
    
    // Simulation control
    void start();
    void stop();
    bool isRunning() const;
    void pause();
    void resume();
    
    // Step execution
    void step(SimulationStep step, Timestamp timestamp = 0.0f);
    void step(const std::vector<CoordinationEvent>& events, SimulationStep step, Timestamp timestamp = 0.0f);
    
    // Agent interaction
    void sendMessage(AgentId source, AgentId target, const std::string& message,
                    const std::map<std::string, float>& data = {});
    void broadcastMessage(AgentId source, const std::string& message,
                         const std::map<std::string, float>& data = {});
    
    // Learning and adaptation
    void triggerLearning(AgentId agentId, const std::string& learningType = "standard");
    void adaptEnvironment(const std::map<std::string, float>& changes);
    
    // Experiment control
    void setStepCondition(bool condition);
    void setReward(float reward);
    void addNoise(float noiseLevel);
    void setSimulationSpeed(float speed);
    void setStepDuration(float duration);
    
    // Parallel processing
    void enableParallelProcessing(bool enable);
    void setThreadCount(size_t count);
    size_t getActiveThreadCount() const;
    
    // Memory optimization
    void setMemoryMode(MultiAgentConfig::MemoryMode mode);
    void enableCheckpointing(bool enable, const std::string& directory = "checkpoints");
    void enableLazyLoading(bool enable);
    
    // Statistics and monitoring
    AgentState getAgentState(AgentId agentId) const;
    std::vector<AgentState> getAllAgentStates() const;
    std::vector<CoordinationEvent> getRecentEvents(size_t count = 100) const;
    MultiAgentConfig::SimulationStats getSimulationStats() const;
    float getSystemPerformance() const;
    
    // Advanced analysis
    std::vector<std::string> detectEmergentBehaviors() const;
    float calculateCollectiveIntelligence() const;
    std::map<std::string, float> analyzeSocialStructure() const;
    
    // Export and import
    bool exportState(const std::string& filepath) const;
    bool importState(const std::string& filepath);
    bool exportConfiguration(const std::string& filepath) const;
    bool importConfiguration(const std::string& filepath);
    
    // Custom environments
    void setEnvironmentParameter(const std::string& name, float value);
    float getEnvironmentParameter(const std::string& name) const;
    void updateEnvironment(TimestepDuration dt);
    
    // Neural network integration
    void integrateNeuralNetwork(AgentId agentId, const std::string& networkType = "mlp");
    void syncNeuralNetworks();
    
    // Performance optimization
    void optimizeConnectivity(float targetDensity);
    void compressAgentMemory(float compressionRatio);
    void enableMemoryPooling(bool enable);
    
    // Get current configuration
    const MultiAgentConfig& getConfig() const;
    void updateConfig(const MultiAgentConfig& config);
    
    // Monitoring and debugging
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    void logSimulationStatus() const;
    void logAgentStatus(AgentId agentId) const;
    
    // Experiment tracking
    void recordExperimentData(const std::string& key, float value);
    float getExperimentData(const std::string& key) const;
    std::map<std::string, float> getAllExperimentData() const;
    
    // Coordination mechanism management
    bool addCoordinationMechanism(const MultiAgentConfig::CoordinationMechanism& mechanism);
    bool enableCoordinationMechanism(const std::string& name, bool enable = true);
    bool isCoordinationMechanismEnabled(const std::string& name) const;
    void executeCoordinationCycle();
    
    // Event processing
    void processEvents();
    void processEvent(const CoordinationEvent& event);
    
    // Cleanup
    void cleanup();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
