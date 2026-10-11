// CompressionEngine.hpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <functional>

namespace nlm {

/**
 * Advanced checkpoint compression engine with multiple algorithms
 * 
 * Features:
 * - Multiple compression algorithms (gzip, zstd, lz4)
 * - Configurable compression levels
 * - Parallel compression for large checkpoints
 * - Delta compression for incremental checkpoints
 * - Compression statistics and optimization
 */

enum class CompressionAlgorithm {
    None,
    Gzip,
    Zstd,
    Lz4,
    Zlib,
    Brotli
};

enum class CompressionMode {
    SingleThread,
    MultiThread,
    Pipeline
};

struct CompressionStats {
    uint64_t originalSize{0};
    uint64_t compressedSize{0};
    double compressionRatio{0.0};
    uint64_t compressionTime{0}; // nanoseconds
    CompressionAlgorithm algorithm{CompressionAlgorithm::None};
    int compressionLevel{0};
    
    CompressionStats() = default;
    CompressionStats(uint64_t orig, uint64_t comp, double ratio, 
                    uint64_t time, CompressionAlgorithm algo, int level)
        : originalSize(orig), compressedSize(comp), compressionRatio(ratio),
          compressionTime(time), algorithm(algo), compressionLevel(level) {}
};

class CompressionEngine {
public:
    CompressionEngine();
    ~CompressionEngine();
    
    // Disable copying
    CompressionEngine(const CompressionEngine&) = delete;
    CompressionEngine& operator=(const CompressionEngine&) = delete;
    
    /**
     * Compress data using specified algorithm
     * @param data Input data to compress
     * @param algorithm Compression algorithm to use
     * @param level Compression level (algorithm-specific)
     * @param mode Compression mode
     * @return Compressed data
     */
    static std::vector<uint8_t> compress(
        const std::vector<uint8_t>& data,
        CompressionAlgorithm algorithm = CompressionAlgorithm::Zstd,
        int level = 3,
        CompressionMode mode = CompressionMode::MultiThread
    );
    
    /**
     * Decompress data using specified algorithm
     * @param data Compressed data
     * @param originalSize Original size before compression
     * @param algorithm Compression algorithm used
     * @return Decompressed data
     */
    static std::vector<uint8_t> decompress(
        const std::vector<uint8_t>& data,
        uint64_t originalSize,
        CompressionAlgorithm algorithm
    );
    
    /**
     * Compress with delta encoding (for incremental checkpoints)
     * @param currentData Current checkpoint data
     * @param baseData Base checkpoint data
     * @param algorithm Compression algorithm
     * @return Delta compressed data
     */
    static std::vector<uint8_t> compressDelta(
        const std::vector<uint8_t>& currentData,
        const std::vector<uint8_t>& baseData,
        CompressionAlgorithm algorithm = CompressionAlgorithm::Zstd
    );
    
    /**
     * Apply delta to base data to reconstruct current data
     * @param baseData Base checkpoint data
     * @param deltaData Delta compressed data
     * @param algorithm Compression algorithm
     * @return Reconstructed data
     */
    static std::vector<uint8_t> applyDelta(
        const std::vector<uint8_t>& baseData,
        const std::vector<uint8_t>& deltaData,
        CompressionAlgorithm algorithm
    );
    
    /**
     * Get compression statistics for data
     * @param data Input data
     * @param algorithm Compression algorithm
     * @param level Compression level
     * @return Compression statistics
     */
    static CompressionStats analyzeCompression(
        const std::vector<uint8_t>& data,
        CompressionAlgorithm algorithm,
        int level = 3
    );
    
    /**
     * Get best compression algorithm for data type
     * @param data Input data
     * @param size Size of data in bytes
     * @return Recommended algorithm
     */
    static CompressionAlgorithm recommendAlgorithm(
        const std::vector<uint8_t>& data,
        size_t size
    );
    
    /**
     * Get available compression algorithms
     * @return Vector of supported algorithms
     */
    static std::vector<CompressionAlgorithm> getAvailableAlgorithms();
    
    /**
     * Check if algorithm is available
     * @param algorithm Algorithm to check
     * @return true if algorithm is available
     */
    static bool isAlgorithmAvailable(CompressionAlgorithm algorithm);
    
    /**
     * Set compression thread pool size
     * @param threads Number of threads for parallel compression
     */
    static void setThreadPoolSize(size_t threads);
    
    /**
     * Get optimal compression settings for data characteristics
     * @param data Data characteristics
     * @param targetRatio Target compression ratio (0.0-1.0)
     * @return Optimal compression settings
     */
    static std::pair<CompressionAlgorithm, int> optimizeSettings(
        const std::vector<uint8_t>& data,
        double targetRatio = 0.5
    );
    
    /**
     * Batch compress multiple data chunks
     * @param dataChunks Vector of data chunks to compress
     * @param algorithm Compression algorithm
     * @param level Compression level
     * @return Vector of compression statistics
     */
    static std::vector<CompressionStats> batchCompress(
        const std::vector<std::vector<uint8_t>>& dataChunks,
        CompressionAlgorithm algorithm = CompressionAlgorithm::Zstd,
        int level = 3
    );
    
    /**
     * Get compression library version
     * @return Version string
     */
    static std::string getVersion();
    
private:
    static size_t threadPoolSize_;
    
    // Platform-specific compression implementations
    static std::vector<uint8_t> compressGzip(
        const std::vector<uint8_t>& data, int level);
    static std::vector<uint8_t> decompressGzip(
        const std::vector<uint8_t>& data, uint64_t originalSize);
    
    static std::vector<uint8_t> compressZstd(
        const std::vector<uint8_t>& data, int level);
    static std::vector<uint8_t> decompressZstd(
        const std::vector<uint8_t>& data, uint64_t originalSize);
    
    static std::vector<uint8_t> compressLz4(
        const std::vector<uint8_t>& data, int level);
    static std::vector<uint8_t> decompressLz4(
        const std::vector<uint8_t>& data, uint64_t originalSize);
    
    static std::vector<uint8_t> compressZlib(
        const std::vector<uint8_t>& data, int level);
    static std::vector<uint8_t> decompressZlib(
        const std::vector<uint8_t>& data, uint64_t originalSize);
    
    // Delta compression implementation
    static std::vector<uint8_t> createDelta(
        const std::vector<uint8_t>& current,
        const std::vector<uint8_t>& base);
    static std::vector<uint8_t> applyDeltaToBuffer(
        std::vector<uint8_t>& buffer,
        const std::vector<uint8_t>& delta);
    
    // Compression utilities
    static uint64_t crc64(const void* data, size_t size);
    static bool isDataCompressible(const std::vector<uint8_t>& data);
};