// Backup manager implementation
#include "BackupManager.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

BackupManager::BackupManager() : backupDir_(), config_(nullptr), 
                               strategy_(BackupStrategy::Full), status_(BackupStatus::Inactive),
                               stats_(), backupEventCallback_(nullptr) {
}

BackupManager::~BackupManager() {
    shutdown();
}

bool BackupManager::configure(const std::string& backupDir, const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    status_ = BackupStatus::Creating;
    
    try {
        backupDir_ = backupDir;
        config_ = config;
        
        // Create backup directory
        if (!createBackupDirectory()) {
            status_ = BackupStatus::Error;
            NLM_LOG_ERROR("Failed to create backup directory: " + backupDir_);
            return false;
        }
        
        // Initialize components
        checkpointCompressor_ = std::make_unique<CheckpointCompressor>();
        checkpointEncryptor_ = std::make_unique<CheckpointEncryptor>();
        checkpointIndexer_ = std::make_unique<CheckpointIndexer>();
        
        // Configure backup strategy
        std::string strategyStr = config_ ? config_->getOr<std::string>("backup_strategy", "full") : "full";
        if (strategyStr == "incremental") {
            strategy_ = BackupStrategy::Incremental;
        } else if (strategyStr == "differential") {
            strategy_ = BackupStrategy::Differential;
        } else if (strategyStr == "mirrored") {
            strategy_ = BackupStrategy::Mirrored;
        } else {
            strategy_ = BackupStrategy::Full;
        }
        
        status_ = BackupStatus::Complete;
        
        NLM_LOG_INFO("Backup manager initialized successfully in: " + backupDir_);
        NLM_LOG_INFO("Backup strategy: " + strategyStr);
        
        return true;
        
    } catch (const std::exception& e) {
        status_ = BackupStatus::Error;
        NLM_LOG_ERROR(std::string("Backup manager configuration failed: ") + e.what());
        return false;
    }
}

void BackupManager::shutdown() {
    if (status_ == BackupStatus::Inactive) return;
    
    NLM_LOG_INFO("Shutting down backup manager...");
    
    // Cleanup components
    checkpointCompressor_.reset();
    checkpointEncryptor_.reset();
    checkpointIndexer_.reset();
    
    status_ = BackupStatus::Inactive;
    NLM_LOG_INFO("Backup manager shutdown complete");
}

bool BackupManager::createBackup(const std::string& checkpointName, const CheckpointOptions& options) {
    auto startTime = std::chrono::high_resolution_clock::now();
    BackupResult result;
    result.success = false;
    
    try {
        status_ = BackupStatus::Creating;
        
        // Validate configuration
        if (!validateConfiguration()) {
            result.message = "Invalid configuration";
            logEvent("create_failed", checkpointName, result);
            return false;
        }
        
        // Create backup based on strategy
        bool success = false;
        switch (strategy_) {
            case BackupStrategy::Full:
                success = createFullBackup(checkpointName);
                break;
            case BackupStrategy::Incremental:
                success = createIncrementalBackup(checkpointName, getLatestCheckpoint());
                break;
            case BackupStrategy::Differential:
                success = createDifferentialBackup(checkpointName, getLatestCheckpoint());
                break;
            case BackupStrategy::Mirrored:
                success = createFullBackup(checkpointName);
                break;
        }
        
        if (!success) {
            status_ = BackupStatus::Error;
            result.message = "Backup creation failed for strategy: " + std::to_string(static_cast<int>(strategy_));
            logEvent("create_failed", checkpointName, result);
            return false;
        }
        
        result.success = true;
        result.message = "Backup created successfully";
        
        updateStats(result);
        logEvent("created", checkpointName, result);
        
        status_ = BackupStatus::Complete;
        NLM_LOG_INFO("Backup created successfully: " + checkpointName);
        return true;
        
    } catch (const std::exception& e) {
        status_ = BackupStatus::Error;
        result.message = std::string("Exception creating backup: ") + e.what();
        logEvent("create_exception", checkpointName, result);
        return false;
    }
}

bool BackupManager::createFullBackup(const std::string& backupName) {
    std::string checkpointPath = getCheckpointPath(backupName);
    if (!std::filesystem::exists(checkpointPath)) {
        NLM_LOG_ERROR("Source checkpoint does not exist: " + backupName);
        return false;
    }
    
    std::string backupPath = getBackupPath(backupName);
    
    // Copy checkpoint file
    if (!std::filesystem::copy_file(checkpointPath, backupPath, 
                                   std::filesystem::copy_options::overwrite_existing)) {
        NLM_LOG_ERROR("Failed to copy checkpoint to backup: " + backupName);
        return false;
    }
    
    // Compress if requested
    if (options_.compression != CompressionLevel::None) {
        if (!compressBackup(checkpointPath, backupPath)) {
            NLM_LOG_WARNING("Compression failed for backup: " + backupName);
        }
    }
    
    // Encrypt if requested
    if (options_.encrypt) {
        std::string encryptedPath = backupPath + ".enc";
        if (!encryptBackup(backupPath, encryptedPath)) {
            NLM_LOG_WARNING("Encryption failed for backup: " + backupName);
        } else {
            std::filesystem::remove(backupPath);
        }
    }
    
    return true;
}

bool BackupManager::createIncrementalBackup(const std::string& backupName, const std::string& baseCheckpoint) {
    // In real implementation, would track changes since base checkpoint
    // For demonstration, just create a full backup
    return createFullBackup(backupName);
}

bool BackupManager::createDifferentialBackup(const std::string& backupName, const std::string& baseCheckpoint) {
    // In real implementation, would track changes since base checkpoint
    // For demonstration, just create a full backup
    return createFullBackup(backupName);
}

bool BackupManager::restoreBackup(const std::string& backupName, const RecoveryOptions& options) {
    auto startTime = std::chrono::high_resolution_clock::now();
    BackupResult result;
    result.success = false;
    
    try {
        status_ = BackupStatus::Restoring;
        
        std::string backupPath = getBackupPath(backupName);
        if (!std::filesystem::exists(backupPath)) {
            result.message = "Backup file does not exist: " + backupName;
            logEvent("restore_failed", backupName, result);
            return false;
        }
        
        // Decrypt if encrypted
        std::string sourcePath = backupPath;
        if (checkpointEncryptor_->isEncrypted(backupPath)) {
            std::string decryptedPath = backupPath + ".dec";
            if (!checkpointEncryptor_->decrypt(backupPath, decryptedPath)) {
                result.message = "Failed to decrypt backup: " + backupName;
                logEvent("restore_failed", backupName, result);
                return false;
            }
            sourcePath = decryptedPath;
        }
        
        // Decompress if needed
        std::string targetPath = options.targetPath.empty() ? 
            getCheckpointPath(backupName) : options.targetPath;
        
        if (!std::filesystem::exists(targetPath) || options.overwriteExisting) {
            if (!std::filesystem::copy_file(sourcePath, targetPath,
                                           std::filesystem::copy_options::overwrite_existing)) {
                result.message = "Failed to restore backup: " + backupName;
                logEvent("restore_failed", backupName, result);
                return false;
            }
        }
        
        // Validate if requested
        if (options.validateAfterRecovery) {
            if (!validateBackup(backupName)) {
                result.message = "Backup validation failed after restore: " + backupName;
                logEvent("restore_failed", backupName, result);
                return false;
            }
        }
        
        result.success = true;
        result.message = "Backup restored successfully";
        
        updateStats(result);
        logEvent("restored", backupName, result);
        
        status_ = BackupStatus::Complete;
        NLM_LOG_INFO("Backup restored successfully: " + backupName);
        return true;
        
    } catch (const std::exception& e) {
        status_ = BackupStatus::Error;
        result.message = std::string("Exception restoring backup: ") + e.what();
        logEvent("restore_exception", backupName, result);
        return false;
    }
}

bool BackupManager::deleteBackup(const std::string& backupName) {
    std::string backupPath = getBackupPath(backupName);
    if (std::filesystem::exists(backupPath)) {
        if (std::remove(backupPath.c_str()) != 0) {
            return false;
        }
    }
    
    if (checkpointIndexer_) {
        checkpointIndexer_->removeEntry(backupName);
    }
    
    return true;
}

std::vector<std::string> BackupManager::listBackups() const {
    std::vector<std::string> backups;
    
    if (checkpointIndexer_) {
        backups = checkpointIndexer_->getAllBackups();
    }
    
    return backups;
}

bool BackupManager::validateBackup(const std::string& backupName) {
    std::string backupPath = getBackupPath(backupName);
    
    if (!std::filesystem::exists(backupPath)) {
        validationErrors_.push_back("Backup file does not exist: " + backupName);
        return false;
    }
    
    // Check file integrity
    std::ifstream file(backupPath, std::ios::binary);
    if (!file.is_open()) {
        validationErrors_.push_back("Cannot open backup file: " + backupName);
        return false;
    }
    
    // In real implementation, would validate backup structure and checksums
    // For demonstration, just check file size
    file.seekg(0, std::ios::end);
    uint64_t fileSize = file.tellg();
    
    if (fileSize == 0) {
        validationErrors_.push_back("Backup file is empty: " + backupName);
        return false;
    }
    
    return true;
}

std::string BackupManager::getStatusString() const {
    switch (status_) {
        case BackupStatus::Inactive: return "Inactive";
        case BackupStatus::Creating: return "Creating";
        case BackupStatus::Restoring: return "Restoring";
        case BackupStatus::Validating: return "Validating";
        case BackupStatus::Error: return "Error";
        case BackupStatus::Complete: return "Complete";
        default: return "Unknown";
    }
}

bool BackupManager::validateConfiguration() const {
    validationErrors_.clear();
    
    if (backupDir_.empty()) {
        validationErrors_.push_back("Backup directory is not configured");
    }
    
    if (!std::filesystem::exists(backupDir_)) {
        validationErrors_.push_back("Backup directory does not exist: " + backupDir_);
    }
    
    return validationErrors_.empty();
}

bool BackupManager::validateBackups() const {
    validationErrors_.clear();
    
    std::vector<std::string> backups = listBackups();
    bool allValid = true;
    
    for (const auto& backup : backups) {
        std::vector<std::string> errors;
        if (!validateBackup(backup)) {
            allValid = false;
            for (const auto& error : errors) {
                validationErrors_.push_back("Backup '" + backup + "': " + error);
            }
        }
    }
    
    return allValid;
}

void BackupManager::updateStats(const BackupResult& result) {
    stats_.totalBackups++;
    if (result.success) {
        stats_.successfulBackups++;
    } else {
        stats_.failedBackups++;
    }
    
    stats_.totalBackupSize += result.backupSize;
    stats_.totalRestoredSize += result.originalSize;
    
    if (stats_.totalBackups > 0) {
        stats_.averageCompressionRatio = 
            static_cast<double>(stats_.totalBackupSize) / stats_.totalBackups;
    }
}

void BackupManager::logEvent(const std::string& eventType, const std::string& backupName,
                             const BackupResult& result) {
    std::stringstream logMsg;
    logMsg << "[Backup] " << eventType << " - Backup: " << backupName;
    
    if (!result.message.empty()) {
        logMsg << " - " << result.message;
    }
    
    if (result.success) {
        NLM_LOG_INFO(logMsg.str());
    } else {
        NLM_LOG_ERROR(logMsg.str());
    }
    
    // Call event callback if registered
    if (backupEventCallback_) {
        backupEventCallback_(backupName, result);
    }
}

bool BackupManager::createBackupDirectory() {
    try {
        if (!std::filesystem::exists(backupDir_)) {
            if (!std::filesystem::create_directories(backupDir_)) {
                return false;
            }
        }
        
        // Create subdirectories
        std::vector<std::string> subdirs = {"full", "incremental", "differential"};
        for (const auto& subdir : subdirs) {
            std::string fullPath = backupDir_ + "/" + subdir;
            if (!std::filesystem::exists(fullPath)) {
                if (!std::filesystem::create_directories(fullPath)) {
                    return false;
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating backup directory: ") + e.what());
        return false;
    }
}

std::string BackupManager::getBackupPath(const std::string& backupName) const {
    return backupDir_ + "/full/" + backupName + ".bak";
}

std::string BackupManager::getCheckpointPath(const std::string& checkpointName) const {
    // In real implementation, would need the main data directory
    // For now, just return a placeholder
    return "./checkpoints/" + checkpointName + ".bin";
}

std::string BackupManager::getLatestCheckpoint() const {
    // In real implementation, would query checkpoint manager for latest
    return "latest_checkpoint";
}

bool BackupManager::compressBackup(const std::string& sourcePath, const std::string& targetPath) {
    // Simple compression simulation
    std::ifstream source(sourcePath, std::ios::binary);
    std::ofstream target(targetPath, std::ios::binary);
    
    if (!source.is_open() || !target.is_open()) {
        return false;
    }
    
    // Read and write with simple compression (just copy for demo)
    target << source.rdbuf();
    
    source.close();
    target.close();
    
    return true;
}

bool BackupManager::encryptBackup(const std::string& sourcePath, const std::string& targetPath) {
    // Simple encryption simulation
    std::ifstream source(sourcePath, std::ios::binary);
    std::ofstream target(targetPath, std::ios::binary);
    
    if (!source.is_open() || !target.is_open()) {
        return false;
    }
    
    // Simple XOR encryption for demo
    char key = 0xAA;
    char ch;
    while (source.get(ch)) {
        target.put(ch ^ key);
    }
    
    source.close();
    target.close();
    
    return true;
}

} // namespace nlm
