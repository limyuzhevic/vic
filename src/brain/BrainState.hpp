#pragma once

#include "Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>
#include <chrono>

namespace nlm {

class BrainState {
public:
    BrainState() = default;
    ~BrainState() = default;
    
    // State initialization and management
    void initializeState(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    void resetState(std::shared_ptr<Brain> brain);
    void checkpointState(std::shared_ptr<Brain> brain, const std::string& label = "");
    void loadState(std::shared_ptr<Brain> brain, const std::string& label = "");
    
    // Development stage management
    void initializeDevelopmentalStage(std::shared_ptr<Brain> brain, 
                                     std::shared_ptr<Config> config);
    void updateDevelopmentalStage(std::shared_ptr<Brain> brain, 
                                 SimulationStep currentStep, Timestamp currentTime);
    DevelopmentalStage getCurrentDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Time and step management
    void updateTime(SimulationStep currentStep, Timestamp currentTime);
    SimulationStep getCurrentStep() const;
    Timestamp getCurrentTime() const;
    TimestepDuration getTimestep() const;
    void setTimestep(TimestepDuration timestep);
    
    // Statistics and metrics tracking
    void initializeStatistics(std::shared_ptr<Brain> brain);
    void updateStatistics(std::shared_ptr<Brain> brain);
    void resetStatistics(std::shared_ptr<Brain> brain);
    
    // Performance monitoring
    void startPerformanceMeasurement(const std::string& operation);
    void endPerformanceMeasurement(const std::string& operation);
    double getPerformanceMetric(const std::string& operation) const;
    void logPerformanceSummary() const;
    
    // Memory management
    void initializeMemoryPools(std::shared_ptr<Brain> brain);
    void monitorMemoryUsage() const;
    bool checkMemoryHealth() const;
    
    // State persistence
    bool saveCompleteState(std::shared_ptr<Brain> brain, const std::string& filepath) const;
    bool loadCompleteState(std::shared_ptr<Brain> brain, const std::string& filepath);
    std::vector<std::string> getAvailableCheckpoints(const std::string& directory) const;
    
    // Error handling and recovery
    bool handleError(std::shared_ptr<Brain> brain, const std::string& errorMessage);
    bool recoverFromError(std::shared_ptr<Brain> brain);
    bool validateStateIntegrity(std::shared_ptr<Brain> brain) const;
    
    // Configuration and parameters
    void configureStateManagement(std::shared_ptr<Config> config);
    void setCheckpointInterval(size_t interval);
    void setConsolidationInterval(size_t interval);
    void enablePerformanceMonitoring(bool enable);
    void setMemoryLimit(size_t limitBytes);
    
    // State validation
    bool validateBrainState(const Brain* brain) const;
    bool validateIntegrationState(const Brain* brain) const;
    bool validatePlasticityState(const Brain* brain) const;
    
    // State serialization
    std::string serializeState(const Brain* brain) const;
    bool deserializeState(std::shared_ptr<Brain> brain, const std::string& serializedState);
    
private:
    // Internal state management
    void updateDevelopmentalProgression(std::shared_ptr<Brain> brain, 
                                        SimulationStep currentStep);
    void updateSleepCycle(std::shared_ptr<Brain> brain, SimulationStep currentStep);
    void updateMemoryReplayCycle(std::shared_ptr<Brain> brain, SimulationStep currentStep);
    
    // Performance measurement helpers
    void recordPerformanceMeasurement(const std::string& operation, 
                                     std::chrono::high_resolution_clock::time_point startTime);
    
    // State consistency checks
    bool checkNeuronConsistency(const Brain* brain) const;
    bool checkSynapseConsistency(const Brain* brain) const;
    bool checkMemoryConsistency(const Brain* brain) const;
    bool checkIntegrationConsistency(const Brain* brain) const;
    
    // Serialization helpers
    void serializeNeurons(const Brain* brain, CheckpointWriter& writer) const;
    void serializeSynapses(const Brain* brain, CheckpointWriter& writer) const;
    void serializeSystems(const Brain* brain, CheckpointWriter& writer) const;
    void deserializeNeurons(std::shared_ptr<Brain> brain, CheckpointReader& reader);
    void deserializeSynapses(std::shared_ptr<Brain> brain, CheckpointReader& reader);
    void deserializeSystems(std::shared_ptr<Brain> brain, CheckpointReader& reader);
    
    // Internal state
    struct StateData {
        SimulationStep currentStep;
        Timestamp currentTime;
        TimestepDuration timestep;
        DevelopmentalStage developmentalStage;
        SimulationStep stepsSinceLastEpisode;
        SimulationStep stepsSinceLastConsolidation;
        SimulationStep stepsSinceDevelopmentUpdate;
        SimulationStep stepsSinceReplay;
        bool isResting;
        double lastReward;
        double lastSurprise;
        std::chrono::high_resolution_clock::time_point lastConsolidationTime;
        
        StateData() 
            : currentStep(0), currentTime(0.0), timestep(0.001), 
              developmentalStage(DevelopmentalStage::Initial),
              stepsSinceLastEpisode(0), stepsSinceLastConsolidation(0),
              stepsSinceDevelopmentUpdate(0), stepsSinceReplay(0),
              isResting(false), lastReward(0.0), lastSurprise(0.0) {}
    };
    
    StateData state_;
    
    // Performance metrics
    struct PerformanceMetrics {
        std::unordered_map<std::string, std::vector<double>> operationTimes;
        std::unordered_map<std::string, size_t> operationCounts;
        double totalProcessingTime;
        size_t totalStepsProcessed;
        size_t memoryAllocations;
        size_t memoryDeallocations;
        
        PerformanceMetrics() 
            : totalProcessingTime(0.0), totalStepsProcessed(0),
              memoryAllocations(0), memoryDeallocations(0) {}
    };
    
    PerformanceMetrics perfMetrics_;
    
    // Configuration
    struct StateConfig {
        size_t checkpointInterval;
        size_t consolidationInterval;
        size_t replayInterval;
        size_t developmentUpdateInterval;
        bool enablePerformanceMonitoring;
        size_t memoryLimitBytes;
        bool enableStateValidation;
        bool enableMemoryPools;
        
        StateConfig()
            : checkpointInterval(1000), consolidationInterval(10000),
              replayInterval(100), developmentUpdateInterval(1000),
              enablePerformanceMonitoring(true), memoryLimitBytes(1024 * 1024 * 1024), // 1GB
              enableStateValidation(true), enableMemoryPools(true) {}
    };
    
    StateConfig config_;
    
    // Checkpoint management
    struct CheckpointInfo {
        std::string label;
        std::string filepath;
        SimulationStep step;
        Timestamp time;
        std::chrono::system_clock::time_point timestamp;
        
        CheckpointInfo() : step(0), time(0.0) {}
        CheckpointInfo(const std::string& lbl, const std::string& path, 
                      SimulationStep s, Timestamp t)
            : label(lbl), filepath(path), step(s), time(t), 
              timestamp(std::chrono::system_clock::now()) {}
    };
    
    std::vector<CheckpointInfo> checkpoints_;
};

} // namespace nlm