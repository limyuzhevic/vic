// CheckpointSystem.cpp
// Implementation of checkpoint reader/writer functionality for neural simulation

#include "CheckpointSystem.hpp"
#include <algorithm>
#include <cstring>
#include <ctime>
#include <zlib.h>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

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
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint header";
            return false;
        }
        
        // Validate header
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header";
            return false;
        }
        
        // Read section headers
        sections_.resize(header_.regionCount);
        for (auto& section : sections_) {
            stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader));
            if (!stream_.good()) {
                error_ = "Failed to read section headers";
                return false;
            }
        }
        
        streamPos_ = stream_.tellg();
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
    streamPos_ = 0;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    try {
        // Find neurons section
        size_t neuronsSectionIndex = 0;
        for (size_t i = 0; i < sections_.size(); ++i) {
            if (sections_[i].type == static_cast<uint32_t>(CheckpointSection::Neurons)) {
                neuronsSectionIndex = i;
                break;
            }
        }
        
        if (sections_[neuronsSectionIndex].size == 0) {
            return true; // Empty data is valid
        }
        
        // Seek to neurons section
        stream_.seekg(streamPos_ + sections_[neuronsSectionIndex].offset);
        
        // Decompress if needed
        size_t dataSize = sections_[neuronsSectionIndex].size;
        size_t compressedSize = sections_[neuronsSectionIndex].compressedSize;
        
        if (compressedSize > 0) {
            std::vector<uint8_t> compressedData(compressedSize);
            std::vector<uint8_t> decompressedData(dataSize);
            
            stream_.read(reinterpret_cast<char*>(compressedData.data()), compressedSize);
            
            if (uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), 
                          &dataSize, 
                          reinterpret_cast<const Bytef*>(compressedData.data()), 
                          compressedSize) != Z_OK) {
                error_ = "Failed to decompress neurons data";
                return false;
            }
            
            // Validate checksum
            uint32_t sectionChecksum = sections_[neuronsSectionIndex].checksum;
            uint32_t computedChecksum = ChecksumCalculator::crc32(decompressedData.data(), dataSize);
            
            if (sectionChecksum != computedChecksum) {
                error_ = "Neurons section checksum validation failed";
                return false;
            }
            
            // Read neuron data
            const float* ptr = reinterpret_cast<const float*>(decompressedData.data());
            size_t floatCount = dataSize / sizeof(float);
            
            if (floatCount >= sizeof(data) / sizeof(float)) {
                std::memcpy(&data, decompressedData.data(), dataSize);
            } else {
                // Partial data - fill what we can
                std::vector<float> temp(ptr, ptr + floatCount);
                std::memcpy(&data, temp.data(), temp.size() * sizeof(float));
            }
        } else {
            // Not compressed
            std::vector<uint8_t> rawData(dataSize);
            stream_.read(reinterpret_cast<char*>(rawData.data()), dataSize);
            
            // Validate checksum
            uint32_t sectionChecksum = sections_[neuronsSectionIndex].checksum;
            uint32_t computedChecksum = ChecksumCalculator::crc32(rawData.data(), dataSize);
            
            if (sectionChecksum != computedChecksum) {
                error_ = "Neurons section checksum validation failed";
                return false;
            }
            
            // Read neuron data
            std::memcpy(&data, rawData.data(), sizeof(data));
        }
        
        // Validate neuron data counts
        if (data.membranePotential.size() > data.restingPotential.size() ||
            data.restingPotential.size() > data.threshold.size() ||
            data.threshold.size() > data.resetPotential.size() ||
            data.resetPotential.size() > data.leakConductance.size()) {
            error_ = "Neurons checkpoint data size mismatch";
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading neurons: ") + e.what();
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    try {
        // Find synapses section
        size_t synapsesSectionIndex = 0;
        for (size_t i = 0; i < sections_.size(); ++i) {
            if (sections_[i].type == static_cast<uint32_t>(CheckpointSection::Synapses)) {
                synapsesSectionIndex = i;
                break;
            }
        }
        
        if (sections_[synapsesSectionIndex].size == 0) {
            return true; // Empty data is valid
        }
        
        // Seek to synapses section
        stream_.seekg(streamPos_ + sections_[synapsesSectionIndex].offset);
        
        // Decompress if needed
        size_t dataSize = sections_[synapsesSectionIndex].size;
        size_t compressedSize = sections_[synapsesSectionIndex].compressedSize;
        
        if (compressedSize > 0) {
            std::vector<uint8_t> compressedData(compressedSize);
            std::vector<uint8_t> decompressedData(dataSize);
            
            stream_.read(reinterpret_cast<char*>(compressedData.data()), compressedSize);
            
            if (uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), 
                          &dataSize, 
                          reinterpret_cast<const Bytef*>(compressedData.data()), 
                          compressedSize) != Z_OK) {
                error_ = "Failed to decompress synapses data";
                return false;
            }
            
            // Validate checksum
            uint32_t sectionChecksum = sections_[synapsesSectionIndex].checksum;
            uint32_t computedChecksum = ChecksumCalculator::crc32(decompressedData.data(), dataSize);
            
            if (sectionChecksum != computedChecksum) {
                error_ = "Synapses section checksum validation failed";
                return false;
            }
            
            // Read synapse data
            std::memcpy(&data, decompressedData.data(), sizeof(data));
        } else {
            // Not compressed
            std::vector<uint8_t> rawData(dataSize);
            stream_.read(reinterpret_cast<char*>(rawData.data()), dataSize);
            
            // Validate checksum
            uint32_t sectionChecksum = sections_[synapsesSectionIndex].checksum;
            uint32_t computedChecksum = ChecksumCalculator::crc32(rawData.data(), dataSize);
            
            if (sectionChecksum != computedChecksum) {
                error_ = "Synapses section checksum validation failed";
                return false;
            }
            
            // Read synapse data
            std::memcpy(&data, rawData.data(), sizeof(data));
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading synapses: ") + e.what();
        return false;
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    try {
        // Find section
        size_t sectionIndex = 0;
        for (size_t i = 0; i < sections_.size(); ++i) {
            if (sections_[i].type == static_cast<uint32_t>(sectionType)) {
                sectionIndex = i;
                break;
            }
        }
        
        if (sections_[sectionIndex].size == 0) {
            return std::vector<uint8_t>(); // Empty section
        }
        
        // Seek to section
        stream_.seekg(streamPos_ + sections_[sectionIndex].offset);
        
        // Decompress if needed
        size_t dataSize = sections_[sectionIndex].size;
        size_t compressedSize = sections_[sectionIndex].compressedSize;
        
        std::vector<uint8_t> result(dataSize);
        
        if (compressedSize > 0) {
            std::vector<uint8_t> compressedData(compressedSize);
            stream_.read(reinterpret_cast<char*>(compressedData.data()), compressedSize);
            
            std::vector<uint8_t> decompressedData(dataSize);
            if (uncompress(reinterpret_cast<Bytef*>(decompressedData.data()), 
                          &dataSize, 
                          reinterpret_cast<const Bytef*>(compressedData.data()), 
                          compressedSize) != Z_OK) {
                error_ = "Failed to decompress section data";
                return std::vector<uint8_t>();
            }
            
            // Validate checksum
            uint32_t sectionChecksum = sections_[sectionIndex].checksum;
            uint32_t computedChecksum = ChecksumCalculator::crc32(decompressedData.data(), dataSize);
            
            if (sectionChecksum != computedChecksum) {
                error_ = "Section checksum validation failed";
                return std::vector<uint8_t>();
            }
            
            std::memcpy(result.data(), decompressedData.data(), dataSize);
        } else {
            // Not compressed
            stream_.read(reinterpret_cast<char*>(result.data()), dataSize);
            
            // Validate checksum
            uint32_t sectionChecksum = sections_[sectionIndex].checksum;
            uint32_t computedChecksum = ChecksumCalculator::crc32(result.data(), dataSize);
            
            if (sectionChecksum != computedChecksum) {
                error_ = "Section checksum validation failed";
                return std::vector<uint8_t>();
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading section: ") + e.what();
        return std::vector<uint8_t>();
    }
}

bool CheckpointReader::validate() const {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint file not open";
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Checkpoint header validation failed";
            return false;
        }
        
        // Validate all sections
        for (const auto& section : sections_) {
            if (section.type >= static_cast<uint32_t>(CheckpointSection::End)) {
                error_ = "Invalid section type";
                return false;
            }
            
            if (section.size > header_.totalSize) {
                error_ = "Section size exceeds checkpoint size";
                return false;
            }
            
            if (section.offset >= header_.totalSize) {
                error_ = "Section offset exceeds checkpoint size";
                return false;
            }
        }
        
        // Validate checksum if present
        if (header_.checksum != 0) {
            uint64_t computedChecksum = computeChecksum();
            if (header_.checksum != computedChecksum) {
                error_ = "Checkpoint header checksum validation failed";
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception validating checkpoint: ") + e.what();
        return false;
    }
}

double CheckpointReader::getProgress() const {
    if (!stream_.is_open() || sections_.empty()) {
        return 0.0;
    }
    
    size_t totalSize = 0;
    for (const auto& section : sections_) {
        totalSize += section.size;
    }
    
    if (totalSize == 0) {
        return 1.0;
    }
    
    size_t readSize = 0;
    for (const auto& section : sections_) {
        readSize += section.size;
    }
    
    return static_cast<double>(readSize) / static_cast<double>(totalSize);
}

uint64_t CheckpointReader::computeChecksum() const {
    // Simple implementation - read entire file and compute checksum
    stream_.seekg(sizeof(CheckpointHeader));
    for (const auto& section : sections_) {
        stream_.seekg(streamPos_ + section.offset);
        // Read section data and compute checksum
        // This is simplified - full implementation would read actual data
    }
    return 0; // Placeholder
}

// CheckpointWriter implementation

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        filepath_ = filepath;
        compression_ = compression;
        
        // Create output stream
        stream_.open(filepath_, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath_;
            return false;
        }
        
        // Initialize header with zeros
        std::memset(&header_, 0, sizeof(CheckpointHeader));
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        finalize();
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
    header_.timestamp = static_cast<uint64_t>(std::time(nullptr));
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        // Convert data to binary
        std::vector<uint8_t> sectionData(sizeof(data));
        std::memcpy(sectionData.data(), &data, sizeof(data));
        
        return writeSection(CheckpointSection::Neurons, sectionData.data(), sectionData.size());
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing neurons: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Convert data to binary
        std::vector<uint8_t> sectionData(sizeof(data));
        std::memcpy(sectionData.data(), &data, sizeof(data));
        
        return writeSection(CheckpointSection::Synapses, sectionData.data(), sectionData.size());
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing synapses: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        // Compress if needed
        std::vector<uint8_t> compressedData;
        size_t compressedSize = size;
        
        if (compression_ != CompressionLevel::None && size > 1024) {
            compressedData.resize(compressBuffer_.size());
            if (compress(reinterpret_cast<Bytef*>(compressBuffer_.data()), 
                        &compressedSize, 
                        reinterpret_cast<const Bytef*>(data), 
                        size) != Z_OK) {
                error_ = "Failed to compress section data";
                return false;
            }
            compressedData.resize(compressedSize);
        } else {
            compressedData.assign(static_cast<const uint8_t*>(data), 
                                 static_cast<const uint8_t*>(data) + size);
        }
        
        // Compute checksum
        uint32_t sectionChecksum = ChecksumCalculator::crc32(compressedData.data(), compressedSize);
        
        // Calculate offset
        uint64_t offset = sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader);
        
        // Write section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = offset;
        sectionHeader.size = size;
        sectionHeader.compressedSize = compression_ != CompressionLevel::None && size > 1024 ? compressedSize : 0;
        sectionHeader.checksum = sectionChecksum;
        
        // Write section header to buffer
        std::vector<uint8_t> sectionHeaderData(sizeof(SectionHeader));
        std::memcpy(sectionHeaderData.data(), &sectionHeader, sizeof(SectionHeader));
        
        stream_.write(reinterpret_cast<const char*>(sectionHeaderData.data()), sizeof(SectionHeader));
        
        // Write section data
        stream_.write(reinterpret_cast<const char*>(compressedData.data()), compressedSize);
        
        // Pad to alignment
        padToAlignment(8);
        
        // Add to sections list
        sections_.push_back(sectionHeader);
        bytesWritten_ += sizeof(SectionHeader) + compressedSize + (padToAlignment(8) - (compressedSize % 8));
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing section: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint file not open for finalization";
            return false;
        }
        
        // Compute total checksum
        header_.checksum = computeChecksum();
        header_.totalSize = bytesWritten_ + sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader);
        
        // Write header
        if (!writeHeader()) {
            return false;
        }
        
        // Finalize compression buffer
        compressBuffer_.clear();
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception finalizing checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    std::remove(filepath_.c_str());
    sections_.clear();
    compressBuffer_.clear();
    bytesWritten_ = 0;
}

uint64_t CheckpointWriter::computeChecksum() const {
    // Simple implementation - compute checksum of all written data
    // This is a simplified version - full implementation would be more robust
    return 0; // Placeholder
}

bool CheckpointWriter::writeHeader() {
    try {
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        return stream_.good();
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing header: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    try {
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
        return stream_.good();
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing section header: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    if (!stream_.good()) {
        return false;
    }
    
    size_t currentPos = stream_.tellp();
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> paddingData(padding, 0);
        stream_.write(reinterpret_cast<const char*>(paddingData.data()), padding);
    }
    
    return stream_.good();
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
    
    // Ensure directory exists
    std::filesystem::create_directories(checkpointDir_);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    try {
        std::string filename = checkpointDir_ + "/";
        if (!name.empty()) {
            filename += name + "_";
        }
        filename += std::to_string(lastSaveStep_) + ".nckpt";
        
        // Create checkpoint state
        CheckpointState state;
        
        // Populate state from brain (using external implementation)
        // This would be implemented by the brain to populate this state
        // For now, return false to indicate not implemented
        return false;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception saving checkpoint: ") + e.what();
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    try {
        std::string filename = checkpointDir_ + "/";
        if (!name.empty()) {
            filename += name + "_";
        }
        filename += "*.nckpt"; // Would need to implement pattern matching
        
        // Load checkpoint state (using external implementation)
        // This would be implemented by the brain to restore this state
        // For now, return false to indicate not implemented
        return false;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception loading checkpoint: ") + e.what();
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    // This would scan the checkpoint directory
    // Placeholder implementation
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    // This would delete old checkpoints
    // Placeholder implementation
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep <= lastSaveStep_) {
        return false;
    }
    
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

} // namespace nlm

// CheckpointState implementation

bool CheckpointState::create(CheckpointState& state, const Brain& brain) {
    try {
        // Copy basic header information
        state.header.majorVersion = CHECKPOINT_VERSION_MAJOR;
        state.header.minorVersion = CHECKPOINT_VERSION_MINOR;
        state.header.patchVersion = CHECKPOINT_VERSION_PATCH;
        state.header.timestamp = static_cast<uint64_t>(std::time(nullptr));
        state.header.neuronCount = brain.getTotalNeuronCount();
        state.header.synapseCount = brain.getTotalSynapseCount();
        state.header.regionCount = brain.getRegionCount();
        state.header.step = brain.pImpl->currentStep;
        state.header.simulationTime = brain.pImpl->currentTime;
        
        // Collect neuron data
        state.neuronData.membranePotential.reserve(state.header.neuronCount);
        state.neuronData.restingPotential.reserve(state.header.neuronCount);
        state.neuronData.threshold.reserve(state.header.neuronCount);
        state.neuronData.resetPotential.reserve(state.header.neuronCount);
        state.neuronData.leakConductance.reserve(state.header.neuronCount);
        
        for (const auto& region : brain.getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (const auto* neuron : pop->getNeurons()) {
                    const auto& brainState = neuron->getState();
                    state.neuronData.membranePotential.push_back(brainState.membranePotential);
                    state.neuronData.restingPotential.push_back(brainState.restingPotential);
                    state.neuronData.threshold.push_back(brainState.threshold);
                    state.neuronData.resetPotential.push_back(brainState.resetPotential);
                    state.neuronData.leakConductance.push_back(brainState.leakConductance);
                    state.neuronData.firingState.push_back(static_cast<uint8_t>(brainState.firingState));
                    state.neuronData.refractoryRemaining.push_back(brainState.refractoryRemaining);
                    state.neuronData.refractoryPeriod.push_back(brainState.refractoryPeriod);
                    state.neuronData.lastSpikeTime.push_back(brainState.lastSpikeTime);
                    state.neuronData.neuronType.push_back(static_cast<uint64_t>(neuron->getNeuronType()));
                    state.neuronData.regionId.push_back(region->getId().index());
                    state.neuronData.populationId.push_back(pop->getId().index());
                }
            }
        }
        
        // Collect synapse data
        for (const auto& region : brain.getRegions()) {
            for (const auto* syn : region->getSynapses()) {
                state.synapseData.sourceNeuron.push_back(syn->getSourceNeuron().index());
                state.synapseData.destinationNeuron.push_back(syn->getDestinationNeuron().index());
                state.synapseData.weight.push_back(syn->getWeight());
                state.synapseData.delay.push_back(syn->getDelay());
                state.synapseData.synapseType.push_back(static_cast<uint8_t>(syn->getType()));
                state.synapseData.eligibilityTrace.push_back(syn->getEligibilityTrace());
                state.synapseData.efficacy.push_back(syn->getEfficacy());
                state.synapseData.shortTermDepression.push_back(syn->getShortTermDepression());
                state.synapseData.shortTermFacilitation.push_back(syn->getShortTermFacilitation());
            }
        }
        
        // Create memory data
        if (brain.getWorkingMemory()) {
            state.memoryData.workingMemoryCapacity = brain.getWorkingMemory()->getCapacity();
            state.memoryData.workingMemoryActiveNeurons = brain.getWorkingMemory()->getMemoryNeurons().size();
            auto workingMemoryNeurons = brain.getWorkingMemory()->getMemoryNeurons();
            for (const auto& neuronId : workingMemoryNeurons) {
                state.memoryData.workingMemoryNeurons.push_back(neuronId.index());
            }
            
            auto workingMemoryData = brain.getWorkingMemory()->getMemoryData();
            for (const auto& activation : workingMemoryData) {
                state.memoryData.workingMemoryActivations.push_back(activation);
            }
        }
        
        // Create neuromodulation data
        if (brain.getDopamine() || brain.getAcetylcholine() || brain.getNorepinephrine() ||
            brain.getSerotonin() || brain.getCuriosity() || brain.getPredictionErrorSignal() || brain.getNovelty()) {
            
            if (brain.getDopamine()) {
                state.neuromodulationData.dopamineLevel = brain.getDopamine()->getLevel();
                state.neuromodulationData.dopaminePlasticityFactor = brain.getDopamine()->getPlasticityFactor();
            }
            if (brain.getAcetylcholine()) {
                state.neuromodulationData.acetylcholineLevel = brain.getAcetylcholine()->getLevel();
                state.neuromodulationData.acetylcholinePlasticityFactor = brain.getAcetylcholine()->getPlasticityFactor();
            }
            if (brain.getNorepinephrine()) {
                state.neuromodulationData.norepinephrineLevel = brain.getNorepinephrine()->getLevel();
                state.neuromodulationData.norepinephrinePlasticityFactor = brain.getNorepinephrine()->getPlasticityFactor();
            }
            if (brain.getSerotonin()) {
                state.neuromodulationData.serotoninLevel = brain.getSerotonin()->getLevel();
                state.neuromodulationData.serotoninPlasticityFactor = brain.getSerotonin()->getPlasticityFactor();
            }
            if (brain.getCuriosity()) {
                state.neuromodulationData.curiosityLevel = brain.getCuriosity()->getLevel();
            }
            if (brain.getPredictionErrorSignal()) {
                state.neuromodulationData.predictionErrorLevel = brain.getPredictionErrorSignal()->getError();
            }
            if (brain.getNovelty()) {
                state.neuromodulationData.noveltyLevel = brain.getNovelty()->getLevel();
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        // Handle error
        return false;
    }
}

bool CheckpointState::load(Brain& brain, const CheckpointState& state) {
    try {
        // Restore neuron data
        size_t neuronIndex = 0;
        for (const auto& region : brain.getRegions()) {
            for (const auto& pop : region->getPopulations()) {
                for (auto* neuron : pop->getNeurons()) {
                    if (neuronIndex < state.neuronData.membranePotential.size()) {
                        neuron->setMembranePotential(state.neuronData.membranePotential[neuronIndex]);
                        neuron->setRestingPotential(state.neuronData.restingPotential[neuronIndex]);
                        neuron->setThreshold(state.neuronData.threshold[neuronIndex]);
                        neuron->setResetPotential(state.neuronData.resetPotential[neuronIndex]);
                        neuron->setLeakConductance(state.neuronData.leakConductance[neuronIndex]);
                        neuron->setFiringState(static_cast<FiringState>(state.neuronData.firingState[neuronIndex]));
                        neuron->setRefractoryPeriod(state.neuronData.refractoryPeriod[neuronIndex]);
                    }
                    neuronIndex++;
                }
            }
        }
        
        // Restore neuromodulation data
        if (brain.getDopamine()) {
            brain.getDopamine()->setLevel(state.neuromodulationData.dopamineLevel);
        }
        if (brain.getAcetylcholine()) {
            brain.getAcetylcholine()->setLevel(state.neuromodulationData.acetylcholineLevel);
        }
        if (brain.getNorepinephrine()) {
            brain.getNorepinephrine()->setLevel(state.neuromodulationData.norepinephrineLevel);
        }
        if (brain.getSerotonin()) {
            brain.getSerotonin()->setLevel(state.neuromodulationData.serotoninLevel);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        // Handle error
        return false;
    }
}

} // namespace nlm