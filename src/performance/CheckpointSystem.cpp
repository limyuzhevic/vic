#include "CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <cstring>
#include <iostream>
#include <filesystem>

namespace nlm {

// CheckpointReader implementation

CheckpointReader::CheckpointReader() : stream_(), header_(), sections_(), error_(), streamPos_(0) {}

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
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
        if (stream_.fail()) {
            error_ = "Failed to read checkpoint header";
            stream_.close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header magic or version";
            stream_.close();
            return false;
        }
        
        // Read section count and sections
        uint32_t sectionCount;
        stream_.read(reinterpret_cast<char*>(&sectionCount), sizeof(sectionCount));
        if (stream_.fail()) {
            error_ = "Failed to read section count";
            stream_.close();
            return false;
        }
        
        sections_.resize(sectionCount);
        for (auto& section : sections_) {
            stream_.read(reinterpret_cast<char*>(&section), sizeof(section));
            if (stream_.fail()) {
                error_ = "Failed to read section headers";
                stream_.close();
                sections_.clear();
                return false;
            }
        }
        
        streamPos_ = stream_.tellg();
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception opening checkpoint: " + std::string(e.what());
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

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    try {
        // Find neurons section
        auto it = std::find_if(sections_.begin(), sections_.end(),
                              [](const SectionHeader& s) { return s.type == static_cast<uint32_t>(CheckpointSection::Neurons); });
        
        if (it == sections_.end()) {
            error_ = "Neurons section not found in checkpoint";
            return false;
        }
        
        const auto& section = *it;
        
        // Seek to section data
        stream_.seekg(streamPos_ + section.offset);
        
        // Read data (simplified - in real implementation would use compression handling)
        data.membranePotential.resize(section.size / (sizeof(float) * 10));  // Estimate
        
        // For simplicity, just read the known structure size
        stream_.read(reinterpret_cast<char*>(&data.membranePotential[0]), 
                    section.size > 0 ? std::min(section.size, sizeof(float) * data.membranePotential.size()) : 0);
        
        if (stream_.fail()) {
            error_ = "Failed to read neuron data from checkpoint";
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception reading neurons: " + std::string(e.what());
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    try {
        // Find synapses section
        auto it = std::find_if(sections_.begin(), sections_.end(),
                              [](const SectionHeader& s) { return s.type == static_cast<uint32_t>(CheckpointSection::Synapses); });
        
        if (it == sections_.end()) {
            error_ = "Synapses section not found in checkpoint";
            return false;
        }
        
        const auto& section = *it;
        
        // Seek to section data
        stream_.seekg(streamPos_ + section.offset);
        
        // Read data (simplified - in real implementation would use compression handling)
        data.weight.resize(section.size / (sizeof(float) * 12));  // Estimate
        
        // For simplicity, just read the known structure size
        stream_.read(reinterpret_cast<char*>(&data.weight[0]), 
                    section.size > 0 ? std::min(section.size, sizeof(float) * data.weight.size()) : 0);
        
        if (stream_.fail()) {
            error_ = "Failed to read synapse data from checkpoint";
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception reading synapses: " + std::string(e.what());
        return false;
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> result;
    
    try {
        // Find section
        auto it = std::find_if(sections_.begin(), sections_.end(),
                              [sectionType](const SectionHeader& s) { 
                                  return s.type == static_cast<uint32_t>(sectionType); 
                              });
        
        if (it == sections_.end()) {
            return result; // Return empty if not found
        }
        
        const auto& section = *it;
        
        // Seek to section data
        stream_.seekg(streamPos_ + section.offset);
        
        // Read section data
        result.resize(section.size);
        stream_.read(reinterpret_cast<char*>(result.data()), section.size);
        
    } catch (const std::exception& e) {
        error_ = "Exception reading section: " + std::string(e.what());
    }
    
    return result;
}

bool CheckpointReader::validate() const {
    // Simple validation - check that we have required sections
    bool hasNeurons = false;
    bool hasSynapses = false;
    
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(CheckpointSection::Neurons)) {
            hasNeurons = true;
        } else if (section.type == static_cast<uint32_t>(CheckpointSection::Synapses)) {
            hasSynapses = true;
        }
    }
    
    return hasNeurons && hasSynapses && !error_.empty();
}

double CheckpointReader::getProgress() const {
    // Simple progress calculation
    if (sections_.empty()) return 0.0;
    
    uint64_t totalSize = 0;
    for (const auto& section : sections_) {
        totalSize += section.size;
    }
    
    uint64_t readSize = 0;
    for (const auto& section : sections_) {
        if (section.offset + section.size <= streamPos_) {
            readSize += section.size;
        }
    }
    
    return totalSize > 0 ? static_cast<double>(readSize) / totalSize : 1.0;
}

// CheckpointWriter implementation

CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(CompressionLevel::Balanced), 
                                     header_(), sections_(), bytesWritten_(0), compressBuffer_(), error_() {}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Cleanup any existing file
        if (std::filesystem::exists(filepath)) {
            std::filesystem::remove(filepath);
        }
        
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        filepath_ = filepath;
        compression_ = compression;
        sections_.clear();
        bytesWritten_ = 0;
        
        // Initialize header
        header_.timestamp = static_cast<uint64_t>(time(nullptr));
        header_.totalSize = 0; // Will be updated when finalized
        header_.checksum = 0;
        
        // Write empty header first (will be updated during finalize)
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        if (stream_.fail()) {
            error_ = "Failed to write checkpoint header";
            stream_.close();
            return false;
        }
        
        // Write section count (placeholder)
        uint32_t sectionCount = 0;
        stream_.write(reinterpret_cast<const char*>(&sectionCount), sizeof(sectionCount));
        if (stream_.fail()) {
            error_ = "Failed to write section count";
            stream_.close();
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception creating checkpoint: " + std::string(e.what());
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
    filepath_.clear();
    sections_.clear();
    bytesWritten_ = 0;
    compressBuffer_.clear();
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
    try {
        // Convert to binary format
        std::vector<uint8_t> neuronData;
        
        // Write each vector with its size
        for (const auto& vec : {
            std::make_pair(&data.membranePotential, sizeof(float)),
            std::make_pair(&data.restingPotential, sizeof(float)),
            std::make_pair(&data.threshold, sizeof(float)),
            std::make_pair(&data.resetPotential, sizeof(float)),
            std::make_pair(&data.leakConductance, sizeof(float)),
            std::make_pair(&data.firingState, sizeof(uint8_t)),
            std::make_pair(&data.refractoryRemaining, sizeof(uint32_t)),
            std::make_pair(&data.refractoryPeriod, sizeof(uint32_t)),
            std::make_pair(&data.lastSpikeTime, sizeof(float)),
            std::make_pair(&data.neuronType, sizeof(uint64_t)),
            std::make_pair(&data.regionId, sizeof(uint64_t)),
            std::make_pair(&data.populationId, sizeof(uint64_t))
        }) {
            if (vec.first && vec.second > 0) {
                size_t count = vec.first->size();
                if (count > 0) {
                    neuronData.insert(neuronData.end(), reinterpret_cast<const uint8_t*>(vec.first->data()), 
                                    reinterpret_cast<const uint8_t*>(vec.first->data() + count) * vec.second);
                }
            }
        }
        
        return writeSection(CheckpointSection::Neurons, neuronData.data(), neuronData.size());
        
    } catch (const std::exception& e) {
        error_ = "Exception writing neurons: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Convert to binary format
        std::vector<uint8_t> synapseData;
        
        // Write each vector with its size
        for (const auto& vec : {
            std::make_pair(&data.sourceNeuron, sizeof(uint64_t)),
            std::make_pair(&data.destinationNeuron, sizeof(uint64_t)),
            std::make_pair(&data.weight, sizeof(float)),
            std::make_pair(&data.delay, sizeof(uint32_t)),
            std::make_pair(&data.synapseType, sizeof(uint8_t)),
            std::make_pair(&data.plasticityFlags, sizeof(uint8_t)),
            std::make_pair(&data.eligibilityTrace, sizeof(float)),
            std::make_pair(&data.efficacy, sizeof(float)),
            std::make_pair(&data.shortTermDepression, sizeof(float)),
            std::make_pair(&data.shortTermFacilitation, sizeof(float))
        }) {
            if (vec.first && vec.second > 0) {
                size_t count = vec.first->size();
                if (count > 0) {
                    synapseData.insert(synapseData.end(), reinterpret_cast<const uint8_t*>(vec.first->data()), 
                                    reinterpret_cast<const uint8_t*>(vec.first->data() + count) * vec.second);
                }
            }
        }
        
        return writeSection(CheckpointSection::Synapses, synapseData.data(), synapseData.size());
        
    } catch (const std::exception& e) {
        error_ = "Exception writing synapses: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        if (!isOpen()) {
            error_ = "Cannot write section: checkpoint file not open";
            return false;
        }
        
        if (!data || size == 0) {
            return true; // Empty section is OK
        }
        
        // Calculate section offset (current position for simplicity)
        uint64_t offset = stream_.tellp();
        
        // Write section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = offset;
        sectionHeader.size = size;
        sectionHeader.compressedSize = 0; // No compression for now
        sectionHeader.checksum = 0; // TODO: Calculate checksum
        
        stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(sectionHeader));
        if (stream_.fail()) {
            error_ = "Failed to write section header";
            return false;
        }
        
        // Write section data
        stream_.write(reinterpret_cast<const char*>(data), size);
        if (stream_.fail()) {
            error_ = "Failed to write section data";
            return false;
        }
        
        // Pad to 8-byte alignment
        size_t currentPos = stream_.tellp();
        if (currentPos % 8 != 0) {
            uint64_t padding[2] = {0, 0};
            size_t padSize = 8 - (currentPos % 8);
            stream_.write(reinterpret_cast<const char*>(&padding[0]), padSize);
        }
        
        sections_.push_back(sectionHeader);
        bytesWritten_ += sizeof(sectionHeader) + size;
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception writing section: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        if (!isOpen()) {
            error_ = "Cannot finalize: checkpoint file not open";
            return false;
        }
        
        // Seek back to beginning to write updated header
        stream_.seekp(0);
        
        // Update section count in header
        uint32_t sectionCount = static_cast<uint32_t>(sections_.size());
        stream_.write(reinterpret_cast<const char*>(&sectionCount), sizeof(sectionCount));
        
        // Seek to end and write footer (placeholder)
        stream_.seekp(0, std::ios::end);
        
        // Write final checksum
        uint64_t checksum = ChecksumCalculator::crc64(
            reinterpret_cast<const void*>(&header_), sizeof(header_));
        stream_.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
        
        if (stream_.fail()) {
            error_ = "Failed to write checkpoint footer";
            return false;
        }
        
        // Update header
        stream_.seekp(0);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        
        stream_.close();
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception finalizing checkpoint: " + std::string(e.what());
        if (stream_.is_open()) {
            stream_.close();
        }
        return false;
    }
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    if (!filepath_.empty() && std::filesystem::exists(filepath_)) {
        std::filesystem::remove(filepath_);
    }
    filepath_.clear();
    sections_.clear();
    bytesWritten_ = 0;
    compressBuffer_.clear();
}

// CheckpointManager implementation

CheckpointManager::CheckpointManager() : checkpointDir_(), saveIntervalSteps_(10000), 
                                       maxCheckpoints_(10), compress_(true), lastSaveStep_(0), 
                                       lastCheckpointPath_(), neuronProvider_(), synapseProvider_(), error_() {}

CheckpointManager::~CheckpointManager() {}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                uint64_t saveIntervalSteps,
                                size_t maxCheckpoints,
                                bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    lastSaveStep_ = 0;
    lastCheckpointPath_.clear();
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Checkpoint providers not set";
        return false;
    }
    
    // Create directory if it doesn't exist
    if (!checkpointDir_.empty() && !std::filesystem::exists(checkpointDir_)) {
        std::filesystem::create_directories(checkpointDir_);
    }
    
    // Generate filename
    std::string filename = checkpointDir_ + "/checkpoint_";
    if (!name.empty()) {
        filename += name + "_";
    }
    filename += std::to_string(lastSaveStep_) + ".ckp";
    
    // Create checkpoint writer
    CheckpointWriter writer;
    CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
    
    if (!writer.create(filename, compression)) {
        error_ = "Failed to create checkpoint: " + writer.getError();
        return false;
    }
    
    // Write neurons
    NeuronCheckpointData neuronData;
    if (!neuronProvider_(neuronData)) {
        error_ = "Failed to get neuron data from provider";
        return false;
    }
    
    if (!writer.writeNeurons(neuronData)) {
        error_ = "Failed to write neurons: " + writer.getError();
        return false;
    }
    
    // Write synapses
    SynapseCheckpointData synapseData;
    if (!synapseProvider_(synapseData)) {
        error_ = "Failed to get synapse data from provider";
        return false;
    }
    
    if (!writer.writeSynapses(synapseData)) {
        error_ = "Failed to write synapses: " + writer.getError();
        return false;
    }
    
    // Finalize checkpoint
    if (!writer.finalize()) {
        error_ = "Failed to finalize checkpoint: " + writer.getError();
        return false;
    }
    
    lastCheckpointPath_ = filename;
    lastSaveStep_ = lastSaveStep_; // Note: In real implementation, would use currentStep
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    // Simplified implementation
    std::string filename = checkpointDir_ + "/checkpoint_" + name + ".ckp";
    
    CheckpointReader reader;
    if (!reader.open(filename)) {
        error_ = "Failed to open checkpoint: " + reader.getError();
        return false;
    }
    
    if (!reader.validate()) {
        error_ = "Invalid checkpoint: " + reader.getError();
        return false;
    }
    
    // In real implementation, would read data and call providers
    lastCheckpointPath_ = filename;
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> result;
    
    if (checkpointDir_.empty() || !std::filesystem::exists(checkpointDir_)) {
        return result;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".ckp") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            // Extract step from filename if possible
            info.step = 0; // TODO: Parse step from filename
            info.simulationTime = 0.0; // TODO: Get from checkpoint
            info.neuronCount = 0; // TODO: Get from checkpoint
            info.synapseCount = 0; // TODO: Get from checkpoint
            info.fileSize = entry.file_size();
            info.timestamp = std::chrono::system_clock::to_time_t(
                std::chrono::file_time_clock::to_sys(entry.last_write_time()));
            
            result.push_back(info);
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(result.begin(), result.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return result;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Sort by timestamp (oldest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Delete oldest checkpoints
    size_t toDelete = checkpoints.size() - keepCount;
    for (size_t i = 0; i < toDelete; ++i) {
        std::string filepath = checkpointDir_ + "/checkpoint_" + checkpoints[i].name + ".ckp";
        if (std::filesystem::exists(filepath)) {
            std::filesystem::remove(filepath);
        }
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep - lastSaveStep_ >= saveIntervalSteps_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

} // namespace nlm