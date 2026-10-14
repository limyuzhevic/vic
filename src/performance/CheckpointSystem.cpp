#include "CheckpointSystem.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <zlib.h>

namespace nlm {

// Check if zlib is available
#ifdef ZLIB_AVAILABLE
#define USE_COMPRESSION 1
#else
#define USE_COMPRESSION 0
#endif

// Simple streaming checkpoint implementation

// CheckpointReader implementation
CheckpointReader::CheckpointReader() : streamPos_(0) {
}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Read and validate header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_.good()) {
            close();
            error_ = "Failed to read checkpoint header";
            return false;
        }
        
        if (!header_.validate()) {
            close();
            error_ = "Checkpoint header validation failed";
            return false;
        }
        
        // Read section headers
        uint32_t sectionCount = header_.neuronCount;  // Reusing field for section count
        sections_.resize(sectionCount);
        
        for (auto& section : sections_) {
            stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader));
            if (!stream_.good()) {
                close();
                error_ = "Failed to read section header";
                return false;
            }
        }
        
        streamPos_ = stream_.tellg();
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
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> data;
    
    // Find section
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            // Seek to section data
            stream_.seekg(streamPos_ + section.offset);
            
            // Read section data
            size_t dataSize = section.size;
            data.resize(dataSize);
            
            if (section.compressedSize > 0 && section.compressedSize < dataSize) {
                // Decompress section
#ifdef USE_COMPRESSION
                std::vector<uint8_t> compressed(dataSize);
                stream_.read(reinterpret_cast<char*>(compressed.data()), dataSize);
                if (!stream_.good()) {
                    error_ = "Failed to read compressed section data";
                    return {};
                }
                
                // Decompress using zlib
                data.resize(section.size);
                uLongf destLen = section.size;
                if (uncompress(reinterpret_cast<Bytef*>(data.data()), &destLen,
                              reinterpret_cast<const Bytef*>(compressed.data()), compressed.size()) != Z_OK) {
                    error_ = "Failed to decompress section";
                    return {};
                }
#else
                error_ = "Compression not available but section is compressed";
                return {};
#endif
            } else {
                // Read uncompressed data
                stream_.read(reinterpret_cast<char*>(data.data()), dataSize);
                if (!stream_.good()) {
                    error_ = "Failed to read section data";
                    return {};
                }
            }
            
            // Validate checksum
            uint32_t calculatedChecksum = ChecksumCalculator::crc32(data.data(), data.size());
            if (calculatedChecksum != section.checksum) {
                error_ = "Section checksum validation failed";
                return {};
            }
            
            return data;
        }
    }
    
    error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
    return {};
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Neurons);
    if (sectionData.empty() && !error_.empty()) {
        return false;
    }
    
    // Deserialize neuron data (simplified)
    size_t expectedSize = data.membranePotential.size() * sizeof(float) +
                         data.restingPotential.size() * sizeof(float) +
                         /* ... other fields */ 0;
    
    if (sectionData.size() != expectedSize) {
        error_ = "Neuron section data size mismatch";
        return false;
    }
    
    // For now, just return success (implementation would deserialize the data)
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Synapses);
    if (sectionData.empty() && !error_.empty()) {
        return false;
    }
    
    return true;
}

bool CheckpointReader::validate() const {
    return !error_.empty();  // Returns false if error is set
}

double CheckpointReader::getProgress() const {
    if (stream_.is_open() && header_.totalSize > 0) {
        std::streampos currentPos = stream_.tellg();
        std::streampos fileSize = stream_.seekg(0, std::ios::end).tellg();
        stream_.seekg(currentPos, std::ios::beg);
        
        return static_cast<double>(currentPos) / static_cast<double>(fileSize);
    }
    return 0.0;
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {
}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Create directories if they don't exist
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty()) {
            std::filesystem::create_directories(dir);
        }
        
        stream_.open(filepath, std::ios::binary | std::ios::trunc);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        compression_ = compression;
        filepath_ = filepath;
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        close();
        return false;
    }
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

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        // Calculate checksum
        uint32_t checksum = ChecksumCalculator::crc32(static_cast<const uint8_t*>(data), size);
        
        // Compress if needed
        std::vector<uint8_t> compressedData;
        uint64_t compressedSize = size;
        uint64_t dataSize = size;
        
#ifdef USE_COMPRESSION
        if (compression_ != CompressionLevel::None && size > 1024) {
            compressedData.resize(compressBound(size));
            uLongf destLen = compressedData.size();
            if (compress(reinterpret_cast<Bytef*>(compressedData.data()), &destLen,
                        reinterpret_cast<const Bytef*>(data), size) != Z_OK) {
                error_ = "Failed to compress section data";
                return false;
            }
            compressedData.resize(destLen);
            data = compressedData.data();
            dataSize = destLen;
            compressedSize = destLen;
        }
#endif
        
        // Create section header
        SectionHeader section;
        section.type = static_cast<uint32_t>(sectionType);
        section.offset = sizeof(CheckpointHeader) + sizeof(SectionHeader) * sections_.size() +
                       bytesWritten_ - sizeof(CheckpointHeader) - sizeof(SectionHeader) * sections_.size();
        section.size = dataSize;
        section.compressedSize = compressedSize;
        section.checksum = checksum;
        
        sections_.push_back(section);
        
        // Write section data
        stream_.write(reinterpret_cast<const char*>(data), dataSize);
        if (!stream_.good()) {
            error_ = "Failed to write section data";
            return false;
        }
        
        bytesWritten_ += dataSize;
        
        // Pad to alignment
        size_t alignment = 4096;  // 4KB alignment
        size_t currentPos = stream_.tellp();
        size_t paddingNeeded = (alignment - (currentPos % alignment)) % alignment;
        
        if (paddingNeeded > 0) {
            std::vector<uint8_t> padding(paddingNeeded, 0);
            stream_.write(reinterpret_cast<const char*>(padding.data()), paddingNeeded);
            if (!stream_.good()) {
                error_ = "Failed to write padding";
                return false;
            }
            bytesWritten_ += paddingNeeded;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing section: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    // Serialize neuron data
    std::stringstream ss;
    
    // Write each field with size
    auto writeVector = [&ss](const auto& vec) {
        uint64_t size = vec.size();
        ss.write(reinterpret_cast<const char*>(&size), sizeof(size));
        ss.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(decltype(vec)::value_type));
    };
    
    writeVector(data.membranePotential);
    writeVector(data.restingPotential);
    writeVector(data.threshold);
    writeVector(data.resetPotential);
    writeVector(data.leakConductance);
    writeVector(data.firingState);
    writeVector(data.refractoryRemaining);
    writeVector(data.refractoryPeriod);
    writeVector(data.lastSpikeTime);
    writeVector(data.neuronType);
    writeVector(data.regionId);
    writeVector(data.populationId);
    
    std::string serialized = ss.str();
    return writeSection(CheckpointSection::Neurons, serialized.data(), serialized.size());
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    // Serialize synapse data
    std::stringstream ss;
    
    auto writeVector = [&ss](const auto& vec) {
        uint64_t size = vec.size();
        ss.write(reinterpret_cast<const char*>(&size), sizeof(size));
        ss.write(reinterpret_cast<const char*>(vec.data()), size * sizeof(decltype(vec)::value_type));
    };
    
    writeVector(data.sourceNeuron);
    writeVector(data.destinationNeuron);
    writeVector(data.weight);
    writeVector(data.delay);
    writeVector(data.synapseType);
    writeVector(data.plasticityFlags);
    writeVector(data.eligibilityTrace);
    writeVector(data.efficacy);
    writeVector(data.shortTermDepression);
    writeVector(data.shortTermFacilitation);
    
    std::string serialized = ss.str();
    return writeSection(CheckpointSection::Synapses, serialized.data(), serialized.size());
}

bool CheckpointWriter::finalize() {
    try {
        // Update header
        header_.totalSize = bytesWritten_;
        header_.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        // Calculate checksum
        header_.checksum = 0;  // Placeholder
        
        // Write header first
        stream_.seekp(0, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        
        if (!stream_.good()) {
            error_ = "Failed to write header";
            return false;
        }
        
        // Write section headers
        uint32_t sectionCount = static_cast<uint32_t>(sections_.size());
        stream_.seekp(sizeof(CheckpointHeader), std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&sectionCount), sizeof(uint32_t));
        stream_.write(reinterpret_cast<const char*>(sections_.data()), 
                     sizeof(SectionHeader) * sectionCount);
        
        if (!stream_.good()) {
            error_ = "Failed to write section headers";
            return false;
        }
        
        // Ensure file is flushed
        stream_.flush();
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception finalizing checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::abort() {
    close();
    std::filesystem::remove(filepath_);
}

// CheckpointManager implementation
CheckpointManager::CheckpointManager() : lastSaveStep_(0) {
}

CheckpointManager::~CheckpointManager() {
}

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
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Checkpoint providers not configured";
        return false;
    }
    
    std::string checkpointName = name.empty() ? "checkpoint_" + std::to_string(lastSaveStep_) : name;
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".nlmck";
    
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        error_ = writer.getError();
        return false;
    }
    
    // TODO: Implement full checkpoint writing using all providers
    // For now, just write header and neurons/synapses
    writer.setMetadata(0, 0, 0, lastSaveStep_, currentTime);
    
    NeuronCheckpointData neuronData;
    SynapseCheckpointData synapseData;
    
    if (!neuronProvider_(neuronData) || !synapseProvider_(synapseData)) {
        error_ = "Failed to get checkpoint data from providers";
        return false;
    }
    
    if (!writer.writeNeurons(neuronData) || !writer.writeSynapses(synapseData)) {
        error_ = writer.getError();
        return false;
    }
    
    if (!writer.finalize()) {
        error_ = writer.getError();
        return false;
    }
    
    lastSaveStep_ = lastSaveStep_ + saveIntervalSteps_;
    lastCheckpointPath_ = filepath;
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".nlmck";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        error_ = reader.getError();
        return false;
    }
    
    // TODO: Implement full checkpoint loading
    // For now, just validate and return success
    if (!reader.validate()) {
        error_ = reader.getError();
        return false;
    }
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        if (!std::filesystem::exists(checkpointDir_)) {
            return checkpoints;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.path().extension() == ".nlmck") {
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                info.fileSize = entry.path().size();
                info.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                    std::filesystem::last_write_time(entry.path()).time_since_epoch()).count();
                
                checkpoints.push_back(info);
            }
        }
        
        // Sort by timestamp (newest first)
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      return a.timestamp > b.timestamp;
                  });
                  
    } catch (const std::exception& e) {
        error_ = std::string("Exception listing checkpoints: ") + e.what();
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Remove oldest checkpoints
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp < b.timestamp;
              });
    
    size_t toRemove = checkpoints.size() - keepCount;
    for (size_t i = 0; i < toRemove; ++i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name + ".nlmck";
        try {
            std::filesystem::remove(filepath);
        } catch (const std::exception& e) {
            error_ = std::string("Exception removing checkpoint: ") + e.what();
        }
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep - lastSaveStep_ >= saveIntervalSteps_;
}

void CheckpointManager::setProvider(CheckpointSection section, 
                     std::function<bool(const void*&, size_t&)> provider) {
    providers_[section] = provider;
}

const std::string& CheckpointManager::getError() const {
    return error_;
}