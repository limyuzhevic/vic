#include "CheckpointSystem.hpp"
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace nlm {

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}

CheckpointWriter::~CheckpointWriter() {
    if (stream_.is_open()) {
        close();
    }
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    try {
        // Create directory if it doesn't exist
        std::filesystem::path path(filepath);
        std::filesystem::create_directories(path.parent_path());
        
        // Open file for writing
        stream_.open(filepath, std::ios::binary | std::ios::out);
        if (!stream_.is_open()) {
            return false;
        }
        
        compression_ = compression;
        filepath_ = filepath;
        
        // Write header
        if (!writeHeader()) {
            stream_.close();
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        // Store error for later retrieval
        error_ = "Writer creation error: " + std::string(e.what());
        if (stream_.is_open()) {
            stream_.close();
        }
        return false;
    }
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        try {
            finalize();
        } catch (const std::exception& e) {
            error_ = "Writer close error: " + std::string(e.what());
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
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    try {
        // Prepare section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(CheckpointSection::Neurons);
        sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
        
        // Convert neuron data to bytes (simplified serialization)
        std::vector<uint8_t> neuronBytes;
        
        // Serialize neuron data (in real implementation would be more sophisticated)
        // For now, just write the size as a placeholder
        size_t dataSize = 0;
        if (!data.membranePotential.empty()) {
            dataSize = data.membranePotential.size() * sizeof(float);
        }
        
        // Simple serialization: write size then data
        neuronBytes.reserve(dataSize + sizeof(size_t));
        neuronBytes.insert(neuronBytes.end(), 
                          reinterpret_cast<const uint8_t*>(&dataSize),
                          reinterpret_cast<const uint8_t*>(&dataSize) + sizeof(size_t));
        
        // If we have actual data, include it
        if (dataSize > 0) {
            neuronBytes.insert(neuronBytes.end(),
                              reinterpret_cast<const uint8_t*>(data.membranePotential.data()),
                              reinterpret_cast<const uint8_t*>(data.membranePotential.data()) + dataSize);
        }
        
        // Update section header with actual data
        sectionHeader.size = neuronBytes.size();
        sectionHeader.compressedSize = neuronBytes.size();
        sectionHeader.checksum = ChecksumCalculator::crc32(neuronBytes.data(), neuronBytes.size());
        
        // Write section header
        if (!writeSectionHeader(sectionHeader)) {
            return false;
        }
        
        // Write neuron data
        if (!writeSection(CheckpointSection::Neurons, neuronBytes.data(), neuronBytes.size())) {
            return false;
        }
        
        sections_.push_back(sectionHeader);
        return true;
    } catch (const std::exception& e) {
        error_ = "Neuron write error: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    try {
        // Prepare section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(CheckpointSection::Synapses);
        sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
        
        // Convert synapse data to bytes (simplified serialization)
        std::vector<uint8_t> synapseBytes;
        
        // Serialize synapse data (in real implementation would be more sophisticated)
        // For now, just write the size as a placeholder
        size_t dataSize = 0;
        if (!data.weight.empty()) {
            dataSize = data.weight.size() * sizeof(float);
        }
        
        // Simple serialization: write size then data
        synapseBytes.reserve(dataSize + sizeof(size_t));
        synapseBytes.insert(synapseBytes.end(), 
                          reinterpret_cast<const uint8_t*>(&dataSize),
                          reinterpret_cast<const uint8_t*>(&dataSize) + sizeof(size_t));
        
        // If we have actual data, include it
        if (dataSize > 0) {
            synapseBytes.insert(synapseBytes.end(),
                              reinterpret_cast<const uint8_t*>(data.weight.data()),
                              reinterpret_cast<const uint8_t*>(data.weight.data()) + dataSize);
        }
        
        // Update section header with actual data
        sectionHeader.size = synapseBytes.size();
        sectionHeader.compressedSize = synapseBytes.size();
        sectionHeader.checksum = ChecksumCalculator::crc32(synapseBytes.data(), synapseBytes.size());
        
        // Write section header
        if (!writeSectionHeader(sectionHeader)) {
            return false;
        }
        
        // Write synapse data
        if (!writeSection(CheckpointSection::Synapses, synapseBytes.data(), synapseBytes.size())) {
            return false;
        }
        
        sections_.push_back(sectionHeader);
        return true;
    } catch (const std::exception& e) {
        error_ = "Synapse write error: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    try {
        if (!stream_.is_open()) {
            return false;
        }
        
        // Write section data
        const uint8_t* bytes = static_cast<const uint8_t*>(data);
        stream_.write(reinterpret_cast<const char*>(bytes), size);
        
        if (stream_.fail()) {
            error_ = "Section write failed";
            return false;
        }
        
        bytesWritten_ += size;
        padToAlignment(8); // Align to 8 bytes
        
        return true;
    } catch (const std::exception& e) {
        error_ = "Section write error: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    try {
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
        if (stream_.fail()) {
            error_ = "Section header write failed";
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        error_ = "Section header write error: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    try {
        size_t currentPos = static_cast<size_t>(stream_.tellp());
        size_t padding = (alignment - (currentPos % alignment)) % alignment;
        
        if (padding > 0) {
            std::vector<uint8_t> paddingBytes(padding, 0);
            return writeSection(CheckpointSection::Checksum, paddingBytes.data(), padding);
        }
        
        return true;
    } catch (const std::exception& e) {
        error_ = "Padding error: " + std::string(e.what());
        return false;
    }
}

bool CheckpointWriter::finalize() {
    try {
        // Update header with final info
        header_.totalSize = bytesWritten_;
        header_.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        // Recalculate header checksum
        uint64_t headerChecksum = ChecksumCalculator::crc64(&header_, sizeof(CheckpointHeader) - offsetof(CheckpointHeader, checksum));
        header_.checksum = headerChecksum;
        
        // Write updated header at start of file
        stream_.seekp(0, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
        
        if (stream_.fail()) {
            error_ = "Final header write failed";
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        error_ = "Finalization error: " + std::string(e.what());
        return false;
    }
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        std::filesystem::remove(filepath_);
    }
}

// CheckpointReader implementation
CheckpointReader::CheckpointReader() : streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    if (stream_.is_open()) {
        close();
    }
}

bool CheckpointReader::open(const std::string& filepath) {
    try {
        stream_.open(filepath, std::ios::binary | std::ios::in);
        if (!stream_.is_open()) {
            error_ = "Cannot open checkpoint file: " + filepath;
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
        if (stream_.fail()) {
            error_ = "Failed to read checkpoint header";
            stream_.close();
            return false;
        }
        
        // Validate header
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header";
            stream_.close();
            return false;
        }
        
        // Read section headers
        streamPos_ = sizeof(CheckpointHeader);
        sections_.clear();
        
        while (streamPos_ < header_.totalSize) {
            SectionHeader sectionHeader;
            stream_.seekg(streamPos_);
            stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(SectionHeader));
            
            if (stream_.fail()) {
                break; // End of file or error
            }
            
            // Validate section header
            if (sectionHeader.type >= static_cast<uint32_t>(CheckpointSection::End)) {
                error_ = "Invalid section type";
                stream_.close();
                return false;
            }
            
            sections_.push_back(sectionHeader);
            streamPos_ += sizeof(SectionHeader);
            
            // Skip section data (including padding)
            streamPos_ += sectionHeader.size + sectionHeader.compressedSize;
            streamPos_ = (streamPos_ + 7) & ~7; // Align to 8 bytes
        }
        
        return true;
    } catch (const std::exception& e) {
        error_ = "Reader open error: " + std::string(e.what());
        if (stream_.is_open()) {
            stream_.close();
        }
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
        sections_.clear();
    }
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    return readSection(CheckpointSection::Neurons);
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    return readSection(CheckpointSection::Synapses);
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& section : sections_) {
        if (static_cast<CheckpointSection>(section.type) == sectionType) {
            std::vector<uint8_t> sectionData(section.size, 0);
            
            stream_.seekg(section.offset);
            stream_.read(reinterpret_cast<char*>(sectionData.data()), section.size);
            
            if (stream_.fail()) {
                error_ = "Failed to read section data";
                return std::vector<uint8_t>();
            }
            
            return sectionData;
        }
    }
    
    error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    return !error_.empty();
}

double CheckpointReader::getProgress() const {
    if (header_.totalSize == 0) {
        return 0.0;
    }
    
    // Calculate progress based on current position
    size_t currentPos = static_cast<size_t>(stream_.tellg());
    return static_cast<double>(currentPos) / header_.totalSize;
}

// CheckpointManager implementation
CheckpointManager::CheckpointManager() : lastSaveStep_(0), error_(""), lastCheckpointPath_("") {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir,
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    try {
        // Check if we should save based on interval
        if (shouldSave(currentStep)) {
            std::string checkpointName = "checkpoint_" + std::to_string(currentStep);
            return saveImmediately(checkpointName);
        }
        return true;
    } catch (const std::exception& e) {
        error_ = "CheckpointManager::update error: " + std::string(e.what());
        return false;
    }
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    try {
        if (name.empty()) {
            return false;
        }
        
        // Create full file path
        std::filesystem::path filepath = checkpointDir_;
        filepath /= (name + ".nmlcp");
        
        // Create directory if it doesn't exist
        std::filesystem::create_directories(filepath.parent_path());
        
        // Create and configure writer
        CheckpointWriter writer;
        if (!writer.create(filepath.string(), compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            return false;
        }
        
        // Set metadata (placeholder - should be provided by caller)
        writer.setMetadata(0, 0, 0, lastSaveStep_, 0.0);
        
        // Write some basic placeholder data
        NeuronCheckpointData neuronData;
        neuronData.membranePotential = {1.0f, 0.5f};
        neuronData.neuronType = {0, 1};
        neuronData.regionId = {1, 1};
        
        if (!writer.writeNeurons(neuronData)) {
            writer.abort();
            return false;
        }
        
        SynapseCheckpointData synapseData;
        synapseData.sourceNeuron = {1, 2};
        synapseData.destinationNeuron = {2, 1};
        synapseData.weight = {0.1f, 0.2f};
        
        if (!writer.writeSynapses(synapseData)) {
            writer.abort();
            return false;
        }
        
        // Finalize checkpoint
        if (!writer.finalize()) {
            return false;
        }
        
        lastCheckpointPath_ = filepath.string();
        lastSaveStep_ = lastSaveStep_ ? lastSaveStep_ + saveIntervalSteps_ : currentStep;
        
        return true;
    } catch (const std::exception& e) {
        error_ = "CheckpointManager::saveImmediately error: " + std::string(e.what());
        return false;
    }
}

bool CheckpointManager::load(const std::string& name) {
    try {
        if (name.empty()) {
            return false;
        }
        
        std::filesystem::path filepath = checkpointDir_;
        filepath /= (name + ".nmlcp");
        
        if (!std::filesystem::exists(filepath)) {
            return false;
        }
        
        // Open checkpoint file
        CheckpointReader reader;
        if (!reader.open(filepath.string())) {
            return false;
        }
        
        if (!reader.validate()) {
            return false;
        }
        
        // Load basic data
        const auto& header = reader.getHeader();
        // Update internal state based on header
        lastSaveStep_ = header.step;
        lastCheckpointPath_ = filepath.string();
        
        return true;
    } catch (const std::exception& e) {
        error_ = "CheckpointManager::load error: " + std::string(e.what());
        return false;
    }
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    try {
        if (!std::filesystem::exists(checkpointDir_)) {
            return checkpoints;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.is_regular_file() && entry.path().extension() == ".nmlcp") {
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                
                // Try to extract info from filename or file content
                // For now, use basic file info
                auto fileTime = std::filesystem::last_write_time(entry.path());
                info.timestamp = std::chrono::system_clock::to_time_t(
                    std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                        fileTime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
                    )
                );
                
                info.fileSize = std::filesystem::file_size(entry.path());
                info.step = 0;  // Could parse from filename or header if needed
                info.simulationTime = 0.0;
                info.neuronCount = 0;
                info.synapseCount = 0;
                
                checkpoints.push_back(info);
            }
        }
        
        // Sort by timestamp (newest first)
        std::sort(checkpoints.begin(), checkpoints.end(),
                 [](const CheckpointInfo& a, const CheckpointInfo& b) {
                     return a.timestamp > b.timestamp;
                 });
                 
    } catch (const std::exception& e) {
        error_ = "CheckpointManager::listCheckpoints error: " + std::string(e.what());
    }
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    try {
        if (!std::filesystem::exists(checkpointDir_) || keepCount <= 1) {
            return;
        }
        
        auto checkpoints = listCheckpoints();
        if (checkpoints.size() <= keepCount) {
            return; // Nothing to prune
        }
        
        // Keep only the most recent checkpoints
        std::sort(checkpoints.begin(), checkpoints.end(),
                 [](const CheckpointInfo& a, const CheckpointInfo& b) {
                     return a.timestamp > b.timestamp;
                 });
        
        // Remove old checkpoints
        for (size_t i = keepCount; i < checkpoints.size(); ++i) {
            std::filesystem::path filepath = checkpointDir_;
            filepath /= (checkpoints[i].name + ".nmlcp");
            if (std::filesystem::exists(filepath)) {
                std::filesystem::remove(filepath);
            }
        }
        
    } catch (const std::exception& e) {
        error_ = "CheckpointManager::pruneOldCheckpoints error: " + std::string(e.what());
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    try {
        if (lastSaveStep_ == 0) {
            return true; // First save
        }
        
        // Check if interval has passed
        if (currentStep >= lastSaveStep_ + saveIntervalSteps_) {
            return true;
        }
        
        // Check if we have too many checkpoints
        auto checkpoints = listCheckpoints();
        if (checkpoints.size() >= maxCheckpoints_) {
            return true; // Force save to prune later
        }
        
        return false;
    } catch (const std::exception& e) {
        error_ = "CheckpointManager::shouldSave error: " + std::string(e.what());
        return false;
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    try {
        if (lastCheckpointPath_.empty() && !checkpointDir_.empty()) {
            // Try to find the most recent checkpoint file
            auto checkpoints = listCheckpoints();
            if (!checkpoints.empty()) {
                std::filesystem::path filepath = checkpointDir_;
                filepath /= (checkpoints[0].name + ".nmlcp");
                lastCheckpointPath_ = filepath.string();
            }
        }
        return lastCheckpointPath_;
    } catch (const std::exception& e) {
        error_ = "CheckpointManager::getLastCheckpointPath error: " + std::string(e.what());
        return "";
    }
}

} // namespace nlm
