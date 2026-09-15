// CheckpointSystem.cpp - Implementation of CheckpointSystem classes

#include "CheckpointSystem.hpp"
#include <stdexcept>
#include <algorithm>

namespace nlm {

// CheckpointReader implementation
CheckpointReader::CheckpointReader() : stream_(), header_(), sections_(), error_(), streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    close();
    
    stream_.open(filepath, std::ios::binary | std::ios::in);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
        return false;
    }
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_) {
        error_ = "Failed to read checkpoint header";
        close();
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header (wrong magic or version)";
        close();
        return false;
    }
    
    // Read sections
    streamPos_ = sizeof(CheckpointHeader);
    for (uint64_t sectionCount = 0; sectionCount < header_.sectionCount; ++sectionCount) {
        SectionHeader sectionHeader;
        stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(SectionHeader));
        if (!stream_) {
            error_ = "Failed to read section header";
            close();
            return false;
        }
        
        sections_.push_back(sectionHeader);
    }
    
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
    
    // Deserialize neuron data
    const uint8_t* ptr = sectionData.data();
    size_t remaining = sectionData.size();
    
    data.membranePotential.clear();
    data.restingPotential.clear();
    data.threshold.clear();
    data.resetPotential.clear();
    data.leakConductance.clear();
    data.firingState.clear();
    data.refractoryRemaining.clear();
    data.refractoryPeriod.clear();
    data.lastSpikeTime.clear();
    data.neuronType.clear();
    data.regionId.clear();
    data.populationId.clear();
    
    size_t neuronCount = 0;
    memcpy(&neuronCount, ptr, std::min(sizeof(size_t), remaining));
    ptr += sizeof(size_t);
    remaining -= sizeof(size_t);
    
    data.membranePotential.reserve(neuronCount);
    data.restingPotential.reserve(neuronCount);
    data.threshold.reserve(neuronCount);
    data.resetPotential.reserve(neuronCount);
    data.leakConductance.reserve(neuronCount);
    data.firingState.reserve(neuronCount);
    data.refractoryRemaining.reserve(neuronCount);
    data.refractoryPeriod.reserve(neuronCount);
    data.lastSpikeTime.reserve(neuronCount);
    data.neuronType.reserve(neuronCount);
    data.regionId.reserve(neuronCount);
    data.populationId.reserve(neuronCount);
    
    for (size_t i = 0; i < neuronCount; ++i) {
        // Read neuron state (simplified - assuming float arrays)
        size_t floatCount = 6;  // membrane, resting, threshold, reset, leak
        size_t uint8Count = 1; // firingState
        size_t uint32Count = 3; // refractoryRemaining, refractoryPeriod, lastSpikeTime (as uint32)
        size_t uint64Count = 3; // neuronType, regionId, populationId
        
        std::vector<float> floats(floatCount);
        std::vector<uint8_t> uint8s(uint8Count);
        std::vector<uint32_t> uint32s(uint32Count);
        std::vector<uint64_t> uint64s(uint64Count);
        
        if (remaining < sizeof(float) * floatCount + sizeof(uint8_t) * uint8Count + 
            sizeof(uint32_t) * uint32Count + sizeof(uint64_t) * uint64Count) {
            break;
        }
        
        memcpy(floats.data(), ptr, sizeof(float) * floatCount);
        ptr += sizeof(float) * floatCount;
        remaining -= sizeof(float) * floatCount;
        
        memcpy(uint8s.data(), ptr, sizeof(uint8_t) * uint8Count);
        ptr += sizeof(uint8_t) * uint8Count;
        remaining -= sizeof(uint8_t) * uint8Count;
        
        memcpy(uint32s.data(), ptr, sizeof(uint32_t) * uint32Count);
        ptr += sizeof(uint32_t) * uint32Count;
        remaining -= sizeof(uint32_t) * uint32Count;
        
        memcpy(uint64s.data(), ptr, sizeof(uint64_t) * uint64Count);
        ptr += sizeof(uint64_t) * uint64Count;
        remaining -= sizeof(uint64_t) * uint64Count;
        
        data.membranePotential.push_back(floats[0]);
        data.restingPotential.push_back(floats[1]);
        data.threshold.push_back(floats[2]);
        data.resetPotential.push_back(floats[3]);
        data.leakConductance.push_back(floats[4]);
        data.firingState.push_back(uint8s[0]);
        data.refractoryRemaining.push_back(uint32s[0]);
        data.refractoryPeriod.push_back(uint32s[1]);
        // Note: lastSpikeTime is stored as float but read as uint32
        float* spikeTime = reinterpret_cast<float*>(&uint32s[2]);
        data.lastSpikeTime.push_back(*spikeTime);
        data.neuronType.push_back(uint64s[0]);
        data.regionId.push_back(uint64s[1]);
        data.populationId.push_back(uint64s[2]);
    }
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    std::vector<uint8_t> sectionData = readSection(CheckpointSection::Synapses);
    if (sectionData.empty()) {
        return false;
    }
    
    // Deserialize synapse data
    const uint8_t* ptr = sectionData.data();
    size_t remaining = sectionData.size();
    
    data.sourceNeuron.clear();
    data.destinationNeuron.clear();
    data.weight.clear();
    data.delay.clear();
    data.synapseType.clear();
    data.plasticityFlags.clear();
    data.eligibilityTrace.clear();
    data.efficacy.clear();
    data.shortTermDepression.clear();
    data.shortTermFacilitation.clear();
    
    size_t synapseCount = 0;
    memcpy(&synapseCount, ptr, std::min(sizeof(size_t), remaining));
    ptr += sizeof(size_t);
    remaining -= sizeof(size_t);
    
    data.sourceNeuron.reserve(synapseCount);
    data.destinationNeuron.reserve(synapseCount);
    data.weight.reserve(synapseCount);
    data.delay.reserve(synapseCount);
    data.synapseType.reserve(synapseCount);
    data.plasticityFlags.reserve(synapseCount);
    data.eligibilityTrace.reserve(synapseCount);
    data.efficacy.reserve(synapseCount);
    data.shortTermDepression.reserve(synapseCount);
    data.shortTermFacilitation.reserve(synapseCount);
    
    for (size_t i = 0; i < synapseCount; ++i) {
        size_t dataSize = sizeof(uint64_t) * 2 +  // source, dest
                         sizeof(float) * 8 +     // weight, delay, eligibility, efficacy, depression, facilitation (6 floats)
                         sizeof(uint8_t) * 2;    // synapseType, plasticityFlags
        
        if (remaining < dataSize) {
            break;
        }
        
        data.sourceNeuron.push_back(*reinterpret_cast<const uint64_t*>(ptr));
        ptr += sizeof(uint64_t);
        
        data.destinationNeuron.push_back(*reinterpret_cast<const uint64_t*>(ptr));
        ptr += sizeof(uint64_t);
        
        float* floatPtr = reinterpret_cast<float*>(ptr);
        data.weight.push_back(floatPtr[0]);
        data.delay.push_back(static_cast<uint32_t>(floatPtr[1]));
        data.eligibilityTrace.push_back(floatPtr[2]);
        data.efficacy.push_back(floatPtr[3]);
        data.shortTermDepression.push_back(floatPtr[4]);
        data.shortTermFacilitation.push_back(floatPtr[5]);
        ptr += sizeof(float) * 6;
        
        data.synapseType.push_back(*reinterpret_cast<const uint8_t*>(ptr));
        ptr += sizeof(uint8_t);
        
        data.plasticityFlags.push_back(*reinterpret_cast<const uint8_t*>(ptr));
        ptr += sizeof(uint8_t);
        
        remaining -= dataSize;
    }
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            stream_.seekg(section.offset);
            
            std::vector<uint8_t> data(section.size);
            if (section.compressedSize > 0 && section.compressedSize < section.size) {
                // Decompress section data
                // For now, treat compressed as regular
                stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            } else {
                stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            }
            
            return data;
        }
    }
    
    error_ = "Section " + std::to_string(static_cast<uint32_t>(sectionType)) + " not found";
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    // Validate checksum if section 0 (checksum) exists
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(CheckpointSection::Checksum)) {
            stream_.seekg(section.offset);
            uint64_t checksum;
            stream_.read(reinterpret_cast<char*>(&checksum), sizeof(uint64_t));
            // In a real implementation, we'd verify the checksum
            return true;
        }
    }
    return true; // No checksum section means assume valid
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    // Simple progress based on sections
    size_t processed = 0;
    for (const auto& section : sections_) {
        if (streamPos_ >= section.offset + section.size) {
            processed++;
        }
    }
    
    return static_cast<double>(processed) / sections_.size();
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(CompressionLevel::Balanced),
    header_(), sections_(), bytesWritten_(0), compressBuffer_() {}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    close();
    
    filepath_ = filepath;
    compression_ = compression;
    
    stream_.open(filepath, std::ios::binary | std::ios::out);
    if (!stream_.is_open()) {
        return false;
    }
    
    header_ = CheckpointHeader(); // Initialize with defaults
    return writeHeader();
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        finalize();
        stream_.close();
    }
    filepath_.clear();
    sections_.clear();
    bytesWritten_ = 0;
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
    return writeSection(CheckpointSection::Neurons, &data, sizeof(data));
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    return writeSection(CheckpointSection::Synapses, &data, sizeof(data));
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    // In a full implementation, this would handle compression and checksum
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(sectionType);
    sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
    sectionHeader.size = size;
    sectionHeader.compressedSize = 0;
    sectionHeader.checksum = 0;
    
    if (!writeSectionHeader(sectionHeader)) {
        return false;
    }
    
    if (!padToAlignment(64)) {
        return false;
    }
    
    stream_.write(reinterpret_cast<const char*>(data), size);
    if (!stream_) {
        return false;
    }
    
    sections_.push_back(sectionHeader);
    bytesWritten_ += size;
    
    return true;
}

bool CheckpointWriter::writeHeader() {
    // Set header values
    header_.sectionCount = static_cast<uint64_t>(sections_.size());
    
    stream_.seekp(0);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    
    if (!stream_) {
        return false;
    }
    
    bytesWritten_ = sizeof(CheckpointHeader);
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    if (!stream_) {
        return false;
    }
    bytesWritten_ += sizeof(SectionHeader);
    return true;
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t current = static_cast<size_t>(stream_.tellp());
    size_t padding = (alignment - (current % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> padBuffer(padding, 0);
        stream_.write(reinterpret_cast<const char*>(padBuffer.data()), padding);
        if (!stream_) {
            return false;
        }
        bytesWritten_ += padding;
    }
    
    return true;
}

bool CheckpointWriter::finalize() {
    // Write checksum section (simplified)
    uint64_t checksum = 0; // Placeholder checksum
    return writeSection(CheckpointSection::Checksum, &checksum, sizeof(checksum));
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    // Delete partial file
    std::remove(filepath_.c_str());
    close();
}

// CheckpointManager implementation
CheckpointManager::CheckpointManager() : 
    checkpointDir_(), saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true),
    lastSaveStep_(0), lastCheckpointPath_(),
    neuronProvider_(), synapseProvider_(), workingMemoryProvider_(),
    episodicMemoryProvider_(), neuromodulationProvider_(),
    developmentProvider_(), predictionProvider_() {}

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
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string checkpointName = name.empty() ? "checkpoint" : name;
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".ckp";
    
    // In a real implementation, this would call the brain's save methods
    // via the provider functions
    
    lastSaveStep_ = currentStep;
    lastCheckpointPath_ = filepath;
    
    // Cleanup old checkpoints
    pruneOldCheckpoints(maxCheckpoints_);
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".ckp";
    
    // In a real implementation, this would call the brain's load methods
    // via the reader functions
    
    lastCheckpointPath_ = filepath;
    return true;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    // In a real implementation, this would list checkpoints and delete old ones
    // based on creation time, keeping the most recent 'keepCount' checkpoints
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    // In a real implementation, this would scan the checkpoint directory
    // and populate the CheckpointInfo structures
    
    return checkpoints;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= lastSaveStep_ + saveIntervalSteps_;
}

} // namespace nlm
