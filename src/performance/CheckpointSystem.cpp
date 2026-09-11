#include "CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

class CheckpointSystem {
public:
    CheckpointSystem();
    ~CheckpointSystem();
    
    // Save checkpoint with compression
    bool saveCheckpoint(const std::string& name = "", CompressionLevel level = CompressionLevel::Balanced);
    
    // Load checkpoint
    bool loadCheckpoint(const std::string& name);
    
    // Delete checkpoint
    bool deleteCheckpoint(const std::string& name);
    
    // List available checkpoints
    std::vector<CheckpointInfo> listCheckpoints() const;
    
    // Get checkpoint directory
    const std::string& getCheckpointDir() const { return checkpointDir_; }
    
    // Set checkpoint directory
    void setCheckpointDir(const std::string& dir);
    
    // Configure checkpoint system
    void configure(uint64_t saveIntervalSteps = 10000, size_t maxCheckpoints = 10, bool compress = true);
    
    // Get current checkpoint info
    CheckpointInfo getCurrentCheckpointInfo() const;
    
    // Validate checkpoint file
    bool validateCheckpoint(const std::string& filepath) const;
    
    // Get checkpoint file size
    uint64_t getCheckpointFileSize(const std::string& name) const;
    
    // Compact checkpoint directory (remove old checkpoints)
    void compactCheckpoints();
    
    // Export checkpoint metadata
    std::string exportMetadata(const std::string& name) const;
    
    // Import checkpoint metadata
    bool importMetadata(const std::string& name, const std::string& metadata);
    
    // Backup checkpoints to remote storage
    bool backupCheckpoints(const std::string& remotePath);
    
    // Restore checkpoints from remote storage
    bool restoreCheckpoints(const std::string& remotePath);
    
    // Create checkpoint snapshot (hard link or copy)
    bool createSnapshot(const std::string& sourceName, const std::string& snapshotName = "");
    
    // Merge checkpoints
    bool mergeCheckpoints(const std::vector<std::string>& sourceNames, const std::string& targetName);
    
    // Get checkpoint statistics
    struct CheckpointStats {
        uint64_t totalCheckpoints = 0;
        uint64_t totalSize = 0;
        uint64_t oldestStep = 0;
        uint64_t newestStep = 0;
        double averageTimePerCheckpoint = 0.0;
    };
    
    CheckpointStats getCheckpointStats() const;
    
    // Clean up old checkpoints (keep most recent N)
    void pruneOldCheckpoints(size_t keepCount);
    
    // Set checkpoint compression level
    void setCompressionLevel(CompressionLevel level) { compressionLevel_ = level; }
    
    // Get current compression level
    CompressionLevel getCompressionLevel() const { return compressionLevel_; }
    
    // Enable/disable checkpointing
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    // Get checkpoint status
    bool isCheckpointing() const { return enabled_ && (lastSaveStep_ > 0); }
    
    // Get last checkpoint save time
    uint64_t getLastSaveStep() const { return lastSaveStep_; }
    
    // Get time since last checkpoint
    uint64_t getTimeSinceLastCheckpoint() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string checkpointDir_;
    uint64_t saveIntervalSteps_;
    size_t maxCheckpoints_;
    bool compress_;
    bool enabled_;
    uint64_t lastSaveStep_;
    CompressionLevel compressionLevel_;
};

} // namespace nlm