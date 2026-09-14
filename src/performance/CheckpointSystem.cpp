// CheckpointSystem.cpp - Implementation of CheckpointSystem

#include "CheckpointSystem.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

CheckpointReader::CheckpointReader() : stream_(), header_(), streamPos_(0) {}

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
        if (stream_.fail()) {
            error_ = "Failed to read checkpoint header";
            stream_.close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header";
            stream_.close();
            return false;
        }
        
        // Read section headers
        sections_.clear();
        for (uint32_t i = 0; i < static_cast<uint32_t>(CheckpointSection::End); ++i) {
            SectionHeader sectionHeader;
            stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(SectionHeader));
            if (stream_.fail() && !stream_.eof()) {
                error_ = "Failed to read section header";
                stream_.close();
                return false;
            }
            
            if (sectionHeader.type == static_cast<uint32_t>(CheckpointSection::End)) {
                break;
            }
            
            sections_.push_back(sectionHeader);
        }
        
        streamPos_ = stream_.tellg();
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception opening checkpoint: " + std::string(e.what());
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
    stream_.clear();
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    data = NeuronCheckpointData();
    
    try {
        // Find neurons section
        for (const auto& section : sections_) {
            if (section.type == static_cast<uint32_t>(CheckpointSection::Neurons)) {
                std::vector<uint8_t> sectionData(section.size, 0);
                stream_.seekg(section.offset);
                stream_.read(reinterpret_cast<char*>(sectionData.data()), section.size);
                if (stream_.fail()) {
                    error_ = "Failed to read neurons section data";
                    return false;
                }
                
                // Deserialize neurons
                size_t neuronCount = data.membranePotential.size();
                if (neuronCount > 0) {
                    size_t expectedSize = neuronCount * sizeof(float);
                    
                    // Read membrane potentials
                    for (size_t i = 0; i < neuronCount && i * sizeof(float) < section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + i * sizeof(float), sizeof(float));
                        data.membranePotential.push_back(val);
                    }
                    
                    // Read resting potentials
                    for (size_t i = 0; i < neuronCount && i * sizeof(float) + expectedSize < section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + expectedSize + i * sizeof(float), sizeof(float));
                        data.restingPotential.push_back(val);
                    }
                    
                    // Read thresholds
                    for (size_t i = 0; i < neuronCount && i * sizeof(float) + 2 * expectedSize < section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + 2 * expectedSize + i * sizeof(float), sizeof(float));
                        data.threshold.push_back(val);
                    }
                    
                    // Read reset potentials
                    for (size_t i = 0; i < neuronCount && i * sizeof(float) + 3 * expectedSize < section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + 3 * expectedSize + i * sizeof(float), sizeof(float));
                        data.resetPotential.push_back(val);
                    }
                    
                    // Read leak conductances
                    for (size_t i = 0; i < neuronCount && i * sizeof(float) + 4 * expectedSize < section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + 4 * expectedSize + i * sizeof(float), sizeof(float));
                        data.leakConductance.push_back(val);
                    }
                    
                    // Read firing states
                    for (size_t i = 0; i < neuronCount && i * sizeof(uint8_t) + 5 * expectedSize < section.size; ++i) {
                        uint8_t val;
                        std::memcpy(&val, sectionData.data() + 5 * expectedSize + i, sizeof(uint8_t));
                        data.firingState.push_back(val);
                    }
                    
                    // Read refractory remaining
                    for (size_t i = 0; i < neuronCount && i * sizeof(uint32_t) + 5.125 * expectedSize < section.size; ++i) {
                        uint32_t val;
                        std::memcpy(&val, sectionData.data() + 5.125 * expectedSize + i * sizeof(uint32_t), sizeof(uint32_t));
                        data.refractoryRemaining.push_back(val);
                    }
                    
                    // Read refractory periods
                    for (size_t i = 0; i < neuronCount && i * sizeof(uint32_t) + 5.25 * expectedSize < section.size; ++i) {
                        uint32_t val;
                        std::memcpy(&val, sectionData.data() + 5.25 * expectedSize + i * sizeof(uint32_t), sizeof(uint32_t));
                        data.refractoryPeriod.push_back(val);
                    }
                    
                    // Read last spike times
                    for (size_t i = 0; i < neuronCount && i * sizeof(float) + 5.5 * expectedSize < section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + 5.5 * expectedSize + i * sizeof(float), sizeof(float));
                        data.lastSpikeTime.push_back(val);
                    }
                }
                
                return true;
            }
        }
        
        error_ = "Neurons section not found";
        return false;
        
    } catch (const std::exception& e) {
        error_ = "Exception reading neurons: " + std::string(e.what());
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    data = SynapseCheckpointData();
    
    try {
        // Find synapses section
        for (const auto& section : sections_) {
            if (section.type == static_cast<uint32_t>(CheckpointSection::Synapses)) {
                std::vector<uint8_t> sectionData(section.size, 0);
                stream_.seekg(section.offset);
                stream_.read(reinterpret_cast<char*>(sectionData.data()), section.size);
                if (stream_.fail()) {
                    error_ = "Failed to read synapses section data";
                    return false;
                }
                
                // Deserialize synapses (simplified version)
                size_t synapseCount = data.weight.size();
                if (synapseCount > 0) {
                    size_t floatSize = sizeof(float);
                    size_t uint64Size = sizeof(uint64_t);
                    size_t uint32Size = sizeof(uint32_t);
                    size_t uint8Size = sizeof(uint8_t);
                    
                    size_t offset = 0;
                    
                    // Read source neurons
                    for (size_t i = 0; i < synapseCount && offset + uint64Size <= section.size; ++i) {
                        uint64_t val;
                        std::memcpy(&val, sectionData.data() + offset, uint64Size);
                        data.sourceNeuron.push_back(val);
                        offset += uint64Size;
                    }
                    
                    // Read destination neurons
                    for (size_t i = 0; i < synapseCount && offset + uint64Size <= section.size; ++i) {
                        uint64_t val;
                        std::memcpy(&val, sectionData.data() + offset, uint64Size);
                        data.destinationNeuron.push_back(val);
                        offset += uint64Size;
                    }
                    
                    // Read weights
                    for (size_t i = 0; i < synapseCount && offset + floatSize <= section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + offset, floatSize);
                        data.weight.push_back(val);
                        offset += floatSize;
                    }
                    
                    // Read delays
                    for (size_t i = 0; i < synapseCount && offset + uint32Size <= section.size; ++i) {
                        uint32_t val;
                        std::memcpy(&val, sectionData.data() + offset, uint32Size);
                        data.delay.push_back(val);
                        offset += uint32Size;
                    }
                    
                    // Read synapse types
                    for (size_t i = 0; i < synapseCount && offset + uint8Size <= section.size; ++i) {
                        uint8_t val;
                        std::memcpy(&val, sectionData.data() + offset, uint8Size);
                        data.synapseType.push_back(val);
                        offset += uint8Size;
                    }
                    
                    // Read plasticity flags
                    for (size_t i = 0; i < synapseCount && offset + uint8Size <= section.size; ++i) {
                        uint8_t val;
                        std::memcpy(&val, sectionData.data() + offset, uint8Size);
                        data.plasticityFlags.push_back(val);
                        offset += uint8Size;
                    }
                    
                    // Read eligibility traces
                    for (size_t i = 0; i < synapseCount && offset + floatSize <= section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + offset, floatSize);
                        data.eligibilityTrace.push_back(val);
                        offset += floatSize;
                    }
                    
                    // Read efficacies
                    for (size_t i = 0; i < synapseCount && offset + floatSize <= section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + offset, floatSize);
                        data.efficacy.push_back(val);
                        offset += floatSize;
                    }
                    
                    // Read short-term depression
                    for (size_t i = 0; i < synapseCount && offset + floatSize <= section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + offset, floatSize);
                        data.shortTermDepression.push_back(val);
                        offset += floatSize;
                    }
                    
                    // Read short-term facilitation
                    for (size_t i = 0; i < synapseCount && offset + floatSize <= section.size; ++i) {
                        float val;
                        std::memcpy(&val, sectionData.data() + offset, floatSize);
                        data.shortTermFacilitation.push_back(val);
                        offset += floatSize;
                    }
                }
                
                return true;
            }
        }
        
        error_ = "Synapses section not found";
        return false;
        
    } catch (const std::exception& e) {
        error_ = "Exception reading synapses: " + std::string(e.what());
        return false;
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> result;
    
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            result.resize(section.size);
            stream_.seekg(section.offset);
            stream_.read(reinterpret_cast<char*>(result.data()), section.size);
            break;
        }
    }
    
    return result;
}

bool CheckpointReader::validate() const {
    // Basic validation - check header magic and version
    return header_.validate();
}

double CheckpointReader::getProgress() const {
    // For now, return a simple progress estimate
    if (sections_.empty()) return 0.0;
    
    // Read from beginning to current position
    size_t current = streamPos_;
    size_t total = 0;
    
    for (const auto& section : sections_) {
        total += section.offset + section.size;
    }
    
    if (total == 0) return 1.0;
    
    return static_cast<double>(current) / static_cast<double>(total);
}

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Close if already open
        close();
        
        // Create directory if it doesn't exist
        std::filesystem::path path(filepath);
        std::filesystem::create_directories(path.parent_path());
        
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            return false;
        }
        
        filepath_ = filepath;
        compression_ = compression;
        bytesWritten_ = 0;
        
        return writeHeader();
        
    } catch (const std::exception& e) {
        error_ = "Exception creating checkpoint: " + std::string(e.what());
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        // Write footer
        uint32_t endMarker = static_cast<uint32_t>(CheckpointSection::End);
        stream_.write(reinterpret_cast<const char*>(&endMarker), sizeof(endMarker));
        
        // Update header with total size
        if (bytesWritten_ > 0) {
            header_.totalSize = bytesWritten_;
            
            stream_.seekp(0);
            stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        }
        
        stream_.close();
        stream_.clear();
    }
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                 uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    
    // Set timestamp
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    header_.timestamp = static_cast<uint64_t>(now_time_t);
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        // Serialize neurons
        std::vector<uint8_t> neuronData;
        
        // Write all neuron properties
        for (float val : data.membranePotential) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            neuronData.insert(neuronData.end(), bytes.begin(), bytes.end());
        }
        
        for (float val : data.restingPotential) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            neuronData.insert(neuronData.end(), bytes.begin(), bytes.end());
        }
        
        for (float val : data.threshold) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            neuronData.insert(neuronData.end(), bytes.begin(), bytes.end());
        }
        
        for (float val : data.resetPotential) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            neuronData.insert(neuronData.end(), bytes.begin(), bytes.end());
        }
        
        for (float val : data.leakConductance) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            neuronData.insert(neuronData.end(), bytes.begin(), bytes.end());
        }
        
        for (uint8_t val : data.firingState) {
            std::vector<uint8_t> bytes(sizeof(uint8_t));
            std::memcpy(bytes.data(), &val, sizeof(uint8_t));
            neuronData.insert(neuronData.end(), bytes.begin(), bytes.end());
        }
        
        for (uint32_t val : data.refractoryRemaining) {
            std::vector<uint8_t> bytes(sizeof(uint32_t));
            std::memcpy(bytes.data(), &val, sizeof(uint32_t));
            neuronData.insert(neuronData.end(), bytes.begin(), bytes.end());
        }
        
        for (uint32_t val : data.refractoryPeriod) {
            std::vector<uint8_t> bytes(sizeof(uint32_t));
            std::memcpy(bytes.data(), &val, sizeof(uint32_t));
            neuronData.insert(neuronData.end(), bytes.begin(), bytes.end());
        }
        
        for (float val : data.lastSpikeTime) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            neuronData.insert(neuronData.end(), bytes.begin(), bytes.end());
        }
        
        // Write section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(CheckpointSection::Neurons);
        sectionHeader.offset = bytesWritten_;
        sectionHeader.size = neuronData.size();
        sectionHeader.compressedSize = neuronData.size();
        
        if (!writeSectionHeader(sectionHeader)) {
            return false;
        }
        
        // Write data
        stream_.write(reinterpret_cast<const char*>(neuronData.data()), neuronData.size());
        if (stream_.fail()) {
            error_ = "Failed to write neurons data";
            return false;
        }
        
        bytesWritten_ += neuronData.size();
        sections_.push_back(sectionHeader);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception writing neurons: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Serialize synapses
        std::vector<uint8_t> synapseData;
        
        // Write all synapse properties
        for (uint64_t val : data.sourceNeuron) {
            std::vector<uint8_t> bytes(sizeof(uint64_t));
            std::memcpy(bytes.data(), &val, sizeof(uint64_t));
            synapseData.insert(synapseData.end(), bytes.begin(), bytes.end());
        }
        
        for (uint64_t val : data.destinationNeuron) {
            std::vector<uint8_t> bytes(sizeof(uint64_t));
            std::memcpy(bytes.data(), &val, sizeof(uint64_t));
            synapseData.insert(synapseData.end(), bytes.begin(), bytes.end());
        }
        
        for (float val : data.weight) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            synapseData.insert(synapseData.end(), bytes.begin(), bytes.end());
        }
        
        for (uint32_t val : data.delay) {
            std::vector<uint8_t> bytes(sizeof(uint32_t));
            std::memcpy(bytes.data(), &val, sizeof(uint32_t));
            synapseData.insert(synapseData.end(), bytes.begin(), bytes.end());
        }
        
        for (uint8_t val : data.synapseType) {
            std::vector<uint8_t> bytes(sizeof(uint8_t));
            std::memcpy(bytes.data(), &val, sizeof(uint8_t));
            synapseData.insert(synapseData.end(), bytes.begin(), bytes.end());
        }
        
        for (uint8_t val : data.plasticityFlags) {
            std::vector<uint8_t> bytes(sizeof(uint8_t));
            std::memcpy(bytes.data(), &val, sizeof(uint8_t));
            synapseData.insert(synapseData.end(), bytes.begin(), bytes.end());
        }
        
        for (float val : data.eligibilityTrace) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            synapseData.insert(synapseData.end(), bytes.begin(), bytes.end());
        }
        
        for (float val : data.efficacy) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            synapseData.insert(synapseData.end(), bytes.begin(), bytes.end());
        }
        
        for (float val : data.shortTermDepression) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            synapseData.insert(synapseData.end(), bytes.begin(), bytes.end());
        }
        
        for (float val : data.shortTermFacilitation) {
            std::vector<uint8_t> bytes(sizeof(float));
            std::memcpy(bytes.data(), &val, sizeof(float));
            synapseData.insert(synapseData.end(), bytes.begin(), bytes.end());
        }
        
        // Write section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(CheckpointSection::Synapses);
        sectionHeader.offset = bytesWritten_;
        sectionHeader.size = synapseData.size();
        sectionHeader.compressedSize = synapseData.size();
        
        if (!writeSectionHeader(sectionHeader)) {
            return false;
        }
        
        // Write data
        stream_.write(reinterpret_cast<const char*>(synapseData.data()), synapseData.size());
        if (stream_.fail()) {
            error_ = "Failed to write synapses data";
            return false;
        }
        
        bytesWritten_ += synapseData.size();
        sections_.push_back(sectionHeader);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception writing synapses: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    // Find existing section
    for (auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            section.size = size;
            section.compressedSize = size;
            
            stream_.seekp(section.offset);
            stream_.write(reinterpret_cast<const char*>(data), size);
            if (stream_.fail()) {
                error_ = "Failed to write section data";
                return false;
            }
            
            return true;
        }
    }
    
    // Create new section
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(sectionType);
    sectionHeader.offset = bytesWritten_;
    sectionHeader.size = size;
    sectionHeader.compressedSize = size;
    
    if (!writeSectionHeader(sectionHeader)) {
        return false;
    }
    
    stream_.write(reinterpret_cast<const char*>(data), size);
    if (stream_.fail()) {
        error_ = "Failed to write section data";
        return false;
    }
    
    bytesWritten_ += size;
    sections_.push_back(sectionHeader);
    
    return true;
}

bool CheckpointWriter::finalize() {
    // Write footer and update header
    close();
    return !error_.empty();
}

void CheckpointWriter::abort() {
    close();
    std::filesystem::remove(filepath_);
}

bool CheckpointWriter::writeHeader() {
    // Set header timestamp
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    header_.timestamp = static_cast<uint64_t>(now_time_t);
    
    // Write header
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    if (stream_.fail()) {
        error_ = "Failed to write checkpoint header";
        return false;
    }
    
    bytesWritten_ += sizeof(CheckpointHeader);
    
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    if (stream_.fail()) {
        error_ = "Failed to write section header";
        return false;
    }
    
    bytesWritten_ += sizeof(SectionHeader);
    
    return true;
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = bytesWritten_;
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> paddingBytes(padding, 0);
        stream_.write(reinterpret_cast<const char*>(paddingBytes.data()), padding);
        if (stream_.fail()) {
            return false;
        }
        bytesWritten_ += padding;
    }
    
    return true;
}

CheckpointManager::CheckpointManager() : lastSaveStep_(0) {}

CheckpointManager::~CheckpointManager() {}

void CheckpointManager::configure(const std::string& checkpointDir,
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(checkpointDir_);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    // Auto-save if it's time
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    // For now, just call the neuron and synapse providers
    // In a full implementation, this would create a checkpoint file
    // with all the registered providers
    
    std::string checkpointName = name.empty() ? "brain_checkpoint" : name;
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".bin";
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath)) {
            return false;
        }
        
        // Use providers to write data
        if (neuronProvider_) {
            NeuronCheckpointData neuronData;
            if (!neuronProvider_(neuronData)) {
                writer.abort();
                return false;
            }
            writer.writeNeurons(neuronData);
        }
        
        if (synapseProvider_) {
            SynapseCheckpointData synapseData;
            if (!synapseProvider_(synapseData)) {
                writer.abort();
                return false;
            }
            writer.writeSynapses(synapseData);
        }
        
        writer.finalize();
        
        lastCheckpointPath_ = filepath;
        lastSaveStep_ = currentStep;
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string checkpointName = name.empty() ? "brain_checkpoint" : name;
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".bin";
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            return false;
        }
        
        if (!reader.validate()) {
            return false;
        }
        
        // In a full implementation, this would restore state using the providers
        // For now, just return success if the file can be opened and validated
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> result;
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".bin") {
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                
                // Parse file to get metadata
                CheckpointReader reader;
                if (reader.open(entry.path().string())) {
                    const auto& header = reader.getHeader();
                    info.step = header.step;
                    info.simulationTime = header.simulationTime;
                    info.neuronCount = header.neuronCount;
                    info.synapseCount = header.synapseCount;
                    info.fileSize = entry.file_size();
                    info.timestamp = header.timestamp;
                }
                
                result.push_back(info);
            }
        }
        
        // Sort by timestamp (newest first)
        std::sort(result.begin(), result.end(),
                  [](const CheckpointInfo& a, const CheckpointInfo& b) {
                      return a.timestamp > b.timestamp;
                  });
        
    } catch (const std::exception& e) {
        // Return empty list on error
    }
    
    return result;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Keep the most recent N checkpoints
    std::vector<std::string> toDelete;
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        toDelete.push_back(checkpointDir_ + "/" + checkpoints[i].name + ".bin");
    }
    
    for (const auto& filepath : toDelete) {
        try {
            std::filesystem::remove(filepath);
        } catch (...) {
            // Ignore errors
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= lastSaveStep_ + saveIntervalSteps_;
}

// Checksum implementations

inline uint64_t ChecksumCalculator::crc64(const void* data, size_t size) {
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
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
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

// Simple CRC-64 table (would normally be precomputed)
const uint64_t ChecksumCalculator::table[256] = {0};

// Placeholder functions for checkpoint providers
namespace nlm {

bool populateNeuronCheckpointData(NeuronCheckpointData& data) {
    // This function would be provided by the Brain class
    // For now, just return true
    return true;
}

bool populateSynapseCheckpointData(SynapseCheckpointData& data) {
    // This function would be provided by the Brain class
    // For now, just return true
    return true;
}

} // namespace nlm
