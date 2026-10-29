#include "CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <chrono>

namespace nlm {

// Checksum calculator implementations
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
    return a ^ (b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2));
}

// CheckpointReader implementation
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
        if (!stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_))) {
            error_ = "Failed to read checkpoint header";
            stream_.close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header magic or version";
            stream_.close();
            return false;
        }
        
        // Read section headers
        sections_.clear();
        for (size_t i = 0; i < header_.regionCount; ++i) {
            SectionHeader sectionHeader;
            if (!stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(sectionHeader))) {
                error_ = "Failed to read section headers";
                stream_.close();
                return false;
            }
            sections_.push_back(sectionHeader);
        }
        
        streamPos_ = stream_.tellg();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception opening checkpoint: ") + e.what();
        if (stream_.is_open()) {
            stream_.close();
        }
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
            stream_.seekg(static_cast<std::streamoff>(section.offset));
            std::vector<uint8_t> data(section.size);
            if (section.compressedSize > 0 && section.compressedSize < section.size) {
                // Decompress (simplified - no actual compression implemented)
                stream_.read(reinterpret_cast<char*>(data.data()), section.compressedSize);
                // For now, assume no compression for simplicity
            } else {
                stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            }
            return data;
        }
    }
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Verify checksum
    if (header_.checksum != 0) {  // Simplified - in real implementation would compute checksum
        error_ = "Checksum validation failed";
        return false;
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    std::ifstream tempStream(stream_.rdbuf()->fd(), std::ios::binary | std::ios::in);
    uint64_t totalBytes = 0;
    for (const auto& section : sections_) {
        tempStream.seekg(static_cast<std::streamoff>(section.offset + section.size));
    }
    tempStream.seekg(0, std::ios::end);
    totalBytes = tempStream.tellg();
    
    tempStream.seekg(static_cast<std::streamoff>(streamPos_));
    uint64_t currentPos = tempStream.tellg();
    
    return static_cast<double>(currentPos) / totalBytes;
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    filepath_ = filepath;
    compression_ = compression;
    
    try {
        // Create directory if it doesn't exist
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        header_.timestamp = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        if (stream_.is_open()) {
            stream_.close();
        }
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        std::filesystem::remove(filepath_);
    }
    sections_.clear();
    compressBuffer_.clear();
}

bool CheckpointWriter::writeHeader() {
    header_.totalSize = bytesWritten_;
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    return stream_.good();
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
    return stream_.good();
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = static_cast<size_t>(stream_.tellp());
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    if (padding > 0) {
        std::vector<uint8_t> paddingBytes(padding, 0);
        stream_.write(reinterpret_cast<const char*>(paddingBytes.data()), padding);
    }
    return stream_.good();
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount, 
                                  uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    // For now, just store neuron data in memory
    // In a full implementation, this would be written as a section
    return true;
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    // For now, just store synapse data in memory
    // In a full implementation, this would be written as a section
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(sectionType);
    sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
    sectionHeader.size = size;
    sectionHeader.compressedSize = 0; // No compression for now
    sectionHeader.checksum = ChecksumCalculator::crc32(data, size);
    
    if (!writeSectionHeader(sectionHeader)) {
        return false;
    }
    
    stream_.write(reinterpret_cast<const char*>(data), size);
    sections_.push_back(sectionHeader);
    bytesWritten_ += sizeof(sectionHeader) + size;
    
    return stream_.good();
}

bool CheckpointWriter::finalize() {
    if (!writeHeader()) {
        error_ = "Failed to write checkpoint header";
        return false;
    }
    
    // Write sections in order
    bool success = true;
    for (const auto& section : sections_) {
        // Seek to section and write data (simplified - in real implementation would read from buffer)
        stream_.seekp(section.offset);
        // Data would be written here in full implementation
        stream_.seekp(0, std::ios::end); // Return to end
    }
    
    // Calculate final checksum
    // header_.checksum = calculate checksum of all sections
    
    stream_.flush();
    return stream_.good();
}

uint64_t CheckpointWriter::getBytesWritten() const {
    return bytesWritten_;
}

// CheckpointManager implementation
CheckpointManager::CheckpointManager() 
    : saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true), 
      lastSaveStep_(0), lastCheckpointPath_(""), neuronProvider_(nullptr), synapseProvider_(nullptr) {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir, 
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return false;
    }
    
    return saveImmediately();
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Checkpoint providers not set";
        return false;
    }
    
    std::string checkpointName = name.empty() ? std::to_string(lastSaveStep_) : name;
    std::string filepath = checkpointDir_ + "/checkpoint_" + checkpointName + ".nlm";
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        NeuronCheckpointData neuronData;
        if (!neuronProvider_(neuronData)) {
            error_ = "Failed to get neuron data from provider";
            return false;
        }
        
        SynapseCheckpointData synapseData;
        if (!synapseProvider_(synapseData)) {
            error_ = "Failed to get synapse data from provider";
            return false;
        }
        
        if (!writer.writeNeurons(neuronData) || !writer.writeSynapses(synapseData)) {
            error_ = "Failed to write checkpoint data";
            return false;
        }
        
        if (!writer.finalize()) {
            error_ = writer.getError();
            return false;
        }
        
        lastCheckpointPath_ = filepath;
        lastSaveStep_ = writer.getHeader().step;
        
        // Prune old checkpoints
        pruneOldCheckpoints(maxCheckpoints_);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception saving checkpoint: ") + e.what();
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/checkpoint_" + name + ".nlm";
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            error_ = "Failed to open checkpoint: " + filepath;
            return false;
        }
        
        if (!reader.validate()) {
            error_ = reader.getError();
            return false;
        }
        
        // In a full implementation, would restore state from reader
        lastCheckpointPath_ = filepath;
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception loading checkpoint: ") + e.what();
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".nlm") {
                auto ftime = std::filesystem::last_write_time(entry.path());
                auto time = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::time_point(
                        std::chrono::duration_cast<std::chrono::system_clock::duration>(
                            ftime.time_since_epoch()
                        )
                    )
                );
                
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                info.name.erase(0, info.name.find_first_of('_') + 1); // Remove "checkpoint_" prefix
                info.step = info.name.empty() ? 0 : std::stoull(info.name);
                info.fileSize = std::filesystem::file_size(entry.path());
                info.timestamp = time;
                
                checkpoints.push_back(info);
            }
        }
        
        // Sort by timestamp (newest first)
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      return a.timestamp > b.timestamp;
                  });
                  
    } catch (const std::exception& e) {
        error_ = std::string("Exception listing checkpoints: ") + e.what();
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() > keepCount) {
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      return a.timestamp > b.timestamp;
                  });
        
        size_t toRemove = checkpoints.size() - keepCount;
        for (size_t i = keepCount; i < checkpoints.size(); ++i) {
            std::filesystem::path filepath = checkpointDir_ + "/checkpoint_" + 
                (checkpoints[i].name.empty() ? "0" : checkpoints[i].name) + ".nlm";
            if (std::filesystem::exists(filepath)) {
                std::filesystem::remove(filepath);
            }
        }
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return (currentStep % saveIntervalSteps_) == 0 && currentStep > lastSaveStep_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

} // namespace nlm
