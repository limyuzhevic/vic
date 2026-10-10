// Diff tracker - change tracking between checkpoints
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <functional>
#include "CheckpointSystem.hpp"

namespace nlm {

// Change entry
struct ChangeEntry {
    std::string changeId;
    uint64_t sourceCheckpointStep;
    uint64_t targetCheckpointStep;
    double timestamp;
    std::string changeType; // "neuron_state", "synapse_state", "connectivity", "plasticity", "memory"
    std::string description;
    double magnitude;
    std::vector<float> impactMetrics;
    
    ChangeEntry() : sourceCheckpointStep(0), targetCheckpointStep(0), timestamp(0.0), magnitude(0.0) {}
};

// Diff result
struct DiffResult {
    bool success;
    std::string message;
    uint64_t totalChanges;
    uint64_t significantChanges;
    std::vector<ChangeEntry> changes;
    double elapsedTime;
    time_t timestamp;
    
    DiffResult() : success(false), totalChanges(0), significantChanges(0), elapsedTime(0.0), timestamp(0) {}
    
    uint32_t getSignificantChangeCount() const {
        return significantChanges;
    }
};

// Diff tracking options
struct DiffTrackingOptions {
    bool trackNeuronStates;
    bool trackSynapseStates;
    bool trackConnectivity;
    bool trackPlasticity;
    bool trackSpikeHistory;
    bool trackMemoryStates;
    bool trackNeuromodulation;
    bool trackDevelopment;
    float significanceThreshold;
    std::chrono::seconds timeout;
    
    DiffTrackingOptions() : trackNeuronStates(true), trackSynapseStates(true), trackConnectivity(true),
                           trackPlasticity(true), trackSpikeHistory(true), trackMemoryStates(true),
                           trackNeuromodulation(true), trackDevelopment(true), significanceThreshold(0.1),
                           timeout(std::chrono::seconds(30)) {}
};

// Diff statistics
struct DiffTrackerStats {
    uint64_t totalDiffsComputed;
    uint64_t successfulDiffs;
    uint64_t failedDiffs;
    uint64_t totalChangesRecorded;
    double averageDiffTime;
    uint32_t errorCount;
    
    DiffTrackerStats() : totalDiffsComputed(0), successfulDiffs(0), failedDiffs(0),
                        totalChangesRecorded(0), averageDiffTime(0.0), errorCount(0) {}
};

// Main diff tracker
class DiffTracker {
public:
    DiffTracker();
    ~DiffTracker();
    
    // Configuration
    bool configure(const std::string& diffDir, const std::shared_ptr<Config>& config);
    void shutdown();
    
    // Diff operations
    bool computeDiff(const std::string& sourceCheckpoint, const std::string& targetCheckpoint,
                    std::vector<ChangeEntry>& changes, const DiffTrackingOptions& options = DiffTrackingOptions());
    bool computeDiffBetweenSteps(uint64_t sourceStep, uint64_t targetStep,
                                std::vector<ChangeEntry>& changes, const DiffTrackingOptions& options = DiffTrackingOptions());
    bool getDiffHistory(const std::string& checkpointName, std::vector<ChangeEntry>& history);
    
    // Change analysis
    bool analyzeChangeImpact(const ChangeEntry& change, std::vector<float>& impactMetrics);
    bool categorizeChange(const ChangeEntry& change, std::string& category);
    bool estimateChangeSignificance(const ChangeEntry& change, float& significance);
    
    // Change management
    bool recordChange(const ChangeEntry& change);
    bool deleteChange(const std::string& changeId);
    std::vector<std::string> listChanges(const std::string& checkpointName) const;
    bool cleanupOldChanges();
    
    // Statistics and monitoring
    DiffTrackerStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Event handling
    using DiffEventCallback = std::function<void(const std::string&, const std::vector<ChangeEntry>&)>;
    void setDiffEventCallback(DiffEventCallback callback) { 
        diffEventCallback_ = callback; 
    }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    DiffResult executeDiffOperation(const std::string& operation,
                                   const std::string& sourceCheckpoint,
                                   const std::string& targetCheckpoint,
                                   const DiffTrackingOptions& options);
    
    bool computeNeuronStateDiff(const std::string& sourceCheckpoint, const std::string& targetCheckpoint,
                               std::vector<ChangeEntry>& changes);
    bool computeSynapseStateDiff(const std::string& sourceCheckpoint, const std::string& targetCheckpoint,
                                std::vector<ChangeEntry>& changes);
    bool computeConnectivityDiff(const std::string& sourceCheckpoint, const std::string& targetCheckpoint,
                                 std::vector<ChangeEntry>& changes);
    bool computePlasticityDiff(const std::string& sourceCheckpoint, const std::string& targetCheckpoint,
                              std::vector<ChangeEntry>& changes);
    bool computeSpikeHistoryDiff(const std::string& sourceCheckpoint, const std::string& targetCheckpoint,
                                 std::vector<ChangeEntry>& changes);
    bool computeMemoryStateDiff(const std::string& sourceCheckpoint, const std::string& targetCheckpoint,
                               std::vector<ChangeEntry>& changes);
    bool computeNeuromodulationDiff(const std::string& sourceCheckpoint, const std::string& targetCheckpoint,
                                   std::vector<ChangeEntry>& changes);
    bool computeDevelopmentDiff(const std::string& sourceCheckpoint, const std::string& targetCheckpoint,
                               std::vector<ChangeEntry>& changes);
    
    void updateStats(const DiffResult& result);
    void logEvent(const std::string& operation, const std::string& sourceCheckpoint,
                  const std::string& targetCheckpoint, const DiffResult& result);
    
    std::string getChangePath(const std::string& changeId) const;
    bool findCheckpointStep(const std::string& checkpointName, uint64_t& step) const;
    
    // Components
    std::unique_ptr<CheckpointIndexer> checkpointIndexer_;
    std::unique_ptr<CheckpointValidator> checkpointValidator_;
    
    // Configuration
    std::string diffDir_;
    std::shared_ptr<Config> config_;
    
    // State
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    DiffTrackerStats stats_;
    
    // Events
    DiffEventCallback diffEventCallback_;
};

} // namespace nlm
