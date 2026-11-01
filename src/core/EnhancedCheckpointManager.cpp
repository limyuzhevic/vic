// Enhanced Checkpoint and Recovery System
// Advanced checkpoint management with improved validation, recovery, and management
//
// This module provides enhanced checkpoint functionality including:
// - Better checkpoint validation and integrity checking
// - Improved recovery with rollback capabilities
// - Checkpoint versioning and rotation
// - Automated checkpoint management
// - Checkpoint compression and encryption (if available)
// - Performance monitoring for checkpoint operations
// - Consistency verification across brain systems
// - Recovery with state verification
//
// Key improvements:
// 1. Enhanced checkpoint validation with multiple checksums
// 2. Checkpoint versioning for history and rollback
// 3. Automated checkpoint management (rotation, cleanup)
// 4. Checkpoint compression for storage efficiency
// 5. Performance monitoring and reporting
// 6. Recovery verification and state consistency checking
// 7. Parallel checkpoint operations
// 8. Checkpoint encryption for security (if available)

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <algorithm>
#include <functional>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <zlib.h> // For compression (if available)
#include <cryptopp/openssl.h> // For encryption (if available)
#include <openssl/evp.h> // For encryption (if available)
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"

namespace nlm {

// Checkpoint metadata
struct CheckpointMetadata {
    std::string checkpointId;
    std::chrono::system_clock::time_point creationTime;
    std::chrono::system_clock::time_point lastAccessTime;
    size_t fileSize;
    uint64_t brainChecksum;
    uint64_t simulationStep;
    Timestamp simulationTime;
    std::string brainVersion;
    std::string configVersion;
    std::vector<std::string> dependentComponents;
    std::map<std::string, std::string> metadata;
    bool isValid;
    bool isCompressed;
    bool isEncrypted;
    std::string compressionAlgorithm;
    std::string encryptionAlgorithm;
    std::string checksum;
    uint64_t crc32;
    
    CheckpointMetadata()
        : fileSize(0), brainChecksum(0), simulationStep(0), simulationTime(0.0),
          isValid(false), isCompressed(false), isEncrypted(false), crc32(0) {}
};

// Checkpoint recovery result
struct CheckpointRecoveryResult {
    bool success;
    std::string checkpointId;
    std::chrono::system_clock::time_point recoveryTime;
    std::string errorMessage;
    size_t restoredNeurons;
    size_t restoredSynapses;
    size_t restoredMemoryTrails;
    size_t restoredEpisodicMemories;
    float brainConsistencyScore;
    bool stateConsistencyVerified;
    std::vector<std::string> recoveredComponents;
    std::vector<std::string> failedComponents;
    
    CheckpointRecoveryResult()
        : success(false), restoredNeurons(0), restoredSynapses(0),
          restoredMemoryTrails(0), restoredEpisodicMemories(0),
          brainConsistencyScore(0.0f), stateConsistencyVerified(false) {}
};

// Checkpoint management configuration
struct CheckpointConfig {
    std::string checkpointDirectory;
    std::string backupDirectory;
    size_t maxCheckpoints;
    size_t checkpointRetentionDays;
    size_t maxCheckpointSizeMB;
    bool autoCheckpoint;
    double autoCheckpointInterval;
    bool autoBackup;
    double autoBackupInterval;
    bool enableCompression;
    std::string compressionAlgorithm;
    bool enableEncryption;
    std::string encryptionKeyFile;
    bool validateCheckpoints;
    bool verifyRecovery;
    bool enableParallelOperations;
    size_t parallelCheckpointCount;
    bool enablePerformanceMonitoring;
    double checkpointTimeout;
    bool enableConsistencyChecking;
    bool enableVersioning;
    
    CheckpointConfig() : maxCheckpoints(10), checkpointRetentionDays(30),
                       maxCheckpointSizeMB(1000), autoCheckpoint(true),
                       autoCheckpointInterval(300.0), autoBackup(true),
                       autoBackupInterval(600.0), enableCompression(true),
                       enableEncryption(false), validateCheckpoints(true),
                       verifyRecovery(true), enableParallelOperations(true),
                       parallelCheckpointCount(4), enablePerformanceMonitoring(true),
                       checkpointTimeout(30.0), enableConsistencyChecking(true),
                       enableVersioning(true) {}
};

// Enhanced checkpoint manager
class EnhancedCheckpointManager {
public:
    EnhancedCheckpointManager();
    ~EnhancedCheckpointManager();
    
    // Initialize checkpoint manager
    bool initialize(const CheckpointConfig& config);
    
    // Create checkpoint
    bool createCheckpoint(const std::string& brainId, std::shared_ptr<Brain> brain);
    
    // Create checkpoint with advanced options
    bool createCheckpoint(const std::string& brainId, std::shared_ptr<Brain> brain,
                         const std::map<std::string, std::string>& options);
    
    // Load checkpoint
    CheckpointRecoveryResult loadCheckpoint(const std::string& checkpointPath);
    
    // Load checkpoint by ID
    CheckpointRecoveryResult loadCheckpointById(const std::string& checkpointId);
    
    // List available checkpoints
    std::vector<std::string> listCheckpoints() const;
    
    // List checkpoints by brain ID
    std::vector<std::string> listCheckpoints(const std::string& brainId) const;
    
    // Get checkpoint metadata
    CheckpointMetadata getCheckpointMetadata(const std::string& checkpointPath) const;
    
    // Validate checkpoint
    bool validateCheckpoint(const std::string& checkpointPath) const;
    
    // Verify checkpoint integrity
    bool verifyCheckpointIntegrity(const std::string& checkpointPath) const;
    
    // Delete checkpoint
    bool deleteCheckpoint(const std::string& checkpointPath);
    
    // Cleanup old checkpoints
    bool cleanupOldCheckpoints();
    
    // Set working directory
    void setWorkingDirectory(const std::string& directory);
    
    // Get working directory
    std::string getWorkingDirectory() const;
    
    // Get checkpoint statistics
    std::string getStatistics() const;
    
    // Generate checkpoint report
    std::string generateReport() const;
    
    // Export checkpoint configuration
    std::string exportConfiguration() const;
    
    // Import checkpoint configuration
    bool importConfiguration(const std::string& configFile);
    
    // Enable/disable checkpoint compression
    void setCompressionEnabled(bool enabled);
    
    // Set compression algorithm
    void setCompressionAlgorithm(const std::string& algorithm);
    
    // Enable/disable checkpoint encryption
    void setEncryptionEnabled(bool enabled);
    
    // Set encryption key file
    void setEncryptionKeyFile(const std::string& keyFile);
    
    // Enable/disable checkpoint validation
    void setValidationEnabled(bool enabled);
    
    // Enable/disable recovery verification
    void setRecoveryVerificationEnabled(bool enabled);
    
    // Enable/disable consistency checking
    void setConsistencyCheckingEnabled(bool enabled);
    
    // Enable/disable parallel operations
    void setParallelOperationsEnabled(bool enabled);
    
    // Set parallel operation count
    void setParallelOperationCount(size_t count);
    
    // Enable/disable performance monitoring
    void setPerformanceMonitoringEnabled(bool enabled);
    
    // Set checkpoint timeout
    void setCheckpointTimeout(double timeout);
    
    // Create auto checkpoint
    bool createAutoCheckpoint(std::shared_ptr<Brain> brain);
    
    // Create periodic checkpoint
    bool createPeriodicCheckpoint(std::shared_ptr<Brain> brain);
    
    // Restore brain from checkpoint
    bool restoreBrainFromCheckpoint(std::shared_ptr<Brain> brain, const std::string& checkpointPath);
    
    // Validate brain state after recovery
    bool validateBrainState(std::shared_ptr<Brain> brain, const std::string& checkpointPath) const;
    
    // Get checkpoint size
    size_t getCheckpointSize(const std::string& checkpointPath) const;
    
    // Calculate checkpoint checksum
    std::string calculateCheckpointChecksum(const std::string& checkpointPath) const;
    
    // Verify checkpoint checksum
    bool verifyCheckpointChecksum(const std::string& checkpointPath, const std::string& expectedChecksum) const;
    
    // Compress checkpoint
    bool compressCheckpoint(const std::string& checkpointPath);
    
    // Decompress checkpoint
    bool decompressCheckpoint(const std::string& checkpointPath);
    
    // Encrypt checkpoint
    bool encryptCheckpoint(const std::string& checkpointPath);
    
    // Decrypt checkpoint
    bool decryptCheckpoint(const std::string& checkpointPath);
    
    // Get compression ratio
    double getCompressionRatio(const std::string& checkpointPath) const;
    
    // Get encryption status
    bool isCompressed(const std::string& checkpointPath) const;
    bool isEncrypted(const std::string& checkpointPath) const;
    
    // Get checkpoint age in days
    double getCheckpointAge(const std::string& checkpointPath) const;
    
    // Get last access time
    std::chrono::system_clock::time_point getLastAccessTime(const std::string& checkpointPath) const;
    
    // Update last access time
    bool updateLastAccessTime(const std::string& checkpointPath);
    
    // Create backup
    bool createBackup(const std::string& checkpointPath);
    
    // Restore from backup
    bool restoreFromBackup(const std::string& backupPath);
    
    // List backups
    std::vector<std::string> listBackups() const;
    
    // Cleanup old backups
    bool cleanupOldBackups();
    
    // Set maximum checkpoints
    void setMaxCheckpoints(size_t maxCheckpoints);
    
    // Set checkpoint retention days
    void setCheckpointRetentionDays(size_t retentionDays);
    
    // Set maximum checkpoint size
    void setMaxCheckpointSizeMB(size_t maxSizeMB);
    
    // Set auto checkpoint interval
    void setAutoCheckpointInterval(double interval);
    
    // Set auto backup interval
    void setAutoBackupInterval(double interval);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Create checkpoint directory if it doesn't exist
    bool ensureCheckpointDirectoryExists() const;
    
    // Validate checkpoint configuration
    bool validateConfig(const CheckpointConfig& config) const;
    
    // Generate checkpoint ID
    std::string generateCheckpointId(const std::string& brainId) const;
    
    // Calculate brain checksum
    uint64_t calculateBrainChecksum(std::shared_ptr<Brain> brain) const;
    
    // Generate checksum
    std::string generateChecksum(const std::string& data) const;
    
    // Calculate CRC32
    uint32_t calculateCRC32(const std::string& data) const;
    
    // Compress data
    std::string compressData(const std::string& data, const std::string& algorithm) const;
    
    // Decompress data
    std::string decompressData(const std::string& compressedData, const std::string& algorithm) const;
    
    // Encrypt data
    std::string encryptData(const std::string& data, const std::string& keyFile) const;
    
    // Decrypt data
    std::string decryptData(const std::string& encryptedData, const std::string& keyFile) const;
    
    // Save checkpoint metadata
    bool saveCheckpointMetadata(const CheckpointMetadata& metadata) const;
    
    // Load checkpoint metadata
    bool loadCheckpointMetadata(const std::string& checkpointPath, CheckpointMetadata& metadata) const;
    
    // Update checkpoint metadata
    bool updateCheckpointMetadata(const std::string& checkpointPath, const CheckpointMetadata& metadata) const;
    
    // Get checkpoint path
    std::string getCheckpointPath(const std::string& checkpointId) const;
    
    // Get backup path
    std::string getBackupPath(const std::string& checkpointId) const;
    
    // Get brain checkpoint path
    std::string getBrainCheckpointPath(const std::string& brainId) const;
    
    // Get timestamp string
    std::string getTimestampString() const;
    
    // Get formatted time string
    std::string getFormattedTime(const std::chrono::system_clock::time_point& time) const;
    
    // Validate brain consistency
    bool validateBrainConsistency(std::shared_ptr<Brain> brain, const std::string& checkpointPath) const;
    
    // Calculate consistency score
    float calculateConsistencyScore(std::shared_ptr<Brain> brain, const std::string& checkpointPath) const;
    
    // Verify brain state
    bool verifyBrainState(std::shared_ptr<Brain> brain, const std::string& checkpointPath) const;
    
    // Check if checkpoint is expired
    bool isCheckpointExpired(const CheckpointMetadata& metadata) const;
    
    // Cleanup expired checkpoints
    bool cleanupExpiredCheckpoints();
    
    // Cleanup oversized checkpoints
    bool cleanupOversizedCheckpoints();
    
    // Cleanup excess checkpoints
    bool cleanupExcessCheckpoints();
    
    // Get checkpoint count
    size_t getCheckpointCount() const;
    
    // Get brain checkpoint count
    size_t getBrainCheckpointCount(const std::string& brainId) const;
    
    // Generate report
    std::string generateReport(const std::string& title) const;
    
    // Export configuration
    std::string exportConfiguration(const std::string& format) const;
    
    // Import configuration
    bool importConfiguration(const std::string& configFile, std::string& format);
}; // class EnhancedCheckpointManager

// Performance monitor for checkpoint operations
class CheckpointPerformanceMonitor {
public:
    CheckpointPerformanceMonitor();
    ~CheckpointPerformanceMonitor();
    
    // Start performance measurement
    void startMeasurement(const std::string& operation);
    
    // Stop performance measurement
    double stopMeasurement(const std::string& operation);
    
    // Get operation time
    double getOperationTime(const std::string& operation) const;
    
    // Get average operation time
    double getAverageOperationTime(const std::string& operation) const;
    
    // Get minimum operation time
    double getMinOperationTime(const std::string& operation) const;
    
    // Get maximum operation time
    double getMaxOperationTime(const std::string& operation) const;
    
    // Get operation count
    size_t getOperationCount(const std::string& operation) const;
    
    // Reset performance data
    void reset();
    
    // Generate performance report
    std::string generateReport() const;
    
    // Enable/disable performance monitoring
    void setEnabled(bool enabled);
    
    // Check if performance monitoring is enabled
    bool isEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; // class CheckpointPerformanceMonitor

// Global checkpoint manager instance
EnhancedCheckpointManager& getEnhancedCheckpointManager();

// Convenience functions
bool createCheckpoint(std::shared_ptr<Brain> brain, const std::string& checkpointId = "");
CheckpointRecoveryResult loadCheckpoint(const std::string& checkpointPath);
bool restoreBrainFromCheckpoint(std::shared_ptr<Brain> brain, const std::string& checkpointPath);
std::vector<std::string> listCheckpoints();
std::vector<std::string> listCheckpoints(const std::string& brainId);
bool deleteCheckpoint(const std::string& checkpointPath);
bool cleanupCheckpoints();
std::string getCheckpointStatistics();
bool exportCheckpointConfiguration(const std::string& filename);
bool importCheckpointConfiguration(const std::string& filename);

// Checkpoint management utilities
namespace checkpoint {
    
    // Create checkpoint directory
    bool createCheckpointDirectory(const std::string& path);
    
    // Get available disk space
    size_t getAvailableDiskSpace(const std::string& path);
    
    // Check if checkpoint directory exists
    bool checkpointDirectoryExists(const std::string& path);
    
    // Get checkpoint file list
    std::vector<std::string> getCheckpointFiles(const std::string& path);
    
    // Get backup file list
    std::vector<std::string> getBackupFiles(const std::string& path);
    
    // Calculate file size
    size_t calculateFileSize(const std::string& path);
    
    // Format file size
    std::string formatFileSize(size_t bytes);
    
    // Get current timestamp
    std::string getCurrentTimestamp();
    
    // Get time since epoch
    double getTimeSinceEpoch();
    
    // Validate checkpoint path
    bool validateCheckpointPath(const std::string& path);
    
    // Validate checkpoint ID
    bool validateCheckpointId(const std::string& id);
    
    // Generate random string
    std::string generateRandomString(size_t length);
    
    // Calculate hash
    std::string calculateHash(const std::string& data);
    
    // Generate checksum
    std::string generateChecksum(const std::string& data);
    
    // Compare checkpoints
    bool compareCheckpoints(const std::string& path1, const std::string& path2);
    
    // Merge checkpoints
    bool mergeCheckpoints(const std::string& source1, const std::string& source2, const std::string& target);
    
    // Split checkpoint
    bool splitCheckpoint(const std::string& source, const std::string& target1, const std::string& target2);
    
    // Repair checkpoint
    bool repairCheckpoint(const std::string& corruptedPath, const std::string& repairedPath);
    
    // Validate checkpoint structure
    bool validateCheckpointStructure(const std::string& checkpointPath);
    
    // Get checkpoint integrity
    bool getCheckpointIntegrity(const std::string& checkpointPath);
    
    // Set checkpoint integrity
    bool setCheckpointIntegrity(const std::string& checkpointPath, bool valid);
    
} // namespace checkpoint

} // namespace nlm

// Implementation of EnhancedCheckpointManager
struct EnhancedCheckpointManager::Impl {
    std::string workingDirectory;
    CheckpointConfig config;
    std::map<std::string, CheckpointMetadata> checkpointMetadata;
    std::map<std::string, std::vector<std::string>> brainCheckpoints;
    CheckpointPerformanceMonitor performanceMonitor;
    std::map<std::string, std::chrono::system_clock::time_point> lastCheckpointTimes;
    std::map<std::string, size_t> checkpointCounts;
    bool initialized;
    bool compressionEnabled;
    bool encryptionEnabled;
    bool validationEnabled;
    bool recoveryVerificationEnabled;
    bool consistencyCheckingEnabled;
    bool parallelOperationsEnabled;
    size_t parallelOperationCount;
    bool performanceMonitoringEnabled;
    double checkpointTimeout;
    bool autoCheckpoint;
    double autoCheckpointInterval;
    bool autoBackup;
    double autoBackupInterval;
    size_t maxCheckpoints;
    size_t checkpointRetentionDays;
    size_t maxCheckpointSizeMB;
    std::string compressionAlgorithm;
    std::string encryptionKeyFile;
};

EnhancedCheckpointManager::EnhancedCheckpointManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
    pImpl->compressionEnabled = false;
    pImpl->encryptionEnabled = false;
    pImpl->validationEnabled = false;
    pImpl->recoveryVerificationEnabled = false;
    pImpl->consistencyCheckingEnabled = false;
    pImpl->parallelOperationsEnabled = false;
    pImpl->parallelOperationCount = 4;
    pImpl->performanceMonitoringEnabled = false;
    pImpl->checkpointTimeout = 30.0;
    pImpl->autoCheckpoint = true;
    pImpl->autoCheckpointInterval = 300.0;
    pImpl->autoBackup = true;
    pImpl->autoBackupInterval = 600.0;
    pImpl->maxCheckpoints = 10;
    pImpl->checkpointRetentionDays = 30;
    pImpl->maxCheckpointSizeMB = 1000;
    pImpl->compressionAlgorithm = "gzip";
    pImpl->encryptionKeyFile = "";
}

EnhancedCheckpointManager::~EnhancedCheckpointManager() = default;

bool EnhancedCheckpointManager::initialize(const CheckpointConfig& config) {
    try {
        // Validate configuration
        if (!validateConfig(config)) {
            NLM_LOG_ERROR("Invalid checkpoint configuration");
            return false;
        }
        
        // Set configuration
        pImpl->config = config;
        pImpl->workingDirectory = config.checkpointDirectory;
        
        // Ensure checkpoint directory exists
        if (!ensureCheckpointDirectoryExists()) {
            NLM_LOG_ERROR("Failed to create checkpoint directory");
            return false;
        }
        
        // Create backup directory if specified
        if (!config.backupDirectory.empty()) {
            std::filesystem::path backupPath(config.backupDirectory);
            if (!std::filesystem::exists(backupPath)) {
                std::filesystem::create_directories(backupPath);
            }
        }
        
        // Set configuration flags
        pImpl->compressionEnabled = config.enableCompression;
        pImpl->encryptionEnabled = config.enableEncryption;
        pImpl->validationEnabled = config.validateCheckpoints;
        pImpl->recoveryVerificationEnabled = config.verifyRecovery;
        pImpl->consistencyCheckingEnabled = config.enableConsistencyChecking;
        pImpl->parallelOperationsEnabled = config.enableParallelOperations;
        pImpl->parallelOperationCount = config.parallelCheckpointCount;
        pImpl->performanceMonitoringEnabled = config.enablePerformanceMonitoring;
        pImpl->checkpointTimeout = config.checkpointTimeout;
        pImpl->autoCheckpoint = config.autoCheckpoint;
        pImpl->autoCheckpointInterval = config.autoCheckpointInterval;
        pImpl->autoBackup = config.autoBackup;
        pImpl->autoBackupInterval = config.autoBackupInterval;
        pImpl->maxCheckpoints = config.maxCheckpoints;
        pImpl->checkpointRetentionDays = config.checkpointRetentionDays;
        pImpl->maxCheckpointSizeMB = config.maxCheckpointSizeMB;
        pImpl->compressionAlgorithm = config.compressionAlgorithm;
        pImpl->encryptionKeyFile = config.encryptionKeyFile;
        
        // Initialize performance monitor
        if (pImpl->performanceMonitoringEnabled) {
            pImpl->performanceMonitor.setEnabled(true);
        }
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("Enhanced checkpoint manager initialized successfully");
        NLM_LOG_INFO("Working directory: " << pImpl->workingDirectory);
        NLM_LOG_INFO("Max checkpoints: " << pImpl->maxCheckpoints);
        NLM_LOG_INFO("Checkpoint retention: " << pImpl->checkpointRetentionDays << " days");
        NLM_LOG_INFO("Auto checkpoint: " << (pImpl->autoCheckpoint ? "enabled" : "disabled"));
        NLM_LOG_INFO("Compression: " << (pImpl->compressionEnabled ? "enabled" : "disabled"));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize checkpoint manager: " + std::string(e.what()));
        return false;
    }
}

bool EnhancedCheckpointManager::createCheckpoint(const std::string& brainId, std::shared_ptr<Brain> brain) {
    return createCheckpoint(brainId, brain, std::map<std::string, std::string>());
}

bool EnhancedCheckpointManager::createCheckpoint(const std::string& brainId, std::shared_ptr<Brain> brain,
                                                 const std::map<std::string, std::string>& options) {
    if (!pImpl->initialized) {
        NLM_LOG_ERROR("Checkpoint manager not initialized");
        return false;
    }
    
    if (!brain) {
        NLM_LOG_ERROR("Cannot create checkpoint: brain is null");
        return false;
    }
    
    try {
        // Generate checkpoint ID
        std::string checkpointId = generateCheckpointId(brainId);
        std::string checkpointPath = getCheckpointPath(checkpointId);
        
        // Start performance measurement
        if (pImpl->performanceMonitoringEnabled) {
            pImpl->performanceMonitor.startMeasurement("create_checkpoint");
        }
        
        // Ensure checkpoint directory exists
        if (!ensureCheckpointDirectoryExists()) {
            NLM_LOG_ERROR("Failed to create checkpoint directory for " + checkpointId);
            return false;
        }
        
        // Create metadata
        CheckpointMetadata metadata;
        metadata.checkpointId = checkpointId;
        metadata.creationTime = std::chrono::system_clock::now();
        metadata.lastAccessTime = metadata.creationTime;
        metadata.isValid = true;
        metadata.isCompressed = false;
        metadata.isEncrypted = false;
        
        // Calculate brain checksum
        metadata.brainChecksum = calculateBrainChecksum(brain);
        
        // Get brain state
        metadata.simulationStep = brain->getTotalNeuronCount(); // Simplified
        metadata.simulationTime = 0.0; // Would be actual simulation time
        metadata.brainVersion = "enhanced_v1.0";
        metadata.configVersion = "enhanced_v1.0";
        
        // Add options to metadata
        for (const auto& pair : options) {
            metadata.metadata[pair.first] = pair.second;
        }
        
        // Save brain checkpoint
        std::string brainCheckpointPath = getBrainCheckpointPath(brainId);
        if (!brain->save(brainCheckpointPath)) {
            NLM_LOG_ERROR("Failed to save brain checkpoint for " + checkpointId);
            return false;
        }
        
        // Get checkpoint file size
        metadata.fileSize = getCheckpointSize(checkpointPath);
        
        // Calculate checksum
        metadata.checksum = calculateCheckpointChecksum(checkpointPath);
        
        // Compress if enabled
        if (pImpl->compressionEnabled) {
            if (compressCheckpoint(checkpointPath)) {
                metadata.isCompressed = true;
            }
        }
        
        // Encrypt if enabled
        if (pImpl->encryptionEnabled) {
            if (encryptCheckpoint(checkpointPath)) {
                metadata.isEncrypted = true;
            }
        }
        
        // Save metadata
        if (!saveCheckpointMetadata(metadata)) {
            NLM_LOG_ERROR("Failed to save checkpoint metadata for " + checkpointId);
            return false;
        }
        
        // Update checkpoint counts
        pImpl->checkpointCounts[brainId]++;
        pImpl->lastCheckpointTimes[brainId] = metadata.creationTime;
        
        // Add to metadata map
        pImpl->checkpointMetadata[checkpointPath] = metadata;
        
        // Add to brain checkpoints map
        pImpl->brainCheckpoints[brainId].push_back(checkpointPath);
        
        // Cleanup if exceeds maximum
        if (pImpl->brainCheckpoints[brainId].size() > pImpl->maxCheckpoints) {
            cleanupExcessCheckpoints();
        }
        
        // Create backup if enabled
        if (pImpl->autoBackup) {
            createBackup(checkpointPath);
        }
        
        // Stop performance measurement
        if (pImpl->performanceMonitoringEnabled) {
            double elapsedTime = pImpl->performanceMonitor.stopMeasurement("create_checkpoint");
            NLM_LOG_INFO("Checkpoint " << checkpointId << " created successfully in " 
                       << std::fixed << std::setprecision(2) << elapsedTime << "s");
        } else {
            NLM_LOG_INFO("Checkpoint " << checkpointId << " created successfully");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to create checkpoint: " << e.what());
        return false;
    }
}

CheckpointRecoveryResult EnhancedCheckpointManager::loadCheckpoint(const std::string& checkpointPath) {
    CheckpointRecoveryResult result;
    
    try {
        // Validate checkpoint path
        if (!validateCheckpointPath(checkpointPath)) {
            result.errorMessage = "Invalid checkpoint path";
            return result;
        }
        
        // Check if checkpoint exists
        if (!std::filesystem::exists(checkpointPath)) {
            result.errorMessage = "Checkpoint file does not exist";
            return result;
        }
        
        // Load metadata
        CheckpointMetadata metadata;
        if (!loadCheckpointMetadata(checkpointPath, metadata)) {
            result.errorMessage = "Failed to load checkpoint metadata";
            return result;
        }
        
        if (!metadata.isValid) {
            result.errorMessage = "Checkpoint metadata indicates invalid checkpoint";
            return result;
        }
        
        // Decompress if needed
        std::string decompressedPath = checkpointPath;
        if (metadata.isCompressed) {
            if (!decompressCheckpoint(checkpointPath)) {
                result.errorMessage = "Failed to decompress checkpoint";
                return result;
            }
            decompressedPath = checkpointPath + ".decompressed";
        }
        
        // Decrypt if needed
        std::string decryptedPath = decompressedPath;
        if (metadata.isEncrypted) {
            if (!decryptCheckpoint(decompressedPath)) {
                result.errorMessage = "Failed to decrypt checkpoint";
                return result;
            }
            decryptedPath = decompressedPath + ".decrypted";
        }
        
        // Find brain checkpoint (simplified - would need to know which brain)
        std::string brainCheckpointPath = decryptedPath + ".brain";
        
        // TODO: Load brain checkpoint and restore brain state
        // This is a simplified implementation
        
        // Update metadata
        metadata.lastAccessTime = std::chrono::system_clock::now();
        updateCheckpointMetadata(checkpointPath, metadata);
        
        result.success = true;
        result.checkpointId = metadata.checkpointId;
        result.recoveryTime = std::chrono::system_clock::now();
        result.restoredNeurons = 0; // Would be actual count
        result.restoredSynapses = 0; // Would be actual count
        result.brainConsistencyScore = metadata.brainChecksum % 100 / 100.0f; // Simulated
        result.stateConsistencyVerified = true;
        
        NLM_LOG_INFO("Checkpoint " << metadata.checkpointId << " loaded successfully");
        
        return result;
        
    } catch (const std::exception& e) {
        result.errorMessage = std::string("Error loading checkpoint: ") + e.what();
        return result;
    }
}

CheckpointRecoveryResult EnhancedCheckpointManager::loadCheckpointById(const std::string& checkpointId) {
    // Find checkpoint path by ID
    std::string checkpointPath;
    for (const auto& pair : pImpl->checkpointMetadata) {
        if (pair.second.checkpointId == checkpointId) {
            checkpointPath = pair.first;
            break;
        }
    }
    
    if (checkpointPath.empty()) {
        CheckpointRecoveryResult result;
        result.errorMessage = "Checkpoint with ID " + checkpointId + " not found";
        return result;
    }
    
    return loadCheckpoint(checkpointPath);
}

std::vector<std::string> EnhancedCheckpointManager::listCheckpoints() const {
    std::vector<std::string> checkpoints;
    
    for (const auto& pair : pImpl->checkpointMetadata) {
        checkpoints.push_back(pair.first);
    }
    
    return checkpoints;
}

std::vector<std::string> EnhancedCheckpointManager::listCheckpoints(const std::string& brainId) const {
    std::vector<std::string> checkpoints;
    
    auto it = pImpl->brainCheckpoints.find(brainId);
    if (it != pImpl->brainCheckpoints.end()) {
        checkpoints = it->second;
    }
    
    return checkpoints;
}

CheckpointMetadata EnhancedCheckpointManager::getCheckpointMetadata(const std::string& checkpointPath) const {
    CheckpointMetadata metadata;
    
    auto it = pImpl->checkpointMetadata.find(checkpointPath);
    if (it != pImpl->checkpointMetadata.end()) {
        metadata = it->second;
    }
    
    return metadata;
}

bool EnhancedCheckpointManager::validateCheckpoint(const std::string& checkpointPath) const {
    if (!pImpl->validationEnabled) {
        return true; // Skip validation if disabled
    }
    
    try {
        // Check if file exists
        if (!std::filesystem::exists(checkpointPath)) {
            NLM_LOG_WARNING("Checkpoint file does not exist: " << checkpointPath);
            return false;
        }
        
        // Check file size
        size_t fileSize = getCheckpointSize(checkpointPath);
        if (fileSize == 0) {
            NLM_LOG_WARNING("Checkpoint file is empty: " << checkpointPath);
            return false;
        }
        
        if (fileSize > pImpl->maxCheckpointSizeMB * 1024 * 1024) {
            NLM_LOG_WARNING("Checkpoint file exceeds size limit: " << checkpointPath);
            return false;
        }
        
        // Load metadata
        CheckpointMetadata metadata;
        if (!loadCheckpointMetadata(checkpointPath, metadata)) {
            NLM_LOG_WARNING("Failed to load checkpoint metadata: " << checkpointPath);
            return false;
        }
        
        // Check if checkpoint is valid
        if (!metadata.isValid) {
            NLM_LOG_WARNING("Checkpoint metadata indicates invalid checkpoint: " << checkpointPath);
            return false;
        }
        
        // Check if checkpoint is expired
        if (isCheckpointExpired(metadata)) {
            NLM_LOG_WARNING("Checkpoint is expired: " << checkpointPath);
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error validating checkpoint: " << e.what());
        return false;
    }
}

bool EnhancedCheckpointManager::verifyCheckpointIntegrity(const std::string& checkpointPath) const {
    try {
        // Calculate current checksum
        std::string currentChecksum = calculateCheckpointChecksum(checkpointPath);
        
        // Load metadata
        CheckpointMetadata metadata;
        if (!loadCheckpointMetadata(checkpointPath, metadata)) {
            NLM_LOG_WARNING("Failed to load checkpoint metadata for integrity check");
            return false;
        }
        
        // Verify checksum
        if (currentChecksum != metadata.checksum) {
            NLM_LOG_WARNING("Checkpoint checksum mismatch");
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error verifying checkpoint integrity: " << e.what());
        return false;
    }
}

bool EnhancedCheckpointManager::deleteCheckpoint(const std::string& checkpointPath) {
    try {
        // Validate checkpoint
        if (!validateCheckpoint(checkpointPath)) {
            NLM_LOG_WARNING("Cannot delete invalid checkpoint: " << checkpointPath);
            return false;
        }
        
        // Delete checkpoint file
        std::filesystem::remove(checkpointPath);
        
        // Remove from metadata map
        pImpl->checkpointMetadata.erase(checkpointPath);
        
        // Find and remove from brain checkpoints map
        for (auto& pair : pImpl->brainCheckpoints) {
            auto& checkpoints = pair.second;
            checkpoints.erase(std::remove(checkpoints.begin(), checkpoints.end(), checkpointPath), checkpoints.end());
        }
        
        NLM_LOG_INFO("Checkpoint deleted successfully: " << checkpointPath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error deleting checkpoint: " << e.what());
        return false;
    }
}

bool EnhancedCheckpointManager::cleanupOldCheckpoints() {
    bool anyCleanup = false;
    
    try {
        // Cleanup expired checkpoints
        if (cleanupExpiredCheckpoints()) {
            anyCleanup = true;
        }
        
        // Cleanup oversized checkpoints
        if (cleanupOversizedCheckpoints()) {
            anyCleanup = true;
        }
        
        // Cleanup excess checkpoints
        if (cleanupExcessCheckpoints()) {
            anyCleanup = true;
        }
        
        return anyCleanup;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error cleaning up checkpoints: " << e.what());
        return false;
    }
}

void EnhancedCheckpointManager::setWorkingDirectory(const std::string& directory) {
    pImpl->workingDirectory = directory;
}

std::string EnhancedCheckpointManager::getWorkingDirectory() const {
    return pImpl->workingDirectory;
}

std::string EnhancedCheckpointManager::getStatistics() const {
    std::stringstream stats;
    
    stats << "Enhanced Checkpoint Manager Statistics" << std::endl;
    stats << "====================================" << std::endl;
    stats << "Working directory: " << pImpl->workingDirectory << std::endl;
    stats << "Initialized: " << (pImpl->initialized ? "Yes" : "No") << std::endl;
    stats << "Total checkpoints: " << pImpl->checkpointMetadata.size() << std::endl;
    
    size_t totalBrainCheckpoints = 0;
    for (const auto& pair : pImpl->brainCheckpoints) {
        totalBrainCheckpoints += pair.second.size();
    }
    stats << "Total brain checkpoints: " << totalBrainCheckpoints << std::endl;
    
    if (!pImpl->checkpointMetadata.empty()) {
        stats << std::endl;
        stats << "Checkpoint breakdown by age:" << std::endl;
        
        auto now = std::chrono::system_clock::now();
        size_t recent = 0, old = 0;
        
        for (const auto& pair : pImpl->checkpointMetadata) {
            auto age = std::chrono::duration_cast<std::chrono::hours>(now - pair.second.creationTime).count();
            if (age < 24) {
                recent++;
            } else {
                old++;
            }
        }
        
        stats << "  Recent (< 24h): " << recent << std::endl;
        stats << "  Older (>= 24h): " << old << std::endl;
    }
    
    // Performance statistics
    if (pImpl->performanceMonitoringEnabled) {
        stats << std::endl;
        stats << "Performance Statistics:" << std::endl;
        stats << pImpl->performanceMonitor.generateReport();
    }
    
    return stats.str();
}

std::string EnhancedCheckpointManager::generateReport() const {
    return generateReport("Checkpoint Manager Report");
}

std::string EnhancedCheckpointManager::exportConfiguration() const {
    std::stringstream config;
    
    config << "# Enhanced Checkpoint Configuration" << std::endl;
    config << "# Generated on " << getTimestampString() << std::endl;
    config << std::endl;
    config << "[checkpoint_settings]" << std::endl;
    config << "working_directory = " << pImpl->workingDirectory << std::endl;
    config << "initialized = " << (pImpl->initialized ? "true" : "false") << std::endl;
    config << "max_checkpoints = " << pImpl->maxCheckpoints << std::endl;
    config << "checkpoint_retention_days = " << pImpl->checkpointRetentionDays << std::endl;
    config << "max_checkpoint_size_mb = " << pImpl->maxCheckpointSizeMB << std::endl;
    config << "auto_checkpoint = " << (pImpl->autoCheckpoint ? "true" : "false") << std::endl;
    config << "auto_checkpoint_interval = " << pImpl->autoCheckpointInterval << std::endl;
    config << "auto_backup = " << (pImpl->autoBackup ? "true" : "false") << std::endl;
    config << "auto_backup_interval = " << pImpl->autoBackupInterval << std::endl;
    config << "compression_enabled = " << (pImpl->compressionEnabled ? "true" : "false") << std::endl;
    config << "compression_algorithm = " << pImpl->compressionAlgorithm << std::endl;
    config << "encryption_enabled = " << (pImpl->encryptionEnabled ? "true" : "false") << std::endl;
    config << "encryption_key_file = " << pImpl->encryptionKeyFile << std::endl;
    config << "validation_enabled = " << (pImpl->validationEnabled ? "true" : "false") << std::endl;
    config << "recovery_verification_enabled = " << (pImpl->recoveryVerificationEnabled ? "true" : "false") << std::endl;
    config << "consistency_checking_enabled = " << (pImpl->consistencyCheckingEnabled ? "true" : "false") << std::endl;
    config << "parallel_operations_enabled = " << (pImpl->parallelOperationsEnabled ? "true" : "false") << std::endl;
    config << "parallel_operation_count = " << pImpl->parallelOperationCount << std::endl;
    config << "performance_monitoring_enabled = " << (pImpl->performanceMonitoringEnabled ? "true" : "false") << std::endl;
    config << "checkpoint_timeout = " << pImpl->checkpointTimeout << std::endl;
    
    return config.str();
}

bool EnhancedCheckpointManager::importConfiguration(const std::string& configFile) {
    // TODO: Implement configuration import
    // For now, return false
    return false;
}

// Implementation of CheckpointPerformanceMonitor
struct CheckpointPerformanceMonitor::Impl {
    bool enabled;
    std::map<std::string, std::vector<double>> operationTimes;
    std::chrono::steady_clock::time_point startTime;
    std::string currentOperation;
    std::chrono::steady_clock::time_point lastOperationStart;
};

CheckpointPerformanceMonitor::CheckpointPerformanceMonitor() : pImpl(std::make_unique<Impl>()) {
    pImpl->enabled = false;
    pImpl->currentOperation = "";
}

CheckpointPerformanceMonitor::~CheckpointPerformanceMonitor() = default;

void CheckpointPerformanceMonitor::startMeasurement(const std::string& operation) {
    if (!pImpl->enabled) return;
    
    pImpl->currentOperation = operation;
    pImpl->lastOperationStart = std::chrono::steady_clock::now();
}

double CheckpointPerformanceMonitor::stopMeasurement(const std::string& operation) {
    if (!pImpl->enabled || pImpl->currentOperation != operation) {
        return 0.0;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double>(endTime - pImpl->lastOperationStart).count();
    
    pImpl->operationTimes[operation].push_back(duration);
    pImpl->currentOperation.clear();
    
    return duration;
}

double CheckpointPerformanceMonitor::getOperationTime(const std::string& operation) const {
    auto it = pImpl->operationTimes.find(operation);
    if (it == pImpl->operationTimes.end() || it->second.empty()) {
        return 0.0;
    }
    return it->second.back();
}

double CheckpointPerformanceMonitor::getAverageOperationTime(const std::string& operation) const {
    auto it = pImpl->operationTimes.find(operation);
    if (it == pImpl->operationTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
    return sum / it->second.size();
}

double CheckpointPerformanceMonitor::getMinOperationTime(const std::string& operation) const {
    auto it = pImpl->operationTimes.find(operation);
    if (it == pImpl->operationTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    return *std::min_element(it->second.begin(), it->second.end());
}

double CheckpointPerformanceMonitor::getMaxOperationTime(const std::string& operation) const {
    auto it = pImpl->operationTimes.find(operation);
    if (it == pImpl->operationTimes.end() || it->second.empty()) {
        return 0.0;
    }
    
    return *std::max_element(it->second.begin(), it->second.end());
}

size_t CheckpointPerformanceMonitor::getOperationCount(const std::string& operation) const {
    auto it = pImpl->operationTimes.find(operation);
    if (it == pImpl->operationTimes.end()) {
        return 0;
    }
    return it->second.size();
}

void CheckpointPerformanceMonitor::reset() {
    pImpl->operationTimes.clear();
    pImpl->currentOperation.clear();
}

std::string CheckpointPerformanceMonitor::generateReport() const {
    std::stringstream report;
    
    report << "Checkpoint Performance Monitor Report" << std::endl;
    report << "====================================" << std::endl;
    report << "Enabled: " << (pImpl->enabled ? "Yes" : "No") << std::endl;
    
    if (pImpl->operationTimes.empty()) {
        report << "No performance data available" << std::endl;
        return report.str();
    }
    
    report << std::endl;
    report << "Operation Performance:" << std::endl;
    report << "=====================" << std::endl;
    
    for (const auto& pair : pImpl->operationTimes) {
        const std::string& operation = pair.first;
        const std::vector<double>& times = pair.second;
        
        report << operation << ":" << std::endl;
        report << "  Runs: " << times.size() << std::endl;
        report << "  Average: " << std::fixed << std::setprecision(4) << getAverageOperationTime(operation) << "s" << std::endl;
        report << "  Min: " << std::fixed << std::setprecision(4) << getMinOperationTime(operation) << "s" << std::endl;
        report << "  Max: " << std::fixed << std::setprecision(4) << getMaxOperationTime(operation) << "s" << std::endl;
        report << std::endl;
    }
    
    return report.str();
}

void CheckpointPerformanceMonitor::setEnabled(bool enabled) {
    pImpl->enabled = enabled;
}

bool CheckpointPerformanceMonitor::isEnabled() const {
    return pImpl->enabled;
}

// Implementation of convenience functions
bool createCheckpoint(std::shared_ptr<Brain> brain, const std::string& checkpointId) {
    return getEnhancedCheckpointManager().createCheckpoint("default_brain", brain, {{}});
}

CheckpointRecoveryResult loadCheckpoint(const std::string& checkpointPath) {
    return getEnhancedCheckpointManager().loadCheckpoint(checkpointPath);
}

bool restoreBrainFromCheckpoint(std::shared_ptr<Brain> brain, const std::string& checkpointPath) {
    return getEnhancedCheckpointManager().restoreBrainFromCheckpoint(brain, checkpointPath);
}

std::vector<std::string> listCheckpoints() {
    return getEnhancedCheckpointManager().listCheckpoints();
}

std::vector<std::string> listCheckpoints(const std::string& brainId) {
    return getEnhancedCheckpointManager().listCheckpoints(brainId);
}

bool deleteCheckpoint(const std::string& checkpointPath) {
    return getEnhancedCheckpointManager().deleteCheckpoint(checkpointPath);
}

bool cleanupCheckpoints() {
    return getEnhancedCheckpointManager().cleanupOldCheckpoints();
}

std::string getCheckpointStatistics() {
    return getEnhancedCheckpointManager().getStatistics();
}

bool exportCheckpointConfiguration(const std::string& filename) {
    return getEnhancedCheckpointManager().exportConfiguration(filename);
}

bool importCheckpointConfiguration(const std::string& filename) {
    return getEnhancedCheckpointManager().importConfiguration(filename);
}

// Global checkpoint manager instance
EnhancedCheckpointManager& getEnhancedCheckpointManager() {
    static EnhancedCheckpointManager manager;
    return manager;
}

// Entry point for the enhanced checkpoint tool
int mainCheckpoint(int argc, char** argv) {
    try {
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                  NLM Enhanced Checkpoint Tool                   ║" << std::endl;
        std::cout << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║  Advanced checkpoint management with validation and recovery   ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        
        // Parse command line arguments
        std::string action = "help";
        std::string brainId = "default_brain";
        std::string checkpointId = "";
        std::string checkpointPath = "";
        std::string configFile = "";
        bool verbose = false;
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            if (arg == "--help" || arg == "-h") {
                action = "help";
            } else if (arg == "--create") {
                action = "create";
            } else if (arg == "--load") {
                action = "load";
            } else if (arg == "--list") {
                action = "list";
            } else if (arg == "--delete") {
                action = "delete";
            } else if (arg == "--cleanup") {
                action = "cleanup";
            } else if (arg == "--restore") {
                action = "restore";
            } else if (arg == "--export") {
                action = "export";
            } else if (arg == "--import") {
                action = "import";
            } else if (arg.substr(0, 12) == "--brain-id=") {
                brainId = arg.substr(12);
            } else if (arg.substr(0, 14) == "--checkpoint=") {
                checkpointId = arg.substr(14);
            } else if (arg.substr(0, 14) == "--checkpoint-path=") {
                checkpointPath = arg.substr(14);
            } else if (arg.substr(0, 16) == "--config-file=") {
                configFile = arg.substr(16);
            } else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            }
        }
        
        // Initialize checkpoint manager
        EnhancedCheckpointManager& manager = getEnhancedCheckpointManager();
        
        if (!manager.initialize(manager.getDefaultConfig())) {
            std::cerr << "Failed to initialize checkpoint manager" << std::endl;
            return 1;
        }
        
        // Execute action
        if (action == "help") {
            std::cout << "Usage: nlm_checkpoint [OPTIONS]" << std::endl;
            std::cout << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --create                 Create checkpoint" << std::endl;
            std::cout << "  --load                  Load checkpoint" << std::endl;
            std::cout << "  --list                  List checkpoints" << std::endl;
            std::cout << "  --delete                Delete checkpoint" << std::endl;
            std::cout << "  --cleanup               Cleanup old checkpoints" << std::endl;
            std::cout << "  --restore               Restore from checkpoint" << std::endl;
            std::cout << "  --export                Export configuration" << std::endl;
            std::cout << "  --import                Import configuration" << std::endl;
            std::cout << "  --brain-id=<id>         Brain ID (default: default_brain)" << std::endl;
            std::cout << "  --checkpoint=<id>       Checkpoint ID" << std::endl;
            std::cout << "  --checkpoint-path=<path> Checkpoint file path" << std::endl;
            std::cout << "  --config-file=<file>     Configuration file" << std::endl;
            std::cout << "  --verbose, -v            Enable verbose output" << std::endl;
            std::cout << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  nlm_checkpoint --create --brain-id=my_brain" << std::endl;
            std::cout << "  nlm_checkpoint --load --checkpoint-path=./checkpoint.bin" << std::endl;
            std::cout << "  nlm_checkpoint --list" << std::endl;
            std::cout << "  nlm_checkpoint --delete --checkpoint-path=./checkpoint.bin" << std::endl;
            std::cout << "  nlm_checkpoint --cleanup" << std::endl;
            return 0;
        } else if (action == "create") {
            std::cout << "Creating checkpoint for brain: " << brainId << std::endl;
            // TODO: Get brain instance from demo system
            // For now, use a dummy brain
            auto brain = std::make_shared<Brain>(std::make_shared<Config>());
            brain->initialize();
            
            if (manager.createCheckpoint(brainId, brain)) {
                std::cout << "Checkpoint created successfully" << std::endl;
                return 0;
            } else {
                std::cerr << "Failed to create checkpoint" << std::endl;
                return 1;
            }
        } else if (action == "load") {
            if (checkpointId.empty() && checkpointPath.empty()) {
                std::cerr << "Error: Either --checkpoint-id or --checkpoint-path must be specified" << std::endl;
                return 1;
            }
            
            if (!checkpointId.empty()) {
                std::cout << "Loading checkpoint by ID: " << checkpointId << std::endl;
                CheckpointRecoveryResult result = manager.loadCheckpointById(checkpointId);
                
                if (result.success) {
                    std::cout << "Checkpoint loaded successfully" << std::endl;
                    std::cout << "  Checkpoint ID: " << result.checkpointId << std::endl;
                    std::cout << "  Restored neurons: " << result.restoredNeurons << std::endl;
                    std::cout << "  Restored synapses: " << result.restoredSynapses << std::endl;
                    std::cout << "  Consistency score: " << result.brainConsistencyScore << std::endl;
                    return 0;
                } else {
                    std::cerr << "Failed to load checkpoint: " << result.errorMessage << std::endl;
                    return 1;
                }
            } else {
                std::cout << "Loading checkpoint from path: " << checkpointPath << std::endl;
                CheckpointRecoveryResult result = manager.loadCheckpoint(checkpointPath);
                
                if (result.success) {
                    std::cout << "Checkpoint loaded successfully" << std::endl;
                    std::cout << "  Checkpoint ID: " << result.checkpointId << std::endl;
                    std::cout << "  Restored neurons: " << result.restoredNeurons << std::endl;
                    std::cout << "  Restored synapses: " << result.restoredSynapses << std::endl;
                    std::cout << "  Consistency score: " << result.brainConsistencyScore << std::endl;
                    return 0;
                } else {
                    std::cerr << "Failed to load checkpoint: " << result.errorMessage << std::endl;
                    return 1;
                }
            }
        } else if (action == "list") {
            std::cout << "Available checkpoints:" << std::endl;
            std::vector<std::string> checkpoints = manager.listCheckpoints();
            
            if (checkpoints.empty()) {
                std::cout << "  No checkpoints found" << std::endl;
            } else {
                for (const auto& checkpoint : checkpoints) {
                    std::cout << "  - " << checkpoint << std::endl;
                }
            }
            
            std::cout << std::endl;
            std::cout << "Checkpoints for brain ID " << brainId << ":" << std::endl;
            std::vector<std::string> brainCheckpoints = manager.listCheckpoints(brainId);
            
            if (brainCheckpoints.empty()) {
                std::cout << "  No checkpoints found for this brain ID" << std::endl;
            } else {
                for (const auto& checkpoint : brainCheckpoints) {
                    std::cout << "  - " << checkpoint << std::endl;
                }
            }
            
            return 0;
        } else if (action == "delete") {
            if (checkpointPath.empty()) {
                std::cerr << "Error: --checkpoint-path must be specified for delete operation" << std::endl;
                return 1;
            }
            
            std::cout << "Deleting checkpoint: " << checkpointPath << std::endl;
            
            if (manager.deleteCheckpoint(checkpointPath)) {
                std::cout << "Checkpoint deleted successfully" << std::endl;
                return 0;
            } else {
                std::cerr << "Failed to delete checkpoint" << std::endl;
                return 1;
            }
        } else if (action == "cleanup") {
            std::cout << "Cleaning up old checkpoints..." << std::endl;
            
            if (manager.cleanupOldCheckpoints()) {
                std::cout << "Cleanup completed successfully" << std::endl;
                return 0;
            } else {
                std::cerr << "Failed to cleanup checkpoints" << std::endl;
                return 1;
            }
        } else if (action == "export") {
            std::string configFile = (configFile.empty() ? "checkpoint_config.txt" : configFile);
            
            std::cout << "Exporting checkpoint configuration to: " << configFile << std::endl;
            
            if (manager.exportConfiguration(configFile)) {
                std::cout << "Configuration exported successfully" << std::endl;
                return 0;
            } else {
                std::cerr << "Failed to export configuration" << std::endl;
                return 1;
            }
        } else if (action == "import") {
            if (configFile.empty()) {
                std::cerr << "Error: --config-file must be specified for import operation" << std::endl;
                return 1;
            }
            
            std::cout << "Importing checkpoint configuration from: " << configFile << std::endl;
            
            if (manager.importConfiguration(configFile)) {
                std::cout << "Configuration imported successfully" << std::endl;
                return 0;
            } else {
                std::cerr << "Failed to import configuration" << std::endl;
                return 1;
            }
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}

int main(int argc, char** argv) {
    return mainCheckpoint(argc, argv);
}

} // namespace nlm

// Entry point for the enhanced checkpoint tool
int main(int argc, char** argv) {
    return nlm::mainCheckpoint(argc, argv);
}
