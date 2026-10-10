// Checkpoint compressor - checkpoint compression
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include "CheckpointSystem.hpp"

namespace nlm {

// Compression result
struct CompressionResult {
    bool success;
    std::string message;
    uint64_t originalSize;
    uint64_t compressedSize;
    double compressionRatio;
    std::string algorithm;
    time_t timestamp;
    
    CompressionResult() : success(false), originalSize(0), compressedSize(0),
                         compressionRatio(0.0), timestamp(0) {}
};

// Compression statistics
struct CompressionStats {
    uint64_t totalOperations;
    uint64_t successfulOperations;
    uint64_t failedOperations;
    uint64_t totalCompressedSize;
    double averageCompressionRatio;
    uint32_t errorCount;
    
    CompressionStats() : totalOperations(0), successfulOperations(0), failedOperations(0),
                        totalCompressedSize(0), averageCompressionRatio(0.0), errorCount(0) {}
};

// Main checkpoint compressor
class CheckpointCompressor {
public:
    CheckpointCompressor();
    ~CheckpointCompressor();
    
    // Compression operations
    bool compress(const void* data, uint64_t dataSize, std::vector<uint8_t>& compressedData,
                  CompressionLevel level = CompressionLevel::Balanced);
    bool compressFile(const std::string& inputPath, const std::string& outputPath,
                     CompressionLevel level = CompressionLevel::Balanced);
    
    // Decompression operations
    bool decompress(const void* compressedData, uint64_t compressedSize,
                    std::vector<uint8_t>& decompressedData);
    bool decompressFile(const std::string& inputPath, const std::string& outputPath);
    
    // Compression management
    bool setCompressionAlgorithm(CompressionAlgorithm algorithm);
    CompressionAlgorithm getCompressionAlgorithm() const { return algorithm_; }
    bool setCompressionLevel(CompressionLevel level) { level_ = level; return true; }
    CompressionLevel getCompressionLevel() const { return level_; }
    
    // Statistics and monitoring
    CompressionStats getStats() const { return stats_; }
    
    bool isHealthy() const { return errorCount_ == 0; }
    
    // Configuration
    void configure(const std::string& tempDir, const std::shared_ptr<Config>& config);
    
    // Event handling
    using CompressionEventCallback = std::function<void(const std::string&, const CompressionResult&)>;
    void setCompressionEventCallback(CompressionEventCallback callback) { 
        compressionEventCallback_ = callback; 
    }
    
    // Thread safety
    void lock() const { mutex_.lock(); }
    void unlock() const { mutex_.unlock(); }
    bool tryLock() const { return mutex_.try_lock(); }

private:
    bool initialize();
    void cleanup();
    
    CompressionResult executeCompressionOperation(const std::string& operation,
                                                  const std::string& sourcePath,
                                                  const std::string& targetPath,
                                                  CompressionLevel level);
    
    void updateStats(const CompressionResult& result);
    void logEvent(const std::string& operation, const std::string& sourcePath,
                  const CompressionResult& result);
    
    // Compression implementations
    bool compressWithZlib(const void* data, uint64_t dataSize, std::vector<uint8_t>& compressedData,
                         CompressionLevel level);
    bool compressWithLz4(const void* data, uint64_t dataSize, std::vector<uint8_t>& compressedData,
                        CompressionLevel level);
    bool compressWithSnappy(const void* data, uint64_t dataSize, std::vector<uint8_t>& compressedData,
                           CompressionLevel level);
    bool compressWithZstd(const void* data, uint64_t dataSize, std::vector<uint8_t>& compressedData,
                         CompressionLevel level);
    
    bool decompressWithZlib(const void* compressedData, uint64_t compressedSize,
                            std::vector<uint8_t>& decompressedData);
    bool decompressWithLz4(const void* compressedData, uint64_t compressedSize,
                           std::vector<uint8_t>& decompressedData);
    bool decompressWithSnappy(const void* compressedData, uint64_t compressedSize,
                             std::vector<uint8_t>& decompressedData);
    bool decompressWithZstd(const void* compressedData, uint64_t compressedSize,
                            std::vector<uint8_t>& decompressedData);
    
    // Components
    std::unique_ptr<CheckpointReader> checkpointReader_;
    std::unique_ptr<CheckpointWriter> checkpointWriter_;
    
    // Configuration
    std::string tempDir_;
    std::shared_ptr<Config> config_;
    CompressionAlgorithm algorithm_;
    CompressionLevel level_;
    
    // State
    mutable std::mutex mutex_;
    uint32_t errorCount_;
    
    // Statistics
    CompressionStats stats_;
    
    // Events
    CompressionEventCallback compressionEventCallback_;
};

} // namespace nlm
