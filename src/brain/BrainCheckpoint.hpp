#pragma once

#include "../performance/CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class Brain;

class BrainCheckpoint {
public:
    BrainCheckpoint() = default;
    ~BrainCheckpoint() = default;
    
    // Configuration and setup
    void configureFromConfig(const Config* config);
    void setCheckpointParameters(size_t maxCheckpoints, size_t checkpointInterval, bool autoSave);
    
    // Checkpoint management
    bool saveCheckpoint(Brain* brain, const std::string& filepath);
    bool loadCheckpoint(Brain* brain, const std::string& filepath);
    bool autoSaveCheckpoint(Brain* brain, Timestamp currentTime);
    void manageCheckpointLifecycle(Brain* brain, Timestamp currentTime);
    
    // Checkpoint access and control
    CheckpointManager* getCheckpointManager() { return checkpointManager_.get(); }
    
    // Checkpoint statistics and diagnostics
    void collectCheckpointStats(Brain* brain);
    size_t getCheckpointCount() const { return checkpointCount_; }
    float getCheckpointRetentionRate() const { return checkpointRetentionRate_; }
    Timestamp getLastCheckpointTime() const { return lastCheckpointTime_; }
    std::vector<std::string> getCheckpointFiles() const { return checkpointFiles_; }
    
    // Checkpoint system management
    void resetCheckpointSystem(Brain* brain);
    void cleanupOldCheckpoints(Brain* brain);
    void backupImportantCheckpoints(Brain* brain);
    void restoreFromBackup(Brain* brain, const std::string& backupFile);
    
    // Checkpoint-specific processing
    bool createCheckpointArchive(Brain* brain, const std::string& archiveName);
    bool extractCheckpointArchive(Brain* brain, const std::string& archiveName, const std::string& extractDir);
    bool verifyCheckpointIntegrity(Brain* brain, const std::string& checkpointFile);
    void optimizeCheckpointStorage(Brain* brain);
    
private:
    std::unique_ptr<CheckpointManager> checkpointManager_;
    
    // Checkpoint statistics
    size_t checkpointCount_ = 0;
    float checkpointRetentionRate_ = 0.8f;
    Timestamp lastCheckpointTime_ = 0.0;
    std::vector<std::string> checkpointFiles_;
    
    // Configuration parameters
    size_t maxCheckpoints_ = 10;
    size_t checkpointInterval_ = 1000;
    bool autoSave_ = true;
    float checkpointCompression_ = 0.5f;
    size_t checkpointMaxSize_ = 100 * 1024 * 1024;  // 100 MB
    bool verifyChecksums_ = true;
    
    // Helper methods
    void initializeCheckpointSystem(Brain* brain);
    void validateCheckpointState(Brain* brain) const;
    void updateCheckpointConnections(Brain* brain);
    void processCheckpointLearning(Brain* brain, TimestepDuration timestep);
    
    friend class BrainCore; // For internal access
};

} // namespace nlm