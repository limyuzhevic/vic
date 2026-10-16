#include "CheckpointSystem.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <cmath>

namespace nlm {

CheckpointReader::CheckpointReader()
    : streamPos_(0)
{
}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    if (isOpen()) {
        close();
    }
    
    // Open file for binary reading
    stream_.open(filepath, std::ios::binary | std::ios::in);
    if (!stream_.is_open()) {
        error_ = "Failed to open file for reading: " + filepath;
        return false;
    }
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (!stream_.good()) {
        error_ = "Failed to read checkpoint header";
        close();
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header (magic number or version mismatch)";
        close();
        return false;
    }
    
    // Validate checksum
    uint64_t checksum = ChecksumCalculator::combine(
        ChecksumCalculator::crc64(&header_, sizeof(header_)),
        header_.totalSize);
    
    if (checksum != header_.checksum) {
        error_ = "Checkpoint header checksum validation failed";
        close();
        return false;
    }
    
    streamPos_ = sizeof(header_);
    sections_.clear();
    
    // Read section headers until we reach the end
    while (stream_.good() && !stream_.eof()) {
        SectionHeader section;
        stream_.read(reinterpret_cast<char*>(&section), sizeof(section));
        if (!stream_.good() && !stream_.eof()) {
            error_ = "Failed to read section header";
            close();
            return false;
        }
        
        if (stream_.eof() || section.type == static_cast<uint32_t>(CheckpointSection::End)) {
            break;
        }
        
        if (section.size == 0) {
            error_ = "Invalid section header (zero size)";
            close();
            return false;
        }
        
        sections_.push_back(section);
        streamPos_ += sizeof(section);
    }
    
    // Validate all sections have checksums (if they had checksums)
    // For now, just validate basic structure
    
    return true;
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    
    error_.clear();
}

bool CheckpointReader::isOpen() const {
    return stream_.is_open();
}

const CheckpointHeader& CheckpointReader::getHeader() const {
    return header_;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    if (!isOpen()) {
        error_ = "CheckpointReader: Not open";
        return false;
    }
    
    // Read neuron data from current section
    size_t neuronCount = 0;
    stream_.read(reinterpret_cast<char*>(&neuronCount), sizeof(neuronCount));
    
    if (!stream_.good()) {
        error_ = "Failed to read neuron count";
        return false;
    }
    
    // Resize output vectors
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
    
    // Read each vector
    if (neuronCount > 0) {
        stream_.read(reinterpret_cast<char*>(data.membranePotential.data()),
                    neuronCount * sizeof(float));
    }
    
    stream_.read(reinterpret_cast<char*>(data.restingPotential.data()),
                neuronCount * sizeof(float));
    
    stream_.read(reinterpret_cast<char*>(data.threshold.data()),
                neuronCount * sizeof(float));
    
    stream_.read(reinterpret_cast<char*>(data.resetPotential.data()),
                neuronCount * sizeof(float));
    
    stream_.read(reinterpret_cast<char*>(data.leakConductance.data()),
                neuronCount * sizeof(float));
    
    stream_.read(reinterpret_cast<char*>(data.firingState.data()),
                neuronCount * sizeof(uint8_t));
    
    stream_.read(reinterpret_cast<char*>(data.refractoryRemaining.data()),
                neuronCount * sizeof(uint32_t));
    
    stream_.read(reinterpret_cast<char*>(data.refractoryPeriod.data()),
                neuronCount * sizeof(uint32_t));
    
    stream_.read(reinterpret_cast<char*>(data.lastSpikeTime.data()),
                neuronCount * sizeof(float));
    
    stream_.read(reinterpret_cast<char*>(data.neuronType.data()),
                neuronCount * sizeof(uint64_t));
    
    stream_.read(reinterpret_cast<char*>(data.regionId.data()),
                neuronCount * sizeof(uint64_t));
    
    stream_.read(reinterpret_cast<char*>(data.populationId.data()),
                neuronCount * sizeof(uint64_t));
    
    if (!stream_.good()) {
        error_ = "Failed to read neuron data";
        return false;
    }
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    if (!isOpen()) {
        error_ = "CheckpointReader: Not open";
        return false;
    }
    
    // Read synapse data from current section
    size_t synapseCount = 0;
    stream_.read(reinterpret_cast<char*>(&synapseCount), sizeof(synapseCount));
    
    if (!stream_.good()) {
        error_ = "Failed to read synapse count";
        return false;
    }
    
    // Resize output vectors
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
    
    // Read each vector
    if (synapseCount > 0) {
        stream_.read(reinterpret_cast<char*>(data.sourceNeuron.data()),
                    synapseCount * sizeof(uint64_t));
        
        stream_.read(reinterpret_cast<char*>(data.destinationNeuron.data()),
                    synapseCount * sizeof(uint64_t));
        
        stream_.read(reinterpret_cast<char*>(data.weight.data()),
                    synapseCount * sizeof(float));
        
        stream_.read(reinterpret_cast<char*>(data.delay.data()),
                    synapseCount * sizeof(uint32_t));
        
        stream_.read(reinterpret_cast<char*>(data.synapseType.data()),
                    synapseCount * sizeof(uint8_t));
        
        stream_.read(reinterpret_cast<char*>(data.plasticityFlags.data()),
                    synapseCount * sizeof(uint8_t));
        
        stream_.read(reinterpret_cast<char*>(data.eligibilityTrace.data()),
                    synapseCount * sizeof(float));
        
        stream_.read(reinterpret_cast<char*>(data.efficacy.data()),
                    synapseCount * sizeof(float));
        
        stream_.read(reinterpret_cast<char*>(data.shortTermDepression.data()),
                    synapseCount * sizeof(float));
        
        stream_.read(reinterpret_cast<char*>(data.shortTermFacilitation.data()),
                    synapseCount * sizeof(float));
    }
    
    if (!stream_.good()) {
        error_ = "Failed to read synapse data";
        return false;
    }
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    if (!isOpen()) {
        error_ = "CheckpointReader: Not open";
        return std::vector<uint8_t>();
    }
    
    // Find the requested section
    size_t sectionIndex = 0;
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            break;
        }
        ++sectionIndex;
    }
    
    if (sectionIndex >= sections_.size()) {
        error_ = "Section not found in checkpoint";
        return std::vector<uint8_t>();
    }
    
    const auto& section = sections_[sectionIndex];
    
    // Seek to section data
    stream_.seekg(streamPos_);
    for (size_t i = 0; i <= sectionIndex; ++i) {
        if (i < sectionIndex) {
            // Skip previous section headers
            stream_.seekg(stream_.tellg() + sizeof(SectionHeader));
        }
        // Skip current section header
        stream_.seekg(stream_.tellg() + sizeof(SectionHeader));
    }
    
    // Read section data
    std::vector<uint8_t> data(section.size);
    stream_.read(reinterpret_cast<char*>(data.data()), section.size);
    
    if (!stream_.good()) {
        error_ = "Failed to read section data";
        return std::vector<uint8_t>();
    }
    
    // Skip section data for next read
    stream_.seekg(stream_.tellg() + section.size);
    
    // Pad to alignment
    size_t padding = (8 - (section.size % 8)) % 8;
    if (padding > 0) {
        stream_.seekg(stream_.tellg() + padding);
    }
    
    return data;
}

bool CheckpointReader::validate() const {
    if (!isOpen()) {
        error_ = "CheckpointReader: Not open";
        return false;
    }
    
    // Check header validation
    if (!header_.validate()) {
        error_ = "Checkpoint header validation failed";
        return false;
    }
    
    // Verify checksum
    uint64_t checksum = ChecksumCalculator::combine(
        ChecksumCalculator::crc64(&header_, sizeof(header_)),
        header_.totalSize);
    
    if (checksum != header_.checksum) {
        error_ = "Checkpoint checksum validation failed";
        return false;
    }
    
    // Check that we can read all sections (basic validation)
    // For now, just return true if header is valid
    return true;
}

const std::string& CheckpointReader::getError() const {
    return error_;
}

double CheckpointReader::getProgress() const {
    if (!isOpen() || sections_.empty()) {
        return 0.0;
    }
    
    // Simple progress: percentage of sections read
    static size_t currentSectionIndex = 0;
    return static_cast<double>(currentSectionIndex) / sections_.size();
}

} // namespace nlm