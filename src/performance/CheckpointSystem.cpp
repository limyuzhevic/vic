// CheckpointSystem.cpp - Implementation of CheckpointWriter and CheckpointReader

#include "performance/CheckpointSystem.hpp"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

CheckpointReader::CheckpointReader() : stream_(), header_(), streamPos_(0) {}
CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        // Check if file exists and is readable
        if (!std::filesystem::exists(filepath)) {
            error_ = "Checkpoint file does not exist: " + filepath;
            return false;
        }
        
        stream_.open(filepath, std::ios::binary | std::ios::in);
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
            error_ = "Invalid checkpoint header (version or magic)";
            return false;
        }
        
        // Read section headers
        sections_.clear();
        size_t numSections = header_.totalSize / sizeof(SectionHeader);
        sections_.reserve(numSections);
        
        for (size_t i = 0; i < numSections; ++i) {
            SectionHeader sectionHeader;
            stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(SectionHeader));
            if (!stream_.good()) {
                close();
                error_ = "Failed to read section header";
                return false;
            }
            sections_.push_back(sectionHeader);
        }
        
        streamPos_ = stream_.tellg();
        
        NLM_LOG_INFO("CheckpointReader: Opened " + filepath + " with " + 
                    std::to_string(sections_.size()) + " sections");
        
        return true;
        
    } catch (const std::exception& e) {
        close();
        error_ = std::string("Exception opening checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    try {
        std::vector<uint8_t> sectionData = readSection(CheckpointSection::Neurons);
        if (sectionData.empty()) {
            error_ = "Failed to read neurons section";
            return false;
        }
        
        // Parse neurons data
        const uint8_t* ptr = sectionData.data();
        size_t size = sectionData.size();
        
        // Read vectors based on size
        auto read_vector = [&](auto& vec) {
            uint64_t count = *reinterpret_cast<const uint64_t*>(ptr);
            ptr += sizeof(uint64_t);
            size -= sizeof(uint64_t);
            
            vec.resize(count);
            if (count > 0) {
                size_t elementSize = sizeof(decltype(vec)::value_type);
                size_t totalSize = count * elementSize;
                if (size < totalSize) {
                    throw std::runtime_error("Not enough data for neurons");
                }
                
                std::memcpy(vec.data(), ptr, totalSize);
                ptr += totalSize;
                size -= totalSize;
            }
        };
        
        read_vector(data.membranePotential);
        read_vector(data.restingPotential);
        read_vector(data.threshold);
        read_vector(data.resetPotential);
        read_vector(data.leakConductance);
        
        // Read uint8_t vectors (enum values)
        auto read_uint8_vector = [&](auto& vec) {
            uint64_t count = *reinterpret_cast<const uint64_t*>(ptr);
            ptr += sizeof(uint64_t);
            size -= sizeof(uint64_t);
            
            vec.resize(count);
            if (count > 0) {
                size_t totalSize = count * sizeof(uint8_t);
                if (size < totalSize) {
                    throw std::runtime_error("Not enough data for uint8 vector");
                }
                
                std::memcpy(vec.data(), ptr, totalSize);
                ptr += totalSize;
                size -= totalSize;
            }
        };
        
        read_uint8_vector(data.firingState);
        read_uint8_vector(data.refractoryRemaining);
        read_uint8_vector(data.refractoryPeriod);
        read_uint8_vector(data.synapseType);
        read_uint8_vector(data.regionId);
        read_uint8_vector(data.populationId);
        
        // Read remaining float and uint64_t vectors
        read_vector(data.lastSpikeTime);
        read_vector(data.neuronType);
        
        NLM_LOG_INFO("CheckpointReader: Read " + std::to_string(data.membranePotential.size()) + 
                    " neurons");
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading neurons: ") + e.what();
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    try {
        std::vector<uint8_t> sectionData = readSection(CheckpointSection::Synapses);
        if (sectionData.empty()) {
            error_ = "Failed to read synapses section";
            return false;
        }
        
        // Parse synapses data (similar to neurons)
        const uint8_t* ptr = sectionData.data();
        size_t size = sectionData.size();
        
        auto read_vector = [&](auto& vec) {
            uint64_t count = *reinterpret_cast<const uint64_t*>(ptr);
            ptr += sizeof(uint64_t);
            size -= sizeof(uint64_t);
            
            vec.resize(count);
            if (count > 0) {
                size_t elementSize = sizeof(decltype(vec)::value_type);
                size_t totalSize = count * elementSize;
                if (size < totalSize) {
                    throw std::runtime_error("Not enough data for synapses");
                }
                
                std::memcpy(vec.data(), ptr, totalSize);
                ptr += totalSize;
                size -= totalSize;
            }
        };
        
        // Read all synapse data vectors
        read_vector(data.sourceNeuron);
        read_vector(data.destinationNeuron);
        read_vector(data.weight);
        read_vector(data.delay);
        read_vector(data.synapseType);
        read_vector(data.plasticityFlags);
        read_vector(data.eligibilityTrace);
        read_vector(data.efficacy);
        read_vector(data.shortTermDepression);
        read_vector(data.shortTermFacilitation);
        
        NLM_LOG_INFO("CheckpointReader: Read " + std::to_string(data.weight.size()) + 
                    " synapses");
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading synapses: ") + e.what();
        return false;
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& section : sections_) {
        if (section.type == sectionType) {
            // Skip to section data
            stream_.seekg(streamPos_ + section.offset, std::ios::beg);
            
            // Allocate buffer for section data
            std::vector<uint8_t> buffer(section.size);
            
            if (section.compressedSize > 0 && section.compressedSize < section.size) {
                // For now, just read uncompressed data
                // TODO: Implement decompression
                error_ = "Compressed sections not yet supported";
                return std::vector<uint8_t>();
            }
            
            stream_.read(reinterpret_cast<char*>(buffer.data()), section.size);
            if (!stream_.good()) {
                error_ = "Failed to read section data";
                return std::vector<uint8_t>();
            }
            
            streamPos_ = stream_.tellg();
            return buffer;
        }
    }
    
    error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    // Validate header
    if (!header_.validate()) {
        error_ = "Invalid header";
        return false;
    }
    
    // Validate checksums
    // For now, just check that we have sections
    if (sections_.empty()) {
        error_ = "No sections found";
        return false;
    }
    
    // Validate section sizes
    for (const auto& section : sections_) {
        if (section.size == 0) {
            error_ = "Empty section found";
            return false;
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    // For now, return a simple progress based on file position
    if (!stream_.is_open()) return 0.0;
    
    std::streampos currentPos = stream_.tellg();
    std::streampos fileSize = stream_.seekg(0, std::ios::end).seekg(0, std::ios::cur);
    
    if (fileSize <= 0) return 0.0;
    
    return static_cast<double>(currentPos) / fileSize;
}

CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(CompressionLevel::Balanced),
                                       header_(), sections_(), bytesWritten_(0) {}
CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Close any existing file
        abort();
        
        filepath_ = filepath;
        compression_ = compression;
        sections_.clear();
        bytesWritten_ = 0;
        
        // Create directory if it doesn't exist
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        
        // Open output file
        stream_.open(filepath, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        // Write header placeholder (will update after all sections)
        header_.timestamp = static_cast<uint64_t>(std::time(nullptr));
        header_.totalSize = sizeof(CheckpointHeader); // Placeholder
        
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_.good()) {
            abort();
            error_ = "Failed to write checkpoint header";
            return false;
        }
        
        bytesWritten_ += sizeof(CheckpointHeader);
        
        NLM_LOG_INFO("CheckpointWriter: Created " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        abort();
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // Finalize header and write section headers
        finalize();
    }
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        std::filesystem::remove(filepath_);
    }
    stream_.clear();
    filepath_.clear();
    sections_.clear();
    bytesWritten_ = 0;
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
        // Create neurons data structure
        struct NeuronsData {
            uint64_t membranePotentialCount;
            uint64_t restingPotentialCount;
            uint64_t thresholdCount;
            uint64_t resetPotentialCount;
            uint64_t leakConductanceCount;
            uint64_t firingStateCount;
            uint64_t refractoryRemainingCount;
            uint64_t refractoryPeriodCount;
            uint64_t lastSpikeTimeCount;
            uint64_t neuronTypeCount;
            uint64_t regionIdCount;
            uint64_t populationIdCount;
            
            // Vector data
            std::vector<float> membranePotential;
            std::vector<float> restingPotential;
            std::vector<float> threshold;
            std::vector<float> resetPotential;
            std::vector<float> leakConductance;
            std::vector<uint8_t> firingState;
            std::vector<uint32_t> refractoryRemaining;
            std::vector<uint32_t> refractoryPeriod;
            std::vector<float> lastSpikeTime;
            std::vector<uint64_t> neuronType;
            std::vector<uint64_t> regionId;
            std::vector<uint64_t> populationId;
        } neuronsData;
        
        // Copy data
        neuronsData.membranePotentialCount = data.membranePotential.size();
        neuronsData.restingPotentialCount = data.restingPotential.size();
        neuronsData.thresholdCount = data.threshold.size();
        neuronsData.resetPotentialCount = data.resetPotential.size();
        neuronsData.leakConductanceCount = data.leakConductance.size();
        neuronsData.firingStateCount = data.firingState.size();
        neuronsData.refractoryRemainingCount = data.refractoryRemaining.size();
        neuronsData.refractoryPeriodCount = data.refractoryPeriod.size();
        neuronsData.lastSpikeTimeCount = data.lastSpikeTime.size();
        neuronsData.neuronTypeCount = data.neuronType.size();
        neuronsData.regionIdCount = data.regionId.size();
        neuronsData.populationIdCount = data.populationId.size();
        
        neuronsData.membranePotential = data.membranePotential;
        neuronsData.restingPotential = data.restingPotential;
        neuronsData.threshold = data.threshold;
        neuronsData.resetPotential = data.resetPotential;
        neuronsData.leakConductance = data.leakConductance;
        neuronsData.firingState = data.firingState;
        neuronsData.refractoryRemaining = data.refractoryRemaining;
        neuronsData.refractoryPeriod = data.refractoryPeriod;
        neuronsData.lastSpikeTime = data.lastSpikeTime;
        neuronsData.neuronType = data.neuronType;
        neuronsData.regionId = data.regionId;
        neuronsData.populationId = data.populationId;
        
        return writeSection(CheckpointSection::Neurons, &neuronsData, sizeof(NeuronsData));
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing neurons: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Create synapses data structure
        struct SynapsesData {
            uint64_t sourceNeuronCount;
            uint64_t destinationNeuronCount;
            uint64_t weightCount;
            uint64_t delayCount;
            uint64_t synapseTypeCount;
            uint64_t plasticityFlagsCount;
            uint64_t eligibilityTraceCount;
            uint64_t efficacyCount;
            uint64_t shortTermDepressionCount;
            uint64_t shortTermFacilitationCount;
            
            // Vector data
            std::vector<uint64_t> sourceNeuron;
            std::vector<uint64_t> destinationNeuron;
            std::vector<float> weight;
            std::vector<uint32_t> delay;
            std::vector<uint8_t> synapseType;
            std::vector<uint8_t> plasticityFlags;
            std::vector<float> eligibilityTrace;
            std::vector<float> efficacy;
            std::vector<float> shortTermDepression;
            std::vector<float> shortTermFacilitation;
        } synapsesData;
        
        // Copy data
        synapsesData.sourceNeuronCount = data.sourceNeuron.size();
        synapsesData.destinationNeuronCount = data.destinationNeuron.size();
        synapsesData.weightCount = data.weight.size();
        synapsesData.delayCount = data.delay.size();
        synapsesData.synapseTypeCount = data.synapseType.size();
        synapsesData.plasticityFlagsCount = data.plasticityFlags.size();
        synapsesData.eligibilityTraceCount = data.eligibilityTrace.size();
        synapsesData.efficacyCount = data.efficacy.size();
        synapsesData.shortTermDepressionCount = data.shortTermDepression.size();
        synapsesData.shortTermFacilitationCount = data.shortTermFacilitation.size();
        
        synapsesData.sourceNeuron = data.sourceNeuron;
        synapsesData.destinationNeuron = data.destinationNeuron;
        synapsesData.weight = data.weight;
        synapsesData.delay = data.delay;
        synapsesData.synapseType = data.synapseType;
        synapsesData.plasticityFlags = data.plasticityFlags;
        synapsesData.eligibilityTrace = data.eligibilityTrace;
        synapsesData.efficacy = data.efficacy;
        synapsesData.shortTermDepression = data.shortTermDepression;
        synapsesData.shortTermFacilitation = data.shortTermFacilitation;
        
        return writeSection(CheckpointSection::Synapses, &synapsesData, sizeof(SynapsesData));
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing synapses: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint file not open";
            return false;
        }
        
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = bytesWritten_ + sizeof(CheckpointHeader) + 
                              sizeof(SectionHeader) * sections_.size();
        sectionHeader.size = size;
        sectionHeader.compressedSize = 0; // No compression for now
        sectionHeader.checksum = 0; // TODO: Calculate checksum
        
        // Write section header
        stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(SectionHeader));
        if (!stream_.good()) {
            error_ = "Failed to write section header";
            return false;
        }
        
        // Write section data
        stream_.write(reinterpret_cast<const char*>(data), size);
        if (!stream_.good()) {
            error_ = "Failed to write section data";
            return false;
        }
        
        // Pad to alignment
        size_t padding = padToAlignment(sizeof(size_t));
        if (padding > 0) {
            std::vector<uint8_t> padBuffer(padding, 0);
            stream_.write(reinterpret_cast<const char*>(padBuffer.data()), padding);
            if (!stream_.good()) {
                error_ = "Failed to write padding";
                return false;
            }
            sectionHeader.size += padding;
        }
        
        sections_.push_back(sectionHeader);
        bytesWritten_ += sizeof(SectionHeader) + size + padding;
        
        NLM_LOG_INFO("CheckpointWriter: Wrote section " + std::to_string(static_cast<uint32_t>(sectionType)) +
                    " (" + std::to_string(size) + " bytes)");
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing section: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        if (!stream_.is_open()) {
            return true; // Already finalized
        }
        
        // Update header with section count and total size
        header_.totalSize = sizeof(CheckpointHeader) + sizeof(SectionHeader) * sections_.size();
        
        // Recalculate checksum
        uint64_t checksum = 0;
        // TODO: Implement proper checksum calculation
        
        // Update header in file
        stream_.seekp(0, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_.good()) {
            error_ = "Failed to update header";
            return false;
        }
        
        // Write section headers
        for (const auto& section : sections_) {
            stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
            if (!stream_.good()) {
                error_ = "Failed to write section header";
                return false;
            }
        }
        
        stream_.flush();
        stream_.close();
        
        NLM_LOG_INFO("CheckpointWriter: Finalized checkpoint with " + 
                    std::to_string(sections_.size()) + " sections (" + 
                    std::to_string(bytesWritten_) + " bytes)");
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception finalizing checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::abort() {
    CheckpointReader::abort();
}

bool CheckpointWriter::writeHeader() {
    return true; // Handled by create()
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    return true; // Handled by writeSection()
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = bytesWritten_ % alignment;
    if (currentPos == 0) return 0;
    
    size_t padding = alignment - currentPos;
    std::vector<uint8_t> padBuffer(padding, 0);
    
    // Note: We don't actually write padding here since writeSection handles it
    return padding;
}

} // namespace nlm