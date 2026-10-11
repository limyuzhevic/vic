// CheckpointManager.hpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <set>
#include <mutex>
#include <atomic>
#include <thread>
#include <future>
#include <chrono>
#include <filesystem>
#include <unordered_map>
#include <deque>
#include "CheckpointSystem.hpp"

namespace nlm {

/**
 * Advanced checkpoint manager with comprehensive checkpointing capabilities
 * 
 * Features:
 * - Incremental checkpointing (save only changed data)
 * - Differential checkpoints (store only differences)
 * - Parallel checkpoint operations
 * - Automatic checkpoint cleanup and lifecycle management
 * - Checkpoint policies (time-based, size-based, event-based)
 * - Rich metadata for checkpoint identification and management
 * - Fast checkpoint indexing and lookup
 * - Remote storage support
 * - Checkpoint validation and integrity verification
 * - Memory-efficient checkpointing (zero-copy optimizations)
 * - Version control and branching
 */

struct CheckpointMetadata {
    std::string name;                    // Human-readable name
    uint64_t step;                      // Simulation step
    double simulationTime;               // Simulation time
    uint64_t neuronCount;               // Number of neurons
    uint64_t synapseCount;              // Number of synapses
    uint64_t regionCount;               // Number of regions
    uint64_t fileSize;                  // File size in bytes
    time_t timestamp;                   // Creation timestamp
    std::string author;                 // Who created this checkpoint
    std::string description;            // Description/notes
    std::string version;                // Software version
    std::vector<std::string> tags;      // Metadata tags for filtering
    std::map<std::string, std::string> customMetadata; // Custom key-value metadata
    uint64_t parentId;                  // ID of parent checkpoint (for branching)
    bool compressed{false};             // Was checkpoint compressed
    uint32_t compressionAlgorithm;      // Compression algorithm used
    uint32_t checksum;                  // Data checksum
    double memoryUsage;                 // Peak memory usage during checkpoint
    double checkpointTime;              // Time taken to create checkpoint
    uint32_t validationCount{0};        // Number of times validated
    bool isValid{true};                 // Current validation status
    std::string storageLocation;       // Where checkpoint is stored
    
    CheckpointMetadata() : step(0), simulationTime(0.0), neuronCount(0),
                          synapseCount(0), regionCount(0), fileSize(0),
                          timestamp(0), parentId(0), checksum(0),
                          memoryUsage(0.0), checkpointTime(0.0) {}
    
    std::string getId() const {
        // Generate unique ID from timestamp and step
        std::stringstream ss;
        ss << "ckpt_" << timestamp << "_" << step;
        return ss.str();
    }
    
    bool isIncremental(uint64_t baseStep) const {
        // Check if this checkpoint is incremental relative to base step
        return step > baseStep;
    }
    
    double getCheckpointAge() const {
        // Get age in seconds since checkpoint was created
        time_t now = std::time(nullptr);
        return static_cast<double>(now - timestamp);
    }
};

struct CheckpointPolicy {
    enum class PolicyType {
        TimeBased,      // Create checkpoints at regular time intervals
        StepBased,      // Create checkpoints at regular step intervals
        SizeBased,      // Create checkpoints when size reaches threshold
        EventBased,     // Create checkpoints on specific events
        Hybrid         // Combination of multiple strategies
    };
    
    PolicyType type{PolicyType::StepBased};
    uint64_t interval{10000};           // Time or step interval
    uint64_t sizeThreshold{100 * 1024 * 1024}; // 100MB size threshold
    std::set<std::string> triggerEvents; // Events that trigger checkpoints
    bool autoCleanup{true};              // Automatically clean old checkpoints
    size_t maxCheckpoints{10};           // Maximum checkpoints to keep
    uint64_t minCheckpointAge{3600};     // Minimum age before cleanup (seconds)
    
    CheckpointPolicy() = default;
    CheckpointPolicy(PolicyType t, uint64_t i) : type(t), interval(i) {}
    
    bool shouldCreateCheckpoint(
        uint64_t currentStep,
        double currentTime,
        uint64_t currentSize,
        const std::string& event
    ) const {
        switch (type) {
            case PolicyType::TimeBased:
                return static_cast<uint64_t>(currentTime) >= interval;
            case PolicyType::StepBased:
                return currentStep >= interval;
            case PolicyType::SizeBased:
                return currentSize >= sizeThreshold;
            case PolicyType::EventBased:
                return triggerEvents.find(event) != triggerEvents.end();
            case PolicyType::Hybrid:
                return (currentStep >= interval) || (currentSize >= sizeThreshold) ||
                       (triggerEvents.find(event) != triggerEvents.end());
            default:
                return false;
        }
    }
};

class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager();
    
    // Disable copying
    CheckpointManager(const CheckpointManager&) = delete;
    CheckpointManager& operator=(const CheckpointManager&) = delete;
    
    /**
     * Configure checkpoint manager
     * @param checkpointDir Directory to store checkpoints
     * @param policy Checkpoint policy configuration
     * @param compression Whether to compress checkpoints
     * @param maxConcurrent Maximum concurrent checkpoint operations
     */
    void configure(
        const std::string& checkpointDir,
        const CheckpointPolicy& policy = CheckpointPolicy(),
        bool compress = true,
        size_t maxConcurrent = 2
    );
    
    /**
     * Set checkpoint data providers
     * @param neuronProvider Function to provide neuron data
     * @param synapseProvider Function to provide synapse data
     * @param plasticityProvider Function to provide plasticity data
     * @param memoryProvider Function to provide memory data
     * @param neuromodulationProvider Function to provide neuromodulation data
     */
    void setDataProviders(
        std::function<bool(const NeuronCheckpointData&)> neuronProvider,
        std::function<bool(const SynapseCheckpointData&)> synapseProvider,
        std::function<bool(const std::vector<uint8_t>&)> plasticityProvider,
        std::function<bool(const std::vector<uint8_t>&)> memoryProvider,
        std::function<bool(const std::vector<uint8_t>&)> neuromodulationProvider
    );
    
    /**
     * Set serialization functions
     * @param brainSerializer Function to serialize brain state
     * @param memorySerializer Function to serialize memory state
     */
    void setSerializers(
        std::function<std::vector<uint8_t>(const Brain&)> brainSerializer,
        std::function<std::vector<uint8_t>(const class Memory&)> memorySerializer
    );
    
    /**
     * Update checkpoint state (call each simulation step)
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     * @param currentSize Current memory usage
     * @param event Optional event that triggered this update
     * @return true if checkpoint was created
     */
    bool update(uint64_t currentStep, double currentTime, 
                uint64_t currentSize = 0, const std::string& event = "");
    
    /**
     * Save checkpoint immediately
     * @param name Optional checkpoint name
     * @param compressionAlgorithm Compression algorithm to use
     * @param compressionLevel Compression level
     * @return Checkpoint ID if successful, empty string otherwise
     */
    std::string saveImmediately(
        const std::string& name = "",
        CompressionAlgorithm compressionAlgorithm = CompressionAlgorithm::Zstd,
        int compressionLevel = 3
    );
    
    /**
     * Load checkpoint
     * @param checkpointId ID of checkpoint to load
     * @return true if successful
     */
    bool load(const std::string& checkpointId);
    
    /**
     * Load latest checkpoint
     * @return true if successful
     */
    bool loadLatest();
    
    /**
     * Load specific checkpoint by step
     * @param step Simulation step
     * @return true if successful
     */
    bool loadByStep(uint64_t step);
    
    /**
     * List available checkpoints
     * @param limit Maximum number of checkpoints to return
     * @param includeInvalid Whether to include invalid checkpoints
     * @return Vector of checkpoint metadata
     */
    std::vector<CheckpointMetadata> listCheckpoints(
        size_t limit = 20,
        bool includeInvalid = false
    ) const;
    
    /**
     * Get checkpoint by ID
     * @param checkpointId Checkpoint ID
     * @return Checkpoint metadata, null if not found
     */
    std::optional<CheckpointMetadata> getCheckpoint(const std::string& checkpointId) const;
    
    /**
     * Get checkpoint by step
     * @param step Simulation step
     * @return Checkpoint metadata, null if not found
     */
    std::optional<CheckpointMetadata> getCheckpointByStep(uint64_t step) const;
    
    /**
     * Delete checkpoint
     * @param checkpointId ID of checkpoint to delete
     * @return true if successful
     */
    bool deleteCheckpoint(const std::string& checkpointId);
    
    /**
     * Delete multiple checkpoints
     * @param checkpointIds Vector of checkpoint IDs to delete
     * @return Number of checkpoints successfully deleted
     */
    size_t deleteCheckpoints(const std::vector<std::string>& checkpointIds);
    
    /**
     * Delete old checkpoints (keep most recent N)
     * @param keepCount Number of checkpoints to keep
     */
    void pruneOldCheckpoints(size_t keepCount);
    
    /**
     * Validate checkpoint integrity
     * @param checkpointId ID of checkpoint to validate
     * @return true if valid
     */
    bool validateCheckpoint(const std::string& checkpointId);
    
    /**
     * Validate all checkpoints
     * @return Number of valid checkpoints
     */
    size_t validateAllCheckpoints();
    
    /**
     * Get checkpoint directory
     */
    const std::string& getCheckpointDir() const { return checkpointDir_; }
    
    /**
     * Get last checkpoint path
     */
    std::string getLastCheckpointPath() const;
    
    /**
     * Get current checkpoint ID
     */
    std::string getCurrentCheckpointId() const { return currentCheckpointId_; }
    
    /**
     * Get checkpoint statistics
     */
    struct CheckpointStats {
        size_t totalCount{0};             // Total checkpoints
        size_t validCount{0};             // Valid checkpoints
        size_t invalidCount{0};           // Invalid checkpoints
        uint64_t totalSize{0};            // Total size in bytes
        uint64_t avgSize{0};              // Average size
        uint64_t maxSize{0};              // Maximum size
        double totalCompressionRatio{0.0}; // Overall compression ratio
        time_t oldestTimestamp{0};        // Timestamp of oldest checkpoint
        time_t newestTimestamp{0};        // Timestamp of newest checkpoint
    };
    
    CheckpointStats getStats() const;
    
    /**
     * Enable/disable auto-cleanup
     */
    void setAutoCleanup(bool enabled) { autoCleanup_ = enabled; }
    
    /**
     * Set checkpoint policy
     */
    void setPolicy(const CheckpointPolicy& policy) { policy_ = policy; }
    
    /**
     * Get checkpoint policy
     */
    const CheckpointPolicy& getPolicy() const { return policy_; }
    
    /**
     * Enable/disable compression
     */
    void setCompression(bool enabled) { compress_ = enabled; }
    
    /**
     * Set compression algorithm
     */
    void setCompressionAlgorithm(CompressionAlgorithm algo) { 
        compressionAlgorithm_ = algo; 
    }
    
    /**
     * Set compression level
     */
    void setCompressionLevel(int level) { compressionLevel_ = level; }
    
    /**
     * Get compression algorithm
     */
    CompressionAlgorithm getCompressionAlgorithm() const { 
        return compressionAlgorithm_; 
    }
    
    /**
     * Get compression level
     */
    int getCompressionLevel() const { return compressionLevel_; }
    
    /**
     * Enable/disable parallel checkpointing
     */
    void setParallelEnabled(bool enabled) { parallelEnabled_ = enabled; }
    
    /**
     * Get number of concurrent checkpoint operations
     */
    size_t getMaxConcurrent() const { return maxConcurrent_; }
    
    /**
     * Set maximum concurrent operations
     */
    void setMaxConcurrent(size_t max) { maxConcurrent_ = max; }
    
    /**
     * Enable/disable incremental checkpointing
     */
    void setIncrementalEnabled(bool enabled) { incrementalEnabled_ = enabled; }
    
    /**
     * Enable/disable differential checkpointing
     */
    void setDifferentialEnabled(bool enabled) { differentialEnabled_ = enabled; }
    
    /**
     * Set storage backend for remote checkpoints
     */
    void setStorageBackend(class RemoteStorage* storage) { storageBackend_ = storage; }
    
    /**
     * Get active checkpoints (currently being written)
     */
    std::vector<std::string> getActiveCheckpoints() const;
    
private:
    // Core configuration
    std::string checkpointDir_;
    CheckpointPolicy policy_;
    bool compress_;
    CompressionAlgorithm compressionAlgorithm_;
    int compressionLevel_;
    bool parallelEnabled_;
    size_t maxConcurrent_;
    bool incrementalEnabled_;
    bool differentialEnabled_;
    bool autoCleanup_;
    
    // State tracking
    std::string currentCheckpointId_;
    uint64_t lastSaveStep_{0};
    double lastSaveTime_{0.0};
    uint64_t currentSize_{0};
    
    // Data providers
    std::function<bool(const NeuronCheckpointData&)> neuronProvider_;
    std::function<bool(const SynapseCheckpointData&)> synapseProvider_;
    std::function<bool(const std::vector<uint8_t>&)> plasticityProvider_;
    std::function<bool(const std::vector<uint8_t>&)> memoryProvider_;
    std::function<bool(const std::vector<uint8_t>&)> neuromodulationProvider_;
    
    // Serializers
    std::function<std::vector<uint8_t>(const class Brain&)> brainSerializer_;
    std::function<std::vector<uint8_t>(const class Memory&)> memorySerializer_;
    
    // Storage backend
    class RemoteStorage* storageBackend_{nullptr};
    
    // Checkpoint data
    std::map<std::string, CheckpointMetadata> checkpoints_;
    std::map<std::string, class CheckpointIndexer*> indexers_;
    
    // Concurrent operations
    std::mutex operationsMutex_;
    std::atomic<size_t> activeOperations_{0};
    std::vector<std::future<bool>> pendingOperations_;
    std::deque<std::string> operationQueue_;
    
    // Background threads
    std::thread cleanupThread_;
    std::atomic<bool> shutdownRequested_{false};
    std::condition_variable cleanupCondition_;
    
    // Performance optimization
    std::mutex dirtyDataMutex_;
    std::vector<uint8_t> dirtyNeuronData_;
    std::vector<uint8_t> dirtySynapseData_;
    uint64_t lastIncrementalBaseStep_{0};
    
    // Statistics
    mutable std::mutex statsMutex_;
    size_t checkpointsCreated_{0};
    size_t checkpointsLoaded_{0};
    size_t checkpointsDeleted_{0};
    uint64_t totalBytesSaved_{0};
    uint64_t totalBytesLoaded_{0};
    
    // Cleanup thread function
    void cleanupThreadFunc();
    
    // Core operations
    bool createCheckpoint(const std::string& name = "");
    bool writeCheckpoint(const CheckpointMetadata& metadata, const std::vector<uint8_t>& data);
    bool readCheckpoint(const CheckpointMetadata& metadata, std::vector<uint8_t>& data);
    
    // Incremental/differential checkpointing
    std::vector<uint8_t> createIncrementalData(
        uint64_t baseStep, const std::vector<uint8_t>& baseData);
    std::vector<uint8_t> createDifferentialData(
        uint64_t baseStep, const std::vector<uint8_t>& baseData);
    
    // Checkpoint metadata management
    void updateCheckpointMetadata(const std::string& checkpointId);
    void cleanupOldCheckpoints();
    
    // File operations
    std::string getCheckpointPath(const std::string& checkpointId) const;
    bool ensureDirectoryExists(const std::string& path) const;
    
    // Statistics
    void incrementCheckpointsCreated();
    void incrementCheckpointsLoaded();
    void incrementCheckpointsDeleted(uint64_t bytes);
    void addToTotalBytesSaved(uint64_t bytes);
    void addToTotalBytesLoaded(uint64_t bytes);
    
    // Validation
    bool validateCheckpointData(const std::vector<uint8_t>& data) const;
    bool validateCheckpointIntegrity(const std::string& checkpointId) const;
    
    // Serialization helpers
    std::vector<uint8_t> serializeBrainData(const class Brain& brain) const;
    std::vector<uint8_t> serializeMemoryData(const class Memory& memory) const;
    
    // Index management
    void updateIndex(const CheckpointMetadata& metadata);
    void rebuildIndex();
    
    friend class CheckpointIndexer; // Allow indexer to access private methods
};

/**
 * Checkpoint indexer for fast lookup and search
 */
class CheckpointIndexer {
public:
    CheckpointIndexer(CheckpointManager& manager);
    ~CheckpointIndexer();
    
    // Disable copying
    CheckpointIndexer(const CheckpointIndexer&) = delete;
    CheckpointIndexer& operator=(const CheckpointIndexer&) = delete;
    
    /**
     * Index checkpoint for fast lookup
     * @param metadata Checkpoint metadata
     */
    void indexCheckpoint(const CheckpointMetadata& metadata);
    
    /**
     * Remove checkpoint from index
     * @param checkpointId Checkpoint ID
     */
    void removeCheckpoint(const std::string& checkpointId);
    
    /**
     * Find checkpoints by tag
     * @param tag Tag to search for
     * @param limit Maximum results
     * @return Vector of checkpoint IDs
     */
    std::vector<std::string> findByTag(
        const std::string& tag,
        size_t limit = 10
    ) const;
    
    /**
     * Find checkpoints by author
     * @param author Author to search for
     * @param limit Maximum results
     * @return Vector of checkpoint IDs
     */
    std::vector<std::string> findByAuthor(
        const std::string& author,
        size_t limit = 10
    ) const;
    
    /**
     * Find checkpoints by step range
     * @param minStep Minimum step
     * @param maxStep Maximum step
     * @return Vector of checkpoint metadata
     */
    std::vector<CheckpointMetadata> findByStepRange(
        uint64_t minStep,
        uint64_t maxStep
    ) const;
    
    /**
     * Find checkpoints by time range
     * @param minTime Minimum timestamp
     * @param maxTime Maximum timestamp
     * @return Vector of checkpoint metadata
     */
    std::vector<CheckpointMetadata> findByTimeRange(
        time_t minTime,
        time_t maxTime
    ) const;
    
    /**
     * Get checkpoint by ID (fast lookup)
     * @param checkpointId Checkpoint ID
     * @return Checkpoint metadata
     */
    std::optional<CheckpointMetadata> getCheckpoint(const std::string& checkpointId) const;
    
    /**
     * Get latest checkpoints
     * @param limit Maximum number of checkpoints
     * @return Vector of checkpoint metadata
     */
    std::vector<CheckpointMetadata> getLatest(size_t limit = 10) const;
    
    /**
     * Get checkpoint count
     */
    size_t getCount() const { return checkpointIndex_.size(); }
    
    /**
     * Rebuild index from disk
     */
    void rebuildIndex();
    
    /**
     * Export index to file
     */
    bool exportIndex(const std::string& filepath) const;
    
    /**
     * Import index from file
     */
    bool importIndex(const std::string& filepath);
    
    /**
     * Clear index
     */
    void clear();
    
private:
    CheckpointManager& manager_;
    std::map<std::string, CheckpointMetadata> checkpointIndex_;
    std::unordered_map<std::string, std::set<std::string>> tagIndex_;
    std::unordered_map<std::string, std::set<std::string>> authorIndex_;
    std::map<uint64_t, std::string> stepIndex_;
    std::map<time_t, std::string> timeIndex_;
    mutable std::mutex indexMutex_;
    
    // Index checkpoint by various criteria
    void indexByTag(const CheckpointMetadata& metadata);
    void indexByAuthor(const CheckpointMetadata& metadata);
    void indexByStep(const CheckpointMetadata& metadata);
    void indexByTime(const CheckpointMetadata& metadata);
};