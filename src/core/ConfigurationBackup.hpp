#pragma once

/**
 * Configuration Backup
 * 
 * Configuration backup and restore system for creating backups, snapshots,
 * and restoring configurations for the NLM brain simulation project.
 * 
 * Features:
 * - Configuration backup creation
 * - Configuration snapshot management
 * - Configuration restore
 * - Configuration rollback
 * - Configuration backup validation
 * - Configuration backup compression
 * - Configuration backup encryption
 * - Configuration backup versioning
 * - Configuration backup monitoring
 * - Configuration backup performance optimization
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <fstream>

namespace nlm {
namespace core {

class ConfigurationManager;

/**
 * Backup type
 */
enum class BackupType {
    FULL,
    INCREMENTAL,
    DIFFERENTIAL,
    SNAPSHOT,
    POINT_IN_TIME
};

/**
 * Backup status
 */
enum class BackupStatus {
    CREATING,
    CREATED,
    RESTORING,
    RESTORED,
    FAILED,
    CORRUPTED
};

/**
 * Backup metadata
 */
struct BackupMetadata {
    std::string backupId;
    BackupType type;
    BackupStatus status;
    std::string description;
    double createdAt;
    double size;
    std::string checksum;
    std::string compressionAlgorithm;
    bool isEncrypted;
    
    BackupMetadata(const std::string& id, BackupType t)
        : backupId(id), type(t), status(BackupStatus::CREATING), createdAt(0.0),
          size(0.0), isEncrypted(false) {}
};

/**
 * Configuration backup
 */
class ConfigurationBackup {
public:
    ConfigurationBackup();
    ~ConfigurationBackup();
    
    // Disable copying, enable moving
    ConfigurationBackup(const ConfigurationBackup&) = delete;
    ConfigurationBackup& operator=(const ConfigurationBackup&) = delete;
    ConfigurationBackup(ConfigurationBackup&&) noexcept = default;
    ConfigurationBackup& operator=(ConfigurationBackup&&) noexcept = default;
    
    // Backup creation
    std::string createBackup(ConfigurationManager& configManager, BackupType type = BackupType::FULL,
                            const std::string& description = "");
    bool createIncrementalBackup(ConfigurationManager& configManager, const std::string& baseBackupId);
    bool createSnapshotBackup(ConfigurationManager& configManager, const std::string& snapshotName);
    
    // Backup validation
    bool validateBackup(const std::string& backupId) const;
    std::vector<std::string> getBackupErrors(const std::string& backupId) const;
    
    // Backup restoration
    bool restoreBackup(const std::string& backupId, ConfigurationManager& configManager);
    bool restoreBackupToFile(const std::string& backupId, const std::string& outputFile);
    bool restoreSnapshot(const std::string& snapshotName, ConfigurationManager& configManager);
    
    // Backup management
    bool removeBackup(const std::string& backupId);
    bool updateBackupMetadata(const std::string& backupId, const BackupMetadata& metadata);
    BackupMetadata* getBackupMetadata(const std::string& backupId);
    const BackupMetadata* getBackupMetadata(const std::string& backupId) const;
    std::vector<std::string> getBackupIds() const;
    
    // Backup filtering
    std::vector<std::string> getBackupsByType(BackupType type) const;
    std::vector<std::string> getBackupsByStatus(BackupStatus status) const;
    std::vector<std::string> getBackupsByDate(double afterDate, double beforeDate) const;
    
    // Backup compression
    bool enableCompression(const std::string& backupId, const std::string& algorithm);
    bool disableCompression(const std::string& backupId);
    bool isCompressionEnabled(const std::string& backupId) const;
    
    // Backup encryption
    bool enableEncryption(const std::string& backupId, const std::string& encryptionKey);
    bool disableEncryption(const std::string& backupId);
    bool isEncryptionEnabled(const std::string& backupId) const;
    
    // Backup versioning
    bool incrementBackupVersion(const std::string& backupId);
    std::vector<std::string> getBackupVersions(const std::string& backupId) const;
    bool restoreBackupVersion(const std::string& backupId, const std::string& version);
    
    // Backup monitoring
    bool enableBackupMonitoring(const std::string& backupId, bool enable);
    bool isBackupMonitoringEnabled(const std::string& backupId) const;
    
    // Backup statistics
    size_t getBackupCount() const { return backups_.size(); }
    double getTotalBackupSize() const;
    double getAverageBackupSize() const;
    
    // Backup loading/saving
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);
    
    // Backup cleanup
    bool cleanupOldBackups(double beforeDate);
    bool cleanupCorruptedBackups();
    bool cleanupFailedBackups();
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Debug and diagnostics
    std::string dump() const;
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    mutable std::mutex mutex_;
    
    std::map<std::string, BackupMetadata> backups_;
    
    std::string generateBackupId() const;
    double calculateBackupSize(const std::string& backupId) const;
    std::string calculateBackupChecksum(const std::string& backupId) const;
    
    friend class ConfigurationManager;
};

} // namespace core
} // namespace nlm
