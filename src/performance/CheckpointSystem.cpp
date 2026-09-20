#include "CheckpointSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>

namespace nlm {

CheckpointReader::CheckpointReader() : stream_(), header_(), sections_(), error_(), streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    close();
    
    // Ensure directory exists
    std::filesystem::path path(filepath);
    std::filesystem::path parent = path.parent_path();
    if (!parent.empty() && !std::filesystem::exists(parent)) {
        std::filesystem::create_directories(parent);
    }
    
    stream_.open(filepath, std::ios::binary | std::ios::in);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file for reading: " + filepath;
        return false;
    }
    
    // Read header
    if (!stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader))) {
        error_ = "Failed to read checkpoint header: " + filepath;
        close();
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header in file: " + filepath;
        close();
        return false;
    }
    
    // Read sections
    streamPos_ = sizeof(CheckpointHeader);
    bool success = readSections();
    
    if (!success) {
        error_ = "Failed to read checkpoint sections";
        close();
        return false;
    }
    
    return true;
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Neurons);
    if (sectionData.empty()) {
        return false;
    }
    
    // Simple serialization: reconstruct from section data
    // In a real implementation, this would parse the actual serialized data
    size_t expectedSize = header_.neuronCount * sizeof(float) * 13; // 13 float arrays per neuron
    if (sectionData.size() < expectedSize) {
        error_ = "Neuron checkpoint data size mismatch";
        return false;
    }
    
    // For now, just return empty data - real implementation would parse
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Synapses);
    if (sectionData.empty()) {
        return false;
    }
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& section : sections_) {
        if (section.type == sectionType) {
            stream_.seekg(section.offset, std::ios::beg);
            std::vector<uint8_t> buffer(section.size);
            if (stream_.read(reinterpret_cast<char*>(buffer.data()), section.size)) {
                return buffer;
            } else {
                error_ = "Failed to read section data";
            }
        }
    }
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    // Basic validation: header is valid, sections are readable
    if (!header_.validate()) {
        return false;
    }
    
    // Check section offsets and sizes
    for (size_t i = 0; i < sections_.size(); ++i) {
        const auto& section = sections_[i];
        if (i > 0) {
            if (section.offset < sections_[i-1].offset + sections_[i-1].size) {
                error_ = "Section overlap detected";
                return false;
            }
        }
    }
    
    // Verify checksum if available
    auto checksumSection = readSection(CheckpointSection::Checksum);
    if (!checksumSection.empty() && checksumSection.size() >= sizeof(uint64_t)) {
        uint64_t fileChecksum = *reinterpret_cast<const uint64_t*>(checksumSection.data());
        uint64_t computedChecksum = computeChecksum();
        if (fileChecksum != computedChecksum) {
            error_ = "Checkpoint integrity check failed";
            return false;
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    size_t totalSize = 0;
    for (const auto& section : sections_) {
        totalSize += section.size;
    }
    
    size_t readSize = 0;
    for (const auto& section : sections_) {
        if (section.offset <= streamPos_ && streamPos_ < section.offset + section.size) {
            readSize += std::min(static_cast<size_t>(section.offset + section.size - streamPos_), 
                              section.size);
        }
    }
    
    return static_cast<double>(readSize) / totalSize;
}

uint64_t CheckpointReader::computeChecksum() const {
    // Simple checksum computation for validation
    stream_.seekg(0, std::ios::beg);
    std::vector<char> buffer(4096);
    uint64_t checksum = 0;
    
    while (stream_.read(buffer.data(), buffer.size())) {
        for (size_t i = 0; i < buffer.size(); ++i) {
            checksum = (checksum + static_cast<uint64_t>(buffer[i])) * 31;
        }
    }
    
    return checksum;
}

CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(CompressionLevel::Balanced), 
                                      header_(), sections_(), bytesWritten_(0), compressBuffer_() {}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    abort();
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Ensure directory exists
    std::filesystem::path path(filepath);
    std::filesystem::path parent = path.parent_path();
    if (!parent.empty() && !std::filesystem::exists(parent)) {
        std::filesystem::create_directories(parent);
    }
    
    stream_.open(filepath, std::ios::binary | std::ios::out);
    if (!stream_.is_open()) {
        error_ = "Failed to create checkpoint file: " + filepath;
        return false;
    }
    
    // Initialize header
    header_.timestamp = static_cast<uint64_t>(std::time(nullptr));
    header_.compressionLevel = static_cast<uint32_t>(compression_);
    
    return writeHeader();
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
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
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    // Simple serialization: concatenate all arrays
    std::vector<uint8_t> buffer;
    
    // Write neuron count
    uint64_t neuronCount = data.membranePotential.size();
    if (neuronCount == 0) return true;
    
    // Write all neuron data arrays
    auto writeArray = [&buffer](const auto& arr) {
        for (const auto& item : arr) {
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&item);
            size_t itemSize = sizeof(item);
            buffer.insert(buffer.end(), bytes, bytes + itemSize);
        }
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
    
    // Write section
    return writeSection(CheckpointSection::Neurons, buffer.data(), buffer.size());
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    // Similar serialization for synapses
    std::vector<uint8_t> buffer;
    
    uint64_t synapseCount = data.sourceNeuron.size();
    if (synapseCount == 0) return true;
    
    auto writeArray = [&buffer](const auto& arr) {
        for (const auto& item : arr) {
            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&item);
            size_t itemSize = sizeof(item);
            buffer.insert(buffer.end(), bytes, bytes + itemSize);
        }
    };
    
    writeArray(data.sourceNeuron);
    writeArray(data.destinationNeuron);
    writeArray(data.weight);
    writeArray(data.delay);
    writeArray(data.synapseType);
    writeArray(data.plasticityFlags);
    writeArray(data.eligibilityTrace);
    writeArray(data.efficacy);
    writeArray(data.shortTermDepression);
    writeArray(data.shortTermFacilitation);
    
    return writeSection(CheckpointSection::Synapses, buffer.data(), buffer.size());
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    // Compress if requested
    std::vector<uint8_t> compressedData;
    size_t compressedSize = size;
    
    if (compression_ != CompressionLevel::None) {
        // Simple compression placeholder
        compressedData.resize(size);
        std::copy(static_cast<const uint8_t*>(data), 
                 static_cast<const uint8_t*>(data) + size,
                 compressedData.data());
        compressedSize = compressedData.size();
    }
    
    // Write section header
    SectionHeader section;
    section.type = static_cast<uint32_t>(sectionType);
    section.offset = sizeof(CheckpointHeader) + sizeof(SectionHeader) * sections_.size();
    section.size = size;
    section.compressedSize = compressedSize;
    section.checksum = 0; // TODO: compute actual checksum
    
    // Write section header
    if (!writeSectionHeader(section)) {
        return false;
    }
    
    // Write section data
    const uint8_t* sourceData = compression_ != CompressionLevel::None ? 
                               compressedData.data() : static_cast<const uint8_t*>(data);
    
    if (!stream_.write(reinterpret_cast<const char*>(sourceData), sourceData[0])) {
        error_ = "Failed to write section data";
        return false;
    }
    
    bytesWritten_ += section.size;
    sections_.push_back(section);
    
    // Pad to alignment
    size_t currentPos = sizeof(CheckpointHeader) + sizeof(SectionHeader) * sections_.size();
    if (currentPos < section.offset + section.size) {
        size_t padding = section.offset + section.size - currentPos;
        std::vector<char> paddingBuffer(padding, 0);
        stream_.write(paddingBuffer.data(), padding);
    }
    
    return true;
}

bool CheckpointWriter::finalize() {
    // Write checksum
    uint64_t checksum = computeChecksum();
    
    if (!writeSection(CheckpointSection::Checksum, &checksum, sizeof(checksum))) {
        return false;
    }
    
    // Update header with total size and checksum
    header_.totalSize = bytesWritten_;
    header_.checksum = checksum;
    
    // Write updated header at beginning of file
    stream_.seekp(0, std::ios::beg);
    if (!stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader))) {
        error_ = "Failed to write final header";
        return false;
    }
    
    return true;
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        // Remove partial file
        if (!filepath_.empty()) {
            std::filesystem::remove(filepath_);
        }
    }
}

uint64_t CheckpointWriter::computeChecksum() const {
    // Simple checksum computation
    stream_.seekp(0, std::ios::beg);
    std::vector<char> buffer(4096);
    uint64_t checksum = 0;
    
    while (stream_.read(buffer.data(), buffer.size())) {
        for (size_t i = 0; i < buffer.size(); ++i) {
            checksum = (checksum + static_cast<uint64_t>(buffer[i])) * 31;
        }
    }
    
    return checksum;
}

bool CheckpointWriter::writeHeader() {
    if (!stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader))) {
        error_ = "Failed to write checkpoint header";
        return false;
    }
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    if (!stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader))) {
        error_ = "Failed to write section header";
        return false;
    }
    return true;
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = stream_.tellp();
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<char> paddingBuffer(padding, 0);
        if (!stream_.write(paddingBuffer.data(), padding)) {
            error_ = "Failed to pad to alignment";
            return false;
        }
    }
    
    return true;
}

CheckpointManager::CheckpointManager() : checkpointDir_(), saveIntervalSteps_(10000), 
                                        maxCheckpoints_(10), compress_(true), lastSaveStep_(0),
                                        lastCheckpointPath_() {}

CheckpointManager::~CheckpointManager() {}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Ensure directory exists
    std::filesystem::path path(checkpointDir);
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directories(path);
    }
}

void CheckpointManager::setNeuronProvider(std::function<bool(NeuronCheckpointData&)> provider) {
    neuronProvider_ = provider;
}

void CheckpointManager::setSynapseProvider(std::function<bool(SynapseCheckpointData&)> provider) {
    synapseProvider_ = provider;
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (currentStep < lastSaveStep_) return false;
    
    if (shouldSave(currentStep)) {
        std::string name = "checkpoint_" + std::to_string(currentStep);
        return saveImmediately(name);
    }
    
    lastSaveStep_ = currentStep;
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        return false;
    }
    
    std::string filepath = checkpointDir_;
    if (!filepath.empty() && filepath.back() != '/') {
        filepath += '/';
    }
    filepath += name + ".nchk";
    
    CheckpointWriter writer;
    CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
    
    if (!writer.create(filepath, compression)) {
        return false;
    }
    
    // Collect checkpoint data
    NeuronCheckpointData neuronData;
    SynapseCheckpointData synapseData;
    
    if (!neuronProvider_(neuronData) || !synapseProvider_(synapseData)) {
        return false;
    }
    
    // Set metadata
    writer.setMetadata(neuronData.membranePotential.size(),
                      synapseData.sourceNeuron.size(),
                      0, // region count
                      lastSaveStep_,
                      currentTime);
    
    // Write data
    bool success = writer.writeNeurons(neuronData) && writer.writeSynapses(synapseData);
    
    if (success) {
        writer.close();
        lastCheckpointPath_ = filepath;
        pruneOldCheckpoints(maxCheckpoints_);
    }
    
    return success;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_;
    if (!filepath.empty() && filepath.back() != '/') {
        filepath += '/';
    }
    filepath += name + ".nchk";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        return false;
    }
    
    // Load data - in real implementation would populate brain state
    // For now just return true if validation passed
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (checkpointDir_.empty()) {
        return checkpoints;
    }
    
    std::filesystem::path dir(checkpointDir_);
    if (!std::filesystem::exists(dir)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".nchk") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            info.step = std::stoull(info.name.substr(11)); // checkpoint_123 -> 123
            
            auto ftime = std::filesystem::last_write_time(entry);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
            );
            info.timestamp = std::chrono::system_clock::to_time_t(sctp);
            
            info.fileSize = entry.file_size();
            info.neuronCount = 0; // Would need to read from file
            info.synapseCount = 0; // Would need to read from file
            
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
    
    // Sort by timestamp (oldest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp < b.timestamp;
              });
    
    // Remove oldest checkpoints
    size_t toRemove = checkpoints.size() - keepCount;
    for (size_t i = 0; i < toRemove; ++i) {
        std::string filepath = checkpointDir_;
        if (!filepath.empty() && filepath.back() != '/') {
            filepath += '/';
        }
        filepath += checkpoints[i].name + ".nchk";
        std::filesystem::remove(filepath);
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_ ||
           currentStep == 0; // Always save at step 0
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

} // namespace nlm
