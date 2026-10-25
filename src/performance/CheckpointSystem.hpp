#pragma once

/**
 * Enhanced CheckpointSystem - Improved brain state serialization and checkpointing
 * 
 * Extended features:
 * - Atomic checkpoint saves with transactional integrity
 * - Incremental checkpointing with delta compression
 * - Automatic recovery with rollback capability
 * - Checkpoint versioning with compatibility validation
 * - Parallel checkpoint I/O operations
 * - Memory-efficient serialization with pooling
 * - Detailed error reporting and recovery logs
 * - Integrity verification with checksums and validation
 * - Recovery validation and consistency checking
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <fstream>
#include <functional>
#include <variant>
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <atomic>
#include <shared_mutex>

namespace nlm {

// Checkpoint version for compatibility with rollback support
constexpr uint32_t CHECKPOINT_VERSION_MAJOR = 2;
constexpr uint32_t CHECKPOINT_VERSION_MINOR = 0;
constexpr uint32_t CHECKPOINT_VERSION_PATCH = 0;

// Enhanced checkpoint magic number
constexpr uint64_t CHECKPOINT_MAGIC = 0x4E4C4D434B545430ULL;  // "NLMCKT0"

// Enhanced compression options
enum class CompressionLevel {
    None,
    Fast,
    Balanced,
    Best,
    DeltaOptimized  // For incremental checkpoints
};

// Checkpoint status for recovery tracking
enum class CheckpointStatus {
    NotStarted,
    Writing,
    WritingComplete,
    Verifying,
    Complete,
    Reading,
    Loading,
    LoadComplete,
    Failed,
    Corrupted,
    Recovering
};

// Checkpoint recovery result
struct RecoveryResult {
    bool success;
    std::string message;
    uint64_t checkpointStep;
    uint64_t recoveredStep;
    double recoveryTime;
    std::vector<std::string> warnings;
    
    RecoveryResult() : success(false), checkpointStep(0), recoveredStep(0), recoveryTime(0.0) {}
};

// Incremental checkpoint delta (changes since last checkpoint)
struct CheckpointDelta {
    uint64_t version;
    uint64_t step;
    double simulationTime;
    uint64_t deltaSize;
    
    std::vector<uint8_t> data;  // Compressed delta data
    std::string checksum;
    
    CheckpointDelta() : version(0), step(0), simulationTime(0.0), deltaSize(0) {}
};

// Atomic checkpoint transaction for transactional integrity
class CheckpointTransaction {
public:
    CheckpointTransaction(const std::string& filepath);
    ~CheckpointTransaction();
    
    bool begin();
    bool commit();
    void rollback();
    bool isActive() const { return active_; }
    
    // Transaction methods
    bool writeSection(CheckpointSection sectionType, const void* data, size_t size);
    bool writeMetadata(const CheckpointHeader& header);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool active_;
};

/**
 * Enhanced Checkpoint reader with recovery support
 */
class CheckpointReader {
public:
    CheckpointReader();
    ~CheckpointReader();
    
    // Disable copying
    CheckpointReader(const CheckpointReader&) = delete;
    CheckpointReader& operator=(const CheckpointReader&) = delete;
    
    /**
     * Open a checkpoint file for reading with integrity validation
     * @return true if successful and file is valid
     */
    bool open(const std::string& filepath);
    
    /**
     * Close the checkpoint file
     */
    void close();
    
    /**
     * Check if checkpoint is open and valid
     */
    bool isOpen() const { return stream_.is_open() && header_.validate(); }
    
    /**
     * Get checkpoint status
     */
    CheckpointStatus getStatus() const { return status_; }
    
    /**
     * Get checkpoint header
     */
    const CheckpointHeader& getHeader() const { return header_; }
    
    /**
     * Read checkpoint data with validation
     */
    bool readNeurons(NeuronCheckpointData& data);
    bool readSynapses(SynapseCheckpointData& data);
    
    /**
     * Read raw section data with integrity checking
     */
    std::vector<uint8_t> readSection(CheckpointSection sectionType);
    
    /**
     * Validate checkpoint integrity with detailed error reporting
     */
    bool validate() const;
    
    /**
     * Get validation error message
     */
    const std::string& getError() const { return error_; }
    
    /**
     * Get reader progress (0.0 - 1.0)
     */
    double getProgress() const;
    
    /**
     * Attempt recovery from corrupted checkpoint
     */
    RecoveryResult attemptRecovery(const std::string& backupPath = "");
    
    /**
     * Get available checkpoints in directory
     */
    static std::vector<std::string> findAvailableCheckpoints(const std::string& directory);
    
private:
    // Recovery operations
    RecoveryResult recoverFromHeader();
    RecoveryResult recoverFromData();
    bool verifyChecksum(const std::vector<uint8_t>& data, uint64_t expectedChecksum);
    
    // Helper functions
    bool readSectionHeaders();
    bool validateSectionConsistency();
    
    std::ifstream stream_;
    CheckpointHeader header_;
    std::vector<SectionHeader> sections_;
    std::string error_;
    size_t streamPos_;
    CheckpointStatus status_;
    time_t creationTime_;
};

/**
 * Enhanced Checkpoint writer with transactional support
 */
class CheckpointWriter {
public:
    CheckpointWriter();
    ~CheckpointWriter();
    
    // Disable copying
    CheckpointWriter(const CheckpointWriter&) = delete;
    CheckpointWriter& operator=(const CheckpointWriter&) = delete;
    
    /**
     * Create a new checkpoint file with atomic write support
     * @param filepath Output file path
     * @param compression Compression level
     * @param enableTransaction Enable atomic transaction (default true)
     * @return true if successful
     */
    bool create(const std::string& filepath, CompressionLevel compression = CompressionLevel::Balanced, 
                bool enableTransaction = true);
    
    /**
     * Close the checkpoint file
     */
    void close();
    
    /**
     * Check if checkpoint is open
     */
    bool isOpen() const { return stream_.is_open(); }
    
    /**
     * Set checkpoint metadata
     */
    void setMetadata(uint64_t neuronCount, uint64_t synapseCount, 
                    uint64_t regionCount, uint64_t step, double simTime,
                    const std::string& description = "");
    
    /**
     * Write checkpoint data with integrity checking
     */
    bool writeNeurons(const NeuronCheckpointData& data);
    bool writeSynapses(const SynapseCheckpointData& data);
    
    /**
     * Write raw section data
     */
    bool writeSection(CheckpointSection sectionType, const void* data, size_t size);
    
    /**
     * Finalize and write footer with transaction support
     */
    bool finalize();
    
    /**
     * Abort checkpoint (cleanup partial file)
     */
    void abort();
    
    /**
     * Get total bytes written
     */
    uint64_t getBytesWritten() const { return bytesWritten_; }
    
    /**
     * Get checkpoint file size
     */
    uint64_t getFileSize() const;
    
    /**
     * Set corruption detection callback
     */
    void setCorruptionCallback(std::function<void(const std::string&)> callback) {
        corruptionCallback_ = callback;
    }
    
private:
    // Transaction support
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();
    
    // Write helpers
    bool writeHeader();
    bool writeSectionHeader(const SectionHeader& header);
    bool padToAlignment(size_t alignment);
    bool computeAndWriteChecksum();
    bool verifyIntegrity();
    
    std::ofstream stream_;
    std::string filepath_;
    CompressionLevel compression_;
    CheckpointHeader header_;
    std::vector<SectionHeader> sections_;
    uint64_t bytesWritten_;
    std::vector<uint8_t> compressBuffer_;
    bool enableTransaction_;
    std::function<void(const std::string&)> corruptionCallback_;
    CheckpointStatus status_;
};

/**
 * Enhanced Checkpoint manager with recovery and rollback support
 */
class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager();
    
    /**
     * Configure checkpoint manager with enhanced options
     */
    void configure(const std::string& checkpointDir, 
                   uint64_t saveIntervalSteps = 10000,
                   size_t maxCheckpoints = 10,
                   bool compress = true,
                   bool enableTransactions = true,
                   bool enableIncremental = false,
                   uint64_t incrementalIntervalSteps = 1000);
    
    /**
     * Set checkpoint data providers
     */
    void setNeuronProvider(std::function<bool(NeuronCheckpointData&)> provider) {
        neuronProvider_ = provider;
    }
    
    void setSynapseProvider(std::function<bool(SynapseCheckpointData&)> provider) {
        synapseProvider_ = provider;
    }
    
    /**
     * Update checkpoint state with recovery support
     */
    bool update(uint64_t currentStep, double currentTime, uint64_t simulationStep = 0);
    
    /**
     * Save checkpoint immediately with transaction support
     */
    bool saveImmediately(const std::string& name = "");
    
    /**
     * Load checkpoint with recovery support
     */
    bool load(const std::string& name);
    
    /**
     * Load checkpoint with automatic recovery
     */
    RecoveryResult loadWithRecovery(const std::string& name, const std::string& backupPath = "");
    
    /**
     * List available checkpoints
     */
    struct CheckpointInfo {
        std::string name;
        uint64_t step;
        double simulationTime;
        uint64_t neuronCount;
        uint64_t synapseCount;
        uint64_t fileSize;
        time_t timestamp;
        CheckpointStatus status;
        std::string description;
        uint64_t version;
        bool isIncremental;
        uint64_t baseStep;
        
        CheckpointInfo() : step(0), simulationTime(0.0), neuronCount(0), 
                          synapseCount(0), fileSize(0), timestamp(0), 
                          status(CheckpointStatus::NotStarted), version(0),
                          isIncremental(false), baseStep(0) {}
    };
    
    std::vector<CheckpointInfo> listCheckpoints() const;
    
    /**
     * Delete old checkpoints (keep most recent N) with validation
     */
    void pruneOldCheckpoints(size_t keepCount);
    
    /**
     * Get checkpoint directory
     */
    const std::string& getCheckpointDir() const { return checkpointDir_; }
    
    /**
     * Get last checkpoint path
     */
    std::string getLastCheckpointPath() const;
    
    /**
     * Check if checkpoint should be saved
     */
    bool shouldSave(uint64_t currentStep) const;
    
    /**
     * Validate checkpoint integrity
     */
    bool validateCheckpoint(const std::string& checkpointPath) const;
    
    /**
     * Get checkpoint recovery status
     */
    CheckpointStatus getRecoveryStatus() const { return recoveryStatus_; }
    
    /**
     * Set recovery callback
     */
    void setRecoveryCallback(std::function<void(const RecoveryResult&)> callback) {
        recoveryCallback_ = callback;
    }
    
private:
    // Helper functions
    bool createCheckpointDirectory();
    std::string generateCheckpointName(uint64_t step, bool isIncremental = false) const;
    bool cleanupOldCheckpoints();
    bool saveIncrementalCheckpoint(uint64_t currentStep, double currentTime);
    bool restoreFromIncremental(const CheckpointDelta& delta);
    
    std::string checkpointDir_;
    uint64_t saveIntervalSteps_;
    size_t maxCheckpoints_;
    bool compress_;
    bool enableTransactions_;
    bool enableIncremental_;
    uint64_t incrementalIntervalSteps_;
    uint64_t lastSaveStep_;
    std::string lastCheckpointPath_;
    
    std::function<bool(NeuronCheckpointData&)> neuronProvider_;
    std::function<bool(SynapseCheckpointData&)> synapseProvider_;
    std::function<void(const RecoveryResult&)> recoveryCallback_;
    
    mutable std::shared_mutex checkpointMutex_;
    CheckpointStatus recoveryStatus_;
    uint64_t currentVersion_;
    
    // Recovery tracking
    struct RecoveryState {
        uint64_t attemptedStep = 0;
        time_t attemptTime = 0;
        std::string lastError;
        bool recoveryAttempted = false;
    } recoveryState_;
};

// Enhanced checksum calculator with multiple algorithms
class EnhancedChecksumCalculator {
public:
    using uint128_t = unsigned __int128;
    
    static uint64_t crc64(const void* data, size_t size);
    static uint32_t crc32(const void* data, size_t size);
    static uint64_t fletcher64(const uint64_t* data, size_t count);
    static uint128_t xxhash128(const void* data, size_t size);
    static uint64_t cityHash64(const void* data, size_t size);
    
    // Combine checksums for incremental updates
    static uint64_t combine(uint64_t a, uint64_t b, uint64_t c = 0);
    
    // Multi-pass checksum for critical data
    static std::vector<uint64_t> computeMultiPassChecksum(const void* data, size_t size);
    
    // Validate checksum with fallback
    static bool validateChecksum(const void* data, size_t size, uint64_t expectedChecksum,
                                 const std::string& algorithm = "crc64");
    
    // Get checksum string representation
    static std::string checksumToString(uint64_t checksum, const std::string& algorithm = "crc64");
    
private:
    static const uint64_t CRC64_TABLE[256];
    static const uint32_t CRC32_TABLE[256];
};

} // namespace nlm