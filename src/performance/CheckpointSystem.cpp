// CheckpointWriter implementation
#include "../performance/CheckpointSystem.hpp"
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <chrono>

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
        NLM_LOG_ERROR("Checkpoint file already open");
        return false;
    }
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Create directory if it doesn't exist
    std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        try {
            std::filesystem::create_directories(dir);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to create checkpoint directory: ") + e.what());
            return false;
        }
    }
    
    try {
        stream_.open(filepath, std::ios::binary | std::ios::trunc);
        if (!stream_.is_open()) {
            NLM_LOG_ERROR("Failed to open checkpoint file for writing: " + filepath);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception opening checkpoint file: ") + e.what());
        return false;
    }
    
    sections_.clear();
    bytesWritten_ = 0;
    
    NLM_LOG_INFO("Created checkpoint file: " + filepath);
    return true;
}

void CheckpointWriter::close() {
    if (isOpen()) {
        try {
            stream_.close();
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception closing checkpoint file: ") + e.what());
        }
    }
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                  uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    header_.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    header_.compressionLevel = static_cast<uint32_t>(compression_);
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        // Serialize neuron data
        std::vector<uint8_t> buffer;
        
        // Write magic header for neurons
        const uint32_t NEURON_MAGIC = 0x4E4C5554ULL;  // "NEUT"
        buffer.insert(buffer.end(), (uint8_t*)&NEURON_MAGIC, (uint8_t*)&NEURON_MAGIC + sizeof(uint32_t));
        
        // Write data size
        uint64_t dataSize = sizeof(float) * (
            data.membranePotential.size() +
            data.restingPotential.size() +
            data.threshold.size() +
            data.resetPotential.size() +
            data.leakConductance.size() +
            data.firingState.size() * sizeof(uint8_t) +
            data.refractoryRemaining.size() * sizeof(uint32_t) +
            data.refractoryPeriod.size() * sizeof(uint32_t) +
            data.lastSpikeTime.size() +
            data.neuronType.size() * sizeof(uint64_t) +
            data.regionId.size() * sizeof(uint64_t) +
            data.populationId.size() * sizeof(uint64_t)
        );
        
        buffer.insert(buffer.end(), (uint8_t*)&dataSize, (uint8_t*)&dataSize + sizeof(uint64_t));
        
        // Write all float arrays
        auto writeArray = [&buffer](const auto& arr) {
            buffer.insert(buffer.end(), (uint8_t*)arr.data(), (uint8_t*)arr.data() + arr.size() * sizeof(decltype(arr)::value_type));
        };
        
        writeArray(data.membranePotential);
        writeArray(data.restingPotential);
        writeArray(data.threshold);
        writeArray(data.resetPotential);
        writeArray(data.leakConductance);
        writeArray(data.firingState);
        writeArray(data.refractoryRemaining);
        writeArray(data.refractoryPeriod);
        writeArray(data.lastSpikeTime);
        writeArray(data.neuronType);
        writeArray(data.regionId);
        writeArray(data.populationId);
        
        // Write section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(CheckpointSection::Neurons);
        sectionHeader.offset = 0;  // Will be updated later
        sectionHeader.size = buffer.size();
        sectionHeader.compressedSize = 0;
        sectionHeader.checksum = 0;
        
        // Calculate checksum (simplified)
        uint32_t checksum = 0;
        for (uint8_t byte : buffer) {
            checksum ^= byte;
        }
        sectionHeader.checksum = checksum;
        
        sections_.push_back(sectionHeader);
        
        // Write section header to file
        stream_.write((char*)&sectionHeader, sizeof(sectionHeader));
        bytesWritten_ += sizeof(sectionHeader);
        
        // Write section data
        stream_.write((char*)buffer.data(), buffer.size());
        bytesWritten_ += buffer.size();
        
        // Update section header with actual offset
        sections_.back().offset = bytesWritten_ - sectionHeader.size - sizeof(SectionHeader);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing neurons checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Serialize synapse data
        std::vector<uint8_t> buffer;
        
        // Write magic header for synapses
        const uint32_t SYNAPSE_MAGIC = 0x5359A502ULL;  // "SYAP"
        buffer.insert(buffer.end(), (uint8_t*)&SYNAPSE_MAGIC, (uint8_t*)&SYNAPSE_MAGIC + sizeof(uint32_t));
        
        // Write data size
        uint64_t dataSize = sizeof(uint64_t) * (
            data.sourceNeuron.size() +
            data.destinationNeuron.size() +
            data.delay.size() +
            data.neuronType.size() * sizeof(uint8_t) +
            data.plasticityFlags.size() * sizeof(uint8_t) +
            data.eligibilityTrace.size() * sizeof(float) +
            data.efficacy.size() * sizeof(float) +
            data.shortTermDepression.size() * sizeof(float) +
            data.shortTermFacilitation.size() * sizeof(float)
        );
        
        buffer.insert(buffer.end(), (uint8_t*)&dataSize, (uint8_t*)&dataSize + sizeof(uint64_t));
        
        // Write all arrays
        auto writeArray = [&buffer](const auto& arr) {
            buffer.insert(buffer.end(), (uint8_t*)arr.data(), (uint8_t*)arr.data() + arr.size() * sizeof(decltype(arr)::value_type));
        };
        
        writeArray(data.sourceNeuron);
        writeArray(data.destinationNeuron);
        writeArray(data.delay);
        writeArray(data.synapseType);
        writeArray(data.plasticityFlags);
        writeArray(data.eligibilityTrace);
        writeArray(data.efficacy);
        writeArray(data.shortTermDepression);
        writeArray(data.shortTermFacilitation);
        
        // Write section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(CheckpointSection::Synapses);
        sectionHeader.offset = 0;
        sectionHeader.size = buffer.size();
        sectionHeader.compressedSize = 0;
        sectionHeader.checksum = 0;
        
        // Calculate checksum
        uint32_t checksum = 0;
        for (uint8_t byte : buffer) {
            checksum ^= byte;
        }
        sectionHeader.checksum = checksum;
        
        sections_.push_back(sectionHeader);
        
        // Update the previous section's header to have correct offset
        if (sections_.size() >= 2) {
            sections_[sections_.size() - 2].offset = bytesWritten_ - sections_[sections_.size() - 2].size - sizeof(SectionHeader);
        }
        
        // Write section header to file
        stream_.write((char*)&sectionHeader, sizeof(sectionHeader));
        bytesWritten_ += sizeof(sectionHeader);
        
        // Write section data
        stream_.write((char*)buffer.data(), buffer.size());
        bytesWritten_ += buffer.size();
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing synapses checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        std::vector<uint8_t> buffer(reinterpret_cast<const uint8_t*>(data), reinterpret_cast<const uint8_t*>(data) + size);
        
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = 0;
        sectionHeader.size = size;
        sectionHeader.compressedSize = 0;
        sectionHeader.checksum = 0;
        
        // Calculate checksum
        uint32_t checksum = 0;
        for (size_t i = 0; i < size; ++i) {
            checksum ^= buffer[i];
        }
        sectionHeader.checksum = checksum;
        
        sections_.push_back(sectionHeader);
        
        // Update previous section offset
        if (sections_.size() >= 2) {
            sections_[sections_.size() - 2].offset = bytesWritten_ - sections_[sections_.size() - 2].size - sizeof(SectionHeader);
        }
        
        // Write section header
        stream_.write((char*)&sectionHeader, sizeof(sectionHeader));
        bytesWritten_ += sizeof(sectionHeader);
        
        // Write section data
        stream_.write((char*)buffer.data(), size);
        bytesWritten_ += size;
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing checkpoint section: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        // Update all section headers with correct offsets
        for (size_t i = 0; i < sections_.size(); ++i) {
            if (i > 0) {
                sections_[i].offset = sections_[i-1].offset + sections_[i-1].size + sizeof(SectionHeader);
            } else {
                sections_[i].offset = 0;
            }
            
            // Write updated header to file at correct position
            stream_.seekp(sections_[i].offset, std::ios::beg);
            stream_.write((char*)&sections_[i], sizeof(SectionHeader));
            
            // Recalculate checksum for this section
            stream_.seekg(sections_[i].offset + sizeof(SectionHeader), std::ios::beg);
            std::vector<uint8_t> sectionData(sections_[i].size);
            stream_.read((char*)sectionData.data(), sectionData.size());
            
            uint32_t checksum = 0;
            for (uint8_t byte : sectionData) {
                checksum ^= byte;
            }
            sections_[i].checksum = checksum;
            
            // Write final header with checksum
            stream_.seekp(sections_[i].offset, std::ios::beg);
            stream_.write((char*)&sections_[i], sizeof(SectionHeader));
            
            // Reset stream position for writing data
            stream_.seekp(sections_[i].offset + sizeof(SectionHeader), std::ios::beg);
        }
        
        // Write end marker
        SectionHeader endHeader;
        endHeader.type = static_cast<uint32_t>(CheckpointSection::End);
        endHeader.offset = bytesWritten_;
        endHeader.size = 0;
        endHeader.compressedSize = 0;
        endHeader.checksum = 0;
        
        stream_.write((char*)&endHeader, sizeof(endHeader));
        bytesWritten_ += sizeof(endHeader);
        
        // Calculate final checksum for header
        uint64_t totalChecksum = ChecksumCalculator::fletcher64(
            reinterpret_cast<const uint64_t*>(&header_), sizeof(header_) / sizeof(uint64_t)
        );
        
        stream_.seekp(0, std::ios::beg);
        header_.checksum = totalChecksum;
        stream_.write((char*)&header_, sizeof(header_));
        
        stream_.flush();
        
        NLM_LOG_INFO("Checkpoint finalized successfully, written " + std::to_string(bytesWritten_) + " bytes");
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception finalizing checkpoint: ") + e.what());
        return false;
    }
}

void CheckpointWriter::abort() {
    if (isOpen()) {
        close();
        // Remove partial file
        std::filesystem::path filepath(filepath_);
        try {
            if (std::filesystem::exists(filepath)) {
                std::filesystem::remove(filepath);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception cleaning up checkpoint file: ") + e.what());
        }
    }
}

uint64_t CheckpointWriter::getBytesWritten() const {
    return bytesWritten_;
}

bool CheckpointWriter::writeHeader() {
    // This is called internally by finalize()
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    // This is called internally by writeSection()
    try {
        stream_.write((char*)&header, sizeof(header));
        bytesWritten_ += sizeof(header);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing section header: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    // This is called internally to align to disk sector boundaries
    size_t currentPos = bytesWritten_;
    size_t paddingNeeded = alignment - (currentPos % alignment);
    
    if (paddingNeeded < alignment) {
        std::vector<uint8_t> padding(paddingNeeded, 0);
        stream_.write((char*)padding.data(), padding.size());
        bytesWritten_ += padding.size();
    }
    
    return true;
}

} // namespace nlm