#pragma once

#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <cmath>
#include "MemoryPool.hpp"

namespace nlm {

// Simple compression utilities for checkpoint data
class CompressionUtils {
public:
    enum CompressionAlgorithm {
        None = 0,      // No compression
        Delta = 1,     // Delta encoding (for sequential data)
        RunLength = 2, // Run-length encoding
        SimpleLZ4 = 3, // Simple LZ4-like compression
        ZlibLike = 4   // Zlib-like compression (simplified)
    };
    
    // Compress data using specified algorithm
    static std::vector<uint8_t> compress(const std::vector<uint8_t>& data, 
                                         CompressionAlgorithm algo = SimpleLZ4);
    
    // Decompress data using specified algorithm
    static std::vector<uint8_t> decompress(const std::vector<uint8_t>& data,
                                           CompressionAlgorithm algo = SimpleLZ4);
    
    // Calculate compression ratio
    static double calculateCompressionRatio(const std::vector<uint8_t>& original,
                                           const std::vector<uint8_t>& compressed);
    
    // Simple delta encoding for sequential data (good for checkpoint changes)
    static std::vector<uint8_t> deltaEncode(const std::vector<uint64_t>& data);
    
    // Simple run-length encoding
    static std::vector<uint8_t> runLengthEncode(const std::vector<uint8_t>& data);
    
    // Simple LZ4-like compression (fast, good compression ratio)
    static std::vector<uint8_t> lz4Compress(const std::vector<uint8_t>& data);
    
    // Simple decompression helper
    static std::vector<uint8_t> lz4Decompress(const std::vector<uint8_t>& data);
    
    // Calculate checksum for data integrity validation
    static uint32_t calculateChecksum(const std::vector<uint8_t>& data);
    
    // Validate data integrity using checksum
    static bool validateIntegrity(const std::vector<uint8_t>& data, uint32_t expectedChecksum);
    
    // Pack multiple small values into larger values (space optimization)
    static std::vector<uint8_t> packData(const std::vector<std::vector<uint8_t>>& dataBlocks);
    
    // Unpack multiple small values from larger values
    static std::vector<std::vector<uint8_t>> unpackData(const std::vector<uint8_t>& packedData,
                                                        const std::vector<size_t>& blockSizes);
    
    // Compress neuron state data using packing and encoding
    static std::vector<uint8_t> compressNeuronData(const std::vector<float>& membranePotential,
                                                  const std::vector<float>& firingRates,
                                                  const std::vector<uint8_t>& firingStates);
    
    // Decompress neuron state data
    static void decompressNeuronData(const std::vector<uint8_t>& compressedData,
                                     std::vector<float>& membranePotential,
                                     std::vector<float>& firingRates,
                                     std::vector<uint8_t>& firingStates);
    
    // Compress synapse data using delta encoding and packing
    static std::vector<uint8_t> compressSynapseData(const std::vector<float>& weights,
                                                   const std::vector<uint32_t>& delays,
                                                   const std::vector<uint8_t>& synapseTypes);
    
    // Decompress synapse data
    static void decompressSynapseData(const std::vector<uint8_t>& compressedData,
                                      std::vector<float>& weights,
                                      std::vector<uint32_t>& delays,
                                      std::vector<uint8_t>& synapseTypes);
    
    // Get optimal compression algorithm for specific data type
    static CompressionAlgorithm getOptimalAlgorithm(const std::vector<uint8_t>& data,
                                                    size_t dataSize);
    
    // Estimate compressed size for different algorithms
    static size_t estimateCompressedSize(const std::vector<uint8_t>& data,
                                         CompressionAlgorithm algo);
    
    // Adaptive compression based on data characteristics
    static std::vector<uint8_t> adaptiveCompress(const std::vector<uint8_t>& data);
    
    // Compression statistics for monitoring
    struct CompressionStats {
        size_t originalSize;
        size_t compressedSize;
        double ratio;
        std::string algorithm;
        size_t compressionTime;
        size_t decompressionTime;
        
        CompressionStats() : originalSize(0), compressedSize(0), ratio(0.0), 
                           compressionTime(0), decompressionTime(0) {}
        
        std::string getSummary() const {
            return "Original: " + std::to_string(originalSize) + " bytes, "
                   "Compressed: " + std::to_string(compressedSize) + " bytes, "
                   "Ratio: " + std::to_string(ratio * 100) + "%, "
                   "Algorithm: " + algorithm;
        }
    };
    
    // Compress data with statistics
    static CompressionStats compressWithStats(const std::vector<uint8_t>& data,
                                              CompressionAlgorithm algo = SimpleLZ4);
};

// Checkpoint compression utility for memory-efficient checkpointing
class CheckpointCompressor {
public:
    enum CompressionLevel {
        Fast = 0,      // Fast compression, lower ratio
        Balanced = 1,  // Balanced compression speed and ratio
        Optimal = 2    // Optimal compression, slower
    };
    
    CheckpointCompressor();
    ~CheckpointCompressor();
    
    // Disable copying
    CheckpointCompressor(const CheckpointCompressor&) = delete;
    CheckpointCompressor& operator=(const CheckpointCompressor&) = delete;
    
    // Compress checkpoint data
    std::vector<uint8_t> compressCheckpoint(const std::vector<uint8_t>& checkpointData,
                                            CompressionLevel level = Balanced);
    
    // Decompress checkpoint data
    std::vector<uint8_t> decompressCheckpoint(const std::vector<uint8_t>& compressedData,
                                              CompressionLevel level = Balanced);
    
    // Validate checkpoint integrity
    bool validateCheckpoint(const std::vector<uint8_t>& compressedData,
                           uint32_t expectedChecksum = 0);
    
    // Get checkpoint metadata (size, checksum, etc.)
    struct CheckpointMetadata {
        uint32_t checksum;
        uint64_t originalSize;
        uint64_t compressedSize;
        CompressionUtils::CompressionAlgorithm algorithm;
        uint64_t timestamp;
        std::string description;
        
        CheckpointMetadata() : checksum(0), originalSize(0), compressedSize(0),
                             algorithm(CompressionUtils::None), timestamp(0) {}
    };
    
    // Get metadata from compressed checkpoint
    CheckpointMetadata getCheckpointMetadata(const std::vector<uint8_t>& compressedData);
    
    // Set compression level
    void setCompressionLevel(CompressionLevel level) { level_ = level; }
    
    // Get current compression level
    CompressionLevel getCompressionLevel() const { return level_; }
    
    // Enable/disable checksum validation
    void setChecksumValidation(bool enable) { validateChecksum_ = enable; }
    
    // Check if checksum validation is enabled
    bool isChecksumValidationEnabled() const { return validateChecksum_; }
    
    // Get compression statistics
    std::string getCompressionStats() const;
    
    // Reset statistics
    void resetStats() { stats_ = {0, 0, 0.0, "", 0, 0}; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    CompressionLevel level_;
    bool validateChecksum_;
    CompressionUtils::CompressionStats stats_;
    
    // Internal compression methods
    std::vector<uint8_t> compressWithAlgorithm(const std::vector<uint8_t>& data,
                                              CompressionUtils::CompressionAlgorithm algo);
    
    std::vector<uint8_t> addMetadata(const std::vector<uint8_t>& data,
                                     const CheckpointMetadata& metadata);
    
    std::pair<std::vector<uint8_t>, CheckpointMetadata> extractMetadata(const std::vector<uint8_t>& data);
};

// Memory-efficient checkpoint validator
class CheckpointValidator {
public:
    CheckpointValidator();
    ~CheckpointValidator();
    
    // Disable copying
    CheckpointValidator(const CheckpointValidator&) = delete;
    CheckpointValidator& operator=(const CheckpointValidator&) = delete;
    
    // Validate checkpoint data integrity
    bool validate(const std::vector<uint8_t>& checkpointData,
                  bool validateContent = true,
                  bool validateMetadata = true) const;
    
    // Check for data corruption
    bool checkForCorruption(const std::vector<uint8_t>& checkpointData) const;
    
    // Validate checkpoint consistency
    bool validateConsistency(const std::vector<uint8_t>& checkpointData) const;
    
    // Check if checkpoint is loadable
    bool isLoadable(const std::vector<uint8_t>& checkpointData,
                   size_t expectedNeuronCount = 0,
                   size_t expectedSynapseCount = 0) const;
    
    // Validate checkpoint against expected brain state
    bool validateAgainstBrainState(const std::vector<uint8_t>& checkpointData,
                                  const class Brain& brain) const;
    
    // Set validation options
    void setStrictValidation(bool strict) { strictValidation_ = strict; }
    
    void setMaxValidationTimeMs(uint32_t timeMs) { maxValidationTimeMs_ = timeMs; }
    
    // Get validation statistics
    struct ValidationStats {
        uint32_t totalValidations;
        uint32_t successfulValidations;
        uint32_t failedValidations;
        uint32_t corruptCheckpoints;
        uint32_t invalidCheckpoints;
        uint64_t totalValidationTime;
        
        ValidationStats() : totalValidations(0), successfulValidations(0),
                          failedValidations(0), corruptCheckpoints(0),
                          invalidCheckpoints(0), totalValidationTime(0) {}
        
        double getSuccessRate() const {
            return totalValidations > 0 ? 
                static_cast<double>(successfulValidations) / totalValidations : 0.0;
        }
    };
    
    ValidationStats getValidationStats() const { return stats_; }
    
    void resetStats() { stats_ = ValidationStats(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool strictValidation_;
    uint32_t maxValidationTimeMs_;
    ValidationStats stats_;
    
    // Internal validation methods
    bool validateChecksum(const std::vector<uint8_t>& checkpointData) const;
    bool validateDataStructure(const std::vector<uint8_t>& checkpointData) const;
    bool validateTemporalConsistency(const std::vector<uint8_t>& checkpointData) const;
    bool validateMemoryConstraints(const std::vector<uint8_t>& checkpointData,
                                   const class Brain& brain) const;
};

} // namespace nlm
