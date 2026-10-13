#pragma once

#include "../core/CheckpointManager.hpp"
#include "BrainInitializer.hpp"
#include <string>
#include <memory>

namespace nlm {

class Brain;

class BrainPersistenceManager {
public:
    explicit BrainPersistenceManager(std::shared_ptr<Config> config);
    ~BrainPersistenceManager() = default;
    
    // Checkpoint operations
    bool saveBrainState(Brain* brain, const std::string& filepath) const;
    bool loadBrainState(Brain* brain, const std::string& filepath);
    
    // Checkpoint management
    void configureCheckpoint(Brain* brain, const std::string& directory, 
                           size_t maxCheckpoints, size_t checkpointInterval,
                           bool compressionEnabled);
    
    // Checkpoint query and status
    bool hasCheckpoint(Brain* brain, const std::string& filepath) const;
    std::string getCheckpointDirectory() const { return checkpointDirectory; }
    size_t getMaxCheckpoints() const { return maxCheckpoints; }
    size_t getCheckpointInterval() const { return checkpointInterval; }
    bool isCompressionEnabled() const { return compressionEnabled; }
    
    // Checkpoint validation
    bool validateCheckpoint(Brain* brain, const std::string& filepath) const;
    std::string getCheckpointError() const { return checkpointError; }
    
    // Checkpoint cleanup
    void cleanupOldCheckpoints(Brain* brain);
    void removeCheckpoint(Brain* brain, const std::string& filepath);
    
    // Brain statistics for checkpoint metadata
    struct BrainStatistics {
        size_t totalNeuronCount;
        size_t totalSynapseCount;
        size_t regionCount;
        SimulationStep currentStep;
        Timestamp currentTime;
        size_t totalSpikes;
        float averageFiringRate;
        float excitationInhibitionRatio;
    };
    
    BrainStatistics collectBrainStatistics(Brain* brain) const;
    
    // Advanced persistence features
    void enableIncrementalBackup(Brain* brain, bool enabled);
    void enableCheckpointCompression(Brain* brain, bool enabled);
    void enableDiffStorage(Brain* brain, bool enabled);
    
    // State management
    void resetPersistenceManager(Brain* brain);
    void updateCheckpointInterval(Brain* brain, size_t newInterval);
    void setCheckpointDirectory(Brain* brain, const std::string& directory);

private:
    // Checkpoint manager
    std::unique_ptr<CheckpointManager> checkpointManager;
    
    // Configuration parameters
    std::string checkpointDirectory;
    size_t maxCheckpoints;
    size_t checkpointInterval;
    bool compressionEnabled;
    bool incrementalBackupEnabled;
    bool diffStorageEnabled;
    std::string checkpointError;
    
    // Helper methods
    void createCheckpointDirectory(const std::string& directory);
    void writeBrainMetadata(Brain* brain, const BrainStatistics& stats) const;
    void readBrainMetadata(Brain* brain, BrainStatistics& stats) const;
    void validateCheckpointData(Brain* brain, const std::string& filepath) const;
    void restoreBrainStateFromCheckpoint(Brain* brain, const std::string& filepath);
    
    // Advanced features
    void enableIncrementalBackup(Brain* brain);
    void enableCheckpointCompression(Brain* brain);
    void enableDiffStorage(Brain* brain);
    
    // Compression and storage helper methods
    void compressCheckpoint(const std::string& filepath) const;
    void storeCheckpointDiff(const std::string& oldFile, const std::string& newFile) const;
    void createIncrementalBackup(const std::string& filepath) const;
};