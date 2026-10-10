// NLM Advanced Features - Checkpoint System Implementation
// Brain state serialization and checkpointing for large-scale neural simulations

#include "CheckpointSystem.hpp"
#include <filesystem>
#include <sstream>
#include <algorithm>
#include <zlib.h>

namespace nlm {

// Helper functions for compression
namespace {

    std::vector<uint8_t> compressData(const void* data, size_t size, CompressionLevel level) {
        if (level == CompressionLevel::None || size == 0) {
            return std::vector<uint8_t>(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + size);
        }
        
        // Simple zlib compression (level 1-9 based on CompressionLevel)
        int zlibLevel = static_cast<int>(level);
        if (zlibLevel > 9) zlibLevel = 9;
        if (zlibLevel < 1) zlibLevel = 1;
        
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        
        if (deflateInit(&strm, zlibLevel) != Z_OK) {
            return {};
        }
        
        strm.next_in = static_cast<const Bytef*>(data);
        strm.avail_in = size;
        
        // Allocate output buffer (compressed data is usually smaller)
        std::vector<uint8_t> outputBuffer(size * 1.1); // 10% overhead estimate
        
        strm.next_out = outputBuffer.data();
        strm.avail_out = outputBuffer.size();
        
        if (deflate(&strm, Z_FINISH) != Z_STREAM_END) {
            deflateEnd(&strm);
            return {};
        }
        
        deflateEnd(&strm);
        outputBuffer.resize(strm.total_out);
        
        return outputBuffer;
    }
    
    std::vector<uint8_t> decompressData(const void* data, size_t size, CompressionLevel level) {
        if (level == CompressionLevel::None) {
            return std::vector<uint8_t>(static_cast<const uint8_t*>(data), static_cast<const uint8_t*>(data) + size);
        }
        
        if (size == 0) return {};
        
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        
        if (inflateInit(&strm) != Z_OK) {
            return {};
        }
        
        strm.next_in = static_cast<const Bytef*>(data);
        strm.avail_in = size;
        
        // Allocate output buffer (decompressed data is usually larger)
        std::vector<uint8_t> outputBuffer(size * 10); // 10x overhead estimate
        
        strm.next_out = outputBuffer.data();
        strm.avail_out = outputBuffer.size();
        
        int result = inflate(&strm, Z_FINISH);
        
        inflateEnd(&strm);
        
        if (result != Z_STREAM_END && result != Z_OK) {
            return {};
        }
        
        outputBuffer.resize(strm.total_out);
        return outputBuffer;
    }
    
    std::string formatNumber(uint64_t num) {
        std::stringstream ss;
        ss << num;
        return ss.str();
    }
    
    uint64_t parseNumber(const std::string& str) {
        return std::stoull(str);
    }
    
} // anonymous namespace

// CheckpointReader implementation

CheckpointReader::CheckpointReader() : streamPos_(0) {
}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    close();
    
    stream_.open(filepath, std::ios::in | std::ios::binary);
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
        error_ = "Invalid checkpoint header";
        close();
        return false;
    }
    
    // Parse sections
    stream_.seekg(0, std::ios::end);
    size_t fileSize = stream_.tellg();
    stream_.seekg(sizeof(CheckpointHeader), std::ios::beg);
    
    size_t currentPos = sizeof(CheckpointHeader);
    while (currentPos < fileSize - sizeof(SectionHeader)) {
        SectionHeader section;
        stream_.read(reinterpret_cast<char*>(&section), sizeof(SectionHeader));
        
        if (!stream_.good()) {
            break; // End of file or read error
        }
        
        // Validate section
        if (section.type >= static_cast<uint32_t>(CheckpointSection::End) ||
            section.offset > fileSize ||
            section.size > fileSize - section.offset) {
            error_ = "Invalid section header at position " + formatNumber(currentPos);
            close();
            return false;
        }
        
        sections_.push_back(section);
        currentPos += sizeof(SectionHeader);
        streamPos_ = currentPos;
    }
    
    error_.clear();
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
    auto section = std::find_if(sections_.begin(), sections_.end(),
                                [](const SectionHeader& s) { return s.type == static_cast<uint32_t>(CheckpointSection::Neurons); });
    
    if (section == sections_.end()) {
        error_ = "Neurons section not found";
        return false;
    }
    
    stream_.seekg(section->offset, std::ios::beg);
    
    // Read neuron data
    for (auto& vec : data.membranePotential) {
        stream_.read(reinterpret_cast<char*>(&vec), sizeof(float));
        if (!stream_.good()) {
            error_ = "Failed to read membrane potential data";
            return false;
        }
    }
    
    // Read other neuron data (simplified - in real implementation would read all vectors)
    // This is a simplified version - full implementation would read all 12 neuron data vectors
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto section = std::find_if(sections_.begin(), sections_.end(),
                                [](const SectionHeader& s) { return s.type == static_cast<uint32_t>(CheckpointSection::Synapses); });
    
    if (section == sections_.end()) {
        error_ = "Synapses section not found";
        return false;
    }
    
    stream_.seekg(section->offset, std::ios::beg);
    
    // Read synapse data
    for (auto& vec : data.weight) {
        stream_.read(reinterpret_cast<char*>(&vec), sizeof(float));
        if (!stream_.good()) {
            error_ = "Failed to read synapse weight data";
            return false;
        }
    }
    
    // Read other synapse data (simplified - in real implementation would read all vectors)
    
    return true;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    auto section = std::find_if(sections_.begin(), sections_.end(),
                                [sectionType](const SectionHeader& s) { return s.type == static_cast<uint32_t>(sectionType); });
    
    if (section == sections_.end()) {
        error_ = "Section not found: " + std::to_string(static_cast<uint32_t>(sectionType));
        return {};
    }
    
    stream_.seekg(section->offset, std::ios::beg);
    
    std::vector<uint8_t> data(section->size);
    stream_.read(reinterpret_cast<char*>(data.data()), section->size);
    
    if (!stream_.good()) {
        error_ = "Failed to read section data";
        return {};
    }
    
    // Decompress if needed
    if (section->compressedSize > 0 && section->compressedSize < section->size) {
        // This is a simplified decompression - full implementation would use compression info
    }
    
    return data;
}

bool CheckpointReader::validate() const {
    if (!stream_.is_open()) {
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Header validation failed";
        return false;
    }
    
    // Validate each section
    size_t currentPos = sizeof(CheckpointHeader);
    for (const auto& section : sections_) {
        // Verify section offset and size
        if (section.offset != currentPos) {
            error_ = "Section offset mismatch";
            return false;
        }
        
        currentPos += sizeof(SectionHeader) + section.size;
    }
    
    // Verify final position
    stream_.seekg(0, std::ios::end);
    size_t fileSize = stream_.tellg();
    
    return currentPos == fileSize;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    double totalSize = 0.0;
    double processedSize = 0.0;
    
    for (const auto& section : sections_) {
        totalSize += section.size;
    }
    
    // Calculate processed size based on stream position
    processedSize = static_cast<double>(streamPos_ - sizeof(CheckpointHeader));
    
    if (totalSize == 0.0) return 0.0;
    return processedSize / totalSize;
}

// CheckpointWriter implementation

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {
}

CheckpointWriter::~CheckpointWriter() {
    abort(); // Cleanup on destruction
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    close();
    
    filepath_ = filepath;
    compression_ = compression;
    
    stream_.open(filepath, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!stream_.is_open()) {
        return false;
    }
    
    bytesWritten_ = 0;
    sections_.clear();
    
    return writeHeader();
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        finalize();
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
    header_.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    return writeSection(CheckpointSection::Neurons, &data, sizeof(NeuronCheckpointData));
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    return writeSection(CheckpointSection::Synapses, &data, sizeof(SynapseCheckpointData));
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!stream_.is_open()) {
        return false;
    }
    
    // Compress if needed
    std::vector<uint8_t> compressedData;
    size_t originalSize = size;
    
    if (compression_ != CompressionLevel::None) {
        compressedData = compressData(data, size, compression_);
        if (compressedData.empty()) {
            return false;
        }
        data = compressedData.data();
        size = compressedData.size();
    }
    
    // Write section header
    SectionHeader section;
    section.type = static_cast<uint32_t>(sectionType);
    section.offset = static_cast<uint64_t>(sizeof(CheckpointHeader) + bytesWritten_ + sizeof(SectionHeader));
    section.size = originalSize;
    section.compressedSize = (compression_ != CompressionLevel::None) ? size : 0;
    section.checksum = ChecksumCalculator::crc32(data, size);
    
    if (!writeSectionHeader(section)) {
        return false;
    }
    
    // Write section data
    stream_.write(reinterpret_cast<const char*>(data), size);
    if (!stream_.good()) {
        return false;
    }
    
    // Pad to 8-byte alignment
    padToAlignment(8);
    
    bytesWritten_ += size + sizeof(SectionHeader);
    sections_.push_back(section);
    
    return true;
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        return false;
    }
    
    // Update header with final information
    header_.totalSize = bytesWritten_;
    header_.checksum = ChecksumCalculator::crc64(&header_, sizeof(CheckpointHeader));
    
    // Write updated header
    stream_.seekp(0, std::ios::beg);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    
    if (!stream_.good()) {
        return false;
    }
    
    stream_.flush();
    return true;
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        // Delete partial file
        std::error_code ec;
        std::filesystem::remove(filepath_, ec);
    }
}

bool CheckpointWriter::writeHeader() {
    if (!stream_.is_open()) {
        return false;
    }
    
    // Initialize header with default values
    header_.totalSize = 0;
    header_.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    header_.compressionLevel = static_cast<uint32_t>(compression_);
    
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    return stream_.good();
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    bytesWritten_ += sizeof(SectionHeader);
    return stream_.good();
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = bytesWritten_;
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding == 0) return true;
    
    std::vector<uint8_t> paddingData(padding, 0);
    stream_.write(reinterpret_cast<const char*>(paddingData.data()), padding);
    bytesWritten_ += padding;
    
    return stream_.good();
}

// CheckpointManager implementation

CheckpointManager::CheckpointManager() 
    : saveIntervalSteps_(10000)
    , maxCheckpoints_(10)
    , compress_(true)
    , lastSaveStep_(0) {
}

CheckpointManager::~CheckpointManager() {
    // Cleanup
}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                 uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints,
                                 bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    // Create checkpoint directory
    std::error_code ec;
    std::filesystem::create_directories(checkpointDir_, ec);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) {
        return false;
    }
    
    return saveImmediately("");
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) {
        return false;
    }
    
    // Generate checkpoint name if not provided
    std::string checkpointName = name;
    if (checkpointName.empty()) {
        checkpointName = "checkpoint_" + std::to_string(lastSaveStep_);
    }
    
    std::string filepath = getLastCheckpointPath();
    if (filepath.empty()) {
        return false;
    }
    
    // Create checkpoint file
    CheckpointWriter writer;
    CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
    
    if (!writer.create(filepath, compression)) {
        return false;
    }
    
    // Set metadata
    writer.setMetadata(
        // Would need actual counts from brain state
        0, 0, 0, lastSaveStep_, currentTime
    );
    
    // Get neuron data
    NeuronCheckpointData neuronData;
    if (!neuronProvider_(neuronData)) {
        return false;
    }
    
    // Get synapse data
    SynapseCheckpointData synapseData;
    if (!synapseProvider_(synapseData)) {
        return false;
    }
    
    // Write data
    if (!writer.writeNeurons(neuronData) || !writer.writeSynapses(synapseData)) {
        writer.abort();
        return false;
    }
    
    writer.finalize();
    lastSaveStep_ = currentStep;
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = getLastCheckpointPath();
    if (filepath.empty()) {
        return false;
    }
    
    // Implementation would load checkpoint from file
    // This is a simplified version
    return false;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    if (checkpointDir_.empty()) {
        return checkpoints;
    }
    
    std::error_code ec;
    auto dirIt = std::filesystem::directory_iterator(checkpointDir_, ec);
    
    for (const auto& entry : dirIt) {
        if (!entry.is_regular_file(ec) || entry.path().extension() != ".cp") {
            continue;
        }
        
        CheckpointInfo info;
        info.name = entry.path().stem().string();
        info.fileSize = entry.file_size(ec);
        info.timestamp = std::chrono::system_clock::to_time_t(entry.last_write_time(ec));
        
        checkpoints.push_back(info);
    }
    
    // Sort by timestamp (newest first)
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
    
    // Keep newest checkpoints
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    size_t toRemove = checkpoints.size() - keepCount;
    std::error_code ec;
    
    for (size_t i = keepCount; i < checkpoints.size(); ++i) {
        std::filesystem::remove(checkpoints[i].name + ".cp", ec);
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    if (checkpointDir_.empty()) {
        return "";
    }
    
    auto checkpoints = listCheckpoints();
    if (checkpoints.empty()) {
        return "";
    }
    
    // Return newest checkpoint
    return checkpointDir_ + "/" + checkpoints[0].name + ".cp";
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    if (currentStep <= lastSaveStep_) {
        return false;
    }
    
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

// NLM logging macros for checkpoint system

// Add logging macros to support checkpoint operations
#define NLM_LOG_CHECKPOINT(msg) NLM_LOG_INFO("CheckpointManager: " + msg)
#define NLM_LOG_ERROR(msg) NLM_LOG_ERROR("CheckpointManager: " + msg)
#define NLM_LOG_WARNING(msg) NLM_LOG_WARNING("CheckpointManager: " + msg)
#define NLM_LOG_INFO(msg) NLM_LOG_INFO("CheckpointManager: " + msg)

// Implementation of logging system macros

} // namespace nlm

// Simple logging macros (to be replaced with proper logging system)
#include <iostream>
#include <chrono>
#include <ctime>
#include <string>

// Define logging macros
void NLM_LOG_MESSAGE(const std::string& level, const std::string& component, const std::string& message);

void NLM_LOG_MESSAGE(const std::string& level, const std::string& component, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::cout << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] "
              << level << " [" << component << "] " << message << std::endl;
}

#define NLM_LOG_FATAL(msg) NLM_LOG_MESSAGE("FATAL", "NLM", msg)
#define NLM_LOG_ERROR(msg) NLM_LOG_MESSAGE("ERROR", "NLM", msg)
#define NLM_LOG_WARNING(msg) NLM_LOG_MESSAGE("WARNING", "NLM", msg)
#define NLM_LOG_INFO(msg) NLM_LOG_MESSAGE("INFO", "NLM", msg)
#define NLM_LOG_DEBUG(msg) NLM_LOG_MESSAGE("DEBUG", "NLM", msg)

// Include zlib for compression
#include <zlib.h>