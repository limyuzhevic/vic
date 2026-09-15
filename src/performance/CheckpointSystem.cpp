#pragma once

#include "../performance/CheckpointSystem.hpp"
#include <algorithm>
#include <chrono>
#include <cstring>

namespace nlm {

CheckpointWriter::CheckpointWriter()
    : bytesWritten_(0)
    , compression_(CompressionLevel::Balanced) {}

CheckpointWriter::~CheckpointWriter() {
    if (isOpen()) {
        close();
    }
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    if (isOpen()) {
        close();
    }
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Open file for binary writing
    stream_.open(filepath, std::ios::binary | std::ios::trunc);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
        return false;
    }
    
    // Initialize header
    header_.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    sections_.clear();
    bytesWritten_ = 0;
    
    return true;
}

void CheckpointWriter::close() {
    if (isOpen()) {
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
    // Write each vector individually for better compatibility
    if (!writeSection(CheckpointSection::Neurons, data.membranePotential.data(), 
                     data.membranePotential.size() * sizeof(float))) return false;
    if (!writeSection(CheckpointSection::Neurons, data.restingPotential.data(),
                     data.restingPotential.size() * sizeof(float))) return false;
    if (!writeSection(CheckpointSection::Neurons, data.threshold.data(),
                     data.threshold.size() * sizeof(float))) return false;
    if (!writeSection(CheckpointSection::Neurons, data.resetPotential.data(),
                     data.resetPotential.size() * sizeof(float))) return false;
    if (!writeSection(CheckpointSection::Neurons, data.leakConductance.data(),
                     data.leakConductance.size() * sizeof(float))) return false;
    if (!writeSection(CheckpointSection::Neurons, data.firingState.data(),
                     data.firingState.size() * sizeof(uint8_t))) return false;
    if (!writeSection(CheckpointSection::Neurons, data.refractoryRemaining.data(),
                     data.refractoryRemaining.size() * sizeof(uint32_t))) return false;
    if (!writeSection(CheckpointSection::Neurons, data.refractoryPeriod.data(),
                     data.refractoryPeriod.size() * sizeof(uint32_t))) return false;
    if (!writeSection(CheckpointSection::Neurons, data.lastSpikeTime.data(),
                     data.lastSpikeTime.size() * sizeof(float))) return false;
    
    return true;
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    // Write each vector individually
    if (!writeSection(CheckpointSection::Synapses, data.sourceNeuron.data(),
                     data.sourceNeuron.size() * sizeof(uint64_t))) return false;
    if (!writeSection(CheckpointSection::Synapses, data.destinationNeuron.data(),
                     data.destinationNeuron.size() * sizeof(uint64_t))) return false;
    if (!writeSection(CheckpointSection::Synapses, data.weight.data(),
                     data.weight.size() * sizeof(float))) return false;
    if (!writeSection(CheckpointSection::Synapses, data.delay.data(),
                     data.delay.size() * sizeof(uint32_t))) return false;
    if (!writeSection(CheckpointSection::Synapses, data.synapseType.data(),
                     data.synapseType.size() * sizeof(uint8_t))) return false;
    if (!writeSection(CheckpointSection::Synapses, data.plasticityFlags.data(),
                     data.plasticityFlags.size() * sizeof(uint8_t))) return false;
    if (!writeSection(CheckpointSection::Synapses, data.eligibilityTrace.data(),
                     data.eligibilityTrace.size() * sizeof(float))) return false;
    
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!data || size == 0) return false;
    
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(sectionType);
    sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
    sectionHeader.size = size;
    sectionHeader.compressedSize = 0;
    sectionHeader.checksum = 0;
    
    if (!writeSectionHeader(sectionHeader)) {
        return false;
    }
    
    stream_.write(reinterpret_cast<const char*>(data), size);
    if (!stream_.good()) {
        error_ = "Failed to write section data";
        return false;
    }
    
    bytesWritten_ += size;
    sections_.push_back(sectionHeader);
    
    return true;
}

bool CheckpointWriter::finalize() {
    if (!writeHeader()) {
        return false;
    }
    
    // Write end marker
    SectionHeader endHeader;
    endHeader.type = static_cast<uint32_t>(CheckpointSection::End);
    endHeader.offset = static_cast<uint64_t>(stream_.tellp());
    endHeader.size = 0;
    endHeader.compressedSize = 0;
    endHeader.checksum = 0;
    
    if (!writeSectionHeader(endHeader)) {
        return false;
    }
    
    stream_.flush();
    return stream_.good();
}

void CheckpointWriter::abort() {
    close();
    if (!filepath_.empty()) {
        std::remove(filepath_.c_str());
    }
}

bool CheckpointWriter::writeHeader() {
    header_.totalSize = bytesWritten_;
    
    stream_.seekp(0, std::ios::beg);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    
    return stream_.good();
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
    uint64_t current = static_cast<uint64_t>(stream_.tellp());
    size_t padding = (alignment - (current % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> pad(padding, 0);
        stream_.write(reinterpret_cast<const char*>(pad.data()), padding);
    }
    
    return stream_.good();
}

} // namespace nlm
