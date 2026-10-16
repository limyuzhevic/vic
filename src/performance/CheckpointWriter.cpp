#include "CheckpointSystem.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>
#include <chrono>

namespace nlm {

CheckpointWriter::CheckpointWriter()
    : compression_(CompressionLevel::Balanced)
    , bytesWritten_(0)
    , compressBuffer_()
{
}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    if (isOpen()) {
        close();
    }
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Open file for binary writing
    stream_.open(filepath_, std::ios::binary | std::ios::out);
    if (!stream_.is_open()) {
        error_ = "Failed to open file for writing: " + filepath;
        return false;
    }
    
    // Initialize header with defaults
    header_.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // Write empty header initially
    if (!writeHeader()) {
        close();
        return false;
    }
    
    bytesWritten_ = 0;
    return true;
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // If file is open, finalize it
        if (!finalize()) {
            abort();
        }
    }
    
    stream_.close();
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                   uint64_t regionCount, uint64_t step, double simTime) {
    if (!isOpen()) {
        return;
    }
    
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    
    // Write updated header
    writeHeader();
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    if (!isOpen()) {
        error_ = "CheckpointWriter: Not open";
        return false;
    }
    
    // Serialize neuron data
    // For now, we'll use a simple binary format
    // In a real implementation, you'd compress and compute checksums
    
    // Write data size first
    size_t neuronCount = data.membranePotential.size();
    stream_.write(reinterpret_cast<const char*>(&neuronCount), sizeof(neuronCount));
    if (!stream_.good()) {
        error_ = "Failed to write neuron count";
        return false;
    }
    
    // Write all neuron vectors
    // Membrane potential
    if (neuronCount > 0) {
        stream_.write(reinterpret_cast<const char*>(data.membranePotential.data()),
                     neuronCount * sizeof(float));
        if (!stream_.good()) {
            error_ = "Failed to write membrane potentials";
            return false;
        }
    }
    
    // Resting potential
    stream_.write(reinterpret_cast<const char*>(data.restingPotential.data()),
                  neuronCount * sizeof(float));
    
    // Threshold
    stream_.write(reinterpret_cast<const char*>(data.threshold.data()),
                  neuronCount * sizeof(float));
    
    // Reset potential
    stream_.write(reinterpret_cast<const char*>(data.resetPotential.data()),
                  neuronCount * sizeof(float));
    
    // Leak conductance
    stream_.write(reinterpret_cast<const char*>(data.leakConductance.data()),
                  neuronCount * sizeof(float));
    
    // Firing state
    stream_.write(reinterpret_cast<const char*>(data.firingState.data()),
                  neuronCount * sizeof(uint8_t));
    
    // Refractory remaining
    stream_.write(reinterpret_cast<const char*>(data.refractoryRemaining.data()),
                  neuronCount * sizeof(uint32_t));
    
    // Refractory period
    stream_.write(reinterpret_cast<const char*>(data.refractoryPeriod.data()),
                  neuronCount * sizeof(uint32_t));
    
    // Last spike time
    stream_.write(reinterpret_cast<const char*>(data.lastSpikeTime.data()),
                  neuronCount * sizeof(float));
    
    // Neuron type
    stream_.write(reinterpret_cast<const char*>(data.neuronType.data()),
                  neuronCount * sizeof(uint64_t));
    
    // Region ID
    stream_.write(reinterpret_cast<const char*>(data.regionId.data()),
                  neuronCount * sizeof(uint64_t));
    
    // Population ID
    stream_.write(reinterpret_cast<const char*>(data.populationId.data()),
                  neuronCount * sizeof(uint64_t));
    
    if (!stream_.good()) {
        error_ = "Failed to write neuron data";
        return false;
    }
    
    bytesWritten_ += neuronCount * sizeof(float) * 12;  // 12 vectors
    return true;
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    if (!isOpen()) {
        error_ = "CheckpointWriter: Not open";
        return false;
    }
    
    // Serialize synapse data
    size_t synapseCount = data.sourceNeuron.size();
    stream_.write(reinterpret_cast<const char*>(&synapseCount), sizeof(synapseCount));
    
    if (synapseCount > 0) {
        // Source neurons
        stream_.write(reinterpret_cast<const char*>(data.sourceNeuron.data()),
                     synapseCount * sizeof(uint64_t));
        
        // Destination neurons
        stream_.write(reinterpret_cast<const char*>(data.destinationNeuron.data()),
                     synapseCount * sizeof(uint64_t));
        
        // Weights
        stream_.write(reinterpret_cast<const char*>(data.weight.data()),
                     synapseCount * sizeof(float));
        
        // Delays
        stream_.write(reinterpret_cast<const char*>(data.delay.data()),
                     synapseCount * sizeof(uint32_t));
        
        // Synapse type
        stream_.write(reinterpret_cast<const char*>(data.synapseType.data()),
                     synapseCount * sizeof(uint8_t));
        
        // Plasticity flags
        stream_.write(reinterpret_cast<const char*>(data.plasticityFlags.data()),
                     synapseCount * sizeof(uint8_t));
        
        // Eligibility trace
        stream_.write(reinterpret_cast<const char*>(data.eligibilityTrace.data()),
                     synapseCount * sizeof(float));
        
        // Efficacy
        stream_.write(reinterpret_cast<const char*>(data.efficacy.data()),
                     synapseCount * sizeof(float));
        
        // Short-term depression
        stream_.write(reinterpret_cast<const char*>(data.shortTermDepression.data()),
                     synapseCount * sizeof(float));
        
        // Short-term facilitation
        stream_.write(reinterpret_cast<const char*>(data.shortTermFacilitation.data()),
                     synapseCount * sizeof(float));
    }
    
    if (!stream_.good()) {
        error_ = "Failed to write synapse data";
        return false;
    }
    
    bytesWritten_ += synapseCount * sizeof(uint64_t) * 3 + synapseCount * sizeof(float) * 7 +
                    synapseCount * sizeof(uint32_t) + synapseCount * sizeof(uint8_t) * 2;
    
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!isOpen()) {
        error_ = "CheckpointWriter: Not open";
        return false;
    }
    
    SectionHeader section;
    section.type = static_cast<uint32_t>(sectionType);
    section.offset = stream_.tellp();
    section.size = size;
    section.compressedSize = 0;  // No compression for now
    section.checksum = 0;  // No checksum for now
    
    // Write section header
    if (!writeSectionHeader(section)) {
        return false;
    }
    
    // Write section data
    stream_.write(reinterpret_cast<const char*>(data), size);
    if (!stream_.good()) {
        error_ = "Failed to write section data";
        return false;
    }
    
    // Pad to alignment
    padToAlignment(8);
    
    bytesWritten_ += size;
    return true;
}

bool CheckpointWriter::finalize() {
    if (!isOpen()) {
        return true;  // Nothing to do
    }
    
    // Calculate total size from current position
    auto currentPos = stream_.tellp();
    header_.totalSize = static_cast<uint64_t>(currentPos);
    
    // Update header at beginning of file
    stream_.seekp(0);
    if (!writeHeader()) {
        close();
        return false;
    }
    
    // Ensure file is properly closed
    stream_.close();
    
    return true;
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    
    // Remove partial file
    if (!filepath_.empty()) {
        std::remove(filepath_.c_str());
        filepath_.clear();
    }
}

uint64_t CheckpointWriter::getBytesWritten() const {
    return bytesWritten_;
}

bool CheckpointWriter::writeHeader() {
    uint64_t checksum = ChecksumCalculator::combine(
        ChecksumCalculator::crc64(&header_, sizeof(header_)),
        header_.totalSize);
    
    // Update checksum in header
    header_.checksum = checksum;
    
    // Write header to file
    stream_.seekp(0);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    
    // Reset stream position
    stream_.seekp(0, std::ios::end);
    
    if (!stream_.good()) {
        error_ = "Failed to write checkpoint header";
        return false;
    }
    
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
    
    if (!stream_.good()) {
        error_ = "Failed to write section header";
        return false;
    }
    
    return true;
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    auto currentPos = stream_.tellp();
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> padBuffer(padding, 0);
        stream_.write(reinterpret_cast<const char*>(padBuffer.data()), padding);
        
        if (!stream_.good()) {
            error_ = "Failed to pad to alignment";
            return false;
        }
    }
    
    return true;
}

} // namespace nlm