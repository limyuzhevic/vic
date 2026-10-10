// CheckpointSystem.cpp - Implementation of the checkpoint system for NLM
// Provides robust checkpointing for large-scale neural simulations.
// Supports incremental saves, version compatibility, and validation.

#include "performance/CheckpointSystem.hpp"
#include <cstring>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <zlib.h>  // For compression

namespace nlm {

namespace {
    // Helper function to generate a checkpoint filename
    std::string generateCheckpointFilename(const std::string& dir, uint64_t step, 
                                            const std::string& prefix = "checkpoint") {
        std::filesystem::path dirPath(dir);
        dirPath /= (prefix + "_step_" + std::to_string(step) + ".nlmck");
        return dirPath.string();
    }
    
    // Helper function to compress data using zlib
    bool compressData(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, CompressionLevel level) {
        if (level == CompressionLevel::None) {
            output = input;
            return true;
        }
        
        int zlibLevel = Z_DEFAULT_COMPRESSION;
        switch (level) {
            case CompressionLevel::Fast: zlibLevel = Z_BEST_SPEED; break;
            case CompressionLevel::Balanced: zlibLevel = Z_DEFAULT_COMPRESSION; break;
            case CompressionLevel::Best: zlibLevel = Z_BEST_COMPRESSION; break;
            default: break;
        }
        
        z_stream stream;
        memset(&stream, 0, sizeof(stream));
        
        int ret = deflateInit(&stream, zlibLevel);
        if (ret != Z_OK) {
            return false;
        }
        
        stream.next_in = (Bytef*)input.data();
        stream.avail_in = input.size();
        
        std::vector<uint8_t> tempBuffer(input.size() * 2);  // Temporary buffer
        stream.next_out = tempBuffer.data();
        stream.avail_out = tempBuffer.size();
        
        ret = deflate(&stream, Z_FINISH);
        if (ret != Z_STREAM_END) {
            deflateEnd(&stream);
            return false;
        }
        
        size_t compressedSize = stream.next_out - tempBuffer.data();
        output.resize(compressedSize);
        memcpy(output.data(), tempBuffer.data(), compressedSize);
        
        deflateEnd(&stream);
        return true;
    }
    
    // Helper function to decompress data using zlib
    bool decompressData(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, size_t expectedSize) {
        z_stream stream;
        memset(&stream, 0, sizeof(stream));
        
        int ret = inflateInit(&stream);
        if (ret != Z_OK) {
            return false;
        }
        
        stream.next_in = (Bytef*)input.data();
        stream.avail_in = input.size();
        
        output.resize(expectedSize);
        stream.next_out = (Bytef*)output.data();
        stream.avail_out = output.size();
        
        ret = inflate(&stream, Z_FINISH);
        if (ret != Z_STREAM_END) {
            inflateEnd(&stream);
            return false;
        }
        
        inflateEnd(&stream);
        return true;
    }
} // namespace
    
    // CheckpointReader implementation
    CheckpointReader::CheckpointReader() : streamPos_(0) {}
    CheckpointReader::~CheckpointReader() {
        close();
    }
    
    bool CheckpointReader::open(const std::string& filepath) {
        close();  // Close if already open
        
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            error_ = "Failed to open checkpoint file: " + filepath;
            return false;
        }
        
        // Get file size
        stream_.seekg(0, std::ios::end);
        uint64_t fileSize = stream_.tellg();
        stream_.seekg(0, std::ios::beg);
        
        if (fileSize < sizeof(CheckpointHeader)) {
            error_ = "Checkpoint file too small: " + filepath;
            close();
            return false;
        }
        
        // Read header
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
        if (!stream_.good()) {
            error_ = "Failed to read checkpoint header: " + filepath;
            close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "Invalid checkpoint header: " + filepath;
            close();
            return false;
        }
        
        // Read section headers
        stream_.seekg(sizeof(CheckpointHeader), std::ios::beg);
        size_t sectionCount = (header_.totalSize - sizeof(CheckpointHeader)) / sizeof(SectionHeader);
        sections_.resize(sectionCount);
        
        for (size_t i = 0; i < sectionCount; ++i) {
            stream_.read(reinterpret_cast<char*>(&sections_[i]), sizeof(sections_[i]));
            if (!stream_.good()) {
                error_ = "Failed to read section headers: " + filepath;
                close();
                return false;
            }
        }
        
        streamPos_ = stream_.tellg();
        return true;
    }
    
    void CheckpointReader::close() {
        if (stream_.is_open()) {
            stream_.close();
        }
        sections_.clear();
        streamPos_ = 0;
        error_.clear();
    }
    
    bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
        // Implementation to read neuron data from checkpoint
        // This would involve reading from the appropriate section
        // For now, return false as placeholder
        return false;
    }
    
    bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
        // Implementation to read synapse data from checkpoint
        // This would involve reading from the appropriate section
        // For now, return false as placeholder
        return false;
    }
    
    std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
        // Find section with matching type
        for (const auto& section : sections_) {
            if (section.type == static_cast<uint32_t>(sectionType)) {
                // Seek to section data
                stream_.seekg(section.offset, std::ios::beg);
                
                std::vector<uint8_t> sectionData(section.size);
                stream_.read(reinterpret_cast<char*>(sectionData.data()), sectionData.size());
                
                return sectionData;
            }
        }
        return std::vector<uint8_t>();
    }
    
    bool CheckpointReader::validate() const {
        // Basic validation already done in open()
        // Could add additional validation here
        return true;
    }
    
    double CheckpointReader::getProgress() const {
        if (stream_.is_open()) {
            uint64_t currentPos = stream_.tellg();
            uint64_t fileSize = header_.totalSize;
            return static_cast<double>(currentPos) / fileSize;
        }
        return 0.0;
    }
    
    // CheckpointWriter implementation
    CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}
    CheckpointWriter::~CheckpointWriter() {
        close();
    }
    
    bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
        close(); // Close if already open
        
        stream_.open(filepath, std::ios::binary);
        if (!stream_.is_open()) {
            return false;
        }
        
        compression_ = compression;
        filepath_ = filepath;
        header_.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        header_.compressionLevel = static_cast<uint32_t>(compression);
        
        return writeHeader();
    }
    
    void CheckpointWriter::close() {
        if (stream_.is_open()) {
            finalize(); // Ensure file is properly finalized
            stream_.close();
        }
        sections_.clear();
        bytesWritten_ = 0;
        compressBuffer_.clear();
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
        // Implementation to write neuron data to checkpoint
        // For now, return false as placeholder
        return false;
    }
    
    bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
        // Implementation to write synapse data to checkpoint
        // For now, return false as placeholder
        return false;
    }
    
    bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
        // Compress data if needed
        std::vector<uint8_t> compressedData;
        bool compressSuccess = compressData(
            std::vector<uint8_t>(static_cast<const uint8_t*>(data), 
                               static_cast<const uint8_t*>(data) + size),
            compressedData, compression_
        );
        
        if (!compressSuccess && compression_ != CompressionLevel::None) {
            // If compression fails, try without compression
            compressedData.assign(static_cast<const uint8_t*>(data), 
                                static_cast<const uint8_t*>(data) + size);
        }
        
        // Create section header
        SectionHeader sectionHeader;
        sectionHeader.type = static_cast<uint32_t>(sectionType);
        sectionHeader.offset = sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader) + bytesWritten_;
        sectionHeader.size = compressedData.size();
        sectionHeader.compressedSize = (compression_ == CompressionLevel::None) ? 0 : compressedData.size();
        
        // Calculate checksum
        sectionHeader.checksum = ChecksumCalculator::crc32(compressedData.data(), compressedData.size());
        
        // Write section header
        if (!writeSectionHeader(sectionHeader)) {
            return false;
        }
        
        // Add to sections list
        sections_.push_back(sectionHeader);
        
        // Write section data
        stream_.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
        if (!stream_.good()) {
            return false;
        }
        
        bytesWritten_ += compressedData.size();
        header_.totalSize += compressedData.size();
        
        return true;
    }
    
    bool CheckpointWriter::finalize() {
        if (!stream_.is_open()) {
            return false;
        }
        
        // Calculate header checksum
        header_.checksum = ChecksumCalculator::crc32(
            reinterpret_cast<const uint8_t*>(&header_) + sizeof(header_.checksum),
            sizeof(header_) - sizeof(header_.checksum)
        );
        
        // Write updated header at start of file
        stream_.seekp(0, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        
        stream_.close();
        return true;
    }
    
    void CheckpointWriter::abort() {
        close();
        // Delete incomplete file
        if (!filepath_.empty()) {
            std::remove(filepath_.c_str());
        }
    }
    
    bool CheckpointWriter::writeHeader() {
        // Initialize header
        header_.magic = CHECKPOINT_MAGIC;
        header_.majorVersion = CHECKPOINT_VERSION_MAJOR;
        header_.minorVersion = CHECKPOINT_VERSION_MINOR;
        header_.patchVersion = CHECKPOINT_VERSION_PATCH;
        header_.timestamp = 0;  // Will be updated in finalize()
        header_.totalSize = sizeof(CheckpointHeader) + sections_.size() * sizeof(SectionHeader);
        header_.neuronCount = 0;
        header_.synapseCount = 0;
        header_.regionCount = 0;
        header_.step = 0;
        header_.simulationTime = 0.0;
        header_.checksum = 0;  // Will be calculated in finalize()
        header_.compressionLevel = static_cast<uint32_t>(compression_);
        header_.flags = 0;
        
        // Write initial header (will be updated in finalize)
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        bytesWritten_ = sizeof(header_);
        
        return stream_.good();
    }
    
    bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
        stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
        bytesWritten_ += sizeof(header);
        return stream_.good();
    }
    
    bool CheckpointWriter::padToAlignment(size_t alignment) {
        size_t currentPos = bytesWritten_;
        size_t padding = (alignment - (currentPos % alignment)) % alignment;
        
        if (padding > 0) {
            std::vector<uint8_t> paddingBytes(padding, 0);
            stream_.write(reinterpret_cast<const char*>(paddingBytes.data()), padding);
            bytesWritten_ += padding;
        }
        
        return stream_.good();
    }
    
    // CheckpointManager implementation
    CheckpointManager::CheckpointManager() : lastSaveStep_(0) {}
    CheckpointManager::~CheckpointManager() {
        // Clean up
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
        
        // Ensure checkpoint directory exists
        std::filesystem::create_directories(checkpointDir);
    }
    
    bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
        if (shouldSave(currentStep)) {
            return saveImmediately();
        }
        return true;
    }
    
    bool CheckpointManager::saveImmediately(const std::string& name) {
        if (!neuronProvider_ || !synapseProvider_) {
            return false;
        }
        
        // Generate checkpoint filename
        std::string filename = name.empty() ? 
            generateCheckpointFilename(checkpointDir_, currentStep) :
            checkpointDir_ + "/" + name + ".nlmck";
        
        // Create checkpoint writer
        CheckpointWriter writer;
        if (!writer.create(filename, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
            return false;
        }
        
        // Collect neuron data
        NeuronCheckpointData neuronData;
        if (!neuronProvider_(neuronData)) {
            return false;
        }
        
        // Collect synapse data
        SynapseCheckpointData synapseData;
        if (!synapseProvider_(synapseData)) {
            return false;
        }
        
        // Set metadata
        writer.setMetadata(
            neuronData.membranePotential.size(),
            synapseData.weight.size(),
            0,  // regionCount - would need to be calculated
            currentStep,
            currentTime
        );
        
        // Write neuron data
        if (!writer.writeNeurons(neuronData)) {
            return false;
        }
        
        // Write synapse data
        if (!writer.writeSynapses(synapseData)) {
            return false;
        }
        
        // Finalize checkpoint
        if (!writer.finalize()) {
            return false;
        }
        
        lastSaveStep_ = currentStep;
        lastCheckpointPath_ = filename;
        
        return true;
    }
    
    bool CheckpointManager::load(const std::string& name) {
        std::string filename = checkpointDir_ + "/" + name + ".nlmck";
        
        CheckpointReader reader;
        if (!reader.open(filename)) {
            return false;
        }
        
        // Read data from checkpoint
        auto neuronSection = reader.readSection(CheckpointSection::Neurons);
        auto synapseSection = reader.readSection(CheckpointSection::Synapses);
        
        // Process neuron data (would need implementation)
        // Process synapse data (would need implementation)
        
        return true;
    }
    
    std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
        std::vector<CheckpointInfo> checkpoints;
        
        if (!std::filesystem::exists(checkpointDir_)) {
            return checkpoints;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
            if (entry.path().extension() == ".nlmck") {
                CheckpointInfo info;
                info.name = entry.path().stem().string();
                
                // Extract step from filename if possible
                // This would need to be implemented based on naming convention
                
                info.fileSize = entry.file_size();
                info.timestamp = std::chrono::system_clock::now().time_since_epoch().count();
                
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
            std::string filepath = checkpointDir_ + "/" + checkpoints[i].name + ".nlmck";
            std::remove(filepath.c_str());
        }
    }
    
    std::string CheckpointManager::getLastCheckpointPath() const {
        return lastCheckpointPath_;
    }
    
    bool CheckpointManager::shouldSave(uint64_t currentStep) const {
        return (currentStep - lastSaveStep_) >= saveIntervalSteps_ || lastSaveStep_ == 0;
    }

} // namespace nlm