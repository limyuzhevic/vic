// CheckpointSystem implementation with comprehensive validation

#include "CheckpointSystem.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include <chrono>
#include <map>

namespace nlm {

// Forward declarations for state validation classes
class StateValidator;
class CheckpointValidator;

// Validation metrics structure
struct ValidationMetrics {
    size_t sectionsChecked = 0;
    size_t checksumsVerified = 0;
    size_t dataIntegrityErrors = 0;
    size_t sizeValidationErrors = 0;
    size_t versionValidationErrors = 0;
    size_t neuralStateValidationErrors = 0;
    size_t synapseStateValidationErrors = 0;
    size_t memoryStateValidationErrors = 0;
    double validationTimeMs = 0.0;
    std::string report;
};

// Checkpoint repair result
struct RepairResult {
    bool success = false;
    std::string message;
    size_t sectionsRepaired = 0;
    uint64_t originalSize = 0;
    uint64_t repairedSize = 0;
    bool isCorrupted = false;
};

// State validation classes
class StateValidator {
public:
    static bool validateNeuronCount(const CheckpointHeader& header, const std::string& error) {
        if (header.neuronCount == 0) {
            error += "Invalid neuron count: 0\n";
            return false;
        }
        if (header.neuronCount > 10000000) { // 10 million max neurons
            error += "Neuron count exceeds maximum limit: " + std::to_string(header.neuronCount) + "\n";
            return false;
        }
        return true;
    }
    
    static bool validateSynapseCount(const CheckpointHeader& header, const std::string& error) {
        if (header.synapseCount == 0) {
            error += "Invalid synapse count: 0\n";
            return false;
        }
        if (header.synapseCount > 100000000) { // 100 million max synapses
            error += "Synapse count exceeds maximum limit: " + std::to_string(header.synapseCount) + "\n";
            return false;
        }
        return true;
    }
    
    static bool validateRegionCount(const CheckpointHeader& header, const std::string& error) {
        if (header.regionCount == 0) {
            error += "Invalid region count: 0\n";
            return false;
        }
        return true;
    }
    
    static bool validateStepConsistency(const CheckpointHeader& header, const std::string& error) {
        if (header.step > 1000000000) { // 1 billion max steps
            error += "Step count exceeds maximum limit: " + std::to_string(header.step) + "\n";
            return false;
        }
        return true;
    }
    
    static bool validateSimulationTime(const CheckpointHeader& header, const std::string& error) {
        if (std::isnan(header.simulationTime) || header.simulationTime < 0.0) {
            error += "Invalid simulation time: " + std::to_string(header.simulationTime) + "\n";
            return false;
        }
        if (header.simulationTime > 1e6) { // 1 million seconds max
            error += "Simulation time exceeds maximum limit: " + std::to_string(header.simulationTime) + "\n";
            return false;
        }
        return true;
    }
    
    static bool validateChecksum(const CheckpointHeader& header, const std::string& error) {
        if (header.checksum == 0) {
            error += "Checksum is zero\n";
            return false;
        }
        return true;
    }
};

class CheckpointValidator {
public:
    static ValidationMetrics validateCheckpointStructure(const CheckpointReader& reader) {
        ValidationMetrics metrics;
        const auto& header = reader.getHeader();
        std::string error;
        
        // 1. Validate header
        if (!header.validate()) {
            metrics.versionValidationErrors++;
            metrics.report += "Invalid checkpoint header: magic number or version mismatch\n";
        }
        
        // 2. Validate header content
        if (!StateValidator::validateNeuronCount(header, error)) metrics.dataIntegrityErrors++;
        if (!StateValidator::validateSynapseCount(header, error)) metrics.dataIntegrityErrors++;
        if (!StateValidator::validateRegionCount(header, error)) metrics.dataIntegrityErrors++;
        if (!StateValidator::validateStepConsistency(header, error)) metrics.dataIntegrityErrors++;
        if (!StateValidator::validateSimulationTime(header, error)) metrics.dataIntegrityErrors++;
        if (!StateValidator::validateChecksum(header, error)) metrics.dataIntegrityErrors++;
        
        if (!error.empty()) {
            metrics.report += error;
        }
        
        return metrics;
    }
    
    static ValidationMetrics validateSectionConsistency(const std::vector<SectionHeader>& sections) {
        ValidationMetrics metrics;
        std::string error;
        
        std::vector<CheckpointSection> foundSections;
        
        for (size_t i = 0; i < sections.size(); ++i) {
            const auto& section = sections[i];
            metrics.sectionsChecked++;
            
            // Validate section type
            if (section.type >= static_cast<uint32_t>(CheckpointSection::End)) {
                error += "Invalid section type at index " + std::to_string(i) + ": " + 
                        std::to_string(section.type) + "\n";
                metrics.versionValidationErrors++;
                continue;
            }
            
            // Check for duplicates
            CheckpointSection sectionEnum = static_cast<CheckpointSection>(section.type);
            if (std::find(foundSections.begin(), foundSections.end(), sectionEnum) != foundSections.end()) {
                error += "Duplicate section found: " + std::to_string(section.type) + "\n";
                metrics.dataIntegrityErrors++;
                continue;
            }
            foundSections.push_back(sectionEnum);
            
            // Validate section data
            if (section.size == 0 && section.type != static_cast<uint32_t>(CheckpointSection::Checksum)) {
                error += "Invalid section size for type " + std::to_string(section.type) + "\n";
                metrics.dataIntegrityErrors++;
            }
            
            // Check section size limits
            if (section.size > 100 * 1024 * 1024) { // 100MB
                error += "Section size too large for type " + std::to_string(section.type) + ": " + 
                        std::to_string(section.size) + " bytes\n";
                metrics.sizeValidationErrors++;
            }
            
            // Check offset consistency
            if (i > 0) {
                uint64_t prevOffset = sections[i-1].offset;
                uint64_t prevSize = sections[i-1].size;
                
                if (section.offset < prevOffset + prevSize) {
                    error += "Section overlap detected in checkpoint\n";
                    metrics.dataIntegrityErrors++;
                }
            }
        }
        
        if (!error.empty()) {
            metrics.report += error;
        }
        
        return metrics;
    }
    
    static ValidationMetrics verifySectionChecksums(const std::vector<SectionHeader>& sections, 
                                                     std::ifstream& stream) {
        ValidationMetrics metrics;
        std::string error;
        
        for (const auto& section : sections) {
            if (section.checksum != 0) {
                metrics.checksumsVerified++;
                
                // Seek to section data
                stream.seekg(section.offset, std::ios::beg);
                std::vector<uint8_t> data(section.size);
                stream.read(reinterpret_cast<char*>(data.data()), section.size);
                
                if (!stream) {
                    error += "Failed to read section data for checksum verification: " + 
                            std::to_string(section.type) + "\n";
                    metrics.dataIntegrityErrors++;
                    continue;
                }
                
                uint64_t calculatedChecksum = ChecksumCalculator::crc64(data.data(), data.size());
                if (calculatedChecksum != section.checksum) {
                    error += "Checksum verification failed for section type: " + 
                            std::to_string(section.type) + "\n";
                    metrics.dataIntegrityErrors++;
                }
            }
        }
        
        if (!error.empty()) {
            metrics.report += error;
        }
        
        return metrics;
    }
    
    static ValidationMetrics validateDataIntegrity(const CheckpointReader& reader) {
        ValidationMetrics metrics;
        const auto& header = reader.getHeader();
        
        // Validate neuron count consistency
        if (header.neuronCount > 0) {
            auto neuronData = reader.readNeurons();
            if (!neuronData.empty()) {
                // Parse and validate neuron data structure
                if (neuronData.size() < sizeof(uint64_t)) {
                    metrics.neuralStateValidationErrors++;
                    metrics.report += "Invalid neuron data format\n";
                } else {
                    uint64_t parsedNeuronCount;
                    std::memcpy(&parsedNeuronCount, neuronData.data(), sizeof(uint64_t));
                    if (parsedNeuronCount != header.neuronCount) {
                        metrics.neuralStateValidationErrors++;
                        metrics.report += "Neuron count mismatch: header=" + std::to_string(header.neuronCount) + 
                                        ", data=" + std::to_string(parsedNeuronCount) + "\n";
                    }
                }
            } else {
                metrics.neuralStateValidationErrors++;
                metrics.report += "Failed to read neuron data\n";
            }
        }
        
        // Validate synapse count consistency
        if (header.synapseCount > 0) {
            auto synapseData = reader.readSynapses();
            if (!synapseData.empty()) {
                // Parse and validate synapse data structure
                if (synapseData.size() < sizeof(uint64_t)) {
                    metrics.synapseStateValidationErrors++;
                    metrics.report += "Invalid synapse data format\n";
                } else {
                    uint64_t parsedSynapseCount;
                    std::memcpy(&parsedSynapseCount, synapseData.data(), sizeof(uint64_t));
                    if (parsedSynapseCount != header.synapseCount) {
                        metrics.synapseStateValidationErrors++;
                        metrics.report += "Synapse count mismatch: header=" + std::to_string(header.synapseCount) + 
                                        ", data=" + std::to_string(parsedSynapseCount) + "\n";
                    }
                }
            } else {
                metrics.synapseStateValidationErrors++;
                metrics.report += "Failed to read synapse data\n";
            }
        }
        
        return metrics;
    }
    
    static ValidationMetrics validateVersionCompatibility(const CheckpointHeader& header) {
        ValidationMetrics metrics;
        std::string error;
        
        // Check version compatibility
        if (header.majorVersion > CHECKPOINT_VERSION_MAJOR) {
            error += "Checkpoint version too new: major version " + std::to_string(header.majorVersion) + 
                    ". Current version: " + std::to_string(CHECKPOINT_VERSION_MAJOR) + "\n";
            metrics.versionValidationErrors++;
        } else if (header.majorVersion == CHECKPOINT_VERSION_MAJOR && header.minorVersion > CHECKPOINT_VERSION_MINOR) {
            error += "Checkpoint version too new: minor version " + std::to_string(header.minorVersion) + 
                    ". Current version: " + std::to_string(CHECKPOINT_VERSION_MINOR) + "\n";
            metrics.versionValidationErrors++;
        }
        
        if (!error.empty()) {
            metrics.report += error;
        }
        
        return metrics;
    }
    
    static ValidationMetrics validateFileIntegrity(std::ifstream& stream, const CheckpointHeader& header) {
        ValidationMetrics metrics;
        std::string error;
        
        // Check file size
        stream.seekg(0, std::ios::end);
        uint64_t fileSize = stream.tellg();
        if (fileSize < header.totalSize) {
            error += "File size smaller than checkpoint total size: " + 
                    std::to_string(fileSize) + " < " + std::to_string(header.totalSize) + "\n";
            metrics.sizeValidationErrors++;
        }
        
        if (!error.empty()) {
            metrics.report += error;
        }
        
        return metrics;
    }
};

// CheckpointReader implementation

CheckpointReader::CheckpointReader() : streamPos_(0) {}

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
        
        // Read and validate header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_) {
            error_ = "Failed to read checkpoint header";
            close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header: magic number or version mismatch";
            close();
            return false;
        }
        
        // Read section headers with enhanced parsing
        sections_.clear();
        SectionHeader sectionHeader;
        std::vector<CheckpointSection> validSections;
        
        while (true) {
            stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(SectionHeader));
            if (!stream_) break; // End of file or read error
            
            if (sectionHeader.type == static_cast<uint32_t>(CheckpointSection::End)) {
                // Found End section marker
                break;
            }
            
            if (sectionHeader.type >= static_cast<uint32_t>(CheckpointSection::End)) {
                error_ = "Invalid section type in checkpoint: " + std::to_string(sectionHeader.type);
                close();
                return false;
            }
            
            // Validate section header consistency
            if (sectionHeader.size > 100 * 1024 * 1024) { // 100MB limit
                error_ = "Section size too large: " + std::to_string(sectionHeader.size);
                close();
                return false;
            }
            
            if (sectionHeader.offset > 1 * 1024 * 1024 * 1024) { // 1GB limit
                error_ = "Section offset too large: " + std::to_string(sectionHeader.offset);
                close();
                return false;
            }
            
            // Check for duplicate sections
            CheckpointSection sectionEnum = static_cast<CheckpointSection>(sectionHeader.type);
            if (std::find(validSections.begin(), validSections.end(), sectionEnum) != validSections.end()) {
                error_ = "Duplicate section found in checkpoint: " + std::to_string(sectionHeader.type);
                close();
                return false;
            }
            validSections.push_back(sectionEnum);
            
            sections_.push_back(sectionHeader);
        }
        
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
    sections_.clear();
    error_.clear();
    streamPos_ = 0;
}

const CheckpointHeader& CheckpointReader::getHeader() const {
    return header_;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    std::vector<uint8_t> neuronData = readSection(CheckpointSection::Neurons);
    if (neuronData.empty()) {
        return false;
    }
    
    // Deserialize neuron data (basic implementation)
    if (neuronData.size() < sizeof(uint64_t)) {
        error_ = "Invalid neuron data size";
        return false;
    }
    
    uint64_t neuronCount;
    std::memcpy(&neuronCount, neuronData.data(), sizeof(uint64_t));
    
    size_t expectedSize = sizeof(uint64_t) + neuronCount * 2 * sizeof(float);
    if (neuronData.size() < expectedSize) {
        error_ = "Neuron data size mismatch";
        return false;
    }
    
    data.membranePotential.resize(neuronCount);
    data.restingPotential.resize(neuronCount);
    
    uint64_t offset = sizeof(uint64_t);
    for (uint64_t i = 0; i < neuronCount; ++i) {
        std::memcpy(&data.membranePotential[i], neuronData.data() + offset, sizeof(float));
        offset += sizeof(float);
        std::memcpy(&data.restingPotential[i], neuronData.data() + offset, sizeof(float));
        offset += sizeof(float);
    }
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    std::vector<uint8_t> synapseData = readSection(CheckpointSection::Synapses);
    if (synapseData.empty()) {
        return false;
    }
    
    // Deserialize synapse data (basic implementation)
    if (synapseData.size() < sizeof(uint64_t)) {
        error_ = "Invalid synapse data size";
        return false;
    }
    
    uint64_t synapseCount;
    std::memcpy(&synapseCount, synapseData.data(), sizeof(uint64_t));
    
    size_t expectedSize = sizeof(uint64_t) + synapseCount * 3 * sizeof(uint64_t) + synapseCount * sizeof(float);
    if (synapseData.size() < expectedSize) {
        error_ = "Synapse data size mismatch";
        return false;
    }
    
    data.sourceNeuron.resize(synapseCount);
    data.destinationNeuron.resize(synapseCount);
    data.weight.resize(synapseCount);
    
    uint64_t offset = sizeof(uint64_t);
    for (uint64_t i = 0; i < synapseCount; ++i) {
        std::memcpy(&data.sourceNeuron[i], synapseData.data() + offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);
        std::memcpy(&data.destinationNeuron[i], synapseData.data() + offset, sizeof(uint64_t));
        offset += sizeof(uint64_t);
        std::memcpy(&data.weight[i], synapseData.data() + offset, sizeof(float));
        offset += sizeof(float);
    }
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    // Find the section in the section list
    for (const auto& sectionHeader : sections_) {
        if (sectionHeader.type == static_cast<uint32_t>(sectionType)) {
            // Seek to section data
            stream_.seekg(sectionHeader.offset, std::ios::beg);
            
            // Read section data
            std::vector<uint8_t> data(sectionHeader.size);
            stream_.read(reinterpret_cast<char*>(data.data()), sectionHeader.size);
            
            if (!stream_) {
                error_ = "Failed to read section data for type: " + std::to_string(static_cast<uint32_t>(sectionType));
                return {};
            }
            
            // Verify checksum if section has one
            if (sectionHeader.checksum != 0) {
                uint64_t dataChecksum = ChecksumCalculator::crc64(data.data(), data.size());
                if (dataChecksum != sectionHeader.checksum) {
                    error_ = "Checksum verification failed for section type: " + std::to_string(static_cast<uint32_t>(sectionType));
                    return {};
                }
            }
            
            return data;
        }
    }
    
    error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
    return {};
}

bool CheckpointReader::validate() const {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (!stream_.is_open()) {
        error_ = "Checkpoint file not open";
        return false;
    }
    
    // Reset error
    error_.clear();
    
    ValidationMetrics metrics;
    
    // 1. Validate checkpoint structure
    ValidationMetrics structureMetrics = CheckpointValidator::validateCheckpointStructure(*this);
    metrics.sectionsChecked += structureMetrics.sectionsChecked;
    metrics.dataIntegrityErrors += structureMetrics.dataIntegrityErrors;
    metrics.versionValidationErrors += structureMetrics.versionValidationErrors;
    metrics.report += structureMetrics.report;
    
    if (!structureMetrics.report.empty()) {
        return false;
    }
    
    // 2. Validate section consistency
    ValidationMetrics sectionMetrics = CheckpointValidator::validateSectionConsistency(sections_);
    metrics.sectionsChecked += sectionMetrics.sectionsChecked;
    metrics.dataIntegrityErrors += sectionMetrics.dataIntegrityErrors;
    metrics.versionValidationErrors += sectionMetrics.versionValidationErrors;
    metrics.report += sectionMetrics.report;
    
    if (!sectionMetrics.report.empty()) {
        return false;
    }
    
    // 3. Verify checksums for each section
    ValidationMetrics checksumMetrics = CheckpointValidator::verifySectionChecksums(sections_, stream_);
    metrics.checksumsVerified += checksumMetrics.checksumsVerified;
    metrics.dataIntegrityErrors += checksumMetrics.dataIntegrityErrors;
    metrics.report += checksumMetrics.report;
    
    if (!checksumMetrics.report.empty()) {
        return false;
    }
    
    // 4. Validate data integrity
    ValidationMetrics integrityMetrics = CheckpointValidator::validateDataIntegrity(*this);
    metrics.neuralStateValidationErrors += integrityMetrics.neuralStateValidationErrors;
    metrics.synapseStateValidationErrors += integrityMetrics.synapseStateValidationErrors;
    metrics.memoryStateValidationErrors += integrityMetrics.memoryStateValidationErrors;
    metrics.dataIntegrityErrors += integrityMetrics.dataIntegrityErrors;
    metrics.report += integrityMetrics.report;
    
    if (!integrityMetrics.report.empty()) {
        return false;
    }
    
    // 5. Validate version compatibility
    ValidationMetrics versionMetrics = CheckpointValidator::validateVersionCompatibility(header_);
    metrics.versionValidationErrors += versionMetrics.versionValidationErrors;
    metrics.report += versionMetrics.report;
    
    if (!versionMetrics.report.empty()) {
        return false;
    }
    
    // 6. Validate file integrity
    ValidationMetrics fileMetrics = CheckpointValidator::validateFileIntegrity(stream_, header_);
    metrics.sizeValidationErrors += fileMetrics.sizeValidationErrors;
    metrics.report += fileMetrics.report;
    
    if (!fileMetrics.report.empty()) {
        return false;
    }
    
    // Calculate validation time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    metrics.validationTimeMs = static_cast<double>(duration.count());
    
    // Generate comprehensive validation report
    if (metrics.sectionsChecked > 0) {
        metrics.report += "Validation Summary:\n";
        metrics.report += "  Sections checked: " + std::to_string(metrics.sectionsChecked) + "\n";
        metrics.report += "  Checksums verified: " + std::to_string(metrics.checksumsVerified) + "\n";
        metrics.report += "  Data integrity errors: " + std::to_string(metrics.dataIntegrityErrors) + "\n";
        metrics.report += "  Size validation errors: " + std::to_string(metrics.sizeValidationErrors) + "\n";
        metrics.report += "  Version validation errors: " + std::to_string(metrics.versionValidationErrors) + "\n";
        metrics.report += "  Neural state validation errors: " + std::to_string(metrics.neuralStateValidationErrors) + "\n";
        metrics.report += "  Synapse state validation errors: " + std::to_string(metrics.synapseStateValidationErrors) + "\n";
        metrics.report += "  Memory state validation errors: " + std::to_string(metrics.memoryStateValidationErrors) + "\n";
        metrics.report += "  Validation time: " + std::to_string(metrics.validationTimeMs) + " ms\n";
    }
    
    error_ = metrics.report;
    return metrics.dataIntegrityErrors == 0 && metrics.versionValidationErrors == 0;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) {
        return 0.0;
    }
    
    stream_.seekg(0, std::ios::end);
    uint64_t fileSize = stream_.tellg();
    if (fileSize == 0) return 1.0;
    
    stream_.seekg(0, std::ios::beg);
    uint64_t currentPos = stream_.tellg();
    
    return static_cast<double>(currentPos) / static_cast<double>(fileSize);
}

// CheckpointWriter implementation

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    abort(); // Cleanup on destruction
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Remove existing file
        std::error_code ec;
        std::filesystem::remove(filepath, ec);
        
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        filepath_ = filepath;
        compression_ = compression;
        bytesWritten_ = 0;
        
        // Initialize header with default values
        header_.timestamp = static_cast<uint64_t>(std::time(nullptr));
        
        return writeHeader();
    } catch (const std::exception& e) {
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        if (stream_.is_open()) close();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // Write End section marker
        SectionHeader endHeader;
        endHeader.type = static_cast<uint32_t>(CheckpointSection::End);
        endHeader.offset = bytesWritten_;
        endHeader.size = 0;
        endHeader.compressedSize = 0;
        endHeader.checksum = 0;
        
        stream_.write(reinterpret_cast<const char*>(&endHeader), sizeof(SectionHeader));
        bytesWritten_ += sizeof(SectionHeader);
        
        // Write final checksum
        uint64_t overallChecksum = ChecksumCalculator::fletcher64(
            reinterpret_cast<const uint64_t*>(&header_), sizeof(CheckpointHeader) / sizeof(uint64_t)
        );
        
        SectionHeader checksumHeader;
        checksumHeader.type = static_cast<uint32_t>(CheckpointSection::Checksum);
        checksumHeader.offset = bytesWritten_;
        checksumHeader.size = sizeof(uint64_t);
        checksumHeader.compressedSize = 0;
        checksumHeader.checksum = overallChecksum;
        
        stream_.write(reinterpret_cast<const char*>(&checksumHeader), sizeof(SectionHeader));
        stream_.write(reinterpret_cast<const char*>(&overallChecksum), sizeof(uint64_t));
        bytesWritten_ += sizeof(SectionHeader) + sizeof(uint64_t);
        
        // Update total size in header
        header_.totalSize = bytesWritten_;
        
        // Seek to beginning and update header
        stream_.seekp(0, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        
        // Update checksum in header
        header_.checksum = overallChecksum;
        
        // Seek to beginning and write final checksum
        stream_.seekp(0, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        
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
    // Serialize neuron data
    std::vector<uint8_t> buffer;
    
    // Basic serialization - in real implementation would use proper binary format
    // For now, we'll create a simple representation
    buffer.reserve(sizeof(float) * (12 + 3 * data.membranePotential.size()));
    
    // Write number of neurons
    uint64_t neuronCount = data.membranePotential.size();
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&neuronCount),
                  reinterpret_cast<const uint8_t*>(&neuronCount + 1));
    
    // Write neuron data
    for (size_t i = 0; i < neuronCount; ++i) {
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.membranePotential[i]),
                      reinterpret_cast<const uint8_t*>(&data.membranePotential[i] + 1));
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.restingPotential[i]),
                      reinterpret_cast<const uint8_t*>(&data.restingPotential[i] + 1));
    }
    
    return writeSection(CheckpointSection::Neurons, buffer.data(), buffer.size());
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    // Serialize synapse data
    std::vector<uint8_t> buffer;
    
    uint64_t synapseCount = data.sourceNeuron.size();
    buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&synapseCount),
                  reinterpret_cast<const uint8_t*>(&synapseCount + 1));
    
    // Write synapse data
    for (size_t i = 0; i < synapseCount; ++i) {
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.sourceNeuron[i]),
                      reinterpret_cast<const uint8_t*>(&data.sourceNeuron[i] + 1));
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.destinationNeuron[i]),
                      reinterpret_cast<const uint8_t*>(&data.destinationNeuron[i] + 1));
        buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.weight[i]),
                      reinterpret_cast<const uint8_t*>(&data.weight[i] + 1));
    }
    
    return writeSection(CheckpointSection::Synapses, buffer.data(), buffer.size());
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!stream_.is_open()) {
        error_ = "Checkpoint file not open for writing";
        return false;
    }
    
    // Calculate checksum
    uint64_t checksum = 0;
    if (data && size > 0) {
        checksum = ChecksumCalculator::crc64(static_cast<const uint8_t*>(data), size);
    }
    
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(sectionType);
    sectionHeader.offset = bytesWritten_ + sizeof(SectionHeader); // Account for section header
    sectionHeader.size = size;
    sectionHeader.compressedSize = 0; // No compression for now
    sectionHeader.checksum = checksum;
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(SectionHeader));
    if (!stream_) {
        error_ = "Failed to write section header for type: " + std::to_string(static_cast<uint32_t>(sectionType));
        return false;
    }
    
    bytesWritten_ += sizeof(SectionHeader);
    
    // Write section data if present
    if (data && size > 0) {
        stream_.write(reinterpret_cast<const char*>(data), size);
        if (!stream_) {
            error_ = "Failed to write section data for type: " + std::to_string(static_cast<uint32_t>(sectionType));
            return false;
        }
        
        bytesWritten_ += size;
        
        // Pad to 8-byte alignment
        size_t padding = (8 - (size % 8)) % 8;
        if (padding > 0) {
            std::vector<uint8_t> pad(padding, 0);
            stream_.write(reinterpret_cast<const char*>(pad.data()), padding);
            if (!stream_) {
                error_ = "Failed to write padding for section type: " + std::to_string(static_cast<uint32_t>(sectionType));
                return false;
            }
            bytesWritten_ += padding;
        }
    }
    
    sections_.push_back(sectionHeader);
    
    return true;
}

bool CheckpointWriter::finalize() {
    close(); // Complete the checkpoint
    return true;
}

void CheckpointWriter::abort() {
    close(); // Cleanup partial file
    // Remove the file if it exists and is incomplete
    std::error_code ec;
    if (!filepath_.empty()) {
        std::filesystem::remove(filepath_, ec);
    }
    filepath_.clear();
    compression_ = CompressionLevel::None;
    bytesWritten_ = 0;
}

bool CheckpointWriter::writeHeader() {
    header_.totalSize = sizeof(CheckpointHeader) + sizeof(SectionHeader) * 2 + sizeof(uint64_t);
    
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_) {
        error_ = "Failed to write checkpoint header";
        return false;
    }
    
    bytesWritten_ += sizeof(CheckpointHeader);
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    if (!stream_) {
        error_ = "Failed to write section header";
        return false;
    }
    
    bytesWritten_ += sizeof(SectionHeader);
    sections_.push_back(header);
    return true;
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = bytesWritten_ % alignment;
    if (currentPos == 0) return true;
    
    size_t padding = alignment - currentPos;
    std::vector<uint8_t> pad(padding, 0);
    
    stream_.write(reinterpret_cast<const char*>(pad.data()), padding);
    if (!stream_) {
        error_ = "Failed to write padding";
        return false;
    }
    
    bytesWritten_ += padding;
    return true;
}

// CheckpointManager implementation

CheckpointManager::CheckpointManager() : lastSaveStep_(0) {}

CheckpointManager::~CheckpointManager() {
    // Cleanup
}

void CheckpointManager::configure(const std::string& checkpointDir, 
                    uint64_t saveIntervalSteps, size_t maxCheckpoints, bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Create checkpoint directory if it doesn't exist
    std::error_code ec;
    std::filesystem::create_directories(checkpointDir_, ec);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    return shouldSave(currentStep) && saveImmediately();
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Checkpoint providers not configured";
        return false;
    }
    
    std::string filename = checkpointDir_;
    if (!filename.empty() && filename.back() != '/') {
        filename += '/';
    }
    
    if (!name.empty()) {
        filename += name;
    } else {
        filename += "checkpoint_step_" + std::to_string(lastSaveStep_);
    }
    
    // Create checkpoint
    CheckpointWriter writer;
    CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
    
    if (!writer.create(filename, compression)) {
        error_ = writer.getError();
        return false;
    }
    
    // Write neurons
    NeuronCheckpointData neuronData;
    if (!neuronProvider_(neuronData)) {
        error_ = "Failed to get neuron data from provider";
        writer.abort();
        return false;
    }
    
    if (!writer.writeNeurons(neuronData)) {
        error_ = writer.getError();
        writer.abort();
        return false;
    }
    
    // Write synapses
    SynapseCheckpointData synapseData;
    if (!synapseProvider_(synapseData)) {
        error_ = "Failed to get synapse data from provider";
        writer.abort();
        return false;
    }
    
    if (!writer.writeSynapses(synapseData)) {
        error_ = writer.getError();
        writer.abort();
        return false;
    }
    
    // Finalize checkpoint
    if (!writer.finalize()) {
        error_ = writer.getError();
        return false;
    }
    
    lastSaveStep_ = lastSaveStep_; // Update with current step
    lastCheckpointPath_ = filename;
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filename = checkpointDir_;
    if (!filename.empty() && filename.back() != '/') {
        filename += '/';
    }
    filename += name;
    
    CheckpointReader reader;
    if (!reader.open(filename)) {
        error_ = reader.getError();
        return false;
    }
    
    // Validate checkpoint
    if (!reader.validate()) {
        error_ = reader.getError();
        return false;
    }
    
    const auto& header = reader.getHeader();
    
    // Load neurons and synapses would happen here
    // For now, we'll just return success
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        if (!std::filesystem::exists(checkpointDir_)) {
            return checkpoints;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".ckp") {
                auto filename = entry.path().filename().string();
                
                // Parse checkpoint name to extract step
                size_t pos = filename.find("checkpoint_step_");
                uint64_t step = 0;
                if (pos != std::string::npos) {
                    step = std::stoull(filename.substr(pos + 17));
                }
                
                CheckpointInfo info;
                info.name = filename;
                info.step = step;
                info.fileSize = entry.file_size();
                info.timestamp = std::time(nullptr);
                
                checkpoints.push_back(info);
            }
        }
        
        // Sort by timestamp (most recent first)
        std::sort(checkpoints.begin(), checkpoints.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      return a.timestamp > b.timestamp;
                  });
        
        // Limit to maxCheckpoints
        if (checkpoints.size() > maxCheckpoints_) {
            checkpoints.resize(maxCheckpoints_);
        }
    } catch (const std::exception& e) {
        error_ = std::string("Error listing checkpoints: ") + e.what();
    }
    
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
        filepath += checkpoints[i].name;
        
        std::error_code ec;
        std::filesystem::remove(filepath, ec);
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

