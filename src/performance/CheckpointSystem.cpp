/*
 * CheckpointSystem.cpp - Implementation of checkpoint save/load system
 * 
 * Provides robust checkpointing for neural simulation brain states.
 * Includes CheckpointReader, CheckpointWriter, and CheckpointManager classes
 * for saving and loading complete brain state.
 */

#include "src/performance/CheckpointSystem.hpp"
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <stdexcept>
#include <chrono>
#include <cstring>

namespace nlm {

namespace {
    // Simple byte swap for cross-platform compatibility
    inline uint64_t byteswap64(uint64_t value) {
        return ((value & 0x00000000000000FF) << 56) |
               ((value & 0x000000000000FF00) << 40) |
               ((value & 0x0000000000FF0000) << 24) |
               ((value & 0x00000000FF000000) << 8) |
               ((value & 0x000000FF00000000) >> 8) |
               ((value & 0x0000FF0000000000) >> 24) |
               ((value & 0x00FF000000000000) >> 40) |
               ((value & 0xFF00000000000000) >> 56);
    }
    
    // Convert CPU endian to file endian (little endian)
    inline uint64_t cpuToFileEndian(uint64_t value) {
        return byteswap64(value);
    }
    
    // Convert file endian to CPU endian
    inline uint64_t fileToCpuEndian(uint64_t value) {
        return byteswap64(value);
    }
    
    // Calculate file checksum
    uint64_t calculateFileChecksum(const void* data, size_t size) {
        return ChecksumCalculator::crc64(data, size);
    }
} // namespace

// CheckpointReader implementation

CheckpointReader::CheckpointReader() : stream_(), header_(), sections_(), error_(), streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        // Close any existing stream
        close();
        
        // Open file in binary mode
        stream_.open(filepath, std::ios::binary | std::ios::in);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Read header
        if (!stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_))) {
            error_ = "Failed to read checkpoint header";
            close();
            return false;
        }
        
        // Convert from file to CPU endian
        header_.magic = fileToCpuEndian(header_.magic);
        header_.majorVersion = fileToCpuEndian(header_.majorVersion);
        header_.minorVersion = fileToCpuEndian(header_.minorVersion);
        header_.patchVersion = fileToCpuEndian(header_.patchVersion);
        header_.timestamp = fileToCpuEndian(header_.timestamp);
        header_.totalSize = fileToCpuEndian(header_.totalSize);
        header_.neuronCount = fileToCpuEndian(header_.neuronCount);
        header_.synapseCount = fileToCpuEndian(header_.synapseCount);
        header_.regionCount = fileToCpuEndian(header_.regionCount);
        header_.step = fileToCpuEndian(header_.step);
        header_.simulationTime = fileToCpuEndian(header_.simulationTime);
        header_.checksum = fileToCpuEndian(header_.checksum);
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint file header (version mismatch)";
            close();
            return false;
        }
        
        // Read sections
        size_t sectionCount = header_.totalSize / sizeof(SectionHeader);
        sections_.resize(sectionCount);
        
        stream_.seekg(sizeof(header_));
        streamPos_ = sizeof(header_);
        
        if (!stream_.read(reinterpret_cast<char*>(sections_.data()), 
                         sectionCount * sizeof(SectionHeader))) {
            error_ = "Failed to read checkpoint sections";
            close();
            return false;
        }
        
        // Convert section headers from file to CPU endian
        for (auto& section : sections_) {
            section.offset = fileToCpuEndian(section.offset);
            section.size = fileToCpuEndian(section.size);
            section.compressedSize = fileToCpuEndian(section.compressedSize);
            section.checksum = fileToCpuEndian(section.checksum);
        }
        
        return validate();
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointReader::open: ") + e.what();
        close();
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
    error_.clear();
    streamPos_ = 0;
}

bool CheckpointReader::validate() const {
    if (!stream_.is_open()) {
        error_ = "Checkpoint reader is not open";
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Check if all sections can be read
    for (const auto& section : sections_) {
        if (section.offset + section.size > header_.totalSize) {
            error_ = "Section exceeds file bounds";
            return false;
        }
    }
    
    // TODO: Implement checksum validation for each section
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> data;
    
    // Find the section
    for (const auto& section : sections_) {
        if (section.type == sectionType) {
            // Read section data
            stream_.seekg(section.offset);
            streamPos_ = section.offset;
            
            data.resize(section.size);
            if (!stream_.read(reinterpret_cast<char*>(data.data()), section.size)) {
                error_ = "Failed to read checkpoint section data";
                return std::vector<uint8_t>();
            }
            
            // TODO: Implement checksum verification
            // TODO: Implement decompression if needed
            
            return data;
        }
    }
    
    error_ = "Section type not found in checkpoint";
    return std::vector<uint8_t>();
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto neuronSection = readSection(CheckpointSection::Neurons);
    if (neuronSection.empty()) {
        return false;
    }
    
    // Check if data size matches expected neuron count
    size_t expectedSize = sizeof(float) * 11 + sizeof(uint64_t) * 4;
    if (neuronSection.size() % expectedSize != 0) {
        error_ = "Invalid neuron data format in checkpoint";
        return false;
    }
    
    size_t neuronCount = header_.neuronCount;
    size_t dataSize = neuronSection.size();
    size_t neuronsInData = dataSize / expectedSize;
    
    if (neuronsInData != neuronCount) {
        error_ = "Neuron count mismatch in checkpoint";
        return false;
    }
    
    // Parse neuron data
    size_t index = 0;
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
    
    for (size_t i = 0; i < neuronCount; ++i) {
        data.membranePotential[i] = *reinterpret_cast<float*>(&neuronSection[index]); index += sizeof(float);
        data.restingPotential[i] = *reinterpret_cast<float*>(&neuronSection[index]); index += sizeof(float);
        data.threshold[i] = *reinterpret_cast<float*>(&neuronSection[index]); index += sizeof(float);
        data.resetPotential[i] = *reinterpret_cast<float*>(&neuronSection[index]); index += sizeof(float);
        data.leakConductance[i] = *reinterpret_cast<float*>(&neuronSection[index]); index += sizeof(float);
        data.firingState[i] = *reinterpret_cast<uint8_t*>(&neuronSection[index]); index += sizeof(uint8_t);
        data.refractoryRemaining[i] = *reinterpret_cast<uint32_t*>(&neuronSection[index]); index += sizeof(uint32_t);
        data.refractoryPeriod[i] = *reinterpret_cast<uint32_t*>(&neuronSection[index]); index += sizeof(uint32_t);
        data.lastSpikeTime[i] = *reinterpret_cast<float*>(&neuronSection[index]); index += sizeof(float);
        data.neuronType[i] = *reinterpret_cast<uint64_t*>(&neuronSection[index]); index += sizeof(uint64_t);
        data.regionId[i] = *reinterpret_cast<uint64_t*>(&neuronSection[index]); index += sizeof(uint64_t);
        data.populationId[i] = *reinterpret_cast<uint64_t*>(&neuronSection[index]); index += sizeof(uint64_t);
    }
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto synapseSection = readSection(CheckpointSection::Synapses);
    if (synapseSection.empty()) {
        return false;
    }
    
    // Check if data size matches expected synapse count
    size_t expectedSize = sizeof(uint64_t) * 2 + sizeof(float) * 5 + sizeof(uint32_t) * 1 + 
                         sizeof(uint8_t) * 3 + sizeof(float) * 4;
    if (synapseSection.size() % expectedSize != 0) {
        error_ = "Invalid synapse data format in checkpoint";
        return false;
    }
    
    size_t synapseCount = header_.synapseCount;
    size_t dataSize = synapseSection.size();
    size_t synapsesInData = dataSize / expectedSize;
    
    if (synapsesInData != synapseCount) {
        error_ = "Synapse count mismatch in checkpoint";
        return false;
    }
    
    // Parse synapse data
    size_t index = 0;
    data.sourceNeuron.resize(synapseCount);
    data.destinationNeuron.resize(synapseCount);
    data.weight.resize(synapseCount);
    data.delay.resize(synapseCount);
    data.synapseType.resize(synapseCount);
    data.plasticityFlags.resize(synapseCount);
    data.eligibilityTrace.resize(synapseCount);
    data.efficacy.resize(synapseCount);
    data.shortTermDepression.resize(synapseCount);
    data.shortTermFacilitation.resize(synapseCount);
    
    for (size_t i = 0; i < synapseCount; ++i) {
        data.sourceNeuron[i] = *reinterpret_cast<uint64_t*>(&synapseSection[index]); index += sizeof(uint64_t);
        data.destinationNeuron[i] = *reinterpret_cast<uint64_t*>(&synapseSection[index]); index += sizeof(uint64_t);
        data.weight[i] = *reinterpret_cast<float*>(&synapseSection[index]); index += sizeof(float);
        data.delay[i] = *reinterpret_cast<uint32_t*>(&synapseSection[index]); index += sizeof(uint32_t);
        data.synapseType[i] = *reinterpret_cast<uint8_t*>(&synapseSection[index]); index += sizeof(uint8_t);
        data.plasticityFlags[i] = *reinterpret_cast<uint8_t*>(&synapseSection[index]); index += sizeof(uint8_t);
        data.eligibilityTrace[i] = *reinterpret_cast<float*>(&synapseSection[index]); index += sizeof(float);
        data.efficacy[i] = *reinterpret_cast<float*>(&synapseSection[index]); index += sizeof(float);
        data.shortTermDepression[i] = *reinterpret_cast<float*>(&synapseSection[index]); index += sizeof(float);
        data.shortTermFacilitation[i] = *reinterpret_cast<float*>(&synapseSection[index]); index += sizeof(float);
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (!header_.validate() || sections_.empty()) {
        return 0.0;
    }
    
    // Simple progress calculation
    double totalSize = header_.totalSize;
    if (totalSize == 0) {
        return 1.0;
    }
    
    double readSize = streamPos_;
    return std::min(1.0, readSize / totalSize);
}

// CheckpointWriter implementation

CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(CompressionLevel::Balanced), 
    header_(), sections_(), bytesWritten_(0), compressBuffer_() {}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Close any existing stream
        abort();
        
        // Store filepath and compression
        filepath_ = filepath;
        compression_ = compression;
        
        // Create directory if needed
        std::filesystem::path dir = std::filesystem::path(filepath_).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        // Open file for writing
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        bytesWritten_ = 0;
        sections_.clear();
        
        // Initialize header
        header_.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointWriter::create: ") + e.what();
        abort();
        return false;
    }
}

void CheckpointWriter::close() {
    abort();
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
        error_ = "Checkpoint writer is not open";
        return false;
    }
    
    // Calculate size
    size_t neuronCount = data.membranePotential.size();
    size_t expectedSize = sizeof(float) * 11 + sizeof(uint64_t) * 4;
    
    if (neuronCount == 0) {
        return true; // Empty checkpoint is valid
    }
    
    if (data.membranePotential.size() != data.restingPotential.size() ||
        data.restingPotential.size() != data.threshold.size() ||
        data.threshold.size() != data.resetPotential.size() ||
        data.resetPotential.size() != data.leakConductance.size() ||
        data.leakConductance.size() != data.firingState.size() ||
        data.firingState.size() != data.refractoryRemaining.size() ||
        data.refractoryRemaining.size() != data.refractoryPeriod.size() ||
        data.refractoryPeriod.size() != data.lastSpikeTime.size() ||
        data.lastSpikeTime.size() != data.neuronType.size() ||
        data.neuronType.size() != data.regionId.size() ||
        data.regionId.size() != data.populationId.size()) {
        error_ = "Neuron data arrays have inconsistent sizes";
        return false;
    }
    
    size_t totalSize = neuronCount * expectedSize;
    
    // Create section header
    SectionHeader section;
    section.type = static_cast<uint32_t>(CheckpointSection::Neurons);
    section.offset = bytesWritten_ + sizeof(header_) + sizeof(SectionHeader) * sections_.size();
    section.size = totalSize;
    section.compressedSize = totalSize;
    section.checksum = 0; // TODO: calculate actual checksum
    
    // Write section header
    uint64_t sectionOffset = fileToCpuEndian(section.offset);
    uint64_t sectionSize = fileToCpuEndian(section.size);
    uint64_t sectionCompressedSize = fileToCpuEndian(section.compressedSize);
    uint32_t sectionChecksum = fileToCpuEndian(section.checksum);
    
    stream_.write(reinterpret_cast<const char*>(&sectionOffset), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&sectionSize), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&sectionCompressedSize), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&sectionChecksum), sizeof(uint32_t));
    bytesWritten_ += sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint32_t);
    
    sections_.push_back(section);
    
    // Write neuron data
    for (size_t i = 0; i < neuronCount; ++i) {
        float membranePotential = data.membranePotential[i];
        float restingPotential = data.restingPotential[i];
        float threshold = data.threshold[i];
        float resetPotential = data.resetPotential[i];
        float leakConductance = data.leakConductance[i];
        uint8_t firingState = data.firingState[i];
        uint32_t refractoryRemaining = data.refractoryRemaining[i];
        uint32_t refractoryPeriod = data.refractoryPeriod[i];
        float lastSpikeTime = data.lastSpikeTime[i];
        uint64_t neuronType = data.neuronType[i];
        uint64_t regionId = data.regionId[i];
        uint64_t populationId = data.populationId[i];
        
        // Write in file endian
        stream_.write(reinterpret_cast<const char*>(&membranePotential), sizeof(float));
        stream_.write(reinterpret_cast<const char*>(&restingPotential), sizeof(float));
        stream_.write(reinterpret_cast<const char*>(&threshold), sizeof(float));
        stream_.write(reinterpret_cast<const char*>(&resetPotential), sizeof(float));
        stream_.write(reinterpret_cast<const char*>(&leakConductance), sizeof(float));
        stream_.write(reinterpret_cast<const char*>(&firingState), sizeof(uint8_t));
        stream_.write(reinterpret_cast<const char*>(&refractoryRemaining), sizeof(uint32_t));
        stream_.write(reinterpret_cast<const char*>(&refractoryPeriod), sizeof(uint32_t));
        stream_.write(reinterpret_cast<const char*>(&lastSpikeTime), sizeof(float));
        stream_.write(reinterpret_cast<const char*>(&neuronType), sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(&regionId), sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(&populationId), sizeof(uint64_t));
    }
    
    bytesWritten_ += totalSize;
    
    return true;
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    if (!isOpen()) {
        error_ = "Checkpoint writer is not open";
        return false;
    }
    
    // Calculate size
    size_t synapseCount = data.sourceNeuron.size();
    size_t expectedSize = sizeof(uint64_t) * 2 + sizeof(float) * 5 + sizeof(uint32_t) * 1 + 
                         sizeof(uint8_t) * 3 + sizeof(float) * 4;
    
    if (synapseCount == 0) {
        return true; // Empty checkpoint is valid
    }
    
    if (data.sourceNeuron.size() != data.destinationNeuron.size() ||
        data.destinationNeuron.size() != data.weight.size() ||
        data.weight.size() != data.delay.size() ||
        data.delay.size() != data.synapseType.size() ||
        data.synapseType.size() != data.plasticityFlags.size() ||
        data.plasticityFlags.size() != data.eligibilityTrace.size() ||
        data.eligibilityTrace.size() != data.efficacy.size() ||
        data.efficacy.size() != data.shortTermDepression.size() ||
        data.shortTermDepression.size() != data.shortTermFacilitation.size()) {
        error_ = "Synapse data arrays have inconsistent sizes";
        return false;
    }
    
    size_t totalSize = synapseCount * expectedSize;
    
    // Create section header
    SectionHeader section;
    section.type = static_cast<uint32_t>(CheckpointSection::Synapses);
    section.offset = bytesWritten_ + sizeof(header_) + sizeof(SectionHeader) * sections_.size();
    section.size = totalSize;
    section.compressedSize = totalSize;
    section.checksum = 0; // TODO: calculate actual checksum
    
    // Write section header
    uint64_t sectionOffset = fileToCpuEndian(section.offset);
    uint64_t sectionSize = fileToCpuEndian(section.size);
    uint64_t sectionCompressedSize = fileToCpuEndian(section.compressedSize);
    uint32_t sectionChecksum = fileToCpuEndian(section.checksum);
    
    stream_.write(reinterpret_cast<const char*>(&sectionOffset), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&sectionSize), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&sectionCompressedSize), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&sectionChecksum), sizeof(uint32_t));
    bytesWritten_ += sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint32_t);
    
    sections_.push_back(section);
    
    // Write synapse data
    for (size_t i = 0; i < synapseCount; ++i) {
        uint64_t sourceNeuron = data.sourceNeuron[i];
        uint64_t destinationNeuron = data.destinationNeuron[i];
        float weight = data.weight[i];
        uint32_t delay = data.delay[i];
        uint8_t synapseType = data.synapseType[i];
        uint8_t plasticityFlags = data.plasticityFlags[i];
        float eligibilityTrace = data.eligibilityTrace[i];
        float efficacy = data.efficacy[i];
        float shortTermDepression = data.shortTermDepression[i];
        float shortTermFacilitation = data.shortTermFacilitation[i];
        
        // Write in file endian
        stream_.write(reinterpret_cast<const char*>(&sourceNeuron), sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(&destinationNeuron), sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(&weight), sizeof(float));
        stream_.write(reinterpret_cast<const char*>(&delay), sizeof(uint32_t));
        stream_.write(reinterpret_cast<const char*>(&synapseType), sizeof(uint8_t));
        stream_.write(reinterpret_cast<const char*>(&plasticityFlags), sizeof(uint8_t));
        stream_.write(reinterpret_cast<const char*>(&eligibilityTrace), sizeof(float));
        stream_.write(reinterpret_cast<const char*>(&efficacy), sizeof(float));
        stream_.write(reinterpret_cast<const char*>(&shortTermDepression), sizeof(float));
        stream_.write(reinterpret_cast<const char*>(&shortTermFacilitation), sizeof(float));
    }
    
    bytesWritten_ += totalSize;
    
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!isOpen()) {
        error_ = "Checkpoint writer is not open";
        return false;
    }
    
    // TODO: Implement compression if needed
    
    // Create section header
    SectionHeader section;
    section.type = static_cast<uint32_t>(sectionType);
    section.offset = bytesWritten_ + sizeof(header_) + sizeof(SectionHeader) * sections_.size();
    section.size = size;
    section.compressedSize = size;
    section.checksum = 0; // TODO: calculate actual checksum
    
    // Write section header
    uint64_t sectionOffset = fileToCpuEndian(section.offset);
    uint64_t sectionSize = fileToCpuEndian(section.size);
    uint64_t sectionCompressedSize = fileToCpuEndian(section.compressedSize);
    uint32_t sectionChecksum = fileToCpuEndian(section.checksum);
    
    stream_.write(reinterpret_cast<const char*>(&sectionOffset), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&sectionSize), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&sectionCompressedSize), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&sectionChecksum), sizeof(uint32_t));
    bytesWritten_ += sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint32_t);
    
    sections_.push_back(section);
    
    // Write section data
    stream_.write(reinterpret_cast<const char*>(data), size);
    bytesWritten_ += size;
    
    return true;
}

bool CheckpointWriter::finalize() {
    if (!isOpen()) {
        error_ = "Checkpoint writer is not open";
        return false;
    }
    
    // Calculate header size (without section count)
    size_t headerSize = sizeof(header_);
    
    // Write header
    uint64_t magic = cpuToFileEndian(header_.magic);
    uint32_t majorVersion = cpuToFileEndian(header_.majorVersion);
    uint32_t minorVersion = cpuToFileEndian(header_.minorVersion);
    uint32_t patchVersion = cpuToFileEndian(header_.patchVersion);
    uint64_t timestamp = cpuToFileEndian(header_.timestamp);
    uint64_t totalSize = cpuToFileEndian(bytesWritten_ - headerSize);
    uint64_t neuronCount = cpuToFileEndian(header_.neuronCount);
    uint64_t synapseCount = cpuToFileEndian(header_.synapseCount);
    uint64_t regionCount = cpuToFileEndian(header_.regionCount);
    uint64_t step = cpuToFileEndian(header_.step);
    double simulationTime = cpuToFileEndian(header_.simulationTime);
    uint64_t checksum = cpuToFileEndian(header_.checksum);
    uint32_t compressionLevel = cpuToFileEndian(header_.compressionLevel);
    uint32_t flags = cpuToFileEndian(header_.flags);
    
    stream_.write(reinterpret_cast<const char*>(&magic), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&majorVersion), sizeof(uint32_t));
    stream_.write(reinterpret_cast<const char*>(&minorVersion), sizeof(uint32_t));
    stream_.write(reinterpret_cast<const char*>(&patchVersion), sizeof(uint32_t));
    stream_.write(reinterpret_cast<const char*>(&timestamp), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&totalSize), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&neuronCount), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&synapseCount), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&regionCount), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&step), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&simulationTime), sizeof(double));
    stream_.write(reinterpret_cast<const char*>(&checksum), sizeof(uint64_t));
    stream_.write(reinterpret_cast<const char*>(&compressionLevel), sizeof(uint32_t));
    stream_.write(reinterpret_cast<const char*>(&flags), sizeof(uint32_t));
    
    header_.totalSize = bytesWritten_ - headerSize;
    
    // Write section headers
    for (const auto& section : sections_) {
        uint64_t sectionOffset = cpuToFileEndian(section.offset);
        uint64_t sectionSize = cpuToFileEndian(section.size);
        uint64_t sectionCompressedSize = cpuToFileEndian(section.compressedSize);
        uint32_t sectionChecksum = cpuToFileEndian(section.checksum);
        
        stream_.write(reinterpret_cast<const char*>(&sectionOffset), sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(&sectionSize), sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(&sectionCompressedSize), sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(&sectionChecksum), sizeof(uint32_t));
    }
    
    stream_.flush();
    
    return true;
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
    bytesWritten_ = 0;
    std::filesystem::remove(filepath_);
}

// CheckpointManager implementation

CheckpointManager::CheckpointManager() : checkpointDir_(), saveIntervalSteps_(10000), 
    maxCheckpoints_(10), compress_(true), lastSaveStep_(0), lastCheckpointPath_(),
    neuronProvider_(), synapseProvider_() {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir, 
    uint64_t saveIntervalSteps, size_t maxCheckpoints, bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    lastSaveStep_ = 0;
    lastCheckpointPath_ = "";
    
    // Create checkpoint directory
    std::filesystem::path dir(checkpointDir_);
    if (!dir.empty()) {
        std::filesystem::create_directories(dir);
    }
}

void CheckpointManager::setNeuronProvider(std::function<bool(NeuronCheckpointData&)> provider) {
    neuronProvider_ = std::move(provider);
}

void CheckpointManager::setSynapseProvider(std::function<bool(SynapseCheckpointData&)> provider) {
    synapseProvider_ = std::move(provider);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Checkpoint providers not set";
        return false;
    }
    
    try {
        // Generate checkpoint filename
        std::string filename;
        if (name.empty()) {
            // Use step number as filename
            filename = checkpointDir_ + "/checkpoint_" + 
                std::to_string(lastSaveStep_) + ".bin";
        } else {
            filename = checkpointDir_ + "/" + name + ".bin";
        }
        
        // Create checkpoint writer
        CheckpointWriter writer;
        CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
        
        if (!writer.create(filename, compression)) {
            error_ = "Failed to create checkpoint writer";
            return false;
        }
        
        // Get checkpoint data from providers
        NeuronCheckpointData neuronData;
        SynapseCheckpointData synapseData;
        
        if (!neuronProvider_(neuronData) || !synapseProvider_(synapseData)) {
            writer.abort();
            error_ = "Failed to get checkpoint data from providers";
            return false;
        }
        
        // Write data to checkpoint
        if (!writer.writeNeurons(neuronData) || !writer.writeSynapses(synapseData)) {
            writer.abort();
            error_ = "Failed to write checkpoint data";
            return false;
        }
        
        if (!writer.finalize()) {
            writer.abort();
            error_ = "Failed to finalize checkpoint";
            return false;
        }
        
        // Update checkpoint manager state
        lastSaveStep_ = lastSaveStep_ + saveIntervalSteps_;
        lastCheckpointPath_ = filename;
        
        // Prune old checkpoints
        pruneOldCheckpoints(maxCheckpoints_);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointManager::saveImmediately: ") + e.what();
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string filename = checkpointDir_ + "/" + name + ".bin";
    
    CheckpointReader reader;
    if (!reader.open(filename)) {
        error_ = "Failed to open checkpoint: " + reader.getError();
        return false;
    }
    
    // TODO: Implement actual loading logic using the providers
    // For now, just validate that the checkpoint can be read
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (checkpointDir_.empty()) {
        return checkpoints;
    }
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                info.fileSize = entry.file_size();
                info.timestamp = std::chrono::system_clock::to_time_t(
                    std::chrono::file_clock::clock::now());
                
                // Extract info from filename if possible
                // In a real implementation, this would parse metadata from the checkpoint
                
                checkpoints.push_back(info);
            }
        }
        
        // Sort by timestamp (newest first)
        std::sort(checkpoints.begin(), checkpoints.end(),
            [](const CheckpointInfo& a, const CheckpointInfo& b) {
                return a.timestamp > b.timestamp;
            });
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception in CheckpointManager::listCheckpoints: ") + e.what();
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Keep the most recent checkpoints
    std::sort(checkpoints.begin(), checkpoints.end(),
        [](const CheckpointInfo& a, const CheckpointInfo& b) {
            return a.timestamp > b.timestamp;
        });
    
    size_t toDelete = checkpoints.size() - keepCount;
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name + ".bin";
        try {
            std::filesystem::remove(filepath);
        } catch (const std::exception& e) {
            // Log error but don't fail the operation
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= lastSaveStep_;
}

} // namespace nlm

namespace nlm {
    // Check if checkpoint is open
    bool CheckpointWriter::isOpen() const {
        return stream_.is_open();
    }
}

namespace nlm {
    // Check if checkpoint is open
    bool CheckpointReader::isOpen() const {
        return stream_.is_open();
    }
}

// Additional helper functions for compatibility

namespace nlm {
    // Get checksum of data
    uint64_t calculateChecksum(const void* data, size_t size) {
        return ChecksumCalculator::crc64(data, size);
    }
    
    // Generate unique checkpoint filename
    std::string generateCheckpointName(const std::string& prefix, uint64_t step) {
        return prefix + "_" + std::to_string(step) + ".bin";
    }
    
    // Validate checkpoint file
    bool validateCheckpoint(const std::string& filepath) {
        CheckpointReader reader;
        return reader.open(filepath) && reader.validate();
    }
}