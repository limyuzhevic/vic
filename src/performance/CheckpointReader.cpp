// CheckpointReader implementation
#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <cstring>

namespace nlm {

CheckpointReader::CheckpointReader()
    : streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    if (isOpen()) {
        close();
    }
    
    stream_.open(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
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
        error_ = "Checkpoint header validation failed";
        close();
        return false;
    }
    
    sections_.clear();
    streamPos_ = sizeof(header_);
    
    // Read section headers until End marker
    while (true) {
        SectionHeader sectionHeader;
        stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(sectionHeader));
        if (!stream_.good()) break;
        
        if (sectionHeader.type == static_cast<uint32_t>(CheckpointSection::End)) {
            break;
        }
        
        sections_.push_back(sectionHeader);
        streamPos_ += sizeof(sectionHeader);
    }
    
    return true;
}

void CheckpointReader::close() {
    if (isOpen()) {
        stream_.close();
    }
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    // Clear data
    data = NeuronCheckpointData();
    
    // Read each field sequentially based on the Checkpoint header metadata
    // For simplicity, we'll read the header to determine counts
    uint64_t neuronCount = header_.neuronCount;
    
    if (neuronCount == 0) return true; // No neurons to read
    
    // Calculate sizes based on counts
    size_t vectorSize = static_cast<size_t>(neuronCount);
    
    // Allocate vectors with proper size
    data.membranePotential.resize(vectorSize);
    data.restingPotential.resize(vectorSize);
    data.threshold.resize(vectorSize);
    data.resetPotential.resize(vectorSize);
    data.leakConductance.resize(vectorSize);
    data.firingState.resize(vectorSize);
    data.refractoryRemaining.resize(vectorSize);
    data.refractoryPeriod.resize(vectorSize);
    data.lastSpikeTime.resize(vectorSize);
    
    // Read data sequentially
    stream_.read(reinterpret_cast<char*>(data.membranePotential.data()), 
                 vectorSize * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.restingPotential.data()),
                 vectorSize * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.threshold.data()),
                 vectorSize * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.resetPotential.data()),
                 vectorSize * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.leakConductance.data()),
                 vectorSize * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.firingState.data()),
                 vectorSize * sizeof(uint8_t));
    stream_.read(reinterpret_cast<char*>(data.refractoryRemaining.data()),
                 vectorSize * sizeof(uint32_t));
    stream_.read(reinterpret_cast<char*>(data.refractoryPeriod.data()),
                 vectorSize * sizeof(uint32_t));
    stream_.read(reinterpret_cast<char*>(data.lastSpikeTime.data()),
                 vectorSize * sizeof(float));
    
    return stream_.good() && !stream_.eof();
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    // Clear data
    data = SynapseCheckpointData();
    
    uint64_t synapseCount = header_.synapseCount;
    
    if (synapseCount == 0) return true; // No synapses to read
    
    size_t vectorSize = static_cast<size_t>(synapseCount);
    
    // Allocate vectors with proper size
    data.sourceNeuron.resize(vectorSize);
    data.destinationNeuron.resize(vectorSize);
    data.weight.resize(vectorSize);
    data.delay.resize(vectorSize);
    data.synapseType.resize(vectorSize);
    data.plasticityFlags.resize(vectorSize);
    data.eligibilityTrace.resize(vectorSize);
    data.efficacy.resize(vectorSize);
    data.shortTermDepression.resize(vectorSize);
    data.shortTermFacilitation.resize(vectorSize);
    
    // Read data sequentially
    stream_.read(reinterpret_cast<char*>(data.sourceNeuron.data()),
                 vectorSize * sizeof(uint64_t));
    stream_.read(reinterpret_cast<char*>(data.destinationNeuron.data()),
                 vectorSize * sizeof(uint64_t));
    stream_.read(reinterpret_cast<char*>(data.weight.data()),
                 vectorSize * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.delay.data()),
                 vectorSize * sizeof(uint32_t));
    stream_.read(reinterpret_cast<char*>(data.synapseType.data()),
                 vectorSize * sizeof(uint8_t));
    stream_.read(reinterpret_cast<char*>(data.plasticityFlags.data()),
                 vectorSize * sizeof(uint8_t));
    stream_.read(reinterpret_cast<char*>(data.eligibilityTrace.data()),
                 vectorSize * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.efficacy.data()),
                 vectorSize * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.shortTermDepression.data()),
                 vectorSize * sizeof(float));
    stream_.read(reinterpret_cast<char*>(data.shortTermFacilitation.data()),
                 vectorSize * sizeof(float));
    
    return stream_.good() && !stream_.eof();
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    // Find section by type
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            // Seek to section data
            stream_.seekg(section.offset);
            
            // Read section data
            std::vector<uint8_t> data(section.size);
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            
            return data;
        }
    }
    
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    if (!header_.validate()) {
        error_ = "Checkpoint header validation failed";
        return false;
    }
    
    if (header_.neuronCount == 0) {
        error_ = "No neurons in checkpoint";
        return false;
    }
    
    return true;
}

const std::string& CheckpointReader::getError() const {
    return error_;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    size_t processed = 0;
    for (const auto& section : sections_) {
        if (section.offset <= streamPos_) {
            processed++;
        }
    }
    
    return static_cast<double>(processed) / sections_.size();
}

} // namespace nlm
