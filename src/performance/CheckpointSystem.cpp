#include "CheckpointSystem.hpp"
#include "core/Logger/Logger.hpp"
#include <filesystem>
#include <cstring>
#include <zlib.h>

namespace nlm {

CheckpointReader::CheckpointReader() : stream_(), header_(), sections_(), error_(), streamPos_(0) {}

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
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint header";
            close();
            return false;
        }
        
        // Validate magic
        if (!header_.validate()) {
            error_ = "Invalid checkpoint file magic number";
            close();
            return false;
        }
        
        // Read section count
        uint32_t sectionCount;
        stream_.read(reinterpret_cast<char*>(&sectionCount), sizeof(uint32_t));
        if (!stream_.good()) {
            error_ = "Failed to read section count";
            close();
            return false;
        }
        
        sections_.resize(sectionCount);
        
        // Read section headers
        for (auto& section : sections_) {
            stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader));
            if (!stream_.good()) {
                error_ = "Failed to read section headers";
                close();
                return false;
            }
        }
        
        streamPos_ = stream_.tellg();
        NLM_LOG_INFO("CheckpointReader opened: " + filepath + " (" + std::to_string(header_.neuronCount) + " neurons, " +
                    std::to_string(header_.synapseCount) + " synapses)");
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

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    try {
        // Find neurons section
        CheckpointSection targetSection = CheckpointSection::Neurons;
        for (const auto& section : sections_) {
            if (section.type == static_cast<uint32_t>(targetSection)) {
                // Seek to section data
                stream_.seekg(section.offset);
                
                // Read neuron count
                uint64_t neuronCount;
                stream_.read(reinterpret_cast<char*>(&neuronCount), sizeof(uint64_t));
                if (!stream_.good()) {
                    error_ = "Failed to read neuron count from section";
                    return false;
                }
                
                // Allocate space
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
                
                // Read data
                stream_.read(reinterpret_cast<char*>(data.membranePotential.data()), 
                           data.membranePotential.size() * sizeof(float));
                stream_.read(reinterpret_cast<char*>(data.restingPotential.data()), 
                           data.restingPotential.size() * sizeof(float));
                stream_.read(reinterpret_cast<char*>(data.threshold.data()), 
                           data.threshold.size() * sizeof(float));
                stream_.read(reinterpret_cast<char*>(data.resetPotential.data()), 
                           data.resetPotential.size() * sizeof(float));
                stream_.read(reinterpret_cast<char*>(data.leakConductance.data()), 
                           data.leakConductance.size() * sizeof(float));
                stream_.read(reinterpret_cast<char*>(data.firingState.data()), 
                           data.firingState.size() * sizeof(uint8_t));
                stream_.read(reinterpret_cast<char*>(data.refractoryRemaining.data()), 
                           data.refractoryRemaining.size() * sizeof(uint32_t));
                stream_.read(reinterpret_cast<char*>(data.refractoryPeriod.data()), 
                           data.refractoryPeriod.size() * sizeof(uint32_t));
                stream_.read(reinterpret_cast<char*>(data.lastSpikeTime.data()), 
                           data.lastSpikeTime.size() * sizeof(float));
                stream_.read(reinterpret_cast<char*>(data.neuronType.data()), 
                           data.neuronType.size() * sizeof(uint64_t));
                stream_.read(reinterpret_cast<char*>(data.regionId.data()), 
                           data.regionId.size() * sizeof(uint64_t));
                stream_.read(reinterpret_cast<char*>(data.populationId.data()), 
                           data.populationId.size() * sizeof(uint64_t));
                
                if (!stream_.good()) {
                    error_ = "Failed to read neuron data from section";
                    return false;
                }
                
                return true;
            }
        }
        
        error_ = "Neurons section not found in checkpoint";
        return false;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading neurons: ") + e.what();
        return false;
    }
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    try {
        // Find synapses section
        CheckpointSection targetSection = CheckpointSection::Synapses;
        for (const auto& section : sections_) {
            if (section.type == static_cast<uint32_t>(targetSection)) {
                // Seek to section data
                stream_.seekg(section.offset);
                
                // Read synapse count
                uint64_t synapseCount;
                stream_.read(reinterpret_cast<char*>(&synapseCount), sizeof(uint64_t));
                if (!stream_.good()) {
                    error_ = "Failed to read synapse count from section";
                    return false;
                }
                
                // Allocate space
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
                
                // Read data
                stream_.read(reinterpret_cast<char*>(data.sourceNeuron.data()), 
                           data.sourceNeuron.size() * sizeof(uint64_t));
                stream_.read(reinterpret_cast<char*>(data.destinationNeuron.data()), 
                           data.destinationNeuron.size() * sizeof(uint64_t));
                stream_.read(reinterpret_cast<char*>(data.weight.data()), 
                           data.weight.size() * sizeof(float));
                stream_.write(reinterpret_cast<const char*>(data.delay.data()), 
                           data.delay.size() * sizeof(uint32_t));
                stream_.write(reinterpret_cast<const char*>(data.synapseType.data()), 
                           data.synapseType.size() * sizeof(uint8_t));
                stream_.write(reinterpret_cast<const char*>(data.plasticityFlags.data()), 
                           data.plasticityFlags.size() * sizeof(uint8_t));
                stream_.write(reinterpret_cast<const char*>(data.eligibilityTrace.data()), 
                           data.eligibilityTrace.size() * sizeof(float));
                stream_.write(reinterpret_cast<const char*>(data.efficacy.data()), 
                           data.efficacy.size() * sizeof(float));
                stream_.write(reinterpret_cast<const char*>(data.shortTermDepression.data()), 
                           data.shortTermDepression.size() * sizeof(float));
                stream_.write(reinterpret_cast<const char*>(data.shortTermFacilitation.data()), 
                           data.shortTermFacilitation.size() * sizeof(float));
                
                if (!stream_.good()) {
                    error_ = "Failed to read synapse data from section";
                    return false;
                }
                
                return true;
            }
        }
        
        error_ = "Synapses section not found in checkpoint";
        return false;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading synapses: ") + e.what();
        return false;
    }
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    try {
        // Find section
        for (const auto& section : sections_) {
            if (section.type == static_cast<uint32_t>(sectionType)) {
                stream_.seekg(section.offset);
                std::vector<uint8_t> data(section.size);
                stream_.read(reinterpret_cast<char*>(data.data()), section.size);
                if (!stream_.good()) {
                    error_ = "Failed to read section data";
                }
                return data;
            }
        }
        
        error_ = "Section not found in checkpoint";
        return {};
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception reading section: ") + e.what();
        return {};
    }
}

bool CheckpointReader::validate() const {
    if (!stream_.is_open()) {
        error_ = "Checkpoint reader is not open";
        return false;
    }
    
    // Basic validation already done in open()
    return true;
}

double CheckpointReader::getProgress() const {
    if (!stream_.is_open() || sections_.empty()) {
        return 0.0;
    }
    
    std::streampos currentPos = stream_.tellg();
    std::streampos fileSize = stream_.seekg(0, std::ios::end).tellg();
    if (fileSize == 0) return 0.0;
    
    return static_cast<double>(currentPos) / static_cast<double>(fileSize);
}

CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(), header_(), sections_(), bytesWritten_(0), compressBuffer_() {}

CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Remove existing file
        if (std::filesystem::exists(filepath)) {
            std::filesystem::remove(filepath);
        }
        
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            error_ = "Failed to create checkpoint file: " + filepath;
            NLM_LOG_ERROR(error_);
            return false;
        }
        
        filepath_ = filepath;
        compression_ = compression;
        bytesWritten_ = 0;
        
        NLM_LOG_INFO("CheckpointWriter created: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception creating checkpoint: ") + e.what();
        close();
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        if (finalize()) {
            NLM_LOG_INFO("Checkpoint finalized successfully");
        } else {
            NLM_LOG_ERROR("Failed to finalize checkpoint");
        }
        stream_.close();
    }
    
    filepath_.clear();
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
        // Write section header
        SectionHeader header;
        header.type = static_cast<uint32_t>(CheckpointSection::Neurons);
        
        // Compute and write current position as offset
        std::streampos currentPos = stream_.tellp();
        header.offset = static_cast<uint64_t>(currentPos) + sizeof(SectionHeader);
        
        // Calculate data size
        uint64_t dataSize = data.membranePotential.size() * sizeof(float) +
                           data.restingPotential.size() * sizeof(float) +
                           data.threshold.size() * sizeof(float) +
                           data.resetPotential.size() * sizeof(float) +
                           data.leakConductance.size() * sizeof(float) +
                           data.firingState.size() * sizeof(uint8_t) +
                           data.refractoryRemaining.size() * sizeof(uint32_t) +
                           data.refractoryPeriod.size() * sizeof(uint32_t) +
                           data.lastSpikeTime.size() * sizeof(float) +
                           data.neuronType.size() * sizeof(uint64_t) +
                           data.regionId.size() * sizeof(uint64_t) +
                           data.populationId.size() * sizeof(uint64_t);
        
        // Write neuron count prefix
        stream_.write(reinterpret_cast<const char*>(&dataSize), sizeof(uint64_t));
        
        // Write neuron data
        stream_.write(reinterpret_cast<const char*>(data.membranePotential.data()),
                     data.membranePotential.size() * sizeof(float));
        stream_.write(reinterpret_cast<const char*>(data.restingPotential.data()),
                     data.restingPotential.size() * sizeof(float));
        stream_.write(reinterpret_cast<const char*>(data.threshold.data()),
                     data.threshold.size() * sizeof(float));
        stream_.write(reinterpret_cast<const char*>(data.resetPotential.data()),
                     data.resetPotential.size() * sizeof(float));
        stream_.write(reinterpret_cast<const char*>(data.leakConductance.data()),
                     data.leakConductance.size() * sizeof(float));
        stream_.write(reinterpret_cast<const char*>(data.firingState.data()),
                     data.firingState.size() * sizeof(uint8_t));
        stream_.write(reinterpret_cast<const char*>(data.refractoryRemaining.data()),
                     data.refractoryRemaining.size() * sizeof(uint32_t));
        stream_.write(reinterpret_cast<const char*>(data.refractoryPeriod.data()),
                     data.refractoryPeriod.size() * sizeof(uint32_t));
        stream_.write(reinterpret_cast<const char*>(data.lastSpikeTime.data()),
                     data.lastSpikeTime.size() * sizeof(float));
        stream_.write(reinterpret_cast<const char*>(data.neuronType.data()),
                     data.neuronType.size() * sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(data.regionId.data()),
                     data.regionId.size() * sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(data.populationId.data()),
                     data.populationId.size() * sizeof(uint64_t));
        
        if (!stream_.good()) {
            error_ = "Failed to write neurons data";
            return false;
        }
        
        // Update section header
        header.size = dataSize;
        header.compressedSize = 0;
        header.checksum = 0;  // Simplified - could implement checksum
        
        // Seek back to write section header
        stream_.seekp(header.offset - sizeof(SectionHeader));
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
        
        // Seek back to end
        stream_.seekp(0, std::ios::end);
        
        sections_.push_back(header);
        bytesWritten_ += sizeof(uint64_t) + dataSize + sizeof(SectionHeader);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing neurons: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Write section header
        SectionHeader header;
        header.type = static_cast<uint32_t>(CheckpointSection::Synapses);
        
        // Compute and write current position as offset
        std::streampos currentPos = stream_.tellp();n
        header.offset = static_cast<uint64_t>(currentPos) + sizeof(SectionHeader);
        
        // Calculate data size
        uint64_t dataSize = data.sourceNeuron.size() * sizeof(uint64_t) +
                           data.destinationNeuron.size() * sizeof(uint64_t) +
                           data.weight.size() * sizeof(float) +
                           data.delay.size() * sizeof(uint32_t) +
                           data.synapseType.size() * sizeof(uint8_t) +
                           data.plasticityFlags.size() * sizeof(uint8_t) +
                           data.eligibilityTrace.size() * sizeof(float) +
                           data.efficacy.size() * sizeof(float) +
                           data.shortTermDepression.size() * sizeof(float) +
                           data.shortTermFacilitation.size() * sizeof(float);
        
        // Write synapse count prefix
        stream_.write(reinterpret_cast<const char*>(&dataSize), sizeof(uint64_t));
        
        // Write synapse data
        stream_.write(reinterpret_cast<const char*>(data.sourceNeuron.data()),
                     data.sourceNeuron.size() * sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(data.destinationNeuron.data()),
                     data.destinationNeuron.size() * sizeof(uint64_t));
        stream_.write(reinterpret_cast<const char*>(data.weight.data()),
                     data.weight.size() * sizeof(float));
        stream_.write(reinterpret_cast<const char*>(data.delay.data()),
                     data.delay.size() * sizeof(uint32_t));
        stream_.write(reinterpret_cast<const char*>(data.synapseType.data()),
                     data.synapseType.size() * sizeof(uint8_t));
        stream_.write(reinterpret_cast<const char*>(data.plasticityFlags.data()),
                     data.plasticityFlags.size() * sizeof(uint8_t));
        stream_.write(reinterpret_cast<const char*>(data.eligibilityTrace.data()),
                     data.eligibilityTrace.size() * sizeof(float));
        stream_.write(reinterpret_cast<const char*>(data.efficacy.data()),
                     data.efficacy.size() * sizeof(float));
        stream_.write(reinterpret_cast<const char*>(data.shortTermDepression.data()),
                     data.shortTermDepression.size() * sizeof(float));
        stream_.write(reinterpret_cast<const char*>(data.shortTermFacilitation.data()),
                     data.shortTermFacilitation.size() * sizeof(float));
        
        if (!stream_.good()) {
            error_ = "Failed to write synapses data";
            return false;
        }
        
        // Update section header
        header.size = dataSize;
        header.compressedSize = 0;
        header.checksum = 0;
        
        // Seek back to write section header
        stream_.seekp(header.offset - sizeof(SectionHeader));
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
        
        // Seek back to end
        stream_.seekp(0, std::ios::end);
        
        sections_.push_back(header);
        bytesWritten_ += sizeof(uint64_t) + dataSize + sizeof(SectionHeader);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing synapses: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        SectionHeader header;
        header.type = static_cast<uint32_t>(sectionType);
        
        std::streampos currentPos = stream_.tellp();
        header.offset = static_cast<uint64_t>(currentPos) + sizeof(SectionHeader);
        
        stream_.write(reinterpret_cast<const char*>(data), size);
        if (!stream_.good()) {
            error_ = "Failed to write section data";
            return false;
        }
        
        header.size = size;
        header.compressedSize = 0;
        header.checksum = 0;
        
        // Seek back to write section header
        stream_.seekp(header.offset - sizeof(SectionHeader));
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
        
        // Seek back to end
        stream_.seekp(0, std::ios::end);
        
        sections_.push_back(header);
        bytesWritten_ += size + sizeof(SectionHeader);
        
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception writing section: ") + e.what();
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        if (!stream_.is_open()) {
            error_ = "Checkpoint writer is not open";
            return false;
        }
        
        // Write final section count
        uint32_t sectionCount = static_cast<uint32_t>(sections_.size());
        stream_.seekp(0, std::ios::end);
        stream_.write(reinterpret_cast<const char*>(&sectionCount), sizeof(uint32_t));
        
        // Write section headers
        for (const auto& section : sections_) {
            stream_.write(reinterpret_cast<const char*>(&section), sizeof(SectionHeader));
        }
        
        // Write checksum
        uint64_t checksum = 0;  // Simplified - could implement checksum
        stream_.write(reinterpret_cast<const char*>(&checksum), sizeof(uint64_t));
        
        // Write compression info
        uint32_t compressionLevel = static_cast<uint32_t>(compression_);
        stream_.write(reinterpret_cast<const char*>(&compressionLevel), sizeof(uint32_t));
        
        // Write padding to alignment
        padToAlignment(4096);  // 4KB alignment
        
        if (!stream_.good()) {
            error_ = "Failed to write checkpoint footer";
            return false;
        }
        
        // Update header
        header_.totalSize = bytesWritten_ + sizeof(CheckpointHeader) + 
                           sizeof(uint32_t) + sizeof(SectionHeader) * sectionCount +
                           sizeof(uint64_t) + sizeof(uint32_t);
        
        // Seek to beginning and write header
        stream_.seekp(0);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        
        stream_.flush();
        NLM_LOG_INFO("Checkpoint finalized, size: " + std::to_string(bytesWritten_) + " bytes");
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("Exception finalizing checkpoint: ") + e.what();
        return false;
    }
}

void CheckpointWriter::abort() {
    close();
    if (!filepath_.empty() && std::filesystem::exists(filepath_)) {
        try {
            std::filesystem::remove(filepath_);
            NLM_LOG_INFO("Cleaned up partial checkpoint: " + filepath_);
        } catch (...) {
            // Ignore cleanup errors
        }
    }
}

bool CheckpointWriter::writeHeader() {
    // Header is already set by setMetadata
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    bytesWritten_ += sizeof(CheckpointHeader);
    return stream_.good();
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    bytesWritten_ += sizeof(SectionHeader);
    return stream_.good();
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    std::streampos current = stream_.tellp();
    size_t remainder = current % alignment;
    if (remainder == 0) return true;
    
    std::vector<char> padding(alignment - remainder, 0);
    stream_.write(padding.data(), padding.size());
    bytesWritten_ += padding.size();
    return stream_.good();
}

CheckpointManager::CheckpointManager() : 
    checkpointDir_(), saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true),
    lastSaveStep_(0), lastCheckpointPath_(), neuronProvider_(), synapseProvider_() {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir, 
                                uint64_t saveIntervalSteps,
                                size_t maxCheckpoints,
                                bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    lastSaveStep_ = 0;
    
    // Create directory if it doesn't exist
    std::filesystem::create_directories(checkpointDir_);
    
    NLM_LOG_INFO("CheckpointManager configured: " + checkpointDir_ + ", interval: " + std::to_string(saveIntervalSteps_));
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return false;
    }
    
    return saveImmediately();
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string filename = name.empty() ? "checkpoint_" + std::to_string(lastSaveStep_) + ".bin" : name;
    std::string fullPath = checkpointDir_ + "/" + filename;
    
    if (neuronProvider_ && synapseProvider_) {
        // Create a temporary checkpoint
        CheckpointWriter writer;
        if (!writer.create(fullPath)) {
            NLM_LOG_ERROR("Failed to create checkpoint: " + fullPath);
            return false;
        }
        
        // Write metadata
        writer.setMetadata(0, 0, 0, lastSaveStep_, currentTime);  // Would need actual counts
        
        // Write data using providers
        NeuronCheckpointData neuronData;
        SynapseCheckpointData synapseData;
        
        if (neuronProvider_(neuronData) && !neuronData.membranePotential.empty()) {
            writer.writeNeurons(neuronData);
        }
        
        if (synapseProvider_(synapseData) && !synapseData.sourceNeuron.empty()) {
            writer.writeSynapses(synapseData);
        }
        
        writer.finalize();
        lastSaveStep_ = 0; // Reset for next save
        lastCheckpointPath_ = fullPath;
        
        NLM_LOG_INFO("Checkpoint saved: " + fullPath);
        return true;
    }
    
    NLM_LOG_ERROR("Checkpoint providers not set");
    return false;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filename = name.empty() ? "checkpoint_" + std::to_string(lastSaveStep_) + ".bin" : name;
    std::string fullPath = checkpointDir_ + "/" + filename;
    
    if (std::filesystem::exists(fullPath)) {
        CheckpointReader reader;
        if (reader.open(fullPath)) {
            NLM_LOG_INFO("Checkpoint loaded: " + fullPath);
            return true;
        } else {
            NLM_LOG_ERROR("Failed to load checkpoint: " + fullPath + " - " + reader.getError());
        }
    }
    
    NLM_LOG_ERROR("Checkpoint file not found: " + fullPath);
    return false;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".bin") {
            CheckpointInfo info;
            info.name = entry.path().filename().string();
            info.fileSize = entry.file_size();
            info.timestamp = std::filesystem::last_write_time(entry.path());
            
            // Extract step from filename
            size_t underscore = info.name.find("checkpoint_");
            if (underscore != std::string::npos) {
                size_t underscore2 = info.name.find("_", underscore + 11);
                if (underscore2 != std::string::npos) {
                    std::string stepStr = info.name.substr(underscore + 11, underscore2 - underscore - 11);
                    try {
                        info.step = std::stoull(stepStr);
                    } catch (...) {
                        info.step = 0;
                    }
                }
            }
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by step number (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.step > b.step;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();n
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    // Delete oldest checkpoints
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        std::string fullPath = checkpointDir_ + "/" + checkpoints[i].name;
        try {
            std::filesystem::remove(fullPath);
            NLM_LOG_INFO("Pruned checkpoint: " + fullPath);
        } catch (...) {
            // Ignore errors
        }
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep >= lastSaveStep_ + saveIntervalSteps_;
}

} // namespace nlm

