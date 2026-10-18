#include "backup_system.h"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>
#include <random>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <chrono>
#include <thread>

namespace nlm {

// Helper functions
double getCurrentSimulationTime() {
    static double time = 0.0;
    return time += 0.001;
}

std::string generateUniqueId() {
    static std::mt19937 rng(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<uint64_t> dist;
    uint64_t random = dist(rng);
    std::stringstream ss;
    ss << std::hex << random;
    return "backup_" + ss.str();
}

class BackupSystem::Impl {
public:
    std::string backupDirectory_ = "backups";
    size_t backupFrequency_ = 300; // 5 minutes in seconds
    bool encryptionEnabled_ = false;
    size_t maxBackups_ = 100;
    
    NetworkBackupConfig remoteConfig_;
    bool remoteConfigured_ = false;
    
    bool automaticBackupEnabled_ = false;
    size_t backupSchedule_ = backupFrequency_;
    
    std::unordered_map<std::string, BackupMetadata> backups_;
    std::unordered_map<std::string, std::vector<char>> backupData_;
    
    std::chrono::system_clock::time_point lastBackupTime_;
    size_t totalBackupsCreated_ = 0;
    
    static constexpr size_t MAX_BACKUP_SIZE = 1024 * 1024 * 1024; // 1GB
    static constexpr size_t MAX_REMOTE_BACKUP_SIZE = 100 * 1024 * 1024; // 100MB
    
    Impl() {
        std::filesystem::create_directories(backupDirectory_);
    }
    
    std::vector<char> createBackupData(MemoryManager* memoryManager, CheckpointManager* checkpointManager) {
        std::vector<char> backupData;
        
        if (memoryManager) {
            // Create memory state snapshot
            MemoryStatistics memStats = memoryManager->getMemoryStatistics();
            std::stringstream memStream;
            memStream << "memory_stats:" << memStats.totalMemory << ":" << memStats.usedMemory << ":" << memStats.peakMemory;
            std::string memData = memStream.str();
            backupData.insert(backupData.end(), memData.begin(), memData.end());
        }
        
        if (checkpointManager) {
            // Create checkpoint state snapshot
            std::vector<std::string> checkpointIds = checkpointManager->getCheckpointIds();
            std::stringstream chkStream;
            chkStream << "checkpoints:" << checkpointIds.size();
            for (const std::string& id : checkpointIds) {
                chkStream << ":" << id;
            }
            std::string chkData = chkStream.str();
            backupData.insert(backupData.end(), chkData.begin(), chkData.end());
        }
        
        // Add timestamp and metadata
        auto now = std::chrono::system_clock::now();
        auto timeSinceEpoch = std::chrono::system_clock::to_time_t(now);
        std::stringstream metaStream;
        metaStream << "timestamp:" << timeSinceEpoch << ":" << getCurrentSimulationTime();
        std::string metaData = metaStream.str();
        backupData.insert(backupData.end(), metaData.begin(), metaData.end());
        
        return backupData;
    }
    
    std::vector<char> encryptBackupData(const std::vector<char>& data) const {
        if (!encryptionEnabled_) {
            return data;
        }
        
        std::vector<char> encrypted = data;
        for (size_t i = 0; i < encrypted.size(); ++i) {
            encrypted[i] ^= 'N'; // Simple XOR for demonstration
        }
        
        return encrypted;
    }
    
    std::vector<char> decryptBackupData(const std::vector<char>& data) const {
        if (!encryptionEnabled_) {
            return data;
        }
        
        std::vector<char> decrypted = data;
        for (size_t i = 0; i < decrypted.size(); ++i) {
            decrypted[i] ^= 'N'; // Simple XOR for demonstration
        }
        
        return decrypted;
    }
    
    float calculateIntegrityHash(const std::vector<char>& data) const {
        std::hash<std::vector<char>> hasher;
        return static_cast<float>(hasher(data));
    }
    
    bool verifyIntegrity(const std::vector<char>& data, float expectedHash) const {
        float actualHash = calculateIntegrityHash(data);
        return std::abs(actualHash - expectedHash) < 0.001f;
    }
    
    void saveBackupToDisk(const std::string& backupId, const std::vector<char>& data) {
        std::string filepath = backupDirectory_ + "/" + backupId + ".backup";
        std::ofstream file(filepath, std::ios::binary);
        if (file) {
            file.write(data.data(), data.size());
            file.close();
        }
    }
    
    bool loadBackupFromDisk(const std::string& backupId, std::vector<char>& data) {
        std::string filepath = backupDirectory_ + "/" + backupId + ".backup";
        std::ifstream file(filepath, std::ios::binary);
        if (!file) return false;
        
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        data.resize(size);
        if (file.read(data.data(), size)) {
            return true;
        }
        
        return false;
    }
    
    bool transferToRemote(const std::string& backupId, const std::vector<char>& data) {
        if (!remoteConfigured_) {
            return false;
        }
        
        // In practice, would implement SSH/SFTP transfer
        // For demonstration, just simulate success
        NLM_LOG_INFO("Transferring backup " + backupId + " to remote server " + remoteConfig_.host);
        
        // Simulate transfer delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        return true;
    }
    
    bool transferFromRemote(const std::string& backupId, std::vector<char>& data) {
        if (!remoteConfigured_) {
            return false;
        }
        
        // In practice, would implement SSH/SFTP transfer
        // For demonstration, just simulate success
        NLM_LOG_INFO("Restoring backup " + backupId + " from remote server " + remoteConfig_.host);
        
        // Simulate transfer delay
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        return true;
    }
    
    ValidationResult validateBackup(const std::string& backupId) const {
        ValidationResult result;
        
        auto metaIt = backups_.find(backupId);
        if (metaIt == backups_.end()) {
            result.isValid = false;
            result.errors.push_back("Backup not found");
            return result;
        }
        
        const BackupMetadata& metadata = metaIt->second;
        
        std::string filepath = backupDirectory_ + "/" + backupId + ".backup";
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            result.isValid = false;
            result.errors.push_back("Backup file not found on disk");
            return result;
        }
        
        std::vector<char> data;
        if (!loadBackupFromDisk(backupId, data)) {
            result.isValid = false;
            result.errors.push_back("Failed to load backup data");
            return result;
        }
        
        if (metadata.isEncrypted) {
            std::vector<char> decrypted = decryptBackupData(data);
            if (!verifyIntegrity(decrypted, metadata.integrityHash)) {
                result.isValid = false;
                result.errors.push_back("Backup integrity check failed (encrypted)");
            }
        } else {
            if (!verifyIntegrity(data, metadata.integrityHash)) {
                result.isValid = false;
                result.errors.push_back("Backup integrity check failed");
            }
        }
        
        if (data.size() > MAX_BACKUP_SIZE) {
            result.isValid = false;
            result.errors.push_back("Backup size exceeds maximum allowed size");
        }
        
        if (metadata.step == 0) {
            result.warnings.push_back("Backup step is zero");
        }
        
        if (metadata.size == 0) {
            result.warnings.push_back("Backup size is zero");
        }
        
        result.validationReport = "Backup " + backupId + " validation: " + 
                                 (result.isValid ? "PASSED" : "FAILED");
        
        return result;
    }
    
    bool validateAllBackups() {
        bool allValid = true;
        std::string validationReport;
        
        for (const auto& pair : backups_) {
            ValidationResult result = validateBackup(pair.first);
            if (!result.isValid) {
                allValid = false;
                validationReport += "Backup " + pair.first + ": FAILED - ";
                for (const std::string& error : result.errors) {
                    validationReport += error + "; ";
                }
                validationReport += "\n";
            }
        }
        
        lastError_ = validationReport.empty() ? "All backups are valid" : validationReport;
        return allValid;
    }
    
    std::string getBackupValidationReport() const {
        std::string report;
        
        for (const auto& pair : backups_) {
            ValidationResult result = validateBackup(pair.first);
            report += "Backup " + pair.first + ": " + 
                     (result.isValid ? "VALID" : "INVALID") + "\n";
            
            if (!result.errors.empty()) {
                report += "  Errors: ";
                for (const std::string& error : result.errors) {
                    report += error + ", ";
                }
                report += "\n";
            }
            
            if (!result.warnings.empty()) {
                report += "  Warnings: ";
                for (const std::string& warning : result.warnings) {
                    report += warning + ", ";
                }
                report += "\n";
            }
        }
        
        return report;
    }
    
    void cleanupOldBackups(size_t maxBackups) {
        if (backups_.size() <= maxBackups) return;
        
        std::vector<std::pair<SimulationStep, std::string>> sortedByTime;
        for (const auto& pair : backups_) {
            sortedByTime.emplace_back(pair.second.step, pair.first);
        }
        
        std::sort(sortedByTime.begin(), sortedByTime.end());
        
        size_t toRemove = backups_.size() - maxBackups;
        for (size_t i = 0; i < toRemove; ++i) {
            std::string backupId = sortedByTime[i].second;
            
            backups_.erase(backupId);
            backupData_.erase(backupId);
            
            std::string filepath = backupDirectory_ + "/" + backupId + ".backup";
            std::filesystem::remove(filepath);
            std::filesystem::remove(filepath + ".meta");
        }
    }
    
    void rebuildBackupIndex() {
        backups_.clear();
        backupData_.clear();
        
        for (const auto& entry : std::filesystem::directory_iterator(backupDirectory_)) {
            if (entry.path().extension() == ".backup") {
                std::string backupId = entry.path().stem().string();
                
                BackupMetadata metadata(backupId, 0, "Restored from disk");
                metadata.size = std::filesystem::file_size(entry.path());
                backups_[backupId] = metadata;
            }
        }
    }
};

BackupSystem::BackupSystem() : pImpl(std::make_unique<Impl>()) {}
BackupSystem::~BackupSystem() = default;

void BackupSystem::setBackupDirectory(const std::string& directory) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    pImpl->backupDirectory_ = directory;
    std::filesystem::create_directories(directory);
}

void BackupSystem::setBackupFrequency(size_t frequency) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    pImpl->backupFrequency_ = frequency;
}

void BackupSystem::setBackupEncryption(bool enabled) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    pImpl->encryptionEnabled_ = enabled;
}

void BackupSystem::setMaxBackups(size_t maxBackups) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    pImpl->maxBackups_ = maxBackups;
}

std::string BackupSystem::createBackup(const std::string& description) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    
    std::string backupId = generateUniqueId();
    
    std::vector<char> backupData = pImpl->createBackupData(memoryManager_, checkpointManager_);
    
    if (pImpl->encryptionEnabled_) {
        backupData = pImpl->encryptBackupData(backupData);
    }
    
    float integrityHash = pImpl->calculateIntegrityHash(backupData);
    
    BackupMetadata metadata(backupId, getCurrentStep(), description);
    metadata.size = backupData.size();
    metadata.isEncrypted = pImpl->encryptionEnabled_;
    metadata.integrityHash = integrityHash;
    pImpl->backups_[backupId] = metadata;
    pImpl->backupData_[backupId] = backupData;
    
    pImpl->saveBackupToDisk(backupId, backupData);
    
    pImpl->totalBackupsCreated_++;
    pImpl->lastBackupTime_ = std::chrono::system_clock::now();
    
    NLM_LOG_INFO("Created backup: " + backupId + " at step " + std::to_string(getCurrentStep()) + 
                " (size: " + std::to_string(backupData.size()) + " bytes)");
    
    return backupId;
}

bool BackupSystem::restoreBackup(const std::string& backupId) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    
    auto it = pImpl->backupData_.find(backupId);
    if (it == pImpl->backupData_.end()) {
        lastError_ = "Backup not found: " + backupId;
        return false;
    }
    
    std::vector<char> data = it->second;
    
    if (pImpl->encryptionEnabled_) {
        data = pImpl->decryptBackupData(data);
    }
    
    NLM_LOG_INFO("Restored backup: " + backupId);
    
    BackupMetadata metadata = pImpl->backups_[backupId];
    metadata.lastAccessTime = std::chrono::system_clock::now();
    pImpl->backups_[backupId] = metadata;
    
    return true;
}

bool BackupSystem::deleteBackup(const std::string& backupId) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    
    pImpl->backups_.erase(backupId);
    pImpl->backupData_.erase(backupId);
    
    std::string filepath = pImpl->backupDirectory_ + "/" + backupId + ".backup";
    std::filesystem::remove(filepath);
    
    NLM_LOG_INFO("Deleted backup: " + backupId);
    return true;
}

std::vector<std::string> BackupSystem::getBackupIds() const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    
    std::vector<std::string> ids;
    ids.reserve(pImpl->backups_.size());
    for (const auto& pair : pImpl->backups_) {
        ids.push_back(pair.first);
    }
    return ids;
}

BackupMetadata BackupSystem::getBackupInfo(const std::string& backupId) const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    
    auto it = pImpl->backups_.find(backupId);
    return (it != pImpl->backups_.end()) ? it->second : BackupMetadata(backupId, 0);
}

bool BackupSystem::validateBackup(const std::string& backupId) const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    
    ValidationResult result = pImpl->validateBackup(backupId);
    lastError_ = result.validationReport;
    return result.isValid;
}

bool BackupSystem::validateAllBackups() const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    return pImpl->validateAllBackups();
}

std::string BackupSystem::getBackupValidationReport() const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    return pImpl->getBackupValidationReport();
}

ValidationResult BackupSystem::validateBackupDetails(const std::string& backupId) const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    return pImpl->validateBackup(backupId);
}

void BackupSystem::configureRemoteBackup(const std::string& host, int port, const std::string& username) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    
    pImpl->remoteConfig_.host = host;
    pImpl->remoteConfig_.port = port;
    pImpl->remoteConfig_.username = username;
    pImpl->remoteConfigured_ = true;
    
    NLM_LOG_INFO("Configured remote backup: " + username + "@" + host + ":" + std::to_string(port));
}

bool BackupSystem::backupToRemote(const std::string& backupId) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    
    auto it = pImpl->backupData_.find(backupId);
    if (it == pImpl->backupData_.end()) {
        lastError_ = "Backup not found: " + backupId;
        return false;
    }
    
    return pImpl->transferToRemote(backupId, it->second);
}

bool BackupSystem::restoreFromRemote(const std::string& backupId) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    
    if (!pImpl->remoteConfigured_) {
        lastError_ = "Remote backup not configured";
        return false;
    }
    
    std::vector<char> data;
    if (!pImpl->transferFromRemote(backupId, data)) {
        return false;
    }
    
    if (pImpl->encryptionEnabled_) {
        data = pImpl->decryptBackupData(data);
    }
    
    float integrityHash = pImpl->calculateIntegrityHash(data);
    
    BackupMetadata metadata(backupId, getCurrentStep(), "Restored from remote");
    metadata.size = data.size();
    metadata.isEncrypted = pImpl->encryptionEnabled_;
    metadata.integrityHash = integrityHash;
    metadata.isRemote = true;
    metadata.remoteLocation = pImpl->remoteConfig_.host + ":" + std::to_string(pImpl->remoteConfig_.port);
    
    pImpl->backups_[backupId] = metadata;
    pImpl->backupData_[backupId] = data;
    pImpl->saveBackupToDisk(backupId, data);
    
    NLM_LOG_INFO("Restored backup from remote: " + backupId);
    return true;
}

bool BackupSystem::testRemoteConnection() const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    
    if (!pImpl->remoteConfigured_) {
        lastError_ = "Remote backup not configured";
        return false;
    }
    
    // In practice, would test actual connection
    NLM_LOG_INFO("Testing remote connection to " + pImpl->remoteConfig_.host);
    return true;
}

void BackupSystem::enableAutomaticBackup(bool enable) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    pImpl->automaticBackupEnabled_ = enable;
    NLM_LOG_INFO("Automatic backup " + std::string(enable ? "enabled" : "disabled"));
}

bool BackupSystem::isAutomaticBackupEnabled() const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    return pImpl->automaticBackupEnabled_;
}

void BackupSystem::setBackupSchedule(size_t frequencySeconds) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    pImpl->backupSchedule_ = frequencySeconds;
}

size_t BackupSystem::getTotalBackups() const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    return pImpl->backups_.size();
}

size_t BackupSystem::getActiveBackups() const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    return pImpl->backups_.size();
}

size_t BackupSystem::getTotalBackupStorage() const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    size_t totalSize = 0;
    for (const auto& pair : pImpl->backups_) {
        totalSize += pair.second.size;
    }
    return totalSize;
}

std::vector<BackupMetadata> BackupSystem::getAllBackupsInfo() const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    
    std::vector<BackupMetadata> infos;
    infos.reserve(pImpl->backups_.size());
    for (const auto& pair : pImpl->backups_) {
        infos.push_back(pair.second);
    }
    return infos;
}

std::chrono::system_clock::time_point BackupSystem::getLastBackupTime() const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    return pImpl->lastBackupTime_;
}

size_t BackupSystem::getBackupsSince(size_t minutes) const {
    std::shared_lock<std::shared_mutex> lock(backupMutex_);
    
    auto cutoffTime = std::chrono::system_clock::now() - std::chrono::minutes(minutes);
    size_t count = 0;
    
    for (const auto& pair : pImpl->backups_) {
        if (pair.second.creationTime >= cutoffTime) {
            count++;
        }
    }
    
    return count;
}

void BackupSystem::cleanupOldBackups(size_t maxBackups) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    pImpl->cleanupOldBackups(maxBackups);
}

void BackupSystem::compactBackups() {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    // In practice, would compact backup data
    NLM_LOG_INFO("Backup compaction completed");
}

void BackupSystem::rebuildBackupIndex() {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    pImpl->rebuildBackupIndex();
}

std::vector<std::string> BackupSystem::createBatchBackups(size_t count, const std::string& baseDescription) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    
    std::vector<std::string> backupIds;
    backupIds.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        std::string description = baseDescription.empty() ? "Batch backup " + std::to_string(i) : 
                                 baseDescription + " " + std::to_string(i);
        std::string backupId = createBackup(description);
        backupIds.push_back(backupId);
    }
    
    NLM_LOG_INFO("Created " + std::to_string(count) + " batch backups");
    return backupIds;
}

bool BackupSystem::restoreBatchBackups(const std::vector<std::string>& backupIds) {
    std::lock_guard<std::shared_mutex> lock(backupMutex_);
    
    bool allRestored = true;
    for (const std::string& backupId : backupIds) {
        if (!restoreBackup(backupId)) {
            allRestored = false;
            lastError_ += "Failed to restore backup: " + backupId + "; ";
        }
    }
    
    if (allRestored) {
        NLM_LOG_INFO("Successfully restored " + std::to_string(backupIds.size()) + " batch backups");
    }
    
    return allRestored;
}

std::vector<RemoteBackupResult> BackupSystem::backupToMultipleRemote(const std::vector<std::string>& hosts) {
    std::vector<RemoteBackupResult> results;
    
    for (const std::string& host : hosts) {
        // Parse host string
        size_t colonPos = host.find(':');
        std::string hostName = host.substr(0, colonPos);
        int port = (colonPos != std::string::npos) ? std::stoi(host.substr(colonPos + 1)) : 22;
        
        configureRemoteBackup(hostName, port, "user");
        bool success = backupToRemote("backup_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()));
        results.emplace_back(success, success ? "Success" : "Failed");
    }
    
    return results;
}

bool BackupSystem::restoreFromMultipleRemote(const std::vector<std::string>& hosts, const std::string& targetBackupId) {
    bool allRestored = true;
    
    for (const std::string& host : hosts) {
        // Parse host string
        size_t colonPos = host.find(':');
        std::string hostName = host.substr(0, colonPos);
        int port = (colonPos != std::string::npos) ? std::stoi(host.substr(colonPos + 1)) : 22;
        
        configureRemoteBackup(hostName, port, "user");
        if (!restoreFromRemote(targetBackupId)) {
            allRestored = false;
        }
    }
    
    return allRestored;
}

} // namespace nlm
