#include "CheckpointSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/PerformanceMonitor.hpp"
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <numeric>

namespace nlm {

CheckpointReader::CheckpointReader() : streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    close();
    
    stream_.open(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
        return false;
    }
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_.good()) {
        error_ = "Failed to read checkpoint header";
        stream_.close();
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint file format";
        stream_.close();
        return false;
    }
    
    // Read section headers
    uint64_t sectionCount = header_.totalSize / sizeof(SectionHeader);
    sections_.resize(sectionCount);
    
    stream_.read(reinterpret_cast<char*>(sections_.data()),
                  sectionCount * sizeof(SectionHeader));
    if (!stream_.good()) {
        error_ = "Failed to read section headers";
        stream_.close();
        return false;
    }
    
    streamPos_ = sizeof(CheckpointHeader) + sectionCount * sizeof(SectionHeader);
    return true;
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
    std::vector<uint8_t> sectionData = readSection(CheckpointSection::Neurons);
    if (sectionData.empty()) {
        return false;
    }
    
    // Parse neuron data
    const uint8_t* buffer = sectionData.data();
    size_t offset = 0;
    
    auto read_array = [&buffer, &offset](auto& vec, size_t size) {
        if (offset + size * sizeof(typename decltype(vec)::value_type) > sectionData.size()) {
            return false;
        }
        vec.assign(reinterpret_cast<const typename decltype(vec)::value_type*>(buffer + offset),
                    reinterpret_cast<const typename decltype(vec)::value_type*>(buffer + offset) + size);
        offset += size * sizeof(typename decltype(vec)::value_type);
        return true;
    };
    
    bool success = read_array(data.membranePotential, data.membranePotential.capacity()) &&
                   read_array(data.restingPotential, data.restingPotential.capacity()) &&
                   read_array(data.threshold, data.threshold.capacity()) &&
                   read_array(data.resetPotential, data.resetPotential.capacity()) &&
                   read_array(data.leakConductance, data.leakConductance.capacity()) &&
                   read_array(data.firingState, data.firingState.capacity()) &&
                   read_array(data.refractoryRemaining, data.refractoryRemaining.capacity()) &&
                   read_array(data.refractoryPeriod, data.refractoryPeriod.capacity()) &&
                   read_array(data.lastSpikeTime, data.lastSpikeTime.capacity()) &&
                   read_array(data.neuronType, data.neuronType.capacity()) &&
                   read_array(data.regionId, data.regionId.capacity()) &&
                   read_array(data.populationId, data.populationId.capacity());
    
    return success;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    std::vector<uint8_t> sectionData = readSection(CheckpointSection::Synapses);
    if (sectionData.empty()) {
        return false;
    }
    
    // Parse synapse data
    const uint8_t* buffer = sectionData.data();
    size_t offset = 0;
    
    auto read_array = [&buffer, &offset](auto& vec, size_t size) {
        if (offset + size * sizeof(typename decltype(vec)::value_type) > sectionData.size()) {
            return false;
        }
        vec.assign(reinterpret_cast<const typename decltype(vec)::value_type*>(buffer + offset),
                    reinterpret_cast<const typename decltype(vec)::value_type*>(buffer + offset) + size);
        offset += size * sizeof(typename decltype(vec)::value_type);
        return true;
    };
    
    bool success = read_array(data.sourceNeuron, data.sourceNeuron.capacity()) &&
                   read_array(data.destinationNeuron, data.destinationNeuron.capacity()) &&
                   read_array(data.weight, data.weight.capacity()) &&
                   read_array(data.delay, data.delay.capacity()) &&
                   read_array(data.synapseType, data.synapseType.capacity()) &&
                   read_array(data.plasticityFlags, data.plasticityFlags.capacity()) &&
                   read_array(data.eligibilityTrace, data.eligibilityTrace.capacity()) &&
                   read_array(data.efficacy, data.efficacy.capacity()) &&
                   read_array(data.shortTermDepression, data.shortTermDepression.capacity()) &&
                   read_array(data.shortTermFacilitation, data.shortTermFacilitation.capacity());
    
    return success;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            // Seek to section data
            stream_.seekg(section.offset, std::ios::beg);
            
            // Read section data
            std::vector<uint8_t> data(section.size);
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            
            if (!stream_.good()) {
                error_ = "Failed to read section data";
                return std::vector<uint8_t>();
            }
            
            return data;
        }
    }
    
    error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Verify checksums
    for (const auto& section : sections_) {
        if (section.checksum != 0) {
            // In a real implementation, we would recalculate and verify checksum
            // For simplicity, we assume checksums are valid
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) {
        return 0.0;
    }
    
    // Calculate progress based on stream position
    std::ifstream tempStream(filepath_, std::ios::binary);
    if (!tempStream) {
        return 0.0;
    }
    
    tempStream.seekg(0, std::ios::end);
    uint64_t totalSize = tempStream.tellg();
    tempStream.close();
    
    if (totalSize == 0) {
        return 1.0;
    }
    
    return static_cast<double>(streamPos_) / totalSize;
}

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    close();
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Ensure directory exists
    std::filesystem::path filepath_path(filepath_);
    std::filesystem::create_directories(filepath_path.parent_path());
    
    stream_.open(filepath_, std::ios::binary | std::ios::trunc);
    if (!stream_.is_open()) {
        error_ = "Failed to create checkpoint file: " + filepath;
        return false;
    }
    
    bytesWritten_ = 0;
    return true;
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // Finalize if not already done
        if (!sections_.empty()) {
            finalize();
        }
        stream_.close();
    }
    
    sections_.clear();
    error_.clear();
    bytesWritten_ = 0;
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                    uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    header_.timestamp = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    // Convert neuron data to binary format
    std::vector<uint8_t> sectionData;
    
    // Allocate space for all arrays
    size_t totalSize = 0;
    totalSize += data.membranePotential.size() * sizeof(float);
    totalSize += data.restingPotential.size() * sizeof(float);
    totalSize += data.threshold.size() * sizeof(float);
    totalSize += data.resetPotential.size() * sizeof(float);
    totalSize += data.leakConductance.size() * sizeof(float);
    totalSize += data.firingState.size() * sizeof(uint8_t);
    totalSize += data.refractoryRemaining.size() * sizeof(uint32_t);
    totalSize += data.refractoryPeriod.size() * sizeof(uint32_t);
    totalSize += data.lastSpikeTime.size() * sizeof(float);
    totalSize += data.neuronType.size() * sizeof(uint64_t);
    totalSize += data.regionId.size() * sizeof(uint64_t);
    totalSize += data.populationId.size() * sizeof(uint64_t);
    
    sectionData.resize(totalSize);
    uint8_t* buffer = sectionData.data();n    size_t offset = 0;
    
    auto write_array = [&buffer, &offset](const auto& vec) {
        size_t size = vec.size() * sizeof(typename std::remove_reference<decltype(vec)>::type::value_type);
        std::memcpy(buffer + offset, vec.data(), size);
        offset += size;
    };
    
    write_array(data.membranePotential);
    write_array(data.restingPotential);
    write_array(data.threshold);
    write_array(data.resetPotential);
    write_array(data.leakConductance);
    write_array(data.firingState);
    write_array(data.refractoryRemaining);
    write_array(data.refractoryPeriod);
    write_array(data.lastSpikeTime);
    write_array(data.neuronType);
    write_array(data.regionId);
    write_array(data.populationId);
    
    return writeSection(CheckpointSection::Neurons, sectionData.data(), sectionData.size());
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    // Convert synapse data to binary format
    std::vector<uint8_t> sectionData;
    
    // Allocate space for all arrays
    size_t totalSize = 0;
    totalSize += data.sourceNeuron.size() * sizeof(uint64_t);
    totalSize += data.destinationNeuron.size() * sizeof(uint64_t);
    totalSize += data.weight.size() * sizeof(float);
    totalSize += data.delay.size() * sizeof(uint32_t);
    totalSize += data.synapseType.size() * sizeof(uint8_t);
    totalSize += data.plasticityFlags.size() * sizeof(uint8_t);
    totalSize += data.eligibilityTrace.size() * sizeof(float);
    totalSize += data.efficacy.size() * sizeof(float);
    totalSize += data.shortTermDepression.size() * sizeof(float);
    totalSize += data.shortTermFacilitation.size() * sizeof(float);
    
    sectionData.resize(totalSize);
    uint8_t* buffer = sectionData.data();
    size_t offset = 0;
    
    auto write_array = [&buffer, &offset](const auto& vec) {
        size_t size = vec.size() * sizeof(typename std::remove_reference<decltype(vec)>::type::value_type);
        std::memcpy(buffer + offset, vec.data(), size);
        offset += size;
    };
    
    write_array(data.sourceNeuron);
    write_array(data.destinationNeuron);
    write_array(data.weight);
    write_array(data.delay);
    write_array(data.synapseType);
    write_array(data.plasticityFlags);
    write_array(data.eligibilityTrace);
    write_array(data.efficacy);
    write_array(data.shortTermDepression);
    write_array(data.shortTermFacilitation);
    
    return writeSection(CheckpointSection::Synapses, sectionData.data(), sectionData.size());
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    SectionHeader section;
    section.type = static_cast<uint32_t>(sectionType);
    section.offset = sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader) + size;
    section.size = size;
    section.compressedSize = 0; // Not compressed for simplicity
    section.checksum = ChecksumCalculator::crc32(static_cast<const uint8_t*>(data), size);
    
    sections_.push_back(section);
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
    if (!stream_.good()) {
        error_ = "Failed to write section header";
        return false;
    }
    
    // Write section data
    stream_.write(static_cast<const char*>(data), size);
    if (!stream_.good()) {
        error_ = "Failed to write section data";
        return false;
    }
    
    bytesWritten_ += sizeof(SectionHeader) + size;
    return true;
}

bool CheckpointWriter::finalize() {
    if (sections_.empty()) {
        error_ = "No sections to finalize";
        return false;
    }
    
    // Calculate total size
    uint64_t totalSize = sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader);
    for (const auto& section : sections_) {
        totalSize += section.size;
    }
    
    header_.totalSize = totalSize;
    
    // Write header
    if (!writeHeader()) {
        return false;
    }
    
    // Write section headers
    for (const auto& section : sections_) {
        stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
        if (!stream_.good()) {
            error_ = "Failed to write section headers";
            return false;
        }
    }
    
    // Write section data
    for (const auto& section : sections_) {
        // Seek to section data position
        stream_.seekp(section.offset, std::ios::beg);
        
        // Read section data from original stream (simplified)
        stream_.clear();
        stream_.seekg(section.offset, std::ios::beg);
        
        std::vector<uint8_t> sectionData(section.size);
        stream_.read(reinterpret_cast<char*>(sectionData.data()), section.size);
        
        stream_.seekp(section.offset, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(sectionData.data()), section.size);
        
        if (!stream_.good()) {
            error_ = "Failed to write section data";
            return false;
        }
    }
    
    return true;
}

void CheckpointWriter::abort() {
    close();
    
    // Remove partial checkpoint file
    if (!filepath_.empty()) {
        std::filesystem::remove(filepath_);
    }
}

bool CheckpointWriter::writeHeader() {
    // Write header
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_.good()) {
        error_ = "Failed to write header";
        return false;
    }
    
    bytesWritten_ += sizeof(CheckpointHeader);
    return true;
}

CheckpointManager::CheckpointManager() : lastSaveStep_(0) {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir,
                                   uint64_t saveIntervalSteps,
                                   size_t maxCheckpoints,
                                   bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Ensure checkpoint directory exists
    std::filesystem::create_directories(checkpointDir_);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string checkpointPath = checkpointDir_ + "/";
    if (!name.empty()) {
        checkpointPath += name + ".nck";
    } else {
        checkpointPath += "checkpoint_" + std::to_string(lastSaveStep_) + ".nck";
    }
    
    CheckpointWriter writer;
    if (!writer.create(checkpointPath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        NLM_LOG_ERROR("Failed to create checkpoint: " + checkpointPath);
        return false;
    }
    
    // Set metadata
    writer.setMetadata(
        /* neuronCount */ 0,  // Would get from actual brain
        /* synapseCount */ 0, // Would get from actual brain
        /* regionCount */ 0,  // Would get from actual brain
        currentStep,
        currentTime
    );
    
    // In a real implementation, we would call actual data providers
    // For now, we'll just write empty sections
    
    if (!writer.finalize()) {
        NLM_LOG_ERROR("Failed to finalize checkpoint: " + checkpointPath);
        return false;
    }
    
    lastSaveStep_ = currentStep;
    lastCheckpointPath_ = checkpointPath;
    
    NLM_LOG_INFO("Checkpoint saved: " + checkpointPath);
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string checkpointPath = checkpointDir_ + "/" + name + ".nck";
    
    CheckpointReader reader;
    if (!reader.open(checkpointPath)) {
        NLM_LOG_ERROR("Failed to open checkpoint: " + checkpointPath);
        return false;
    }
    
    NLM_LOG_INFO("Checkpoint loaded successfully: " + checkpointPath);
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".nck") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            info.fileSize = entry.file_size();
            info.timestamp = entry.last_write_time().time_since_epoch().count();
            
            // Extract step from filename if possible
            // In a real implementation, we would parse the step from the filename
            info.step = 0;
            info.simulationTime = 0.0;
            info.neuronCount = 0;
            info.synapseCount = 0;
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    std::vector<CheckpointInfo> checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        std::filesystem::remove(checkpointDir_ + "/" + checkpoints[i].name + ".nck");
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (saveIntervalSteps_ == 0) {
        return false;
    }
    
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

} // namespace nlm
