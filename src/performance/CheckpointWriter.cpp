#include "../performance/CheckpointSystem.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace nlm {

CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(CompressionLevel::Balanced),
                                       header_(), sections_(), bytesWritten_(0), compressBuffer_() {}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        close(); // Close any existing file
        
        filepath_ = filepath;
        compression_ = compression;
        
        // Create directories if they don't exist
        std::filesystem::path path(filepath);
        if (!path.parent_path().empty() && !std::filesystem::exists(path.parent_path())) {
            if (!std::filesystem::create_directories(path.parent_path())) {
                error_ = "Failed to create checkpoint directory: " + path.parent_path().string();
                return false;
            }
        }
        
        // Open output stream
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        // Initialize header
        header_.timestamp = static_cast<uint64_t>(std::time(nullptr));
        bytesWritten_ = 0;
        
        // Set default values for header (will be updated by setMetadata)
        header_.magic = CHECKPOINT_MAGIC;
        header_.majorVersion = CHECKPOINT_VERSION_MAJOR;
        header_.minorVersion = CHECKPOINT_VERSION_MINOR;
        header_.patchVersion = CHECKPOINT_VERSION_PATCH;
        header_.totalSize = 0; // Will be updated in finalize
        header_.neuronCount = 0;
        header_.synapseCount = 0;
        header_.regionCount = 0;
        header_.step = 0;
        header_.simulationTime = 0.0;
        header_.checksum = 0;
        header_.compressionLevel = static_cast<uint32_t>(compression_);
        header_.flags = 0;
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointWriter::create: ") + e.what();
        close();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    stream_.clear();
    compressBuffer_.clear();
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
        // Serialize neuron data
        std::vector<uint8_t> sectionData;
        
        // Serialize each vector
        sectionData.insert(sectionData.end(),
                          reinterpret_cast<const uint8_t*>(&data.membranePotential.size()),
                          reinterpret_cast<const uint8_t*>(&data.membranePotential.size()) + sizeof(uint64_t));
        
        // Serialize vector data (in real implementation, would use more efficient serialization)
        for (float value : data.membranePotential) {
            uint8_t bytes[4];
            std::memcpy(bytes, &value, sizeof(float));
            sectionData.insert(sectionData.end(), bytes, bytes + sizeof(float));
        }
        
        // Similar serialization for other fields...
        // For simplicity, just write a basic header
        sectionData = "NEURONS_DATA"; // Placeholder
        
        return writeSection(CheckpointSection::Neurons, sectionData.data(), sectionData.size());
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointWriter::writeNeurons: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Serialize synapse data (similar to neurons)
        std::vector<uint8_t> sectionData;
        sectionData = "SYNAPSES_DATA"; // Placeholder
        
        return writeSection(CheckpointSection::Synapses, sectionData.data(), sectionData.size());
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointWriter::writeSynapses: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint file not open";
            return false;
        }
        
        // Compress if needed
        const void* dataToWrite = data;
        size_t dataSize = size;
        
        if (compression_ != CompressionLevel::None && size > 1024) { // Only compress large sections
            // Compress data (simplified)
            compressBuffer_.resize(size * 2); // Simple expansion for worst case
            
            // For now, just write uncompressed
            dataToWrite = data;
            dataSize = size;
        }
        
        // Store section header
        SectionHeader sectionHeader;
        sectionHeader.type = sectionType;
        sectionHeader.offset = bytesWritten_ + sizeof(CheckpointHeader) + sizeof(uint64_t) +
                             sizeof(SectionHeader) * sections_.size();
        sectionHeader.size = dataSize;
        sectionHeader.compressedSize = (compression_ != CompressionLevel::None) ? dataSize : 0;
        sectionHeader.checksum = 0; // TODO: Calculate checksum
        
        sections_.push_back(sectionHeader);
        
        // Write section data
        stream_.write(reinterpret_cast<const char*>(dataToWrite), dataSize);
        bytesWritten_ += dataSize;
        
        if (!stream_) {
            error_ = "Failed to write section data";
            return false;
        }
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointWriter::writeSection: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint file not open";
            return false;
        }
        
        // Update total size
        header_.totalSize = bytesWritten_;
        
        // Calculate checksum (simplified)
        header_.checksum = ChecksumCalculator::fletcher64(
            reinterpret_cast<const uint64_t*>(&header_), sizeof(CheckpointHeader) / sizeof(uint64_t));
        
        // Write header
        bool headerWritten = writeHeader();
        if (!headerWritten) {
            error_ = "Failed to write checkpoint header";
            return false;
        }
        
        // Write footer
        CheckpointSection endSection = CheckpointSection::End;
        stream_.write(reinterpret_cast<const char*>(&endSection), sizeof(CheckpointSection));
        bytesWritten_ += sizeof(CheckpointSection);
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointWriter::finalize: ") + e.what();
        return false;
    }
}

void CheckpointWriter::abort() {
    close();
    // Remove incomplete file
    std::filesystem::remove(filepath_);
}

bool CheckpointWriter::writeHeader() {
    try {
        // Write section count
        uint64_t sectionCount = sections_.size();
        stream_.write(reinterpret_cast<const char*>(&sectionCount), sizeof(uint64_t));
        if (!stream_ || stream_.gcount() != sizeof(uint64_t)) {
            error_ = "Failed to write section count";
            return false;
        }
        bytesWritten_ += sizeof(uint64_t);
        
        // Write section headers
        for (const auto& section : sections_) {
            stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
            if (!stream_ || stream_.gcount() != sizeof(SectionHeader)) {
                error_ = "Failed to write section header";
                return false;
            }
            bytesWritten_ += sizeof(SectionHeader);
        }
        
        // Write header
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_ || stream_.gcount() != sizeof(CheckpointHeader)) {
            error_ = "Failed to write header";
            return false;
        }
        bytesWritten_ += sizeof(CheckpointHeader);
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointWriter::writeHeader: ") + e.what();
        return false;
    }
}

} // namespace nlm
