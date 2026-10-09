// CheckpointSystem Implementation
// This file provides the implementations for CheckpointReader and CheckpointWriter
// including support for all new checkpoint sections

#include "CheckpointSystem.hpp"
#include <stdexcept>
#include <algorithm>
#include <cstring>

namespace nlm {

// Helper functions
static uint64_t swapBytes(uint64_t value) {
    return ((value & 0xFF00000000000000ULL) >> 56) |
           ((value & 0x00FF000000000000ULL) >> 40) |
           ((value & 0x0000FF0000000000ULL) >> 24) |
           ((value & 0x000000FF00000000ULL) >> 8) |
           ((value & 0x00000000FF000000ULL) << 8) |
           ((value & 0x0000000000FF0000ULL) << 24) |
           ((value & 0x000000000000FF00ULL) << 40) |
           ((value & 0x00000000000000FFULL) << 56);
}

static uint32_t swapBytes(uint32_t value) {
    return ((value & 0xFF000000) >> 24) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x000000FF) << 24);
}

// Checksum implementation
inline uint64_t ChecksumCalculator::crc64(const void* data, size_t size) {
    static const uint64_t table[256] = { /* precomputed */ };
    uint64_t crc = 0xFFFFFFFFFFFFFFFF;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        uint8_t idx = (crc ^ bytes[i]) & 0xFF;
        crc = table[idx] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFFFFFFFFFF;
}

inline uint32_t ChecksumCalculator::crc32(const void* data, size_t size) {
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    
    return ~crc;
}

inline uint64_t ChecksumCalculator::fletcher64(const uint64_t* data, size_t count) {
    uint64_t sum1 = 0, sum2 = 0;
    for (size_t i = 0; i < count; ++i) {
        sum1 = (sum1 + data[i]) % 0xFFFFFFFFFFFFFFFF;
        sum2 = (sum2 + sum1) % 0xFFFFFFFFFFFFFFFF;
    }
    return (sum2 << 32) | sum1;
}

inline uint64_t ChecksumCalculator::combine(uint64_t a, uint64_t b) {
    // XOR combine for incremental checksums
    return a ^ (b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2));
}

// CheckpointReader implementation
CheckpointReader::CheckpointReader() : error_(), streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        stream_.open(filepath, std::ios::in | std::ios::binary);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint header";
            close();
            return false;
        }
        
        // Validate header
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header";
            close();
            return false;
        }
        
        // Read section headers
        sections_.clear();
        SectionHeader sectionHeader;
        
        // Read number of sections (from end of file or header)
        // For simplicity, we'll read until end of file
        stream_.seekg(0, std::ios::end);
        size_t fileSize = stream_.tellg();
        stream_.seekg(0, std::ios::beg);
        
        while (stream_.tellg() < fileSize - sizeof(SectionHeader)) {
            stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(sectionHeader));
            if (!stream_.good()) break;
            
            // Validate section header
            if (sectionHeader.offset >= fileSize || sectionHeader.size > fileSize - sectionHeader.offset) {
                error_ = "Invalid section header in checkpoint";
                close();
                return false;
            }
            
            sections_.push_back(sectionHeader);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception opening checkpoint file: " + std::string(e.what());
        close();
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> data;
    
    // Find section with specified type
    auto sectionIt = std::find_if(sections_.begin(), sections_.end(),
                                  [sectionType](const SectionHeader& header) {
                                      return header.type == static_cast<uint32_t>(sectionType);
                                  });
    
    if (sectionIt == sections_.end()) {
        error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
        return data;
    }
    
    const SectionHeader& sectionHeader = *sectionIt;
    
    try {
        // Seek to section
        stream_.seekg(sectionHeader.offset, std::ios::beg);
        
        // Read section data
        if (sectionHeader.compressedSize > 0) {
            // Decompress section
            std::vector<uint8_t> compressedData(sectionHeader.compressedSize);
            stream_.read(reinterpret_cast<char*>(compressedData.data()), compressedData.size());
            
            if (!stream_.good()) {
                error_ = "Failed to read compressed section data";
                return data;
            }
            
            // TODO: Implement decompression
            // For now, assume data is not compressed
            data = compressedData;
        } else {
            // Read uncompressed data
            data.resize(sectionHeader.size);
            stream_.read(reinterpret_cast<char*>(data.data()), data.size());
            
            if (!stream_.good()) {
                error_ = "Failed to read section data";
                return std::vector<uint8_t>();
            }
        }
        
        return data;
        
    } catch (const std::exception& e) {
        error_ = "Exception reading section: " + std::string(e.what());
        return std::vector<uint8_t>();
    }
}

bool CheckpointReader::validate() const {
    if (!isOpen()) {
        error_ = "Checkpoint file is not open";
        return false;
    }
    
    // Validate header
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Validate section headers
    for (const auto& section : sections_) {
        if (section.type >= static_cast<uint32_t>(CheckpointSection::End)) {
            error_ = "Invalid section type in checkpoint";
            return false;
        }
        
        if (section.offset >= header_.totalSize || section.size > header_.totalSize - section.offset) {
            error_ = "Invalid section size in checkpoint";
            return false;
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (!isOpen() || sections_.empty()) {
        return 0.0;
    }
    
    // Calculate progress based on current position
    // For simplicity, we'll return a fixed progress
    return 1.0; // Assume complete
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        stream_.open(filepath, std::ios::out | std::ios::binary);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        compression_ = compression;
        filepath_ = filepath;
        
        // Initialize header
        header_.magic = CHECKPOINT_MAGIC;
        header_.majorVersion = CHECKPOINT_VERSION_MAJOR;
        header_.minorVersion = CHECKPOINT_VERSION_MINOR;
        header_.patchVersion = CHECKPOINT_VERSION_PATCH;
        header_.timestamp = 0; // TODO: Get actual timestamp
        header_.totalSize = 0;
        header_.neuronCount = 0;
        header_.synapseCount = 0;
        header_.regionCount = 0;
        header_.step = 0;
        header_.simulationTime = 0.0;
        header_.checksum = 0;
        header_.compressionLevel = static_cast<uint32_t>(compression_);
        header_.flags = 0;
        
        // Write header (placeholder)
        if (!writeHeader()) {
            error_ = "Failed to write checkpoint header";
            close();
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception creating checkpoint file: " + std::string(e.what());
        abort();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // Finalize checkpoint
        if (!finalize()) {
            // Error already logged
        }
        stream_.close();
    }
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount, 
                    uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        if (!isOpen()) {
            error_ = "Checkpoint file is not open";
            return false;
        }
        
        if (!data || size == 0) {
            error_ = "Invalid section data";
            return false;
        }
        
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
        sectionHeader.size = size;
        sectionHeader.compressedSize = 0; // TODO: Implement compression
        sectionHeader.checksum = 0; // TODO: Calculate checksum
        
        // Write section header
        if (!writeSectionHeader(sectionHeader)) {
            error_ = "Failed to write section header";
            return false;
        }
        
        // Write section data
        stream_.write(reinterpret_cast<const char*>(data), size);
        if (!stream_.good()) {
            error_ = "Failed to write section data";
            return false;
        }
        
        // Update total size
        header_.totalSize = static_cast<uint64_t>(stream_.tellp());
        bytesWritten_ += size;
        
        // Store section header for later finalization
        sections_.push_back(sectionHeader);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception writing section: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::finalize() {
    if (!isOpen()) {
        error_ = "Checkpoint file is not open";
        return false;
    }
    
    try {
        // Pad to alignment
        if (!padToAlignment(8)) {
            error_ = "Failed to pad to alignment";
            return false;
        }
        
        // Update total size
        header_.totalSize = static_cast<uint64_t>(stream_.tellp());
        
        // Calculate checksum
        header_.checksum = ChecksumCalculator::crc32(&header_, sizeof(header_));
        
        // Write updated header
        stream_.seekp(0, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        
        if (!stream_.good()) {
            error_ = "Failed to write final checkpoint header";
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception finalizing checkpoint: " + std::string(e.what());
        return false;
    }
}

void CheckpointWriter::abort() {
    if (isOpen()) {
        stream_.close();
        // Remove partial file
        std::remove(filepath_.c_str());
    }
}

bool CheckpointWriter::writeHeader() {
    try {
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        if (!stream_.good()) {
            error_ = "Failed to write checkpoint header";
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        error_ = "Exception writing header: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    try {
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
        if (!stream_.good()) {
            error_ = "Failed to write section header";
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        error_ = "Exception writing section header: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    try {
        size_t currentPos = static_cast<size_t>(stream_.tellp());
        size_t padding = (alignment - (currentPos % alignment)) % alignment;
        
        if (padding > 0) {
            std::vector<uint8_t> paddingData(padding, 0);
            stream_.write(reinterpret_cast<const char*>(paddingData.data()), paddingData.size());
            if (!stream_.good()) {
                error_ = "Failed to pad to alignment";
                return false;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        error_ = "Exception padding to alignment: " + std::string(e.what());
        return false;
    }
}

// Initialize static instances
const std::string CheckpointReader::DEFAULT_CHECKPOINT_DIR = "./checkpoints";
const std::string CheckpointWriter::DEFAULT_CHECKPOINT_DIR = "./checkpoints";

} // namespace nlm