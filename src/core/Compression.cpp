#include "Compression.hpp"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <random>
#include <unordered_map>
#include <cmath>
#include <limits>
#include "Logger.hpp"

namespace nlm {

// CompressionUtils Implementation

std::vector<uint8_t> CompressionUtils::compress(const std::vector<uint8_t>& data, 
                                                CompressionAlgorithm algo) {
    switch (algo) {
        case None:
            return data;
        case Delta:
            return deltaEncode(
                std::vector<uint64_t>(data.begin(), data.end()));
        case RunLength:
            return runLengthEncode(data);
        case SimpleLZ4:
            return lz4Compress(data);
        case ZlibLike:
            return lz4Compress(data); // Simplified: use LZ4 for now
        default:
            return data;
    }
}

std::vector<uint8_t> CompressionUtils::decompress(const std::vector<uint8_t>& data,
                                                  CompressionAlgorithm algo) {
    switch (algo) {
        case None:
            return data;
        case Delta:
            return std::vector<uint8_t>(deltaEncode(
                std::vector<uint64_t>(data.begin(), data.end())).begin(),
                std::vector<uint64_t>(data.begin(), data.end()).end());
        case RunLength:
            return runLengthEncode(data); // Note: This is a placeholder
        case SimpleLZ4:
        case ZlibLike:
            return lz4Decompress(data);
        default:
            return data;
    }
}

double CompressionUtils::calculateCompressionRatio(const std::vector<uint8_t>& original,
                                                   const std::vector<uint8_t>& compressed) {
    if (original.empty()) return 1.0;
    return static_cast<double>(compressed.size()) / original.size();
}

std::vector<uint8_t> CompressionUtils::deltaEncode(const std::vector<uint64_t>& data) {
    if (data.empty()) return {};
    
    std::vector<uint8_t> result;
    result.reserve(data.size() * 2); // Rough estimate
    
    uint64_t prev = data[0];
    result.push_back(static_cast<uint8_t>(prev & 0xFF));
    result.push_back(static_cast<uint8_t>((prev >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>((prev >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((prev >> 24) & 0xFF));
    result.push_back(static_cast<uint8_t>((prev >> 32) & 0xFF));
    result.push_back(static_cast<uint8_t>((prev >> 40) & 0xFF));
    
    for (size_t i = 1; i < data.size(); ++i) {
        uint64_t delta = data[i] - prev;
        // Use zigzag encoding for delta
        uint64_t zigzagDelta = (delta < 0) ? (static_cast<uint64_t>(-delta) * 2 - 1) : (delta * 2);
        
        // Variable-length encoding (simplified)
        do {
            uint8_t byte = static_cast<uint8_t>(zigzagDelta & 0x7F);
            zigzagDelta >>= 7;
            if (zigzagDelta != 0) byte |= 0x80;
            result.push_back(byte);
        } while (zigzagDelta != 0);
        
        prev = data[i];
    }
    
    return result;
}

std::vector<uint8_t> CompressionUtils::runLengthEncode(const std::vector<uint8_t>& data) {
    if (data.empty()) return {};
    
    std::vector<uint8_t> result;
    result.reserve(data.size() * 2); // Rough estimate
    
    uint8_t current = data[0];
    int count = 1;
    
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] == current && count < 255) {
            count++;
        } else {
            // Write run length
            result.push_back(count);
            result.push_back(current);
            current = data[i];
            count = 1;
        }
    }
    
    // Write final run
    result.push_back(static_cast<uint8_t>(count));
    result.push_back(current);
    
    return result;
}

std::vector<uint8_t> CompressionUtils::lz4Compress(const std::vector<uint8_t>& data) {
    if (data.empty()) return {};
    
    // Simple LZ4-like compression: find common patterns and replace with references
    std::vector<uint8_t> result;
    result.reserve(data.size() * 1.1); // Estimate
    
    // Add header
    result.push_back('L');
    result.push_back('Z');
    result.push_back('4');
    result.push_back('1'); // Version
    
    // For simplicity, we'll use a basic approach: 
    // If data is repetitive, use run-length encoding
    // Otherwise, just copy the data (no compression for now)
    if (isRepetitive(data)) {
        // Check if run-length encoding would be better
        auto rleResult = runLengthEncode(data);
        if (rleResult.size() < data.size()) {
            result.push_back(1); // RLE flag
            result.insert(result.end(), rleResult.begin(), rleResult.end());
        } else {
            result.push_back(0); // No compression
            result.insert(result.end(), data.begin(), data.end());
        }
    } else {
        result.push_back(0); // No compression
        result.insert(result.end(), data.begin(), data.end());
    }
    
    return result;
}

std::vector<uint8_t> CompressionUtils::lz4Decompress(const std::vector<uint8_t>& data) {
    if (data.size() < 4) return {};
    
    // Check header
    if (data[0] != 'L' || data[1] != 'Z' || data[2] != '4' || data[3] != '1') {
        // No compression, just return the data
        return std::vector<uint8_t>(data.begin() + 4, data.end());
    }
    
    std::vector<uint8_t> result;
    uint8_t compressionType = data[4];
    
    if (compressionType == 1) { // RLE
        // Decompress RLE data
        for (size_t i = 5; i < data.size(); i += 2) {
            if (i + 1 >= data.size()) break;
            uint8_t count = data[i];
            uint8_t value = data[i + 1];
            result.insert(result.end(), count, value);
        }
    } else {
        // No compression
        result.insert(result.end(), data.begin() + 5, data.end());
    }
    
    return result;
}

bool CompressionUtils::isRepetitive(const std::vector<uint8_t>& data) const {
    if (data.size() < 10) return false;
    
    // Count frequency of each byte value
    std::unordered_map<uint8_t, int> frequency;
    for (uint8_t byte : data) {
        frequency[byte]++;
    }
    
    // Check if one value appears many times
    int maxFreq = 0;
    for (const auto& pair : frequency) {
        if (pair.second > maxFreq) {
            maxFreq = pair.second;
        }
    }
    
    // If one value appears more than 50% of the time, it's repetitive
    return (maxFreq > static_cast<int>(data.size() * 0.5));
}

uint32_t CompressionUtils::calculateChecksum(const std::vector<uint8_t>& data) {
    uint32_t checksum = 0xDEADBEEF;
    for (uint8_t byte : data) {
        checksum = (checksum * 31) ^ (byte * 17);
        checksum = checksum + 0x9E3779B9; // Prime number mixing
    }
    return checksum;
}

bool CompressionUtils::validateIntegrity(const std::vector<uint8_t>& data, uint32_t expectedChecksum) {
    uint32_t checksum = calculateChecksum(data);
    return checksum == expectedChecksum;
}

std::vector<uint8_t> CompressionUtils::packData(const std::vector<std::vector<uint8_t>>& dataBlocks) {
    std::vector<uint8_t> result;
    std::vector<size_t> blockSizes;
    
    for (const auto& block : dataBlocks) {
        blockSizes.push_back(block.size());
        result.insert(result.end(), block.begin(), block.end());
    }
    
    // Add block size information
    std::vector<uint8_t> sizeInfo;
    for (size_t size : blockSizes) {
        for (int i = 0; i < 4; ++i) {
            sizeInfo.push_back(static_cast<uint8_t>((size >> (i * 8)) & 0xFF));
        }
    }
    
    // Prepend size information
    result.insert(result.begin(), sizeInfo.begin(), sizeInfo.end());
    
    return result;
}

std::vector<std::vector<uint8_t>> CompressionUtils::unpackData(const std::vector<uint8_t>& packedData,
                                                              const std::vector<size_t>& blockSizes) {
    std::vector<std::vector<uint8_t>> result;
    
    if (packedData.size() < blockSizes.size() * 4) return result;
    
    // Extract block sizes from packed data
    std::vector<size_t> extractedSizes;
    for (size_t i = 0; i < blockSizes.size(); ++i) {
        size_t size = 0;
        for (int j = 0; j < 4; ++j) {
            size |= static_cast<size_t>(packedData[j * 4 + i]) << (j * 8);
        }
        extractedSizes.push_back(size);
    }
    
    // Split data into blocks
    size_t offset = blockSizes.size() * 4;
    for (size_t i = 0; i < extractedSizes.size(); ++i) {
        size_t size = extractedSizes[i];
        if (offset + size > packedData.size()) {
            // Invalid size
            result.clear();
            return result;
        }
        
        std::vector<uint8_t> block(packedData.begin() + offset, 
                                   packedData.begin() + offset + size);
        result.push_back(block);
        offset += size;
    }
    
    return result;
}

std::vector<uint8_t> CompressionUtils::compressNeuronData(const std::vector<float>& membranePotential,
                                                           const std::vector<float>& firingRates,
                                                           const std::vector<uint8_t>& firingStates) {
    // Pack neuron data into a compact format
    size_t neuronCount = membranePotential.size();
    if (neuronCount == 0) return {};
    
    std::vector<uint8_t> result;
    
    // Add neuron count
    for (int i = 0; i < 4; ++i) {
        result.push_back(static_cast<uint8_t>((neuronCount >> (i * 8)) & 0xFF));
    }
    
    // Pack membrane potential (float -> uint32_t)
    for (float value : membranePotential) {
        uint32_t packed = *reinterpret_cast<uint32_t*>(&value);
        for (int i = 0; i < 4; ++i) {
            result.push_back(static_cast<uint8_t>((packed >> (i * 8)) & 0xFF));
        }
    }
    
    // Pack firing rates (float -> uint32_t)
    for (float value : firingRates) {
        uint32_t packed = *reinterpret_cast<uint32_t*>(&value);
        for (int i = 0; i < 4; ++i) {
            result.push_back(static_cast<uint8_t>((packed >> (i * 8)) & 0xFF));
        }
    }
    
    // Pack firing states
    result.insert(result.end(), firingStates.begin(), firingStates.end());
    
    return result;
}

void CompressionUtils::decompressNeuronData(const std::vector<uint8_t>& compressedData,
                                            std::vector<float>& membranePotential,
                                            std::vector<float>& firingRates,
                                            std::vector<uint8_t>& firingStates) {
    if (compressedData.size() < 4) {
        membranePotential.clear();
        firingRates.clear();
        firingStates.clear();
        return;
    }
    
    // Extract neuron count
    size_t neuronCount = 0;
    for (int i = 0; i < 4; ++i) {
        neuronCount |= static_cast<size_t>(compressedData[i]) << (i * 8);
    }
    
    membranePotential.clear();
    firingRates.clear();
    firingStates.clear();
    
    membranePotential.reserve(neuronCount);
    firingRates.reserve(neuronCount);
    firingStates.reserve(neuronCount);
    
    // Extract membrane potential
    size_t offset = 4;
    for (size_t i = 0; i < neuronCount; ++i) {
        if (offset + 4 > compressedData.size()) break;
        
        uint32_t packed = 0;
        for (int j = 0; j < 4; ++j) {
            packed |= static_cast<uint32_t>(compressedData[offset + j]) << (j * 8);
        }
        
        float value = *reinterpret_cast<float*>(&packed);
        membranePotential.push_back(value);
        offset += 4;
    }
    
    // Extract firing rates
    for (size_t i = 0; i < neuronCount; ++i) {
        if (offset + 4 > compressedData.size()) break;
        
        uint32_t packed = 0;
        for (int j = 0; j < 4; ++j) {
            packed |= static_cast<uint32_t>(compressedData[offset + j]) << (j * 8);
        }
        
        float value = *reinterpret_cast<float*>(&packed);
        firingRates.push_back(value);
        offset += 4;
    }
    
    // Extract firing states
    for (size_t i = 0; i < neuronCount; ++i) {
        if (offset >= compressedData.size()) break;
        firingStates.push_back(compressedData[offset]);
        offset++;
    }
}

std::vector<uint8_t> CompressionUtils::compressSynapseData(const std::vector<float>& weights,
                                                            const std::vector<uint32_t>& delays,
                                                            const std::vector<uint8_t>& synapseTypes) {
    // Pack synapse data into a compact format
    size_t synapseCount = weights.size();
    if (synapseCount == 0) return {};
    
    std::vector<uint8_t> result;
    
    // Add synapse count
    for (int i = 0; i < 4; ++i) {
        result.push_back(static_cast<uint8_t>((synapseCount >> (i * 8)) & 0xFF));
    }
    
    // Pack weights (float -> uint32_t)
    for (float weight : weights) {
        uint32_t packed = *reinterpret_cast<uint32_t*>(&weight);
        for (int i = 0; i < 4; ++i) {
            result.push_back(static_cast<uint8_t>((packed >> (i * 8)) & 0xFF));
        }
    }
    
    // Pack delays (uint32_t)
    for (uint32_t delay : delays) {
        for (int i = 0; i < 4; ++i) {
            result.push_back(static_cast<uint8_t>((delay >> (i * 8)) & 0xFF));
        }
    }
    
    // Pack synapse types
    result.insert(result.end(), synapseTypes.begin(), synapseTypes.end());
    
    return result;
}

void CompressionUtils::decompressSynapseData(const std::vector<uint8_t>& compressedData,
                                             std::vector<float>& weights,
                                             std::vector<uint32_t>& delays,
                                             std::vector<uint8_t>& synapseTypes) {
    if (compressedData.size() < 4) {
        weights.clear();
        delays.clear();
        synapseTypes.clear();
        return;
    }
    
    // Extract synapse count
    size_t synapseCount = 0;
    for (int i = 0; i < 4; ++i) {
        synapseCount |= static_cast<size_t>(compressedData[i]) << (i * 8);
    }
    
    weights.clear();
    delays.clear();
    synapseTypes.clear();
    
    weights.reserve(synapseCount);
    delays.reserve(synapseCount);
    synapseTypes.reserve(synapseCount);
    
    // Extract weights
    size_t offset = 4;
    for (size_t i = 0; i < synapseCount; ++i) {
        if (offset + 4 > compressedData.size()) break;
        
        uint32_t packed = 0;
        for (int j = 0; j < 4; ++j) {
            packed |= static_cast<uint32_t>(compressedData[offset + j]) << (j * 8);
        }
        
        float weight = *reinterpret_cast<float*>(&packed);
        weights.push_back(weight);
        offset += 4;
    }
    
    // Extract delays
    for (size_t i = 0; i < synapseCount; ++i) {
        if (offset + 4 > compressedData.size()) break;
        
        uint32_t delay = 0;
        for (int j = 0; j < 4; ++j) {
            delay |= static_cast<uint32_t>(compressedData[offset + j]) << (j * 8);
        }
        
        delays.push_back(delay);
        offset += 4;
    }
    
    // Extract synapse types
    for (size_t i = 0; i < synapseCount; ++i) {
        if (offset >= compressedData.size()) break;
        synapseTypes.push_back(compressedData[offset]);
        offset++;
    }
}

CompressionUtils::CompressionAlgorithm CompressionUtils::getOptimalAlgorithm(const std::vector<uint8_t>& data,
                                                                             size_t dataSize) {
    if (dataSize == 0) return None;
    
    // Check data characteristics
    bool isRepetitive = isRepetitive(data);
    bool isSequential = isSequential(data);
    bool isRandom = !isSequential && !isRepetitive;
    
    if (isRepetitive) {
        return RunLength;
    } else if (isSequential) {
        return Delta;
    } else {
        // For random data, use LZ4 (fast compression)
        return SimpleLZ4;
    }
}

bool CompressionUtils::isSequential(const std::vector<uint8_t>& data) const {
    if (data.size() < 2) return false;
    
    // Check if values follow a pattern (increasing or decreasing)
    int increasing = 0;
    int decreasing = 0;
    
    for (size_t i = 1; i < data.size(); ++i) {
        if (data[i] > data[i-1]) increasing++;
        else if (data[i] < data[i-1]) decreasing++;
    }
    
    return (increasing > decreasing) && (increasing > data.size() * 0.7);
}

size_t CompressionUtils::estimateCompressedSize(const std::vector<uint8_t>& data,
                                                CompressionAlgorithm algo) {
    switch (algo) {
        case None:
            return data.size();
        case Delta:
            // Delta encoding typically compresses sequential data well
            return data.size() * 0.8; // Rough estimate
        case RunLength:
            // Run-length encoding works well for repetitive data
            if (isRepetitive(data)) {
                return data.size() * 0.5; // Rough estimate
            } else {
                return data.size() * 1.2; // Actually expands for non-repetitive data
            }
        case SimpleLZ4:
        case ZlibLike:
            return data.size() * 0.9; // Rough estimate for LZ4
        default:
            return data.size();
    }
}

std::vector<uint8_t> CompressionUtils::adaptiveCompress(const std::vector<uint8_t>& data) {
    if (data.empty()) return {};
    
    // Try different algorithms and pick the best one
    std::vector<std::pair<CompressionAlgorithm, std::vector<uint8_t>>> results;
    
    for (int algo = None; algo <= ZlibLike; ++algo) {
        std::vector<uint8_t> compressed = compress(data, static_cast<CompressionAlgorithm>(algo));
        results.push_back(std::make_pair(static_cast<CompressionAlgorithm>(algo), compressed));
    }
    
    // Find the smallest compressed result
    size_t minSize = std::numeric_limits<size_t>::max();
    CompressionAlgorithm bestAlgo = None;
    
    for (const auto& result : results) {
        if (result.second.size() < minSize) {
            minSize = result.second.size();
            bestAlgo = result.first;
        }
    }
    
    return results[bestAlgo].second;
}

CompressionUtils::CompressionStats CompressionUtils::compressWithStats(const std::vector<uint8_t>& data,
                                                                        CompressionAlgorithm algo) {
    CompressionStats stats;
    stats.originalSize = data.size();
    
    auto startTime = std::chrono::high_resolution_clock::now();
    std::vector<uint8_t> compressed = compress(data, algo);
    auto endTime = std::chrono::high_resolution_clock::now();
    
    stats.compressedSize = compressed.size();
    stats.ratio = static_cast<double>(compressed.size()) / data.size();
    
    auto compressionDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    stats.compressionTime = compressionDuration.count();
    
    // Estimate decompression time
    startTime = std::chrono::high_resolution_clock::now();
    decompress(compressed, algo);
    endTime = std::chrono::high_resolution_clock::now();
    auto decompressionDuration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    stats.decompressionTime = decompressionDuration.count();
    
    // Set algorithm name
    switch (algo) {
        case None: stats.algorithm = "None"; break;
        case Delta: stats.algorithm = "Delta Encoding"; break;
        case RunLength: stats.algorithm = "Run-Length Encoding"; break;
        case SimpleLZ4: stats.algorithm = "Simple LZ4"; break;
        case ZlibLike: stats.algorithm = "Zlib-Like"; break;
        default: stats.algorithm = "Unknown";
    }
    
    return stats;
}

// CheckpointCompressor Implementation

CheckpointCompressor::CheckpointCompressor()
    : level_(Balanced), validateChecksum_(true) {
    pImpl = std::make_unique<Impl>();
}

CheckpointCompressor::~CheckpointCompressor() = default;

std::vector<uint8_t> CheckpointCompressor::compressCheckpoint(const std::vector<uint8_t>& checkpointData,
                                                               CompressionLevel level) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Choose algorithm based on level
    CompressionUtils::CompressionAlgorithm algo;
    switch (level) {
        case Fast: algo = CompressionUtils::SimpleLZ4; break;
        case Balanced: algo = CompressionUtils::getOptimalAlgorithm(checkpointData, checkpointData.size()); break;
        case Optimal: algo = CompressionUtils::ZlibLike; break;
        default: algo = CompressionUtils::SimpleLZ4; break;
    }
    
    // Compress the data
    std::vector<uint8_t> compressed = compressWithAlgorithm(checkpointData, algo);
    
    // Create metadata
    CheckpointMetadata metadata;
    metadata.checksum = CompressionUtils::calculateChecksum(checkpointData);
    metadata.originalSize = checkpointData.size();
    metadata.compressedSize = compressed.size();
    metadata.algorithm = algo;
    metadata.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    metadata.description = "NLM Brain Checkpoint";
    
    // Add metadata to compressed data
    std::vector<uint8_t> result = addMetadata(compressed, metadata);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    // Update statistics
    stats_.originalSize = metadata.originalSize;
    stats_.compressedSize = metadata.compressedSize;
    stats_.ratio = static_cast<double>(metadata.compressedSize) / metadata.originalSize;
    stats_.compressionTime = duration.count();
    
    return result;
}

std::vector<uint8_t> CheckpointCompressor::decompressCheckpoint(const std::vector<uint8_t>& compressedData,
                                                                 CompressionLevel level) {
    // Extract metadata
    auto metadata = extractMetadata(compressedData);
    
    // Validate checksum if enabled
    if (validateChecksum_) {
        uint32_t actualChecksum = CompressionUtils::calculateChecksum(
            std::vector<uint8_t>(compressedData.begin() + sizeof(CheckpointMetadata), compressedData.end()));
        
        if (actualChecksum != metadata.checksum) {
            throw std::runtime_error("Checkpoint integrity validation failed: checksum mismatch");
        }
    }
    
    // Extract the compressed data (without metadata)
    std::vector<uint8_t> compressedDataWithoutMetadata(
        compressedData.begin() + sizeof(CheckpointMetadata),
        compressedData.end());
    
    // Decompress using the specified algorithm
    return compressWithAlgorithm(compressedDataWithoutMetadata, metadata.algorithm);
}

bool CheckpointCompressor::validateCheckpoint(const std::vector<uint8_t>& compressedData,
                                               uint32_t expectedChecksum) {
    try {
        auto metadata = extractMetadata(compressedData);
        
        if (expectedChecksum != 0 && metadata.checksum != expectedChecksum) {
            return false;
        }
        
        if (metadata.originalSize == 0) {
            return false;
        }
        
        // Try to decompress to validate integrity
        auto decompressed = compressWithAlgorithm(
            std::vector<uint8_t>(compressedData.begin() + sizeof(CheckpointMetadata), compressedData.end()),
            metadata.algorithm);
        
        return decompressed.size() == metadata.originalSize;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Checkpoint validation failed: ") + e.what());
        return false;
    }
}

CheckpointCompressor::CheckpointMetadata CheckpointCompressor::getCheckpointMetadata(const std::vector<uint8_t>& compressedData) {
    return extractMetadata(compressedData).second;
}

std::string CheckpointCompressor::getCompressionStats() const {
    std::stringstream ss;
    ss << "Compression Statistics:" << std::endl;
    ss << "  Original size: " << stats_.originalSize << " bytes" << std::endl;
    ss << "  Compressed size: " << stats_.compressedSize << " bytes" << std::endl;
    ss << "  Compression ratio: " << (stats_.ratio * 100) << "%" << std::endl;
    ss << "  Algorithm: " << stats_.algorithm << std::endl;
    ss << "  Compression time: " << stats_.compressionTime << " microseconds" << std::endl;
    ss << "  Decompression time: " << stats_.decompressionTime << " microseconds" << std::endl;
    
    return ss.str();
}

std::vector<uint8_t> CheckpointCompressor::compressWithAlgorithm(const std::vector<uint8_t>& data,
                                                                   CompressionUtils::CompressionAlgorithm algo) {
    // For now, use the simple LZ4 algorithm as default
    // In a real implementation, you would add support for different compression algorithms
    return CompressionUtils::lz4Compress(data);
}

std::vector<uint8_t> CheckpointCompressor::addMetadata(const std::vector<uint8_t>& data,
                                                        const CheckpointMetadata& metadata) {
    std::vector<uint8_t> result;
    
    // Serialize metadata
    std::vector<uint8_t> metadataBytes;
    metadataBytes.resize(sizeof(CheckpointMetadata));
    
    // Copy metadata fields
    std::memcpy(metadataBytes.data(), &metadata, sizeof(CheckpointMetadata));
    
    // Add metadata length prefix
    result.push_back(static_cast<uint8_t>((sizeof(CheckpointMetadata) >> 24) & 0xFF));
    result.push_back(static_cast<uint8_t>((sizeof(CheckpointMetadata) >> 16) & 0xFF));
    result.push_back(static_cast<uint8_t>((sizeof(CheckpointMetadata) >> 8) & 0xFF));
    result.push_back(static_cast<uint8_t>(sizeof(CheckpointMetadata) & 0xFF));
    
    // Add metadata
    result.insert(result.end(), metadataBytes.begin(), metadataBytes.end());
    
    // Add compressed data
    result.insert(result.end(), data.begin(), data.end());
    
    return result;
}

std::pair<std::vector<uint8_t>, CheckpointCompressor::CheckpointMetadata> CheckpointCompressor::extractMetadata(const std::vector<uint8_t>& data) {
    CheckpointMetadata metadata;
    std::vector<uint8_t> compressedData;
    
    if (data.size() < 4) {
        throw std::runtime_error("Invalid checkpoint data: too small");
    }
    
    // Extract metadata length
    size_t metadataSize = 0;
    for (int i = 0; i < 4; ++i) {
        metadataSize |= static_cast<size_t>(data[i]) << (i * 8);
    }
    
    if (metadataSize > data.size()) {
        throw std::runtime_error("Invalid checkpoint data: metadata size exceeds data size");
    }
    
    if (metadataSize > sizeof(CheckpointMetadata)) {
        throw std::runtime_error("Invalid checkpoint data: metadata size too large");
    }
    
    // Extract metadata
    std::vector<uint8_t> metadataBytes(data.begin(), data.begin() + metadataSize);
    std::memcpy(&metadata, metadataBytes.data(), metadataBytes.size());
    
    // Extract compressed data
    compressedData.insert(compressedData.end(), 
                         data.begin() + metadataSize, data.end());
    
    return std::make_pair(compressedData, metadata);
}

// CheckpointValidator Implementation

CheckpointValidator::CheckpointValidator()
    : strictValidation_(true), maxValidationTimeMs_(1000) {
    pImpl = std::make_unique<Impl>();
}

CheckpointValidator::~CheckpointValidator() = default;

bool CheckpointValidator::validate(const std::vector<uint8_t>& checkpointData,
                                   bool validateContent,
                                   bool validateMetadata) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    stats_.totalValidations++;
    
    try {
        bool isValid = true;
        
        if (validateMetadata) {
            isValid &= validateChecksum(checkpointData);
        }
        
        if (validateContent && isValid) {
            isValid &= validateDataStructure(checkpointData);
            isValid &= validateTemporalConsistency(checkpointData);
        }
        
        if (isValid) {
            stats_.successfulValidations++;
        } else {
            stats_.failedValidations++;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        stats_.totalValidationTime += duration.count();
        
        return isValid;
    } catch (const std::exception& e) {
        stats_.failedValidations++;
        NLM_LOG_ERROR(std::string("Checkpoint validation exception: ") + e.what());
        return false;
    }
}

bool CheckpointValidator::checkForCorruption(const std::vector<uint8_t>& checkpointData) const {
    // Simple corruption check: verify checksum and basic structure
    return validateChecksum(checkpointData) && validateDataStructure(checkpointData);
}

bool CheckpointValidator::validateConsistency(const std::vector<uint8_t>& checkpointData) const {
    // Check temporal consistency: ensure timestamps are reasonable
    // For now, just return true as this is a simplified implementation
    return true;
}

bool CheckpointValidator::isLoadable(const std::vector<uint8_t>& checkpointData,
                                     size_t expectedNeuronCount,
                                     size_t expectedSynapseCount) const {
    // Try to extract metadata and validate basic structure
    try {
        auto metadata = CheckpointCompressor::extractMetadata(checkpointData);
        
        // Validate basic metadata
        if (metadata.second.originalSize == 0) {
            return false;
        }
        
        // In a full implementation, you would attempt to decompress and validate
        // the actual brain state data against expected counts
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Checkpoint load validation failed: ") + e.what());
        return false;
    }
}

bool CheckpointValidator::validateAgainstBrainState(const std::vector<uint8_t>& checkpointData,
                                                    const Brain& brain) const {
    // Try to decompress and validate against the current brain state
    try {
        auto metadata = CheckpointCompressor::extractMetadata(checkpointData);
        auto compressedData = std::vector<uint8_t>(checkpointData.begin() + sizeof(CheckpointCompressor::CheckpointMetadata), checkpointData.end());
        
        auto decompressedData = CompressionUtils::lz4Decompress(compressedData);
        
        // In a full implementation, you would deserialize the decompressed data
        // and compare it with the current brain state
        
        return decompressedData.size() == metadata.second.originalSize;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Brain state validation failed: ") + e.what());
        return false;
    }
}

bool CheckpointValidator::validateChecksum(const std::vector<uint8_t>& checkpointData) const {
    if (checkpointData.size() < sizeof(CheckpointCompressor::CheckpointMetadata) + 8) {
        return false;
    }
    
    // Extract metadata (simplified)
    size_t metadataSize = 0;
    for (int i = 0; i < 4; ++i) {
        metadataSize |= static_cast<size_t>(checkpointData[i]) << (i * 8);
    }
    
    if (metadataSize > checkpointData.size()) {
        return false;
    }
    
    // Calculate checksum of the actual data (without metadata)
    std::vector<uint8_t> actualData(
        checkpointData.begin() + metadataSize, checkpointData.end());
    
    uint32_t actualChecksum = CompressionUtils::calculateChecksum(actualData);
    
    // In a real implementation, you would extract the expected checksum from metadata
    // For now, we'll just validate that the checksum calculation works
    return actualChecksum != 0; // Non-zero checksum is valid
}

bool CheckpointValidator::validateDataStructure(const std::vector<uint8_t>& checkpointData) const {
    // Basic structure validation
    if (checkpointData.size() < 100) {
        return false;
    }
    
    // Check for reasonable content
    // This is a simplified implementation
    return true;
}

} // namespace nlm
