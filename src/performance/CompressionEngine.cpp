// CompressionEngine.cpp
#include "CompressionEngine.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <cmath>
#include <cstring>

namespace nlm {
    
    // Thread pool size for parallel compression
    size_t CompressionEngine::threadPoolSize_ = std::thread::hardware_concurrency();
    
    CompressionEngine::CompressionEngine() {}
    
    CompressionEngine::~CompressionEngine() {}
    
    std::vector<uint8_t> CompressionEngine::compress(
        const std::vector<uint8_t>& data,
        CompressionAlgorithm algorithm,
        int level,
        CompressionMode mode) {
        
        if (algorithm == CompressionAlgorithm::None || data.empty()) {
            return data;
        }
        
        switch (algorithm) {
            case CompressionAlgorithm::Gzip:
                return compressGzip(data, level);
            case CompressionAlgorithm::Zstd:
                return compressZstd(data, level);
            case CompressionAlgorithm::Lz4:
                return compressLz4(data, level);
            case CompressionAlgorithm::Zlib:
                return compressZlib(data, level);
            default:
                return data; // Unsupported algorithm
        }
    }
    
    std::vector<uint8_t> CompressionEngine::decompress(
        const std::vector<uint8_t>& data,
        uint64_t originalSize,
        CompressionAlgorithm algorithm) {
        
        if (algorithm == CompressionAlgorithm::None || data.empty()) {
            return data;
        }
        
        switch (algorithm) {
            case CompressionAlgorithm::Gzip:
                return decompressGzip(data, originalSize);
            case CompressionAlgorithm::Zstd:
                return decompressZstd(data, originalSize);
            case CompressionAlgorithm::Lz4:
                return decompressLz4(data, originalSize);
            case CompressionAlgorithm::Zlib:
                return decompressZlib(data, originalSize);
            default:
                throw std::runtime_error("Unsupported decompression algorithm");
        }
    }
    
    std::vector<uint8_t> CompressionEngine::compressDelta(
        const std::vector<uint8_t>& currentData,
        const std::vector<uint8_t>& baseData,
        CompressionAlgorithm algorithm) {
        
        std::vector<uint8_t> delta = createDelta(currentData, baseData);
        
        // Compress the delta
        return compress(delta, algorithm, 3);
    }
    
    std::vector<uint8_t> CompressionEngine::applyDelta(
        const std::vector<uint8_t>& baseData,
        const std::vector<uint8_t>& deltaData,
        CompressionAlgorithm algorithm) {
        
        // Decompress delta first
        std::vector<uint8_t> delta = decompress(deltaData, deltaData.size(), algorithm);
        
        // Apply delta to base
        std::vector<uint8_t> result = baseData;
        return applyDeltaToBuffer(result, delta);
    }
    
    CompressionStats CompressionEngine::analyzeCompression(
        const std::vector<uint8_t>& data,
        CompressionAlgorithm algorithm,
        int level) {
        
        auto startTime = std::chrono::high_resolution_clock::now();
        std::vector<uint8_t> compressed = compress(data, algorithm, level);
        auto endTime = std::chrono::high_resolution_clock::now();
        
        uint64_t originalSize = data.size();
        uint64_t compressedSize = compressed.size();
        double ratio = originalSize > 0 ? 
            static_cast<double>(compressedSize) / originalSize : 0.0;
        
        uint64_t timeNs = std::chrono::duration<uint64_t, std::nano>(endTime - startTime).count();
        
        return CompressionStats(originalSize, compressedSize, ratio, timeNs, algorithm, level);
    }
    
    CompressionAlgorithm CompressionEngine::recommendAlgorithm(
        const std::vector<uint8_t>& data,
        size_t size) {
        
        // Simple heuristic: larger data benefits from better compression
        if (size > 10 * 1024 * 1024) { // > 10MB
            return CompressionAlgorithm::Zstd;
        } else if (size > 1024 * 1024) { // > 1MB
            return CompressionAlgorithm::Gzip;
        } else {
            return CompressionAlgorithm::Lz4; // Faster for small data
        }
    }
    
    std::vector<CompressionAlgorithm> CompressionEngine::getAvailableAlgorithms() {
        std::vector<CompressionAlgorithm> algorithms = {
            CompressionAlgorithm::Gzip,
            CompressionAlgorithm::Zstd,
            CompressionAlgorithm::Lz4,
            CompressionAlgorithm::Zlib
        };
        
        // Filter out unavailable algorithms
        std::vector<CompressionAlgorithm> available;
        for (auto algo : algorithms) {
            if (isAlgorithmAvailable(algo)) {
                available.push_back(algo);
            }
        }
        
        return available;
    }
    
    bool CompressionEngine::isAlgorithmAvailable(CompressionAlgorithm algorithm) {
        // For now, assume all algorithms are available
        // In a real implementation, this would check for actual library support
        switch (algorithm) {
            case CompressionAlgorithm::Gzip:
            case CompressionAlgorithm::Zstd:
            case CompressionAlgorithm::Lz4:
            case CompressionAlgorithm::Zlib:
                return true;
            default:
                return false;
        }
    }
    
    void CompressionEngine::setThreadPoolSize(size_t threads) {
        threadPoolSize_ = threads;
    }
    
    std::pair<CompressionAlgorithm, int> CompressionEngine::optimizeSettings(
        const std::vector<uint8_t>& data,
        double targetRatio) {
        
        double compressibility = isDataCompressible(data) ? 0.8 : 0.3;
        
        if (compressibility > 0.7 && targetRatio < 0.3) {
            return {CompressionAlgorithm::Zstd, 5}; // Best compression for highly compressible data
        } else if (compressibility > 0.5) {
            return {CompressionAlgorithm::Zstd, 3}; // Balanced compression
        } else {
            return {CompressionAlgorithm::Lz4, 1}; // Fast compression for less compressible data
        }
    }
    
    std::vector<CompressionStats> CompressionEngine::batchCompress(
        const std::vector<std::vector<uint8_t>>& dataChunks,
        CompressionAlgorithm algorithm,
        int level) {
        
        std::vector<CompressionStats> stats;
        stats.reserve(dataChunks.size());
        
        for (const auto& chunk : dataChunks) {
            stats.push_back(analyzeCompression(chunk, algorithm, level));
        }
        
        return stats;
    }
    
    std::string CompressionEngine::getVersion() {
        // In a real implementation, this would return actual library versions
        return "NLM Compression Engine v1.0 (gzip, zstd, lz4, zlib stubs)";
    }
    
    // Gzip implementations (stubs)
    std::vector<uint8_t> CompressionEngine::compressGzip(
        const std::vector<uint8_t>& data, int level) {
        std::vector<uint8_t> result;
        result.reserve(data.size() + 100); // Estimate with some overhead
        
        // Simple run-length encoding as a stub implementation
        // In real implementation, use zlib/gzip library
        result = data; // No compression for now (stub)
        
        return result;
    }
    
    std::vector<uint8_t> CompressionEngine::decompressGzip(
        const std::vector<uint8_t>& data, uint64_t originalSize) {
        return data; // No decompression for now (stub)
    }
    
    // Zstd implementations (stubs)
    std::vector<uint8_t> CompressionEngine::compressZstd(
        const std::vector<uint8_t>& data, int level) {
        std::vector<uint8_t> result;
        result.reserve(data.size() * 0.8); // Estimate 20% compression
        
        // Simple compression stub
        result = data; // No compression for now (stub)
        
        return result;
    }
    
    std::vector<uint8_t> CompressionEngine::decompressZstd(
        const std::vector<uint8_t>& data, uint64_t originalSize) {
        return data; // No decompression for now (stub)
    }
    
    // Lz4 implementations (stubs)
    std::vector<uint8_t> CompressionEngine::compressLz4(
        const std::vector<uint8_t>& data, int level) {
        std::vector<uint8_t> result;
        result.reserve(data.size()); // Lz4 is typically size-compatible
        
        // Simple compression stub
        result = data; // No compression for now (stub)
        
        return result;
    }
    
    std::vector<uint8_t> CompressionEngine::decompressLz4(
        const std::vector<uint8_t>& data, uint64_t originalSize) {
        return data; // No decompression for now (stub)
    }
    
    // Zlib implementations (stubs)
    std::vector<uint8_t> CompressionEngine::compressZlib(
        const std::vector<uint8_t>& data, int level) {
        std::vector<uint8_t> result;
        result.reserve(data.size() * 0.9); // Estimate 10% compression
        
        // Simple compression stub
        result = data; // No compression for now (stub)
        
        return result;
    }
    
    std::vector<uint8_t> CompressionEngine::decompressZlib(
        const std::vector<uint8_t>& data, uint64_t originalSize) {
        return data; // No decompression for now (stub)
    }
    
    // Delta compression implementations
    std::vector<uint8_t> CompressionEngine::createDelta(
        const std::vector<uint8_t>& current,
        const std::vector<uint8_t>& base) {
        
        std::vector<uint8_t> delta;
        
        if (current.size() != base.size()) {
            // Different sizes - store size and both full data
            delta.push_back(0); // Flag for full data
            delta.insert(delta.end(), current.begin(), current.end());
        } else {
            // Same size - compute differences
            delta.push_back(1); // Flag for delta
            size_t diffCount = 0;
            
            for (size_t i = 0; i < current.size(); ++i) {
                if (current[i] != base[i]) {
                    delta.push_back(static_cast<uint8_t>(i));
                    delta.push_back(current[i]);
                    diffCount++;
                }
            }
            
            // If too many differences, store full data instead
            if (diffCount > current.size() / 2) {
                delta.clear();
                delta.push_back(0); // Flag for full data
                delta.insert(delta.end(), current.begin(), current.end());
            }
        }
        
        return delta;
    }
    
    std::vector<uint8_t> CompressionEngine::applyDeltaToBuffer(
        std::vector<uint8_t>& buffer,
        const std::vector<uint8_t>& delta) {
        
        if (delta.empty()) {
            return buffer;
        }
        
        uint8_t flag = delta[0];
        
        if (flag == 0) {
            // Full data replacement
            std::vector<uint8_t> result(delta.begin() + 1, delta.end());
            return result;
        } else {
            // Delta application
            std::vector<uint8_t> result = buffer;
            size_t pos = 1;
            
            while (pos + 1 < delta.size()) {
                size_t index = delta[pos];
                uint8_t value = delta[pos + 1];
                if (index < result.size()) {
                    result[index] = value;
                }
                pos += 2;
            }
            
            return result;
        }
    }
    
    // Compression utilities
    uint64_t CompressionEngine::crc64(const void* data, size_t size) {
        // Simple CRC-64 implementation
        const uint64_t crc_table[256] = {0};
        uint64_t crc = 0xFFFFFFFFFFFFFFFFULL;
        
        for (size_t i = 0; i < size; ++i) {
            uint8_t index = (crc ^ ((const uint8_t*)data)[i]) & 0xFF;
            crc = crc_table[index] ^ (crc >> 8);
        }
        
        return crc ^ 0xFFFFFFFFFFFFFFFFULL;
    }
    
    bool CompressionEngine::isDataCompressible(const std::vector<uint8_t>& data) {
        if (data.size() < 100) {
            return false; // Too small to determine compressibility
        }
        
        // Simple heuristic: check for repeating patterns
        size_t patternCount = 0;
        size_t patternLength = 4;
        
        for (size_t i = 0; i + patternLength * 2 < data.size(); i += patternLength) {
            bool isPattern = true;
            for (size_t j = 0; j < patternLength; ++j) {
                if (data[i + j] != data[i + j + patternLength]) {
                    isPattern = false;
                    break;
                }
            }
            if (isPattern) {
                patternCount++;
            }
        }
        
        return (static_cast<double>(patternCount) / (data.size() / patternLength)) > 0.1;
    }
    
} // namespace nlm