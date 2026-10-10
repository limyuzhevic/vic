// Backup manager - backup and recovery management for checkpoints
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <mutex>
#include "CheckpointSystem.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Backup status
enum class BackupStatus {
    Inactive,
    Creating,
    Restoring,
    Validating,
    Error,
    Complete
};

// Backup result
struct BackupResult {
    bool success;
    std::string message;
    uint64_t originalSize;
    uint64_t backupSize;
    double compressionRatio;
    time_t timestamp;
    
    BackupResult() : success(false), originalSize(0), backupSize(0), 
                     compressionRatio(0.0), timestamp(0) {}
};

// Recovery options
struct RecoveryOptions {
    std::string targetPath;
    bool overwriteExisting;
    bool validateAfterRecovery;
    bool restorePermissions;
    std::chrono::seconds timeout;
    
    RecoveryOptions() : overwriteExisting(false), validateAfterRecovery(true),
                       restorePermissions(false), timeout(std::chrono::seconds(60)) {}
};

// Backup strategy
enum class BackupStrategy {
    Full,
    Incremental,
    Differential,
    Mirrored
};

// Main backup manager
class BackupManager {
public:
    BackupManager();
    ~BackupManager();
    
    // Configuration
    bool configure(const std::string& backupDir, const std::shared_ptr<Config>& config);
    void shutdown();
    
    // Backup operations
    bool createBackup(const std::string& checkpointName, const CheckpointOptions& options = CheckpointOptions());
    bool createFullBackup(const std::string& backupName);
    bool createIncrementalBackup(const std::string& backupName);
    bool createDifferentialBackup(const std::string& backupName);
    
    // Recovery operations
    bool restoreBackup(const std::string& backupName, const RecoveryOptions& options = RecoveryOptions());
    bool restoreToPointInTime(const std::string& checkpointName, const std::chrono::system_clock::time_point& timePoint);
    
    // Backup management
    bool deleteBackup(const std::string& backupName);
    std::vector<std::string> listBackups() const;
    bool validateBackup(const std::string& backupName);
    
    // Strategy management
    void setBackupStrategy(BackupStrategy strategy) { strategy_ = strategy; }
    BackupStrategy getBackupStrategy() const { return strategy_; }
    
    // Status and monitoring
    BackupStatus getStatus() const { return status_; }
    std::string getStatusString() const;
    bool isHealthy() const { return status_ == BackupStatus::Complete; }
    
    // Statistics
    struct BackupStats {
        uint64_t totalBackups;
        uint64_t successfulBackups;
        uint64_t failedBackups;
        uint64_t totalBackupSize;
        uint64_t totalRestoredSize;
        double averageCompressionRatio;
        
        BackupStats() : totalBackups(0), successfulBackups(0), failedBackups(0),
                       totalBackupSize(0), totalRestoredSize(0), averageCompressionRatio(0.0) {}
    };
    
    BackupStats getStats() const { return stats_; }
    
    // Event handling
    using BackupEventCallback = std::function<void(const std::string&, const BackupResult&)>;
    void setBackupEventCallback(BackupEventCallback callback) { 
        backupEventCallback_ = callback; 
    }
    
    // Validation
    bool validateConfiguration() const;
    bool validateBackups() const;
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    bool createBackupDirectory();
    std::string getBackupPath(const std::string& backupName) const;
    std::string getCheckpointPath(const std::string& checkpointName) const;
    
    bool compressBackup(const std::string& sourcePath, const std::string& targetPath);
    bool encryptBackup(const std::string& sourcePath, const std::string& targetPath);
    bool indexBackup(const std::string& backupName, const std::string& path);
    
    BackupResult executeBackupOperation(const std::string& operation, 
                                       const std::string& name,
                                       const CheckpointOptions& options);
    
    bool performIncrementalBackup(const std::string& backupName, const std::string& baseCheckpoint);
    bool performDifferentialBackup(const std::string& backupName, const std::string& baseCheckpoint);
    
    void updateStats(const BackupResult& result);
    void logEvent(const std::string& eventType, const std::string& backupName,
                  const BackupResult& result);
    
    // Components
    std::unique_ptr<CheckpointCompressor> checkpointCompressor_;
    std::unique_ptr<CheckpointEncryptor> checkpointEncryptor_;
    std::unique_ptr<CheckpointIndexer> checkpointIndexer_;
    
    // Configuration
    std::string backupDir_;
    std::shared_ptr<Config> config_;
    BackupStrategy strategy_;
    
    // State
    BackupStatus status_;
    mutable std::mutex mutex_;
    
    // Statistics
    BackupStats stats_;
    
    // Events
    BackupEventCallback backupEventCallback_;
    
    // Validation
    std::vector<std::string> validationErrors_;
};

} // namespace nlm
