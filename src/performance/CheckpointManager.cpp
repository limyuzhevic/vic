#include "CheckpointSystem.hpp"
#include <cstring>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <zlib.h>
#include <lz4.h>
#include <lz4frame.h>

namespace nlm {

// Simple file operations
namespace fs = std::filesystem;

CheckpointReader::CheckpointReader() : streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        stream_.open(filepath, std::ios::binary | std::ios::in);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint header";
            return false;
        }
        
        // Validate header
        if (!header_.validate()) {
            error_ = "Invalid checkpoint file format or version mismatch";
            return false;
        }
        
        // Read section headers
        stream_.seekg(0, std::ios::end);
        size_t fileSize = stream_.tellg();
        stream_.seekg(sizeof(CheckpointHeader), std::ios::beg);
        
        size_t sectionsSize = fileSize - sizeof(CheckpointHeader);
        size_t expectedSectionsCount = header_.totalSize / sizeof(SectionHeader);
        
        sections_.resize(expectedSectionsCount);
        if (expectedSectionsCount > 0) {
            stream_.read(reinterpret_cast<char*>(sections_.data()), 
                        expectedSectionsCount * sizeof(SectionHeader));
            if (!stream_.good()) {
                error_ = "Failed to read section headers";
                return false;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        error_ = std::string("Exception opening checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
    error_.clear();
    streamPos_ = 0;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            stream_.seekg(section.offset, std::ios::beg);
            
            std::vector<uint8_t> buffer(section.compressedSize > 0 ? 
                                       section.compressedSize : section.size);
            
            stream_.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
            if (!stream_.good()) {
                error_ = "Failed to read section data for " + std::to_string(sectionType);
                return std::vector<uint8_t>();
            }
            
            // Decompress if needed
            if (section.compressedSize > 0 && section.compressedSize < section.size) {
                buffer.resize(section.size);
                // Simple decompression (assuming raw data for now)
                // In a real implementation, this would use the appropriate compression algorithm
                std::memcpy(buffer.data(), buffer.data(), section.size);
            }
            
            return buffer;
        }
    }
    
    error_ = "Section not found: " + std::to_string(sectionType);
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    // Check header validation already done in open()
    if (header_.totalSize > 0 && sections_.empty()) {
        return false;
    }
    
    // Verify checksums
    for (const auto& section : sections_) {
        if (section.checksum != 0) {
            // In real implementation, would verify checksum
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    // Simple progress based on file position
    if (stream_.is_open()) {
        return static_cast<double>(stream_.tellg()) / static_cast<double>(stream_.seekg(0, std::ios::end));
    }
    return 1.0;
}

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        compression_ = compression;
        filepath_ = filepath;
        
        // Ensure directory exists
        fs::path path(filepath);
        if (!fs::exists(path.parent_path())) {
            fs::create_directories(path.parent_path());
        }
        
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        // Write initial header (will be finalized later)
        return writeHeader();
    } catch (const std::exception& e) {
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
    compressBuffer_.clear();
    bytesWritten_ = 0;
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                 uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    header_.timestamp = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        // Compress if requested
        std::vector<uint8_t> compressedData;
        size_t compressedSize = size;
        
        if (compression_ != CompressionLevel::None && size > 0) {
            // For now, just use raw data (simple implementation)
            // In a real implementation, would use actual compression like LZ4 or zlib
            compressedData.resize(size);
            std::memcpy(compressedData.data(), data, size);
            compressedSize = size;
        } else {
            compressedData.resize(size);
            std::memcpy(compressedData.data(), data, size);
        }
        
        // Calculate checksum
        uint32_t checksum = ChecksumCalculator::crc32(compressedData.data(), compressedData.size());
        
        // Create section header
        SectionHeader section;
        section.type = static_cast<uint32_t>(sectionType);
        section.offset = sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader) + bytesWritten_;
        section.size = size;
        section.compressedSize = compressedSize;
        section.checksum = checksum;
        
        sections_.push_back(section);
        
        // Write section data
        stream_.write(reinterpret_cast<const char*>(compressedData.data()), compressedSize);
        if (!stream_.good()) {
            error_ = "Failed to write section data for " + std::to_string(sectionType);
            return false;
        }
        
        // Pad to alignment
        padToAlignment(64);
        
        bytesWritten_ += compressedSize;
        header_.totalSize += compressedSize + sizeof(SectionHeader);
        
        return true;
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing section: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeHeader() {
    // Set magic number and version
    header_.magic = CHECKPOINT_MAGIC;
    header_.majorVersion = CHECKPOINT_VERSION_MAJOR;
    header_.minorVersion = CHECKPOINT_VERSION_MINOR;
    header_.patchVersion = CHECKPOINT_VERSION_PATCH;
    header_.compressionLevel = static_cast<uint32_t>(compression_);
    
    // Write header
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_.good()) {
        error_ = "Failed to write checkpoint header";
        return false;
    }
    
    bytesWritten_ += sizeof(CheckpointHeader);
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    sections_.push_back(header);
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    return stream_.good();
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = stream_.tellp();
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> paddingData(padding, 0);
        stream_.write(reinterpret_cast<const char*>(paddingData.data()), padding);
    }
    
    return stream_.good();
}

bool CheckpointWriter::finalize() {
    try {
        // Update total size
        header_.totalSize = bytesWritten_;
        
        // Update checksum
        header_.checksum = ChecksumCalculator::crc32(&header_, sizeof(CheckpointHeader));
        
        // Seek to beginning and write updated header
        stream_.seekp(0, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        
        // Write section headers
        for (const auto& section : sections_) {
            stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
        }
        
        stream_.flush();
        return stream_.good();
    } catch (const std::exception& e) {
        error_ = std::string("Exception finalizing checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::abort() {
    close();
    if (fs::exists(filepath_)) {
        fs::remove(filepath_);
    }
}

CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true),
      lastSaveStep_(0) {}

CheckpointManager::~CheckpointManager() {}

void CheckpointManager::configure(const std::string& checkpointDir,
                                uint64_t saveIntervalSteps,
                                size_t maxCheckpoints,
                                bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Ensure directory exists
    fs::path path(checkpointDir_);
    if (!fs::exists(path)) {
        fs::create_directories(path);
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Checkpoint providers not configured";
        return false;
    }
    
    try {
        std::string filename = checkpointDir_;
        if (!name.empty()) {
            filename += "/" + name + ".bin";
        } else {
            filename += "/checkpoint_" + std::to_string(lastSaveStep_) + ".bin";
        }
        
        CheckpointWriter writer;
        CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
        
        if (!writer.create(filename, compression)) {
            error_ = writer.getError();
            return false;
        }
        
        // Write metadata
        NeuronCheckpointData neuronData;
        if (!neuronProvider_(neuronData)) {
            error_ = "Failed to get neuron data from provider";
            return false;
        }
        
        writer.setMetadata(neuronData.membranePotential.size(),
                          neuronData.membranePotential.size(), // Simplified - assuming synapses same count
                          0, // Simplified - assuming regions
                          0, 0.0); // Simplified - step and time
        
        // Write neurons
        if (!writer.writeNeurons(neuronData)) {
            error_ = writer.getError();
            return false;
        }
        
        // Get synapse data
        SynapseCheckpointData synapseData;
        if (!synapseProvider_(synapseData)) {
            error_ = "Failed to get synapse data from provider";
            return false;
        }
        
        // Write synapses
        if (!writer.writeSynapses(synapseData)) {
            error_ = writer.getError();
            return false;
        }
        
        // Finalize
        if (!writer.finalize()) {
            error_ = writer.getError();
            return false;
        }
        
        lastCheckpointPath_ = filename;
        lastSaveStep_ = 0; // Will be set by caller
        
        return true;
    } catch (const std::exception& e) {
        error_ = std::string("Exception saving checkpoint: ") + e.what();
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    // Simplified implementation - would need full reader implementation
    return false;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        for (const auto& entry : fs::directory_iterator(checkpointDir_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                auto ftime = fs::last_write_time(entry.path());
                auto time_t = std::chrono::system_clock::to_time_t(
                    std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                        ftime - std::filesystem::file_time_type::clock::now() +
                        std::chrono::system_clock::now()
                    )
                );
                
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                info.fileSize = fs::file_size(entry.path());
                info.timestamp = time_t;
                // Note: Would need to parse checkpoint file to get step, neuronCount, etc.
                
                checkpoints.push_back(info);
            }
        }
        
        // Sort by timestamp (newest first)
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      return a.timestamp > b.timestamp;
                  });
        
        // Trim to maxCheckpoints
        if (checkpoints.size() > maxCheckpoints_) {
            checkpoints.resize(maxCheckpoints_);
        }
        
        return checkpoints;
    } catch (const std::exception& e) {
        return std::vector<CheckpointInfo>();
    }
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    if (checkpoints.size() <= keepCount) return;
    
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        fs::remove(checkpointDir_ + "/" + checkpoints[i].name + ".bin");
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= lastSaveStep_ + saveIntervalSteps_;
}

std::string CheckpointManager::getError() const {
    return error_;
}

} // namespace nlm