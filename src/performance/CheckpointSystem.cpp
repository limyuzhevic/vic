// CheckpointSystem.cpp - Implementation of checkpoint serialization/deserialization

#include "CheckpointSystem.hpp"
#include <stdexcept>
#include <cstring>
#include <filesystem>
#include <algorithm>

namespace nlm {

// CheckpointWriter implementations

CheckpointWriter::CheckpointWriter() 
    : bytesWritten_(0), compression_(CompressionLevel::Balanced) {}

CheckpointWriter::~CheckpointWriter() {
    if (isOpen()) {
        close();
    }
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Close existing file if open
        if (isOpen()) {
            close();
        }
        
        filepath_ = filepath;
        compression_ = compression;
        
        // Create directories if needed
        std::filesystem::path path(filepath);
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }
        
        // Open file for binary writing
        stream_.open(filepath, std::ios::binary | std::ios::trunc);
        if (!stream_.is_open()) {
            return false;
        }
        
        sections_.clear();
        bytesWritten_ = 0;
        
        NLM_LOG_INFO("Created checkpoint file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::close() {
    if (isOpen()) {
        finalize();
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
    header_.timestamp = time(nullptr);
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        // Convert neuron data to binary format
        std::vector<uint8_t> neuronBytes;
        
        // Write number of neurons first
        uint64_t neuronCount = data.membranePotential.size();
        neuronBytes.insert(neuronBytes.end(), 
                          reinterpret_cast<const uint8_t*>(&neuronCount),
                          reinterpret_cast<const uint8_t*>(&neuronCount) + sizeof(uint64_t));
        
        // Write membrane potentials
        size_t floatCount = data.membranePotential.size();
        neuronBytes.insert(neuronBytes.end(),
                          reinterpret_cast<const uint8_t*>(data.membranePotential.data()),
                          reinterpret_cast<const uint8_t*>(data.membranePotential.data()) + 
                          floatCount * sizeof(float));
        
        // Write other arrays similarly
        // This is a simplified implementation - full implementation would handle all fields
        
        if (compression_ != CompressionLevel::None) {
            // Apply compression (simplified)
            // In a full implementation, use zlib or similar
        }
        
        return writeSection(CheckpointSection::Neurons, neuronBytes.data(), neuronBytes.size());
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing neurons: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        std::vector<uint8_t> synapseBytes;
        
        // Write number of synapses
        uint64_t synapseCount = data.weight.size();
        synapseBytes.insert(synapseBytes.end(),
                          reinterpret_cast<const uint8_t*>(&synapseCount),
                          reinterpret_cast<const uint8_t*>(&synapseCount) + sizeof(uint64_t));
        
        // Write synapse data
        // This is simplified - would need proper serialization of all fields
        
        return writeSection(CheckpointSection::Synapses, synapseBytes.data(), synapseBytes.size());
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing synapses: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        // Ensure data is aligned
        if (!padToAlignment(8)) {
            return false;
        }
        
        uint64_t currentPos = stream_.tellp();
        
        SectionHeader section;
        section.type = static_cast<uint32_t>(sectionType);
        section.offset = currentPos;
        section.size = size;
        section.compressedSize = (compression_ != CompressionLevel::None) ? size : 0;
        
        // Calculate checksum
        section.checksum = ChecksumCalculator::crc32(data, size);
        
        // Write section header
        stream_.write(reinterpret_cast<const char*>(&section), sizeof(section));
        
        // Write data
        if (size > 0) {
            stream_.write(reinterpret_cast<const char*>(data), size);
            bytesWritten_ += size;
        }
        
        sections_.push_back(section);
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing section: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        if (!isOpen()) {
            return false;
        }
        
        // Update header with final info
        uint64_t totalSize = stream_.tellp();
        header_.totalSize = totalSize;
        
        // Write header at the beginning
        stream_.seekp(0, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        stream_.seekp(0, std::ios::end);
        
        // Write section headers
        for (const auto& section : sections_) {
            // Rewrite section header at correct position
            stream_.seekp(section.offset, std::ios::beg);
            stream_.write(reinterpret_cast<const char*>(&section), sizeof(section));
            stream_.seekp(0, std::ios::end);
        }
        
        // Write end marker
        CheckpointSection endSection = CheckpointSection::End;
        stream_.write(reinterpret_cast<const char*>(&endSection), sizeof(endSection));
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception finalizing checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::abort() {
    if (isOpen()) {
        close();
        std::filesystem::remove(filepath_);
    }
}

bool CheckpointWriter::writeHeader() {
    // Header should have been written at creation time
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
    return !stream_.fail();
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    uint64_t currentPos = stream_.tellp();
    uint64_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> paddingBytes(padding, 0);
        stream_.write(reinterpret_cast<const char*>(paddingBytes.data()), padding);
    }
    
    return !stream_.fail();
}

// CheckpointReader implementations

CheckpointReader::CheckpointReader() : streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    if (isOpen()) {
        close();
    }
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        if (isOpen()) {
            close();
        }
        
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
        if (!stream_.good()) {
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header";
            return false;
        }
        
        // Read section headers
        while (stream_.good() && streamPos_ < header_.totalSize) {
            CheckpointSection sectionType;
            stream_.read(reinterpret_cast<char*>(&sectionType), sizeof(sectionType));
            
            if (streamPos_ >= header_.totalSize || sectionType == CheckpointSection::End) {
                break;
            }
            
            if (sectionType == CheckpointSection::Header) {
                // Skip header (already read)
                continue;
            }
            
            SectionHeader section;
            section.type = static_cast<uint32_t>(sectionType);
            stream_.read(reinterpret_cast<char*>(&section.offset), sizeof(section.offset));
            stream_.read(reinterpret_cast<const char*>(&section.size), sizeof(section.size));
            stream_.read(reinterpret_cast<const char*>(&section.compressedSize), sizeof(section.compressedSize));
            stream_.read(reinterpret_cast<const char*>(&section.checksum), sizeof(section.checksum));
            
            if (!stream_.good()) {
                break;
            }
            
            sections_.push_back(section);
            streamPos_ += sizeof(sectionType) + sizeof(section.offset) + 
                         sizeof(section.size) + sizeof(section.compressedSize) + 
                         sizeof(section.checksum);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception opening checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointReader::close() {
    if (isOpen()) {
        stream_.close();
    }
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    try {
        auto section = findSection(CheckpointSection::Neurons);
        if (!section) {
            error_ = "Neurons section not found";
            return false;
        }
        
        // Seek to section data
        stream_.seekg(section->offset, std::ios::beg);
        
        // Read neuron count
        uint64_t neuronCount;
        stream_.read(reinterpret_cast<char*>(&neuronCount), sizeof(uint64_t));
        
        // Clear existing data
        data.membranePotential.clear();
        data.membranePotential.reserve(neuronCount);
        
        // Read membrane potentials
        size_t floatCount = neuronCount;
        std::vector<float> potentials(floatCount);
        stream_.read(reinterpret_cast<char*>(potentials.data()), floatCount * sizeof(float));
        
        data.membranePotential = std::move(potentials);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading neurons: ") + e.what();
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    try {
        auto section = findSection(CheckpointSection::Synapses);
        if (!section) {
            error_ = "Synapses section not found";
            return false;
        }
        
        stream_.seekg(section->offset, std::ios::beg);
        
        // Read synapse count
        uint64_t synapseCount;
        stream_.read(reinterpret_cast<char*>(&synapseCount), sizeof(uint64_t));
        
        // Clear existing data
        data.weight.clear();
        data.weight.reserve(synapseCount);
        
        // Read weights (simplified - would need to read all fields)
        std::vector<float> weights(synapseCount);
        stream_.read(reinterpret_cast<char*>(weights.data()), synapseCount * sizeof(float));
        
        data.weight = std::move(weights);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading synapses: ") + e.what();
        return false;
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    auto section = findSection(sectionType);
    if (!section) {
        return std::vector<uint8_t>();
    }
    
    std::vector<uint8_t> data(section->size);
    stream_.seekg(section->offset, std::ios::beg);
    stream_.read(reinterpret_cast<char*>(data.data()), section->size);
    
    return data;
}

bool CheckpointReader::validate() const {
    if (!isOpen()) {
        error_ = "Reader not open";
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid header";
        return false;
    }
    
    // Validate section checksums
    for (const auto& section : sections_) {
        if (!validateSection(section)) {
            return false;
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (header_.totalSize == 0) {
        return 1.0;
    }
    
    uint64_t currentPos = stream_.tellg();
    return static_cast<double>(currentPos) / static_cast<double>(header_.totalSize);
}

std::optional<SectionHeader> CheckpointReader::findSection(CheckpointSection sectionType) const {
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            return section;
        }
    }
    return std::nullopt;
}

bool CheckpointReader::validateSection(const SectionHeader& section) const {
    // Seek to section data and validate checksum
    stream_.seekg(section.offset, std::ios::beg);
    
    // Read data for checksum validation
    std::vector<uint8_t> buffer(section.size);
    stream_.read(reinterpret_cast<char*>(buffer.data()), section.size);
    
    uint32_t calculatedChecksum = ChecksumCalculator::crc32(buffer.data(), buffer.size());
    return calculatedChecksum == section.checksum;
}

// CheckpointManager implementations

CheckpointManager::CheckpointManager() 
    : saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true), 
      lastSaveStep_(0) {}

CheckpointManager::~CheckpointManager() {}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                uint64_t saveIntervalSteps,
                                size_t maxCheckpoints,
                                bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    std::filesystem::create_directories(checkpointDir_);
}

void CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        saveImmediately();
    }
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    try {
        std::string filename = checkpointDir_ + "/checkpoint_";
        if (!name.empty()) {
            filename += name + "_";
        }
        filename += std::to_string(lastSaveStep_) + ".bin";
        
        // In a full implementation, this would call the brain to save its state
        // For now, this is a placeholder
        
        NLM_LOG_INFO("Saving checkpoint: " + filename);
        lastCheckpointPath_ = filename;
        lastSaveStep_ = 0; // Reset for next save
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in saveImmediately: ") + e.what();
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    try {
        std::string filepath = checkpointDir_ + "/";
        if (!name.empty()) {
            filepath += name + "_";
        }
        filepath += "checkpoint_" + std::to_string(lastSaveStep_) + ".bin";
        
        // In a full implementation, this would load the brain state
        NLM_LOG_INFO("Loading checkpoint: " + filepath);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in load: ") + e.what();
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                info.step = extractStepFromFilename(info.name);
                info.fileSize = entry.file_size();
                info.timestamp = std::filesystem::last_write_time(entry).time_since_epoch().count();
                // In a full implementation, would read header for more metadata
                checkpoints.push_back(info);
            }
        }
        
        // Sort by step (newest first)
        std::sort(checkpoints.begin(), checkpoints.end(),
                 [](const CheckpointInfo& a, const CheckpointInfo& b) {
                     return a.step > b.step;
                 });
        
        return checkpoints;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception listing checkpoints: ") + e.what();
        return checkpoints;
    }
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Sort by step (oldest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
             [](const CheckpointInfo& a, const CheckpointInfo& b) {
                 return a.step < b.step;
             });
    
    // Delete oldest checkpoints
    size_t toDelete = checkpoints.size() - keepCount;
    for (size_t i = 0; i < toDelete; ++i) {
        std::filesystem::remove(checkpointDir_ + "/" + checkpoints[i].name + ".bin");
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep < lastSaveStep_) {
        return false;
    }
    
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

uint64_t CheckpointManager::extractStepFromFilename(const std::string& filename) const {
    // Extract step number from filename like "checkpoint_1234.bin"
    size_t pos = filename.find_last_of('_');
    if (pos != std::string::npos) {
        try {
            return std::stoull(filename.substr(pos + 1));
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

} // namespace nlm
