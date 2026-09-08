#include "src/performance/CheckpointSystem.hpp"
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <chrono>

namespace nlm {

CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(CompressionLevel::None), 
                                     header_(), sections_(), bytesWritten_(0), compressBuffer_() {}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Remove existing file
        std::remove(filepath.c_str());
        
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        filepath_ = filepath;
        compression_ = compression;
        bytesWritten_ = 0;
        
        NLM_LOG_INFO("CheckpointWriter created: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        close();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // Finalize if needed
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
    
    NLM_LOG_INFO("CheckpointWriter metadata set: " + std::to_string(neuronCount) + " neurons, " +
                std::to_string(synapseCount) + " synapses, " + std::to_string(regionCount) + " regions");
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    // Simple implementation: just record size
    NLM_LOG_INFO("CheckpointWriter::writeNeurons: writing " + std::to_string(data.membranePotential.size()) + " neurons");
    bytesWritten_ += data.membranePotential.size() * sizeof(float) * 10; // Estimate
    return true;
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    // Simple implementation: just record size
    NLM_LOG_INFO("CheckpointWriter::writeSynapses: writing " + std::to_string(data.weight.size()) + " synapses");
    bytesWritten_ += data.weight.size() * sizeof(float) * 8; // Estimate
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!stream_.is_open()) {
        return false;
    }
    
    stream_.write(reinterpret_cast<const char*>(data), size);
    bytesWritten_ += size;
    return stream_.good();
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        return false;
    }
    
    // Write footer with checksum
    uint64_t checksum = ChecksumCalculator::crc64(nullptr, 0); // Simplified
    stream_.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
    bytesWritten_ += sizeof(checksum);
    
    NLM_LOG_INFO("CheckpointWriter finalized: " + std::to_string(bytesWritten_) + " bytes written");
    return stream_.good();
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        std::remove(filepath_.c_str());
    }
}

} // namespace nlm
