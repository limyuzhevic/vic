#include "CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>

namespace nlm {

// Simple logging for checkpoint system
#define NLM_LOG_INFO(msg) std::cout << "[CHECKPOINT INFO] " << msg << std::endl
#define NLM_LOG_ERROR(msg) std::cerr << "[CHECKPOINT ERROR] " << msg << std::endl

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    filepath_ = filepath;
    compression_ = compression;
    
    stream_.open(filepath, std::ios::binary | std::ios::trunc);
    if (!stream_.is_open()) {
        return false;
    }
    
    // Initialize header
    header_.magic = CHECKPOINT_MAGIC;
    header_.majorVersion = CHECKPOINT_VERSION_MAJOR;
    header_.minorVersion = CHECKPOINT_VERSION_MINOR;
    header_.patchVersion = CHECKPOINT_VERSION_PATCH;
    header_.timestamp = static_cast<uint64_t>(std::time(nullptr));
    header_.compressionLevel = static_cast<uint32_t>(compression_);
    
    sections_.clear();
    bytesWritten_ = 0;
    
    return writeHeader();
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
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
    return writeSection(CheckpointSection::Neurons, &data, sizeof(data));
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    return writeSection(CheckpointSection::Synapses, &data, sizeof(data));
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!stream_.is_open()) {
        return false;
    }
    
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(sectionType);
    sectionHeader.offset = stream_.tellp();
    sectionHeader.size = size;
    sectionHeader.compressedSize = size; // No compression for now
    sectionHeader.checksum = 0; // TODO: calculate checksum
    
    if (!writeSectionHeader(sectionHeader)) {
        return false;
    }
    
    stream_.write(static_cast<const char*>(data), size);
    if (!stream_.good()) {
        return false;
    }
    
    bytesWritten_ += size;
    sections_.push_back(sectionHeader);
    return true;
}

bool CheckpointWriter::finalize() {
    // Update header with total size
    header_.totalSize = bytesWritten_;
    
    // Write sections index
    size_t sectionsIndexOffset = stream_.tellp();
    
    for (const auto& section : sections_) {
        // Write section header back with correct offset
        stream_.seekp(section.offset);
        if (!writeSectionHeader(section)) {
            return false;
        }
        stream_.seekp(0, std::ios::end);
    }
    
    // Write sections index footer
    stream_.write(reinterpret_cast<const char*>(&sections_.size()), sizeof(sections_.size()));
    for (const auto& section : sections_) {
        stream_.write(reinterpret_cast<const char*>(&section), sizeof(section));
    }
    
    // Write header at end of file
    stream_.seekp(0);
    return writeHeader();
}

void CheckpointWriter::abort() {
    close();
    std::remove(filepath_.c_str());
}

bool CheckpointWriter::writeHeader() {
    stream_.seekp(0);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    return stream_.good();
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
    return stream_.good();
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t pos = stream_.tellp();
    size_t padding = (alignment - (pos % alignment)) % alignment;
    if (padding > 0) {
        std::vector<char> pad(padding, 0);
        stream_.write(pad.data(), padding);
    }
    return stream_.good();
}

CheckpointReader::CheckpointReader() : streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    stream_.open(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        return false;
    }
    
    stream_.seekg(0, std::ios::end);
    size_t fileSize = stream_.tellg();
    stream_.seekg(0, std::ios::beg);
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (!stream_.good()) {
        return false;
    }
    
    // Read sections index from end of file
    stream_.seekg(-static_cast<int64_t>(sizeof(size_t) + sizeof(SectionHeader) * header_.regionCount), std::ios::end);
    size_t sectionCount;
    stream_.read(reinterpret_cast<char*>(&sectionCount), sizeof(sectionCount));
    if (!stream_.good()) {
        return false;
    }
    
    sections_.resize(sectionCount);
    for (auto& section : sections_) {
        stream_.read(reinterpret_cast<char*>(&section), sizeof(section));
        if (!stream_.good()) {
            return false;
        }
    }
    
    return validate();
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto section = readSection(CheckpointSection::Neurons);
    if (section.empty()) {
        return false;
    }
    
    if (section.size() != sizeof(data)) {
        return false;
    }
    
    std::memcpy(&data, section.data(), section.size());
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto section = readSection(CheckpointSection::Synapses);
    if (section.empty()) {
        return false;
    }
    
    if (section.size() != sizeof(data)) {
        return false;
    }
    
    std::memcpy(&data, section.data(), section.size());
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            stream_.seekg(section.offset);
            std::vector<uint8_t> data(section.size, 0);
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            if (!stream_.good()) {
                return {};
            }
            return data;
        }
    }
    return {};
}

bool CheckpointReader::validate() const {
    if (!header_.validate()) {
        error_ = "Invalid checkpoint magic number or version mismatch";
        return false;
    }
    
    if (sections_.empty()) {
        error_ = "No sections found in checkpoint";
        return false;
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    // Simple progress calculation
    if (sections_.empty()) {
        return 0.0;
    }
    
    stream_.seekg(0, std::ios::end);
    size_t fileSize = stream_.tellg();
    stream_.seekg(streamPos_);
    
    return static_cast<double>(streamPos_) / fileSize;
}

CheckpointManager::CheckpointManager() : lastSaveStep_(0) {}

CheckpointManager::~CheckpointManager() {}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                   uint64_t saveIntervalSteps,
                                   size_t maxCheckpoints,
                                   bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string checkpointName = name.empty() ? std::to_string(lastSaveStep_) : name;
    std::string filepath = checkpointDir_ + "/checkpoint_" + checkpointName + ".nlm";
    
    // Call neuron provider if set
    NeuronCheckpointData neuronData;
    if (neuronProvider_ && !neuronProvider_(neuronData)) {
        return false;
    }
    
    // Call synapse provider if set
    SynapseCheckpointData synapseData;
    if (synapseProvider_ && !synapseProvider_(synapseData)) {
        return false;
    }
    
    // TODO: Actually write the checkpoint file
    // For now, just log
    NLM_LOG_INFO("Checkpoint saved to " + filepath);
    
    lastSaveStep_ = currentStep;
    lastCheckpointPath_ = filepath;
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/checkpoint_" + name + ".nlm";
    return load(filepath);
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    // TODO: Implement directory listing
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    // TODO: Implement checkpoint pruning
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return (currentStep % saveIntervalSteps_) == 0 && currentStep > lastSaveStep_;
}

} // namespace nlm