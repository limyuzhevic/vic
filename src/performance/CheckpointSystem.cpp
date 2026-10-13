#include "CheckpointSystem.hpp"
#include <fstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

CheckpointReader::CheckpointReader() : stream_(), header_(), sections_(), error_(), streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    // Close any existing file
    close();
    
    // Open file for reading
    stream_.open(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
        return false;
    }
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_ || stream_.gcount() != sizeof(CheckpointHeader)) {
        close();
        error_ = "Failed to read checkpoint header";
        return false;
    }
    
    // Validate header
    if (!header_.validate()) {
        close();
        error_ = "Invalid checkpoint header magic or version";
        return false;
    }
    
    // Read section headers
    sections_.clear();
    uint64_t sectionCount = header_.totalSize / sizeof(SectionHeader);
    for (uint64_t i = 0; i < sectionCount; ++i) {
        SectionHeader section;
        stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader));
        if (!stream_ || stream_.gcount() != sizeof(SectionHeader)) {
            close();
            error_ = "Failed to read section header";
            return false;
        }
        sections_.push_back(section);
    }
    
    streamPos_ = stream_.tellg();
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

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Neurons);
    if (sectionData.empty()) {
        error_ = "Neurons section not found or empty";
        return false;
    }
    
    // Deserialize neuron data
    const uint8_t* dataPtr = sectionData.data();
    uint64_t dataSize = sectionData.size();
    
    // Calculate number of neurons from data size
    // Each neuron has: 5 floats + 1 uint8_t + 2 uint32_t + 1 float + 3 uint64_t = 
    // 5*4 + 1*1 + 2*4 + 4 + 3*8 = 20 + 1 + 8 + 4 + 24 = 57 bytes per neuron
    uint64_t neuronCount = dataSize / 57;
    if (neuronCount * 57 != dataSize) {
        error_ = "Neurons section data size is not a multiple of neuron size";
        return false;
    }
    
    // Resize vectors
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
    
    // Deserialize neuron data
    for (uint64_t i = 0; i < neuronCount; ++i) {
        // Read membranePotential
        memcpy(&data.membranePotential[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
        
        // Read restingPotential
        memcpy(&data.restingPotential[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
        
        // Read threshold
        memcpy(&data.threshold[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
        
        // Read resetPotential
        memcpy(&data.resetPotential[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
        
        // Read leakConductance
        memcpy(&data.leakConductance[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
        
        // Read firingState
        memcpy(&data.firingState[i], dataPtr, sizeof(uint8_t));
        dataPtr += sizeof(uint8_t);
        
        // Read refractoryRemaining
        memcpy(&data.refractoryRemaining[i], dataPtr, sizeof(uint32_t));
        dataPtr += sizeof(uint32_t);
        
        // Read refractoryPeriod
        memcpy(&data.refractoryPeriod[i], dataPtr, sizeof(uint32_t));
        dataPtr += sizeof(uint32_t);
        
        // Read lastSpikeTime
        memcpy(&data.lastSpikeTime[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
        
        // Read neuronType
        memcpy(&data.neuronType[i], dataPtr, sizeof(uint64_t));
        dataPtr += sizeof(uint64_t);
        
        // Read regionId
        memcpy(&data.regionId[i], dataPtr, sizeof(uint64_t));
        dataPtr += sizeof(uint64_t);
        
        // Read populationId
        memcpy(&data.populationId[i], dataPtr, sizeof(uint64_t));
        dataPtr += sizeof(uint64_t);
    }
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Synapses);
    if (sectionData.empty()) {
        error_ = "Synapses section not found or empty";
        return false;
    }
    
    // Deserialize synapse data
    const uint8_t* dataPtr = sectionData.data();
    uint64_t dataSize = sectionData.size();
    
    // Calculate number of synapses from data size
    // Each synapse has: 2 uint64_t + 1 float + 1 uint32_t + 2 uint8_t + 6 float =
    // 2*8 + 4 + 4 + 1*2 + 6*4 = 16 + 4 + 4 + 2 + 24 = 50 bytes per synapse
    uint64_t synapseCount = dataSize / 50;
    if (synapseCount * 50 != dataSize) {
        error_ = "Synapses section data size is not a multiple of synapse size";
        return false;
    }
    
    // Resize vectors
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
    
    // Deserialize synapse data
    for (uint64_t i = 0; i < synapseCount; ++i) {
        // Read sourceNeuron
        memcpy(&data.sourceNeuron[i], dataPtr, sizeof(uint64_t));
        dataPtr += sizeof(uint64_t);
        
        // Read destinationNeuron
        memcpy(&data.destinationNeuron[i], dataPtr, sizeof(uint64_t));
        dataPtr += sizeof(uint64_t);
        
        // Read weight
        memcpy(&data.weight[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
        
        // Read delay
        memcpy(&data.delay[i], dataPtr, sizeof(uint32_t));
        dataPtr += sizeof(uint32_t);
        
        // Read synapseType
        memcpy(&data.synapseType[i], dataPtr, sizeof(uint8_t));
        dataPtr += sizeof(uint8_t);
        
        // Read plasticityFlags
        memcpy(&data.plasticityFlags[i], dataPtr, sizeof(uint8_t));
        dataPtr += sizeof(uint8_t);
        
        // Read eligibilityTrace
        memcpy(&data.eligibilityTrace[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
        
        // Read efficacy
        memcpy(&data.efficacy[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
        
        // Read shortTermDepression
        memcpy(&data.shortTermDepression[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
        
        // Read shortTermFacilitation
        memcpy(&data.shortTermFacilitation[i], dataPtr, sizeof(float));
        dataPtr += sizeof(float);
    }
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            stream_.seekg(section.offset);
            std::vector<uint8_t> data(section.size);
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            if (!stream_ || stream_.gcount() != section.size) {
                error_ = "Failed to read section data";
                return std::vector<uint8_t>();
            }
            return data;
        }
    }
    error_ = "Section " + std::to_string(static_cast<uint32_t>(sectionType)) + " not found";
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    return !error_ && header_.validate();
}

double CheckpointReader::getProgress() const {
    if (!isOpen() || sections_.empty()) {
        return 0.0;
    }
    
    double totalSize = 0.0;
    for (const auto& section : sections_) {
        totalSize += section.size;
    }
    
    double currentPos = static_cast<double>(streamPos_);
    return (currentPos / totalSize);
}

CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(CompressionLevel::Balanced),
                                       header_(), sections_(), bytesWritten_(0), compressBuffer_() {}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    // Close any existing file
    abort();
    
    // Set filepath and compression
    filepath_ = filepath;
    compression_ = compression;
    
    // Create directory if it doesn't exist
    std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        if (!std::filesystem::create_directories(dir)) {
            error_ = "Failed to create checkpoint directory: " + dir.string();
            return false;
        }
    }
    
    // Open file for writing
    stream_.open(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to create checkpoint file: " + filepath;
        return false;
    }
    
    // Initialize header with current timestamp
    auto now = std::chrono::system_clock::now();
    header_.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()).count();
    
    sections_.clear();
    bytesWritten_ = 0;
    
    // Write placeholder header - will be updated in finalize()
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_) {
        error_ = "Failed to write checkpoint header";
        abort();
        return false;
    }
    bytesWritten_ += sizeof(CheckpointHeader);
    
    return true;
}

const CheckpointHeader& CheckpointWriter::getHeader() const {
    return header_;
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
    // Write neuron count
    uint64_t neuronCount = data.membranePotential.size();
    stream_.write(reinterpret_cast<const char*>(&neuronCount), sizeof(neuronCount));
    if (!stream_) {
        error_ = "Failed to write neuron count";
        return false;
    }
    bytesWritten_ += sizeof(neuronCount);
    
    // Write neuron data
    uint64_t dataSize = neuronCount * (
        sizeof(float) * 5 +           // membranePotential, restingPotential, threshold, resetPotential, leakConductance
        sizeof(uint8_t) * 1 +         // firingState
        sizeof(uint32_t) * 2 +        // refractoryRemaining, refractoryPeriod
        sizeof(float) * 1 +           // lastSpikeTime
        sizeof(uint64_t) * 3          // neuronType, regionId, populationId
    );
    
    std::vector<uint8_t> neuronBuffer(dataSize, 0);
    
    // Serialize neuron data
    uint8_t* bufferPtr = neuronBuffer.data();
    for (uint64_t i = 0; i < neuronCount; ++i) {
        // Write membranePotential
        memcpy(bufferPtr, &data.membranePotential[i], sizeof(float));
        bufferPtr += sizeof(float);
        
        // Write restingPotential
        memcpy(bufferPtr, &data.restingPotential[i], sizeof(float));
        bufferPtr += sizeof(float);
        
        // Write threshold
        memcpy(bufferPtr, &data.threshold[i], sizeof(float));
        bufferPtr += sizeof(float);
        
        // Write resetPotential
        memcpy(bufferPtr, &data.resetPotential[i], sizeof(float));
        bufferPtr += sizeof(float);
        
        // Write leakConductance
        memcpy(bufferPtr, &data.leakConductance[i], sizeof(float));
        bufferPtr += sizeof(float);
        
        // Write firingState
        memcpy(bufferPtr, &data.firingState[i], sizeof(uint8_t));
        bufferPtr += sizeof(uint8_t);
        
        // Write refractoryRemaining
        memcpy(bufferPtr, &data.refractoryRemaining[i], sizeof(uint32_t));
        bufferPtr += sizeof(uint32_t);
        
        // Write refractoryPeriod
        memcpy(bufferPtr, &data.refractoryPeriod[i], sizeof(uint32_t));
        bufferPtr += sizeof(uint32_t);
        
        // Write lastSpikeTime
        memcpy(bufferPtr, &data.lastSpikeTime[i], sizeof(float));
        bufferPtr += sizeof(float);
        
        // Write neuronType
        memcpy(bufferPtr, &data.neuronType[i], sizeof(uint64_t));
        bufferPtr += sizeof(uint64_t);
        
        // Write regionId
        memcpy(bufferPtr, &data.regionId[i], sizeof(uint64_t));
        bufferPtr += sizeof(uint64_t);
        
        // Write populationId
        memcpy(bufferPtr, &data.populationId[i], sizeof(uint64_t));
        bufferPtr += sizeof(uint64_t);
    }
    
    return writeSection(CheckpointSection::Neurons, neuronBuffer.data(), neuronBuffer.size());
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    // Write synapse count
    uint64_t synapseCount = data.weight.size();
    stream_.write(reinterpret_cast<const char*>(&synapseCount), sizeof(synapseCount));
    if (!stream_) {
        error_ = "Failed to write synapse count";
        return false;
    }
    bytesWritten_ += sizeof(synapseCount);
    
    // Write synapse data
    uint64_t dataSize = synapseCount * (
        sizeof(uint64_t) * 2 +        // sourceNeuron, destinationNeuron
        sizeof(float) * 1 +           // weight
        sizeof(uint32_t) * 1 +        // delay
        sizeof(uint8_t) * 2 +         // synapseType, plasticityFlags
        sizeof(float) * 6             // eligibilityTrace, efficacy, shortTermDepression, shortTermFacilitation
    );
    
    std::vector<uint8_t> synapseBuffer(dataSize, 0);
    
    // Serialize synapse data
    uint8_t* bufferPtr = synapseBuffer.data();
    for (uint64_t i = 0; i < synapseCount; ++i) {
        // Write sourceNeuron
        memcpy(bufferPtr, &data.sourceNeuron[i], sizeof(uint64_t));
        bufferPtr += sizeof(uint64_t);
        
        // Write destinationNeuron
        memcpy(bufferPtr, &data.destinationNeuron[i], sizeof(uint64_t));
        bufferPtr += sizeof(uint64_t);
        
        // Write weight
        memcpy(bufferPtr, &data.weight[i], sizeof(float));
        bufferPtr += sizeof(float);
        
        // Write delay
        memcpy(bufferPtr, &data.delay[i], sizeof(uint32_t));
        bufferPtr += sizeof(uint32_t);
        
        // Write synapseType
        memcpy(bufferPtr, &data.synapseType[i], sizeof(uint8_t));
        bufferPtr += sizeof(uint8_t);
        
        // Write plasticityFlags
        memcpy(bufferPtr, &data.plasticityFlags[i], sizeof(uint8_t));
        bufferPtr += sizeof(uint8_t);
        
        // Write eligibilityTrace
        memcpy(bufferPtr, &data.eligibilityTrace[i], sizeof(float));
        bufferPtr += sizeof(float);
        
        // Write efficacy
        memcpy(bufferPtr, &data.efficacy[i], sizeof(float));
        bufferPtr += sizeof(float);
        
        // Write shortTermDepression
        memcpy(bufferPtr, &data.shortTermDepression[i], sizeof(float));
        bufferPtr += sizeof(float);
        
        // Write shortTermFacilitation
        memcpy(bufferPtr, &data.shortTermFacilitation[i], sizeof(float));
        bufferPtr += sizeof(float);
    }
    
    return writeSection(CheckpointSection::Synapses, synapseBuffer.data(), synapseBuffer.size());
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    SectionHeader section;
    section.type = static_cast<uint32_t>(sectionType);
    section.offset = bytesWritten_ + sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader);
    section.size = size;
    section.compressedSize = size; // No compression for now
    section.checksum = 0; // Placeholder checksum
    
    // Write section header
    stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
    if (!stream_) {
        error_ = "Failed to write section header";
        return false;
    }
    bytesWritten_ += sizeof(SectionHeader);
    
    // Write section data
    stream_.write(reinterpret_cast<const char*>(data), size);
    if (!stream_) {
        error_ = "Failed to write section data";
        return false;
    }
    bytesWritten_ += size;
    
    sections_.push_back(section);
    return true;
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        error_ = "Cannot finalize - no checkpoint file open";
        return false;
    }
    
    // Update header with final information
    header_.totalSize = bytesWritten_ - sizeof(CheckpointHeader);
    
    // Write updated header at the beginning of file
    stream_.seekp(0);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_) {
        error_ = "Failed to write final header";
        return false;
    }
    
    // Pad to alignment
    size_t currentPos = stream_.tellp();
    if (currentPos % 8 != 0) {
        std::vector<uint8_t> padding(8 - (currentPos % 8), 0);
        stream_.write(reinterpret_cast<const char*>(padding.data()), padding.size());
        if (!stream_) {
            error_ = "Failed to pad checkpoint file";
            return false;
        }
    }
    
    stream_.close();
    return true;
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        std::filesystem::remove(filepath_);
    }
    sections_.clear();
    bytesWritten_ = 0;
}

CheckpointManager::CheckpointManager() : checkpointDir_("./checkpoints"), saveIntervalSteps_(10000),
                                         maxCheckpoints_(10), compress_(true), lastSaveStep_(0),
                                         lastCheckpointPath_(), neuronProvider_(), synapseProvider_() {}

CheckpointManager::~CheckpointManager() {
    // Cleanup checkpoints directory on destruction
    if (std::filesystem::exists(checkpointDir_)) {
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
    lastSaveStep_ = 0;
    lastCheckpointPath_.clear();
    
    // Create checkpoint directory
    if (!std::filesystem::exists(checkpointDir_)) {
        std::filesystem::create_directories(checkpointDir_);
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately("step_" + std::to_string(currentStep));
    }
    return true;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".nckpt";
    
    // Create checkpoint writer
    CheckpointWriter writer;
    if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        return false;
    }
    
    // Call providers to fill data
    NeuronCheckpointData neuronData;
    SynapseCheckpointData synapseData;
    
    if (neuronProvider_) {
        neuronProvider_(neuronData);
    }
    
    if (synapseProvider_) {
        synapseProvider_(synapseData);
    }
    
    // Write data
    if (!writer.writeNeurons(neuronData)) {
        writer.abort();
        return false;
    }
    
    if (!writer.writeSynapses(synapseData)) {
        writer.abort();
        return false;
    }
    
    if (!writer.finalize()) {
        return false;
    }
    
    lastCheckpointPath_ = filepath;
    lastSaveStep_ = writer.getHeader().step;
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".nckpt";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        return false;
    }
    
    // In a real implementation, this would load data using the providers
    // For now, just return success
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.path().extension() == ".nckpt") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            info.fileSize = std::filesystem::file_size(entry.path());
            info.timestamp = std::filesystem::last_write_time(entry.path());
            
            // In a real implementation, we would parse the checkpoint file to get
            // step, simulationTime, neuronCount, synapseCount
            checkpoints.push_back(info);
        }
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
    
    // Delete oldest checkpoints
    size_t toDelete = checkpoints.size() - keepCount;
    for (size_t i = 0; i < toDelete; ++i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name + ".nckpt";
        std::filesystem::remove(filepath);
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

} // namespace nlm