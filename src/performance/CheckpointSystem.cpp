// CheckpointSystem implementation
// Provides working checkpoint reader/writer classes with binary format

#include "CheckpointSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <cstring>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace nlm {

CheckpointReader::CheckpointReader()
    : stream_(), header_(), sections_(), error_(), streamPos_(0) {
}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        close();
        stream_.open(filepath, std::ios::binary | std::ios::in);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (stream_.fail() || stream_.bad()) {
            error_ = "Failed to read checkpoint header";
            close();
            return false;
        }
        
        // Validate header
        if (!header_.validate()) {
            error_ = "Invalid checkpoint file (wrong magic or version)";
            close();
            return false;
        }
        
        // Read section headers
        size_t sectionCount = header_.totalSize / sizeof(SectionHeader);
        sections_.resize(sectionCount);
        
        for (size_t i = 0; i < sectionCount; ++i) {
            stream_.read(reinterpret_cast<char*>(&sections_[i]), sizeof(SectionHeader));
            if (stream_.fail() || stream_.bad()) {
                error_ = "Failed to read section headers";
                close();
                return false;
            }
        }
        
        streamPos_ = sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader);
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception opening checkpoint: " + std::string(e.what());
        close();
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
        auto sectionIt = std::find_if(sections_.begin(), sections_.end(),
            [](const SectionHeader& h) { return h.type == static_cast<uint32_t>(CheckpointSection::Neurons); });
        
        if (sectionIt == sections_.end()) {
            error_ = "Neurons section not found in checkpoint";
            return false;
        }
        
        // Save current position
        auto currentPos = stream_.tellg();
        
        // Seek to neurons section
        stream_.seekg(sectionIt->offset);
        
        // Read neuron count
        uint64_t neuronCount = 0;
        stream_.read(reinterpret_cast<char*>(&neuronCount), sizeof(neuronCount));
        
        if (stream_.fail() || stream_.bad()) {
            error_ = "Failed to read neuron count from checkpoint";
            stream_.clear();
            stream_.seekg(currentPos);
            return false;
        }
        
        // Resize data vectors
        data.membranePotential.resize(neuronCount);
        data.restingPotential.resize(neuronCount);
        data.threshold.resize(neuronCount);
        data.resetPotential.resize(neuronCount);
        data.leakConductance.resize(neuronCount);
        data.firingState.resize(neuronCount);
        data.refractoryRemaining.resize(neuronCount);
        data.refractoryPeriod.resize(neuronCount);
        data.lastSpikeTime.resize(neuronCount);
        data.neuronType.resize(neuronCount);
        data.regionId.resize(neuronCount);
        data.populationId.resize(neuronCount);
        
        // Read neuron data
        for (size_t i = 0; i < neuronCount; ++i) {
            stream_.read(reinterpret_cast<char*>(&data.membranePotential[i]), sizeof(float));
            stream_.read(reinterpret_cast<char*>(&data.restingPotential[i]), sizeof(float));
            stream_.read(reinterpret_cast<char*>(&data.threshold[i]), sizeof(float));
            stream_.read(reinterpret_cast<char*>(&data.resetPotential[i]), sizeof(float));
            stream_.read(reinterpret_cast<char*>(&data.leakConductance[i]), sizeof(float));
            stream_.read(reinterpret_cast<char*>(&data.firingState[i]), sizeof(uint8_t));
            stream_.read(reinterpret_cast<char*>(&data.refractoryRemaining[i]), sizeof(uint32_t));
            stream_.read(reinterpret_cast<char*>(&data.refractoryPeriod[i]), sizeof(uint32_t));
            stream_.read(reinterpret_cast<char*>(&data.lastSpikeTime[i]), sizeof(float));
            stream_.read(reinterpret_cast<char*>(&data.neuronType[i]), sizeof(uint64_t));
            stream_.read(reinterpret_cast<char*>(&data.regionId[i]), sizeof(uint64_t));
            stream_.read(reinterpret_cast<char*>(&data.populationId[i]), sizeof(uint64_t));
            
            if (stream_.fail() || stream_.bad()) {
                error_ = "Failed to read neuron data from checkpoint";
                stream_.clear();
                stream_.seekg(currentPos);
                return false;
            }
        }
        
        // Restore original position
        stream_.seekg(currentPos);
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception reading neurons from checkpoint: " + std::string(e.what());
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    try {
        auto sectionIt = std::find_if(sections_.begin(), sections_.end(),
            [](const SectionHeader& h) { return h.type == static_cast<uint32_t>(CheckpointSection::Synapses); });
        
        if (sectionIt == sections_.end()) {
            error_ = "Synapses section not found in checkpoint";
            return false;
        }
        
        // Save current position
        auto currentPos = stream_.tellg();
        
        // Seek to synapses section
        stream_.seekg(sectionIt->offset);
        
        // Read synapse count
        uint64_t synapseCount = 0;
        stream_.read(reinterpret_cast<char*>(&synapseCount), sizeof(synapseCount));
        
        if (stream_.fail() || stream_.bad()) {
            error_ = "Failed to read synapse count from checkpoint";
            stream_.clear();
            stream_.seekg(currentPos);
            return false;
        }
        
        // Resize data vectors
        data.sourceNeuron.resize(synapseCount);
        data.destinationNeuron.resize(synapseCount);
        data.weight.resize(synapseCount);
        data.delay.resize(synapseCount);
        data.synapseType.resize(synapseCount);
        data.plasticityFlags.resize(synapseCount);
        data.eligibilityTrace.resize(synapseCount);
        data.efficacy.resize(synapseCount);
        data.shortTermDepression.resize(synapseCount);
        data.shortTermFacilitation.resize(synapseCount);
        
        // Read synapse data
        for (size_t i = 0; i < synapseCount; ++i) {
            stream_.read(reinterpret_cast<char*>(&data.sourceNeuron[i]), sizeof(uint64_t));
            stream_.read(reinterpret_cast<char*>(&data.destinationNeuron[i]), sizeof(uint64_t));
            stream_.read(reinterpret_cast<char*>(&data.weight[i]), sizeof(float));
            stream_.read(reinterpret_cast<char*>(&data.delay[i]), sizeof(uint32_t));
            stream_.read(reinterpret_cast<char*>(&data.synapseType[i]), sizeof(uint8_t));
            stream_.read(reinterpret_cast<char*>(&data.plasticityFlags[i]), sizeof(uint8_t));
            stream_.read(reinterpret_cast<char*>(&data.eligibilityTrace[i]), sizeof(float));
            stream_.read(reinterpret_cast<char*>(&data.efficacy[i]), sizeof(float));
            stream_.read(reinterpret_cast<char*>(&data.shortTermDepression[i]), sizeof(float));
            stream_.read(reinterpret_cast<char*>(&data.shortTermFacilitation[i]), sizeof(float));
            
            if (stream_.fail() || stream_.bad()) {
                error_ = "Failed to read synapse data from checkpoint";
                stream_.clear();
                stream_.seekg(currentPos);
                return false;
            }
        }
        
        // Restore original position
        stream_.seekg(currentPos);
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception reading synapses from checkpoint: " + std::string(e.what());
        return false;
    }
}

bool CheckpointReader::validate() const {
    if (!isOpen()) {
        error_ = "Checkpoint file is not open";
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Simple checksum validation - can be expanded
    uint64_t computedChecksum = ChecksumCalculator::fletcher64(
        reinterpret_cast<const uint64_t*>(&header_), sizeof(header_) / sizeof(uint64_t)
    );
    
    if (computedChecksum != header_.checksum) {
        error_ = "Checkpoint checksum validation failed";
        return false;
    }
    
    error_.clear();
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    auto currentPos = stream_.tellg();
    auto endPos = stream_.seekg(0, std::ios::end).tellg();
    
    double progress = static_cast<double>(currentPos) / static_cast<double>(endPos);
    return std::min(progress, 1.0);
}

CheckpointWriter::CheckpointWriter()
    : stream_(), filepath_(), compression_(CompressionLevel::None),
      header_(), sections_(), bytesWritten_(0),
      compressBuffer_() {
}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        abort();
        
        filepath_ = filepath;
        compression_ = compression;
        
        // Ensure directory exists
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        bytesWritten_ = 0;
        sections_.clear();
        
        // Initialize header
        header_.timestamp = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception creating checkpoint: " + std::string(e.what());
        abort();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
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

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        // Create section data
        std::vector<uint8_t> sectionData;
        
        // Write neuron count
        uint64_t neuronCount = data.membranePotential.size();
        sectionData.insert(sectionData.end(),
                          reinterpret_cast<const uint8_t*>(&neuronCount),
                          reinterpret_cast<const uint8_t*>(&neuronCount + 1));
        
        // Write all neuron fields
        for (size_t i = 0; i < neuronCount; ++i) {
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.membranePotential[i]),
                              reinterpret_cast<const uint8_t*>(&data.membranePotential[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.restingPotential[i]),
                              reinterpret_cast<const uint8_t*>(&data.restingPotential[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.threshold[i]),
                              reinterpret_cast<const uint8_t*>(&data.threshold[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.resetPotential[i]),
                              reinterpret_cast<const uint8_t*>(&data.resetPotential[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.leakConductance[i]),
                              reinterpret_cast<const uint8_t*>(&data.leakConductance[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.firingState[i]),
                              reinterpret_cast<const uint8_t*>(&data.firingState[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.refractoryRemaining[i]),
                              reinterpret_cast<const uint8_t*>(&data.refractoryRemaining[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.refractoryPeriod[i]),
                              reinterpret_cast<const uint8_t*>(&data.refractoryPeriod[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.lastSpikeTime[i]),
                              reinterpret_cast<const uint8_t*>(&data.lastSpikeTime[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.neuronType[i]),
                              reinterpret_cast<const uint8_t*>(&data.neuronType[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.regionId[i]),
                              reinterpret_cast<const uint8_t*>(&data.regionId[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.populationId[i]),
                              reinterpret_cast<const uint8_t*>(&data.populationId[i] + 1));
        }
        
        return writeSection(CheckpointSection::Neurons, sectionData.data(), sectionData.size());
        
    } catch (const std::exception& e) {
        error_ = "Exception writing neurons to checkpoint: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Create section data
        std::vector<uint8_t> sectionData;
        
        // Write synapse count
        uint64_t synapseCount = data.weight.size();
        sectionData.insert(sectionData.end(),
                          reinterpret_cast<const uint8_t*>(&synapseCount),
                          reinterpret_cast<const uint8_t*>(&synapseCount + 1));
        
        // Write all synapse fields
        for (size_t i = 0; i < synapseCount; ++i) {
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.sourceNeuron[i]),
                              reinterpret_cast<const uint8_t*>(&data.sourceNeuron[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.destinationNeuron[i]),
                              reinterpret_cast<const uint8_t*>(&data.destinationNeuron[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.weight[i]),
                              reinterpret_cast<const uint8_t*>(&data.weight[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.delay[i]),
                              reinterpret_cast<const uint8_t*>(&data.delay[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.synapseType[i]),
                              reinterpret_cast<const uint8_t*>(&data.synapseType[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.plasticityFlags[i]),
                              reinterpret_cast<const uint8_t*>(&data.plasticityFlags[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.eligibilityTrace[i]),
                              reinterpret_cast<const uint8_t*>(&data.eligibilityTrace[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.efficacy[i]),
                              reinterpret_cast<const uint8_t*>(&data.efficacy[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.shortTermDepression[i]),
                              reinterpret_cast<const uint8_t*>(&data.shortTermDepression[i] + 1));
            sectionData.insert(sectionData.end(),
                              reinterpret_cast<const uint8_t*>(&data.shortTermFacilitation[i]),
                              reinterpret_cast<const uint8_t*>(&data.shortTermFacilitation[i] + 1));
        }
        
        return writeSection(CheckpointSection::Synapses, sectionData.data(), sectionData.size());
        
    } catch (const std::exception& e) {
        error_ = "Exception writing synapses to checkpoint: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint file is not open for writing";
            return false;
        }
        
        // Pad to 8-byte alignment for performance
        size_t alignedSize = size;
        if (size % 8 != 0) {
            alignedSize = size + (8 - (size % 8));
        }
        
        // Record current position
        uint64_t offset = stream_.tellp();
        
        // Write section data
        stream_.write(reinterpret_cast<const char*>(data), size);
        if (stream_.fail() || stream_.bad()) {
            error_ = "Failed to write section data to checkpoint";
            return false;
        }
        
        // Pad if needed
        if (alignedSize > size) {
            std::vector<uint8_t> padding(alignedSize - size, 0);
            stream_.write(reinterpret_cast<const char*>(padding.data()), padding.size());
        }
        
        // Create section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = offset;
        sectionHeader.size = size;
        sectionHeader.compressedSize = (compression_ != CompressionLevel::None) ? alignedSize : 0;
        sectionHeader.checksum = ChecksumCalculator::crc32(data, size);
        
        // Write section header
        stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(SectionHeader));
        if (stream_.fail() || stream_.bad()) {
            error_ = "Failed to write section header to checkpoint";
            return false;
        }
        
        // Update header for size calculation
        header_.totalSize += sizeof(SectionHeader) + alignedSize;
        
        sections_.push_back(sectionHeader);
        bytesWritten_ += size + sizeof(SectionHeader);
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception writing checkpoint section: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint file is not open for writing";
            return false;
        }
        
        // Compute checksum
        header_.checksum = ChecksumCalculator::fletcher64(
            reinterpret_cast<const uint64_t*>(&header_), sizeof(header_) / sizeof(uint64_t)
        );
        
        // Write header at the beginning (after we know the total size)
        stream_.clear();
        stream_.seekp(0);
        
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        if (stream_.fail() || stream_.bad()) {
            error_ = "Failed to write checkpoint header";
            return false;
        }
        
        // Seek back to end for footer
        stream_.seekp(0, std::ios::end);
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception finalizing checkpoint: " + std::string(e.what());
        return false;
    }
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    // Remove file if it exists (partial write)
    if (!filepath_.empty()) {
        std::filesystem::path filepath(filepath_);
        if (std::filesystem::exists(filepath)) {
            std::filesystem::remove(filepath);
        }
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> result;
    
    auto sectionIt = std::find_if(sections_.begin(), sections_.end(),
        [](const SectionHeader& h) { return h.type == static_cast<uint32_t>(sectionType); });
    
    if (sectionIt == sections_.end()) {
        error_ = "Section not found in checkpoint";
        return result;
    }
    
    auto currentPos = stream_.tellg();
    stream_.seekg(sectionIt->offset);
    
    result.resize(sectionIt->size);
    stream_.read(reinterpret_cast<char*>(result.data()), result.size());
    
    stream_.seekg(currentPos);
    
    return result;
}

// End of CheckpointSystem.cpp
} // namespace nlm