#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <functional>
#include <chrono>
#include <thread>

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "TrainingLoop.hpp" // For TrainingExperience
#include "ReinforcementLearner.hpp" // For RLAlgorithm

namespace nlm {

// Resource allocation configuration
struct ResourceConfig {
    size_t cpuCores;
    size_t gpuMemory; // in GB
    size_t ramMemory; // in GB
    size_t maxConcurrentExperiments;
    float cpuUsageLimit;
    float gpuUsageLimit;
    float memoryUsageLimit;
    std::string priority; // "high", "medium", "low"
    
    ResourceConfig() : cpuCores(1), gpuMemory(0), ramMemory(8), maxConcurrentExperiments(1),
                      cpuUsageLimit(0.8f), gpuUsageLimit(0.9f), memoryUsageLimit(0.9f),
                      priority("medium") {}
};

// Simulation schedule entry
struct SimulationSchedule {
    std::string experimentId;
    std::string algorithmName;
    RLAlgorithm algorithm;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    size_t episodeCount;
    double estimatedDuration;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    ResourceConfig resources;
    bool active;
    bool completed;
    float progress; // 0.0 to 1.0
    double currentReward;
    size_t currentSteps;
    
    SimulationSchedule() : episodeCount(0), estimatedDuration(0.0), active(false),
                          completed(false), progress(0.0), currentReward(0.0), currentSteps(0) {}
};

// Experiment configuration for scheduling
struct ExperimentConfig {
    std::string experimentName;
    std::string description;
    RLAlgorithm algorithm;
    size_t episodeCount;
    std::shared_ptr<Config> config;
    ResourceConfig resources;
    std::vector<std::string> tags;
    std::chrono::system_clock::time_point scheduledTime;
    std::chrono::system_clock::duration priority;
    bool requiresGPU;
    bool distributedTraining;
    
    ExperimentConfig() : episodeCount(100), priority(std::chrono::seconds(0)), // FIFO
                        requiresGPU(false), distributedTraining(false) {}
};

// Training job for distributed execution
struct TrainingJob {
    std::string jobId;
    std::string experimentId;
    size_t jobIndex;
    size_t totalJobs;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<ReinforcementLearner> learner;
    size_t startEpisode;
    size_t endEpisode;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    bool completed;
    float progress;
    double totalReward;
    size_t totalSteps;
    
    TrainingJob() : jobIndex(0), totalJobs(1), startEpisode(0), endEpisode(0),
                   completed(false), progress(0.0), totalReward(0.0), totalSteps(0) {}
};

// ============= SIMULATION SCHEDULER CLASS IMPLEMENTATION =============

class SimulationScheduler {
public:
    SimulationScheduler();
    
    // Core scheduling methods
    bool scheduleSimulation(const ExperimentConfig& experiment);
    bool scheduleMultipleExperiments(const std::vector<ExperimentConfig>& experiments);
    
    // Resource management
    bool allocateResources(const ResourceConfig& resources);
    ResourceConfig getAvailableResources() const;
    std::vector<ResourceConfig> getResourceUsage() const;
    
    // Progress monitoring
    SimulationSchedule* getSimulationSchedule(const std::string& experimentId);
    std::vector<SimulationSchedule> getActiveSimulations() const;
    std::vector<SimulationSchedule> getCompletedSimulations() const;
    std::vector<TrainingJob> getDistributedJobs(const std::string& experimentId) const;
    
    // Schedule management
    bool startSimulation(const std::string& experimentId);
    bool pauseSimulation(const std::string& experimentId);
    bool resumeSimulation(const std::string& experimentId);
    bool cancelSimulation(const std::string& experimentId);
    bool completeSimulation(const std::string& experimentId);
    
    // Progress tracking
    bool updateSimulationProgress(const std::string& experimentId, float progress);
    bool updateJobProgress(const std::string& experimentId, size_t jobIndex, float progress);
    
    // Schedule adjustment
    bool adjustSchedule(const std::string& experimentId, const ResourceConfig& newResources);
    bool rescheduleSimulation(const std::string& experimentId, size_t newEpisodeCount);
    bool optimizeResourceAllocation();
    
    // Query methods
    bool isSimulationActive(const std::string& experimentId) const;
    bool isSimulationCompleted(const std::string& experimentId) const;
    size_t getActiveSimulationCount() const { return activeSimulations_.size(); }
    size_t getCompletedSimulationCount() const { return completedSimulations_.size(); }
    
    // Callbacks
    SimulationScheduler& setSimulationStartCallback(
        std::function<void(const SimulationSchedule&)> callback);
    SimulationScheduler& setSimulationProgressCallback(
        std::function<void(const SimulationSchedule&)> callback);
    SimulationScheduler& setSimulationCompleteCallback(
        std::function<void(const SimulationSchedule&)> callback);
    SimulationScheduler& setResourceAllocationCallback(
        std::function<void(const ResourceConfig&)> callback);
    
    // Statistics and reporting
    std::string getSchedulerStatus() const;
    std::string getResourceReport() const;
    std::string getSimulationReport(const std::string& experimentId) const;
    std::string getOverallScheduleReport() const;
    
    // Cleanup
    void cleanupCompletedSimulations();
    void stopAllSimulations();
    
    // Static factory methods
    static std::unique_ptr<SimulationScheduler> createDistributedScheduler(
        const ResourceConfig& masterResources,
        const std::vector<ResourceConfig>& workerResources);
    
    static std::unique_ptr<SimulationScheduler> createCloudScheduler(
        const std::string& cloudProvider,
        const std::string& region,
        const std::string& instanceType);
    
protected:
    // Internal methods
    bool canScheduleExperiment(const ExperimentConfig& experiment) const;
    ResourceConfig calculateRequiredResources(const ExperimentConfig& experiment) const;
    bool hasAvailableResources(const ResourceConfig& required) const;
    void allocateResourcesToSimulation(SimulationSchedule& simulation);
    void releaseResourcesFromSimulation(SimulationSchedule& simulation);
    std::vector<TrainingJob> createDistributedJobs(const SimulationSchedule& simulation);
    
    // Time management
    std::chrono::system_clock::time_point getCurrentTime() const;
    double calculateEstimatedDuration(const ExperimentConfig& experiment) const;
    
    // Simulation execution
    bool runSimulation(SimulationSchedule& simulation);
    bool runDistributedSimulation(SimulationSchedule& simulation);
    
    // Members
    std::vector<SimulationSchedule> scheduledSimulations_;
    std::vector<SimulationSchedule> activeSimulations_;
    std::vector<SimulationSchedule> completedSimulations_;
    
    ResourceConfig systemResources_;
    ResourceConfig availableResources_;
    
    std::mutex mutex_;
    
    // Callbacks
    std::function<void(const SimulationSchedule&)> simulationStartCallback_;
    std::function<void(const SimulationSchedule&)> simulationProgressCallback_;
    std::function<void(const SimulationSchedule&)> simulationCompleteCallback_;
    std::function<void(const ResourceConfig&)> resourceAllocationCallback_;
    
    // State
    bool running_;
    size_t simulationIdCounter_;
};

// ============= ADVANCED SIMULATION SCHEDULER FACTORY =============

class SimulationSchedulerFactory {
public:
    static std::unique_ptr<SimulationScheduler> createLocalScheduler(
        const ResourceConfig& systemResources);
    
    static std::unique_ptr<SimulationScheduler> createClusterScheduler(
        const std::vector<ResourceConfig>& workerResources,
        const ResourceConfig& masterResources);
    
    static std::unique_ptr<SimulationScheduler> createCloudScheduler(
        const std::string& provider,
        const std::string& region,
        const std::string& credentials);
    
    static std::vector<ExperimentConfig> optimizeExperimentSchedule(
        const std::vector<ExperimentConfig>& experiments,
        const ResourceConfig& availableResources);
    
    static std::string generateExperimentId(const std::string& prefix = "exp");
    static std::chrono::system_clock::duration calculatePriority(
        const ExperimentConfig& experiment, const ResourceConfig& resources);
};

} // namespace nlm
