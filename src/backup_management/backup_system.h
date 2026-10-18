#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <future>
#include <numeric>
#include <algorithm>
#include <random>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <fstream>

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "memory_manager.h"
#include "checkpoint_manager.h"

namespace nlm {

// Network backup configuration
struct NetworkBackupConfig {
    std::string host;
    int port;
    std::string username;
    std::string password;
    std::string keyPath;
    bool useSSL;
    int timeout;
    
    NetworkBackupConfig() : port(22), useSSL(true), timeout(30) {}
};

// Remote backup result
struct RemoteBackupResult {
    bool success;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    
    RemoteBackupResult(bool success, const std::string& message) 
        : success(success), message(message), 
          timestamp(std::chrono::system_clock::now()) {}
};

// Forward declarations
class MemoryManager;
class CheckpointManager;

class BackupSystem {
public:
    BackupSystem();
    ~BackupSystem();
    
    // Backup Configuration
    void setBackupDirectory(const std::string& directory);
    void setBackupFrequency(size_t frequency);
    void setBackupEncryption(bool enabled);
    void setMaxBackups(size_t maxBackups);
    
    // Backup Operations
    std::string createBackup(const std::string& description = "");
    bool restoreBackup(const std::string& backupId);
    bool deleteBackup(const std::string& backupId);
    std::vector<std::string> getBackupIds() const;
    BackupMetadata getBackupInfo(const std::string& backupId) const;
    
    // Backup Validation
    bool validateBackup(const std::string& backupId) const;
    bool validateAllBackups() const;
    std::string getBackupValidationReport() const;
    ValidationResult validateBackupDetails(const std::string& backupId) const;
    
    // Remote Backup
    void configureRemoteBackup(const std::string& host, int port, const std::string& username);
    bool backupToRemote(const std::string& backupId);
    bool restoreFromRemote(const std::string& backupId);
    bool testRemoteConnection() const;
    
    // Automatic Backup
    void enableAutomaticBackup(bool enable);
    bool isAutomaticBackupEnabled() const;
    void setBackupSchedule(size_t frequencySeconds);
    
    // Integration
    void setMemoryManager(MemoryManager* manager) { memoryManager_ = manager; }
    void setCheckpointManager(CheckpointManager* manager) { checkpointManager_ = manager; }
    
    // Statistics and Monitoring
    size_t getTotalBackups() const;
    size_t getActiveBackups() const;
    size_t getTotalBackupStorage() const;
    std::vector<BackupMetadata> getAllBackupsInfo() const;
    std::chrono::system_clock::time_point getLastBackupTime() const;
    size_t getBackupsSince(size_t minutes) const;
    
    // Cleanup and Maintenance
    void cleanupOldBackups(size_t maxBackups);
    void compactBackups();
    void rebuildBackupIndex();
    
    // Batch Operations
    std::vector<std::string> createBatchBackups(size_t count, const std::string& baseDescription = "");
    bool restoreBatchBackups(const std::vector<std::string>& backupIds);
    
    // Remote Operations
    std::vector<RemoteBackupResult> backupToMultipleRemote(const std::vector<std::string>& hosts);
    bool restoreFromMultipleRemote(const std::vector<std::string>& hosts, const std::string& targetBackupId);
    
    // Error Handling
    std::string getLastError() const { return lastError_; }
    void clearError() { lastError_.clear(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    MemoryManager* memoryManager_ = nullptr;
    CheckpointManager* checkpointManager_ = nullptr;
    mutable std::shared_mutex backupMutex_;
    
    std::string lastError_;
};

} // namespace nlm
