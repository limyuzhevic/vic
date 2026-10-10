// Checkpoint compressor implementation
#include "CheckpointCompressor.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

CheckpointCompressor::CheckpointCompressor() : tempDir_(), config_(nullptr),
                                              algorithm_(CompressionAlgorithm::Zlib),
                                              level_(CompressionLevel::Balanced),
                                              errorCount_(0), stats_(), compressionEventCallback_(nullptr) {
}

CheckpointCompressor::~CheckpointCompressor() {
    shutdown();
}

void CheckpointCompressor::configure(const std::string& tempDir, const std::shared_ptr<Config>& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    tempDir_ = tempDir;
    config_ = config;
    
    NLM_LOG_INFO("Checkpoint compressor configured successfully");
}

bool CheckpointCompressor::compress(const void* data, uint64_t dataSize, std::vector<uint8_t>& compressedData,
                                    CompressionLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    CompressionResult result;
    result.success = false;
    
    try {
        // Execute compression based on algorithm
        bool success = false;
        switch (algorithm_) {
            case CompressionAlgorithm::Zlib:
                success = compressWithZlib(data, dataSize, compressedData, level);
                result.algorithm = "zlib";
                break;
            case CompressionAlgorithm::LZ4:
                success = compressWithLz4(data, dataSize, compressedData, level);
                result.algorithm = "lz4";
                break;
            case CompressionAlgorithm::Snappy:
                success = compressWithSnappy(data, dataSize, compressedData, level);
                result.algorithm = "snappy";
                break;
            case CompressionAlgorithm::Zstd:
                success = compressWithZstd(data, dataSize, compressedData, level);
                result.algorithm = "zstd";
                break;
            default:
                success = compressWithZlib(data, dataSize, compressedData, level);
                result.algorithm = "zlib";
                break;
        }
        
        if (!success) {
            errorCount_++;
            result.message = "Compression failed with algorithm: " + result.algorithm;
            logEvent("compress", "", result);
            return false;
        }
        
        result.success = true;
        result.originalSize = dataSize;
        result.compressedSize = compressedData.size();
        result.compressionRatio = (result.originalSize > 0) ? 
            (1.0 - static_cast<double>(result.compressedSize) / result.originalSize) * 100.0 : 0.0;
        
        updateStats(result);
        logEvent("compress", "", result);
        
        NLM_LOG_INFO("Data compressed successfully with " + result.algorithm + ": " + 
                    std::to_string(result.compressionRatio) + "% ratio");
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception during compression: ") + e.what();
        logEvent("compress", "", result);
        return false;
    }
}

bool CheckpointCompressor::compressFile(const std::string& inputPath, const std::string& outputPath,
                                         CompressionLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    CompressionResult result;
    result.success = false;
    
    try {
        // Read input file
        std::ifstream inputFile(inputPath, std::ios::binary);
        if (!inputFile.is_open()) {
            result.message = "Cannot open input file: " + inputPath;
            logEvent("compress_file", inputPath, result);
            return false;
        }
        
        inputFile.seekg(0, std::ios::end);
        uint64_t fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> fileData(fileSize);
        inputFile.read(reinterpret_cast<char*>(fileData.data()), fileSize);
        inputFile.close();
        
        // Compress data
        std::vector<uint8_t> compressedData;
        if (!compress(fileData.data(), fileSize, compressedData, level)) {
            return false;
        }
        
        // Write compressed data to output file
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile.is_open()) {
            result.message = "Cannot open output file for writing: " + outputPath;
            logEvent("compress_file", inputPath, result);
            return false;
        }
        
        outputFile.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
        outputFile.close();
        
        result.success = true;
        result.originalSize = fileSize;
        result.compressedSize = compressedData.size();
        result.compressionRatio = (result.originalSize > 0) ? 
            (1.0 - static_cast<double>(result.compressedSize) / result.originalSize) * 100.0 : 0.0;
        
        updateStats(result);
        logEvent("compress_file", inputPath, result);
        
        NLM_LOG_INFO("File compressed successfully: " + inputPath + " -> " + outputPath + 
                    " (" + std::to_string(result.compressionRatio) + "%)");
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception compressing file: ") + e.what();
        logEvent("compress_file", inputPath, result);
        return false;
    }
}

bool CheckpointCompressor::decompress(const void* compressedData, uint64_t compressedSize,
                                       std::vector<uint8_t>& decompressedData) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    CompressionResult result;
    result.success = false;
    
    try {
        // Execute decompression based on algorithm
        bool success = false;
        switch (algorithm_) {
            case CompressionAlgorithm::Zlib:
                success = decompressWithZlib(compressedData, compressedSize, decompressedData);
                result.algorithm = "zlib";
                break;
            case CompressionAlgorithm::LZ4:
                success = decompressWithLz4(compressedData, compressedSize, decompressedData);
                result.algorithm = "lz4";
                break;
            case CompressionAlgorithm::Snappy:
                success = decompressWithSnappy(compressedData, compressedSize, decompressedData);
                result.algorithm = "snappy";
                break;
            case CompressionAlgorithm::Zstd:
                success = decompressWithZstd(compressedData, compressedSize, decompressedData);
                result.algorithm = "zstd";
                break;
            default:
                success = decompressWithZlib(compressedData, compressedSize, decompressedData);
                result.algorithm = "zlib";
                break;
        }
        
        if (!success) {
            errorCount_++;
            result.message = "Decompression failed with algorithm: " + result.algorithm;
            logEvent("decompress", "", result);
            return false;
        }
        
        result.success = true;
        result.originalSize = decompressedData.size();
        result.compressedSize = compressedSize;
        result.compressionRatio = (result.compressedSize > 0) ? 
            (1.0 - static_cast<double>(result.originalSize) / result.compressedSize) * 100.0 : 0.0;
        
        updateStats(result);
        logEvent("decompress", "", result);
        
        NLM_LOG_INFO("Data decompressed successfully with " + result.algorithm + ": " + 
                    std::to_string(result.originalSize) + " bytes");
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception during decompression: ") + e.what();
        logEvent("decompress", "", result);
        return false;
    }
}

bool CheckpointCompressor::decompressFile(const std::string& inputPath, const std::string& outputPath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    CompressionResult result;
    result.success = false;
    
    try {
        // Read compressed input file
        std::ifstream inputFile(inputPath, std::ios::binary);
        if (!inputFile.is_open()) {
            result.message = "Cannot open compressed input file: " + inputPath;
            logEvent("decompress_file", inputPath, result);
            return false;
        }
        
        inputFile.seekg(0, std::ios::end);
        uint64_t fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);
        
        std::vector<uint8_t> compressedData(fileSize);
        inputFile.read(reinterpret_cast<char*>(compressedData.data()), fileSize);
        inputFile.close();
        
        // Decompress data
        std::vector<uint8_t> decompressedData;
        if (!decompress(compressedData.data(), compressedData.size(), decompressedData)) {
            return false;
        }
        
        // Write decompressed data to output file
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile.is_open()) {
            result.message = "Cannot open output file for writing: " + outputPath;
            logEvent("decompress_file", inputPath, result);
            return false;
        }
        
        outputFile.write(reinterpret_cast<const char*>(decompressedData.data()), decompressedData.size());
        outputFile.close();
        
        result.success = true;
        result.originalSize = decompressedData.size();
        result.compressedSize = fileSize;
        result.compressionRatio = (result.compressedSize > 0) ? 
            (1.0 - static_cast<double>(result.originalSize) / result.compressedSize) * 100.0 : 0.0;
        
        updateStats(result);
        logEvent("decompress_file", inputPath, result);
        
        NLM_LOG_INFO("File decompressed successfully: " + inputPath + " -> " + outputPath + 
                    " (" + std::to_string(result.originalSize) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        errorCount_++;
        result.message = std::string("Exception decompressing file: ") + e.what();
        logEvent("decompress_file", inputPath, result);
        return false;
    }
}

void CheckpointCompressor::updateStats(const CompressionResult& result) {
    stats_.totalOperations++;
    
    if (result.success) {
        stats_.successfulOperations++;
        stats_.totalCompressedSize += result.compressedSize;
        
        if (stats_.totalOperations > 0) {
            stats_.averageCompressionRatio = 
                static_cast<double>(stats_.totalCompressedSize) / stats_.totalOperations;
        }
    } else {
        stats_.failedOperations++;
    }
}

void CheckpointCompressor::logEvent(const std::string& operation, const std::string& sourcePath,
                                     const CompressionResult& result) {
    std::stringstream logMsg;
    logMsg << "[CheckpointCompressor] " << operation;
    
    if (!sourcePath.empty()) {
        logMsg << " - Source: " << sourcePath;
    }
    
    if (!result.message.empty()) {
        logMsg << " - " << result.message;
    }
    
    if (result.success) {
        NLM_LOG_INFO(logMsg.str());
    } else {
        NLM_LOG_ERROR(logMsg.str());
    }
    
    // Call event callback if registered
    if (compressionEventCallback_) {
        compressionEventCallback_(operation, result);
    }
}

bool CheckpointCompressor::initialize() {
    return true;
}

void CheckpointCompressor::cleanup() {
    checkpointReader_.reset();
    checkpointWriter_.reset();
}

bool CheckpointCompressor::compressWithZlib(const void* data, uint64_t dataSize, std::vector<uint8_t>& compressedData,
                                            CompressionLevel level) {
    // Simple zlib simulation - in real implementation would use actual zlib
    // For demonstration, just reduce size slightly by removing some bytes
    compressedData.resize(dataSize / 2); // 50% reduction
    for (uint64_t i = 0; i < compressedData.size(); ++i) {
        compressedData[i] = static_cast<uint8_t>((i * 137) % 256); // Simple deterministic compression
    }
    return true;
}

bool CheckpointCompressor::compressWithLz4(const void* data, uint64_t dataSize, std::vector<uint8_t>& compressedData,
                                           CompressionLevel level) {
    // Simple LZ4 simulation - in real implementation would use actual LZ4
    compressedData.resize(dataSize * 0.6); // 40% reduction
    return true;
}

bool CheckpointCompressor::compressWithSnappy(const void* data, uint64_t dataSize, std::vector<uint8_t>& compressedData,
                                              CompressionLevel level) {
    // Simple Snappy simulation - in real implementation would use actual Snappy
    compressedData.resize(dataSize * 0.7); // 30% reduction
    return true;
}

bool CheckpointCompressor::compressWithZstd(const void* data, uint64_t dataSize, std::vector<uint8_t>& compressedData,
                                            CompressionLevel level) {
    // Simple Zstd simulation - in real implementation would use actual Zstd
    compressedData.resize(dataSize * 0.8); // 20% reduction
    return true;
}

bool CheckpointCompressor::decompressWithZlib(const void* compressedData, uint64_t compressedSize,
                                              std::vector<uint8_t>& decompressedData) {
    // Simple zlib decompression simulation
    decompressedData.resize(compressedSize * 2); // Double size to simulate decompression
    return true;
}

bool CheckpointCompressor::decompressWithLz4(const void* compressedData, uint64_t compressedSize,
                                             std::vector<uint8_t>& decompressedData) {
    // Simple LZ4 decompression simulation
    decompressedData.resize(compressedSize * 1.5); // 1.5x size to simulate decompression
    return true;
}

bool CheckpointCompressor::decompressWithSnappy(const void* compressedData, uint64_t compressedSize,
                                                std::vector<uint8_t>& decompressedData) {
    // Simple Snappy decompression simulation
    decompressedData.resize(compressedSize * 1.8); // 1.8x size to simulate decompression
    return true;
}

bool CheckpointCompressor::decompressWithZstd(const void* compressedData, uint64_t compressedSize,
                                              std::vector<uint8_t>& decompressedData) {
    // Simple Zstd decompression simulation
    decompressedData.resize(compressedSize * 1.6); // 1.6x size to simulate decompression
    return true;
}

} // namespace nlm
