#include "CheckpointSystem.hpp"
#include <fstream>
#include <sstream>
#include <cstring>
#include <filesystem>
#include <zlib.h>

namespace nlm {

CheckpointReader::CheckpointReader() : streamPos_(0) {}
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
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint header";
            stream_.close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint magic or version";
            stream_.close();
            return false;
        }
        
        // Read section headers
        size_t numSections = header_.totalSize / sizeof(SectionHeader);
        sections_.resize(numSections);
        
        for (size_t i = 0; i < numSections; ++i) {
            stream_.read(reinterpret_cast<char*>(&sections_[i]), sizeof(SectionHeader));
            if (!stream_.good()) {
                error_ = "Failed to read section headers";
                stream_.close();
                return false;
            }
        }
        
        streamPos_ = stream_.tellg();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception opening checkpoint: ") + e.what();
        if (stream_.is_open()) {
            stream_.close();
        }
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Neurons);
    if (sectionData.empty()) {
        error_ = "Failed to read neurons section";
        return false;
    }
    
    // Deserialize neuron data
    const uint8_t* ptr = sectionData.data();
    const uint8_t* end = ptr + sectionData.size();
    
    auto readFloats = [&](std::vector<float>& vec, size_t count) {
        size_t bytesNeeded = count * sizeof(float);
        if (ptr + bytesNeeded > end) return false;
        std::copy(ptr, ptr + bytesNeeded, reinterpret_cast<uint8_t*>(vec.data()));
        ptr += bytesNeeded;
        return true;
    };
    
    auto readUints = [&](std::vector<uint64_t>& vec, size_t count) {
        size_t bytesNeeded = count * sizeof(uint64_t);
        if (ptr + bytesNeeded > end) return false;
        std::copy(ptr, ptr + bytesNeeded, reinterpret_cast<uint8_t*>(vec.data()));
        ptr += bytesNeeded;
        return true;
    };
    
    auto readUints8 = [&](std::vector<uint8_t>& vec, size_t count) {
        size_t bytesNeeded = count * sizeof(uint8_t);
        if (ptr + bytesNeeded > end) return false;
        std::copy(ptr, ptr + bytesNeeded, reinterpret_cast<uint8_t*>(vec.data()));
        ptr += bytesNeeded;
        return true;
    };
    
    data.membranePotential.resize(header_.neuronCount);
    data.restingPotential.resize(header_.neuronCount);
    data.threshold.resize(header_.neuronCount);
    data.resetPotential.resize(header_.neuronCount);
    data.leakConductance.resize(header_.neuronCount);
    data.firingState.resize(header_.neuronCount);
    data.refractoryRemaining.resize(header_.neuronCount);
    data.refractoryPeriod.resize(header_.neuronCount);
    data.lastSpikeTime.resize(header_.neuronCount);
    data.neuronType.resize(header_.neuronCount);
    data.regionId.resize(header_.neuronCount);
    data.populationId.resize(header_.neuronCount);
    
    if (!readFloats(data.membranePotential, header_.neuronCount)) return false;
    if (!readFloats(data.restingPotential, header_.neuronCount)) return false;
    if (!readFloats(data.threshold, header_.neuronCount)) return false;
    if (!readFloats(data.resetPotential, header_.neuronCount)) return false;
    if (!readFloats(data.leakConductance, header_.neuronCount)) return false;
    if (!readUints8(data.firingState, header_.neuronCount)) return false;
    if (!readUints(data.refractoryRemaining, header_.neuronCount)) return false;
    if (!readUints(data.refractoryPeriod, header_.neuronCount)) return false;
    if (!readFloats(data.lastSpikeTime, header_.neuronCount)) return false;
    if (!readUints(data.neuronType, header_.neuronCount)) return false;
    if (!readUints(data.regionId, header_.neuronCount)) return false;
    if (!readUints(data.populationId, header_.neuronCount)) return false;
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Synapses);
    if (sectionData.empty()) {
        error_ = "Failed to read synapses section";
        return false;
    }
    
    // Deserialize synapse data
    const uint8_t* ptr = sectionData.data();
    const uint8_t* end = ptr + sectionData.size();
    
    auto readFloats = [&](std::vector<float>& vec, size_t count) {
        size_t bytesNeeded = count * sizeof(float);
        if (ptr + bytesNeeded > end) return false;
        std::copy(ptr, ptr + bytesNeeded, reinterpret_cast<uint8_t*>(vec.data()));
        ptr += bytesNeeded;
        return true;
    };
    
    auto readUints = [&](std::vector<uint64_t>& vec, size_t count) {
        size_t bytesNeeded = count * sizeof(uint64_t);
        if (ptr + bytesNeeded > end) return false;
        std::copy(ptr, ptr + bytesNeeded, reinterpret_cast<uint8_t*>(vec.data()));
        ptr += bytesNeeded;
        return true;
    };
    
    auto readUints8 = [&](std::vector<uint8_t>& vec, size_t count) {
        size_t bytesNeeded = count * sizeof(uint8_t);
        if (ptr + bytesNeeded > end) return false;
        std::copy(ptr, ptr + bytesNeeded, reinterpret_cast<uint8_t*>(vec.data()));
        ptr += bytesNeeded;
        return true;
    };
    
    data.sourceNeuron.resize(header_.synapseCount);
    data.destinationNeuron.resize(header_.synapseCount);
    data.weight.resize(header_.synapseCount);
    data.delay.resize(header_.synapseCount);
    data.synapseType.resize(header_.synapseCount);
    data.plasticityFlags.resize(header_.synapseCount);
    data.eligibilityTrace.resize(header_.synapseCount);
    data.efficacy.resize(header_.synapseCount);
    data.shortTermDepression.resize(header_.synapseCount);
    data.shortTermFacilitation.resize(header_.synapseCount);
    
    if (!readUints(data.sourceNeuron, header_.synapseCount)) return false;
    if (!readUints(data.destinationNeuron, header_.synapseCount)) return false;
    if (!readFloats(data.weight, header_.synapseCount)) return false;
    if (!readUints(data.delay, header_.synapseCount)) return false;
    if (!readUints8(data.synapseType, header_.synapseCount)) return false;
    if (!readUints8(data.plasticityFlags, header_.synapseCount)) return false;
    if (!readFloats(data.eligibilityTrace, header_.synapseCount)) return false;
    if (!readFloats(data.efficacy, header_.synapseCount)) return false;
    if (!readFloats(data.shortTermDepression, header_.synapseCount)) return false;
    if (!readFloats(data.shortTermFacilitation, header_.synapseCount)) return false;
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            if (stream_.seekg(streamPos_ + section.offset).fail()) {
                error_ = "Failed to seek to section";
                return {};
            }
            
            std::vector<uint8_t> data(section.size);
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            if (!stream_.good()) {
                error_ = "Failed to read section data";
                return {};
            }
            
            // Decompress if needed
            if (section.compressedSize > 0 && section.compressedSize < section.size) {
                data.resize(section.compressedSize);
                // Decompress using zlib
                uLongf destLen = section.size;
                if (uncompress(reinterpret_cast<Bytef*>(data.data()), &destLen,
                              reinterpret_cast<const Bytef*>(data.data()), data.size()) != Z_OK) {
                    error_ = "Failed to decompress section data";
                    return {};
                }
                data.resize(destLen);
            }
            
            return data;
        }
    }
    
    error_ = "Section not found";
    return {};
}

bool CheckpointReader::validate() const {
    return isOpen() && header_.validate() && error_.empty();
}

double CheckpointReader::getProgress() const {
    if (!isOpen() || sections_.empty()) return 0.0;
    
    std::ifstream tempStream(filepath_, std::ios::binary);
    if (!tempStream.is_open()) return 0.0;
    
    double progress = static_cast<double>(stream_.tellg()) / static_cast<double>(header_.totalSize);
    return std::min(progress, 1.0);
}

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}
CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        close(); // Close any existing stream
        
        // Ensure directory exists
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            return false;
        }
        
        filepath_ = filepath;
        compression_ = compression;
        
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    filepath_.clear();
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
    // Convert neuron data to binary format
    std::vector<uint8_t> buffer;
    
    // Write neuron data in the correct order
    auto writeFloats = [&](const std::vector<float>& vec) {
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(vec.data()),
                     reinterpret_cast<const uint8_t*>(vec.data()) + vec.size() * sizeof(float));
    };
    
    auto writeUints = [&](const std::vector<uint64_t>& vec) {
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(vec.data()),
                     reinterpret_cast<const uint8_t*>(vec.data()) + vec.size() * sizeof(uint64_t));
    };
    
    auto writeUints8 = [&](const std::vector<uint8_t>& vec) {
        buffer.insert(buffer.end(), vec.begin(), vec.end());
    };
    
    writeFloats(data.membranePotential);
    writeFloats(data.restingPotential);
    writeFloats(data.threshold);
    writeFloats(data.resetPotential);
    writeFloats(data.leakConductance);
    writeUints8(data.firingState);
    writeUints(data.refractoryRemaining);
    writeUints(data.refractoryPeriod);
    writeFloats(data.lastSpikeTime);
    writeUints(data.neuronType);
    writeUints(data.regionId);
    writeUints(data.populationId);
    
    // Compress if needed
    std::vector<uint8_t> compressedData;
    bool compressed = false;
    
    if (compression_ != CompressionLevel::None && buffer.size() > 1024) {
        compressedData.resize(compressBound(buffer.size()));
        uLongf destLen = compressBound(buffer.size());
        
        if (compress(reinterpret_cast<Bytef*>(compressedData.data()), &destLen,
                     reinterpret_cast<const Bytef*>(buffer.data()), buffer.size()) == Z_OK) {
            if (destLen < buffer.size()) {
                buffer = std::move(compressedData);
                compressed = true;
            }
        }
    }
    
    // Write section
    return writeSection(CheckpointSection::Neurons, buffer.data(), buffer.size());
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    // Convert synapse data to binary format
    std::vector<uint8_t> buffer;
    
    auto writeFloats = [&](const std::vector<float>& vec) {
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(vec.data()),
                     reinterpret_cast<const uint8_t*>(vec.data()) + vec.size() * sizeof(float));
    };
    
    auto writeUints = [&](const std::vector<uint64_t>& vec) {
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(vec.data()),
                     reinterpret_cast<const uint8_t*>(vec.data()) + vec.size() * sizeof(uint64_t));
    };
    
    auto writeUints8 = [&](const std::vector<uint8_t>& vec) {
        buffer.insert(buffer.end(), vec.begin(), vec.end());
    };
    
    writeUints(data.sourceNeuron);
    writeUints(data.destinationNeuron);
    writeFloats(data.weight);
    writeUints(data.delay);
    writeUints8(data.synapseType);
    writeUints8(data.plasticityFlags);
    writeFloats(data.eligibilityTrace);
    writeFloats(data.efficacy);
    writeFloats(data.shortTermDepression);
    writeFloats(data.shortTermFacilitation);
    
    // Compress if needed
    std::vector<uint8_t> compressedData;
    bool compressed = false;
    
    if (compression_ != CompressionLevel::None && buffer.size() > 1024) {
        compressedData.resize(compressBound(buffer.size()));
        uLongf destLen = compressBound(buffer.size());
        
        if (compress(reinterpret_cast<Bytef*>(compressedData.data()), &destLen,
                     reinterpret_cast<const Bytef*>(buffer.data()), buffer.size()) == Z_OK) {
            if (destLen < buffer.size()) {
                buffer = std::move(compressedData);
                compressed = true;
            }
        }
    }
    
    // Write section
    return writeSection(CheckpointSection::Synapses, buffer.data(), buffer.size());
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    // Compress if needed
    std::vector<uint8_t> compressedBuffer;
    size_t actualSize = size;
    size_t compressedSize = 0;
    
    if (compression_ != CompressionLevel::None && size > 1024) {
        compressedBuffer.resize(compressBound(size));
        uLongf destLen = compressBound(size);
        
        if (compress(reinterpret_cast<Bytef*>(compressedBuffer.data()), &destLen,
                     reinterpret_cast<const Bytef*>(data), size) == Z_OK) {
            if (destLen < size) {
                compressedBuffer.resize(destLen);
                data = compressedBuffer.data();
                actualSize = destLen;
                compressedSize = destLen;
            }
        }
    }
    
    // Prepare section header
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(sectionType);
    sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
    sectionHeader.size = actualSize;
    sectionHeader.compressedSize = (compressedSize > 0 && compressedSize < actualSize) ? 
        compressedSize : 0;
    sectionHeader.checksum = ChecksumCalculator::crc32(static_cast<const uint8_t*>(data), actualSize);
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(SectionHeader));
    if (!stream_.good()) {
        return false;
    }
    
    // Write section data
    stream_.write(reinterpret_cast<const char*>(data), actualSize);
    if (!stream_.good()) {
        return false;
    }
    
    // Pad to 8-byte alignment
    size_t currentPos = stream_.tellp();
    if (currentPos % 8 != 0) {
        std::vector<uint8_t> padding(8 - (currentPos % 8), 0);
        stream_.write(reinterpret_cast<const char*>(padding.data()), padding.size());
    }
    
    sections_.push_back(sectionHeader);
    bytesWritten_ += sizeof(SectionHeader) + actualSize;
    
    return true;
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        return false;
    }
    
    // Calculate and write checksum
    uint64_t checksum = 0;
    for (const auto& section : sections_) {
        checksum = ChecksumCalculator::combine(checksum, section.checksum);
    }
    
    // Write footer with checksum
    stream_.write(reinterpret_cast<const char*>(&checksum), sizeof(checksum));
    
    // Update header with total size
    auto currentPos = stream_.tellp();
    stream_.seekp(0);
    
    header_.totalSize = sizeof(CheckpointHeader) + 
        sections_.size() * sizeof(SectionHeader) + currentPos - sizeof(CheckpointHeader);
    
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    
    // Write section headers
    for (const auto& section : sections_) {
        stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
    }
    
    stream_.close();
    return true;
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        std::filesystem::remove(filepath_);
    }
    close();
}

CheckpointManager::CheckpointManager() : lastSaveStep_(0) {}
CheckpointManager::~CheckpointManager() {
    if (stream_.is_open()) {
        stream_.close();
    }
}

void CheckpointManager::configure(const std::string& checkpointDir,
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Ensure checkpoint directory exists
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
    std::string checkpointName = name.empty() ? std::to_string(lastSaveStep_) : name;
    std::string filepath = checkpointDir_ + "/checkpoint_" + checkpointName + ".nckpt";
    
    // This is a simplified implementation - in reality, you'd need to gather all the checkpoint data
    // from the brain and its components
    
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        return false;
    }
    
    // Set metadata (placeholder - should get from brain)
    writer.setMetadata(1000, 10000, 10, lastSaveStep_, currentTime);
    
    // Write dummy data (placeholder - should get from brain)
    NeuronCheckpointData neurons;
    neurons.membranePotential.resize(1000, -70.0f);
    neurons.restingPotential.resize(1000, -70.0f);
    neurons.threshold.resize(1000, -55.0f);
    neurons.resetPotential.resize(1000, -70.0f);
    neurons.leakConductance.resize(1000, 10.0f);
    neurons.firingState.resize(1000, 0);
    neurons.refractoryRemaining.resize(1000, 0);
    neurons.refractoryPeriod.resize(1000, 5);
    neurons.lastSpikeTime.resize(1000, -1.0f);
    neurons.neuronType.resize(1000, 0);
    neurons.regionId.resize(1000, 0);
    neurons.populationId.resize(1000, 0);
    
    SynapseCheckpointData synapses;
    synapses.sourceNeuron.resize(10000);
    synapses.destinationNeuron.resize(10000);
    synapses.weight.resize(10000, 0.5f);
    synapses.delay.resize(10000, 1);
    synapses.synapseType.resize(10000, 0);
    synapses.plasticityFlags.resize(10000, 0);
    synapses.eligibilityTrace.resize(10000, 0.0f);
    synapses.efficacy.resize(10000, 1.0f);
    synapses.shortTermDepression.resize(10000, 0.0f);
    synapses.shortTermFacilitation.resize(10000, 0.0f);
    
    if (!writer.writeNeurons(neurons) || !writer.writeSynapses(synapses)) {
        writer.abort();
        return false;
    }
    
    if (!writer.finalize()) {
        writer.abort();
        return false;
    }
    
    lastSaveStep_ = currentStep;
    lastCheckpointPath_ = filepath;
    
    // Clean up old checkpoints
    pruneOldCheckpoints(maxCheckpoints_);
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/checkpoint_" + name + ".nckpt";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        return false;
    }
    
    // This is a simplified implementation - in reality, you'd need to load the data back into the brain
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        std::filesystem::path dir(checkpointDir_);
        if (!std::filesystem::exists(dir)) {
            return checkpoints;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".nckpt") {
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                info.step = std::stoull(info.name.substr(11)); // Remove "checkpoint_" prefix
                info.fileSize = std::filesystem::file_size(entry.path());
                info.timestamp = std::filesystem::last_write_time(entry.path()).time_since_epoch().count();
                
                checkpoints.push_back(info);
            }
        }
        
        // Sort by timestamp (newest first)
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      return a.timestamp > b.timestamp;
                  });
        
    } catch (const std::exception&) {
        // Ignore errors and return empty list
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() > keepCount) {
        // Remove oldest checkpoints
        for (size_t i = keepCount; i < checkpoints.size(); ++i) {
            std::filesystem::path filepath = checkpointDir_ + "/checkpoint_" + checkpoints[i].name + ".nckpt";
            std::filesystem::remove(filepath);
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= (lastSaveStep_ + saveIntervalSteps_);
}

} // namespace nlm

// Include the zlib headers inline
#include <zlib.h>

// Add missing includes for filesystem
#include <filesystem>
#include <chrono>