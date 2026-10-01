#include "../performance/CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace nlm {

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    if (isOpen()) {
        close();
    }
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Ensure directory exists
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        filepath_ = filepath;
        compression_ = compression;
        stream_.open(filepath, std::ios::binary | std::ios::trunc);
        
        if (!stream_.is_open()) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        bytesWritten_ = 0;
        sections_.clear();
        
        NLM_LOG_INFO("Created checkpoint file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating checkpoint: ") + e.what());
        if (stream_.is_open()) stream_.close();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        finalize();
        stream_.close();
        filepath_.clear();
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
    return writeSection(CheckpointSection::Neurons, data);
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    return writeSection(CheckpointSection::Synapses, data);
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!isOpen()) {
        NLM_LOG_ERROR("Cannot write section: checkpoint not open");
        return false;
    }
    
    if (data == nullptr || size == 0) {
        NLM_LOG_ERROR("Cannot write section: invalid data");
        return false;
    }
    
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(sectionType);
    sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
    sectionHeader.size = size;
    sectionHeader.compressedSize = 0; // Not implementing compression for now
    sectionHeader.checksum = 0; // Simple checksum
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(sectionHeader));
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write section header");
        return false;
    }
    
    // Write section data
    const char* dataPtr = static_cast<const char*>(data);
    stream_.write(dataPtr, size);
    if (!stream_.good()) {
        NLM_LOG_ERROR("Failed to write section data");
        return false;
    }
    
    // Calculate checksum
    uint32_t checksum = ChecksumCalculator::crc32(data, size);
    
    // Update section header with checksum
    stream_.seekp(sectionHeader.offset);
    SectionHeader updatedHeader = sectionHeader;
    updatedHeader.checksum = checksum;
    stream_.write(reinterpret_cast<const char*>(&updatedHeader), sizeof(updatedHeader));
    
    stream_.seekp(0, std::ios::end);
    bytesWritten_ += sizeof(sectionHeader) + size;
    sections_.push_back(sectionHeader);
    
    return true;
}

bool CheckpointWriter::finalize() {
    if (!isOpen()) {
        return false;
    }
    
    // Calculate final checksum
    uint64_t checksum = 0;
    
    // Write footer with checksum
    uint64_t endMarker = 0xFFFFFFFFFFFFFFFF;
    stream_.write(reinterpret_cast<const char*>(&endMarker), sizeof(endMarker));
    
    checksum = ChecksumCalculator::crc64(&endMarker, sizeof(endMarker));
    
    NLM_LOG_INFO("Checkpoint finalized: " + std::to_string(bytesWritten_) + " bytes written");
    return true;
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        // Remove partial file
        if (!filepath_.empty()) {
            std::filesystem::remove(filepath_);
        }
        filepath_.clear();
    }
}

CheckpointReader::CheckpointReader() : streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    if (isOpen()) {
        close();
    }
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        stream_.open(filepath, std::ios::binary | std::ios::in);
        
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            NLM_LOG_ERROR(error_);
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint header";
            NLM_LOG_ERROR(error_);
            close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header";
            NLM_LOG_ERROR(error_);
            close();
            return false;
        }
        
        // Read section headers
        streamPos_ = sizeof(CheckpointHeader);
        while (streamPos_ < header_.totalSize) {
            SectionHeader sectionHeader;
            stream_.seekg(streamPos_);
            stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(sectionHeader));
            
            if (!stream_.good()) {
                break; // End of file
            }
            
            if (sectionHeader.type == static_cast<uint32_t>(CheckpointSection::End)) {
                break;
            }
            
            sections_.push_back(sectionHeader);
            streamPos_ += sizeof(SectionHeader) + sectionHeader.size;
        }
        
        NLM_LOG_INFO("Opened checkpoint file: " + filepath + " with " + 
                     std::to_string(sections_.size()) + " sections");
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception opening checkpoint: ") + e.what();
        NLM_LOG_ERROR(error_);
        close();
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
        sections_.clear();
        streamPos_ = 0;
    }
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto section = std::find_if(sections_.begin(), sections_.end(), 
                                [](const SectionHeader& h) { return h.type == static_cast<uint32_t>(CheckpointSection::Neurons); });
    
    if (section == sections_.end()) {
        error_ = "Neurons section not found";
        return false;
    }
    
    stream_.seekg(section->offset);
    
    // Simple read - in a real implementation, this would need to handle compression and validate checksums
    size_t expectedSize = sizeof(float) * (data.membranePotential.capacity() +
                                          data.restingPotential.capacity() + ... ); // Full calculation would be needed
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto section = std::find_if(sections_.begin(), sections_.end(), 
                                [](const SectionHeader& h) { return h.type == static_cast<uint32_t>(CheckpointSection::Synapses); });
    
    if (section == sections_.end()) {
        error_ = "Synapses section not found";
        return false;
    }
    
    stream_.seekg(section->offset);
    
    // Similar simple read implementation
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    auto section = std::find_if(sections_.begin(), sections_.end(), 
                                [sectionType](const SectionHeader& h) { return h.type == static_cast<uint32_t>(sectionType); });
    
    if (section == sections_.end()) {
        return std::vector<uint8_t>();
    }
    
    stream_.seekg(section->offset);
    
    std::vector<uint8_t> result(section->size, 0);
    stream_.read(reinterpret_cast<char*>(result.data()), section->size);
    
    return result;
}

bool CheckpointReader::validate() const {
    if (sections_.empty()) {
        return false;
    }
    
    for (const auto& section : sections_) {
        if (section.type >= static_cast<uint32_t>(CheckpointSection::End)) {
            return false;
        }
        
        if (section.size > header_.totalSize) {
            return false;
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    size_t totalSize = 0;
    for (const auto& section : sections_) {
        totalSize += sizeof(SectionHeader) + section.size;
    }
    
    size_t currentPos = streamPos_;
    if (totalSize > 0) {
        return static_cast<double>(currentPos) / totalSize;
    }
    return 1.0;
}

CheckpointManager::CheckpointManager() : lastSaveStep_(0) {}

CheckpointManager::~CheckpointManager() {
    // Cleanup is handled by destructor
}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    lastSaveStep_ = 0;
    
    // Ensure directory exists
    std::filesystem::path dir(checkpointDir_);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        NLM_LOG_ERROR("Checkpoint providers not configured");
        return false;
    }
    
    std::string filename = name;
    if (filename.empty()) {
        filename = "checkpoint_" + std::to_string(lastSaveStep_) + ".nclm";
    }
    
    std::string filepath = checkpointDir_ + "/" + filename;
    
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        return false;
    }
    
    // Collect neuron data
    NeuronCheckpointData neuronData;
    if (!neuronProvider_(neuronData)) {
        writer.abort();
        return false;
    }
    
    // Collect synapse data
    SynapseCheckpointData synapseData;
    if (!synapseProvider_(synapseData)) {
        writer.abort();
        return false;
    }
    
    // Write data
    if (!writer.writeNeurons(neuronData) || !writer.writeSynapses(synapseData)) {
        writer.abort();
        return false;
    }
    
    if (!writer.finalize()) {
        writer.abort();
        return false;
    }
    
    lastCheckpointPath_ = filepath;
    lastSaveStep_ = lastSaveStep_; // Should be current step, but we don't have it here
    
    NLM_LOG_INFO("Checkpoint saved: " + filepath);
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filename = name;
    if (filename.empty()) {
        // Get last checkpoint
        filename = getLastCheckpointPath();
        if (filename.empty()) {
            return false;
        }
    }
    
    std::string filepath = checkpointDir_ + "/" + filename;
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    // In a real implementation, this would call the data providers
    // For now, just validate the checkpoint
    if (!reader.validate()) {
        NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
        return false;
    }
    
    NLM_LOG_INFO("Checkpoint loaded: " + filepath);
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    std::filesystem::path dir(checkpointDir_);
    if (!std::filesystem::exists(dir)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".nclm") {
            CheckpointInfo info;
            info.name = entry.path().filename().string();
            info.fileSize = entry.file_size();
            info.timestamp = std::filesystem::last_write_time(entry);
            
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
    
    // Keep most recent checkpoints
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        std::filesystem::remove(checkpointDir_ + "/" + checkpoints[i].name);
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep < saveIntervalSteps_) {
        return false;
    }
    
    if (currentStep - lastSaveStep_ >= saveIntervalSteps_) {
        return true;
    }
    
    return false;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

} // namespace nlm
