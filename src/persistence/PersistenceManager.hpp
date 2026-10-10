// Persistence manager - main controller for checkpoint system
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <atomic>

#include "CheckpointSystem.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Persistence status
enum class PersistenceStatus {
    Inactive,
    Initializing,
    Active,
    Saving,
    Loading,
    Error,
    Shutdown
};

// Persistence operation result
struct PersistenceResult {
    bool success;
    std::string message;
    uint64_t bytesProcessed;
    double elapsedTime;
    time_t timestamp;
    
    PersistenceResult() : success(false), bytesProcessed(0), elapsedTime(0.0), timestamp(0) {}
};

// Checkpoint options
struct CheckpointOptions {
    std::string name;
    CompressionLevel compression;
    bool validateAfterSave;
    bool backupBeforeSave;
    bool encrypt;
    std::string metadata;
    uint64_t expectedSize;
    std::chrono::seconds timeout;
    
    CheckpointOptions() : compression(CompressionLevel::Balanced), validateAfterSave(true),
                         backupBeforeSave(false), encrypt(false), expectedSize(0), 
                         timeout(std::chrono::seconds(30)) {}
};

// Backup policy
enum class BackupPolicy {
    None,
    Full,
    Incremental,
    Differential
};

// Retention policy
struct RetentionPolicy {
    uint32_t maxCheckpoints;
    std::chrono::days maxAge;
    uint64_t maxSize;
    bool autoPrune;
    
    RetentionPolicy() : maxCheckpoints(10), maxAge(std::chrono::days(30)),
                       maxSize(1024 * 1024 * 1024), autoPrune(true) {} // 1GB default
};

// Compression algorithm
enum class CompressionAlgorithm {
    None,
    Zlib,
    LZ4,
    Snappy,
    Zstd
};

// Encryption algorithm
enum class EncryptionAlgorithm {
    None,
    AES128,
    AES256,
    ChaCha20
};

// Main persistence manager
class PersistenceManager {
public:
    PersistenceManager();
    ~PersistenceManager();
    
    // Configuration
    bool configure(const std::string& dataDir, const std::shared_ptr<Config>& config);
    void shutdown();
    
    // Checkpoint operations
    bool createCheckpoint(const std::string& name, const CheckpointOptions& options = CheckpointOptions());
    bool saveCheckpoint(const std::string& name, const CheckpointOptions& options = CheckpointOptions());
    bool loadCheckpoint(const std::string& name);
    bool deleteCheckpoint(const std::string& name);
    
    std::vector<std::string> listCheckpoints() const;
    bool validateCheckpoint(const std::string& name);
    
    // Resume operations
    bool resumeFromCheckpoint(const std::string& name);
    bool createIncrementalCheckpoint();
    std::string getLastCheckpoint() const;
    std::vector<std::string> getCheckpointHistory(size_t limit = 10) const;
    
    // Persistence operations
    bool saveToFile(const std::string& path);
    bool loadFromFile(const std::string& path);
    bool exportToJSON(const std::string& path);
    bool importFromJSON(const std::string& path);
    
    // Management operations
    void setBackupPolicy(BackupPolicy policy) { backupPolicy_ = policy; }
    void setCompression(CompressionAlgorithm algorithm) { compressionAlgorithm_ = algorithm; }
    void setEncryption(EncryptionAlgorithm algorithm) { encryptionAlgorithm_ = algorithm; }
    void setRetentionPolicy(const RetentionPolicy& policy) { retentionPolicy_ = policy; }
    
    // Status and monitoring
    PersistenceStatus getStatus() const { return status_; }
    std::string getStatusString() const;
    bool isHealthy() const { return status_ == PersistenceStatus::Active && errorCount_ == 0; }
    
    uint64_t getTotalCheckpoints() const;
    uint64_t getTotalSize() const;
    double getUptime() const;
    
    // Event handling
    using CheckpointEventCallback = std::function<void(const std::string&, const PersistenceResult&)>;
    void setCheckpointEventCallback(CheckpointEventCallback callback) { 
        checkpointEventCallback_ = callback; 
    }
    
    // Statistics
    struct PersistenceStats {
        uint64_t checkpointsCreated;
        uint64_t checkpointsLoaded;
        uint64_t checkpointsDeleted;
        uint64_t bytesWritten;
        uint64_t bytesRead;
        double totalTimeSpent;
        uint32_t errorCount;
        
        PersistenceStats() : checkpointsCreated(0), checkpointsLoaded(0), checkpointsDeleted(0),
                           bytesWritten(0), bytesRead(0), totalTimeSpent(0.0), errorCount(0) {}
    };
    
    PersistenceStats getStats() const { return stats_; }
    
    // Validation
    bool validateConfiguration() const;
    bool validateCheckpoints() const;
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    bool createCheckpointDirectory();
    bool setupCompression() { return true; } // Placeholder
    bool setupEncryption() { return true; } // Placeholder
    bool setupIndexing() { return true; } // Placeholder
    
    PersistenceResult executeCheckpointOperation(const std::string& operation, 
                                                const std::string& name,
                                                const CheckpointOptions& options);
    
    bool validateCheckpointIntegrity(const std::string& checkpointName);
    bool performBackup(const std::string& checkpointName);
    bool applyRetentionPolicy();
    
    void updateStats(const PersistenceResult& result);
    void logEvent(const std::string& eventType, const std::string& checkpointName,
                  const PersistenceResult& result);
    
    void setStatus(PersistenceStatus status) {
        status_ = status;
        statusChangeTime_ = std::chrono::steady_clock::now();
    }
    
    std::string generateCheckpointName(const std::string& baseName = "checkpoint") const;
    std::string getCheckpointPath(const std::string& name) const;
    std::string getBackupPath(const std::string& checkpointName) const;
    
    // Components
    std::unique_ptr<CheckpointManager> checkpointManager_;
    std::unique_ptr<CheckpointValidator> checkpointValidator_;
    std::unique_ptr<CheckpointIndexer> checkpointIndexer_;
    std::unique_ptr<CheckpointCompressor> checkpointCompressor_;
    std::unique_ptr<CheckpointEncryptor> checkpointEncryptor_;
    std::unique_ptr<ResumeManager> resumeManager_;
    std::unique_ptr<BackupManager> backupManager_;
    std::unique_ptr<MemoryPersistence> memoryPersistence_;
    std::unique_ptr<NeuralStatePersistence> neuralStatePersistence_;
    std::unique_ptr<ConfigurationPersistence> configurationPersistence_;
    std::unique_ptr<NeuromodulationPersistence> neuromodulationPersistence_;
    std::unique_ptr<DevelopmentPersistence> developmentPersistence_;
    std::unique_ptr<VersionControl> versionControl_;
    std::unique_ptr<DiffTracker> diffTracker_;
    
    // Configuration
    std::string dataDir_;
    std::shared_ptr<Config> config_;
    BackupPolicy backupPolicy_;
    CompressionAlgorithm compressionAlgorithm_;
    EncryptionAlgorithm encryptionAlgorithm_;
    RetentionPolicy retentionPolicy_;
    
    // State
    PersistenceStatus status_;
    time_t startTime_;
    std::chrono::steady_clock::time_point statusChangeTime_;
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    PersistenceStats stats_;
    
    // Events
    CheckpointEventCallback checkpointEventCallback_;
    
    // Validation
    std::vector<std::string> validationErrors_;
};

} // namespace nlm
