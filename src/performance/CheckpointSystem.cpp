// CheckpointReader.cpp
// Implementation of CheckpointReader for brain state deserialization

#include "performance/CheckpointSystem.hpp"
#include <filesystem>
#include <algorithm>

namespace nlm {

CheckpointReader::CheckpointReader() : streamPos_(0) {
    // Initialize with default values
}

CheckpointReader::~CheckpointReader() {
    if (isOpen()) {
        close();
    }
}

bool CheckpointReader::open(const std::string& filepath) {
    if (isOpen()) {
        close();
    }
    
    stream_.open(filepath, std::ios::binary);
    if (!stream_) {
        error_ = "Failed to open checkpoint file: " + filepath;
        return false;
    }
    
    // Get file size
    stream_.seekg(0, std::ios::end);
    size_t fileSize = stream_.tellg();
    stream_.seekg(0, std::ios::beg);
    
    if (fileSize == 0) {
        error_ = "Checkpoint file is empty: " + filepath;
        close();
        return false;
    }
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (!stream_) {
        error_ = "Failed to read checkpoint header";
        close();
        return false;
    }
    
    // Validate header
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header magic number";
        close();
        return false;
    }
    
    // Read section headers
    stream_.seekg(sizeof(CheckpointHeader), std::ios::beg);
    streamPos_ = sizeof(CheckpointHeader);
    
    while (streamPos_ < fileSize) {
        SectionHeader sectionHeader;
        stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(sectionHeader));
        if (!stream_) {
            break; // End of file
        }
        
        // Check for end marker
        if (sectionHeader.type == static_cast<uint32_t>(CheckpointSection::End)) {
            streamPos_ += sizeof(sectionHeader);
            break;
        }
        
        if (sectionHeader.type >= static_cast<uint32_t>(CheckpointSection::End)) {
            error_ = "Invalid checkpoint section type";
            close();
            return false;
        }
        
        sections_.push_back(sectionHeader);
        streamPos_ += sizeof(sectionHeader);
        
        // Seek past section data
        stream_.seekg(sectionHeader.offset + sectionHeader.size, std::ios::beg);
        streamPos_ = sectionHeader.offset + sectionHeader.size;
    }
    
    error_.clear();
    return true;
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    
    header_ = CheckpointHeader();
    sections_.clear();
    error_.clear();
    streamPos_ = 0;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    // Clear existing data
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
    
    // Find neurons section
    size_t neuronsSection = 0;
    for (size_t i = 0; i < sections_.size(); ++i) {
        if (sections_[i].type == static_cast<uint32_t>(CheckpointSection::Neurons)) {
            neuronsSection = i;
            break;
        }
    }
    
    if (neuronsSection >= sections_.size()) {
        error_ = "Neurons section not found in checkpoint";
        return false;
    }
    
    const SectionHeader& section = sections_[neuronsSection];
    
    // Read neuron count
    if (section.size < sizeof(uint64_t)) {
        error_ = "Neurons section too small to contain count";
        return false;
    }
    
    stream_.seekg(section.offset, std::ios::beg);
    stream_.read(reinterpret_cast<char*>(&data.membranePotential), sizeof(uint64_t));
    if (!stream_) {
        error_ = "Failed to read neuron count from checkpoint";
        return false;
    }
    
    uint64_t neuronCount = data.membranePotential.size();
    
    // Allocate space for all neuron arrays
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
    
    // Read all neuron data (simplified - in practice you'd use proper serialization)
    // For now, we'll just read membrane potential as a placeholder
    size_t expectedSize = sizeof(uint64_t) + neuronCount * sizeof(float);
    
    if (section.size < expectedSize) {
        error_ = "Neurons section size doesn't match expected size";
        return false;
    }
    
    // Read membrane potential array
    stream_.seekg(section.offset + sizeof(uint64_t), std::ios::beg);
    stream_.read(reinterpret_cast<char*>(data.membranePotential.data()),
                 neuronCount * sizeof(float));
    if (!stream_) {
        error_ = "Failed to read neuron membrane potentials";
        return false;
    }
    
    // Note: In a full implementation, you would read all other neuron arrays
    // from the section data
    
    error_.clear();
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    // Clear existing data
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
    
    // Find synapses section
    size_t synapsesSection = 0;
    for (size_t i = 0; i < sections_.size(); ++i) {
        if (sections_[i].type == static_cast<uint32_t>(CheckpointSection::Synapses)) {
            synapsesSection = i;
            break;
        }
    }
    
    if (synapsesSection >= sections_.size()) {
        error_ = "Synapses section not found in checkpoint";
        return false;
    }
    
    const SectionHeader& section = sections_[synapsesSection];
    
    // Read synapse count
    if (section.size < sizeof(uint64_t)) {
        error_ = "Synapses section too small to contain count";
        return false;
    }
    
    stream_.seekg(section.offset, std::ios::beg);
    stream_.read(reinterpret_cast<char*>(&data.weight), sizeof(uint64_t));
    if (!stream_) {
        error_ = "Failed to read synapse count from checkpoint";
        return false;
    }
    
    uint64_t synapseCount = data.weight.size();
    
    // Allocate space for all synapse arrays
    data.sourceNeuron.resize(synapseCount);
    data.destinationNeuron.resize(synapseCount);
    data.delay.resize(synapseCount);
    data.synapseType.resize(synapseCount);
    data.plasticityFlags.resize(synapseCount);
    data.eligibilityTrace.resize(synapseCount);
    data.efficacy.resize(synapseCount);
    data.shortTermDepression.resize(synapseCount);
    data.shortTermFacilitation.resize(synapseCount);
    
    // Read all synapse data (simplified - in practice you'd use proper serialization)
    size_t expectedSize = sizeof(uint64_t) + synapseCount * sizeof(float);
    
    if (section.size < expectedSize) {
        error_ = "Synapses section size doesn't match expected size";
        return false;
    }
    
    // Read weight array as placeholder
    stream_.seekg(section.offset + sizeof(uint64_t), std::ios::beg);
    stream_.read(reinterpret_cast<char*>(data.weight.data()),
                 synapseCount * sizeof(float));
    if (!stream_) {
        error_ = "Failed to read synapse weights";
        return false;
    }
    
    // Note: In a full implementation, you would read all other synapse arrays
    // from the section data
    
    error_.clear();
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> data;
    
    for (const SectionHeader& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            stream_.seekg(section.offset, std::ios::beg);
            data.resize(section.size);
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            break;
        }
    }
    
    if (data.empty()) {
        error_ = "Section not found in checkpoint";
    }
    
    return data;
}

bool CheckpointReader::validate() const {
    if (!header_.validate()) {
        return false;
    }
    
    if (sections_.empty()) {
        return true;  // Empty checkpoint is valid (just header)
    }
    
    // Validate section offsets
    for (const SectionHeader& section : sections_) {
        if (section.type >= static_cast<uint32_t>(CheckpointSection::End)) {
            return false;
        }
        
        if (section.offset >= header_.totalSize) {
            return false;
        }
        
        if (section.offset + section.size > header_.totalSize) {
            return false;
        }
    }
    
    // Check for duplicate sections
    for (size_t i = 0; i < sections_.size(); ++i) {
        for (size_t j = i + 1; j < sections_.size(); ++j) {
            if (sections_[i].type == sections_[j].type) {
                return false;  // Duplicate sections
            }
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) {
        return 0.0;
    }
    
    double totalSize = 0.0;
    double processedSize = 0.0;
    
    for (const SectionHeader& section : sections_) {
        totalSize += sizeof(SectionHeader) + section.size;
    }
    
    // Calculate processed size (simplified)
    processedSize = streamPos_;
    
    return totalSize > 0.0 ? processedSize / totalSize : 0.0;
}

} // namespace nlm
