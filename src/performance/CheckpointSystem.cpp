#include "CheckpointSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include <filesystem>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <ctime>

namespace nlm {

// CheckpointReader implementation
CheckpointReader::CheckpointReader() : stream_(), header_(), streamPos_(0) {}
CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        stream_.open(filepath, std::ios::binary | std::ios::in);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            NLM_LOG_ERROR(error_);
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_) {
            error_ = "Failed to read checkpoint header";
            close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header: wrong magic or version";
            close();
            return false;
        }
        
        // Read section headers
        sections_.clear();
        SectionHeader section;
        
        // Skip to first section offset
        stream_.seekg(sizeof(CheckpointHeader));
        
        while (stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader))) {
            if (section.type == CheckpointSection::End || section.type == CheckpointSection::Header) {
                break;
            }
            sections_.push_back(section);
        }
        
        if (error_.empty()) {
            NLM_LOG_INFO("CheckpointReader opened: " + filepath + ", " + 
                        std::to_string(sections_.size()) + " sections");
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
    streamPos_ = 0;
}

bool CheckpointReader::validate() const {
    if (!isOpen()) {
        error_ = "Checkpoint file is not open";
        return false;
    }
    
    uint64_t calculatedChecksum = 0;
    
    // Validate sections
    for (const auto& section : sections_) {
        stream_.seekg(section.offset);
        
        std::vector<uint8_t> buffer(section.size);
        stream_.read(reinterpret_cast<char*>(buffer.data()), section.size);
        
        if (!stream_) {
            error_ = "Failed to read section data";
            return false;
        }
        
        // For now, just check that we can read the data
        // Real checksum validation would use section.checksum
    }
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> result;
    
    // Find section
    for (const auto& section : sections_) {
        if (section.type == sectionType) {
            stream_.seekg(section.offset);
            result.resize(section.size);
            stream_.read(reinterpret_cast<char*>(result.data()), section.size);
            break;
        }
    }
    
    return result;
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Neurons);
    
    if (sectionData.empty()) {
        return false;
    }
    
    // Parse neuron data
    const uint8_t* ptr = sectionData.data();
    
    auto readVector = [&](auto& vec, size_t count) {
        vec.resize(count);
        std::copy(ptr, ptr + count * sizeof(decltype(vec)::value_type), 
                  reinterpret_cast<char*>(vec.data()));
        ptr += count * sizeof(decltype(vec)::value_type);
    };
    
    readVector(data.membranePotential, header_.neuronCount);
    readVector(data.restingPotential, header_.neuronCount);
    readVector(data.threshold, header_.neuronCount);
    readVector(data.resetPotential, header_.neuronCount);
    readVector(data.leakConductance, header_.neuronCount);
    readVector(data.firingState, header_.neuronCount);
    readVector(data.refractoryRemaining, header_.neuronCount);
    readVector(data.refractoryPeriod, header_.neuronCount);
    readVector(data.lastSpikeTime, header_.neuronCount);
    readVector(data.neuronType, header_.neuronCount);
    readVector(data.regionId, header_.neuronCount);
    readVector(data.populationId, header_.neuronCount);
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto sectionData = readSection(CheckpointSection::Synapses);
    
    if (sectionData.empty()) {
        return false;
    }
    
    // Parse synapse data
    const uint8_t* ptr = sectionData.data();
    
    auto readVector = [&](auto& vec, size_t count) {
        vec.resize(count);
        std::copy(ptr, ptr + count * sizeof(decltype(vec)::value_type), 
                  reinterpret_cast<char*>(vec.data()));
        ptr += count * sizeof(decltype(vec)::value_type);
    };
    
    readVector(data.sourceNeuron, header_.synapseCount);
    readVector(data.destinationNeuron, header_.synapseCount);
    readVector(data.weight, header_.synapseCount);
    readVector(data.delay, header_.synapseCount);
    readVector(data.synapseType, header_.synapseCount);
    readVector(data.plasticityFlags, header_.synapseCount);
    readVector(data.eligibilityTrace, header_.synapseCount);
    readVector(data.efficacy, header_.synapseCount);
    readVector(data.shortTermDepression, header_.synapseCount);
    readVector(data.shortTermFacilitation, header_.synapseCount);
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 1.0;
    
    // Calculate based on stream position
    stream_.seekg(0, std::ios::end);
    auto fileSize = stream_.tellg();
    stream_.seekg(0, std::ios::beg);
    
    return static_cast<double>(streamPos_) / fileSize;
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), bytesWritten_(0) {}
CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Close if already open
        if (stream_.is_open()) {
            stream_.close();
        }
        
        filepath_ = filepath;
        compression_ = compression;
        
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            NLM_LOG_ERROR(error_);
            return false;
        }
        
        bytesWritten_ = 0;
        sections_.clear();
        
        NLM_LOG_INFO("CheckpointWriter created: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        abort();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        if (!finalize()) {
            NLM_LOG_ERROR("Failed to finalize checkpoint during close");
        }
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
    header_.timestamp = static_cast<uint64_t>(std::time(nullptr));
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    if (!stream_.is_open()) {
        error_ = "Checkpoint not open for writing neurons";
        return false;
    }
    
    // Calculate section size
    size_t totalSize = 0;
    
    auto getVectorSize = [](const auto& vec) {
        return vec.size() * sizeof(decltype(vec)::value_type);
    };
    
    totalSize += getVectorSize(data.membranePotential);
    totalSize += getVectorSize(data.restingPotential);
    totalSize += getVectorSize(data.threshold);
    totalSize += getVectorSize(data.resetPotential);
    totalSize += getVectorSize(data.leakConductance);
    totalSize += getVectorSize(data.firingState);
    totalSize += getVectorSize(data.refractoryRemaining);
    totalSize += getVectorSize(data.refractoryPeriod);
    totalSize += getVectorSize(data.lastSpikeTime);
    totalSize += getVectorSize(data.neuronType);
    totalSize += getVectorSize(data.regionId);
    totalSize += getVectorSize(data.populationId);
    
    // Write section
    SectionHeader section;
    section.type = CheckpointSection::Neurons;
    section.offset = sizeof(CheckpointHeader) + sizeof(SectionHeader) * sections_.size();
    section.size = totalSize;
    section.compressedSize = compression_ == CompressionLevel::None ? totalSize : 0;
    section.checksum = 0; // TODO: Calculate checksum
    
    if (!writeSectionHeader(section)) {
        return false;
    }
    
    // Write data
    auto writeVector = [&](const auto& vec) {
        if (vec.empty()) return;
        stream_.write(reinterpret_cast<const char*>(vec.data()),
                     vec.size() * sizeof(decltype(vec)::value_type));
        if (!stream_) {
            error_ = "Failed to write neuron data";
            return false;
        }
        bytesWritten_ += vec.size() * sizeof(decltype(vec)::value_type);
        return true;
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
    
    sections_.push_back(section);
    return true;
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    if (!stream_.is_open()) {
        error_ = "Checkpoint not open for writing synapses";
        return false;
    }
    
    // Calculate section size
    size_t totalSize = 0;
    
    auto getVectorSize = [](const auto& vec) {
        return vec.size() * sizeof(decltype(vec)::value_type);
    };
    
    totalSize += getVectorSize(data.sourceNeuron);
    totalSize += getVectorSize(data.destinationNeuron);
    totalSize += getVectorSize(data.weight);
    totalSize += getVectorSize(data.delay);
    totalSize += getVectorSize(data.synapseType);
    totalSize += getVectorSize(data.plasticityFlags);
    totalSize += getVectorSize(data.eligibilityTrace);
    totalSize += getVectorSize(data.efficacy);
    totalSize += getVectorSize(data.shortTermDepression);
    totalSize += getVectorSize(data.shortTermFacilitation);
    
    // Write section
    SectionHeader section;
    section.type = CheckpointSection::Synapses;
    section.offset = sizeof(CheckpointHeader) + sizeof(SectionHeader) * sections_.size();
    section.size = totalSize;
    section.compressedSize = compression_ == CompressionLevel::None ? totalSize : 0;
    section.checksum = 0; // TODO: Calculate checksum
    
    if (!writeSectionHeader(section)) {
        return false;
    }
    
    // Write data
    auto writeVector = [&](const auto& vec) {
        if (vec.empty()) return;
        stream_.write(reinterpret_cast<const char*>(vec.data()),
                     vec.size() * sizeof(decltype(vec)::value_type));
        if (!stream_) {
            error_ = "Failed to write synapse data";
            return false;
        }
        bytesWritten_ += vec.size() * sizeof(decltype(vec)::value_type);
        return true;
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
    
    sections_.push_back(section);
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!stream_.is_open()) {
        error_ = "Checkpoint not open for writing section";
        return false;
    }
    
    SectionHeader section;
    section.type = sectionType;
    section.offset = sizeof(CheckpointHeader) + sizeof(SectionHeader) * sections_.size();
    section.size = size;
    section.compressedSize = compression_ == CompressionLevel::None ? size : 0;
    section.checksum = 0; // TODO: Calculate checksum
    
    if (!writeSectionHeader(section)) {
        return false;
    }
    
    if (data && size > 0) {
        stream_.write(reinterpret_cast<const char*>(data), size);
        if (!stream_) {
            error_ = "Failed to write section data";
            return false;
        }
        bytesWritten_ += size;
    }
    
    sections_.push_back(section);
    return true;
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        error_ = "Cannot finalize checkpoint - not open";
        return false;
    }
    
    // Write final header
    if (!writeHeader()) {
        return false;
    }
    
    // Write sections (already written during calls)
    
    // Pad to alignment
    padToAlignment(64);
    
    stream_.flush();
    
    NLM_LOG_INFO("Checkpoint finalized: " + filepath_ + ", " + 
                std::to_string(bytesWritten_) + " bytes written");
    
    return true;
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        // Delete incomplete file
        std::filesystem::remove(filepath_);
    }
    sections_.clear();
    bytesWritten_ = 0;
}

bool CheckpointWriter::writeHeader() {
    // Update header with checksum and final size
    header_.totalSize = bytesWritten_ + sizeof(CheckpointHeader) + 
                      sizeof(SectionHeader) * sections_.size();
    
    // Write header at beginning
    stream_.seekp(0);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    
    if (!stream_) {
        error_ = "Failed to write checkpoint header";
        return false;
    }
    
    bytesWritten_ += sizeof(CheckpointHeader);
    
    // Write section headers
    for (const auto& section : sections_) {
        if (!writeSectionHeader(section)) {
            return false;
        }
    }
    
    return true;
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& section) {
    stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
    if (!stream_) {
        error_ = "Failed to write section header";
        return false;
    }
    bytesWritten_ += sizeof(SectionHeader);
    return true;
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = stream_.tellp();
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> pad(padding, 0);
        stream_.write(reinterpret_cast<const char*>(pad.data()), padding);
        if (!stream_) {
            error_ = "Failed to write padding";
            return false;
        }
        bytesWritten_ += padding;
    }
    
    return true;
}

// CheckpointManager implementation
CheckpointManager::CheckpointManager() : saveIntervalSteps_(10000), maxCheckpoints_(10), 
                                         compress_(true), lastSaveStep_(0) {}
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
    
    NLM_LOG_INFO("CheckpointManager configured: " + checkpointDir_ + 
                ", interval=" + std::to_string(saveIntervalSteps_) + 
                ", max=" + std::to_string(maxCheckpoints_));
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (neuronProvider_ && synapseProvider_) {
        return saveImmediately();
    }
    return false;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Checkpoint providers not set";
        return false;
    }
    
    // Generate checkpoint name
    std::string checkpointName = name.empty() ? 
        "checkpoint_step_" + std::to_string(lastSaveStep_) : name;
    
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".nckpt";
    
    try {
        CheckpointWriter writer;
        if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            error_ = writer.getError();
            return false;
        }
        
        // Collect neuron data
        NeuronCheckpointData neuronData;
        if (!neuronProvider_(neuronData)) {
            error_ = "Failed to get neuron data for checkpoint";
            return false;
        }
        
        // Collect synapse data
        SynapseCheckpointData synapseData;
        if (!synapseProvider_(synapseData)) {
            error_ = "Failed to get synapse data for checkpoint";
            return false;
        }
        
        // Write data
        if (!writer.writeNeurons(neuronData)) {
            error_ = writer.getError();
            return false;
        }
        
        if (!writer.writeSynapses(synapseData)) {
            error_ = writer.getError();
            return false;
        }
        
        writer.finalize();
        
        // Update last checkpoint path
        lastCheckpointPath_ = filepath;
        lastSaveStep_ = currentStep;
        
        NLM_LOG_INFO("Checkpoint saved: " + filepath + 
                    ", " + std::to_string(writer.getBytesWritten()) + " bytes");
        
        // Prune old checkpoints
        pruneOldCheckpoints(maxCheckpoints_);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception saving checkpoint: ") + e.what();
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".nckpt";
    
    try {
        CheckpointReader reader;
        if (!reader.open(filepath)) {
            error_ = reader.getError();
            return false;
        }
        
        if (!reader.validate()) {
            error_ = "Invalid checkpoint: " + reader.getError();
            return false;
        }
        
        // TODO: Read and apply checkpoint data to brain
        NLM_LOG_INFO("Checkpoint load requested (not fully implemented): " + filepath);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception loading checkpoint: ") + e.what();
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".nckpt") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            
            // Try to read header
            std::ifstream stream(entry.path(), std::ios::binary);
            if (stream.is_open()) {
                CheckpointHeader header;
                stream.read(reinterpret_cast<char*>(&header), sizeof(CheckpointHeader));
                
                if (header.validate()) {
                    info.step = header.step;
                    info.simulationTime = header.simulationTime;
                    info.neuronCount = header.neuronCount;
                    info.synapseCount = header.synapseCount;
                    info.fileSize = std::filesystem::file_size(entry.path());
                    info.timestamp = static_cast<time_t>(header.timestamp);
                }
            }
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by step number (descending)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.step > b.step;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Delete oldest checkpoints
    for (size_t i = checkpoints.size() - 1; 
         i >= keepCount && i < checkpoints.size(); --i) {
        std::string filepath = checkpointDir_ + "/" + checkpoints[i].name + ".nckpt";
        std::error_code ec;
        std::filesystem::remove(filepath, ec);
        if (ec) {
            NLM_LOG_ERROR("Failed to remove old checkpoint: " + filepath);
        }
    }
    
    NLM_LOG_INFO("Pruned old checkpoints: kept " + std::to_string(keepCount) + 
                " of " + std::to_string(checkpoints.size()));
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

} // namespace nlm
