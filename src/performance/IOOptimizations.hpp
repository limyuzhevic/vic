#pragma once

/**
 * I/O Optimizations
 * 
 * Comprehensive I/O optimization strategies for neural simulation:
 * 
 * I/O Optimization Categories:
 * 1. Buffered I/O for large datasets
 * 2. Asynchronous I/O operations
 * 3. Data compression for storage/transmission
 * 4. File format optimization
 * 5. Network I/O optimization
 * 6. Memory-mapped I/O
 * 7. Parallel I/O operations
 * 
 * Integration:
 * - Works with NLM I/O infrastructure
 * - Provides optimized file operations
 * - Supports compression/decompression
 * - Implements efficient serialization
 */

#include <cstddef>
#include <cstdint>
#include <cstring> // For strcpy
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include <chrono>
#include <filesystem>

namespace nlm {

/**
 * Buffered I/O for efficient file operations
 */
class BufferedIO {
public:
    struct Buffer {
        std::vector<char> data;
        size_t position;
        
        Buffer() : position(0) {}
        Buffer(size_t size) : data(size), position(0) {}
        
        size_t read(char* buffer, size_t bytes) {
            size_t bytesToRead = std::min(bytes, data.size() - position);
            std::copy(data.begin() + position, data.begin() + position + bytesToRead, buffer);
            position += bytesToRead;
            return bytesToRead;
        }
        
        void write(const char* buffer, size_t bytes) {
            if (position + bytes > data.size()) {
                data.resize(position + bytes);
            }
            std::copy(buffer, buffer + bytes, data.begin() + position);
            position += bytes;
        }
        
        void seek(size_t pos) {
            position = std::min(pos, data.size());
        }
        
        size_t tell() const { return position; }
        size_t size() const { return data.size(); }
    };
    
    /**
     * Read file with buffering
     */
    static std::string readFileBuffered(const std::string& filename, size_t bufferSize = 64 * 1024) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::string content;
        content.reserve(fileSize);
        
        std::vector<char> buffer(bufferSize);
        while (file) {
            file.read(buffer.data(), bufferSize);
            content.append(buffer.data(), file.gcount());
        }
        
        return content;
    }
    
    /**
     * Write file with buffering
     */
    static void writeFileBuffered(const std::string& filename, const std::string& content,
                                  size_t bufferSize = 64 * 1024) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        const char* data = content.data();
        size_t remaining = content.size();
        
        while (remaining > 0) {
            size_t toWrite = std::min(remaining, bufferSize);
            file.write(data, toWrite);
            data += toWrite;
            remaining -= toWrite;
        }
    }
};

/**
 * Data compression for I/O optimization
 */
class DataCompressor {
public:
    enum class CompressionAlgorithm {
        None,
        Zlib,
        LZ4,
        Zstd,
        Brotli
    };
    
    /**
     * Compress data with specified algorithm
     */
    static std::vector<char> compress(const std::vector<char>& data, 
                                     CompressionAlgorithm algorithm = CompressionAlgorithm::Zlib);
    
    /**
     * Decompress data
     */
    static std::vector<char> decompress(const std::vector<char>& compressedData,
                                        CompressionAlgorithm algorithm = CompressionAlgorithm::Zlib);
    
    /**
     * Write compressed data to file
     */
    static void writeCompressedFile(const std::string& filename, const std::vector<char>& data,
                                    CompressionAlgorithm algorithm = CompressionAlgorithm::Zlib);
    
    /**
     * Read compressed data from file
     */
    static std::vector<char> readCompressedFile(const std::string& filename,
                                                CompressionAlgorithm algorithm = CompressionAlgorithm::Zlib);
    
    /**
     * Get compression ratio
     */
    static double getCompressionRatio(const std::vector<char>& original,
                                      const std::vector<char>& compressed) {
        if (compressed.empty()) return 0.0;
        return static_cast<double>(original.size()) / compressed.size();
    }
};

/**
 * Asynchronous I/O operations
 */
class AsyncIO {
public:
    /**
     * Asynchronous file read
     */
    class AsyncRead {
    public:
        AsyncRead(const std::string& filename, size_t bufferSize = 64 * 1024)
            : filename_(filename), bufferSize_(bufferSize), completed_(false) {}
        
        /**
         * Start asynchronous read
         */
        void start() {
            // In real implementation, would use threads or async I/O
            data_ = BufferedIO::readFileBuffered(filename_, bufferSize_);
            completed_ = true;
        }
        
        /**
         * Check if read is completed
         */
        bool isCompleted() const { return completed_; }
        
        /**
         * Get read data
         */
        const std::string& getData() const { return data_; }
        
    private:
        std::string filename_;
        size_t bufferSize_;
        std::string data_;
        bool completed_;
    };
    
    /**
     * Asynchronous file write
     */
    class AsyncWrite {
    public:
        AsyncWrite(const std::string& filename, const std::string& data,
                   size_t bufferSize = 64 * 1024)
            : filename_(filename), data_(data), bufferSize_(bufferSize), completed_(false) {}
        
        /**
         * Start asynchronous write
         */
        void start() {
            // In real implementation, would use threads or async I/O
            BufferedIO::writeFileBuffered(filename_, data_, bufferSize_);
            completed_ = true;
        }
        
        /**
         * Check if write is completed
         */
        bool isCompleted() const { return completed_; }
        
    private:
        std::string filename_;
        std::string data_;
        size_t bufferSize_;
        bool completed_;
    };
    
    /**
     * Batch asynchronous operations
     */
    static void executeBatch(const std::vector<std::function<void()>>& operations) {
        // In real implementation, would use thread pool or async I/O
        for (const auto& op : operations) {
            op();
        }
    }
};

/**
 * Memory-mapped I/O for large files
 */
class MemoryMappedIO {
public:
    /**
     * Memory-map a file for reading
     */
    static std::unique_ptr<char[]> mapFileRead(const std::string& filename,
                                               size_t& mappedSize);
    
    /**
     * Unmap file
     */
    static void unmapFile(std::unique_ptr<char[]>& mappedData);
    
    /**
     * Memory-map a file for reading/writing
     */
    static std::unique_ptr<char[]> mapFileReadWrite(const std::string& filename,
                                                    size_t& mappedSize);
};

/**
 * File format optimization for neural data
 */
class NeuralFileFormat {
public:
    struct NeuralFileHeader {
        uint32_t magicNumber;
        uint32_t version;
        uint64_t fileSize;
        uint64_t dataSize;
        uint64_t metadataSize;
        uint32_t neuronCount;
        uint32_t synapseCount;
        uint32_t stepCount;
        char compressionType[16];
        char formatName[64];
        
        NeuralFileHeader() : magicNumber(0x4E4C4D4F), version(1), fileSize(0), dataSize(0),
                            metadataSize(0), neuronCount(0), synapseCount(0), stepCount(0) {
            strcpy(compressionType, "none");
            strcpy(formatName, "NLM_Neural_Data");
        }
    };
    
    /**
     * Write neural data to optimized file format
     */
    static void writeNeuralFile(const std::string& filename,
                                const std::vector<std::vector<float>>& neuronData,
                                const std::vector<std::vector<float>>& synapseData,
                                const std::vector<uint64_t>& metadata);
    
    /**
     * Read neural data from optimized file format
     */
    static std::tuple<std::vector<std::vector<float>>, std::vector<std::vector<float>>, std::vector<uint64_t>>
        readNeuralFile(const std::string& filename);
    
    /**
     * Write checkpoint data
     */
    static void writeCheckpoint(const std::string& filename,
                                const std::vector<std::vector<float>>& state,
                                uint64_t stepNumber);
    
    /**
     * Read checkpoint data
     */
    static std::vector<std::vector<float>> readCheckpoint(const std::string& filename,
                                                         uint64_t stepNumber = 0);
    
    /**
     * Get file extension for neural data
     */
    static std::string getFileExtension() { return ".nlmdata"; }
    
    /**
     * Get file extension for checkpoints
     */
    static std::string getCheckpointExtension() { return ".nlmckpt"; }
};

/**
 * I/O efficiency analyzer
 */
class IOEfficiencyAnalyzer {
public:
    struct IOStats {
        uint64_t readBytes;
        uint64_t writeBytes;
        uint64_t readTimeNs;
        uint64_t writeTimeNs;
        uint64_t cacheHits;
        uint64_t cacheMisses;
        double readThroughputMBps;
        double writeThroughputMBps;
        double compressionRatio;
        
        IOStats() : readBytes(0), writeBytes(0), readTimeNs(0), writeTimeNs(0),
                   cacheHits(0), cacheMisses(0), readThroughputMBps(0.0),
                   writeThroughputMBps(0.0), compressionRatio(0.0) {}
    };
    
    /**
     * Analyze I/O performance
     */
    static IOStats analyzeFileIO(const std::string& filename, bool read = true);
    
    /**
     * Calculate I/O efficiency score
     */
    static double calculateIOEfficiency(const IOStats& stats) {
        if (stats.readTimeNs == 0 || stats.writeTimeNs == 0) return 0.0;
        
        double readEfficiency = stats.readThroughputMBps > 0.0 ? 1.0 / stats.readThroughputMBps : 0.0;
        double writeEfficiency = stats.writeThroughputMBps > 0.0 ? 1.0 / stats.writeThroughputMBps : 0.0;
        double cacheEfficiency = static_cast<double>(stats.cacheHits) / (stats.cacheHits + stats.cacheMisses);
        
        return readEfficiency * writeEfficiency * cacheEfficiency;
    }
    
    /**
     * Optimize file access patterns
     */
    static void optimizeFileAccess(const std::string& filename, bool sequential = true);
    
    /**
     * Batch file operations for efficiency
     */
    static void batchFileOperations(const std::vector<std::string>& filenames,
                                    const std::function<void(const std::string&)>& operation);
};

} // namespace nlm