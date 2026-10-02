#include "CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <zlib.h>  // For compression (if available)

namespace nlm {

CheckpointWriter::CheckpointWriter()
    : bytesWritten_(0)
{
}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    close();  // Close any existing file
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Ensure directory exists
    std::filesystem::path path(filepath_);
    if (!std::filesystem::exists(path.parent_path())) {
        try {
            std::filesystem::create_directories(path.parent_path());
        } catch (const std::filesystem::filesystem_error& e) {
            NLM_LOG_ERROR("Failed to create checkpoint directory: " + std::string(e.what()));
            return false;
        }
    }
    
    // Open file for writing
    stream_.open(filepath_, std::ios::binary);
    if (!stream_.is_open()) {
        NLM_LOG_ERROR("Failed to open checkpoint file for writing: " + filepath_);
        return false;
    }
    
    NLM_LOG_INFO("Created checkpoint file: " + filepath_);
    return true;
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // Finalize the checkpoint if we're closing
        if (compression_ != CompressionLevel::None && bytesWritten_ > 0) {
            // If we have compression, we need to pad and finalize
            bool finalized = finalize();
            if (!finalized) {
                NLM_LOG_WARNING("Failed to finalize checkpoint before closing");
            }
        }
        stream_.close();
        stream_.clear();
    }
    filepath_.clear();
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                   uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    header_.timestamp = static_cast<uint64_t>(std::time(nullptr));
    
    // Update header when finalized
    if (stream_.is_open()) {
        writeHeader();
    }
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    if (!stream_.is_open()) {
        NLM_LOG_ERROR("Checkpoint not open for writing neurons");
        return false;
    }
    
    // Write section header
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(CheckpointSection::Neurons);
    sectionHeader.offset = 0;  // Will be set during finalize
    
    // Calculate section size
    size_t sectionSize = sizeof(NeuronCheckpointData);
    size_t vectorMultiplier = data.membranePotential.size();
    sectionSize += vectorMultiplier * sizeof(float) * 9;  // 9 vectors in NeuronCheckpointData
    sectionSize += vectorMultiplier * sizeof(uint8_t) * 2;  // firingState, synapseType
    sectionSize += vectorMultiplier * sizeof(uint32_t) * 2;  // refractoryRemaining, refractoryPeriod
    sectionSize += vectorMultiplier * sizeof(float);  // lastSpikeTime
    sectionSize += vectorMultiplier * sizeof(uint64_t) * 3;  // neuronType, regionId, populationId
    
    sectionHeader.size = sectionSize;
    sectionHeader.checksum = 0;  // Will be calculated later
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(sectionHeader));
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write neuron section header");
        return false;
    }
    
    bytesWritten_ += sizeof(sectionHeader);
    
    // Write neuron data
    // Write membrane potentials
    for (float val : data.membranePotential) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write resting potentials
    for (float val : data.restingPotential) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write thresholds
    for (float val : data.threshold) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write reset potentials
    for (float val : data.resetPotential) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write leak conductances
    for (float val : data.leakConductance) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write firing states
    for (uint8_t val : data.firingState) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write refractory remaining
    for (uint32_t val : data.refractoryRemaining) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write refractory periods
    for (uint32_t val : data.refractoryPeriod) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write last spike times
    for (float val : data.lastSpikeTime) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write neuron types
    for (uint64_t val : data.neuronType) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write region IDs
    for (uint64_t val : data.regionId) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write population IDs
    for (uint64_t val : data.populationId) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write neuron data");
        return false;
    }
    
    bytesWritten_ += sectionSize;
    sections_.push_back(sectionHeader);
    return true;
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    if (!stream_.is_open()) {
        NLM_LOG_ERROR("Checkpoint not open for writing synapses");
        return false;
    }
    
    // Write section header
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(CheckpointSection::Synapses);
    sectionHeader.offset = 0;  // Will be set during finalize
    
    // Calculate section size
    size_t sectionSize = sizeof(SynapseCheckpointData);
    size_t vectorMultiplier = data.sourceNeuron.size();
    sectionSize += vectorMultiplier * sizeof(uint64_t) * 2;  // sourceNeuron, destinationNeuron
    sectionSize += vectorMultiplier * sizeof(float);  // weight
    sectionSize += vectorMultiplier * sizeof(uint32_t);  // delay
    sectionSize += vectorMultiplier * sizeof(uint8_t);  // synapseType
    sectionSize += vectorMultiplier * sizeof(uint8_t);  // plasticityFlags
    sectionSize += vectorMultiplier * sizeof(float);  // eligibilityTrace
    sectionSize += vectorMultiplier * sizeof(float);  // efficacy
    sectionSize += vectorMultiplier * sizeof(float);  // shortTermDepression
    sectionSize += vectorMultiplier * sizeof(float);  // shortTermFacilitation
    
    sectionHeader.size = sectionSize;
    sectionHeader.checksum = 0;  // Will be calculated later
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(sectionHeader));
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write synapse section header");
        return false;
    }
    
    bytesWritten_ += sizeof(sectionHeader);
    
    // Write synapse data
    // Write source neurons
    for (uint64_t val : data.sourceNeuron) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write destination neurons
    for (uint64_t val : data.destinationNeuron) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write weights
    for (float val : data.weight) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write delays
    for (uint32_t val : data.delay) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write synapse types
    for (uint8_t val : data.synapseType) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write plasticity flags
    for (uint8_t val : data.plasticityFlags) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write eligibility traces
    for (float val : data.eligibilityTrace) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write efficacies
    for (float val : data.efficacy) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write short-term depression values
    for (float val : data.shortTermDepression) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    // Write short-term facilitation values
    for (float val : data.shortTermFacilitation) {
        stream_.write(reinterpret_cast<const char*>(&val), sizeof(val));
    }
    
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write synapse data");
        return false;
    }
    
    bytesWritten_ += sectionSize;
    sections_.push_back(sectionHeader);
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!stream_.is_open()) {
        NLM_LOG_ERROR("Checkpoint not open for writing section");
        return false;
    }
    
    // Write section header
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(sectionType);
    sectionHeader.offset = 0;  // Will be set during finalize
    sectionHeader.size = size;
    sectionHeader.checksum = 0;  // Will be calculated later
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(sectionHeader));
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write section header");
        return false;
    }
    
    bytesWritten_ += sizeof(sectionHeader);
    
    // Write section data
    stream_.write(reinterpret_cast<const char*>(data), size);
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write section data");
        return false;
    }
    
    bytesWritten_ += size;
    sections_.push_back(sectionHeader);
    return true;
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        NLM_LOG_ERROR("Cannot finalize checkpoint - not open");
        return false;
    }
    
    // Update section offsets
    uint64_t currentOffset = sizeof(CheckpointHeader);  // Skip header initially
    for (auto& section : sections_) {
        section.offset = currentOffset;
        currentOffset += section.size;
    }
    
    // Go back and write updated section headers
    for (const auto& section : sections_) {
        uint64_t pos = sizeof(CheckpointHeader) + (&section - &sections_[0]) * sizeof(SectionHeader);
        stream_.seekp(pos);
        stream_.write(reinterpret_cast<const char*>(&section), sizeof(section));
        if (!stream_.good()) {
            NLM_LOG_ERROR("Failed to write section header during finalization");
            return false;
        }
    }
    
    // Write footer (checksum)
    uint64_t checksum = ChecksumCalculator::crc64(nullptr, 0);  // Simple checksum for now
    stream_.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
    
    // Calculate and write final header
    header_.totalSize = bytesWritten_ + sizeof(CheckpointHeader) + sizeof(uint64_t);  // + checksum
    
    stream_.seekp(0);
    bool success = writeHeader();
    
    if (success) {
        stream_.flush();
        NLM_LOG_INFO("Checkpoint finalized successfully (" + std::to_string(bytesWritten_) + " bytes)");
    }
    
    return success;
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    filepath_.clear();
    
    // Remove incomplete file
    if (!filepath_.empty()) {
        std::error_code ec;
        std::filesystem::remove(filepath_, ec);
        if (ec) {
            NLM_LOG_WARNING("Failed to remove incomplete checkpoint file: " + ec.message());
        }
    }
}

bool CheckpointWriter::writeHeader() {
    // Ensure header is at start of file
    stream_.seekp(0);
    
    // Write header
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write checkpoint header");
        return false;
    }
    
    // Write section headers
    for (const auto& section : sections_) {
        stream_.write(reinterpret_cast<const char*>(&section), sizeof(section));
        if (!stream_.good()) {
            NLM_LOG_ERROR("Failed to write section header");
            return false;
        }
    }
    
    return true;
}

} // namespace nlm
