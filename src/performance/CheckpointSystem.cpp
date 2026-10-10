// CheckpointSystem.cpp
// Implementation of checkpointing system for NLM

#include "CheckpointSystem.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace nlm {

// Static member initialization
const uint64_t CheckpointCalculator::TABLE_SIZE = 256;

// Precomputed CRC-64 table (simplified)
static uint64_t precomputed_crc64_table[256] = {0};

// Initialize CRC table
static void initialize_crc64_table() {
    static bool initialized = false;
    if (initialized) return;
    
    // Simple CRC-64 polynomial
    const uint64_t polynomial = 0x42F0E1EBA9EA3693;
    
    for (uint32_t i = 0; i < 256; ++i) {
        uint64_t crc = i;
        for (int j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
        precomputed_crc64_table[i] = crc;
    }
    
    initialized = true;
}

// CheckpointReader implementation
CheckpointReader::CheckpointReader() : streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        stream_.open(filepath, std::ios::binary | std::ios::in);
        if (!stream_) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Get file size
        stream_.seekg(0, std::ios::end);
        uint64_t fileSize = stream_.tellg();
        stream_.seekg(0, std::ios::beg);
        
        if (fileSize < sizeof(CheckpointHeader)) {
            error_ = "Checkpoint file too small: " + filepath;
            stream_.close();
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_) {
            error_ = "Failed to read checkpoint header";
            stream_.close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header";
            stream_.close();
            return false;
        }
        
        // Read section headers
        size_t numSections = fileSize / sizeof(SectionHeader);
        sections_.resize(numSections);
        
        stream_.read(reinterpret_cast<char*>(sections_.data()), 
                     numSections * sizeof(SectionHeader));
        if (!stream_) {
            error_ = "Failed to read section headers";
            stream_.close();
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception opening checkpoint: ") + e.what();
        stream_.close();
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
    std::vector<uint8_t> sectionData = readSection(CheckpointSection::Neurons);
    if (sectionData.empty()) {
        return false;
    }
    
    size_t neuronCount = data.membranePotential.capacity();
    if (neuronCount == 0) {
        return true; // Empty data is valid
    }
    
    try {
        // Check if we have enough data
        size_t expectedSize = neuronCount * sizeof(float) * 11 +  // 11 float arrays
                             neuronCount * sizeof(uint8_t) * 3 +   // 3 uint8_t arrays
                             neuronCount * sizeof(uint32_t) * 3 +  // 3 uint32_t arrays
                             neuronCount * sizeof(uint64_t) * 4;   // 4 uint64_t arrays
        
        if (sectionData.size() < expectedSize) {
            error_ = "Insufficient neuron data in checkpoint";
            return false;
        }
        
        const uint8_t* ptr = sectionData.data();
        
        // Read membrane potential
        auto& destMembrane = data.membranePotential;
        destMembrane.resize(neuronCount);
        std::memcpy(destMembrane.data(), ptr, neuronCount * sizeof(float));
        ptr += neuronCount * sizeof(float);
        
        // Read resting potential
        auto& destResting = data.restingPotential;
        destResting.resize(neuronCount);
        std::memcpy(destResting.data(), ptr, neuronCount * sizeof(float));
        ptr += neuronCount * sizeof(float);
        
        // Read threshold
        auto& destThreshold = data.threshold;
        destThreshold.resize(neuronCount);
        std::memcpy(destThreshold.data(), ptr, neuronCount * sizeof(float));
        ptr += neuronCount * sizeof(float);
        
        // Read reset potential
        auto& destReset = data.resetPotential;
        destReset.resize(neuronCount);
        std::memcpy(destReset.data(), ptr, neuronCount * sizeof(float));
        ptr += neuronCount * sizeof(float);
        
        // Read leak conductance
        auto& destLeak = data.leakConductance;
        destLeak.resize(neuronCount);
        std::memcpy(destLeak.data(), ptr, neuronCount * sizeof(float));
        ptr += neuronCount * sizeof(float);
        
        // Read firing state
        auto& destFiring = data.firingState;
        destFiring.resize(neuronCount);
        std::memcpy(destFiring.data(), ptr, neuronCount * sizeof(uint8_t));
        ptr += neuronCount * sizeof(uint8_t);
        
        // Read refractory remaining
        auto& destRefRemaining = data.refractoryRemaining;
        destRefRemaining.resize(neuronCount);
        std::memcpy(destRefRemaining.data(), ptr, neuronCount * sizeof(uint32_t));
        ptr += neuronCount * sizeof(uint32_t);
        
        // Read refractory period
        auto& destRefPeriod = data.refractoryPeriod;
        destRefPeriod.resize(neuronCount);
        std::memcpy(destRefPeriod.data(), ptr, neuronCount * sizeof(uint32_t));
        ptr += neuronCount * sizeof(uint32_t);
        
        // Read last spike time
        auto& destLastSpike = data.lastSpikeTime;
        destLastSpike.resize(neuronCount);
        std::memcpy(destLastSpike.data(), ptr, neuronCount * sizeof(float));
        ptr += neuronCount * sizeof(float);
        
        // Read neuron type
        auto& destType = data.neuronType;
        destType.resize(neuronCount);
        std::memcpy(destType.data(), ptr, neuronCount * sizeof(uint64_t));
        ptr += neuronCount * sizeof(uint64_t);
        
        // Read region ID
        auto& destRegion = data.regionId;
        destRegion.resize(neuronCount);
        std::memcpy(destRegion.data(), ptr, neuronCount * sizeof(uint64_t));
        ptr += neuronCount * sizeof(uint64_t);
        
        // Read population ID
        auto& destPop = data.populationId;
        destPop.resize(neuronCount);
        std::memcpy(destPop.data(), ptr, neuronCount * sizeof(uint64_t));
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading neurons: ") + e.what();
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    std::vector<uint8_t> sectionData = readSection(CheckpointSection::Synapses);
    if (sectionData.empty()) {
        return false;
    }
    
    // Synapse data layout: source, dest, weight, delay, type, flags, eligibility, efficacy, depression, facilitation
    size_t synapseCount = data.weight.capacity();
    if (synapseCount == 0) {
        return true; // Empty data is valid
    }
    
    try {
        // Check if we have enough data
        size_t expectedSize = synapseCount * (sizeof(uint64_t) * 2 +  // source, dest
                                             sizeof(float) * 7 +        // weight, eligibility, efficacy, depression, facilitation
                                             sizeof(uint32_t) +         // delay
                                             sizeof(uint8_t) * 2);     // type, flags
        
        if (sectionData.size() < expectedSize) {
            error_ = "Insufficient synapse data in checkpoint";
            return false;
        }
        
        const uint8_t* ptr = sectionData.data();
        
        // Read source neurons
        auto& destSource = data.sourceNeuron;
        destSource.resize(synapseCount);
        std::memcpy(destSource.data(), ptr, synapseCount * sizeof(uint64_t));
        ptr += synapseCount * sizeof(uint64_t);
        
        // Read destination neurons
        auto& destDest = data.destinationNeuron;
        destDest.resize(synapseCount);
        std::memcpy(destDest.data(), ptr, synapseCount * sizeof(uint64_t));
        ptr += synapseCount * sizeof(uint64_t);
        
        // Read weights
        auto& destWeight = data.weight;
        destWeight.resize(synapseCount);
        std::memcpy(destWeight.data(), ptr, synapseCount * sizeof(float));
        ptr += synapseCount * sizeof(float);
        
        // Read delays
        auto& destDelay = data.delay;
        destDelay.resize(synapseCount);
        std::memcpy(destDelay.data(), ptr, synapseCount * sizeof(uint32_t));
        ptr += synapseCount * sizeof(uint32_t);
        
        // Read synapse types
        auto& destType = data.synapseType;
        destType.resize(synapseCount);
        std::memcpy(destType.data(), ptr, synapseCount * sizeof(uint8_t));
        ptr += synapseCount * sizeof(uint8_t);
        
        // Read plasticity flags
        auto& destFlags = data.plasticityFlags;
        destFlags.resize(synapseCount);
        std::memcpy(destFlags.data(), ptr, synapseCount * sizeof(uint8_t));
        ptr += synapseCount * sizeof(uint8_t);
        
        // Read eligibility traces
        auto& destEligibility = data.eligibilityTrace;
        destEligibility.resize(synapseCount);
        std::memcpy(destEligibility.data(), ptr, synapseCount * sizeof(float));
        ptr += synapseCount * sizeof(float);
        
        // Read efficacy
        auto& destEfficacy = data.efficacy;
        destEfficacy.resize(synapseCount);
        std::memcpy(destEfficacy.data(), ptr, synapseCount * sizeof(float));
        ptr += synapseCount * sizeof(float);
        
        // Read short-term depression
        auto& destDepression = data.shortTermDepression;
        destDepression.resize(synapseCount);
        std::memcpy(destDepression.data(), ptr, synapseCount * sizeof(float));
        ptr += synapseCount * sizeof(float);
        
        // Read short-term facilitation
        auto& destFacilitation = data.shortTermFacilitation;
        destFacilitation.resize(synapseCount);
        std::memcpy(destFacilitation.data(), ptr, synapseCount * sizeof(float));
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading synapses: ") + e.what();
        return false;
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            // Seek to section data
            stream_.seekg(section.offset, std::ios::beg);
            
            // Read section data
            std::vector<uint8_t> data(section.size);
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            
            if (!stream_) {
                error_ = "Failed to read section data";
                return {};
            }
            
            return data;
        }
    }
    
    error_ = "Section not found in checkpoint";
    return {};
}

bool CheckpointReader::validate() const {
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    if (sections_.empty()) {
        error_ = "No sections in checkpoint";
        return false;
    }
    
    // Check for required sections
    bool hasNeurons = false;
    bool hasSynapses = false;
    
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(CheckpointSection::Neurons)) {
            hasNeurons = true;
        } else if (section.type == static_cast<uint32_t>(CheckpointSection::Synapses)) {
            hasSynapses = true;
        }
    }
    
    if (!hasNeurons) {
        error_ = "Missing neurons section";
        return false;
    }
    
    if (!hasSynapses) {
        error_ = "Missing synapses section";
        return false;
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    size_t totalSize = 0;
    for (const auto& section : sections_) {
        totalSize += section.size;
    }
    
    if (totalSize == 0) return 1.0;
    
    return static_cast<double>(streamPos_) / totalSize;
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {
    initialize_crc64_table();
}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Create directory if it doesn't exist
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        stream_.open(filepath, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!stream_) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        compression_ = compression;
        filepath_ = filepath;
        
        return true;
        
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
    error_.clear();
    compressBuffer_.clear();
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                 uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    
    // Set timestamp
    auto now = std::chrono::system_clock::now();
    header_.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    return writeSection(CheckpointSection::Neurons, data);
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    return writeSection(CheckpointSection::Synapses, data);
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    // Compress if requested
    std::vector<uint8_t> sectionData;
    size_t compressedSize = size;
    
    if (compression_ != CompressionLevel::None) {
        sectionData = compressSection(static_cast<const uint8_t*>(data), size, compressedSize);
        if (sectionData.empty()) {
            return false;
        }
        data = sectionData.data();
        size = compressedSize;
    }
    
    // Calculate checksum
    uint32_t checksum = ChecksumCalculator::crc32(static_cast<const uint8_t*>(data), size);
    
    // Create section header
    SectionHeader section;
    section.type = static_cast<uint32_t>(sectionType);
    section.offset = sizeof(CheckpointHeader) + 
                    sizeof(SectionHeader) * sections_.size() + 
                    bytesWritten_;
    section.size = size;
    section.compressedSize = (compression_ != CompressionLevel::None) ? compressedSize : 0;
    section.checksum = checksum;
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
    if (!stream_) {
        error_ = "Failed to write section header";
        return false;
    }
    
    // Write section data
    stream_.write(reinterpret_cast<const char*>(data), size);
    if (!stream_) {
        error_ = "Failed to write section data";
        return false;
    }
    
    sections_.push_back(section);
    bytesWritten_ += size + sizeof(SectionHeader);
    
    return true;
}

bool CheckpointWriter::finalize() {
    // Update header
    header_.totalSize = bytesWritten_;
    header_.checksum = ChecksumCalculator::crc64(
        reinterpret_cast<const uint8_t*>(&header_), sizeof(CheckpointHeader));
    
    // Write header
    stream_.seekg(0, std::ios::beg);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    
    if (!stream_) {
        error_ = "Failed to write checkpoint header";
        return false;
    }
    
    // Write section headers
    stream_.write(reinterpret_cast<const char*>(sections_.data()),
                  sections_.size() * sizeof(SectionHeader));
    
    if (!stream_) {
        error_ = "Failed to write section headers";
        return false;
    }
    
    stream_.close();
    return true;
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        // Remove incomplete file
        if (!filepath_.empty() && std::filesystem::exists(filepath_)) {
            std::filesystem::remove(filepath_);
        }
    }
    sections_.clear();
    error_.clear();
    compressBuffer_.clear();
}

std::vector<uint8_t> CheckpointWriter::compressSection(const uint8_t* data, size_t size, size_t& compressedSize) {
    // Simple compression: run-length encoding for simple case
    // In a real implementation, this would use a proper compression algorithm
    
    if (size < 100 || compression_ == CompressionLevel::None) {
        compressedSize = size;
        std::vector<uint8_t> result(data, data + size);
        return result;
    }
    
    // Simple RLE compression for demonstration
    std::vector<uint8_t> result;
    result.reserve(size * 0.8f); // Assume 20% compression
    
    const uint8_t* ptr = data;
    const uint8_t* end = data + size;
    
    while (ptr < end) {
        uint8_t byte = *ptr;
        size_t count = 1;
        
        // Count consecutive identical bytes
        while (++ptr < end && *ptr == byte && count < 255) {
            ++count;
        }
        
        // Store run length and byte
        result.push_back(count);
        result.push_back(byte);
    }
    
    compressedSize = result.size();
    return result;
}

// CheckpointManager implementation
CheckpointManager::CheckpointManager() : lastSaveStep_(0) {}

CheckpointManager::~CheckpointManager() {}

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
    std::string checkpointName = name;
    if (checkpointName.empty()) {
        checkpointName = std::to_string(lastSaveStep_);
    }
    
    std::string filepath = checkpointDir_ + "/checkpoint_" + checkpointName + ".nclm";
    
    try {
        // Create writer
        CheckpointWriter writer;
        
        if (compression_) {
            writer.create(filepath, CompressionLevel::Balanced);
        } else {
            writer.create(filepath, CompressionLevel::None);
        }
        
        // TODO: In a real implementation, the brain would provide the data
        // For now, just create an empty checkpoint
        writer.setMetadata(0, 0, 0, lastSaveStep_, currentTime);
        writer.finalize();
        
        lastSaveStep_ = currentStep;
        lastCheckpointPath_ = filepath;
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/checkpoint_" + name + ".nclm";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    return reader.validate();
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find("checkpoint_") == 0 && filename.find(".nclm") != std::string::npos) {
                CheckpointInfo info;
                info.name = filename.substr(11, filename.size() - 15); // Remove "checkpoint_" and ".nclm"
                // TODO: Parse step, time, etc. from filename or file content
                info.step = 0;
                info.simulationTime = 0.0;
                info.neuronCount = 0;
                info.synapseCount = 0;
                info.fileSize = entry.file_size();
                info.timestamp = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now());
                
                checkpoints.push_back(info);
            }
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
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Sort by timestamp (oldest first) and remove oldest
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp < b.timestamp;
              });
    
    size_t toRemove = checkpoints.size() - keepCount;
    for (size_t i = 0; i < toRemove; ++i) {
        std::string filepath = checkpointDir_ + "/checkpoint_" + checkpoints[i].name + ".nclm";
        if (std::filesystem::exists(filepath)) {
            std::filesystem::remove(filepath);
        }
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (saveIntervalSteps_ == 0) return false;
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

} // namespace nlm
