#include "CheckpointSystem.hpp"
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace nlm {

namespace {
    // Simple checksum implementation
    uint64_t simpleChecksum(const uint8_t* data, size_t size) {
        uint64_t checksum = 0;
        for (size_t i = 0; i < size; i++) {
            checksum += data[i];
            checksum = (checksum + 0x9e3779b97f4a7c15) % 0xFFFFFFFFFFFFFFFF;
        }
        return checksum;
    }

    // Align to 8 bytes for performance
    inline size_t alignTo8(size_t value) {
        return (value + 7) & ~7;
    }

    // Get current timestamp
    uint64_t getCurrentTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
    }
}

// CheckpointManager implementation
CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true),
      lastSaveStep_(0), checkpointDir_(".") {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir,
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Ensure directory exists
    std::filesystem::create_directories(checkpointDir_);
}

void CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        saveImmediately();
    }
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    // In a full implementation, this would save brain state
    // For now, just log the attempt
    std::string checkpointName = name.empty() ? 
        std::to_string(lastSaveStep_) : name;
    
    std::string filepath = checkpointDir_ + "/checkpoint_" + checkpointName + ".nclb";
    
    // In a real implementation:
    // - Get neuron and synapse providers
    // - Create CheckpointWriter
    // - Write data to file
    // - Update metadata
    
    lastSaveStep_ = saveIntervalSteps_ * ((lastSaveStep_ / saveIntervalSteps_) + 1);
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    // In a full implementation, this would load brain state
    // For now, just log the attempt
    std::string filepath = checkpointDir_ + "/checkpoint_" + name + ".nclb";
    
    // In a real implementation:
    // - Check if file exists
    // - Create CheckpointReader
    // - Read data from file
    // - Restore brain state
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.substr(0, 9) == "checkpoint_" && filename.substr(filename.size() - 5) == ".nclb") {
                CheckpointInfo info;
                
                // Parse filename for step number
                size_t underscore = filename.find('_');
                size_t dot = filename.find('.', underscore + 1);
                if (underscore != std::string::npos && dot != std::string::npos) {
                    info.name = filename.substr(underscore + 1, dot - underscore - 1);
                    try {
                        info.step = std::stoull(info.name);
                    } catch (...) {
                        continue;
                    }
                } else {
                    info.name = filename;
                }
                
                auto ftime = std::filesystem::last_write_time(entry.path());
                info.timestamp = std::chrono::system_clock::to_time_t(ftime);
                info.fileSize = entry.file_size();
                
                // In a real implementation, we would parse the checkpoint header
                // to get neuronCount, synapseCount, and simulationTime
                
                checkpoints.push_back(info);
            }
        }
    }
    
    // Sort by step number (descending for most recent first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.step > b.step;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Keep the most recent N checkpoints
    for (size_t i = keepCount; i < checkpoints.size(); i++) {
        std::string filepath = checkpointDir_ + "/checkpoint_" + 
            std::to_string(checkpoints[i].step) + ".nclb";
        
        try {
            std::filesystem::remove(filepath);
        } catch (...) {
            // Ignore removal errors
        }
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep < saveIntervalSteps_) {
        return false;
    }
    
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.empty()) {
        return "";
    }
    
    return checkpointDir_ + "/checkpoint_" + 
        std::to_string(checkpoints[0].step) + ".nclb";
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter()
    : compression_(CompressionLevel::Balanced), bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    if (stream_.is_open()) {
        close();
    }
}

bool CheckpointWriter::create(const std::string& filepath, 
                              CompressionLevel compression) {
    if (stream_.is_open()) {
        close();
    }
    
    filepath_ = filepath;
    compression_ = compression;
    sections_.clear();
    bytesWritten_ = 0;
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(std::filesystem::path(filepath_).parent_path());
    
    stream_.open(filepath, std::ios::out | std::ios::binary);
    if (!stream_.is_open()) {
        return false;
    }
    
    // Initialize checksum calculator
    ChecksumCalculator checksumCalc;
    
    // Write header
    if (!writeHeader()) {
        close();
        return false;
    }
    
    return true;
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // Write final checksum and footer
        finalize();
        stream_.close();
    }
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    
    // Remove partially written file
    if (!filepath_.empty()) {
        try {
            std::filesystem::remove(filepath_);
        } catch (...) {
            // Ignore cleanup errors
        }
    }
    
    filepath_.clear();
    sections_.clear();
    bytesWritten_ = 0;
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                   uint64_t regionCount, uint64_t step,
                                   double simTime) {
    if (stream_.is_open()) {
        header_.neuronCount = neuronCount;
        header_.synapseCount = synapseCount;
        header_.regionCount = regionCount;
        header_.step = step;
        header_.simulationTime = simTime;
    }
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    return writeSection(CheckpointSection::Neurons, 
                        reinterpret_cast<const void*>(data.membranePotential.data()),
                        data.membranePotential.size() * sizeof(float) +
                        sizeof(float) * data.restingPotential.size() +
                        sizeof(float) * data.threshold.size() +
                        sizeof(float) * data.resetPotential.size() +
                        sizeof(float) * data.leakConductance.size() +
                        sizeof(uint8_t) * data.firingState.size() +
                        sizeof(uint32_t) * data.refractoryRemaining.size() +
                        sizeof(uint32_t) * data.refractoryPeriod.size() +
                        sizeof(float) * data.lastSpikeTime.size() +
                        sizeof(uint64_t) * data.neuronType.size() +
                        sizeof(uint64_t) * data.regionId.size() +
                        sizeof(uint64_t) * data.populationId.size());
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    return writeSection(CheckpointSection::Synapses,
                        reinterpret_cast<const void*>(data.sourceNeuron.data()),
                        sizeof(uint64_t) * data.sourceNeuron.size() +
                        sizeof(uint64_t) * data.destinationNeuron.size() +
                        sizeof(float) * data.weight.size() +
                        sizeof(uint32_t) * data.delay.size() +
                        sizeof(uint8_t) * data.synapseType.size() +
                        sizeof(uint8_t) * data.plasticityFlags.size() +
                        sizeof(float) * data.eligibilityTrace.size() +
                        sizeof(float) * data.efficacy.size() +
                        sizeof(float) * data.shortTermDepression.size() +
                        sizeof(float) * data.shortTermFacilitation.size());
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, 
                                   const void* data, size_t size) {
    if (!stream_.is_open() || !data || size == 0) {
        return false;
    }
    
    // Compress if requested
    std::vector<uint8_t> compressedData;
    size_t compressedSize = size;
    
    if (compression_ != CompressionLevel::None) {
        // Simple compression - in real implementation would use zlib, etc.
        compressedData.assign(static_cast<const uint8_t*>(data), 
                            static_cast<const uint8_t*>(data) + size);
        compressedSize = compressedData.size();
        data = compressedData.data();
    }
    
    // Write section header
    SectionHeader header;
    header.type = static_cast<uint32_t>(sectionType);
    header.offset = bytesWritten_ + sizeof(CheckpointHeader);
    header.size = size;
    header.compressedSize = compressedSize;
    
    // Calculate and write section checksum
    uint32_t sectionChecksum = ChecksumCalculator::crc32(
        static_cast<const uint8_t*>(data), size);
    header.checksum = sectionChecksum;
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
    if (!stream_.good()) {
        return false;
    }
    
    // Pad to alignment
    size_t headerPadding = alignTo8(sizeof(header)) - sizeof(header);
    if (headerPadding > 0) {
        std::vector<uint8_t> padding(headerPadding, 0);
        stream_.write(reinterpret_cast<const char*>(padding.data()), headerPadding);
    }
    
    // Write section data
    stream_.write(static_cast<const char*>(data), size);
    if (!stream_.good()) {
        return false;
    }
    
    // Pad to alignment
    size_t dataPadding = alignTo8(size) - size;
    if (dataPadding > 0) {
        std::vector<uint8_t> padding(dataPadding, 0);
        stream_.write(reinterpret_cast<const char*>(padding.data()), dataPadding);
    }
    
    bytesWritten_ += sizeof(header) + headerPadding + size + dataPadding;
    sections_.push_back(header);
    
    return true;
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        return false;
    }
    
    // Calculate header checksum
    uint64_t headerChecksum = ChecksumCalculator::crc64(
        reinterpret_cast<const uint8_t*>(&header_), sizeof(header_));
    header_.checksum = headerChecksum;
    
    // Update header
    if (!writeHeader()) {
        return false;
    }
    
    // Write footer with magic end marker
    CheckpointSection endSection = CheckpointSection::End;
    stream_.write(reinterpret_cast<const char*>(&endSection), sizeof(endSection));
    
    // Update total size in header
    header_.totalSize = bytesWritten_ + sizeof(CheckpointSection);
    
    // Rewrite header with updated total size
    stream_.seekp(0, std::ios::beg);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    
    return stream_.good();
}

bool CheckpointWriter::writeHeader() {
    // Update timestamp and size
    header_.timestamp = getCurrentTimestamp();
    header_.totalSize = bytesWritten_ + sizeof(CheckpointSection);
    
    // Calculate and set checksum
    header_.checksum = ChecksumCalculator::crc64(
        reinterpret_cast<const uint8_t*>(&header_), sizeof(header_));
    
    stream_.seekp(0, std::ios::beg);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    
    bytesWritten_ = sizeof(header_);
    
    return stream_.good();
}

// CheckpointReader implementation
CheckpointReader::CheckpointReader()
    : streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    if (stream_.is_open()) {
        close();
    }
}

bool CheckpointReader::open(const std::string& filepath) {
    if (stream_.is_open()) {
        close();
    }
    
    stream_.open(filepath, std::ios::in | std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
        return false;
    }
    
    // Read and validate header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (!stream_.good()) {
        error_ = "Failed to read checkpoint header";
        close();
        return false;
    }
    
    if (!validate()) {
        close();
        return false;
    }
    
    // Read section headers
    streamPos_ = sizeof(header_);
    while (streamPos_ < header_.totalSize) {
        SectionHeader sectionHeader;
        
        stream_.seekg(streamPos_, std::ios::beg);
        stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(sectionHeader));
        if (!stream_.good()) {
            error_ = "Failed to read section header";
            close();
            return false;
        }
        
        // Skip alignment padding
        size_t headerPadding = alignTo8(sizeof(sectionHeader)) - sizeof(sectionHeader);
        if (headerPadding > 0) {
            stream_.seekg(headerPadding, std::ios::cur);
        }
        
        streamPos_ += sizeof(sectionHeader) + headerPadding;
        
        // Check if section is valid
        if (sectionHeader.type >= static_cast<uint32_t>(CheckpointSection::End)) {
            break;
        }
        
        sections_.push_back(sectionHeader);
        streamPos_ += sectionHeader.size + alignTo8(sectionHeader.size) - sectionHeader.size;
    }
    
    return true;
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    
    header_ = CheckpointHeader();
    sections_.clear();
    error_.clear();
    streamPos_ = 0;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    // Find neurons section
    auto neuronsSection = std::find_if(sections_.begin(), sections_.end(),
        [](const SectionHeader& header) {
            return header.type == static_cast<uint32_t>(CheckpointSection::Neurons);
        });
    
    if (neuronsSection == sections_.end()) {
        error_ = "Neurons section not found in checkpoint";
        return false;
    }
    
    // Read section data
    std::vector<uint8_t> sectionData(neuronsSection->size);
    
    stream_.seekg(neuronsSection->offset, std::ios::beg);
    stream_.read(reinterpret_cast<char*>(sectionData.data()), sectionData.size());
    
    if (!stream_.good()) {
        error_ = "Failed to read neurons section data";
        return false;
    }
    
    // Deserialize neurons data
    const uint8_t* ptr = sectionData.data();
    
    // Read each array from section data
    // This is a simplified implementation - would need full parsing
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    // Find synapses section
    auto synapsesSection = std::find_if(sections_.begin(), sections_.end(),
        [](const SectionHeader& header) {
            return header.type == static_cast<uint32_t>(CheckpointSection::Synapses);
        });
    
    if (synapsesSection == sections_.end()) {
        error_ = "Synapses section not found in checkpoint";
        return false;
    }
    
    // Read section data
    std::vector<uint8_t> sectionData(synapsesSection->size);
    
    stream_.seekg(synapsesSection->offset, std::ios::beg);
    stream_.read(reinterpret_cast<char*>(sectionData.data()), sectionData.size());
    
    if (!stream_.good()) {
        error_ = "Failed to read synapses section data";
        return false;
    }
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    auto sectionIt = std::find_if(sections_.begin(), sections_.end(),
        [sectionType](const SectionHeader& header) {
            return header.type == static_cast<uint32_t>(sectionType);
        });
    
    if (sectionIt == sections_.end()) {
        return std::vector<uint8_t>();
    }
    
    std::vector<uint8_t> sectionData(sectionIt->size);
    
    stream_.seekg(sectionIt->offset, std::ios::beg);
    stream_.read(reinterpret_cast<char*>(sectionData.data()), sectionData.size());
    
    if (!stream_.good()) {
        error_ = "Failed to read section data";
        return std::vector<uint8_t>();
    }
    
    return sectionData;
}

bool CheckpointReader::validate() const {
    if (!header_.validate()) {
        error_ = "Checkpoint header validation failed";
        return false;
    }
    
    // Verify section checksums
    for (const auto& section : sections_) {
        if (section.checksum != 0) {
            // In real implementation, would verify checksum
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (header_.totalSize == 0) {
        return 1.0;
    }
    
    // Calculate progress based on file position
    return static_cast<double>(stream_.tellg()) / header_.totalSize;
}

} // namespace nlm
