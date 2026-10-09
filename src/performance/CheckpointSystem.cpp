// CheckpointSystem.cpp implementation

#include "performance/CheckpointSystem.hpp"
#include <iostream>
#include <filesystem>
#include <sstream>
#include <algorithm>

namespace nlm {

CheckpointReader::CheckpointReader() : streamPos_(0) {}
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
            close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header magic number";
            close();
            return false;
        }
        
        // Read section headers
        sections_.reserve(header_.regionCount * 4);  // Estimate
        for (uint64_t i = 0; i < header_.regionCount; ++i) {
            SectionHeader section;
            stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader));
            if (!stream_.good()) {
                error_ = "Failed to read section header";
                close();
                return false;
            }
            sections_.push_back(section);
        }
        
        streamPos_ = stream_.tellg();
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception opening checkpoint: " + std::string(e.what());
        close();
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
        // Find neurons section
        for (const auto& section : sections_) {
            if (section.type == CheckpointSection::Neurons) {
                // Read data from current stream position
                stream_.seekg(streamPos_ + section.offset);
                
                // Read size first
                uint64_t size;
                stream_.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));
                if (!stream_.good()) return false;
                
                // Resize data vectors
                data.membranePotential.resize(size);
                data.restingPotential.resize(size);
                data.threshold.resize(size);
                data.resetPotential.resize(size);
                data.leakConductance.resize(size);
                data.firingState.resize(size);
                data.refractoryRemaining.resize(size);
                data.refractoryPeriod.resize(size);
                data.lastSpikeTime.resize(size);
                data.neuronType.resize(size);
                data.regionId.resize(size);
                data.populationId.resize(size);
                
                // Read the actual data
                for (size_t i = 0; i < size; ++i) {
                    stream_.read(reinterpret_cast<char*>(&data.membranePotential[i]), sizeof(float));
                    stream_.read(reinterpret_cast<char*>(&data.restingPotential[i]), sizeof(float));
                    stream_.read(reinterpret_cast<char*>(&data.threshold[i]), sizeof(float));
                    stream_.read(reinterpret_cast<char*>(&data.resetPotential[i]), sizeof(float));
                    stream_.read(reinterpret_cast<char*>(&data.leakConductance[i]), sizeof(float));
                    stream_.read(reinterpret_cast<char*>(&data.firingState[i]), sizeof(uint8_t));
                    stream_.read(reinterpret_cast<char*>(&data.refractoryRemaining[i]), sizeof(uint32_t));
                    stream_.read(reinterpret_cast<char*>(&data.refractoryPeriod[i]), sizeof(uint32_t));
                    stream_.read(reinterpret_cast<char*>(&data.lastSpikeTime[i]), sizeof(float));
                    stream_.read(reinterpret_cast<char*>(&data.neuronType[i]), sizeof(uint64_t));
                    stream_.read(reinterpret_cast<char*>(&data.regionId[i]), sizeof(uint64_t));
                    stream_.read(reinterpret_cast<char*>(&data.populationId[i]), sizeof(uint64_t));
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
    try {
        // Find synapses section
        for (const auto& section : sections_) {
            if (section.type == CheckpointSection::Synapses) {
                // Read data from current stream position
                stream_.seekg(streamPos_ + section.offset);
                
                // Read size first
                uint64_t size;
                stream_.read(reinterpret_cast<char*>(&size), sizeof(uint64_t));
                if (!stream_.good()) return false;
                
                // Resize data vectors
                data.sourceNeuron.resize(size);
                data.destinationNeuron.resize(size);
                data.weight.resize(size);
                data.delay.resize(size);
                data.synapseType.resize(size);
                data.plasticityFlags.resize(size);
                data.eligibilityTrace.resize(size);
                data.efficacy.resize(size);
                data.shortTermDepression.resize(size);
                data.shortTermFacilitation.resize(size);
                
                // Read the actual data
                for (size_t i = 0; i < size; ++i) {
                    stream_.read(reinterpret_cast<char*>(&data.sourceNeuron[i]), sizeof(uint64_t));
                    stream_.read(reinterpret_cast<char*>(&data.destinationNeuron[i]), sizeof(uint64_t));
                    stream_.read(reinterpret_cast<char*>(&data.weight[i]), sizeof(float));
                    stream_.read(reinterpret_cast<char*>(&data.delay[i]), sizeof(uint32_t));
                    stream_.read(reinterpret_cast<char*>(&data.synapseType[i]), sizeof(uint8_t));
                    stream_.read(reinterpret_cast<char*>(&data.plasticityFlags[i]), sizeof(uint8_t));
                    stream_.read(reinterpret_cast<char*>(&data.eligibilityTrace[i]), sizeof(float));
                    stream_.read(reinterpret_cast<char*>(&data.efficacy[i]), sizeof(float));
                    stream_.read(reinterpret_cast<char*>(&data.shortTermDepression[i]), sizeof(float));
                    stream_.read(reinterpret_cast<char*>(&data.shortTermFacilitation[i]), sizeof(float));
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
    try {
        for (const auto& section : sections_) {
            if (section.type == sectionType) {
                stream_.seekg(streamPos_ + section.offset);
                
                std::vector<uint8_t> data(section.size);
                stream_.read(reinterpret_cast<char*>(data.data()), section.size);
                
                if (!stream_.good()) {
                    error_ = "Failed to read section data";
                    return std::vector<uint8_t>();
                }
                
                return data;
            }
        }
        
        error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
        return std::vector<uint8_t>();
        
    } catch (const std::exception& e) {
        error_ = "Exception reading section: " + std::string(e.what());
        return std::vector<uint8_t>();
    }
}

bool CheckpointReader::validate() const {
    if (!stream_.is_open()) {
        error_ = "Checkpoint file is not open";
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Verify all sections are present
    std::vector<bool> sectionFound(static_cast<size_t>(CheckpointSection::End), false);
    for (const auto& section : sections_) {
        if (static_cast<size_t>(section.type) < sectionFound.size()) {
            sectionFound[static_cast<size_t>(section.type)] = true;
        }
    }
    
    // Check for required sections
    if (!sectionFound[static_cast<size_t>(CheckpointSection::Header)]) {
        error_ = "Missing header section";
        return false;
    }
    
    if (!sectionFound[static_cast<size_t>(CheckpointSection::Neurons)]) {
        error_ = "Missing neurons section";
        return false;
    }
    
    if (!sectionFound[static_cast<size_t>(CheckpointSection::Synapses)]) {
        error_ = "Missing synapses section";
        return false;
    }
    
    if (!sectionFound[static_cast<size_t>(CheckpointSection::Checksum)]) {
        error_ = "Missing checksum section";
        return false;
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (!stream_.is_open() || sections_.empty()) {
        return 0.0;
    }
    
    // Simple progress based on section reading
    return 0.5;  // Placeholder - could track actual reading progress
}

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}
CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    close();
    
    try {
        // Create parent directory if it doesn't exist
        std::filesystem::path path(filepath);
        if (!path.parent_path().empty()) {
            std::filesystem::create_directories(path.parent_path());
        }
        
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            return false;
        }
        
        compression_ = compression;
        filepath_ = filepath;
        
        // Initialize header (will be filled when finalized)
        header_ = CheckpointHeader{};
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception creating checkpoint: " + std::string(e.what());
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
    if (stream_.is_open()) {
        header_.neuronCount = neuronCount;
        header_.synapseCount = synapseCount;
        header_.regionCount = regionCount;
        header_.step = step;
        header_.simulationTime = simTime;
    }
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint not open";
            return false;
        }
        
        // Write section header
        SectionHeader section;
        section.type = CheckpointSection::Neurons;
        section.offset = static_cast<uint64_t>(stream_.tellp());
        section.size = sizeof(uint64_t) + // size
                      sizeof(float) * data.membranePotential.size() + // membranePotential
                      sizeof(float) * data.restingPotential.size() + // restingPotential
                      sizeof(float) * data.threshold.size() + // threshold
                      sizeof(float) * data.resetPotential.size() + // resetPotential
                      sizeof(float) * data.leakConductance.size() + // leakConductance
                      sizeof(uint8_t) * data.firingState.size() + // firingState
                      sizeof(uint32_t) * data.refractoryRemaining.size() + // refractoryRemaining
                      sizeof(uint32_t) * data.refractoryPeriod.size() + // refractoryPeriod
                      sizeof(float) * data.lastSpikeTime.size() + // lastSpikeTime
                      sizeof(uint64_t) * data.neuronType.size() + // neuronType
                      sizeof(uint64_t) * data.regionId.size() + // regionId
                      sizeof(uint64_t) * data.populationId.size(); // populationId
        
        stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
        if (!stream_.good()) {
            error_ = "Failed to write section header";
            return false;
        }
        
        // Write size first
        uint64_t size = data.membranePotential.size();
        stream_.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));
        
        // Write all data
        auto writeData = [&](const auto& vector) {
            stream_.write(reinterpret_cast<const char*>(vector.data()),
                         vector.size() * sizeof(decltype(vector)::value_type));
        };
        
        writeData(data.membranePotential);
        writeData(data.restingPotential);
        writeData(data.threshold);
        writeData(data.resetPotential);
        writeData(data.leakConductance);
        writeData(data.firingState);
        writeData(data.refractoryRemaining);
        writeData(data.refractoryPeriod);
        writeData(data.lastSpikeTime);
        writeData(data.neuronType);
        writeData(data.regionId);
        writeData(data.populationId);
        
        if (!stream_.good()) {
            error_ = "Failed to write neuron data";
            return false;
        }
        
        bytesWritten_ += sizeof(SectionHeader) + section.size;
        sections_.push_back(section);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception writing neurons: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint not open";
            return false;
        }
        
        // Write section header
        SectionHeader section;
        section.type = CheckpointSection::Synapses;
        section.offset = static_cast<uint64_t>(stream_.tellp());
        section.size = sizeof(uint64_t) + // size
                      sizeof(uint64_t) * data.sourceNeuron.size() + // sourceNeuron
                      sizeof(uint64_t) * data.destinationNeuron.size() + // destinationNeuron
                      sizeof(float) * data.weight.size() + // weight
                      sizeof(uint32_t) * data.delay.size() + // delay
                      sizeof(uint8_t) * data.synapseType.size() + // synapseType
                      sizeof(uint8_t) * data.plasticityFlags.size() + // plasticityFlags
                      sizeof(float) * data.eligibilityTrace.size() + // eligibilityTrace
                      sizeof(float) * data.efficacy.size() + // efficacy
                      sizeof(float) * data.shortTermDepression.size() + // shortTermDepression
                      sizeof(float) * data.shortTermFacilitation.size(); // shortTermFacilitation
        
        stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
        if (!stream_.good()) {
            error_ = "Failed to write section header";
            return false;
        }
        
        // Write size first
        uint64_t size = data.sourceNeuron.size();
        stream_.write(reinterpret_cast<const char*>(&size), sizeof(uint64_t));
        
        // Write all data
        auto writeData = [&](const auto& vector) {
            stream_.write(reinterpret_cast<const char*>(vector.data()),
                         vector.size() * sizeof(decltype(vector)::value_type));
        };
        
        writeData(data.sourceNeuron);
        writeData(data.destinationNeuron);
        writeData(data.weight);
        writeData(data.delay);
        writeData(data.synapseType);
        writeData(data.plasticityFlags);
        writeData(data.eligibilityTrace);
        writeData(data.efficacy);
        writeData(data.shortTermDepression);
        writeData(data.shortTermFacilitation);
        
        if (!stream_.good()) {
            error_ = "Failed to write synapse data";
            return false;
        }
        
        bytesWritten_ += sizeof(SectionHeader) + section.size;
        sections_.push_back(section);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception writing synapses: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint not open";
            return false;
        }
        
        SectionHeader section;
        section.type = sectionType;
        section.offset = static_cast<uint64_t>(stream_.tellp());
        section.size = size;
        section.compressedSize = 0;
        section.checksum = ChecksumCalculator::crc32(static_cast<const uint8_t*>(data), size);
        
        stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
        if (!stream_.good()) {
            error_ = "Failed to write section header";
            return false;
        }
        
        stream_.write(reinterpret_cast<const char*>(data), size);
        if (!stream_.good()) {
            error_ = "Failed to write section data";
            return false;
        }
        
        bytesWritten_ += sizeof(SectionHeader) + section.size;
        sections_.push_back(section);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception writing section: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint not open";
            return false;
        }
        
        // Calculate total size
        uint64_t totalSize = sizeof(CheckpointHeader) + // Header
                           sizeof(SectionHeader) * sections_.size() + // Section headers
                           bytesWritten_; // Data
        
        header_.totalSize = totalSize;
        header_.timestamp = static_cast<uint64_t>(std::time(nullptr));
        
        // Write header
        stream_.seekp(0);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        
        // Write section headers
        for (const auto& section : sections_) {
            stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
        }
        
        // Write checksum
        CheckpointSection checksumSection = CheckpointSection::Checksum;
        SectionHeader checksumHeader;
        checksumHeader.type = checksumSection;
        checksumHeader.offset = static_cast<uint64_t>(stream_.tellp());
        checksumHeader.size = sizeof(uint64_t);
        checksumHeader.compressedSize = 0;
        checksumHeader.checksum = ChecksumCalculator::crc64(
            reinterpret_cast<const void*>(&header_), sizeof(CheckpointHeader));
        
        stream_.write(reinterpret_cast<const char*>(&checksumHeader), sizeof(SectionHeader));
        stream_.write(reinterpret_cast<const char*>(&checksumHeader.checksum), sizeof(uint64_t));
        
        // Write End section marker
        SectionHeader endHeader;
        endHeader.type = CheckpointSection::End;
        endHeader.offset = 0;
        endHeader.size = 0;
        endHeader.compressedSize = 0;
        endHeader.checksum = 0;
        
        stream_.write(reinterpret_cast<const char*>(&endHeader), sizeof(SectionHeader));
        
        stream_.flush();
        
        if (!stream_.good()) {
            error_ = "Failed to write checkpoint footer";
            close();
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = "Exception finalizing checkpoint: " + std::string(e.what());
        close();
        return false;
    }
}

void CheckpointWriter::abort() {
    close();
}

CheckpointManager::CheckpointManager() : lastSaveStep_(0), lastCheckpointPath_("") {}
CheckpointManager::~CheckpointManager() {
    // Ensure directory is cleaned up if it exists
    if (!checkpointDir_.empty() && std::filesystem::exists(checkpointDir_)) {
        std::filesystem::remove_all(checkpointDir_);
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
    
    if (!checkpointDir_.empty() && !std::filesystem::exists(checkpointDir_)) {
        std::filesystem::create_directories(checkpointDir_);
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (saveIntervalSteps_ == 0) return true;
    
    if (currentStep - lastSaveStep_ >= saveIntervalSteps_) {
        return saveImmediately();
    }
    
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string checkpointName = name;
    if (checkpointName.empty()) {
        checkpointName = "checkpoint_" + std::to_string(lastSaveStep_);
    }
    
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".bin";
    
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        return false;
    }
    
    // Set metadata
    writer.setMetadata(100, 1000, 10, lastSaveStep_, currentTime);
    
    // Write dummy data (in real implementation, would get actual brain state)
    writer.writeNeurons(NeuronCheckpointData{});
    writer.writeSynapses(SynapseCheckpointData{});
    
    if (!writer.finalize()) {
        return false;
    }
    
    lastSaveStep_ = lastSaveStep_ + saveIntervalSteps_;
    lastCheckpointPath_ = filepath;
    
    // Prune old checkpoints
    pruneOldCheckpoints(maxCheckpoints_);
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".bin";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        error_ = reader.getError();
        return false;
    }
    
    // In real implementation, would restore brain state from checkpoint
    // For now, just read and validate
    reader.readNeurons(NeuronCheckpointData{});
    reader.readSynapses(SynapseCheckpointData{});
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            info.fileSize = entry.file_size();
            info.timestamp = std::filesystem::last_write_time(entry).time_since_epoch().count();
            
            // Extract metadata from filename or would parse checkpoint
            info.step = 0;
            info.simulationTime = 0.0;
            info.neuronCount = 0;
            info.synapseCount = 0;
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by modification time (most recent first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Remove oldest checkpoints
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name + ".bin";
        std::filesystem::remove(filepath);
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return (saveIntervalSteps_ > 0) && 
           ((currentStep - lastSaveStep_) >= saveIntervalSteps_);
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

void CheckpointManager::setNeuronProvider(std::function<bool(NeuronCheckpointData&)> provider) {
    neuronProvider_ = provider;
}

void CheckpointManager::setSynapseProvider(std::function<bool(SynapseCheckpointData&)> provider) {
    synapseProvider_ = provider;
}