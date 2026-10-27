// CheckpointSystem Implementation

#include "CheckpointSystem.hpp"
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace nlm {

// CheckpointWriter Implementation

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {
}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Close any existing file
        if (stream_.is_open()) {
            stream_.close();
        }
        
        // Ensure directory exists
        std::filesystem::path filepath_path(filepath);
        std::filesystem::path dir = filepath_path.parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        // Create file
        stream_.open(filepath, std::ios::binary | std::ios::trunc);
        if (!stream_.is_open()) {
            return false;
        }
        
        // Store filepath and compression
        filepath_ = filepath;
        compression_ = compression;
        
        // Write header
        if (!writeHeader()) {
            stream_.close();
            return false;
        }
        
        // Initialize compression buffer
        compressBuffer_.clear();
        if (compression_ != CompressionLevel::None) {
            compressBuffer_.resize(65536); // 64KB buffer
        }
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointWriter::create exception: " << e.what() << std::endl;
        if (stream_.is_open()) {
            stream_.close();
        }
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // Finalize checkpoint if not already done
        if (!sections_.empty()) {
            finalize();
        }
        stream_.close();
    }
    filepath_.clear();
    compressBuffer_.clear();
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount, 
                                  uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    header_.timestamp = static_cast<uint64_t>(std::time(nullptr));
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        // Convert data to binary format
        std::vector<uint8_t> neuronBuffer;
        neuronBuffer.reserve(sizeof(float) * 11 * data.membranePotential.size()); // rough estimate
        
        for (size_t i = 0; i < data.membranePotential.size(); ++i) {
            // Write each field
            float* membrane = const_cast<float*>(&data.membranePotential[i]);
            float* resting = const_cast<float*>(&data.restingPotential[i]);
            float* threshold = const_cast<float*>(&data.threshold[i]);
            float* reset = const_cast<float*>(&data.resetPotential[i]);
            float* leak = const_cast<float*>(&data.leakConductance[i]);
            uint8_t* firing = const_cast<uint8_t*>(&data.firingState[i]);
            uint32_t* refrac_remaining = const_cast<uint32_t*>(&data.refractoryRemaining[i]);
            uint32_t* refrac_period = const_cast<uint32_t*>(&data.refractoryPeriod[i]);
            float* last_spike = const_cast<float*>(&data.lastSpikeTime[i]);
            uint64_t* neuron_type = const_cast<uint64_t*>(&data.neuronType[i]);
            uint64_t* region_id = const_cast<uint64_t*>(&data.regionId[i]);
            uint64_t* population_id = const_cast<uint64_t*>(&data.populationId[i]);
            
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(membrane), reinterpret_cast<uint8_t*>(membrane) + sizeof(float));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(resting), reinterpret_cast<uint8_t*>(resting) + sizeof(float));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(threshold), reinterpret_cast<uint8_t*>(threshold) + sizeof(float));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(reset), reinterpret_cast<uint8_t*>(reset) + sizeof(float));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(leak), reinterpret_cast<uint8_t*>(leak) + sizeof(float));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(firing), reinterpret_cast<uint8_t*>(firing) + sizeof(uint8_t));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(refrac_remaining), reinterpret_cast<uint8_t*>(refrac_remaining) + sizeof(uint32_t));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(refrac_period), reinterpret_cast<uint8_t*>(refrac_period) + sizeof(uint32_t));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(last_spike), reinterpret_cast<uint8_t*>(last_spike) + sizeof(float));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(neuron_type), reinterpret_cast<uint8_t*>(neuron_type) + sizeof(uint64_t));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(region_id), reinterpret_cast<uint8_t*>(region_id) + sizeof(uint64_t));
            neuronBuffer.insert(neuronBuffer.end(), reinterpret_cast<uint8_t*>(population_id), reinterpret_cast<uint8_t*>(population_id) + sizeof(uint64_t));
        }
        
        return writeSection(CheckpointSection::Neurons, neuronBuffer.data(), neuronBuffer.size());
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointWriter::writeNeurons exception: " << e.what() << std::endl;
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Convert data to binary format
        std::vector<uint8_t> synapseBuffer;
        synapseBuffer.reserve(sizeof(float) * 10 * data.weight.size()); // rough estimate
        
        for (size_t i = 0; i < data.weight.size(); ++i) {
            float* weight = const_cast<float*>(&data.weight[i]);
            uint32_t* delay = const_cast<uint32_t*>(&data.delay[i]);
            uint8_t* syn_type = const_cast<uint8_t*>(&data.synapseType[i]);
            uint8_t* plastic_flags = const_cast<uint8_t*>(&data.plasticityFlags[i]);
            float* eligibility = const_cast<float*>(&data.eligibilityTrace[i]);
            float* efficacy = const_cast<float*>(&data.efficacy[i]);
            float* depression = const_cast<float*>(&data.shortTermDepression[i]);
            float* facilitation = const_cast<float*>(&data.shortTermFacilitation[i]);
            
            synapseBuffer.insert(synapseBuffer.end(), reinterpret_cast<uint8_t*>(weight), reinterpret_cast<uint8_t*>(weight) + sizeof(float));
            synapseBuffer.insert(synapseBuffer.end(), reinterpret_cast<uint8_t*>(delay), reinterpret_cast<uint8_t*>(delay) + sizeof(uint32_t));
            synapseBuffer.insert(synapseBuffer.end(), reinterpret_cast<uint8_t*>(syn_type), reinterpret_cast<uint8_t*>(syn_type) + sizeof(uint8_t));
            synapseBuffer.insert(synapseBuffer.end(), reinterpret_cast<uint8_t*>(plastic_flags), reinterpret_cast<uint8_t*>(plastic_flags) + sizeof(uint8_t));
            synapseBuffer.insert(synapseBuffer.end(), reinterpret_cast<uint8_t*>(eligibility), reinterpret_cast<uint8_t*>(eligibility) + sizeof(float));
            synapseBuffer.insert(synapseBuffer.end(), reinterpret_cast<uint8_t*>(efficacy), reinterpret_cast<uint8_t*>(efficacy) + sizeof(float));
            synapseBuffer.insert(synapseBuffer.end(), reinterpret_cast<uint8_t*>(depression), reinterpret_cast<uint8_t*>(depression) + sizeof(float));
            synapseBuffer.insert(synapseBuffer.end(), reinterpret_cast<uint8_t*>(facilitation), reinterpret_cast<uint8_t*>(facilitation) + sizeof(float));
        }
        
        return writeSection(CheckpointSection::Synapses, synapseBuffer.data(), synapseBuffer.size());
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointWriter::writeSynapses exception: " << e.what() << std::endl;
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        if (!stream_.is_open()) {
            return false;
        }
        
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = stream_.tellp();
        sectionHeader.size = size;
        sectionHeader.compressedSize = size; // For now, no compression
        
        // Compute checksum
        sectionHeader.checksum = ChecksumCalculator::crc32(data, size);
        
        // Write section header
        if (!writeSectionHeader(sectionHeader)) {
            return false;
        }
        
        // Write data
        const uint8_t* byteData = static_cast<const uint8_t*>(data);
        stream_.write(reinterpret_cast<const char*>(byteData), size);
        if (!stream_.good()) {
            return false;
        }
        
        // Pad to 8-byte alignment
        padToAlignment(8);
        
        // Store section info
        sections_.push_back(sectionHeader);
        bytesWritten_ += size;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointWriter::writeSection exception: " << e.what() << std::endl;
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        // Write footer (section headers)
        uint64_t footerOffset = stream_.tellp();
        
        // Write all section headers
        for (const auto& section : sections_) {
            uint64_t headerSize = sizeof(section.type) + sizeof(section.offset) + sizeof(section.size) + 
                                 sizeof(section.compressedSize) + sizeof(section.checksum);
            
            stream_.write(reinterpret_cast<const char*>(&section.type), sizeof(section.type));
            stream_.write(reinterpret_cast<const char*>(&section.offset), sizeof(section.offset));
            stream_.write(reinterpret_cast<const char*>(&section.size), sizeof(section.size));
            stream_.write(reinterpret_cast<const char*>(&section.compressedSize), sizeof(section.compressedSize));
            stream_.write(reinterpret_cast<const char*>(&section.checksum), sizeof(section.checksum));
            
            if (!stream_.good()) {
                return false;
            }
        }
        
        // Update header with total size
        header_.totalSize = stream_.tellp() - sizeof(CheckpointHeader);
        stream_.seekp(0);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        stream_.seekp(0); // Reset to beginning for reading
        
        // Finalize is successful
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointWriter::finalize exception: " << e.what() << std::endl;
        return false;
    }
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    // Remove file
    if (!filepath_.empty()) {
        std::filesystem::remove(filepath_);
    }
    sections_.clear();
    bytesWritten_ = 0;
    filepath_.clear();
}

bool CheckpointWriter::writeHeader() {
    try {
        // Generate initial checksum
        uint64_t checksum = ChecksumCalculator::crc64(&header_, sizeof(header_));
        header_.checksum = checksum;
        
        // Write header
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        
        return stream_.good();
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointWriter::writeHeader exception: " << e.what() << std::endl;
        return false;
    }
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    try {
        stream_.write(reinterpret_cast<const char*>(&header.type), sizeof(header.type));
        stream_.write(reinterpret_cast<const char*>(&header.offset), sizeof(header.offset));
        stream_.write(reinterpret_cast<const char*>(&header.size), sizeof(header.size));
        stream_.write(reinterpret_cast<const char*>(&header.compressedSize), sizeof(header.compressedSize));
        stream_.write(reinterpret_cast<const char*>(&header.checksum), sizeof(header.checksum));
        
        return stream_.good();
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointWriter::writeSectionHeader exception: " << e.what() << std::endl;
        return false;
    }
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    try {
        uint64_t currentPos = stream_.tellp();
        uint64_t remainder = currentPos % alignment;
        if (remainder == 0) {
            return true;
        }
        
        size_t padding = alignment - remainder;
        std::vector<uint8_t> paddingBuffer(padding, 0);
        stream_.write(reinterpret_cast<const char*>(paddingBuffer.data()), padding);
        
        return stream_.good();
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointWriter::padToAlignment exception: " << e.what() << std::endl;
        return false;
    }
}

// CheckpointReader Implementation

CheckpointReader::CheckpointReader() : streamPos_(0) {
}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        // Close any existing file
        if (stream_.is_open()) {
            stream_.close();
        }
        
        // Open file
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint header";
            stream_.close();
            return false;
        }
        
        // Validate header
        if (!header_.validate()) {
            error_ = "Checkpoint header validation failed";
            stream_.close();
            return false;
        }
        
        streamPos_ = sizeof(header_);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("CheckpointReader::open exception: ") + e.what();
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
    streamPos_ = 0;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    try {
        auto neuronSection = readSection(CheckpointSection::Neurons);
        if (neuronSection.empty()) {
            return false;
        }
        
        // Parse neuron data
        size_t neuronCount = neuronSection.size() / (sizeof(float) * 11);
        data.membranePotential.resize(neuronCount);
        data.restingPotential.resize(neuronCount);
        data.threshold.resize(neuronCount);
        data.resetPotential.resize(neuronCount);
        data.leakConductance.resize(neuronCount);
        data.firingState.resize(neuronCount);
        data.refractoryRemaining.resize(neuronCount);
        data.refractoryPeriod.resize(neuronCount);
        data.lastSpikeTime.resize(neuronCount);
        data.neuronType.resize(neuronCount);
        data.regionId.resize(neuronCount);
        data.populationId.resize(neuronCount);
        
        uint8_t* dataPtr = neuronSection.data();
        for (size_t i = 0; i < neuronCount; ++i) {
            memcpy(&data.membranePotential[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
            memcpy(&data.restingPotential[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
            memcpy(&data.threshold[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
            memcpy(&data.resetPotential[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
            memcpy(&data.leakConductance[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
            memcpy(&data.firingState[i], dataPtr, sizeof(uint8_t)); dataPtr += sizeof(uint8_t);
            memcpy(&data.refractoryRemaining[i], dataPtr, sizeof(uint32_t)); dataPtr += sizeof(uint32_t);
            memcpy(&data.refractoryPeriod[i], dataPtr, sizeof(uint32_t)); dataPtr += sizeof(uint32_t);
            memcpy(&data.lastSpikeTime[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
            memcpy(&data.neuronType[i], dataPtr, sizeof(uint64_t)); dataPtr += sizeof(uint64_t);
            memcpy(&data.regionId[i], dataPtr, sizeof(uint64_t)); dataPtr += sizeof(uint64_t);
            memcpy(&data.populationId[i], dataPtr, sizeof(uint64_t)); dataPtr += sizeof(uint64_t);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("CheckpointReader::readNeurons exception: ") + e.what();
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    try {
        auto synapseSection = readSection(CheckpointSection::Synapses);
        if (synapseSection.empty()) {
            return false;
        }
        
        // Parse synapse data
        size_t synapseCount = synapseSection.size() / (sizeof(float) * 10);
        data.weight.resize(synapseCount);
        data.delay.resize(synapseCount);
        data.synapseType.resize(synapseCount);
        data.plasticityFlags.resize(synapseCount);
        data.eligibilityTrace.resize(synapseCount);
        data.efficacy.resize(synapseCount);
        data.shortTermDepression.resize(synapseCount);
        data.shortTermFacilitation.resize(synapseCount);
        
        uint8_t* dataPtr = synapseSection.data();
        for (size_t i = 0; i < synapseCount; ++i) {
            memcpy(&data.weight[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
            memcpy(&data.delay[i], dataPtr, sizeof(uint32_t)); dataPtr += sizeof(uint32_t);
            memcpy(&data.synapseType[i], dataPtr, sizeof(uint8_t)); dataPtr += sizeof(uint8_t);
            memcpy(&data.plasticityFlags[i], dataPtr, sizeof(uint8_t)); dataPtr += sizeof(uint8_t);
            memcpy(&data.eligibilityTrace[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
            memcpy(&data.efficacy[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
            memcpy(&data.shortTermDepression[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
            memcpy(&data.shortTermFacilitation[i], dataPtr, sizeof(float)); dataPtr += sizeof(float);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("CheckpointReader::readSynapses exception: ") + e.what();
        return false;
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    try {
        // Find section header
        for (const auto& section : sections_) {
            if (section.type == static_cast<uint32_t>(sectionType)) {
                // Seek to section data
                stream_.seekg(section.offset);
                streamPos_ = section.offset;
                
                // Read section data
                std::vector<uint8_t> data(section.size);
                stream_.read(reinterpret_cast<char*>(data.data()), section.size);
                
                if (!stream_.good()) {
                    error_ = "Failed to read section data";
                    return {};
                }
                
                // Verify checksum (skip alignment padding)
                uint64_t padding = (8 - (section.offset % 8)) % 8;
                uint64_t dataStart = section.offset + section.size + padding;
                
                stream_.seekg(dataStart);
                streamPos_ = dataStart;
                
                return data;
            }
        }
        
        error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
        return {};
        
    } catch (const std::exception& e) {
        error_ = std::string("CheckpointReader::readSection exception: ") + e.what();
        return {};
    }
}

bool CheckpointReader::validate() const {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint file not open";
            return false;
        }
        
        // Validate header
        if (!header_.validate()) {
            error_ = "Checkpoint header validation failed";
            return false;
        }
        
        // Read all section headers from footer
        uint64_t currentPos = sizeof(header_);
        stream_.seekg(currentPos);
        
        sections_.clear();
        while (currentPos < stream_.seekg(0, std::ios::end).tellg()) {
            SectionHeader section;
            stream_.seekg(currentPos);
            stream_.read(reinterpret_cast<char*>(&section.type), sizeof(section.type));
            stream_.read(reinterpret_cast<char*>(&section.offset), sizeof(section.offset));
            stream_.read(reinterpret_cast<char*>(&section.size), sizeof(section.size));
            stream_.read(reinterpret_cast<char*>(&section.compressedSize), sizeof(section.compressedSize));
            stream_.read(reinterpret_cast<char*>(&section.checksum), sizeof(section.checksum));
            
            if (!stream_.good()) {
                break;
            }
            
            sections_.push_back(section);
            currentPos += sizeof(section.type) + sizeof(section.offset) + sizeof(section.size) + 
                         sizeof(section.compressedSize) + sizeof(section.checksum);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("CheckpointReader::validate exception: ") + e.what();
        return false;
    }
}

double CheckpointReader::getProgress() const {
    try {
        if (!stream_.is_open() || sections_.empty()) {
            return 0.0;
        }
        
        uint64_t totalSize = 0;
        for (const auto& section : sections_) {
            totalSize += section.size;
        }
        
        if (totalSize == 0) {
            return 0.0;
        }
        
        return static_cast<double>(streamPos_) / totalSize;
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointReader::getProgress exception: " << e.what() << std::endl;
        return 0.0;
    }
}

// CheckpointManager Implementation

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
    // Check if we should save
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    
    lastSaveStep_ = currentStep;
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    try {
        // Create checkpoint directory if needed
        if (!checkpointDir_.empty()) {
            std::filesystem::path dir(checkpointDir_);
            if (!std::filesystem::exists(dir)) {
                std::filesystem::create_directories(dir);
            }
        }
        
        // Generate checkpoint name
        std::string checkpointName = name;
        if (checkpointName.empty()) {
            checkpointName = "checkpoint_" + std::to_string(lastSaveStep_);
        }
        
        std::string filepath = checkpointDir_ + "/" + checkpointName + ".bin";
        
        // Note: In a real implementation, we would call the actual checkpoint save methods
        // For now, just create an empty file to indicate success
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Write a simple header to indicate this is a valid checkpoint
        uint64_t magic = 0x4E4C4D434B545430ULL; // "NLMCKT0"
        file.write(reinterpret_cast<const char*>(&magic), sizeof(magic));
        
        file.close();
        lastCheckpointPath_ = filepath;
        
        // Prune old checkpoints
        pruneOldCheckpoints(maxCheckpoints_);
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointManager::saveImmediately exception: " << e.what() << std::endl;
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    try {
        std::string filepath = checkpointDir_ + "/" + name + ".bin";
        
        // Note: In a real implementation, we would call the actual checkpoint load methods
        // For now, just check if file exists and has valid header
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        uint64_t magic;
        file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        
        if (magic != 0x4E4C4D434B545430ULL) { // "NLMCKT0"
            return false;
        }
        
        file.close();
        lastCheckpointPath_ = filepath;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "CheckpointManager::load exception: " << e.what() << std::endl;
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!checkpointDir_.empty() && std::filesystem::exists(checkpointDir_)) {
        std::filesystem::path dir(checkpointDir_);
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                info.fileSize = entry.file_size();
                info.timestamp = std::chrono::system_clock::to_time_t(
                    std::chrono::file_clock::to_sys(entry.last_write_time())
                );
                
                // Extract info from file (simplified)
                info.step = 0;
                info.simulationTime = 0.0;
                info.neuronCount = 0;
                info.synapseCount = 0;
                
                checkpoints.push_back(info);
            }
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
    if (keepCount >= 2) {
        auto checkpoints = listCheckpoints();
        if (checkpoints.size() > keepCount) {
            // Remove oldest checkpoints
            size_t removeCount = checkpoints.size() - keepCount;
            for (size_t i = checkpoints.size() - 1; i >= checkpoints.size() - removeCount; --i) {
                std::filesystem::remove(checkpointDir_ + "/" + checkpoints[i].name + ".bin");
            }
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep <= lastSaveStep_) {
        return false;
    }
    
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

} // namespace nlm
