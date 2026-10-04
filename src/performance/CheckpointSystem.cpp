// Checkpoint system implementation for NLM neural simulation project
// Comprehensive checkpoint management with error handling and recovery

#include "CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Exception.hpp"
#include "../core/Validation/Validation.hpp"
#include <filesystem>
#include <thread>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

// Directory operations
namespace filesystem = std::filesystem;

// Initialize error handling for checkpoint system
inline void initializeCheckpointErrorHandling() {
    // Register for error reporting
    ErrorReporter::setInstance(&ErrorReporter::getInstance());
}

// Checkpoint file implementation
CheckpointWriter::CheckpointWriter()
    : bytesWritten_(0)
    , compression_(CompressionLevel::Balanced) {
    
}

CheckpointWriter::~CheckpointWriter() {
    if (isOpen()) {
        close();
    }
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Validate file path
        if (filepath.empty()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralIOException>(
                    "create", "", "Empty file path", "CheckpointWriter::create"
                )
            );
            return false;
        }
        
        // Ensure directory exists
        filesystem::path filepathObj(filepath);
        if (!filepathObj.parent_path().empty()) {
            filesystem::create_directories(filepathObj.parent_path());
        }
        
        // Open file for binary writing
        stream_.open(filepath, std::ios::binary | std::ios::trunc | std::ios::out);
        if (!stream_.is_open()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralIOException>(
                    "create", filepath, "Failed to open file for writing", "CheckpointWriter::create"
                )
            );
            return false;
        }
        
        filepath_ = filepath;
        compression_ = compression;
        bytesWritten_ = 0;
        sections_.clear();
        
        NLM_LOG_INFO("Created checkpoint file: " + filepath);
        return true;
        
    } catch (const std::filesystem::filesystem_error& e) {
        ErrorReporter::getInstance().log(
            std::make_shared<NeuralIOException>(
                "create", filepath, std::string("Filesystem error: ") + e.what(), "CheckpointWriter::create"
            )
        );
        return false;
    } catch (...) {
        ErrorReporter::getInstance().log(
            std::make_shared<NeuralIOException>(
                "create", filepath, "Unknown error creating checkpoint file", "CheckpointWriter::create"
            )
        );
        return false;
    }
}

void CheckpointWriter::close() {
    try {
        if (isOpen()) {
            // Finalize checkpoint if not already finalized
            if (bytesWritten_ > 0) {
                finalize();
            }
            
            stream_.close();
            NLM_LOG_INFO("Closed checkpoint file: " + filepath_);
        }
    } catch (...) {
        // Log error but don't throw
        NLM_LOG_ERROR("Error closing checkpoint file: " + filepath_);
    }
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                   uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    header_.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        // Validate data
        if (data.membranePotential.size() != data.restingPotential.size() ||
            data.membranePotential.size() != data.threshold.size()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "CheckpointWriter", "writeNeurons", "Invalid neuron data structure", "CheckpointWriter::writeNeurons"
                )
            );
            return false;
        }
        
        // Convert to binary format
        std::vector<uint8_t> buffer;
        size_t count = data.membranePotential.size();
        
        buffer.reserve(count * sizeof(float) * 10); // Approximate size
        
        // Write neuron data
        for (size_t i = 0; i < count; ++i) {
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.membranePotential[i]),
                         reinterpret_cast<const uint8_t*>(&data.membranePotential[i]) + sizeof(float));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.restingPotential[i]),
                         reinterpret_cast<const uint8_t*>(&data.restingPotential[i]) + sizeof(float));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.threshold[i]),
                         reinterpret_cast<const uint8_t*>(&data.threshold[i]) + sizeof(float));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.resetPotential[i]),
                         reinterpret_cast<const uint8_t*>(&data.resetPotential[i]) + sizeof(float));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.leakConductance[i]),
                         reinterpret_cast<const uint8_t*>(&data.leakConductance[i]) + sizeof(float));
            buffer.insert(buffer.end(), &data.firingState[i], &data.firingState[i] + sizeof(uint8_t));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.refractoryRemaining[i]),
                         reinterpret_cast<const uint8_t*>(&data.refractoryRemaining[i]) + sizeof(uint32_t));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.refractoryPeriod[i]),
                         reinterpret_cast<const uint8_t*>(&data.refractoryPeriod[i]) + sizeof(uint32_t));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.lastSpikeTime[i]),
                         reinterpret_cast<const uint8_t*>(&data.lastSpikeTime[i]) + sizeof(float));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.neuronType[i]),
                         reinterpret_cast<const uint8_t*>(&data.neuronType[i]) + sizeof(uint64_t));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.regionId[i]),
                         reinterpret_cast<const uint8_t*>(&data.regionId[i]) + sizeof(uint64_t));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.populationId[i]),
                         reinterpret_cast<const uint8_t*>(&data.populationId[i]) + sizeof(uint64_t));
        }
        
        return writeSection(CheckpointSection::Neurons, buffer.data(), buffer.size());
        
    } catch (const std::bad_alloc&) {
        ErrorReporter::getInstance().log(
            std::make_shared<NeuralMemoryAllocationError>(
                count * sizeof(float) * 10, "Failed to allocate buffer for neuron checkpoint", "CheckpointWriter::writeNeurons"
            )
        );
        return false;
    } catch (...) {
        ErrorReporter::getInstance().log(
            std::make_shared<NeuralIOException>(
                "writeNeurons", filepath_, "Unknown error writing neuron checkpoint", "CheckpointWriter::writeNeurons"
            )
        );
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Validate data
        if (data.weight.size() != data.sourceNeuron.size() ||
            data.weight.size() != data.destinationNeuron.size()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "CheckpointWriter", "writeSynapses", "Invalid synapse data structure", "CheckpointWriter::writeSynapses"
                )
            );
            return false;
        }
        
        // Convert to binary format
        std::vector<uint8_t> buffer;
        size_t count = data.weight.size();
        
        buffer.reserve(count * sizeof(float) * 10); // Approximate size
        
        // Write synapse data
        for (size_t i = 0; i < count; ++i) {
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.sourceNeuron[i]),
                         reinterpret_cast<const uint8_t*>(&data.sourceNeuron[i]) + sizeof(uint64_t));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.destinationNeuron[i]),
                         reinterpret_cast<const uint8_t*>(&data.destinationNeuron[i]) + sizeof(uint64_t));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.weight[i]),
                         reinterpret_cast<const uint8_t*>(&data.weight[i]) + sizeof(float));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.delay[i]),
                         reinterpret_cast<const uint8_t*>(&data.delay[i]) + sizeof(uint32_t));
            buffer.insert(buffer.end(), &data.synapseType[i], &data.synapseType[i] + sizeof(uint8_t));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.plasticityFlags[i]),
                         reinterpret_cast<const uint8_t*>(&data.plasticityFlags[i]) + sizeof(uint8_t));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.eligibilityTrace[i]),
                         reinterpret_cast<const uint8_t*>(&data.eligibilityTrace[i]) + sizeof(float));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.efficacy[i]),
                         reinterpret_cast<const uint8_t*>(&data.efficacy[i]) + sizeof(float));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.shortTermDepression[i]),
                         reinterpret_cast<const uint8_t*>(&data.shortTermDepression[i]) + sizeof(float));
            buffer.insert(buffer.end(), reinterpret_cast<const uint8_t*>(&data.shortTermFacilitation[i]),
                         reinterpret_cast<const uint8_t*>(&data.shortTermFacilitation[i]) + sizeof(float));
        }
        
        return writeSection(CheckpointSection::Synapses, buffer.data(), buffer.size());
        
    } catch (const std::bad_alloc&) {
        ErrorReporter::getInstance().log(
            std::make_shared<NeuralMemoryAllocationError>(
                count * sizeof(float) * 10, "Failed to allocate buffer for synapse checkpoint", "CheckpointWriter::writeSynapses"
            )
        );
        return false;
    } catch (...) {
        ErrorReporter::getInstance().log(
            std::make_shared<NeuralIOException>(
                "writeSynapses", filepath_, "Unknown error writing synapse checkpoint", "CheckpointWriter::writeSynapses"
            )
        );
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        if (!isOpen()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "CheckpointWriter", "writeSection", "Checkpoint file not open", "CheckpointWriter::writeSection"
                )
            );
            return false;
        }
        
        if (!data || size == 0) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralSystemStateError>(
                    "CheckpointWriter", "writeSection", "Invalid data or size", "CheckpointWriter::writeSection"
                )
            );
            return false;
        }
        
        // Calculate checksum
        uint32_t checksum = ChecksumCalculator::crc32(data, size);
        
        // Prepare section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = bytesWritten_;
        sectionHeader.size = size;
        sectionHeader.compressedSize = 0;
        sectionHeader.checksum = checksum;
        
        // Pad to alignment
        size_t alignment = 8;
        size_t padSize = (alignment - (bytesWritten_ % alignment)) % alignment;
        if (padSize > 0) {
            uint8_t padding[8] = {0};
            stream_.write(reinterpret_cast<const char*>(padding), padSize);
            bytesWritten_ += padSize;
        }
        
        // Write section header
        stream_.write(reinterpret_cast<const char*>(&sectionHeader), sizeof(SectionHeader));
        if (!stream_.good()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralIOException>(
                    "writeSection", filepath_, "Failed to write section header", "CheckpointWriter::writeSection"
                )
            );
            return false;
        }
        
        bytesWritten_ += sizeof(SectionHeader);
        
        // Write section data
        stream_.write(reinterpret_cast<const char*>(data), size);
        if (!stream_.good()) {
            ErrorReporter::getInstance().log(
                std::make_shared<NeuralIOException>(
                    "writeSection", filepath_, "Failed to write section data", "CheckpointWriter::writeSection"
                )
            );
            return false;
        }
        
        bytesWritten_ += size;
        sections_.push_back(sectionHeader);
        
        return true;
        
    } catch (...) {
        ErrorReporter::getInstance().log(
            std::make_shared<NeuralIOException>(
                "writeSection", filepath_, "Unknown error writing section", "CheckpointWriter::writeSection"
            )
        );
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        if (!isOpen()) {
            return false;
        }
        
        // Write footer (section count and checksum)
        uint32_t sectionCount = static_cast<uint32_t>(sections_.size());
        stream_.write(reinterpret_cast<const char*>(&sectionCount), sizeof(sectionCount));
        if (!stream_.good()) {
            return false;
        }
        
        // Calculate and write overall checksum
        uint64_t overallChecksum = header_.checksum;
        for (const auto& section : sections_) {
            overallChecksum = ChecksumCalculator::combine(overallChecksum, section.checksum);
        }
        
        stream_.write(reinterpret_cast<const char*>(&overallChecksum), sizeof(overallChecksum));
        
        header_.totalSize = bytesWritten_;
        header_.checksum = overallChecksum;
        
        // Write final header (overwrite first part of file)
        stream_.seekp(0);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        
        stream_.flush();
        
        NLM_LOG_INFO("Checkpoint finalized: " + std::to_string(bytesWritten_) + " bytes, " +
                    std::to_string(sectionCount) + " sections, checksum: " + std::to_string(overallChecksum));
        
        return true;
        
    } catch (...) {
        NLM_LOG_ERROR("Error finalizing checkpoint");
        return false;
    }
}

void CheckpointWriter::abort() {
    if (isOpen()) {
        close();
        // Remove partial file
        try {
            filesystem::remove(filepath_);
            NLM_LOG_INFO("Checkpoint file cleanup: " + filepath_);
        } catch (...) {
            NLM_LOG_WARNING("Failed to cleanup checkpoint file: " + filepath_);
        }
    }
}

// Checkpoint reader implementation
CheckpointReader::CheckpointReader() : streamPos_(0) {
    
}

CheckpointReader::~CheckpointReader() {
    if (isOpen()) {
        close();
    }
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        // Validate file path
        if (filepath.empty()) {
            error_ = "Empty file path";
            return false;
        }
        
        // Check if file exists
        if (!filesystem::exists(filepath)) {
            error_ = "Checkpoint file does not exist: " + filepath;
            return false;
        }
        
        // Open file for binary reading
        stream_.open(filepath, std::ios::binary | std::ios::in);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file for reading: " + filepath;
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint header: " + filepath;
            stream_.close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header (magic/checksum mismatch) in: " + filepath;
            stream_.close();
            return false;
        }
        
        // Read section count and checksums (at end of file)
        stream_.seekg(stream_.seekg(0, std::ios::end) - sizeof(uint64_t));
        stream_.read(reinterpret_cast<char*>(&overallChecksum_), sizeof(uint64_t));
        
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint footer: " + filepath;
            stream_.close();
            return false;
        }
        
        // Read section headers
        streamPos_ = sizeof(CheckpointHeader);
        stream_.seekg(streamPos_);
        
        size_t sectionCount = header_.totalSize / sizeof(SectionHeader);
        sections_.reserve(sectionCount);
        
        for (size_t i = 0; i < sectionCount; ++i) {
            SectionHeader section;
            stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader));
            if (!stream_.good()) {
                error_ = "Failed to read section header " + std::to_string(i) + ": " + filepath;
                stream_.close();
                return false;
            }
            
            sections_.push_back(section);
            streamPos_ += sizeof(SectionHeader);
            stream_.seekg(streamPos_);
        }
        
        NLM_LOG_INFO("Opened checkpoint file: " + filepath + ", " + std::to_string(sectionCount) + " sections");
        return true;
        
    } catch (...) {
        error_ = "Unknown error opening checkpoint file: " + filepath;
        stream_.close();
        return false;
    }
}

void CheckpointReader::close() {
    if (isOpen()) {
        stream_.close();
        NLM_LOG_INFO("Closed checkpoint file");
    }
}

bool CheckpointReader::validate() const {
    if (!isOpen()) {
        return false;
    }
    
    // Validate header
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header";
        return false;
    }
    
    // Validate section counts
    if (sections_.size() == 0) {
        error_ = "No sections found in checkpoint";
        return false;
    }
    
    // Validate section positions
    for (size_t i = 0; i < sections_.size(); ++i) {
        const auto& section = sections_[i];
        if (section.size > header_.totalSize - section.offset) {
            error_ = "Section " + std::to_string(i) + " extends beyond file end";
            return false;
        }
        
        if (i < sections_.size() - 1) {
            // Check overlap with next section
            const auto& nextSection = sections_[i + 1];
            if (section.offset + section.size > nextSection.offset) {
                error_ = "Section " + std::to_string(i) + " overlaps with section " + std::to_string(i + 1);
                return false;
            }
        }
    }
    
    return true;
}

// Checkpoint manager implementation
CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0)
    , checkpointVersionMajor_(1)
    , checkpointVersionMinor_(0)
    , checkpointVersionPatch_(0) {
    
}

CheckpointManager::~CheckpointManager() {
    // Cleanup checkpoint directory
    try {
        if (!checkpointDir_.empty()) {
            filesystem::remove_all(checkpointDir_);
            NLM_LOG_INFO("Checkpoint directory cleanup: " + checkpointDir_);
        }
    } catch (...) {
        // Don't throw from destructor
    }
}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    try {
        // Cleanup old directory
        if (!checkpointDir_.empty() && checkpointDir_ != checkpointDir) {
            filesystem::remove_all(checkpointDir_);
        }
        
        // Create new directory
        checkpointDir_ = checkpointDir;
        filesystem::create_directories(checkpointDir_);
        
        saveIntervalSteps_ = saveIntervalSteps;
        maxCheckpoints_ = maxCheckpoints;
        compress_ = compress;
        lastSaveStep_ = 0;
        
        NLM_LOG_INFO("Checkpoint manager configured: " + checkpointDir_ + ", " +
                    std::to_string(saveIntervalSteps) + " steps interval, " +
                    std::to_string(maxCheckpoints) + " max checkpoints");
        
    } catch (...) {
        NLM_LOG_ERROR("Failed to configure checkpoint manager");
        throw;
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    try {
        // Check if we should save
        if (shouldSave(currentStep)) {
            return saveImmediately();
        }
        return true;
        
    } catch (...) {
        NLM_LOG_ERROR("Error updating checkpoint manager");
        return false;
    }
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    try {
        if (!neuronProvider_ || !synapseProvider_) {
            NLM_LOG_WARNING("Checkpoint providers not set");
            return false;
        }
        
        // Generate checkpoint name
        std::string checkpointName = name.empty() ? 
            "checkpoint_" + std::to_string(currentStep_) + ".nchk" : 
            name + ".nchk";
        
        std::string filepath = checkpointDir_ + "/" + checkpointName;
        
        // Create checkpoint writer
        CheckpointWriter writer;
        if (!writer.create(filepath, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            return false;
        }
        
        // Write header
        writer.setMetadata(
            neuronCount_, 
            synapseCount_, 
            regionCount_, 
            step_, 
            simulationTime_
        );
        
        // Create neuron checkpoint data
        NeuronCheckpointData neuronData;
        if (!neuronProvider_(neuronData)) {
            writer.abort();
            return false;
        }
        
        // Write neuron data
        if (!writer.writeNeurons(neuronData)) {
            writer.abort();
            return false;
        }
        
        // Create synapse checkpoint data
        SynapseCheckpointData synapseData;
        if (!synapseProvider_(synapseData)) {
            writer.abort();
            return false;
        }
        
        // Write synapse data
        if (!writer.writeSynapses(synapseData)) {
            writer.abort();
            return false;
        }
        
        // Finalize checkpoint
        if (!writer.finalize()) {
            writer.abort();
            return false;
        }
        
        // Update manager state
        lastSaveStep_ = currentStep;
        lastCheckpointPath_ = filepath;
        
        // Prune old checkpoints
        pruneOldCheckpoints(maxCheckpoints_);
        
        NLM_LOG_INFO("Checkpoint saved: " + filepath);
        return true;
        
    } catch (...) {
        NLM_LOG_ERROR("Error saving checkpoint");
        return false;
    }
}

// Other methods continue...

} // namespace nlm
