// Advanced batch processing and agent management tools
// Provides tools for expert users to manage multiple agent instances
// and run complex batch experiments

#ifndef ADVANCED_BATCH_TOOLS_HPP
#define ADVANCED_BATCH_TOOLS_HPP

#include "../agent/AgentBrain.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace nlm {

/**
 * Agent manager for handling multiple agent instances in batch processing
 */
class AgentManager {
public:
    AgentManager();
    ~AgentManager();
    
    /**
     * Create a new agent with the specified configuration
     */
    std::shared_ptr<AgentBrain> createAgent(const std::shared_ptr<Brain>& brain,
                                            const SimpleWorld& world);
    
    /**
     * Remove an agent from management
     */
    bool removeAgent(const std::shared_ptr<AgentBrain>& agent);
    
    /**
     * Get all managed agents
     */
    std::vector<std::shared_ptr<AgentBrain>> getAllAgents() const;
    
    /**
     * Execute batch operations on all agents
     */
    struct BatchOperationResult {
        size_t totalAgents;
        size_t successfulOperations;
        size_t failedOperations;
        std::vector<std::string> errors;
        double executionTime;
    };
    
    BatchOperationResult executeBatchOperation(
        std::function<void(std::shared_ptr<AgentBrain>)> operation);
    
    /**
     * Synchronize all agents (useful for replay scenarios)
     */
    bool synchronizeAllAgents();
    
    /**
     * Get agent statistics
     */
    struct AgentStatistics {
        size_t agentId;
        std::string agentType;
        size_t neuronCount;
        size_t synapseCount;
        size_t activeTraces;
        float curiosityLevel;
        float noveltyLevel;
        float dopamineLevel;
        double lastUpdateTime;
    };
    
    std::vector<AgentStatistics> getAgentStatistics() const;
    
    /**
     * Batch save all agent states
     */
    bool batchSave(const std::string& directory);
    
    /**
     * Batch load all agent states
     */
    bool batchLoad(const std::string& directory);
    
    /**
     * Monitor agent performance
     */
    struct AgentPerformanceMetrics {
        double processingSpeed;  // steps per second
        double memoryUsage;      // MB
        double cpuUsage;         // %
        size_t totalSpikes;
        float averageFiringRate;
        double lastMeasurementTime;
    };
    
    std::unordered_map<size_t, AgentPerformanceMetrics> monitorAgentPerformance() const;
    
    /**
     * Get management statistics
     */
    struct ManagerStatistics {
        size_t totalAgents;
        size_t activeAgents;
        size_t inactiveAgents;
        double uptime;           // seconds
        size_t totalOperations;
        size_t successfulOperations;
        size_t failedOperations;
        double averageOperationTime;
    };
    
    ManagerStatistics getManagerStatistics() const;
    
    /**
     * Stop all agents
     */
    void stopAllAgents();
    
    /**
     * Start all agents
     */
    void startAllAgents();
    
    /**
     * Schedule agents for parallel processing
     */
    struct ParallelProcessingResult {
        bool success;
        std::vector<std::string> errors;
        double totalExecutionTime;
        size_t totalStepsProcessed;
    };
    
    ParallelProcessingResult parallelProcess(
        std::function<void(std::shared_ptr<AgentBrain>, size_t step)> processFunction,
        size_t steps,
        size_t numThreads = std::thread::hardware_concurrency());
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Batch experiment runner for managing complex experimental scenarios
 */
class BatchExperimentRunner {
public:
    BatchExperimentRunner();
    ~BatchExperimentRunner();
    
    /**
     * Add an agent to the batch experiment
     */
    bool addAgent(const std::shared_ptr<AgentBrain>& agent,
                  const std::string& experimentName,
                  const std::unordered_map<std::string, std::string>& parameters);
    
    /**
     * Remove an agent from the batch experiment
     */
    bool removeAgent(size_t agentId);
    
    /**
     * Run the batch experiment
     */
    struct BatchExperimentResult {
        bool success;
        size_t totalAgents;
        size_t completedAgents;
        size_t failedAgents;
        double totalExecutionTime;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Aggregate results
        float averageTotalReward;
        float averageFinalFiringRate;
        float averageMemoryRetention;
        size_t totalMemoryEpisodes;
    };
    
    BatchExperimentResult runBatchExperiment(size_t steps);
    
    /**
     * Run batch experiment with checkpointing
     */
    BatchExperimentResult runBatchExperimentWithCheckpointing(
        size_t steps,
        const std::string& checkpointDirectory,
        size_t checkpointInterval = 100);
    
    /**
     * Get batch experiment configuration
     */
    struct BatchExperimentConfig {
        size_t numAgents;
        size_t stepsPerAgent;
        bool enableCheckpointing;
        bool enableReplay;
        bool enableDevelopment;
        std::string checkpointDirectory;
        size_t checkpointInterval;
        size_t maxConcurrentAgents;
        std::string experimentName;
        std::unordered_map<std::string, std::string> globalParameters;
    };
    
    BatchExperimentConfig getConfig() const;
    
    /**
     * Set batch experiment configuration
     */
    void setConfig(const BatchExperimentConfig& config);
    
    /**
     * Get experiment progress
     */
    struct ExperimentProgress {
        bool running;
        size_t currentStep;
        size_t totalSteps;
        size_t completedAgents;
        size_t totalAgents;
        double estimatedTimeRemaining;
        std::string currentExperiment;
    };
    
    ExperimentProgress getProgress() const;
    
    /**
     * Cancel the current batch experiment
     */
    void cancelExperiment();
    
    /**
     * Pause the current batch experiment
     */
    void pauseExperiment();
    
    /**
     * Resume the current batch experiment
     */
    void resumeExperiment();
    
    /**
     * Get experiment results
     */
    std::vector<BatchExperimentResult> getExperimentResults() const;
    
    /**
     * Export experiment results
     */
    bool exportResults(const std::string& filepath, const std::string& format = "json");
    
    /**
     * Get agent manager instance
     */
    std::shared_ptr<AgentManager> getAgentManager() const;
    
    /**
     * Clean up completed agents
     */
    size_t cleanupCompletedAgents();
    
    /**
     * Reinitialize failed agents
     */
    bool reinitializeFailedAgents();
    
    /**
     * Generate experiment report
     */
    std::string generateExperimentReport() const;
    
    /**
     * Schedule agents for staggered execution (to avoid synchronization issues)
     */
    bool staggerAgentExecution(double offsetPerAgent = 0.1);
    
    /**
     * Run batch experiment with varying parameters (parameter sweep)
     */
    struct ParameterSweepResult {
        std::unordered_map<std::string, std::vector<float>> parameterValues;
        std::vector<BatchExperimentResult> results;
        bool success;
        std::vector<std::string> errors;
    };
    
    ParameterSweepResult runParameterSweep(
        const std::unordered_map<std::string, std::vector<float>>& parameterRanges,
        size_t stepsPerConfiguration);
    
    /**
     * Get experiment status
     */
    struct ExperimentStatus {
        bool running;
        bool paused;
        std::string currentState;
        time_t startTime;
        time_t lastUpdateTime;
        std::string currentExperimentName;
        size_t agentsRunning;
        size_t totalAgents;
    };
    
    ExperimentStatus getStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Enhanced batch tools for expert users
 */
class AdvancedBatchTools {
public:
    AdvancedBatchTools();
    ~AdvancedBatchTools();
    
    /**
     * Create an agent manager instance
     */
    std::shared_ptr<AgentManager> createAgentManager();
    
    /**
     * Create a batch experiment runner instance
     */
    std::shared_ptr<BatchExperimentRunner> createBatchExperimentRunner();
    
    /**
     * Run a comprehensive experiment suite
     */
    struct ComprehensiveExperimentResult {
        bool success;
        std::vector<std::string> errors;
        std::unordered_map<std::string, BatchExperimentResult> experimentResults;
        double totalExecutionTime;
        size_t totalAgentsProcessed;
    };
    
    ComprehensiveExperimentResult runComprehensiveExperimentSuite(
        const std::vector<BatchExperimentConfig>& experimentConfigs);
    
    /**
     * Export all agent states
     */
    bool exportAllAgentStates(const std::string& directory);
    
    /**
     * Import all agent states
     */
    bool importAllAgentStates(const std::string& directory);
    
    /**
     * Generate comprehensive system report
     */
    std::string generateSystemReport() const;
    
    /**
     * Monitor system health
     */
    struct SystemHealthReport {
        bool healthy;
        double systemUptime;
        size_t totalMemoryUsage;
        double cpuUsage;
        size_t activeAgents;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        time_t lastCheckTime;
    };
    
    SystemHealthReport getSystemHealth() const;
    
    /**
     * Generate performance benchmarks
     */
    struct SystemBenchmarks {
        double cpuBenchmark;           // instructions per second
        double memoryBandwidth;        // bytes per second
        double diskIO;                 // MB/s
        size_t maxConcurrentAgents;
        double avgAgentProcessingTime; // ms
        double avgAgentMemoryUsage;    // MB
    };
    
    SystemBenchmarks generateBenchmarks() const;
    
    /**
     * Schedule periodic maintenance tasks
     */
    bool scheduleMaintenanceTask(
        std::function<void()> task,
        std::chrono::duration<double> interval);
    
    /**
     * Get tool statistics
     */
    struct ToolStatistics {
        size_t totalAgentManagersCreated;
        size_t totalBatchExperimentRunnersCreated;
        double totalExecutionTime;
        size_t totalAgentsProcessed;
        size_t totalExperimentsRun;
        time_t sessionStartTime;
    };
    
    ToolStatistics getStatistics() const;
    
    /**
     * Clean up resources
     */
    void cleanup();
    
    /**
     * Get singleton instance
     */
    static std::shared_ptr<AdvancedBatchTools> getInstance();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // ADVANCED_BATCH_TOOLS_HPP