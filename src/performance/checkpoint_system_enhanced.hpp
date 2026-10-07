#pragma once

/**
 * Enhanced Checkpoint System with Streaming I/O and Advanced Compression
 * 
 * Provides high-performance checkpointing with:
 * - Streaming I/O for large checkpoint files
 * - Advanced compression algorithms
 * - Incremental checkpointing
 * - Memory-mapped file support
 * - Parallel checkpoint I/O
 * - Checksum and integrity validation
 * - Network transfer support
 * - Checkpoint diffing
 * 
 * Performance features:
 * - Zero-copy operations where possible
 * - Async I/O for better throughput
 * - Adaptive compression based on data characteristics
 * - Background checkpoint maintenance
 * - Checksummer for data integrity
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <fstream>
#include <filesystem>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <variant>
#include <zlib.h>
#include <lz4.h>
#include <zstd.h>
#include <immintrin.h>

// Async I/O support
#ifdef _WIN32
    #include <windows.h>
    #include <mswsock.h>
#else
    #include <aio.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <cstring>
#endif

namespace nlm {

/**
 * Advanced compression algorithms for checkpoint data
 */
enum class CompressionAlgorithm {
    None,
    Zlib,
    LZ4,
    Zstd,
    Snappy,
    Brotli,
    LZMA,
    Deflate
};

/**
 * Compression level and strategy configuration
 */
struct CompressionConfig {
    CompressionAlgorithm algorithm;
    int level;
    size_t chunkSize;
    bool enableAsyncCompression;
    size_t maxMemoryUsage;
    double targetCompressionRatio;
    
    CompressionConfig() : algorithm(CompressionAlgorithm::Zlib),
                         level(6),
                         chunkSize(65536),
                         enableAsyncCompression(true),
                         maxMemoryUsage(1024 * 1024 * 1024),  // 1GB
                         targetCompressionRatio(0.8) {}  // 80% compression target
};

/**
 * Streaming checkpoint data processor
 */
class StreamingCheckpointProcessor {
public:
    struct StreamConfig {
        size_t bufferSize;
        bool enableAsyncIO;
        size_t readAheadSize;
        size_t writeBehindSize;
        std::chrono::milliseconds timeout;
        
        StreamConfig() : bufferSize(64 * 1024),  // 64KB buffer
                        enableAsyncIO(true),
                        readAheadSize(1024 * 1024),  // 1MB read ahead
                        writeBehindSize(256 * 1024),  // 256KB write behind
                        timeout(std::chrono::milliseconds(5000)) {}
    };
    
public:
    StreamingCheckpointProcessor(const StreamConfig& config = StreamConfig());
    ~StreamingCheckpointProcessor();
    
    // Disable copying
    StreamingCheckpointProcessor(const StreamingCheckpointProcessor&) = delete;
    StreamingCheckpointProcessor& operator=(const StreamingCheckpointProcessor&) = delete;
    
    /**
     * Open a checkpoint file for streaming read
     */
    bool openForRead(const std::string& filepath);
    
    /**
     * Open a checkpoint file for streaming write
     */
    bool openForWrite(const std::string& filepath, const CompressionConfig& compressionConfig);
    
    /**
     * Read data from checkpoint stream
     */
    bool readData(void* buffer, size_t size);
    
    /**
     * Read data with checksum validation
     */
    bool readDataWithChecksum(void* buffer, size_t size, uint64_t checksum);
    
    /**
     * Write data to checkpoint stream
     */
    bool writeData(const void* buffer, size_t size);
    
    /**
     * Write data with checksum
     */
    bool writeDataWithChecksum(const void* buffer, size_t size);
    
    /**
     * Get current position in stream
     */
    uint64_t getPosition() const;
    
    /**
     * Get stream size
     */
    uint64_t getStreamSize() const;
    
    /**
     * Seek to specific position
     */
    bool seekTo(uint64_t position);
    
    /**
     * Get stream status
     */
    struct StreamStatus {
        bool isOpen;
        uint64_t currentPosition;
        uint64_t streamSize;
        size_t readBufferUsage;
        size_t writeBufferUsage;
        uint64_t totalBytesRead;
        uint64_t totalBytesWritten;
        bool isCompressed;
        double compressionRatio;
        
        StreamStatus() : isOpen(false), currentPosition(0), streamSize(0),
                       readBufferUsage(0), writeBufferUsage(0),
                       totalBytesRead(0), totalBytesWritten(0),
                       isCompressed(false), compressionRatio(0.0) {}
    };
    
    StreamStatus getStatus() const;
    
    /**
     * Close the stream
     */
    void close();
    
    /**
     * Flush pending data
     */
    bool flush();
    
private:
    StreamConfig config_;
    
    // File handling
    std::string filepath_;
    std::ifstream readStream_;
    std::ofstream writeStream_;
    
    // Async I/O support
    #ifdef _WIN32
        HANDLE hFile_;
        OVERLAPPED overlappedRead_;
        OVERLAPPED overlappedWrite_;
    #else
        int fileDescriptor_;
        aiocb readAIO_;
        aiocb writeAIO_;
    #endif
    
    // Buffer management
    std::vector<char> readBuffer_;
    std::vector<char> writeBuffer_;
    std::deque<std::vector<char>> pendingWrites_;
    
    // Compression support
    CompressionConfig compressionConfig_;
    std::unique_ptr<z_stream> zstream_;
    std::vector<char> compressionBuffer_;
    
    // Statistics
    uint64_t currentPosition_;
    uint64_t streamSize_;
    uint64_t totalBytesRead_;
    uint64_t totalBytesWritten_;
    
    // Thread safety
    mutable std::mutex ioMutex_;
    std::condition_variable readAvailable_;
    std::condition_variable writeAvailable_;
}; 

/**
 * Advanced compression engine for checkpoint data
 */
class CheckpointCompressionEngine {
public:
    CheckpointCompressionEngine(const CompressionConfig& config = CompressionConfig());
    ~CheckpointCompressionEngine();
    
    // Disable copying
    CheckpointCompressionEngine(const CheckpointCompressionEngine&) = delete;
    CheckpointCompressionEngine& operator=(const CheckpointCompressionEngine&) = delete;
    
    /**
     * Compress data using the configured algorithm
     */
    std::vector<char> compress(const void* data, size_t size);
    
    /**
     * Decompress data
     */
    bool decompress(const char* compressedData, size_t compressedSize, void* output, size_t outputSize);
    
    /**
     * Get compression ratio estimate for given data
     */
    double estimateCompressionRatio(const void* data, size_t size) const;
    
    /**
     * Get current compression statistics
     */
    struct CompressionStats {
        uint64_t totalDataSize;
        uint64_t totalCompressedSize;
        double averageCompressionRatio;
        uint64_t compressionOperations;
        uint64_t decompressionOperations;
        uint64_t totalCompressionTimeNs;
        uint64_t totalDecompressionTimeNs;
        
        CompressionStats() : totalDataSize(0), totalCompressedSize(0),
                           averageCompressionRatio(0.0), compressionOperations(0),
                           decompressionOperations(0), totalCompressionTimeNs(0),
                           totalDecompressionTimeNs(0) {}
    };
    
    CompressionStats getStats() const { return stats_; }
    
    /**
     * Reset statistics
     */
    void resetStats() {
        stats_ = CompressionStats();
    }
    
private:
    CompressionConfig config_;
    CompressionStats stats_;
    
    // Compression algorithm implementations
    std::vector<char> compressWithZlib(const void* data, size_t size);
    std::vector<char> compressWithLZ4(const void* data, size_t size);
    std::vector<char> compressWithZstd(const void* data, size_t size);
    
    bool decompressWithZlib(const char* data, size_t size, void* output, size_t outputSize);
    bool decompressWithLZ4(const char* data, size_t size, void* output, size_t outputSize);
    bool decompressWithZstd(const char* data, size_t size, void* output, size_t outputSize);
    
    // SIMD-optimized compression functions
    #ifdef __AVX2__
    static void SIMDCompressBlock(const uint8_t* input, uint8_t* output, size_t size);
    static void SIMDDecompressBlock(const uint8_t* input, uint8_t* output, size_t size);
    #endif
};

/**
 * Incremental checkpoint system with delta tracking
 */
class IncrementalCheckpointSystem {
public:
    struct CheckpointMetadata {
        uint64_t checkpointId;
        uint64_t timestamp;
        uint64_t baseCheckpointId;
        uint64_t dataSize;
        uint64_t compressedSize;
        double compressionRatio;
        uint64_t checksum;
        std::string description;
        
        CheckpointMetadata() : checkpointId(0), timestamp(0), baseCheckpointId(0),
                              dataSize(0), compressedSize(0), compressionRatio(0.0),
                              checksum(0) {}
    };
    
    struct CheckpointDiff {
        std::vector<uint64_t> modifiedOffsets;
        std::vector<size_t> modifiedSizes;
        std::vector<uint8_t> modifiedData;
        uint64_t checksum;
        
        CheckpointDiff() : checksum(0) {}
    };
    
public:
    IncrementalCheckpointSystem(const std::string& checkpointDir, 
                                size_t maxCheckpoints = 10,
                                size_t maxCheckpointSize = 1024 * 1024 * 1024);  // 1GB
    ~IncrementalCheckpointSystem();
    
    // Disable copying
    IncrementalCheckpointSystem(const IncrementalCheckpointSystem&) = delete;
    IncrementalCheckpointSystem& operator=(const IncrementalCheckpointSystem&) = delete;
    
    /**
     * Create a new incremental checkpoint
     */
    uint64_t createCheckpoint(const std::string& description = "");
    
    /**
     * Create checkpoint from a base checkpoint
     */
    uint64_t createCheckpoint(uint64_t baseCheckpointId, const std::string& description = "");
    
    /**
     * Save checkpoint data with incremental updates
     */
    bool saveCheckpoint(uint64_t checkpointId, const void* data, size_t size,
                       const CheckpointDiff* diff = nullptr);
    
    /**
     * Load checkpoint data
     */
    bool loadCheckpoint(uint64_t checkpointId, void* output, size_t& sizeOut);
    
    /**
     * Get list of available checkpoints
     */
    std::vector<CheckpointMetadata> listCheckpoints() const;
    
    /**
     * Delete old checkpoints (keep most recent N)
     */
    void pruneCheckpoints(size_t keepCount);
    
    /**
     * Get checkpoint by description
     */
    std::optional<CheckpointMetadata> getCheckpointByDescription(const std::string& description) const;
    
    /**
     * Get memory-mapped access to checkpoint
     */
    bool mmapCheckpoint(uint64_t checkpointId, void** mappedMemory, uint64_t& mappedSize);
    
    /**
     * Get checkpoint integrity report
     */
    struct IntegrityReport {
        uint64_t totalCheckpoints;
        uint64_t validCheckpoints;
        uint64_t corruptedCheckpoints;
        uint64_t checksumErrors;
        std::vector<uint64_t> corruptedCheckpointIds;
        double integrityScore;
        
        IntegrityReport() : totalCheckpoints(0), validCheckpoints(0),
                          corruptedCheckpoints(0), checksumErrors(0),
                          integrityScore(0.0) {}
    };
    
    IntegrityReport checkIntegrity() const;
    
private:
    std::string checkpointDir_;
    size_t maxCheckpoints_;
    size_t maxCheckpointSize_;
    
    // Checkpoint management
    std::vector<CheckpointMetadata> checkpoints_;
    mutable std::mutex checkpointsMutex_;
    
    // Checkpoint file paths
    std::string getCheckpointPath(uint64_t checkpointId) const;
    std::string getCheckpointIndexPath(uint64_t checkpointId) const;
    
    // Checksum calculation
    uint64_t calculateChecksum(const void* data, size_t size);
    
    // Diff computation
    CheckpointDiff computeDiff(uint64_t checkpointId, const void* newData, size_t newSize);
    
    // Compression for diffs
    std::vector<uint8_t> compressDiff(const CheckpointDiff& diff);
    bool decompressDiff(const std::vector<uint8_t>& compressedDiff, CheckpointDiff& diff);
    
    // Cleanup
    void cleanupOldCheckpoints();
};

/**
 * Parallel checkpoint I/O system
 */
class ParallelCheckpointIO {
public:
    struct IOConfig {
        size_t numIOThreads;
        size_t queueSize;
        size_t blockSize;
        bool enableCaching;
        size_t cacheSize;
        std::chrono::milliseconds timeout;
        
        IOConfig() : numIOThreads(std::thread::hardware_concurrency()),
                     queueSize(1024),
                     blockSize(64 * 1024),  // 64KB blocks
                     enableCaching(true),
                     cacheSize(64 * 1024 * 1024),  // 64MB cache
                     timeout(std::chrono::milliseconds(5000)) {}
    };
    
public:
    ParallelCheckpointIO(const IOConfig& config = IOConfig());
    ~ParallelCheckpointIO();
    
    // Disable copying
    ParallelCheckpointIO(const ParallelCheckpointIO&) = delete;
    ParallelCheckpointIO& operator=(const ParallelCheckpointIO&) = delete;
    
    /**
     * Submit read operation
     */
    std::future<bool> submitRead(const std::string& filepath, void* buffer, size_t size);
    
    /**
     * Submit write operation
     */
    std::future<bool> submitWrite(const std::string& filepath, const void* buffer, size_t size);
    
    /**
     * Wait for all pending operations
     */
    void waitForCompletion();
    
    /**
     * Get I/O statistics
     */
    struct IOStats {
        uint64_t totalOperations;
        uint64_t completedOperations;
        uint64_t failedOperations;
        uint64_t totalBytesRead;
        uint64_t totalBytesWritten;
        double readThroughputMBps;
        double writeThroughputMBps;
        double averageLatencyMs;
        
        IOStats() : totalOperations(0), completedOperations(0),
                   failedOperations(0), totalBytesRead(0),
                   totalBytesWritten(0), readThroughputMBps(0.0),
                   writeThroughputMBps(0.0), averageLatencyMs(0.0) {}
    };
    
    IOStats getStats() const;
    
    /**
     * Get current queue size
     */
    size_t getQueueSize() const;
    
    /**
     * Shutdown the I/O system
     */
    void shutdown();
    
private:
    IOConfig config_;
    
    // Thread pool
    std::vector<std::thread> ioThreads_;
    std::queue<std::function<bool()>> taskQueue_;
    mutable std::mutex queueMutex_;
    std::condition_variable taskAvailable_;
    std::condition_variable tasksCompleted_;
    
    // Statistics
    mutable std::atomic<uint64_t> totalOperations_{0};
    mutable std::atomic<uint64_t> completedOperations_{0};
    mutable std::atomic<uint64_t> failedOperations_{0};
    mutable std::atomic<uint64_t> totalBytesRead_{0};
    mutable std::atomic<uint64_t> totalBytesWritten_{0};
    mutable std::atomic<uint64_t> totalLatency_{0};
    
    // Shutdown flag
    std::atomic<bool> shutdown_{false};
    
    // Task processing
    void processTasks();
    
    // File I/O operations
    static bool readFile(const std::string& filepath, void* buffer, size_t size);
    static bool writeFile(const std::string& filepath, const void* buffer, size_t size);
};

/**
 * High-level checkpoint manager
 * Integrates all checkpointing components for end-to-end checkpoint management
 */
class CheckpointManager {
public:
    struct ManagerConfig {
        std::string checkpointDir;
        size_t maxCheckpoints;
        bool enableIncremental;
        bool enableCompression;
        bool enableParallelIO;
        bool enableStreaming;
        std::chrono::milliseconds checkpointInterval;
        size_t memoryBudgetMB;
        
        ManagerConfig() : checkpointDir("./checkpoints"),
                         maxCheckpoints(10),
                         enableIncremental(true),
                         enableCompression(true),
                         enableParallelIO(true),
                         enableStreaming(true),
                         checkpointInterval(std::chrono::minutes(5)),
                         memoryBudgetMB(1024) {}  // 1GB memory budget
    };
    
public:
    CheckpointManager(const ManagerConfig& config = ManagerConfig());
    ~CheckpointManager();
    
    // Disable copying
    CheckpointManager(const CheckpointManager&) = delete;
    CheckpointManager& operator=(const CheckpointManager&) = delete;
    
    /**
     * Create a new checkpoint
     */
    uint64_t createCheckpoint(const std::string& description = "");
    
    /**
     * Save checkpoint with all necessary data
     */
    bool saveCheckpoint(uint64_t checkpointId, const std::string& description = "");
    
    /**
     * Load a checkpoint
     */
    bool loadCheckpoint(uint64_t checkpointId);
    
    /**
     * Delete a checkpoint
     */
    bool deleteCheckpoint(uint64_t checkpointId);
    
    /**
     * List all checkpoints
     */
    std::vector<IncrementalCheckpointSystem::CheckpointMetadata> listCheckpoints() const;
    
    /**
     * Get current checkpoint statistics
     */
    struct CheckpointStats {
        uint64_t totalCheckpoints;
        uint64_t totalMemoryUsage;
        uint64_t totalCompressedSize;
        double averageCompressionRatio;
        uint64_t lastCheckpointId;
        std::chrono::steady_clock::time_point lastCheckpointTime;
        
        CheckpointStats() : totalCheckpoints(0), totalMemoryUsage(0),
                           totalCompressedSize(0), averageCompressionRatio(0.0),
                           lastCheckpointId(0), lastCheckpointTime() {}
    };
    
    CheckpointStats getStats() const;
    
    /**
     * Perform checkpoint maintenance (cleanup, optimization)
     */
    void performMaintenance();
    
    /**
     * Configure checkpointing behavior
     */
    void configure(const ManagerConfig& config);
    
    /**
     * Get the underlying incremental checkpoint system
     */
    const IncrementalCheckpointSystem& getIncrementalSystem() const { return incrementalSystem_; }
    
private:
    ManagerConfig config_;
    
    // Components
    IncrementalCheckpointSystem incrementalSystem_;
    std::optional<ParallelCheckpointIO> parallelIO_;
    std::optional<StreamingCheckpointProcessor> streamingProcessor_;
    std::optional<CheckpointCompressionEngine> compressionEngine_;
    
    // Statistics
    mutable std::mutex statsMutex_;
    CheckpointStats stats_;
    
    // Checkpoint lifecycle
    uint64_t generateCheckpointId();
    bool ensureComponentsInitialized();
};

} // namespace nlm
