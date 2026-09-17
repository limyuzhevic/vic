// src/performance/CheckpointSystem.cpp
#include "CheckpointSystem.hpp"
#include <cstring>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <zlib.h>  // For compression if available

namespace nlm {

CheckpointReader::CheckpointReader() : streamPos_(0), error_("") {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    close();
    
    try {
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint header";
            stream_.close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header (magic/version mismatch)";
            stream_.close();
            return false;
        }
        
        // Read section headers
        sections_.resize(header_.regionCount * 2);  // Approximate sections
        streamPos_ = sizeof(CheckpointHeader);
        
        for (size_t i = 0; i < sections_.size() && streamPos_ < header_.totalSize; ++i) {
            stream_.seekg(streamPos_);
            stream_.read(reinterpret_cast<char*>(&sections_[i]), sizeof(SectionHeader));
            if (!stream_.good()) break;
            
            streamPos_ += sizeof(SectionHeader) + sections_[i].size;
            if (sections_[i].compressedSize > 0) {
                streamPos_ += sections_[i].compressedSize - sections_[i].size;
            }
        }
        
        error_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception opening checkpoint: ") + e.what();
        close();
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    streamPos_ = 0;
    sections_.clear();
    error_.clear();
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    if (!isOpen()) return false;
    
    try {
        auto section = readSection(CheckpointSection::Neurons);
        if (section.empty()) {
            error_ = "Failed to read neurons section";
            return false;
        }
        
        const uint8_t* ptr = section.data();
        const uint8_t* end = ptr + section.size();
        
        // Deserialize neurons data
        data.membranePotential.clear();
        data.restingPotential.clear();
        data.threshold.clear();
        data.resetPotential.clear();
        data.leakConductance.clear();
        data.firingState.clear();
        data.refractoryRemaining.clear();
        data.refractoryPeriod.clear();
        data.lastSpikeTime.clear();
        data.neuronType.clear();
        data.regionId.clear();
        data.populationId.clear();
        
        while (ptr + sizeof(float) <= end) {
            data.membranePotential.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        while (ptr + sizeof(float) <= end) {
            data.restingPotential.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        while (ptr + sizeof(float) <= end) {
            data.threshold.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        while (ptr + sizeof(float) <= end) {
            data.resetPotential.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        while (ptr + sizeof(float) <= end) {
            data.leakConductance.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        while (ptr + sizeof(uint8_t) <= end) {
            data.firingState.push_back(*reinterpret_cast<const uint8_t*>(ptr)); ptr += sizeof(uint8_t);
        }
        while (ptr + sizeof(uint32_t) <= end) {
            data.refractoryRemaining.push_back(*reinterpret_cast<const uint32_t*>(ptr)); ptr += sizeof(uint32_t);
        }
        while (ptr + sizeof(uint32_t) <= end) {
            data.refractoryPeriod.push_back(*reinterpret_cast<const uint32_t*>(ptr)); ptr += sizeof(uint32_t);
        }
        while (ptr + sizeof(float) <= end) {
            data.lastSpikeTime.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        while (ptr + sizeof(uint64_t) <= end) {
            data.neuronType.push_back(*reinterpret_cast<const uint64_t*>(ptr)); ptr += sizeof(uint64_t);
        }
        while (ptr + sizeof(uint64_t) <= end) {
            data.regionId.push_back(*reinterpret_cast<const uint64_t*>(ptr)); ptr += sizeof(uint64_t);
        }
        while (ptr + sizeof(uint64_t) <= end) {
            data.populationId.push_back(*reinterpret_cast<const uint64_t*>(ptr)); ptr += sizeof(uint64_t);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading neurons: ") + e.what();
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    if (!isOpen()) return false;
    
    try {
        auto section = readSection(CheckpointSection::Synapses);
        if (section.empty()) {
            error_ = "Failed to read synapses section";
            return false;
        }
        
        const uint8_t* ptr = section.data();
        const uint8_t* end = ptr + section.size();
        
        // Deserialize synapses data
        data.sourceNeuron.clear();
        data.destinationNeuron.clear();
        data.weight.clear();
        data.delay.clear();
        data.synapseType.clear();
        data.plasticityFlags.clear();
        data.eligibilityTrace.clear();
        data.efficacy.clear();
        data.shortTermDepression.clear();
        data.shortTermFacilitation.clear();
        
        while (ptr + sizeof(uint64_t) <= end) {
            data.sourceNeuron.push_back(*reinterpret_cast<const uint64_t*>(ptr)); ptr += sizeof(uint64_t);
        }
        while (ptr + sizeof(uint64_t) <= end) {
            data.destinationNeuron.push_back(*reinterpret_cast<const uint64_t*>(ptr)); ptr += sizeof(uint64_t);
        }
        while (ptr + sizeof(float) <= end) {
            data.weight.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        while (ptr + sizeof(uint32_t) <= end) {
            data.delay.push_back(*reinterpret_cast<const uint32_t*>(ptr)); ptr += sizeof(uint32_t);
        }
        while (ptr + sizeof(uint8_t) <= end) {
            data.synapseType.push_back(*reinterpret_cast<const uint8_t*>(ptr)); ptr += sizeof(uint8_t);
        }
        while (ptr + sizeof(uint8_t) <= end) {
            data.plasticityFlags.push_back(*reinterpret_cast<const uint8_t*>(ptr)); ptr += sizeof(uint8_t);
        }
        while (ptr + sizeof(float) <= end) {
            data.eligibilityTrace.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        while (ptr + sizeof(float) <= end) {
            data.efficacy.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        while (ptr + sizeof(float) <= end) {
            data.shortTermDepression.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        while (ptr + sizeof(float) <= end) {
            data.shortTermFacilitation.push_back(*reinterpret_cast<const float*>(ptr)); ptr += sizeof(float);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading synapses: ") + e.what();
        return false;
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> data;
    
    // Find section
    SectionHeader sectionHeader;
    bool found = false;
    size_t sectionIndex = 0;
    
    for (size_t i = 0; i < sections_.size(); ++i) {
        if (sections_[i].type == static_cast<uint32_t>(sectionType)) {
            sectionHeader = sections_[i];
            sectionIndex = i;
            found = true;
            break;
        }
    }
    
    if (!found) {
        return data;
    }
    
    try {
        stream_.seekg(sectionHeader.offset);
        data.resize(sectionHeader.size);
        stream_.read(reinterpret_cast<char*>(data.data()), data.size());
        
        if (sectionHeader.compressedSize > 0 && sectionHeader.compressedSize != sectionHeader.size) {
            // Decompress (simplified - would use zlib in real implementation)
            data.resize(sectionHeader.compressedSize);
        }
        
    } catch (...) {
        data.clear();
    }
    
    return data;
}

bool CheckpointReader::validate() const {
    return header_.validate() && !error_.empty();
}

double CheckpointReader::getProgress() const {
    if (header_.totalSize == 0) return 1.0;
    return static_cast<double>(streamPos_) / header_.totalSize;
}

CheckpointWriter::CheckpointWriter() : bytesWritten_(0), compression_(CompressionLevel::Balanced) {}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    close();
    
    try {
        filepath_ = filepath;
        compression_ = compression;
        
        // Ensure directory exists
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty()) {
            std::filesystem::create_directories(dir);
        }
        
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        bytesWritten_ = 0;
        sections_.clear();
        
        // Write placeholder header
        writeHeader();
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        close();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // Finalize by writing footer and updating header
        finalize();
        stream_.close();
    }
    filepath_.clear();
    sections_.clear();
    compressBuffer_.clear();
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
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    return writeSection(CheckpointSection::Neurons, data);
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    return writeSection(CheckpointSection::Synapses, data);
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!isOpen()) return false;
    
    try {
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = sizeof(CheckpointHeader) + sizeof(SectionHeader) * sections_.size() +
                             std::accumulate(sections_.begin(), sections_.end(), 0,
                                           [](size_t sum, const SectionHeader& h) { return sum + h.size + h.compressedSize; });
        
        // Simple compression (placeholder - would use zlib)
        std::vector<uint8_t> compressedData;
        uint64_t compressedSize = size;
        
        if (compression_ != CompressionLevel::None) {
            compressedData.resize(size * 2);  // Simplified compression
            // In real implementation, use zlib or similar
            std::copy(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + size, compressedData.begin());
            compressedSize = compressedData.size();
        }
        
        sectionHeader.size = size;
        sectionHeader.compressedSize = compressedSize;
        sectionHeader.checksum = 0;  // Would compute checksum here
        
        // Write section header
        stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(SectionHeader));
        bytesWritten_ += sizeof(SectionHeader);
        
        // Write data
        const uint8_t* writeData = compression_ != CompressionLevel::None ? compressedData.data() : static_cast<const uint8_t*>(data);
        stream_.write(reinterpret_cast<const char*>(writeData), compressedSize);
        bytesWritten_ += compressedSize;
        
        sections_.push_back(sectionHeader);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing section: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const std::vector<uint8_t>& data) {
    return writeSection(sectionType, data.data(), data.size());
}

bool CheckpointWriter::finalize() {
    if (!isOpen()) return false;
    
    try {
        // Calculate total size
        header_.totalSize = sizeof(CheckpointHeader) + sizeof(SectionHeader) * sections_.size() +
                          std::accumulate(sections_.begin(), sections_.end(), 0,
                                        [](size_t sum, const SectionHeader& h) { return sum + h.size + h.compressedSize; });
        
        // Update header
        writeHeader();
        
        // Write end section
        SectionHeader endSection;
        endSection.type = static_cast<uint32_t>(CheckpointSection::End);
        endSection.size = 0;
        endSection.compressedSize = 0;
        endSection.checksum = 0;
        
        stream_.write(reinterpret_cast<const char*>(&endSection), sizeof(SectionHeader));
        bytesWritten_ += sizeof(SectionHeader);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception finalizing checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::abort() {
    close();
    if (!filepath_.empty()) {
        std::filesystem::remove(filepath_);
    }
}

bool CheckpointWriter::writeHeader() {
    // Compute checksum
    header_.checksum = ChecksumCalculator::crc64(nullptr, 0);  // Simplified
    
    stream_.seekp(0);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    bytesWritten_ = sizeof(CheckpointHeader);
    
    return true;
}

CheckpointManager::CheckpointManager() : saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true),
    lastSaveStep_(0), checkpointDir_("."), lastCheckpointPath_("") {}

CheckpointManager::~CheckpointManager() {}

void CheckpointManager::configure(const std::string& checkpointDir, uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints, bool compress) {
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
    std::string checkpointName = name.empty() ? "checkpoint_" + std::to_string(lastSaveStep_) : name;
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".nclm";
    
    // Call provider functions to get data
    NeuronCheckpointData neuronData;
    SynapseCheckpointData synapseData;
    
    if (neuronProvider_ && !neuronProvider_(neuronData)) return false;
    if (synapseProvider_ && !synapseProvider_(synapseData)) return false;
    
    // Create checkpoint writer
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        return false;
    }
    
    // Write metadata
    writer.setMetadata(neuronData.membranePotential.size(), synapseData.weight.size(),
                      0, lastSaveStep_, currentTime);  // Region count would be passed from brain
    
    // Write data
    if (!writer.writeNeurons(neuronData) || !writer.writeSynapses(synapseData)) {
        writer.abort();
        return false;
    }
    
    if (!writer.finalize()) {
        writer.abort();
        return false;
    }
    
    lastSaveStep_ = currentStep;
    lastCheckpointPath_ = filepath;
    
    // Cleanup old checkpoints
    pruneOldCheckpoints(maxCheckpoints_);
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".nclm";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    // Validate checkpoint
    if (!reader.validate()) {
        return false;
    }
    
    // Read data (providers would be called here)
    NeuronCheckpointData neuronData;
    if (!reader.readNeurons(neuronData)) {
        return false;
    }
    
    SynapseCheckpointData synapseData;
    if (!reader.readSynapses(synapseData)) {
        return false;
    }
    
    // Call load providers
    // In real implementation, this would restore state to brain
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.path().extension() == ".nclm") {
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                info.fileSize = entry.file_size();
                info.timestamp = std::chrono::system_clock::to_time_t(
                    std::chrono::file_clock::to_sys(entry.last_write_time()));
                
                // Parse from header in real implementation
                info.step = 0;
                info.simulationTime = 0.0;
                info.neuronCount = 0;
                info.synapseCount = 0;
                
                checkpoints.push_back(info);
            }
        }
        
        // Sort by timestamp
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      return a.timestamp > b.timestamp;
                  });
                  
    } catch (...) {
        // Handle filesystem errors
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) return;
    
    // Keep most recent N checkpoints
    std::vector<std::string> toDelete;
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        toDelete.push_back(checkpoints[i].name);
    }
    
    for (const auto& name : toDelete) {
        std::filesystem::path filepath = checkpointDir_ + "/" + name + ".nclm";
        try {
            std::filesystem::remove(filepath);
        } catch (...) {
            // Handle delete errors
        }
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= saveIntervalSteps_ && (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

} // namespace nlm
