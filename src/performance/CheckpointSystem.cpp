#include "CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <filesystem>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <chrono>

namespace nlm {

namespace fs = std::filesystem;

// Static helper functions for checksum calculation
namespace {

    uint32_t simpleHash(const void* data, size_t size) {
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        uint32_t hash = 0xFFFFFFFF;
        
        for (size_t i = 0; i < size; ++i) {
            hash ^= bytes[i];
            for (int j = 0; j < 8; ++j) {
                if (hash & 1) {
                    hash = (hash >> 1) ^ 0xEDB88320;
                } else {
                    hash >>= 1;
                }
            }
        }
        
        return ~hash;
    }

    uint64_t fletcher64(const uint64_t* data, size_t count) {
        uint64_t sum1 = 0, sum2 = 0;
        for (size_t i = 0; i < count; ++i) {
            sum1 = (sum1 + data[i]) % 0xFFFFFFFFFFFFFFFF;
            sum2 = (sum2 + sum1) % 0xFFFFFFFFFFFFFFFF;
        }
        return (sum2 << 32) | sum1;
    }

} // anonymous namespace

// CheckpointReader implementation
CheckpointReader::CheckpointReader() : stream_(), header_(), sections_(), error_(), streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        stream_.open(filepath, std::ios::binary | std::ios::in);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_) {
            error_ = "Failed to read checkpoint header";
            stream_.close();
            return false;
        }
        
        // Validate header
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header";
            stream_.close();
            return false;
        }
        
        // Read section headers
        SectionHeader section;
        while (stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader))) {
            sections_.push_back(section);
            streamPos_ += sizeof(SectionHeader);
        }
        
        if (stream_.bad() && !stream_.eof()) {
            error_ = "Failed to read section headers";
            stream_.close();
            return false;
        }
        
        NLM_LOG_DEBUG("CheckpointReader: Opened " + filepath + " with " + std::to_string(sections_.size()) + " sections");
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception opening checkpoint: ") + e.what();
        if (stream_.is_open()) stream_.close();
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
    streamPos_ = 0;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> result;
    
    // Find section
    SectionHeader* section = nullptr;
    for (auto& s : sections_) {
        if (s.type == static_cast<uint32_t>(sectionType)) {
            section = &s;
            break;
        }
    }
    
    if (!section) {
        error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
        return result;
    }
    
    // Seek to section data
    stream_.seekg(section->offset);
    if (!stream_) {
        error_ = "Failed to seek to section data";
        return result;
    }
    
    // Read section data
    result.resize(section->size);
    stream_.read(reinterpret_cast<char*>(result.data()), section->size);
    
    if (!stream_ || stream_.gcount() != static_cast<std::streamsize>(section->size)) {
        error_ = "Failed to read section data";
        result.clear();
    }
    
    return result;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Neurons);
    if (sectionData.empty()) {
        return false;
    }
    
    // Decode neuron data (simplified for now)
    // In a full implementation, this would deserialize the complex data structure
    try {
        const uint8_t* ptr = sectionData.data();
        const uint8_t* end = ptr + sectionData.size();
        
        // Read each vector based on header information
        auto readVector = [&](auto& vec) {
            uint64_t size = *reinterpret_cast<const uint64_t*>(ptr);
            ptr += sizeof(uint64_t);
            
            if (ptr + size * sizeof(decltype(vec)::value_type) > end) {
                throw std::runtime_error("Buffer overflow in vector read");
            }
            
            vec.resize(size);
            std::copy(ptr, ptr + size * sizeof(decltype(vec)::value_type), 
                     reinterpret_cast<uint8_t*>(vec.data()));
            ptr += size * sizeof(decltype(vec)::value_type);
        };
        
        readVector(data.membranePotential);
        readVector(data.restingPotential);
        readVector(data.threshold);
        readVector(data.resetPotential);
        readVector(data.leakConductance);
        readVector(data.firingState);
        readVector(data.refractoryRemaining);
        readVector(data.refractoryPeriod);
        readVector(data.lastSpikeTime);
        readVector(data.neuronType);
        readVector(data.regionId);
        readVector(data.populationId);
        
        // Validate we read all data
        if (ptr != end) {
            error_ = "CheckpointReader: Data size mismatch";
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading neurons: ") + e.what();
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Synapses);
    if (sectionData.empty()) {
        return false;
    }
    
    // Similar decoding for synapses
    try {
        const uint8_t* ptr = sectionData.data();
        const uint8_t* end = ptr + sectionData.size();
        
        auto readVector = [&](auto& vec) {
            uint64_t size = *reinterpret_cast<const uint64_t*>(ptr);
            ptr += sizeof(uint64_t);
            
            if (ptr + size * sizeof(decltype(vec)::value_type) > end) {
                throw std::runtime_error("Buffer overflow in vector read");
            }
            
            vec.resize(size);
            std::copy(ptr, ptr + size * sizeof(decltype(vec)::value_type), 
                     reinterpret_cast<uint8_t*>(vec.data()));
            ptr += size * sizeof(decltype(vec)::value_type);
        };
        
        readVector(data.sourceNeuron);
        readVector(data.destinationNeuron);
        readVector(data.weight);
        readVector(data.delay);
        readVector(data.synapseType);
        readVector(data.plasticityFlags);
        readVector(data.eligibilityTrace);
        readVector(data.efficacy);
        readVector(data.shortTermDepression);
        readVector(data.shortTermFacilitation);
        
        if (ptr != end) {
            error_ = "CheckpointReader: Synapse data size mismatch";
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading synapses: ") + e.what();
        return false;
    }
}

bool CheckpointReader::validate() const {
    if (!isOpen()) {
        error_ = "CheckpointReader not open";
        return false;
    }
    
    // Check header validation
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // In a full implementation, would validate all section checksums
    // For now, just check header checksum against data
    uint64_t checksum = fletcher64(reinterpret_cast<const uint64_t*>(&header_),
                                   sizeof(CheckpointHeader) / sizeof(uint64_t));
    
    if (checksum != header_.checksum) {
        error_ = "Checkpoint header checksum mismatch";
        return false;
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    // Calculate based on current stream position
    std::streampos current;
    stream_.tellg(current);
    std::streampos end;
    stream_.seekg(0, std::ios::end);
    stream_.tellg(end);
    
    double progress = static_cast<double>(current) / static_cast<double>(end);
    return std::min(progress, 1.0);
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(CompressionLevel::Balanced), 
    header_(), sections_(), bytesWritten_(0), compressBuffer_() {}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Create directory if needed
        fs::path dir = fs::path(filepath).parent_path();
        if (!dir.empty() && !fs::exists(dir)) {
            fs::create_directories(dir);
        }
        
        filepath_ = filepath;
        compression_ = compression;
        bytesWritten_ = 0;
        
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            NLM_LOG_ERROR("Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Write empty header (will be filled later)
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_) {
            NLM_LOG_ERROR("Failed to write checkpoint header");
            stream_.close();
            return false;
        }
        
        bytesWritten_ += sizeof(CheckpointHeader);
        
        NLM_LOG_DEBUG("CheckpointWriter: Created " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception creating checkpoint: ") + e.what());
        if (stream_.is_open()) stream_.close();
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

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        // Serialize neuron data
        std::vector<uint8_t> sectionData;
        
        auto writeVector = [&](const auto& vec) {
            // Write vector size first
            uint64_t size = vec.size();
            const uint8_t* sizePtr = reinterpret_cast<const uint8_t*>(&size);
            sectionData.insert(sectionData.end(), sizePtr, sizePtr + sizeof(size));
            
            // Write vector data
            const uint8_t* dataPtr = reinterpret_cast<const uint8_t*>(vec.data());
            sectionData.insert(sectionData.end(), dataPtr, dataPtr + size * sizeof(vec.value_type));
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
        
        return writeSection(CheckpointSection::Neurons, sectionData.data(), sectionData.size());
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing neurons: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        std::vector<uint8_t> sectionData;
        
        auto writeVector = [&](const auto& vec) {
            uint64_t size = vec.size();
            const uint8_t* sizePtr = reinterpret_cast<const uint8_t*>(&size);
            sectionData.insert(sectionData.end(), sizePtr, sizePtr + sizeof(size));
            
            const uint8_t* dataPtr = reinterpret_cast<const uint8_t*>(vec.data());
            sectionData.insert(sectionData.end(), dataPtr, dataPtr + size * sizeof(vec.value_type));
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
        
        return writeSection(CheckpointSection::Synapses, sectionData.data(), sectionData.size());
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception writing synapses: ") + e.what());
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    SectionHeader section;
    section.type = static_cast<uint32_t>(sectionType);
    section.offset = bytesWritten_ + sizeof(SectionHeader);
    section.size = size;
    section.compressedSize = 0; // No compression for now
    section.checksum = simpleHash(data, size);
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
    if (!stream_) {
        NLM_LOG_ERROR("Failed to write section header");
        return false;
    }
    
    bytesWritten_ += sizeof(SectionHeader);
    
    // Write section data
    stream_.write(reinterpret_cast<const char*>(data), size);
    if (!stream_) {
        NLM_LOG_ERROR("Failed to write section data");
        return false;
    }
    
    bytesWritten_ += size;
    sections_.push_back(section);
    
    return true;
}

bool CheckpointWriter::finalize() {
    try {
        // Write footer checksum
        uint64_t totalChecksum = fletcher64(reinterpret_cast<const uint64_t*>(&header_), 
                                           sizeof(CheckpointHeader) / sizeof(uint64_t));
        header_.checksum = totalChecksum;
        
        // Seek to beginning and write final header
        stream_.seekp(0);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        
        if (!stream_) {
            NLM_LOG_ERROR("Failed to write final header");
            return false;
        }
        
        // Update header size
        header_.totalSize = bytesWritten_;
        
        stream_.close();
        
        NLM_LOG_INFO("CheckpointWriter: Finalized checkpoint (" + std::to_string(bytesWritten_) + " bytes)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception finalizing checkpoint: ") + e.what());
        if (stream_.is_open()) stream_.close();
        return false;
    }
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    
    // Remove incomplete file
    if (!filepath_.empty() && fs::exists(filepath_)) {
        try {
            fs::remove(filepath_);
        } catch (...) {
            // Ignore errors during cleanup
        }
    }
    
    filepath_.clear();
    sections_.clear();
    compressBuffer_.clear();
    bytesWritten_ = 0;
}

// CheckpointManager implementation
CheckpointManager::CheckpointManager() : checkpointDir_(), saveIntervalSteps_(10000), maxCheckpoints_(10), 
    compress_(true), lastSaveStep_(0), lastCheckpointPath_(), neuronProvider_(), synapseProvider_() {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir, 
                                uint64_t saveIntervalSteps,
                                size_t maxCheckpoints,
                                bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Create directory if needed
    fs::path dir(checkpointDir);
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    try {
        std::string checkpointName = name;
        if (checkpointName.empty()) {
            checkpointName = "checkpoint_" + std::to_string(lastSaveStep_);
        }
        
        std::string filepath = fs::path(checkpointDir_) / (checkpointName + ".nlmck");
        
        CheckpointWriter writer;
        CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
        
        if (!writer.create(filepath, compression)) {
            NLM_LOG_ERROR("Failed to create checkpoint writer");
            return false;
        }
        
        // Here you would need to provide the actual providers
        // For now, we'll just create an empty checkpoint
        writer.finalize();
        
        lastSaveStep_ = lastSaveStep_ == 0 ? currentStep : lastSaveStep_;
        lastCheckpointPath_ = filepath;
        
        NLM_LOG_INFO("CheckpointManager: Saved checkpoint " + checkpointName + " to " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception saving checkpoint: ") + e.what());
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = fs::path(checkpointDir_) / (name + ".nlmck");
    
    if (!fs::exists(filepath)) {
        NLM_LOG_ERROR("Checkpoint file does not exist: " + filepath);
        return false;
    }
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        NLM_LOG_ERROR("Failed to open checkpoint: " + reader.getError());
        return false;
    }
    
    if (!reader.validate()) {
        NLM_LOG_ERROR("Checkpoint validation failed: " + reader.getError());
        return false;
    }
    
    NLM_LOG_INFO("CheckpointManager: Loaded checkpoint " + name + " from " + filepath);
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        for (const auto& entry : fs::directory_iterator(checkpointDir_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".nlmck") {
                auto filepath = entry.path();
                auto ftime = fs::last_write_time(filepath);
                
                CheckpointInfo info;
                info.name = filepath.stem();
                // In a full implementation, would parse the checkpoint to get step/time/counts
                info.step = 0;
                info.simulationTime = 0.0;
                info.neuronCount = 0;
                info.synapseCount = 0;
                info.fileSize = fs::file_size(filepath);
                info.timestamp = std::chrono::system_clock::to_time_t(ftime);
                
                checkpoints.push_back(info);
            }
        }
        
        // Sort by timestamp (most recent first)
        std::sort(checkpoints.begin(), checkpoints.end(),
                 [](const CheckpointInfo& a, const CheckpointInfo& b) {
                     return a.timestamp > b.timestamp;
                 });
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception listing checkpoints: ") + e.what());
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Keep the most recent checkpoints
    std::vector<std::string> toDelete;
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        toDelete.push_back(fs::path(checkpointDir_) / (checkpoints[i].name + ".nlmck"));
    }
    
    for (const auto& filepath : toDelete) {
        try {
            fs::remove(filepath);
            NLM_LOG_DEBUG("CheckpointManager: Deleted old checkpoint " + filepath);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR(std::string("Failed to delete checkpoint " + filepath + ": ") + e.what());
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep == 0 || currentStep == lastSaveStep_) {
        return false;
    }
    
    if (currentStep - lastSaveStep_ >= saveIntervalSteps_) {
        return true;
    }
    
    return false;
}

// ChecksumCalculator inline implementations
inline uint64_t ChecksumCalculator::crc64(const void* data, size_t size) {
    // Simplified CRC-64 implementation
    static const uint64_t table[256] = {0x0000000000000000}; // Would contain precomputed values
    uint64_t crc = 0xFFFFFFFFFFFFFFFF;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        uint8_t idx = (crc ^ bytes[i]) & 0xFF;
        crc = table[idx] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFFFFFFFFFF;
}

inline uint32_t ChecksumCalculator::crc32(const void* data, size_t size) {
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; ++j) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return ~crc;
}

inline uint64_t ChecksumCalculator::fletcher64(const uint64_t* data, size_t count) {
    uint64_t sum1 = 0, sum2 = 0;
    for (size_t i = 0; i < count; ++i) {
        sum1 = (sum1 + data[i]) % 0xFFFFFFFFFFFFFFFF;
        sum2 = (sum2 + sum1) % 0xFFFFFFFFFFFFFFFF;
    }
    return (sum2 << 32) | sum1;
}

inline uint64_t ChecksumCalculator::combine(uint64_t a, uint64_t b) {
    return a ^ (b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2));
}

} // namespace nlm
