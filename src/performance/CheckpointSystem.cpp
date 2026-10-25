#include "CheckpointSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <chrono>
#include <numeric>
#include <zlib.h>

namespace nlm {

// Helper functions for checkpoint system
namespace {

// Compression helper functions
hsize_t compressBuffer(const void* source, size_t sourceSize, void* destination, size_t destinationSize) {
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    
    if (deflateInit(&strm, Z_BEST_COMPRESSION) != Z_OK) {
        return 0;
    }
    
    strm.next_in = static_cast<const Bytef*>(source);
    strm.avail_in = static_cast<uInt>(sourceSize);
    
    strm.next_out = static_cast<Bytef*>(destination);
    strm.avail_out = static_cast<uInt>(destinationSize);
    
    if (deflate(&strm, Z_FINISH) != Z_STREAM_END) {
        deflateEnd(&strm);
        return 0;
    }
    
    hsize_t compressedSize = strm.next_out - static_cast<Bytef*>(destination);
    deflateEnd(&strm);
    
    return compressedSize;
}

void decompressBuffer(const void* source, size_t sourceSize, void* destination, size_t destinationSize) {
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    
    if (inflateInit(&strm) != Z_OK) {
        throw std::runtime_error("Failed to initialize decompression");
    }
    
    strm.next_in = static_cast<const Bytef*>(source);
    strm.avail_in = static_cast<uInt>(sourceSize);
    
    strm.next_out = static_cast<Bytef*>(destination);
    strm.avail_out = static_cast<uInt>(destinationSize);
    
    if (inflate(&strm, Z_FINISH) != Z_STREAM_END) {
        inflateEnd(&strm);
        throw std::runtime_error("Failed to decompress data");
    }
    
    inflateEnd(&strm);
}

} // namespace

// CheckpointReader implementation
CheckpointReader::CheckpointReader() : streamPos_(0), error_("Success") {}
CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    if (isOpen()) {
        close();
    }
    
    stream_.open(filepath, std::ios::in | std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to open checkpoint file: " + filepath;
        return false;
    }
    
    // Read header
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
    if (stream_.gcount() != sizeof(CheckpointHeader)) {
        error_ = "Failed to read checkpoint header";
        close();
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header or version mismatch";
        close();
        return false;
    }
    
    // Read sections
    bool success = readSections();
    if (!success) {
        close();
        return false;
    }
    
    return true;
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
    streamPos_ = 0;
    error_ = "Success";
}

bool CheckpointReader::readSections() {
    // Read section count (simplified implementation)
    // In a full implementation, this would read from the footer
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    // Find section in the sections list
    for (const auto& section : sections_) {
        if (section.type == sectionType) {
            stream_.seekg(static_cast<std::streampos>(section.offset));
            std::vector<uint8_t> data(section.size, 0);
            
            if (section.compressedSize > 0) {
                // Decompress
                // Simplified - would need actual decompression buffer
            }
            
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            return data;
        }
    }
    
    return std::vector<uint8_t>();
}

bool CheckpointReader::validate() const {
    // Check header
    if (!header_.validate()) {
        return false;
    }
    
    // Calculate and compare checksum
    // In a full implementation, would read and validate all sections
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (!isOpen() || header_.totalSize == 0) {
        return 0.0;
    }
    
    std::streampos currentPos = stream_.tellg();
    return static_cast<double>(currentPos) / static_cast<double>(header_.totalSize);
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {}
CheckpointWriter::~CheckpointWriter() {
    abort();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    if (isOpen()) {
        abort();
    }
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Ensure directory exists
    std::filesystem::path dir(filepath_.substr(0, filepath_.find_last_of('/')));
    if (!dir.empty()) {
        std::filesystem::create_directories(dir);
    }
    
    stream_.open(filepath_, std::ios::out | std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to create checkpoint file: " + filepath_;
        return false;
    }
    
    return true;
}

void CheckpointWriter::close() {
    if (isOpen()) {
        if (!finalize()) {
            error_ = "Failed to finalize checkpoint";
        }
        stream_.close();
    }
    sections_.clear();
    compressBuffer_.clear();
}

void CheckpointWriter::abort() {
    if (isOpen()) {
        stream_.close();
        std::filesystem::remove(filepath_);
    }
    sections_.clear();
    compressBuffer_.clear();
}

void CheckpointWriter::setMetadata(uint64_t neuronCount, uint64_t synapseCount,
                                 uint64_t regionCount, uint64_t step, double simTime) {
    header_.neuronCount = neuronCount;
    header_.synapseCount = synapseCount;
    header_.regionCount = regionCount;
    header_.step = step;
    header_.simulationTime = simTime;
    header_.timestamp = static_cast<uint64_t>(std::chrono::system_clock::now().time_since_epoch().count());
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    // Compress if needed
    size_t totalSize = 0;
    for (const auto& vec : {
        data.membranePotential, data.restingPotential, data.threshold,
        data.resetPotential, data.leakConductance, data.lastSpikeTime
    }) {
        totalSize += vec.size() * sizeof(float);
    }
    
    // Add other arrays
    totalSize += data.firingState.size() * sizeof(uint8_t);
    totalSize += data.refractoryRemaining.size() * sizeof(uint32_t);
    totalSize += data.refractoryPeriod.size() * sizeof(uint32_t);
    totalSize += data.neuronType.size() * sizeof(uint64_t);
    totalSize += data.regionId.size() * sizeof(uint64_t);
    totalSize += data.populationId.size() * sizeof(uint64_t);
    
    if (compression_ != CompressionLevel::None && totalSize > 1024) {
        // Compress data
        // Simplified - would need proper packing of all arrays
        // For now, just write uncompressed
    }
    
    // Write section header
    SectionHeader sectionHeader;
    sectionHeader.type = CheckpointSection::Neurons;
    sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
    sectionHeader.size = static_cast<uint64_t>(totalSize);
    sectionHeader.compressedSize = 0;
    sectionHeader.checksum = 0;
    
    if (!writeSectionHeader(sectionHeader)) {
        return false;
    }
    
    // Write all neuron data
    // Implementation would serialize all vectors in the proper order
    return true;
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    // Similar implementation to writeNeurons
    size_t totalSize = 0;
    for (const auto& vec : {
        data.weight, data.eligibilityTrace, data.efficacy,
        data.shortTermDepression, data.shortTermFacilitation
    }) {
        totalSize += vec.size() * sizeof(float);
    }
    
    totalSize += data.sourceNeuron.size() * sizeof(uint64_t);
    totalSize += data.destinationNeuron.size() * sizeof(uint64_t);
    totalSize += data.delay.size() * sizeof(uint32_t);
    totalSize += data.synapseType.size() * sizeof(uint8_t);
    totalSize += data.plasticityFlags.size() * sizeof(uint8_t);
    
    SectionHeader sectionHeader;
    sectionHeader.type = CheckpointSection::Synapses;
    sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
    sectionHeader.size = static_cast<uint64_t>(totalSize);
    sectionHeader.compressedSize = 0;
    sectionHeader.checksum = 0;
    
    if (!writeSectionHeader(sectionHeader)) {
        return false;
    }
    
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    SectionHeader sectionHeader;
    sectionHeader.type = sectionType;
    sectionHeader.offset = static_cast<uint64_t>(stream_.tellp());
    sectionHeader.size = static_cast<uint64_t>(size);
    
    if (compression_ != CompressionLevel::None) {
        // Calculate compressed size
        size_t compressedSize = compressBuffer(data, size, compressBuffer_.data(), compressBuffer_.size());
        if (compressedSize > 0 && compressedSize < size) {
            sectionHeader.compressedSize = static_cast<uint64_t>(compressedSize);
            stream_.write(reinterpret_cast<const char*>(compressBuffer_.data()), compressedSize);
        } else {
            sectionHeader.compressedSize = 0;
            stream_.write(reinterpret_cast<const char*>(data), size);
        }
    } else {
        sectionHeader.compressedSize = 0;
        stream_.write(reinterpret_cast<const char*>(data), size);
    }
    
    // Calculate checksum (simplified)
    sectionHeader.checksum = ChecksumCalculator::crc32(data, size);
    
    // Write section header
    if (!writeSectionHeader(sectionHeader)) {
        return false;
    }
    
    bytesWritten_ += sectionHeader.size;
    return true;
}

bool CheckpointWriter::finalize() {
    if (!isOpen()) {
        return false;
    }
    
    // Calculate total size
    header_.totalSize = static_cast<uint64_t>(stream_.tellp());
    
    // Calculate and set checksum (simplified)
    // In a full implementation, would read all data sections
    
    // Write header
    if (!writeHeader()) {
        return false;
    }
    
    // Write footer (section count and directory)
    // Simplified implementation - would need full section directory
    
    return true;
}

bool CheckpointWriter::writeHeader() {
    // Write header to beginning of file
    stream_.seekp(0);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    return !stream_.fail();
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    sections_.push_back(header);
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    return !stream_.fail();
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t current = static_cast<size_t>(stream_.tellp());
    size_t padding = (alignment - (current % alignment)) % alignment;
    
    if (padding > 0) {
        std::vector<uint8_t> pad(padding, 0);
        stream_.write(reinterpret_cast<const char*>(pad.data()), padding);
    }
    
    return !stream_.fail();
}

// CheckpointManager implementation
CheckpointManager::CheckpointManager() : saveIntervalSteps_(10000), maxCheckpoints_(10),
    compress_(true), lastSaveStep_(0), lastCheckpointPath_(""),
    neuronProvider_(nullptr), synapseProvider_(nullptr) {}

CheckpointManager::~CheckpointManager() {
    // Clean up checkpoint directory
    if (!checkpointDir_.empty()) {
        pruneOldCheckpoints(0); // Remove all
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
    
    // Create directory
    std::filesystem::path dir(checkpointDir_);
    std::filesystem::create_directories(dir);
    
    lastSaveStep_ = 0;
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return false;
    }
    
    std::string name = "checkpoint_" + std::to_string(currentStep);
    return saveImmediately(name);
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        error_ = "Checkpoint providers not set";
        return false;
    }
    
    std::string filepath = checkpointDir_;
    if (!filepath.empty() && filepath.back() != '/') {
        filepath += '/';
    }
    filepath += name + ".nlmckpt";
    
    CheckpointWriter writer;
    CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
    
    if (!writer.create(filepath, compression)) {
        return false;
    }
    
    // Write metadata
    NeuronCheckpointData neuronData;
    SynapseCheckpointData synapseData;
    
    uint64_t neuronCount = 0, synapseCount = 0;
    
    if (neuronProvider_(neuronData)) {
        neuronCount = neuronData.membranePotential.size();
    }
    
    if (synapseProvider_(synapseData)) {
        synapseCount = synapseData.weight.size();
    }
    
    writer.setMetadata(neuronCount, synapseCount, 1, lastSaveStep_, currentTime);
    
    if (!writer.writeNeurons(neuronData)) {
        return false;
    }
    
    if (!writer.writeSynapses(synapseData)) {
        return false;
    }
    
    writer.close();
    
    lastSaveStep_ = lastSaveStep_; // Update
    lastCheckpointPath_ = filepath;
    
    // Prune old checkpoints
    pruneOldCheckpoints(maxCheckpoints_);
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_;
    if (!filepath.empty() && filepath.back() != '/') {
        filepath += '/';
    }
    filepath += name + ".nlmckpt";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        return false;
    }
    
    // In a full implementation, would load checkpoint data
    // For now, just validate
    
    reader.close();
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (!std::filesystem::exists(checkpointDir_)) {
        return checkpoints;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.path().extension() == ".nlmckpt") {
            CheckpointInfo info;
            info.name = entry.path().stem().string();
            
            // Parse step from name (checkpoint_12345)
            size_t underscore = info.name.find_last_of('_');
            if (underscore != std::string::npos && info.name.size() > underscore + 1) {
                info.step = std::stoull(info.name.substr(underscore + 1));
            }
            
            info.fileSize = entry.file_size();
            info.timestamp = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::time_point(
                    std::chrono::duration_cast<std::chrono::system_clock::duration>(
                        std::chrono::seconds(entry.last_write_time().time_since_epoch().count())
                    )
                )
            );
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by step (most recent first)
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
    
    // Keep the most recent 'keepCount' checkpoints
    std::vector<std::string> toDelete;
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        toDelete.push_back(checkpointDir_ + '/' + checkpoints[i].name + ".nlmckpt");
    }
    
    for (const auto& filepath : toDelete) {
        try {
            std::filesystem::remove(filepath);
        } catch (const std::exception& e) {
            // Log error but continue
        }
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep <= lastSaveStep_) {
        return false;
    }
    
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

} // namespace nlm