#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../memory_management/memory_manager.h"
#include "../checkpoint_management/checkpoint_manager.h"
#include "../memory_monitoring/memory_usage_monitor.h"
#include "../backup_management/backup_system.h"
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace nlm {

class EnhancedMemoryManagementCLI {
public:
    EnhancedMemoryManagementCLI() {
        // Initialize components
        memoryManager_ = std::make_shared<MemoryManager>();
        checkpointManager_ = std::make_shared<CheckpointManager>();
        memoryMonitor_ = std::make_shared<MemoryUsageMonitor>();
        backupSystem_ = std::make_shared<BackupSystem>();
        
        // Link components
        memoryManager_->setCheckpointManager(checkpointManager_.get());
        memoryManager_->setMemoryUsageMonitor(memoryMonitor_.get());
        memoryManager_->setBackupSystem(backupSystem_.get());
        
        checkpointManager_->setMemoryManager(memoryManager_.get());
        memoryMonitor_->setMemoryManager(memoryManager_.get());
        backupSystem_->setMemoryManager(memoryManager_.get());
        backupSystem_->setCheckpointManager(checkpointManager_.get());
        
        NLM_LOG_INFO("Enhanced Memory Management CLI initialized");
    }
    
    void initializeWithBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
        memoryManager_->initialize(brain_.get());
        
        // Configure initial settings
        memoryManager_->setWorkingMemoryCapacity(1000);
        memoryManager_->setEpisodicMemoryCapacity(10000);
        memoryManager_->setAssociationDecayRate(0.01f);
        memoryManager_->setMemoryDecayRate(0.01f);
        
        memoryMonitor_->startTracking();
        
        NLM_LOG_INFO("Enhanced Memory Management CLI configured with brain");
    }
    
    // Memory Management Commands
    void memoryInfo() {
        NLM_LOG_INFO("=== Memory Management Status ===");
        
        MemoryStatistics stats = memoryManager_->getMemoryStatistics();
        NLM_LOG_INFO("Memory Usage: " + std::to_string(stats.usedMemory) + " / " + std::to_string(stats.totalMemory) + " bytes");
        NLM_LOG_INFO("Peak Memory: " + std::to_string(stats.peakMemory) + " bytes");
        NLM_LOG_INFO("Memory Efficiency: " + std::to_string(stats.efficiency * 100) + "%");
        NLM_LOG_INFO("Allocated Objects: " + std::to_string(stats.allocatedObjects));
        NLM_LOG_INFO("Deallocated Objects: " + std::to_string(stats.deallocatedObjects));
        
        MemoryStatistics monitorStats = memoryMonitor_->getCurrentMemoryStats();
        NLM_LOG_INFO("Monitor Memory Efficiency: " + std::to_string(monitorStats.efficiency * 100) + "%");
        NLM_LOG_INFO("Memory Fragmentation: " + std::to_string(monitorStats.fragmentation * 100) + "%");
        
        NLM_LOG_INFO("Memory Pressure Level: " + std::to_string(memoryManager_->getMemoryPressureLevel()));
    }
    
    void workingMemorySetCapacity(size_t capacity) {
        memoryManager_->setWorkingMemoryCapacity(capacity);
        NLM_LOG_INFO("Working memory capacity set to: " + std::to_string(capacity));
    }
    
    void workingMemoryStore(NeuronId neuron, float activation) {
        memoryManager_->storeToWorkingMemory(neuron, activation);
        NLM_LOG_INFO("Stored neuron " + std::to_string(neuron) + " with activation: " + std::to_string(activation));
    }
    
    void workingMemoryRetrieve(NeuronId neuron) {
        float activation = memoryManager_->retrieveFromWorkingMemory(neuron);
        NLM_LOG_INFO("Retrieved neuron " + std::to_string(neuron) + ": activation = " + std::to_string(activation));
    }
    
    void workingMemoryClear() {
        memoryManager_->clearWorkingMemory();
        NLM_LOG_INFO("Working memory cleared");
    }
    
    void episodicMemorySetCapacity(size_t capacity) {
        memoryManager_->setEpisodicMemoryCapacity(capacity);
        NLM_LOG_INFO("Episodic memory capacity set to: " + std::to_string(capacity));
    }
    
    void episodicMemoryStore(const AdvancedEpisodicMemoryItem& item) {
        memoryManager_->storeToEpisodicMemory(item);
        NLM_LOG_INFO("Stored episodic memory item with timestamp: " + std::to_string(item.timestamp));
    }
    
    void episodicMemoryRetrieve(SimulationStep id) {
        const AdvancedEpisodicMemoryItem* item = memoryManager_->retrieveFromEpisodicMemory(id);
        if (item) {
            NLM_LOG_INFO("Retrieved episodic memory item with reward: " + std::to_string(item->reward));
        } else {
            NLM_LOG_WARNING("Episodic memory item not found");
        }
    }
    
    void episodicMemoryGetRecent(size_t count) {
        std::vector<SimulationStep> recent = memoryManager_->getRecentEpisodes(count);
        NLM_LOG_INFO("Recent episodes (count: " + std::to_string(recent.size()) + "):");
        for (SimulationStep step : recent) {
            NLM_LOG_INFO("  - Episode " + std::to_string(step));
        }
    }
    
    void episodicMemoryConsolidate() {
        memoryManager_->consolidateMemory();
        NLM_LOG_INFO("Memory consolidation completed");
    }
    
    void associativeMemorySetDecay(float rate) {
        memoryManager_->setAssociationDecayRate(rate);
        NLM_LOG_INFO("Associative memory decay rate set to: " + std::to_string(rate));
    }
    
    void associativeMemoryStore(const AdvancedPattern& pattern1, const AdvancedPattern& pattern2, float strength) {
        memoryManager_->storeAssociation(pattern1, pattern2, strength);
        NLM_LOG_INFO("Stored associative memory with strength: " + std::to_string(strength));
    }
    
    void associativeMemoryRecall(const AdvancedPattern& cue) {
        std::vector<float> recalled = memoryManager_->recallPattern(cue);
        NLM_LOG_INFO("Associative memory recall result (count: " + std::to_string(recalled.size()) + "):");
        for (size_t i = 0; i < recalled.size(); ++i) {
            NLM_LOG_INFO("  - Similarity " + std::to_string(i) + ": " + std::to_string(recalled[i]));
        }
    }
    
    void associativeMemoryClear() {
        memoryManager_->clearAssociativeMemory();
        NLM_LOG_INFO("Associative memory cleared");
    }
    
    void memoryConsolidate() {
        memoryManager_->consolidateMemories();
        memoryManager_->consolidateImportantMemories();
        memoryManager_->consolidateUnusedMemories(0.5);
        NLM_LOG_INFO("Memory consolidation completed");
    }
    
    void memoryForget(NeuronId neuron) {
        memoryManager_->forgetMemory(neuron);
        NLM_LOG_INFO("Forgetting memory for neuron " + std::to_string(neuron));
    }
    
    void memoryForgetUnused(double threshold) {
        memoryManager_->forgetUnusedMemories(threshold);
        NLM_LOG_INFO("Forgetting unused memories with threshold: " + std::to_string(threshold));
    }
    
    void memorySetDecayRate(float rate) {
        memoryManager_->setMemoryDecayRate(rate);
        NLM_LOG_INFO("Memory decay rate set to: " + std::to_string(rate));
    }
    
    void memoryReduceInterference() {
        memoryManager_->reduceMemoryInterference();
        NLM_LOG_INFO("Memory interference reduced");
    }
    
    void memoryEnhanceRecall() {
        memoryManager_->enhanceMemoryRecall();
        NLM_LOG_INFO("Memory recall enhanced");
    }
    
    void memoryCalculateInterference(NeuronId neuron) {
        float interference = memoryManager_->calculateMemoryInterference(neuron);
        NLM_LOG_INFO("Memory interference for neuron " + std::to_string(neuron) + ": " + std::to_string(interference));
    }
    
    // Memory Optimization Commands
    void memoryOptimize() {
        memoryManager_->optimizeMemoryUsage();
        memoryMonitor_->optimizeMemoryUsage();
        NLM_LOG_INFO("Memory optimization completed");
    }
    
    void memoryPreallocate(size_t size) {
        memoryManager_->preallocateMemory(size);
        NLM_LOG_INFO("Memory preallocated: " + std::to_string(size) + " bytes");
    }
    
    void memoryCleanup() {
        memoryManager_->cleanupUnusedMemory();
        memoryMonitor_->optimizeMemoryUsage();
        NLM_LOG_INFO("Memory cleanup completed");
    }
    
    void memoryHandlePressure() {
        memoryManager_->handleMemoryPressure();
        NLM_LOG_INFO("Memory pressure handled");
    }
    
    // Memory Monitoring Commands
    void monitorStart() {
        memoryMonitor_->startTracking();
        NLM_LOG_INFO("Memory monitoring started");
    }
    
    void monitorStop() {
        memoryMonitor_->stopTracking();
        NLM_LOG_INFO("Memory monitoring stopped");
    }
    
    void monitorStats() {
        MemoryStatistics stats = memoryMonitor_->getCurrentMemoryStats();
        NLM_LOG_INFO("=== Memory Monitor Statistics ===");
        NLM_LOG_INFO("Total Memory: " + std::to_string(stats.totalMemory) + " bytes");
        NLM_LOG_INFO("Used Memory: " + std::to_string(stats.usedMemory) + " bytes");
        NLM_LOG_INFO("Peak Memory: " + std::to_string(stats.peakMemory) + " bytes");
        NLM_LOG_INFO("Allocated Objects: " + std::to_string(stats.allocatedObjects));
        NLM_LOG_INFO("Deallocated Objects: " + std::to_string(stats.deallocatedObjects));
        NLM_LOG_INFO("Efficiency: " + std::to_string(stats.efficiency * 100) + "%");
        NLM_LOG_INFO("Fragmentation: " + std::to_string(stats.fragmentation * 100) + "%");
    }
    
    void monitorLeaks() {
        size_t leakCount = memoryMonitor_->detectMemoryLeaks();
        NLM_LOG_INFO("Detected memory leaks: " + std::to_string(leakCount));
        
        std::vector<std::string> leaks = memoryMonitor_->getDetectedLeaks();
        if (!leaks.empty()) {
            NLM_LOG_INFO("Leaked sources:");
            for (const std::string& leak : leaks) {
                NLM_LOG_INFO("  - " + leak);
            }
        }
    }
    
    void monitorFragmentation() {
        double fragmentation = memoryMonitor_->calculateMemoryFragmentation();
        NLM_LOG_INFO("Memory fragmentation: " + std::to_string(fragmentation * 100) + "%");
    }
    
    void monitorOptimize() {
        memoryMonitor_->optimizeMemoryUsage();
        NLM_LOG_INFO("Memory monitor optimization completed");
    }
    
    void monitorRecommendations() {
        std::vector<std::string> recommendations = memoryMonitor_->getOptimizationRecommendations();
        NLM_LOG_INFO("Memory optimization recommendations:");
        for (const std::string& recommendation : recommendations) {
            NLM_LOG_INFO("  - " + recommendation);
        }
    }
    
    void monitorApplyRecommendation(const std::string& recommendation) {
        memoryMonitor_->applyMemoryOptimization(recommendation);
        NLM_LOG_INFO("Applied recommendation: " + recommendation);
    }
    
    void monitorEnableLeakDetection(bool enable) {
        memoryMonitor_->enableLeakDetection(enable);
        NLM_LOG_INFO("Leak detection " + std::string(enable ? "enabled" : "disabled"));
    }
    
    // Checkpoint Management Commands
    void checkpointSetDirectory(const std::string& directory) {
        checkpointManager_->setCheckpointDirectory(directory);
        NLM_LOG_INFO("Checkpoint directory set to: " + directory);
    }
    
    void checkpointSetFrequency(size_t frequency) {
        checkpointManager_->setCheckpointFrequency(frequency);
        NLM_LOG_INFO("Checkpoint frequency set to: " + std::to_string(frequency) + " steps");
    }
    
    void checkpointSetCompression(bool enabled) {
        checkpointManager_->setCheckpointCompression(enabled);
        NLM_LOG_INFO("Checkpoint compression " + std::string(enabled ? "enabled" : "disabled"));
    }
    
    void checkpointSetEncryption(bool enabled) {
        checkpointManager_->setCheckpointEncryption(enabled);
        NLM_LOG_INFO("Checkpoint encryption " + std::string(enabled ? "enabled" : "disabled"));
    }
    
    void checkpointSetMaxSize(size_t size) {
        checkpointManager_->setMaxCheckpointSize(size);
        NLM_LOG_INFO("Maximum checkpoint size set to: " + std::to_string(size) + " bytes");
    }
    
    std::string checkpointCreate(const std::string& description) {
        std::string checkpointId = checkpointManager_->createCheckpoint(description);
        NLM_LOG_INFO("Created checkpoint: " + checkpointId);
        return checkpointId;
    }
    
    bool checkpointRestore(const std::string& checkpointId) {
        bool success = checkpointManager_->restoreCheckpoint(checkpointId);
        if (success) {
            NLM_LOG_INFO("Restored checkpoint: " + checkpointId);
        } else {
            NLM_LOG_ERROR("Failed to restore checkpoint: " + checkpointId);
        }
        return success;
    }
    
    bool checkpointDelete(const std::string& checkpointId) {
        bool success = checkpointManager_->deleteCheckpoint(checkpointId);
        if (success) {
            NLM_LOG_INFO("Deleted checkpoint: " + checkpointId);
        } else {
            NLM_LOG_ERROR("Failed to delete checkpoint: " + checkpointId);
        }
        return success;
    }
    
    void checkpointList() {
        NLM_LOG_INFO("=== Checkpoints ===");
        std::vector<std::string> checkpointIds = checkpointManager_->getCheckpointIds();
        NLM_LOG_INFO("Total checkpoints: " + std::to_string(checkpointIds.size()));
        
        for (const std::string& id : checkpointIds) {
            CheckpointMetadata metadata = checkpointManager_->getCheckpointInfo(id);
            NLM_LOG_INFO("  - " + id + " (step: " + std::to_string(metadata.step) + ", size: " + 
                        std::to_string(metadata.size) + " bytes, " + 
                        (metadata.isCompressed ? "compressed, " : "") +
                        (metadata.isEncrypted ? "encrypted" : "") + ")");
        }
    }
    
    std::string checkpointIncremental(const std::string& baseCheckpointId) {
        std::string checkpointId = checkpointManager_->createIncrementalCheckpoint(baseCheckpointId);
        if (!checkpointId.empty()) {
            NLM_LOG_INFO("Created incremental checkpoint: " + checkpointId + " from " + baseCheckpointId);
        } else {
            NLM_LOG_ERROR("Failed to create incremental checkpoint from " + baseCheckpointId);
        }
        return checkpointId;
    }
    
    bool checkpointApplyIncremental(const std::string& checkpointId) {
        bool success = checkpointManager_->applyIncrementalCheckpoint(checkpointId);
        if (success) {
            NLM_LOG_INFO("Applied incremental checkpoint: " + checkpointId);
        } else {
            NLM_LOG_ERROR("Failed to apply incremental checkpoint: " + checkpointId);
        }
        return success;
    }
    
    std::string checkpointDifferential(const std::string& baseCheckpointId) {
        std::string checkpointId = checkpointManager_->createDifferentialCheckpoint(baseCheckpointId);
        if (!checkpointId.empty()) {
            NLM_LOG_INFO("Created differential checkpoint: " + checkpointId + " from " + baseCheckpointId);
        } else {
            NLM_LOG_ERROR("Failed to create differential checkpoint from " + baseCheckpointId);
        }
        return checkpointId;
    }
    
    bool checkpointApplyDifferential(const std::string& checkpointId) {
        bool success = checkpointManager_->applyDifferentialCheckpoint(checkpointId);
        if (success) {
            NLM_LOG_INFO("Applied differential checkpoint: " + checkpointId);
        } else {
            NLM_LOG_ERROR("Failed to apply differential checkpoint: " + checkpointId);
        }
        return success;
    }
    
    bool checkpointValidate(const std::string& checkpointId) {
        bool valid = checkpointManager_->validateCheckpoint(checkpointId);
        if (valid) {
            NLM_LOG_INFO("Checkpoint validation passed: " + checkpointId);
        } else {
            NLM_LOG_WARNING("Checkpoint validation failed: " + checkpointId);
            NLM_LOG_WARNING(checkpointManager_->getCheckpointValidationReport());
        }
        return valid;
    }
    
    bool checkpointValidateAll() {
        bool valid = checkpointManager_->validateAllCheckpoints();
        if (valid) {
            NLM_LOG_INFO("All checkpoints validated successfully");
        } else {
            NLM_LOG_WARNING("Some checkpoints failed validation");
            NLM_LOG_WARNING(checkpointManager_->getCheckpointValidationReport());
        }
        return valid;
    }
    
    void checkpointEnableParallel(bool enable) {
        checkpointManager_->enableParallelCheckpoints(enable);
        NLM_LOG_INFO("Parallel checkpointing " + std::string(enable ? "enabled" : "disabled"));
    }
    
    void checkpointSetCompressionAlgorithm(const std::string& algorithm) {
        CompressionAlgorithm alg;
        if (algorithm == "gzip") alg = CompressionAlgorithm::Gzip;
        else if (algorithm == "lz4") alg = CompressionAlgorithm::LZ4;
        else if (algorithm == "zstd") alg = CompressionAlgorithm::Zstd;
        else if (algorithm == "brotli") alg = CompressionAlgorithm::Brotli;
        else alg = CompressionAlgorithm::None;
        
        checkpointManager_->setCompressionAlgorithm(alg);
        NLM_LOG_INFO("Compression algorithm set to: " + algorithm);
    }
    
    void checkpointSetEncryptionAlgorithm(const std::string& algorithm) {
        EncryptionAlgorithm alg;
        if (algorithm == "aes128") alg = EncryptionAlgorithm::AES128;
        else if (algorithm == "aes256") alg = EncryptionAlgorithm::AES256;
        else if (algorithm == "chacha20") alg = EncryptionAlgorithm::ChaCha20;
        else alg = EncryptionAlgorithm::None;
        
        checkpointManager_->setEncryptionAlgorithm(alg);
        NLM_LOG_INFO("Encryption algorithm set to: " + algorithm);
    }
    
    void checkpointSetEncryptionKey(const std::string& key) {
        checkpointManager_->setEncryptionKey(key);
        NLM_LOG_INFO("Encryption key set");
    }
    
    void checkpointSetEncryptionSalt(const std::string& salt) {
        checkpointManager_->setEncryptionSalt(salt);
        NLM_LOG_INFO("Encryption salt set");
    }
    
    void checkpointCleanup(size_t maxCheckpoints) {
        checkpointManager_->cleanupOldCheckpoints(maxCheckpoints);
        NLM_LOG_INFO("Cleaned up old checkpoints, keeping " + std::to_string(maxCheckpoints) + " most recent");
    }
    
    void checkpointCompact() {
        checkpointManager_->compactCheckpoints();
        NLM_LOG_INFO("Checkpoint compaction completed");
    }
    
    void checkpointRebuildIndex() {
        checkpointManager_->rebuildIndex();
        NLM_LOG_INFO("Checkpoint index rebuilt");
    }
    
    std::vector<std::string> checkpointCreateBatch(size_t count, const std::string& baseDescription) {
        return checkpointManager_->createBatchCheckpoints(count, baseDescription);
    }
    
    bool checkpointRestoreBatch(const std::vector<std::string>& checkpointIds) {
        return checkpointManager_->restoreBatchCheckpoints(checkpointIds);
    }
    
    // Backup Management Commands
    void backupSetDirectory(const std::string& directory) {
        backupSystem_->setBackupDirectory(directory);
        NLM_LOG_INFO("Backup directory set to: " + directory);
    }
    
    void backupSetFrequency(size_t frequency) {
        backupSystem_->setBackupFrequency(frequency);
        NLM_LOG_INFO("Backup frequency set to: " + std::to_string(frequency) + " seconds");
    }
    
    void backupSetEncryption(bool enabled) {
        backupSystem_->setBackupEncryption(enabled);
        NLM_LOG_INFO("Backup encryption " + std::string(enabled ? "enabled" : "disabled"));
    }
    
    void backupSetMaxBackups(size_t maxBackups) {
        backupSystem_->setMaxBackups(maxBackups);
        NLM_LOG_INFO("Maximum backups set to: " + std::to_string(maxBackups));
    }
    
    std::string backupCreate(const std::string& description) {
        std::string backupId = backupSystem_->createBackup(description);
        NLM_LOG_INFO("Created backup: " + backupId);
        return backupId;
    }
    
    bool backupRestore(const std::string& backupId) {
        bool success = backupSystem_->restoreBackup(backupId);
        if (success) {
            NLM_LOG_INFO("Restored backup: " + backupId);
        } else {
            NLM_LOG_ERROR("Failed to restore backup: " + backupId);
        }
        return success;
    }
    
    bool backupDelete(const std::string& backupId) {
        bool success = backupSystem_->deleteBackup(backupId);
        if (success) {
            NLM_LOG_INFO("Deleted backup: " + backupId);
        } else {
            NLM_LOG_ERROR("Failed to delete backup: " + backupId);
        }
        return success;
    }
    
    void backupList() {
        NLM_LOG_INFO("=== Backups ===");
        std::vector<std::string> backupIds = backupSystem_->getBackupIds();
        NLM_LOG_INFO("Total backups: " + std::to_string(backupIds.size()));
        
        for (const std::string& id : backupIds) {
            BackupMetadata metadata = backupSystem_->getBackupInfo(id);
            NLM_LOG_INFO("  - " + id + " (step: " + std::to_string(metadata.step) + ", size: " + 
                        std::to_string(metadata.size) + " bytes, " + 
                        (metadata.isEncrypted ? "encrypted" : "") + ", " +
                        (metadata.isRemote ? "remote" : "local") + ")");
        }
    }
    
    bool backupValidate(const std::string& backupId) {
        bool valid = backupSystem_->validateBackup(backupId);
        if (valid) {
            NLM_LOG_INFO("Backup validation passed: " + backupId);
        } else {
            NLM_LOG_WARNING("Backup validation failed: " + backupId);
            NLM_LOG_WARNING(backupSystem_->getBackupValidationReport());
        }
        return valid;
    }
    
    bool backupValidateAll() {
        bool valid = backupSystem_->validateAllBackups();
        if (valid) {
            NLM_LOG_INFO("All backups validated successfully");
        } else {
            NLM_LOG_WARNING("Some backups failed validation");
            NLM_LOG_WARNING(backupSystem_->getBackupValidationReport());
        }
        return valid;
    }
    
    void backupConfigureRemote(const std::string& host, int port, const std::string& username) {
        backupSystem_->configureRemoteBackup(host, port, username);
        NLM_LOG_INFO("Configured remote backup: " + username + "@" + host + ":" + std::to_string(port));
    }
    
    bool backupToRemote(const std::string& backupId) {
        bool success = backupSystem_->backupToRemote(backupId);
        if (success) {
            NLM_LOG_INFO("Backed up to remote: " + backupId);
        } else {
            NLM_LOG_ERROR("Failed to backup to remote: " + backupId);
        }
        return success;
    }
    
    bool backupRestoreFromRemote(const std::string& backupId) {
        bool success = backupSystem_->restoreFromRemote(backupId);
        if (success) {
            NLM_LOG_INFO("Restored backup from remote: " + backupId);
        } else {
            NLM_LOG_ERROR("Failed to restore backup from remote: " + backupId);
        }
        return success;
    }
    
    bool backupTestRemoteConnection() {
        bool success = backupSystem_->testRemoteConnection();
        if (success) {
            NLM_LOG_INFO("Remote connection test passed");
        } else {
            NLM_LOG_ERROR("Remote connection test failed");
        }
        return success;
    }
    
    void backupEnableAutomatic(bool enable) {
        backupSystem_->enableAutomaticBackup(enable);
        NLM_LOG_INFO("Automatic backup " + std::string(enable ? "enabled" : "disabled"));
    }
    
    void backupSetSchedule(size_t frequencySeconds) {
        backupSystem_->setBackupSchedule(frequencySeconds);
        NLM_LOG_INFO("Backup schedule set to: " + std::to_string(frequencySeconds) + " seconds");
    }
    
    void backupCleanup(size_t maxBackups) {
        backupSystem_->cleanupOldBackups(maxBackups);
        NLM_LOG_INFO("Cleaned up old backups, keeping " + std::to_string(maxBackups) + " most recent");
    }
    
    void backupCompact() {
        backupSystem_->compactBackups();
        NLM_LOG_INFO("Backup compaction completed");
    }
    
    void backupRebuildIndex() {
        backupSystem_->rebuildBackupIndex();
        NLM_LOG_INFO("Backup index rebuilt");
    }
    
    std::vector<std::string> backupCreateBatch(size_t count, const std::string& baseDescription) {
        return backupSystem_->createBatchBackups(count, baseDescription);
    }
    
    bool backupRestoreBatch(const std::vector<std::string>& backupIds) {
        return backupSystem_->restoreBatchBackups(backupIds);
    }
    
    std::vector<RemoteBackupResult> backupToMultipleRemote(const std::vector<std::string>& hosts) {
        return backupSystem_->backupToMultipleRemote(hosts);
    }
    
    bool backupRestoreFromMultipleRemote(const std::vector<std::string>& hosts, const std::string& targetBackupId) {
        return backupSystem_->restoreFromMultipleRemote(hosts, targetBackupId);
    }
    
    // System Status Commands
    void systemStatus() {
        NLM_LOG_INFO("=== Enhanced Memory Management System Status ===");
        
        // Memory Manager Status
        MemoryStatistics memStats = memoryManager_->getMemoryStatistics();
        NLM_LOG_INFO("Memory Manager: " + std::to_string(memStats.usedMemory) + " / " + 
                    std::to_string(memStats.totalMemory) + " bytes used (" +
                    std::to_string(memStats.efficiency * 100) + "% efficiency)");
        
        // Checkpoint Manager Status
        NLM_LOG_INFO("Checkpoint Manager: " + std::to_string(checkpointManager_->getTotalCheckpoints()) + 
                    " checkpoints, " + std::to_string(checkpointManager_->getTotalStorageSize()) + " bytes storage");
        
        // Memory Monitor Status
        MemoryStatistics monitorStats = memoryMonitor_->getCurrentMemoryStats();
        NLM_LOG_INFO("Memory Monitor: " + std::to_string(monitorStats.fragmentation * 100) + "% fragmentation, " +
                    std::to_string(memoryMonitor_->detectMemoryLeaks()) + " leaks detected");
        
        // Backup System Status
        NLM_LOG_INFO("Backup System: " + std::to_string(backupSystem_->getTotalBackups()) + 
                    " backups, " + std::to_string(backupSystem_->getTotalBackupStorage()) + " bytes storage");
        
        NLM_LOG_INFO("All components initialized: " + std::to_string(memoryManager_ != nullptr && 
                    checkpointManager_ != nullptr && memoryMonitor_ != nullptr && 
                    backupSystem_ != nullptr));
    }
    
    // Integration with Brain
    void integrateWithBrain(std::shared_ptr<Brain> brain) {
        initializeWithBrain(brain);
        NLM_LOG_INFO("Enhanced memory management integrated with brain");
    }

private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<MemoryManager> memoryManager_;
    std::shared_ptr<CheckpointManager> checkpointManager_;
    std::shared_ptr<MemoryUsageMonitor> memoryMonitor_;
    std::shared_ptr<BackupSystem> backupSystem_;
};

} // namespace nlm
