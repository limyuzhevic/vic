#pragma once

/**
 * CheckpointOptimizer - Differential and incremental checkpointing
 * 
 * Provides advanced checkpointing optimizations including:
 * - Differential checkpointing (only save changed data)
 * - Incremental backup (chain of checkpoints)
 * - Checkpoint compression (zstd, lz4, gzip)
 * - Distributed checkpoint storage
 * - Checkpoint validation and integrity checks
 * 
 * Features:
 * - Binary delta compression for minimal checkpoint size
 * - Checkpoint versioning and rollback support
 * - Memory-efficient chunked writing
 * - Network-aware checkpoint distribution
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <mutex>
#include <zlib.h>
#include <lz4.h>
#include <thread>
#include <future>

namespace nlm {

// Compression algorithms
enum class CompressionAlgorithm {
    None,
    Zlib,
    Zstd,
    LZ4,
    Brotli
};

// Checkpoint types
enum class CheckpointType {
    Full,        // Complete checkpoint
    Incremental, // Only changes since last checkpoint
    Differential // Differences from a base checkpoint
};

// Checkpoint metadata
struct CheckpointMetadata {
    uint64_t version;                    // Checkpoint format version
    uint64_t sequence;                   // Sequence number
    uint64_t timestamp;                  // Creation timestamp
    uint64_t size;                       // Uncompressed size
    uint64_t compressedSize;             // Compressed size
    double compressionRatio;             // size / compressedSize
    uint64_t checksum;                   // Data checksum
    uint64_t parentSequence;             // For incremental/diff
    std::string algorithm;               // Compression algorithm used
    std::string description;             // Human-readable description
    std::vector<std::string> tags;      // Tags for filtering (e.g., "stable", "checkpointing")
};

// Delta record (for incremental/differential checkpoints)
struct DeltaRecord {
    uint64_t offset;                    // Offset in checkpoint file
    uint64_t size;                      // Size of delta data
    uint64_t checksum;                  // Delta checksum
    uint8_t compression;                // Compression type for this delta
    std::vector<uint8_t> data;          // Delta payload (compressed)
};

// Chunk information for distributed storage
struct ChunkInfo {
    uint64_t chunkId;                   // Unique chunk ID
    uint64_t offset;                    // Offset in original file
    uint64_t size;                      // Uncompressed chunk size
    uint64_t compressedSize;            // Compressed size
    std::string algorithm;              // Compression used
    std::vector<uint8_t> data;          // Compressed chunk data
    std::string nodeId;                 // Source node ID (for distributed)
    uint64_t replicationCount;          // Number of replicas
    std::vector<std::string> replicas;  // Replica node IDs
};

/**
 * Differential checkpoint calculator
 */
class DifferentialCalculator {
public:
    DifferentialCalculator() = default;
    ~DifferentialCalculator() = default;
    
    /**
     * Calculate delta between two checkpoint snapshots
     * @param oldData Previous checkpoint data
     * @param newData Current checkpoint data
     * @return Vector of delta records
     */
    std::vector<DeltaRecord> calculateDelta(const std::vector<uint8_t>& oldData,
                                           const std::vector<uint8_t>& newData);
    
    /**
     * Apply delta to base checkpoint
     * @param baseData Base checkpoint data
     * @param deltaRecords Delta records to apply
     * @return Combined checkpoint data
     */
    std::vector<uint8_t> applyDelta(const std::vector<uint8_t>& baseData,
                                   const std::vector<DeltaRecord>& deltaRecords);
    
    /**
     * Calculate checksum of data
     */
    static uint64_t calculateChecksum(const void* data, size_t size);
};

/**
 * Compression utilities for checkpoints
 */
class CheckpointCompressor {
public:
    CheckpointCompressor() = default;
    ~CheckpointCompressor() = default;
    
    /**
     * Compress data with specified algorithm
     */
    std::vector<uint8_t> compress(const void* data, size_t size, 
                                 CompressionAlgorithm algorithm,
                                 int compressionLevel = 6);
    
    /**
     * Decompress data
     */
    std::vector<uint8_t> decompress(const void* data, size_t size,
                                   CompressionAlgorithm algorithm);
    
    /**
     * Get compression ratio
     */
    static double getCompressionRatio(size_t originalSize, size_t compressedSize) {
        return compressedSize > 0 ? static_cast<double>(originalSize) / compressedSize : 1.0;
    }
    
    /**
     * Choose best compression algorithm for data
     */
    static CompressionAlgorithm chooseAlgorithm(const void* data, size_t size) {
        if (size < 1024) return CompressionAlgorithm::None;
        // Simple heuristic: choose based on size
        return CompressionAlgorithm::Zstd;
    }
};

/**
 * Chunked checkpoint writer for large checkpoints
 */
class ChunkedCheckpointWriter {
public:
    struct Config {
        size_t chunkSize;               // Size of each chunk (1MB-1GB)
        size_t maxConcurrentWrites;     // Max parallel chunk writes
        std::string tempDir;            // Temporary directory for chunks
        bool verifyWrites;              // Verify chunks after writing
    };
    
    ChunkedCheckpointWriter(const Config& config);
    ~ChunkedCheckpointWriter();
    
    /**
     * Write checkpoint data to chunks
     */
    bool writeChunks(const std::vector<uint8_t>& data, 
                     const std::string& basePath,
                     const std::string& name);
    
    /**
     * Read checkpoint chunks
     */
    std::vector<uint8_t> readChunks(const std::string& basePath,
                                    const std::string& name,
                                    uint64_t offset,
                                    uint64_t size);
    
    /**
     * List available checkpoints
     */
    std::vector<std::string> listCheckpoints(const std::string& basePath) const;
    
    /**
     * Clean up chunks
     */
    void cleanup(const std::string& basePath, const std::string& name = "");

private:
    Config config_;
    std::mutex writeMutex_;
};

/**
 * Distributed checkpoint storage
 */
class DistributedCheckpointStorage {
public:
    struct NodeInfo {
        std::string nodeId;
        std::string host;               // Network host
        uint16_t port;                  // Network port
        uint32_t capacity;              // Storage capacity (MB)
        uint32_t used;                  // Used storage (MB)
        double load;                    // System load (0-1)
        bool available;                 // Node availability
    };
    
    DistributedCheckpointStorage(const std::vector<NodeInfo>& nodes);
    ~DistributedCheckpointStorage();
    
    /**
     * Store checkpoint across multiple nodes
     */
    bool store(const std::vector<uint8_t>& data,
               const std::string& name,
               const std::vector<std::string>& preferredNodes = {});
    
    /**
     * Retrieve checkpoint from storage
     */
    std::optional<std::vector<uint8_t>> retrieve(const std::string& name,
                                                 const std::string& nodeId = "");
    
    /**
     * Get chunk locations
     */
    std::vector<std::string> getChunkLocations(const std::string& name,
                                               const std::string& nodeId = "");
    
    /**
     * Monitor storage health
     */
    std::vector<NodeInfo> monitorStorage() const;
    
    /**
     * Rebalance data across nodes
     */
    void rebalance(const std::string& name);

private:
    std::vector<NodeInfo> nodes_;
    mutable std::mutex nodesMutex_;
    std::unordered_map<std::string, std::vector<ChunkInfo>> checkpointIndex_;
};

/**
 * Advanced Checkpoint Optimizer - Main class
 */
class CheckpointOptimizer {
public:
    struct Config {
        CheckpointType type;                     // Checkpoint type
        CompressionAlgorithm compression;        // Compression algorithm
        int compressionLevel;                    // Compression level (0-9)
        uint64_t maxCheckpointSize;              // Max checkpoint size (MB)
        uint64_t incrementalInterval;            // Incremental checkpoint interval
        uint64_t differentialInterval;           // Differential checkpoint interval
        size_t chunkSize;                        // Chunk size for distributed storage
        size_t maxConcurrentOperations;          // Max parallel operations
        std::string checkpointDir;               // Base directory for checkpoints
        std::string compressionAlgorithm;        // Algorithm name (zstd, lz4, etc.)
        bool verifyIntegrity;                    // Verify checkpoint integrity
        bool enableCompression;                  // Enable compression
        bool enableDistributed;                  // Enable distributed storage
    };
    
    CheckpointOptimizer(const Config& config);
    ~CheckpointOptimizer();
    
    /**
     * Configure checkpoint data providers
     */
    void setNeuronProvider(std::function<std::vector<uint8_t>(void)> provider) {
        neuronProvider_ = std::move(provider);
    }
    
    void setSynapseProvider(std::function<std::vector<uint8_t>(void)> provider) {
        synapseProvider_ = std::move(provider);
    }
    
    void setPlasticityProvider(std::function<std::vector<uint8_t>(void)> provider) {
        plasticityProvider_ = std::move(provider);
    }
    
    /**
     * Create a checkpoint
     */
    bool createCheckpoint(const std::string& name = "", uint64_t sequence = 0);
    
    /**
     * Load a checkpoint
     */
    bool loadCheckpoint(const std::string& name);
    
    /**
     * Get checkpoint metadata
     */
    CheckpointMetadata getMetadata(const std::string& name) const;
    
    /**
     * List available checkpoints
     */
    std::vector<std::string> listCheckpoints() const;
    
    /**
     * Delete checkpoints
     */
    bool deleteCheckpoint(const std::string& name);
    
    /**
     * Clean up old checkpoints (keep N most recent)
     */
    void pruneOldCheckpoints(size_t keepCount);
    
    /**
     * Calculate storage usage
     */
    uint64_t calculateStorageUsage(const std::string& basePath) const;
    
    /**
     * Compact checkpoints (merge incremental into full)
     */
    bool compactCheckpoints(const std::string& basePath,
                           const std::string& outputName,
                           double compactionThreshold = 0.5);

private:
    Config config_;
    DifferentialCalculator differentialCalculator_;
    CheckpointCompressor compressor_;
    ChunkedCheckpointWriter chunkWriter_;
    DistributedCheckpointStorage distStorage_;
    
    std::function<std::vector<uint8_t>(void)> neuronProvider_;
    std::function<std::vector<uint8_t>(void)> synapseProvider_;
    std::function<std::vector<uint8_t>(void)> plasticityProvider_;
    
    std::unordered_map<std::string, CheckpointMetadata> checkpointMetadata_;
    mutable std::mutex metadataMutex_;
    
    uint64_t lastIncrementalStep_;
    uint64_t lastDifferentialStep_;
    
    // Helper methods
    std::vector<uint8_t> collectCheckpointData() const;
    bool writeCheckpoint(const std::vector<uint8_t>& data, 
                        const std::string& name,
                        const CheckpointMetadata& metadata);
    bool validateCheckpoint(const std::string& name) const;
};

} // namespace nlm