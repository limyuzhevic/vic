NLM Checkpoint System Implementation
// Phase 5: Performance Infrastructure - Checkpoint Implementation

#include "performance/CheckpointSystem.hpp"
#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <iomanip>

namespace nlm {

// CheckpointWriter implementation

CheckpointWriter::CheckpointWriter()
    : compression_(CompressionLevel::Balanced)
    , bytesWritten_(0)
    , compressBuffer_() {}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    if (stream_.is_open()) {
        close();
    }
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Ensure directory exists
    std::filesystem::path dir = filepath_;
    dir.remove_filename();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        try {
            std::filesystem::create_directories(dir);
        } catch (const std::exception&) {
            NLM_LOG_ERROR("Failed to create checkpoint directory: " + filepath_);
            return false;
        }
    }
    
    // Create file for writing
    stream_.open(filepath_, std::ios::binary | std::ios::trunc);
    if (!stream_.is_open()) {
        NLM_LOG_ERROR("Failed to open checkpoint file for writing: " + filepath_);
        return false;
    }
    
    bytesWritten_ = 0;
    sections_.clear();
    compressBuffer_.clear();
    
    NLM_LOG_INFO("Created checkpoint file: " + filepath_);
    return true;
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
    header_.timestamp = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    // Convert neuron data to binary format
    std::vector<uint8_t> neuronBytes;
    std::stringstream ss;
    
    // Write header
    ss.write(reinterpret_cast<const char*>(&data.membranePotential.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.restingPotential.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.threshold.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.resetPotential.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.leakConductance.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.firingState.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.refractoryRemaining.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.refractoryPeriod.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.lastSpikeTime.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.neuronType.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.regionId.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.populationId.size()), sizeof(size_t));
    
    // Write data arrays
    for (float val : data.membranePotential) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    for (float val : data.restingPotential) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    for (float val : data.threshold) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    for (float val : data.resetPotential) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    for (float val : data.leakConductance) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    for (uint8_t val : data.firingState) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint8_t));
    }
    for (uint32_t val : data.refractoryRemaining) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
    }
    for (uint32_t val : data.refractoryPeriod) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
    }
    for (float val : data.lastSpikeTime) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    for (uint64_t val : data.neuronType) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));
    }
    for (uint64_t val : data.regionId) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));
    }
    for (uint64_t val : data.populationId) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));
    }
    
    neuronBytes = std::vector<uint8_t>(ss.str().begin(), ss.str().end());
    
    // Write section header
    SectionHeader section;
    section.type = static_cast<uint32_t>(CheckpointSection::Neurons);
    section.offset = bytesWritten_;
    section.size = static_cast<uint64_t>(neuronBytes.size());
    section.compressedSize = (compression_ != CompressionLevel::None) ? 
        static_cast<uint64_t>(compressBuffer_.size()) : section.size;
    section.checksum = ChecksumCalculator::crc32(neuronBytes.data(), neuronBytes.size());
    
    if (!writeSectionHeader(section)) {
        return false;
    }
    
    // Write data
    if (compression_ != CompressionLevel::None) {
        // Simple compression placeholder - in real implementation would use zlib/bzip2
        std::copy(neuronBytes.begin(), neuronBytes.end(), std::back_inserter(compressBuffer_));
        if (!writeSection(CheckpointSection::Neurons, compressBuffer_.data(), 
                         static_cast<size_t>(section.compressedSize))) {
            return false;
        }
        compressBuffer_.clear();
    } else {
        if (!writeSection(CheckpointSection::Neurons, neuronBytes.data(), 
                         neuronBytes.size())) {
            return false;
        }
    }
    
    sections_.push_back(section);
    bytesWritten_ += section.size;
    return true;
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    std::vector<uint8_t> synapseBytes;
    std::stringstream ss;
    
    // Write header
    ss.write(reinterpret_cast<const char*>(&data.sourceNeuron.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.destinationNeuron.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.weight.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.delay.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.synapseType.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.plasticityFlags.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.eligibilityTrace.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.efficacy.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.shortTermDepression.size()), sizeof(size_t));
    ss.write(reinterpret_cast<const char*>(&data.shortTermFacilitation.size()), sizeof(size_t));
    
    // Write data arrays
    for (uint64_t val : data.sourceNeuron) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));
    }
    for (uint64_t val : data.destinationNeuron) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));
    }
    for (float val : data.weight) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    for (uint32_t val : data.delay) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
    }
    for (uint8_t val : data.synapseType) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint8_t));
    }
    for (uint8_t val : data.plasticityFlags) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(uint8_t));
    }
    for (float val : data.eligibilityTrace) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    for (float val : data.efficacy) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    for (float val : data.shortTermDepression) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    for (float val : data.shortTermFacilitation) {
        ss.write(reinterpret_cast<const char*>(&val), sizeof(float));
    }
    
    synapseBytes = std::vector<uint8_t>(ss.str().begin(), ss.str().end());
    
    // Write section header
    SectionHeader section;
    section.type = static_cast<uint32_t>(CheckpointSection::Synapses);
    section.offset = bytesWritten_;
    section.size = static_cast<uint64_t>(synapseBytes.size());
    section.compressedSize = (compression_ != CompressionLevel::None) ? 
        static_cast<uint64_t>(compressBuffer_.size()) : section.size;
    section.checksum = ChecksumCalculator::crc32(synapseBytes.data(), synapseBytes.size());
    
    if (!writeSectionHeader(section)) {
        return false;
    }
    
    // Write data
    if (compression_ != CompressionLevel::None) {
        std::copy(synapseBytes.begin(), synapseBytes.end(), std::back_inserter(compressBuffer_));
        if (!writeSection(CheckpointSection::Synapses, compressBuffer_.data(), 
                         static_cast<size_t>(section.compressedSize))) {
            return false;
        }
        compressBuffer_.clear();
    } else {
        if (!writeSection(CheckpointSection::Synapses, synapseBytes.data(), 
                         synapseBytes.size())) {
            return false;
        }
    }
    
    sections_.push_back(section);
    bytesWritten_ += section.size;
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!stream_.is_open() || !data || size == 0) {
        return false;
    }
    
    uint64_t currentPos = static_cast<uint64_t>(stream_.tellp());
    
    stream_.write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
    if (stream_.fail()) {
        NLM_LOG_ERROR("Failed to write checkpoint section data");
        return false;
    }
    
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    if (stream_.fail()) {
        NLM_LOG_ERROR("Failed to write checkpoint section header");
        return false;
    }
    return true;
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        return false;
    }
    
    // Update header
    header_.totalSize = bytesWritten_;
    
    // Write header
    if (!writeHeader()) {
        return false;
    }
    
    // Write checksum
    uint64_t checksum = 0; // Placeholder for full checksum calculation
    stream_.write(reinterpret_cast<const char*>(&checksum), sizeof(uint64_t));
    
    stream_.flush();
    return !stream_.fail();
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        // Remove incomplete file
        std::filesystem::path filepath(filepath_);
        if (std::filesystem::exists(filepath)) {
            std::filesystem::remove(filepath);
        }
    }
}

bool CheckpointWriter::writeHeader() {
    uint64_t currentPos = static_cast<uint64_t>(stream_.tellp());
    
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    if (stream_.fail()) {
        NLM_LOG_ERROR("Failed to write checkpoint header");
        return false;
    }
    
    return true;
}

// CheckpointReader implementation

CheckpointReader::CheckpointReader()
    : streamPos_(0)
    , error_()
    , header_()
    , stream_()
    , sections_()
    {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    if (stream_.is_open()) {
        close();
    }
    
    stream_.open(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
        NLM_LOG_ERROR(error_);
        return false;
    }
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
    if (stream_.fail()) {
        error_ = "Failed to read checkpoint header";
        NLM_LOG_ERROR(error_);
        close();
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Checkpoint validation failed - invalid magic or version";
        NLM_LOG_ERROR(error_);
        close();
        return false;
    }
    
    // Read section headers
    while (stream_.tellg() < header_.totalSize) {
        SectionHeader section;
        stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader));
        if (stream_.fail()) {
            error_ = "Failed to read section header";
            NLM_LOG_ERROR(error_);
            close();
            return false;
        }
        
        sections_.push_back(section);
    }
    
    streamPos_ = static_cast<size_t>(stream_.tellg());
    NLM_LOG_INFO("Opened checkpoint file: " + filepath + " (" + 
                std::to_string(header_.neuronCount) + " neurons, " +
                std::to_string(header_.synapseCount) + " synapses)");
    return true;
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
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Check section positions
    for (size_t i = 0; i < sections_.size(); ++i) {
        const auto& section = sections_[i];
        if (section.offset >= header_.totalSize) {
            error_ = "Section " + std::to_string(i) + " offset out of bounds";
            return false;
        }
        
        if (i + 1 < sections_.size()) {
            if (section.offset + section.size > sections_[i + 1].offset) {
                error_ = "Section " + std::to_string(i) + " overlaps next section";
                return false;
            }
        }
    }
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> data;
    
    // Find section
    auto it = std::find_if(sections_.begin(), sections_.end(),
                          [sectionType](const SectionHeader& section) {
                              return section.type == static_cast<uint32_t>(sectionType);
                          });
    
    if (it == sections_.end()) {
        error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
        return data;
    }
    
    // Read section data
    stream_.seekg(static_cast<std::streamoff>(it->offset), std::ios::beg);
    data.resize(it->size);
    
    stream_.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));
    if (stream_.fail()) {
        error_ = "Failed to read section data";
        data.clear();
    }
    
    return data;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    std::vector<uint8_t> sectionData = readSection(CheckpointSection::Neurons);
    if (sectionData.empty()) {
        return false;
    }
    
    std::stringstream ss(std::string(sectionData.begin(), sectionData.end()), 
                         std::ios::binary);
    
    // Read sizes
    size_t membranePotentialSize, restingPotentialSize, thresholdSize,
           resetPotentialSize, leakConductanceSize, firingStateSize,
           refractoryRemainingSize, refractoryPeriodSize, lastSpikeTimeSize,
           neuronTypeSize, regionIdSize, populationIdSize;
    
    ss.read(reinterpret_cast<char*>(&membranePotentialSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&restingPotentialSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&thresholdSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&resetPotentialSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&leakConductanceSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&firingStateSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&refractoryRemainingSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&refractoryPeriodSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&lastSpikeTimeSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&neuronTypeSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&regionIdSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&populationIdSize), sizeof(size_t));
    
    // Reserve space
    data.membranePotential.resize(membranePotentialSize);
    data.restingPotential.resize(restingPotentialSize);
    data.threshold.resize(thresholdSize);
    data.resetPotential.resize(resetPotentialSize);
    data.leakConductance.resize(leakConductanceSize);
    data.firingState.resize(firingStateSize);
    data.refractoryRemaining.resize(refractoryRemainingSize);
    data.refractoryPeriod.resize(refractoryPeriodSize);
    data.lastSpikeTime.resize(lastSpikeTimeSize);
    data.neuronType.resize(neuronTypeSize);
    data.regionId.resize(regionIdSize);
    data.populationId.resize(populationIdSize);
    
    // Read data
    auto readFloatArray = [&ss](auto& vec) {
        for (auto& val : vec) {
            ss.read(reinterpret_cast<char*>(&val), sizeof(float));
        }
    };
    
    auto readU8Array = [&ss](auto& vec) {
        for (auto& val : vec) {
            ss.read(reinterpret_cast<char*>(&val), sizeof(uint8_t));
        }
    };
    
    auto readU32Array = [&ss](auto& vec) {
        for (auto& val : vec) {
            ss.read(reinterpret_cast<char*>(&val), sizeof(uint32_t));
        }
    };
    
    auto readU64Array = [&ss](auto& vec) {
        for (auto& val : vec) {
            ss.read(reinterpret_cast<char*>(&val), sizeof(uint64_t));
        }
    };
    
    readFloatArray(data.membranePotential);
    readFloatArray(data.restingPotential);
    readFloatArray(data.threshold);
    readFloatArray(data.resetPotential);
    readFloatArray(data.leakConductance);
    readU8Array(data.firingState);
    readU32Array(data.refractoryRemaining);
    readU32Array(data.refractoryPeriod);
    readFloatArray(data.lastSpikeTime);
    readU64Array(data.neuronType);
    readU64Array(data.regionId);
    readU64Array(data.populationId);
    
    return !ss.fail() && !data.membranePotential.empty();
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    std::vector<uint8_t> sectionData = readSection(CheckpointSection::Synapses);
    if (sectionData.empty()) {
        return false;
    }
    
    std::stringstream ss(std::string(sectionData.begin(), sectionData.end()), 
                         std::ios::binary);
    
    // Read sizes
    size_t sourceNeuronSize, destinationNeuronSize, weightSize,
           delaySize, synapseTypeSize, plasticityFlagsSize,
           eligibilityTraceSize, efficacySize, shortTermDepressionSize,
           shortTermFacilitationSize;
    
    ss.read(reinterpret_cast<char*>(&sourceNeuronSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&destinationNeuronSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&weightSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&delaySize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&synapseTypeSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&plasticityFlagsSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&eligibilityTraceSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&efficacySize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&shortTermDepressionSize), sizeof(size_t));
    ss.read(reinterpret_cast<char*>(&shortTermFacilitationSize), sizeof(size_t));
    
    // Reserve space
    data.sourceNeuron.resize(sourceNeuronSize);
    data.destinationNeuron.resize(destinationNeuronSize);
    data.weight.resize(weightSize);
    data.delay.resize(delaySize);
    data.synapseType.resize(synapseTypeSize);
    data.plasticityFlags.resize(plasticityFlagsSize);
    data.eligibilityTrace.resize(eligibilityTraceSize);
    data.efficacy.resize(efficacySize);
    data.shortTermDepression.resize(shortTermDepressionSize);
    data.shortTermFacilitation.resize(shortTermFacilitationSize);
    
    // Read data
    auto readFloatArray = [&ss](auto& vec) {
        for (auto& val : vec) {
            ss.read(reinterpret_cast<char*>(&val), sizeof(float));
        }
    };
    
    auto readU8Array = [&ss](auto& vec) {
        for (auto& val : vec) {
            ss.read(reinterpret_cast<char*>(&val), sizeof(uint8_t));
        }
    };
    
    auto readU32Array = [&ss](auto& vec) {
        for (auto& val : vec) {
            ss.read(reinterpret_cast<char*>(&val), sizeof(uint32_t));
        }
    };
    
    auto readU64Array = [&ss](auto& vec) {
        for (auto& val : vec) {
            ss.read(reinterpret_cast<char*>(&val), sizeof(uint64_t));
        }
    };
    
    readU64Array(data.sourceNeuron);
    readU64Array(data.destinationNeuron);
    readFloatArray(data.weight);
    readU32Array(data.delay);
    readU8Array(data.synapseType);
    readU8Array(data.plasticityFlags);
    readFloatArray(data.eligibilityTrace);
    readFloatArray(data.efficacy);
    readFloatArray(data.shortTermDepression);
    readFloatArray(data.shortTermFacilitation);
    
    return !ss.fail() && !data.weight.empty();
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) {
        return 0.0;
    }
    
    return static_cast<double>(streamPos_) / header_.totalSize;
}

} // namespace nlm

static_assert(sizeof(void*) == 8, "Checkpoint system requires 64-bit platform");

// Initialize static table for CRC64
// In a real implementation, this would be properly initialized
namespace nlm {
    namespace {
        uint64_t CRC64Table[256];
        bool crc64TableInitialized = false;
        
        void initCRC64Table() {
            if (crc64TableInitialized) return;
            
            const uint64_t polynomial = 0x95AC9324236C9C88ULL;
            for (int i = 0; i < 256; ++i) {
                uint64_t crc = i;
                for (int j = 0; j < 8; ++j) {
                    if (crc & 1) {
                        crc = (crc >> 1) ^ polynomial;
                    } else {
                        crc >>= 1;
                    }
                }
                CRC64Table[i] = crc;
            }
            crc64TableInitialized = true;
        }
    }
    
    uint64_t ChecksumCalculator::crc64(const void* data, size_t size) {
        initCRC64Table();
        
        uint64_t crc = 0xFFFFFFFFFFFFFFFFULL;
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        
        for (size_t i = 0; i < size; ++i) {
            uint8_t idx = (crc ^ bytes[i]) & 0xFF;
            crc = CRC64Table[idx] ^ (crc >> 8);
        }
        
        return crc ^ 0xFFFFFFFFFFFFFFFFULL;
    }
    
    // Simplified CRC32 implementation
    uint32_t ChecksumCalculator::crc32(const void* data, size_t size) {
        uint32_t crc = 0xFFFFFFFF;
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        
        for (size_t i = 0; i < size; ++i) {
            crc ^= bytes[i];
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
            }
        }
        
        return ~crc;
    }
    
    uint64_t ChecksumCalculator::fletcher64(const uint64_t* data, size_t count) {
        uint64_t sum1 = 0, sum2 = 0;
        for (size_t i = 0; i < count; ++i) {
            sum1 = (sum1 + data[i]) % 0xFFFFFFFFFFFFFFFF;
            sum2 = (sum2 + sum1) % 0xFFFFFFFFFFFFFFFF;
        }
        return (sum2 << 32) | sum1;
    }
    
    uint64_t ChecksumCalculator::combine(uint64_t a, uint64_t b) {
        return a ^ (b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2));
    }
} // namespace nlm
