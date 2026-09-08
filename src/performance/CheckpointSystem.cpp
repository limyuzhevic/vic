#include "Brain.hpp"

namespace nlm {

// Checkpoint implementation with full section support

CheckpointWriter::CheckpointWriter() : bytesWritten_(0) {
    // Initialize
}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    filepath_ = filepath;
    compression_ = compression;
    
    // Open file for writing
    stream_.open(filepath, std::ios::binary | std::ios::trunc);
    if (!stream_.is_open()) {
        return false;
    }
    
    bytesWritten_ = 0;
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
}

bool CheckpointWriter::writeNeurons(const NeuronCheckpointData& data) {
    return writeSection(CheckpointSection::Neurons, &data, sizeof(data));
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    return writeSection(CheckpointSection::Synapses, &data, sizeof(data));
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    if (!stream_.is_open() || !data || size == 0) {
        return false;
    }
    
    // Compress if needed
    const void* sourceData = data;
    size_t sourceSize = size;
    
    if (compression_ != CompressionLevel::None) {
        // For now, just use uncompressed data
        sourceData = data;
        sourceSize = size;
    }
    
    // Write section header
    SectionHeader sectionHeader;
    sectionHeader.type = static_cast<uint32_t>(sectionType);
    sectionHeader.offset = bytesWritten_ + sizeof(CheckpointHeader);
    sectionHeader.size = sourceSize;
    sectionHeader.compressedSize = (compression_ != CompressionLevel::None) ? sourceSize : 0;
    sectionHeader.checksum = ChecksumCalculator::crc32(sourceData, sourceSize);
    
    if (!writeSectionHeader(sectionHeader)) {
        return false;
    }
    
    // Write section data
    stream_.write(static_cast<const char*>(sourceData), sourceSize);
    if (!stream_.good()) {
        return false;
    }
    
    bytesWritten_ += sizeof(SectionHeader) + sourceSize;
    sections_.push_back(sectionHeader);
    
    return true;
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        return false;
    }
    
    // Calculate and write checksum
    header_.checksum = 0; // Placeholder
    
    // Write header to beginning of file
    stream_.seekp(0);
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    
    return stream_.good();
}

void CheckpointWriter::abort() {
    if (stream_.is_open()) {
        stream_.close();
        // Remove partial file
        stream_.open(filepath_, std::ios::binary | std::ios::trunc);
        stream_.close();
    }
}

bool CheckpointWriter::writeHeader() {
    if (!stream_.is_open()) {
        return false;
    }
    
    std::time_t timestamp = std::time(nullptr);
    header_.timestamp = static_cast<uint64_t>(timestamp);
    
    // Write header
    stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    bytesWritten_ += sizeof(header_);
    
    return stream_.good();
}

bool CheckpointWriter::writeSectionHeader(const SectionHeader& header) {
    if (!stream_.is_open()) {
        return false;
    }
    
    stream_.write(reinterpret_cast<const char*>(&header), sizeof(header));
    bytesWritten_ += sizeof(header);
    
    return stream_.good();
}

bool CheckpointWriter::padToAlignment(size_t alignment) {
    size_t currentPos = stream_.tellp();
    size_t padBytes = (alignment - (currentPos % alignment)) % alignment;
    
    if (padBytes > 0) {
        std::vector<char> padding(padBytes, 0);
        stream_.write(padding.data(), padBytes);
    }
    
    return stream_.good();
}

// CheckpointReader implementation

CheckpointReader::CheckpointReader() : streamPos_(0) {
}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    stream_.open(filepath, std::ios::binary);
    if (!stream_.is_open()) {
        return false;
    }
    
    return readHeader();
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    sections_.clear();
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    std::vector<uint8_t> buffer = readSection(CheckpointSection::Neurons);
    if (buffer.empty()) return false;
    
    if (buffer.size() >= sizeof(data)) {
        std::memcpy(&data, buffer.data(), sizeof(data));
        return true;
    }
    
    return false;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    std::vector<uint8_t> buffer = readSection(CheckpointSection::Synapses);
    if (buffer.empty()) return false;
    
    if (buffer.size() >= sizeof(data)) {
        std::memcpy(&data, buffer.data(), sizeof(data));
        return true;
    }
    
    return false;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    std::vector<uint8_t> result;
    
    for (const auto& section : sections_) {
        if (static_cast<CheckpointSection>(section.type) == sectionType) {
            stream_.seekg(section.offset);
            result.resize(section.size);
            stream_.read(reinterpret_cast<char*>(result.data()), section.size);
            break;
        }
    }
    
    return result;
}

bool CheckpointReader::validate() const {
    return header_.validate();
}

double CheckpointReader::getProgress() const {
    // Placeholder implementation
    return 0.0;
}

bool CheckpointReader::readHeader() {
    if (!stream_.is_open()) {
        return false;
    }
    
    stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (!stream_.good() || header_.magic != CHECKPOINT_MAGIC) {
        error_ = "Invalid checkpoint magic number";
        return false;
    }
    
    if (!header_.validate()) {
        error_ = "Checkpoint header validation failed";
        return false;
    }
    
    // Read section headers
    size_t currentPos = sizeof(header_);
    while (currentPos < header_.totalSize) {
        SectionHeader sectionHeader;
        stream_.seekg(currentPos);
        stream_.read(reinterpret_cast<char*>(&sectionHeader), sizeof(sectionHeader));
        
        if (!stream_.good()) break;
        
        sections_.push_back(sectionHeader);
        currentPos += sizeof(SectionHeader) + sectionHeader.size;
    }
    
    return true;
}

// Checksum implementation

inline uint64_t ChecksumCalculator::crc64(const void* data, size_t size) {
    // Simple CRC-64 implementation
    static const uint64_t table[256] = { /* precomputed */ };
    uint64_t crc = 0xFFFFFFFFFFFFFFFF;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        uint8_t idx = (crc ^ bytes[i]) & 0xFF;
        crc = table[idx] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFFFFFFFFFF;
}

inline uint32_t ChecksumCalculator::crc32(const void* data, size_t size) {
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    
    return ~crc;
}

inline uint64_t ChecksumCalculator::fletcher64(const uint64_t* data, size_t count) {
    uint64_t sum1 = 0, sum2 = 0;
    for (size_t i = 0; i < count; ++i) {
        sum1 = (sum1 + data[i]) % 0xFFFFFFFFFFFFFFFF;
        sum2 = (sum2 + sum1) % 0xFFFFFFFFFFFFFFFF;
    }
    return (sum2 << 32) | sum1;
}

inline uint64_t ChecksumCalculator::combine(uint64_t a, uint64_t b) {
    // XOR combine for incremental checksums
    return a ^ (b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2));
}

// CheckpointManager implementation

CheckpointManager::CheckpointManager() : lastSaveStep_(0) {
}

CheckpointManager::~CheckpointManager() {
}

void CheckpointManager::configure(const std::string& checkpointDir, 
                                   uint64_t saveIntervalSteps,
                                   size_t maxCheckpoints,
                                   bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    
    std::filesystem::create_directories(checkpointDir_);
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    if (shouldSave(currentStep)) {
        return saveImmediately();
    }
    return false;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    std::string filename = name.empty() ? "checkpoint" : name;
    
    if (neuronProvider_) {
        NeuronCheckpointData neuronData;
        if (neuronProvider_(neuronData)) {
            // TODO: Write neurons using CheckpointWriter
        }
    }
    
    if (synapseProvider_) {
        SynapseCheckpointData synapseData;
        if (synapseProvider_(synapseData)) {
            // TODO: Write synapses using CheckpointWriter
        }
    }
    
    lastSaveStep_ = getCurrentStep();
    lastCheckpointPath_ = "";
    
    return true;
}

bool CheckpointManager::load(const std::string& name) {
    std::string filepath = checkpointDir_ + "/" + name + ".ckpt";
    
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        return false;
    }
    
    if (neuronProvider_) {
        NeuronCheckpointData neuronData;
        if (reader.readNeurons(neuronData)) {
            if (!neuronProvider_(neuronData)) {
                return false;
            }
        }
    }
    
    if (synapseProvider_) {
        SynapseCheckpointData synapseData;
        if (reader.readSynapses(synapseData)) {
            if (!synapseProvider_(synapseData)) {
                return false;
            }
        }
    }
    
    lastCheckpointPath_ = filepath;
    
    return true;
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> checkpoints;
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".ckpt") {
            std::string filename = entry.path().stem().string();
            
            // Parse timestamp from filename (assuming format: checkpoint_XXXXXX.ckpt)
            // For now, use file modification time
            auto ftime = std::filesystem::last_write_time(entry);
            auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
            );
            time_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                sctp.time_since_epoch()
            ).count();
            
            CheckpointInfo info;
            info.name = filename;
            info.step = 0;  // TODO: Extract from filename
            info.simulationTime = 0.0;  // TODO: Extract from filename
            info.neuronCount = 0;  // TODO: Read from header
            info.synapseCount = 0;  // TODO: Read from header
            info.fileSize = entry.file_size();
            info.timestamp = timestamp;
            
            checkpoints.push_back(info);
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(checkpoints.begin(), checkpoints.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) {
                  return a.timestamp > b.timestamp;
              });
    
    return checkpoints;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    std::vector<CheckpointInfo> checkpoints = listCheckpoints();
    
    if (checkpoints.size() <= keepCount) {
        return;
    }
    
    size_t toDelete = checkpoints.size() - keepCount;
    for (size_t i = 0; i < toDelete; ++i) {
        std::filesystem::remove(checkpointDir_ + "/" + checkpoints[i].name + ".ckpt");
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return (currentStep - lastSaveStep_) >= saveIntervalSteps_;
}

} // namespace nlm
