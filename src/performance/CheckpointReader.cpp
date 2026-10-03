#include "../performance/CheckpointSystem.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace nlm {

CheckpointReader::CheckpointReader() : stream_(), header_(), sections_(), error_(), streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        close(); // Close any existing file
        
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_ || stream_.gcount() != sizeof(CheckpointHeader)) {
            error_ = "Failed to read checkpoint header";
            close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Checkpoint header validation failed";
            close();
            return false;
        }
        
        // Read section count
        uint64_t sectionCount;
        stream_.read(reinterpret_cast<char*>(&sectionCount), sizeof(uint64_t));
        if (!stream_ || stream_.gcount() != sizeof(uint64_t)) {
            error_ = "Failed to read section count";
            close();
            return false;
        }
        
        // Read section headers
        sections_.resize(sectionCount);
        for (size_t i = 0; i < sectionCount; ++i) {
            stream_.read(reinterpret_cast<char*>(&sections_[i]), sizeof(SectionHeader));
            if (!stream_ || stream_.gcount() != sizeof(SectionHeader)) {
                error_ = "Failed to read section header " + std::to_string(i);
                close();
                return false;
            }
        }
        
        streamPos_ = stream_.tellg();
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointReader::open: ") + e.what();
        close();
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    stream_.clear();
    sections_.clear();
    streamPos_ = 0;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    try {
        auto section = readSection(CheckpointSection::Neurons);
        if (section.empty()) {
            error_ = "Neurons section not found or empty";
            return false;
        }
        
        // Deserialize neuron data from section
        const uint8_t* buffer = section.data();
        size_t bufferSize = section.size();
        size_t offset = 0;
        
        // Read membrane potential
        if (!deserializeVector(buffer, offset, bufferSize, data.membranePotential)) {
            error_ = "Failed to deserialize neuron membrane potential";
            return false;
        }
        
        // Read resting potential
        if (!deserializeVector(buffer, offset, bufferSize, data.restingPotential)) {
            error_ = "Failed to deserialize neuron resting potential";
            return false;
        }
        
        // Read threshold
        if (!deserializeVector(buffer, offset, bufferSize, data.threshold)) {
            error_ = "Failed to deserialize neuron threshold";
            return false;
        }
        
        // Read reset potential
        if (!deserializeVector(buffer, offset, bufferSize, data.resetPotential)) {
            error_ = "Failed to deserialize neuron reset potential";
            return false;
        }
        
        // Read leak conductance
        if (!deserializeVector(buffer, offset, bufferSize, data.leakConductance)) {
            error_ = "Failed to deserialize neuron leak conductance";
            return false;
        }
        
        // Read firing state
        if (!deserializeVector(buffer, offset, bufferSize, data.firingState)) {
            error_ = "Failed to deserialize neuron firing state";
            return false;
        }
        
        // Read refractory remaining
        if (!deserializeVector(buffer, offset, bufferSize, data.refractoryRemaining)) {
            error_ = "Failed to deserialize neuron refractory remaining";
            return false;
        }
        
        // Read refractory period
        if (!deserializeVector(buffer, offset, bufferSize, data.refractoryPeriod)) {
            error_ = "Failed to deserialize neuron refractory period";
            return false;
        }
        
        // Read last spike time
        if (!deserializeVector(buffer, offset, bufferSize, data.lastSpikeTime)) {
            error_ = "Failed to deserialize neuron last spike time";
            return false;
        }
        
        // Read neuron type
        if (!deserializeVector(buffer, offset, bufferSize, data.neuronType)) {
            error_ = "Failed to deserialize neuron type";
            return false;
        }
        
        // Read region ID
        if (!deserializeVector(buffer, offset, bufferSize, data.regionId)) {
            error_ = "Failed to deserialize neuron region ID";
            return false;
        }
        
        // Read population ID
        if (!deserializeVector(buffer, offset, bufferSize, data.populationId)) {
            error_ = "Failed to deserialize neuron population ID";
            return false;
        }
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointReader::readNeurons: ") + e.what();
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    try {
        auto section = readSection(CheckpointSection::Synapses);
        if (section.empty()) {
            error_ = "Synapses section not found or empty";
            return false;
        }
        
        // Deserialize synapse data from section
        const uint8_t* buffer = section.data();
        size_t bufferSize = section.size();
        size_t offset = 0;
        
        // Read source neuron
        if (!deserializeVector(buffer, offset, bufferSize, data.sourceNeuron)) {
            error_ = "Failed to deserialize synapse source neuron";
            return false;
        }
        
        // Read destination neuron
        if (!deserializeVector(buffer, offset, bufferSize, data.destinationNeuron)) {
            error_ = "Failed to deserialize synapse destination neuron";
            return false;
        }
        
        // Read weight
        if (!deserializeVector(buffer, offset, bufferSize, data.weight)) {
            error_ = "Failed to deserialize synapse weight";
            return false;
        }
        
        // Read delay
        if (!deserializeVector(buffer, offset, bufferSize, data.delay)) {
            error_ = "Failed to deserialize synapse delay";
            return false;
        }
        
        // Read synapse type
        if (!deserializeVector(buffer, offset, bufferSize, data.synapseType)) {
            error_ = "Failed to deserialize synapse type";
            return false;
        }
        
        // Read plasticity flags
        if (!deserializeVector(buffer, offset, bufferSize, data.plasticityFlags)) {
            error_ = "Failed to deserialize synapse plasticity flags";
            return false;
        }
        
        // Read eligibility trace
        if (!deserializeVector(buffer, offset, bufferSize, data.eligibilityTrace)) {
            error_ = "Failed to deserialize synapse eligibility trace";
            return false;
        }
        
        // Read efficacy
        if (!deserializeVector(buffer, offset, bufferSize, data.efficacy)) {
            error_ = "Failed to deserialize synapse efficacy";
            return false;
        }
        
        // Read short term depression
        if (!deserializeVector(buffer, offset, bufferSize, data.shortTermDepression)) {
            error_ = "Failed to deserialize synapse short term depression";
            return false;
        }
        
        // Read short term facilitation
        if (!deserializeVector(buffer, offset, bufferSize, data.shortTermFacilitation)) {
            error_ = "Failed to deserialize synapse short term facilitation";
            return false;
        }
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointReader::readSynapses: ") + e.what();
        return false;
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> sectionData;
    
    try {
        // Find the section
        auto it = std::find_if(sections_.begin(), sections_.end(),
            [sectionType](const SectionHeader& header) {
                return header.type == sectionType;
            });
        
        if (it == sections_.end()) {
            error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
            return sectionData;
        }
        
        // Read section data
        stream_.seekg(it->offset);
        sectionData.resize(it->size);
        
        if (!stream_.read(reinterpret_cast<char*>(sectionData.data()), sectionData.size()) || 
            stream_.gcount() != static_cast<std::streamsize>(sectionData.size())) {
            error_ = "Failed to read section data";
            sectionData.clear();
            return sectionData;
        }
        
        error_.clear();
        return sectionData;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointReader::readSection: ") + e.what();
        sectionData.clear();
        return sectionData;
    }
}

bool CheckpointReader::validate() const {
    return header_.validate() && error_.empty();
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) {
        return 0.0;
    }
    
    // Calculate progress based on number of sections read
    size_t sectionsRead = 0;
    for (const auto& section : sections_) {
        if (section.type != CheckpointSection::End) {
            sectionsRead++;
        }
    }
    
    return static_cast<double>(sectionsRead) / sections_.size();
}

// Helper function to deserialize vectors
bool CheckpointReader::deserializeVector(const uint8_t*& buffer, size_t& offset, 
                                         size_t bufferSize, auto& data) {
    // Read count
    if (offset + sizeof(uint64_t) > bufferSize) {
        return false;
    }
    
    uint64_t count = *reinterpret_cast<const uint64_t*>(buffer + offset);
    offset += sizeof(uint64_t);
    
    data.resize(count);
    
    // Read elements
    size_t elementSize = sizeof(typename decltype(data)::value_type);
    size_t totalSize = count * elementSize;
    
    if (offset + totalSize > bufferSize) {
        return false;
    }
    
    std::copy(buffer + offset, buffer + offset + totalSize, 
              reinterpret_cast<uint8_t*>(data.data()));
    offset += totalSize;
    
    return true;
}

} // namespace nlm
