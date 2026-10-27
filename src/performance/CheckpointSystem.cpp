#include "CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <cstring>
#include <filesystem>
#include <iostream>

namespace nlm {

// CheckpointSystem Implementation

CheckpointWriter::CheckpointWriter()
    : compression_(CompressionLevel::Balanced)
    , bytesWritten_(0) {
}

CheckpointWriter::~CheckpointWriter() {
    if (isOpen()) {
        close();
    }
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        close(); // Ensure any previous file is closed
        
        filepath_ = filepath;
        compression_ = compression;
        
        // Create directory if it doesn't exist
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        stream_.open(filepath, std::ios::binary | std::ios::trunc);
        if (!stream_) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        bytesWritten_ = 0;
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating checkpoint writer: ") + e.what());
        close();
        return false;
    }
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
    if (!isOpen()) {
        NLM_LOG_ERROR("Cannot write neurons - checkpoint not open");
        return false;
    }
    
    try {
        // Create neuron section data
        NeuronCheckpointData sectionData = data;
        
        // Compress if needed
        std::vector<uint8_t> sectionBuffer;
        if (compression_ != CompressionLevel::None) {
            // Simple placeholder compression - in real implementation would use actual compression
            sectionBuffer.resize(sizeof(NeuronCheckpointData));
            std::memcpy(sectionBuffer.data(), &sectionData, sizeof(NeuronCheckpointData));
        } else {
            sectionBuffer.resize(sizeof(NeuronCheckpointData));
            std::memcpy(sectionBuffer.data(), &sectionData, sizeof(NeuronCheckpointData));
        }
        
        return writeSection(CheckpointSection::Neurons, sectionBuffer.data(), sectionBuffer.size());
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing neurons: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    if (!isOpen()) {
        NLM_LOG_ERROR("Cannot write synapses - checkpoint not open");
        return false;
    }
    
    try {
        // Create synapse section data
        SynapseCheckpointData sectionData = data;
        
        // Compress if needed
        std::vector<uint8_t> sectionBuffer;
        if (compression_ != CompressionLevel::None) {
            // Simple placeholder compression
            sectionBuffer.resize(sizeof(SynapseCheckpointData));
            std::memcpy(sectionBuffer.data(), &sectionData, sizeof(SynapseCheckpointData));
        } else {
            sectionBuffer.resize(sizeof(SynapseCheckpointData));
            std::memcpy(sectionBuffer.data(), &sectionData, sizeof(SynapseCheckpointData));
        }
        
        return writeSection(CheckpointSection::Synapses, sectionBuffer.data(), sectionBuffer.size());
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing synapses: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!isOpen() || !data || size == 0) {
        return false;
    }
    
    try {
        // Write section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = static_cast<uint64_t>(bytesWritten_);
        sectionHeader.size = size;
        sectionHeader.compressedSize = size; // Placeholder for compression
        sectionHeader.checksum = 0; // Placeholder for checksum
        
        if (!writeSectionHeader(sectionHeader)) {
            return false;
        }
        
        // Write section data
        stream_.write(static_cast<const char*>(data), size);
        if (!stream_) {
            NLM_LOG_ERROR(std::string("Failed to write section data for type ") + 
                         std::to_string(static_cast<int>(sectionType)));
            return false;
        }
        
        bytesWritten_ += size;
        
        // Update header size (placeholder - would need two-pass in real implementation)
        // For now, just update bytesWritten_ in header
        header_.totalSize = bytesWritten_;
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing section: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::finalize() {
    if (!isOpen()) {
        return false;
    }
    
    try {
        // Write final section marker
        SectionHeader endSection;
        endSection.type = static_cast<uint32_t>(CheckpointSection::End);
        endSection.offset = 0;
        endSection.size = 0;
        endSection.compressedSize = 0;
        endSection.checksum = 0;
        
        writeSectionHeader(endSection);
        
        // Update header
        header_.totalSize = bytesWritten_;
        header_.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        // For now, skip actual checksum calculation
        // header_.checksum = ChecksumCalculator::crc64(&header_, sizeof(CheckpointHeader));
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception finalizing checkpoint: ") + e.what());
        return false;
    }
}

void CheckpointWriter::abort() {
    close();
    if (std::filesystem::exists(filepath_)) {
        std::filesystem::remove(filepath_);
    }
}

bool CheckpointWriter::writeHeader() {
    try {
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_) {
            return false;
        }
        
        bytesWritten_ += sizeof(CheckpointHeader);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing header: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    try {
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
        if (!stream_) {
            NLM_LOG_ERROR("Failed to write section header");
            return false;
        }
        
        bytesWritten_ += sizeof(SectionHeader);
        sections_.push_back(header);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing section header: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = stream_.tellp();
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> paddingData(padding, 0);
        stream_.write(reinterpret_cast<const char*>(paddingData.data()), padding);
        if (!stream_) {
            return false;
        }
        
        bytesWritten_ += padding;
    }
    
    return true;
}

// CheckpointReader Implementation

CheckpointReader::CheckpointReader()
    : streamPos_(0) {
}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        close(); // Ensure any previous file is closed
        
        stream_.open(filepath, std::ios::binary);
        if (!stream_) {
            NLM_LOG_ERROR("Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_) {
            NLM_LOG_ERROR("Failed to read checkpoint header");
            close();
            return false;
        }
        
        if (!header_.validate()) {
            NLM_LOG_ERROR("Invalid checkpoint header (magic/number mismatch)");
            close();
            return false;
        }
        
        // Read section headers
        while (streamPos_ < header_.totalSize) {
            SectionHeader sectionHeader;
            stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(SectionHeader));
            if (!stream_) {
                break; // Reached end or error
            }
            
            // Check if this is an End section
            if (sectionHeader.type == static_cast<uint32_t>(CheckpointSection::End)) {
                break;
            }
            
            sections_.push_back(sectionHeader);
            streamPos_ += sizeof(SectionHeader);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception opening checkpoint reader: ") + e.what());
        close();
        return false;
    }
}

void CheckpointReader::close() {
    if (isOpen()) {
        stream_.close();
    }
    sections_.clear();
    streamPos_ = 0;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    std::vector<uint8_t> sectionData = readSection(CheckpointSection::Neurons);
    if (sectionData.empty()) {
        return false;
    }
    
    // For now, just return success - real implementation would deserialize
    data.membranePotential.resize(10); // Placeholder
    data.restingPotential.resize(10);
    // ... other fields
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    std::vector<uint8_t> sectionData = readSection(CheckpointSection::Synapses);
    if (sectionData.empty()) {
        return false;
    }
    
    // For now, just return success - real implementation would deserialize
    data.sourceNeuron.resize(10); // Placeholder
    data.destinationNeuron.resize(10);
    // ... other fields
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    // Find section header
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            // In real implementation, would seek to section and read data
            // For now, return empty vector
            return std::vector<uint8_t>();
        }
    }
    
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    // Basic validation - check header and sections
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Check section consistency
    // For now, assume valid
    return true;
}

double CheckpointReader::getProgress() const {
    if (header_.totalSize == 0) return 0.0;
    return static_cast<double>(streamPos_) / header_.totalSize;
}

// CheckpointManager Implementation

CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0) {
}

CheckpointManager::~CheckpointManager() {
    // Close any open checkpoints
}

void CheckpointManager::configure(const std::string& checkpointDir,
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Ensure directory exists
    std::filesystem::path dir(checkpointDir_);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return false;
    }
    
    return saveImmediately();
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    try {
        // Generate checkpoint name
        std::string checkpointName = name.empty() ? "checkpoint_" + std::to_string(lastSaveStep_) : name;
        std::string filepath = checkpointDir_ + "/" + checkpointName + ".nlmck";
        
        // Create checkpoint writer
        CheckpointWriter writer;
        if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            return false;
        }
        
        // In real implementation, would provide data through providers
        // writer.setNeuronProvider(neuronProvider_);
        // writer.setSynapseProvider(synapseProvider_);
        
        if (!writer.finalize()) {
            writer.abort();
            return false;
        }
        
        lastSaveStep_ = currentStep;
        lastCheckpointPath_ = filepath;
        
        // Prune old checkpoints
        pruneOldCheckpoints(maxCheckpoints_);
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".nlmck";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        NLM_LOG_ERROR("Invalid checkpoint: " + reader.getError());
        return false;
    }
    
    // In real implementation, would load data into Brain
    // This is a placeholder
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.path().extension() == ".nlmck") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            info.fileSize = entry.path().file_size();
            info.timestamp = std::chrono::system_clock::now();
            
            // In real implementation, would read metadata from file
            info.step = 0;  // Placeholder
            info.simulationTime = 0.0;  // Placeholder
            info.neuronCount = 0;  // Placeholder
            info.synapseCount = 0;  // Placeholder
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return checkpoints;
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
        std::filesystem::path filepath = checkpointDir_ + "/" + checkpoints[i].name + ".nlmck";
        try {
            if (std::filesystem::exists(filepath)) {
                std::filesystem::remove(filepath);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Exception deleting checkpoint ") + 
                         checkpoints[i].name + ": " + e.what());
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep == 0) {
        return true; // First step
    }
    
    if (currentStep >= lastSaveStep_ + saveIntervalSteps_) {
        return true;
    }
    
    return false;
}

} // namespace nlm
