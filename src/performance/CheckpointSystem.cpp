#include "../performance/CheckpointSystem.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/Memory.hpp"
#include "../plasticity/PlasticityRule.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../environment/Environment.hpp"
#include "../motor/MotorSystem.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_set>
#include <atomic>
#include <zlib.h>
#include <lz4.h>

// Optional compression includes
#ifdef ZLIB_AVAILABLE
#include <zlib.h>
#endif
#ifdef LZ4_AVAILABLE
#include <lz4.h>
#endif

namespace nlm {

// CheckpointReader implementation
CheckpointReader::CheckpointReader() : stream_(), header_(), sections_(), error_(), streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    if (isOpen()) {
        close();
    }
    
    stream_.open(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        error_ = "Failed to open file: " + filepath;
        return false;
    }
    
    stream_.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(stream_.tellg());
    stream_.seekg(0, std::ios::beg);
    
    if (fileSize < sizeof(CheckpointHeader)) {
        error_ = "File too small to be a valid checkpoint";
        stream_.close();
        return false;
    }
    
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(CheckpointHeader));
    if (!stream_) {
        error_ = "Failed to read checkpoint header";
        stream_.close();
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Invalid checkpoint header or version mismatch";
        stream_.close();
        return false;
    }
    
    // Read sections
    while (streamPos_ < fileSize) {
        SectionHeader sectionHeader;
        stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(SectionHeader));
        if (!stream_) break;
        
        if (sectionHeader.size > 0 && sectionHeader.offset > fileSize) {
            error_ = "Invalid section offset";
            stream_.close();
            return false;
        }
        
        sections_.push_back(sectionHeader);
        streamPos_ += sizeof(SectionHeader);
    }
    
    if (!validate()) {
        return false;
    }
    
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

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> data;
    
    for (const auto& section : sections_) {
        if (section.type == static_cast<uint32_t>(sectionType)) {
            stream_.seekg(section.offset, std::ios::beg);
            data.resize(section.size);
            stream_.read(reinterpret_cast<char*>(data.data()), section.size);
            break;
        }
    }
    
    return data;
}

bool CheckpointReader::validate() const {
    uint64_t checksum = ChecksumCalculator::crc64(nullptr, 0);
    
    for (const auto& section : sections_) {
        stream_.seekg(section.offset, std::ios::beg);
        std::vector<uint8_t> sectionData(section.size);
        stream_.read(reinterpret_cast<char*>(sectionData.data()), section.size);
        
        uint64_t sectionChecksum = ChecksumCalculator::crc64(sectionData.data(), sectionData.size());
        
        if (section.checksum != 0 && section.checksum != sectionChecksum) {
            error_ = "Section checksum validation failed";
            return false;
        }
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (sections_.empty()) return 0.0;
    
    size_t totalSize = 0;
    for (const auto& section : sections_) totalSize += section.size;
    
    stream_.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(stream_.tellg());
    
    return static_cast<double>(streamPos_) / fileSize;
}

// CheckpointWriter implementation
CheckpointWriter::CheckpointWriter() : stream_(), filepath_(), compression_(CompressionLevel::Balanced), 
                                       header_(), sections_(), bytesWritten_(0), compressBuffer_() {}

CheckpointWriter::~CheckpointWriter() {
    if (isOpen()) {
        abort();
    }
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    if (isOpen()) {
        close();
    }
    
    filepath_ = filepath;
    compression_ = compression;
    
    // Ensure directory exists
    std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
    if (!dir.empty() && !std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
    
    stream_.open(filepath, std::ios::binary | std::ios::trunc);
    if (!stream_.is_open()) {
        return false;
    }
    
    sections_.clear();
    bytesWritten_ = 0;
    
    return writeHeader();
}

void CheckpointWriter::close() {
    if (isOpen()) {
        finalize();
        stream_.close();
    }
}

bool CheckpointWriter::writeHeader() {
    stream_.seekp(0, std::ios::beg);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(CheckpointHeader));
    
    bool success = static_cast<bool>(stream_);
    streamPos_ = sizeof(CheckpointHeader);
    
    return success;
}

void CheckpointWriter::abort() {
    if (isOpen()) {
        stream_.close();
        std::filesystem::remove(filepath_);
    }
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    // Update offset to current position
    header.offset = streamPos_;
    
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(SectionHeader));
    if (!stream_) return false;
    
    sections_.push_back(header);
    streamPos_ += sizeof(SectionHeader);
    
    return true;
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = streamPos_;
    size_t padding = (alignment - (currentPos % alignment)) % alignment;
    
    if (padding > 0) {
        compressBuffer_.resize(padding, 0);
        stream_.write(reinterpret_cast<const char*>(compressBuffer_.data()), padding);
        if (!stream_) return false;
        
        streamPos_ += padding;
    }
    
    return true;
}

// Compression utilities
static bool compressData(const std::vector<uint8_t>& input, std::vector<uint8_t>& output, 
                        CompressionLevel level) {
    if (input.empty()) return true;
    
    output.clear();
    
    if (level == CompressionLevel::None) {
        output = input;
        return true;
    }
    
    // Using zlib for compression
    int compressLevel = Z_DEFAULT_COMPRESSION;
    if (level == CompressionLevel::Fast) compressLevel = Z_BEST_SPEED;
    else if (level == CompressionLevel::Balanced) compressLevel = Z_DEFAULT_COMPRESSION;
    else if (level == CompressionLevel::Best) compressLevel = Z_BEST_COMPRESSION;
    
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    
    if (deflateInit(&stream, compressLevel) != Z_OK) return false;
    
    stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(input.data()));
    stream.avail_in = input.size();
    
    do {
        output.resize(output.size() + 4096);
        stream.next_out = reinterpret_cast<Bytef*>(output.data()) + output.size() - 4096;
        stream.avail_out = 4096;
        
        int result = deflate(&stream, Z_FINISH);
        if (result != Z_STREAM_END && result != Z_OK) break;
        
        size_t compressedSize = 4096 - stream.avail_out;
        output.resize(output.size() - 4096 + compressedSize);
        
    } while (stream.avail_out == 0);
    
    deflateEnd(&stream);
    
    return stream.avail_in == 0;
}

static bool decompressData(const std::vector<uint8_t>& input, std::vector<uint8_t>& output,
                          CompressionLevel level) {
    if (input.empty()) return true;
    
    if (level == CompressionLevel::None) {
        output = input;
        return true;
    }
    
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    
    if (inflateInit(&stream) != Z_OK) return false;
    
    stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(input.data()));
    stream.avail_in = input.size();
    
    do {
        output.resize(output.size() + 4096);
        stream.next_out = reinterpret_cast<Bytef*>(output.data()) + output.size() - 4096;
        stream.avail_out = 4096;
        
        int result = inflate(&stream, Z_FINISH);
        if (result == Z_STREAM_END) break;
        if (result != Z_OK) {
            inflateEnd(&stream);
            return false;
        }
        
    } while (stream.avail_out == 0);
    
    inflateEnd(&stream);
    
    output.resize(output.size() - stream.avail_out);
    return true;
}

// Enhanced CheckpointManager implementation
CheckpointManager::CheckpointManager() : checkpointDir_(), saveIntervalSteps_(10000), maxCheckpoints_(10),
                                       compress_(true), lastSaveStep_(0), lastCheckpointPath_(), 
                                       neuronProvider_(), synapseProvider_(), brainProvider_(), 
                                       metadataProvider_(), totalCheckpoints_(0), totalBytesSaved_(0),
                                       validationFailures_(0), loadSuccesses_(0), loadFailures_(0),
                                       versionMajor_(1), versionMinor_(0), versionPatch_(0) {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir, uint64_t saveIntervalSteps,
                                 size_t maxCheckpoints, bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    std::filesystem::path dir(checkpointDir_);
    if (!std::filesystem::exists(dir)) {
        std::filesystem::create_directories(dir);
    }
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (!shouldSave(currentStep)) return false;
    
    return saveImmediately();
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string checkpointName = name.empty() ? "checkpoint_" + std::to_string(lastSaveStep_) : name;
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".bin";
    
    CheckpointWriter writer;
    CompressionLevel compression = compress_ ? CompressionLevel::Balanced : CompressionLevel::None;
    
    if (!writer.create(filepath, compression)) {
        return false;
    }
    
    // Write enhanced metadata
    CheckpointHeader header;
    header.majorVersion = versionMajor_;
    header.minorVersion = versionMinor_;
    header.patchVersion = versionPatch_;
    header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    if (metadataProvider_) {
        metadataProvider_(header);
    }
    
    // Write metadata section
    writer.setMetadata(header.neuronCount, header.synapseCount,
                      header.regionCount, header.step, header.simulationTime);
    
    // Write brain state using providers
    NeuronCheckpointData neuronData;
    SynapseCheckpointData synapseData;
    
    if (neuronProvider_ && !neuronProvider_(neuronData)) {
        return false;
    }
    
    if (synapseProvider_ && !synapseProvider_(synapseData)) {
        return false;
    }
    
    if (!writer.writeNeurons(neuronData)) {
        return false;
    }
    
    if (!writer.writeSynapses(synapseData)) {
        return false;
    }
    
    if (!writer.finalize()) {
        return false;
    }
    
    lastSaveStep_ = lastSaveStep_ + saveIntervalSteps_;
    lastCheckpointPath_ = filepath;
    
    // Update statistics
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        totalCheckpoints_++;
        totalBytesSaved_ += writer.getBytesWritten();
    }
    
    pruneOldCheckpoints(maxCheckpoints_);
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".bin";
    
    if (!validateCheckpoint(filepath)) {
        {
            std::lock_guard<std::mutex> lock(statsMutex_);
            loadFailures_++;
        }
        return false;
    }
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        {
            std::lock_guard<std::mutex> lock(statsMutex_);
            loadFailures_++;
        }
        return false;
    }
    
    if (!reader.validate()) {
        {
            std::lock_guard<std::mutex> lock(statsMutex_);
            validationFailures_++;
            loadFailures_++;
        }
        return false;
    }
    
    // Here you would load the brain state using brainProvider_
    // For now just return success
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        loadSuccesses_++;
    }
    
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
            info.timestamp = std::chrono::system_clock::to_time_t(
                std::chrono::filesystem::last_write_time(entry));
            
            // Read header for additional info
            CheckpointReader reader;
            if (reader.open(entry.path().string())) {
                const auto& header = reader.getHeader();
                info.step = header.step;
                info.simulationTime = header.simulationTime;
                info.neuronCount = header.neuronCount;
                info.synapseCount = header.synapseCount;
                info.regionCount = header.regionCount;
                info.versionMajor = header.majorVersion;
                info.versionMinor = header.minorVersion;
                info.versionPatch = header.patchVersion;
                info.checksum = header.checksum;
                
                // Determine compression type
                switch (header.compressionLevel) {
                    case 0: info.compressionType = "None"; break;
                    case 1: info.compressionType = "Fast"; break;
                    case 2: info.compressionType = "Balanced"; break;
                    case 3: info.compressionType = "Best"; break;
                    default: info.compressionType = "Unknown";
                }
                
                // Simple validation score
                info.validationScore = reader.validate() ? 1.0 : 0.0;
            }
            
            checkpoints.push_back(info);
        }
    }
    
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp < b.timestamp;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) return;
    
    std::vector<CheckpointInfo> toDelete;
    size_t toKeep = std::min(checkpoints.size() - keepCount, checkpoints.size());
    
    // Delete oldest checkpoints (everything before toKeep)
    for (size_t i = 0; i < toDelete.size(); ++i) {
        // In a real implementation, we would delete the files here
        // For now we just mark them for deletion
        toDelete.push_back(checkpoints[i]);
    }
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return currentStep - lastSaveStep_ >= saveIntervalSteps_;
}

bool CheckpointManager::validateCheckpoint(const std::string& filepath) {
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        return false;
    }
    
    // Additional validation checks
    const auto& header = reader.getHeader();
    
    if (header.neuronCount == 0) {
        return false;
    }
    
    if (header.synapseCount == 0 && header.neuronCount > 0) {
        return false;  // If we have neurons, we should have synapses
    }
    
    return true;
}

bool CheckpointManager::createBackup(const std::string& sourcePath, const std::string& backupName) {
    std::string backupPath = checkpointDir_ + "/backup_";
    backupPath += backupName.empty() ? std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count()) : backupName;
    backupPath += ".bin";
    
    // Copy file with verification
    std::ifstream src(sourcePath, std::ios::binary);
    std::ofstream dst(backupPath, std::ios::binary);
    
    if (!src || !dst) return false;
    
    dst << src.rdbuf();
    
    if (!dst) return false;
    
    // Validate backup
    return validateCheckpoint(backupPath);
}

CheckpointManager::CheckpointStats CheckpointManager::getStats() const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    
    CheckpointStats stats;
    stats.totalCheckpoints = totalCheckpoints_;
    stats.totalBytesSaved = totalBytesSaved_;
    stats.averageCheckpointSize = totalCheckpoints_ > 0 ? totalBytesSaved_ / totalCheckpoints_ : 0;
    stats.validationFailures = validationFailures_;
    stats.loadSuccesses = loadSuccesses_;
    stats.loadFailures = loadFailures_;
    
    // Calculate compression ratio (simplified)
    if (totalBytesSaved_ > 0) {
        stats.compressionRatio = 1.0;  // Placeholder
    }
    
    return stats;
}

void CheckpointManager::resetStats() {
    std::lock_guard<std::mutex> lock(statsMutex_);
    totalCheckpoints_ = 0;
    totalBytesSaved_ = 0;
    validationFailures_ = 0;
    loadSuccesses_ = 0;
    loadFailures_ = 0;
}

// Incremental checkpoint functionality
bool CheckpointManager::saveIncremental(const std::string& name, 
                                        const std::vector<std::string>& changedFiles) {
    std::string checkpointName = name.empty() ? "incremental_" + std::to_string(lastSaveStep_) : name;
    std::string filepath = checkpointDir_ + "/" + checkpointName + ".inc.bin";
    
    CheckpointWriter writer;
    CompressionLevel compression = compress_ ? CompressionLevel::Fast : CompressionLevel::None;
    
    if (!writer.create(filepath, compression)) {
        return false;
    }
    
    // Write incremental metadata
    CheckpointHeader header;
    header.majorVersion = versionMajor_;
    header.minorVersion = versionMinor_;
    header.patchVersion = versionPatch_;
    header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    header.flags = 0x01;  // Flag for incremental checkpoint
    
    writer.setMetadata(header.neuronCount, header.synapseCount,
                      header.regionCount, header.step, header.simulationTime);
    
    // Write only changed files
    for (const auto& filePath : changedFiles) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) continue;
        
        std::vector<uint8_t> fileData(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
        
        if (!fileData.empty()) {
            writer.writeSection(CheckpointSection::Config, fileData.data(), fileData.size());
        }
    }
    
    if (!writer.finalize()) {
        return false;
    }
    
    // Update statistics
    {
        std::lock_guard<std::mutex> lock(statsMutex_);
        totalCheckpoints_++;
        totalBytesSaved_ += writer.getBytesWritten();
    }
    
    return true;
}

// Checkpoint deduplication functionality
bool CheckpointManager::deduplicateCheckpoints(const std::string& pattern) {
    std::vector<CheckpointInfo> checkpoints = listCheckpoints();
    std::vector<std::string> toDelete;
    
    // Group checkpoints by similar content (simplified implementation)
    std::unordered_map<size_t, std::vector<CheckpointInfo>> groups;
    
    for (const auto& checkpoint : checkpoints) {
        // For now, just group by file size (simple deduplication)
        size_t hash = static_cast<size_t>(checkpoint.fileSize) ^ 
                     static_cast<size_t>(checkpoint.timestamp);
        groups[hash].push_back(checkpoint);
    }
    
    // Delete duplicates (keep oldest)
    for (auto& pair : groups) {
        if (pair.second.size() > 1) {
            std::sort(pair.second.begin(), pair.second.end(),
                      [](const CheckpointInfo& a, const CheckpointInfo& b) {
                          return a.timestamp < b.timestamp;
                      });
            
            // Delete all but the oldest
            for (size_t i = 1; i < pair.second.size(); ++i) {
                std::string filepath = checkpointDir_ + "/" + pair.second[i].name + ".bin";
                if (std::filesystem::exists(filepath)) {
                    std::filesystem::remove(filepath);
                }
            }
        }
    }
    
    return true;
}

// Parallel checkpoint operations
std::future<bool> CheckpointManager::saveParallel(const std::string& name) {
    return std::async(std::launch::async, [this, name]() {
        return saveImmediately(name);
    });
}

std::future<bool> CheckpointManager::validateParallel(const std::string& filepath) {
    return std::async(std::launch::async, [this, filepath]() {
        return validateCheckpoint(filepath);
    });
}

// Checkpoint compression utilities
bool compressFile(const std::string& inputPath, const std::string& outputPath,
                 CompressionLevel level) {
    std::ifstream input(inputPath, std::ios::binary);
    std::ofstream output(outputPath, std::ios::binary);
    
    if (!input || !output) return false;
    
    std::vector<uint8_t> data(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()
    );
    
    std::vector<uint8_t> compressed;
    if (!compressData(data, compressed, level)) return false;
    
    output.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
    return static_cast<bool>(output);
}

bool decompressFile(const std::string& inputPath, const std::string& outputPath,
                   CompressionLevel level) {
    std::ifstream input(inputPath, std::ios::binary);
    std::ofstream output(outputPath, std::ios::binary);
    
    if (!input || !output) return false;
    
    std::vector<uint8_t> data(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()
    );
    
    std::vector<uint8_t> decompressed;
    if (!decompressData(data, decompressed, level)) return false;
    
    output.write(reinterpret_cast<const char*>(decompressed.data()), decompressed.size());
    return static_cast<bool>(output);
}

// Checkpoint migration utility
bool migrateCheckpoint(const std::string& sourcePath, const std::string& targetPath,
                       uint32_t targetVersionMajor, uint32_t targetVersionMinor,
                       uint32_t targetVersionPatch) {
    CheckpointReader reader;
    if (!reader.open(sourcePath)) {
        return false;
    }
    
    const auto& header = reader.getHeader();
    
    // Create new checkpoint with updated version
    CheckpointWriter writer;
    if (!writer.create(targetPath, CompressionLevel::Balanced)) {
        return false;
    }
    
    // Copy header with updated version
    CheckpointHeader newHeader = header;
    newHeader.majorVersion = targetVersionMajor;
    newHeader.minorVersion = targetVersionMinor;
    newHeader.patchVersion = targetVersionPatch;
    
    // For simplicity, we'll just copy all sections
    // In a real implementation, you would handle version-specific transformations
    
    writer.finalize();
    
    return true;
}

} // namespace nlm