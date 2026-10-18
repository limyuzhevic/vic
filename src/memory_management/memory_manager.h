#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <chrono>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <lru_cache>
#include <numeric>
#include <algorithm>
#include <random>
#include <cmath>

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Advanced Memory Item with rich metadata
struct AdvancedMemoryItem {
    NeuronId neuronId;
    float activationValue;
    SimulationStep timestamp;
    std::vector<float> pattern;
    std::vector<NeuronId> associatedNeurons;
    std::vector<float> associatedWeights;
    std::string metadata;
    float relevanceScore;
    float decayRate;
    SimulationStep lastAccessTime;
    
    AdvancedMemoryItem(NeuronId id, float value, const std::vector<float>& pat = {})
        : neuronId(id), activationValue(value), timestamp(getCurrentStep()),
          pattern(pat), relevanceScore(1.0f), decayRate(0.01f),
          lastAccessTime(getCurrentStep()) {}
};

// Episodic Memory with advanced encoding/decoding
struct AdvancedEpisodicMemoryItem {
    SimulationStep timestamp;
    std::vector<float> sensoryPattern;
    std::vector<float> motorPattern;
    float reward;
    float energy;
    std::string location;
    ActionType action;
    std::vector<NeuronId> activeNeurons;
    std::vector<float> neuronActivations;
    float noveltyScore;
    float stressLevel;
    std::vector<float> predictionError;
    std::unordered_map<std::string, float> contextualFactors;
    
    AdvancedEpisodicMemoryItem() : timestamp(0), reward(0), energy(0), 
                                   noveltyScore(0), stressLevel(0) {}
};

// Pattern for associative memory
struct AdvancedPattern {
    std::vector<float> vector;
    std::vector<NeuronId> neurons;
    float magnitude;
    std::unordered_map<std::string, float> features;
    
    AdvancedPattern(const std::vector<float>& vec) : vector(vec), magnitude(0.0f) {
        magnitude = std::sqrt(std::accumulate(vec.begin(), vec.end(), 0.0f, 
                                            [](float sum, float v) { return sum + v * v; }));
        if (magnitude > 0.0f) {
            for (float& v : vector) v /= magnitude;
        }
    }
};

// Memory item with advanced features
struct AdvancedAssociativeItem {
    AdvancedPattern pattern1;
    AdvancedPattern pattern2;
    float strength;
    SimulationStep lastUpdated;
    float decayRate;
    std::unordered_map<std::string, float> associationFeatures;
    
    AdvancedAssociativeItem(const AdvancedPattern& p1, const AdvancedPattern& p2, float str = 1.0f)
        : pattern1(p1), pattern2(p2), strength(str), lastUpdated(getCurrentStep()),
          decayRate(0.01f) {}
};

// Checkpoint metadata
struct CheckpointMetadata {
    std::string id;
    SimulationStep step;
    std::string description;
    SimulationStep creationTime;
    SimulationStep lastAccessTime;
    size_t size;
    bool isCompressed;
    bool isEncrypted;
    std::string compressionAlgorithm;
    float integrityHash;
    std::unordered_map<std::string, std::string> metadataTags;
    
    CheckpointMetadata(const std::string& id, SimulationStep step, const std::string& desc = "")
        : id(id), step(step), description(desc), creationTime(getCurrentStep()),
          lastAccessTime(getCurrentStep()), size(0), isCompressed(false), 
          isEncrypted(false), integrityHash(0.0f) {}
};

// Memory Statistics
struct MemoryStatistics {
    size_t totalMemory;
    size_t usedMemory;
    size_t peakMemory;
    size_t allocatedObjects;
    size_t deallocatedObjects;
    double efficiency;
    double fragmentation;
    std::chrono::system_clock::time_point timestamp;
    
    MemoryStatistics() : totalMemory(0), usedMemory(0), peakMemory(0), 
                         allocatedObjects(0), deallocatedObjects(0),
                         efficiency(0.0), fragmentation(0.0) {}
};

// Backup metadata
struct BackupMetadata {
    std::string id;
    SimulationStep step;
    std::string description;
    SimulationStep creationTime;
    SimulationStep lastAccessTime;
    size_t size;
    bool isEncrypted;
    float integrityHash;
    std::string remoteLocation;
    bool isRemote;
    
    BackupMetadata(const std::string& id, SimulationStep step, const std::string& desc = "")
        : id(id), step(step), description(desc), creationTime(getCurrentStep()),
          lastAccessTime(getCurrentStep()), size(0), isEncrypted(false), 
          integrityHash(0.0f), isRemote(false) {}
};

// Forward declarations for cyclic dependencies
class CheckpointManager;
class MemoryUsageMonitor;
class BackupSystem;

class MemoryManager {
public:
    MemoryManager();
    ~MemoryManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Working Memory Management
    void setWorkingMemoryCapacity(size_t capacity);
    size_t getWorkingMemoryCapacity() const;
    void storeToWorkingMemory(NeuronId neuronId, float activation, float strength = 1.0f);
    float retrieveFromWorkingMemory(NeuronId neuronId) const;
    void clearWorkingMemory();
    float getWorkingMemoryLoad() const;
    
    // Episodic Memory Management
    void storeToEpisodicMemory(const AdvancedEpisodicMemoryItem& item);
    const AdvancedEpisodicMemoryItem* retrieveFromEpisodicMemory(SimulationStep id) const;
    std::vector<SimulationStep> getRecentEpisodes(size_t count) const;
    void consolidateMemory();
    void setEpisodicMemoryCapacity(size_t capacity);
    
    // Associative Memory Management
    void storeAssociation(const AdvancedPattern& pattern1, const AdvancedPattern& pattern2, float strength);
    std::vector<float> recallPattern(const AdvancedPattern& cue) const;
    void clearAssociativeMemory();
    void setAssociationDecayRate(float rate);
    
    // Memory Consolidation
    void consolidateMemories();
    void consolidateImportantMemories();
    void consolidateUnusedMemories(double threshold);
    
    // Memory Forgetting
    void forgetMemory(NeuronId neuronId);
    void forgetUnusedMemories(double threshold);
    void setMemoryDecayRate(float rate);
    
    // Memory Interference Management
    void reduceMemoryInterference();
    void enhanceMemoryRecall();
    float calculateMemoryInterference(NeuronId neuronId) const;
    
    // Integration with other systems
    void setCheckpointManager(CheckpointManager* manager) { checkpointManager_ = manager; }
    void setMemoryUsageMonitor(MemoryUsageMonitor* monitor) { memoryMonitor_ = monitor; }
    void setBackupSystem(BackupSystem* backup) { backupSystem_ = backup; }
    
    // Performance and optimization
    void optimizeMemoryUsage();
    void preallocateMemory(size_t size);
    void cleanupUnusedMemory();
    
    // Memory pressure management
    void handleMemoryPressure();
    float getMemoryPressureLevel() const;
    
    // Statistics and monitoring
    MemoryStatistics getMemoryStatistics() const;
    size_t getTotalMemoryUsage() const;
    size_t getPeakMemoryUsage() const;\n    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    CheckpointManager* checkpointManager_;
    MemoryUsageMonitor* memoryMonitor_;
    BackupSystem* backupSystem_;
    mutable std::shared_mutex memoryMutex_;
};

class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager();
    
    // Checkpoint Configuration
    void setCheckpointDirectory(const std::string& directory);
    void setCheckpointFrequency(size_t frequency);
    void setCheckpointCompression(bool enabled);
    void setCheckpointEncryption(bool enabled);
    void setMaxCheckpointSize(size_t size);
    
    // Checkpoint Operations
    std::string createCheckpoint(const std::string& description = "");
    bool restoreCheckpoint(const std::string& checkpointId);
    bool deleteCheckpoint(const std::string& checkpointId);
    std::vector<std::string> getCheckpointIds() const;
    CheckpointMetadata getCheckpointInfo(const std::string& checkpointId) const;
    
    // Incremental Checkpointing
    std::string createIncrementalCheckpoint(const std::string& baseCheckpointId);
    bool applyIncrementalCheckpoint(const std::string& checkpointId);
    
    // Differential Checkpointing
    std::string createDifferentialCheckpoint(const std::string& baseCheckpointId);
    bool applyDifferentialCheckpoint(const std::string& checkpointId);
    
    // Checkpoint Validation
    bool validateCheckpoint(const std::string& checkpointId) const;
    bool validateAllCheckpoints() const;
    std::string getCheckpointValidationReport() const;
    
    // Parallel Operations
    void enableParallelCheckpoints(bool enable);
    bool areParallelCheckpointsEnabled() const;
    
    // Compression and Encryption
    void setCompressionAlgorithm(const std::string& algorithm);
    void setEncryptionKey(const std::string& key);
    
    // Memory Integration
    void setMemoryManager(MemoryManager* manager) { memoryManager_ = manager; }
    
    // Statistics and Monitoring
    size_t getTotalCheckpoints() const;
    size_t getActiveCheckpoints() const;
    size_t getTotalStorageSize() const;
    std::vector<CheckpointMetadata> getAllCheckpointsInfo() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    MemoryManager* memoryManager_;
    mutable std::shared_mutex checkpointMutex_;
};

class MemoryUsageMonitor {
public:
    MemoryUsageMonitor();
    ~MemoryUsageMonitor();
    
    // Memory Tracking
    void startTracking();
    void stopTracking();
    void trackMemoryAllocation(size_t size, const std::string& source);
    void trackMemoryDeallocation(size_t size, const std::string& source);
    
    // Memory Analysis
    MemoryStatistics getCurrentMemoryStats() const;
    std::vector<MemoryStatistics> getMemoryUsageHistory() const;
    size_t detectMemoryLeaks() const;
    double calculateMemoryFragmentation() const;
    
    // Memory Optimization
    void optimizeMemoryUsage();
    std::vector<std::string> getOptimizationRecommendations() const;
    void applyMemoryOptimization(const std::string& recommendation);
    
    // Leak Detection
    void enableLeakDetection(bool enable);
    bool isLeakDetectionEnabled() const;
    std::vector<std::string> getDetectedLeaks() const;
    
    // Integration
    void setMemoryManager(MemoryManager* manager) { memoryManager_ = manager; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    MemoryManager* memoryManager_;
    mutable std::shared_mutex monitorMutex_;
};

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
    
    // Remote Backup
    void configureRemoteBackup(const std::string& host, int port, const std::string& username);
    bool backupToRemote(const std::string& backupId);
    bool restoreFromRemote(const std::string& backupId);
    
    // Automatic Backup
    void enableAutomaticBackup(bool enable);
    bool isAutomaticBackupEnabled() const;
    void setBackupSchedule(size_t frequencySeconds);
    
    // Integration
    void setMemoryManager(MemoryManager* manager) { memoryManager_ = manager; }
    void setCheckpointManager(CheckpointManager* manager) { checkpointManager_ = manager; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    MemoryManager* memoryManager_;
    CheckpointManager* checkpointManager_;
    mutable std::shared_mutex backupMutex_;
};

} // namespace nlm
