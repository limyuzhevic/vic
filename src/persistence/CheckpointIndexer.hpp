// Checkpoint indexer - checkpoint indexing and search
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

// Checkpoint index entry
struct CheckpointIndexEntry {
    std::string name;
    uint64_t step;
    double simulationTime;
    uint64_t neuronCount;
    uint64_t synapseCount;
    uint64_t regionCount;
    uint64_t fileSize;
    time_t timestamp;
    uint64_t checksum;
    std::vector<std::string> tags;
    std::string description;
    
    // For fast search
    std::string nameLower; // Lowercase for case-insensitive search
    
    CheckpointIndexEntry() : step(0), simulationTime(0.0), neuronCount(0), synapseCount(0),
                            regionCount(0), fileSize(0), checksum(0), timestamp(0) {}
};

// Index search criteria
struct IndexSearchCriteria {
    std::vector<std::string> namePatterns;
    uint64_t minStep;
    uint64_t maxStep;
    double minSimulationTime;
    double maxSimulationTime;
    uint64_t minNeuronCount;
    uint64_t maxNeuronCount;
    std::vector<std::string> requiredTags;
    std::vector<std::string> excludedTags;
    time_t minTimestamp;
    time_t maxTimestamp;
    
    IndexSearchCriteria() : minStep(0), maxStep(UINT64_MAX), minSimulationTime(0.0), maxSimulationTime(DBL_MAX),
                          minNeuronCount(0), maxNeuronCount(UINT64_MAX), minTimestamp(0), maxTimestamp(UINT64_MAX) {}
};

// Index statistics
struct IndexStats {
    uint64_t totalCheckpoints;
    uint64_t entriesIndexed;
    uint64_t indexSize;
    double indexCreationTime;
    uint32_t errorCount;
    
    IndexStats() : totalCheckpoints(0), entriesIndexed(0), indexSize(0), indexCreationTime(0.0), errorCount(0) {}
};

// Main checkpoint indexer
class CheckpointIndexer {
public:
    CheckpointIndexer();
    ~CheckpointIndexer();
    
    // Indexing operations
    bool indexCheckpoint(const CheckpointIndexEntry& entry);
    bool updateCheckpoint(const std::string& name, const CheckpointIndexEntry& entry);
    bool removeCheckpoint(const std::string& name);
    bool clearIndex();
    
    // Search operations
    std::vector<CheckpointIndexEntry> search(const IndexSearchCriteria& criteria) const;
    std::vector<CheckpointIndexEntry> searchByName(const std::string& pattern) const;
    std::vector<CheckpointIndexEntry> searchByTag(const std::string& tag) const;
    std::vector<CheckpointIndexEntry> searchByTimeRange(time_t minTime, time_t maxTime) const;
    std::vector<CheckpointIndexEntry> searchByStepRange(uint64_t minStep, uint64_t maxStep) const;
    
    // Get specific checkpoints
    std::optional<CheckpointIndexEntry> getCheckpoint(const std::string& name) const;
    CheckpointIndexEntry getLatestCheckpoint() const;
    std::vector<CheckpointIndexEntry> getRecentCheckpoints(size_t limit = 10) const;
    
    // Management operations
    bool addTagToCheckpoint(const std::string& name, const std::string& tag);
    bool removeTagFromCheckpoint(const std::string& name, const std::string& tag);
    bool updateCheckpointDescription(const std::string& name, const std::string& description);
    
    // Statistics and monitoring
    IndexStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Event handling
    using IndexEventCallback = std::function<void(const std::string&, const CheckpointIndexEntry&)>;
    void setIndexEventCallback(IndexEventCallback callback) { 
        indexEventCallback_ = callback; 
    }
    
    // Configuration
    void configure(const std::string& indexDir, const std::shared_ptr<Config>& config);
    
    // Persistence
    bool saveIndex(const std::string& filepath) const;
    bool loadIndex(const std::string& filepath);
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    bool saveIndexToFile(const std::string& filepath) const;
    bool loadIndexFromFile(const std::string& filepath);
    
    void updateStats();
    void logEvent(const std::string& eventType, const std::string& checkpointName,
                  const CheckpointIndexEntry& entry);
    
    // Search helpers
    bool matchesCriteria(const CheckpointIndexEntry& entry, const IndexSearchCriteria& criteria) const;
    bool matchesNamePattern(const std::string& entryName, const std::string& pattern) const;
    bool matchesTagRequirement(const CheckpointIndexEntry& entry, const std::vector<std::string>& requiredTags,
                             const std::vector<std::string>& excludedTags) const;
    
    // Components
    std::unordered_map<std::string, CheckpointIndexEntry> index_;
    std::unordered_map<std::string, std::vector<std::string>> nameToTagsMap_;
    std::unordered_map<std::string, std::vector<std::string>> tagToNamesMap_;
    
    // Configuration
    std::string indexDir_;
    std::shared_ptr<Config> config_;
    
    // State
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    IndexStats stats_;
    
    // Events
    IndexEventCallback indexEventCallback_;
};

} // namespace nlm
