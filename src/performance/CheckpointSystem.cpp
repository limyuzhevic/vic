#include "CheckpointSystem.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <system_error>

// Forward declarations for error handling
namespace nlm {
namespace {

// Global error reporting function for checkpoint operations
void reportCheckpointError(const std::string& operation, const std::string& filepath, 
                          const std::string& error) {
    std::cerr << "[CHECKPOINT ERROR] " << operation << " failed for " << filepath 
              << ": " << error << std::endl;
}

// Helper function to create backup of existing checkpoint
bool createCheckpointBackup(const std::string& filepath) {
    try {
        if (!std::filesystem::exists(filepath)) {
            return true;  // Nothing to backup
        }
        
        std::string backupPath = filepath + ".backup";
        if (std::filesystem::exists(backupPath)) {
            // Remove old backup
            std::filesystem::remove(backupPath);
        }
        
        std::filesystem::copy_file(filepath, backupPath, 
                                  std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (const std::exception& e) {
        reportCheckpointError("Backup creation", filepath, std::string("Backup failed: ") + e.what());
        return false;
    }
}

// Helper function to validate checkpoint integrity
bool validateCheckpointIntegrity(const std::string& filepath) {
    CheckpointReader reader;
    if (!reader.open(filepath)) {
        return false;
    }
    
    if (!reader.validate()) {
        reportCheckpointError("Integrity validation", filepath, "Checkpoint validation failed");
        return false;
    }
    
    return true;
}

// Enhanced checkpoint metadata with additional validation fields
struct EnhancedCheckpointHeader {
    uint32_t magic;
    uint32_t version;
    uint64_t creationTime;
    uint64_t neuronCount;
    uint64_t synapseCount;
    uint64_t regionCount;
    uint64_t step;
    double simulationTime;
    uint64_t totalSize;
    uint32_t dataChecksum;
    uint32_t headerChecksum;
    uint32_t integritySeed;
    uint8_t compressionLevel;
    uint8_t flags;  // Additional flags for future use
    char reserved[12];  // Padding for future expansion
    
    EnhancedCheckpointHeader() : magic(0xC0D3BEEF),
                                  version(2),
                                  creationTime(0),
                                  neuronCount(0),
                                  synapseCount(0),
                                  regionCount(0),
                                  step(0),
                                  simulationTime(0.0),
                                  totalSize(0),
                                  dataChecksum(0),
                                  headerChecksum(0),
                                  integritySeed(0xDEADBEEF),
                                  compressionLevel(0),
                                  flags(0) {
        std::memset(reserved, 0, sizeof(reserved));
    }
    
    bool validate() const {
        if (magic != 0xC0D3BEEF) return false;
        if (version < 1 || version > 2) return false;
        if (integritySeed != 0xDEADBEEF) return false;
        if (totalSize == 0) return false;
        return true;
    }
    
    void computeChecksums() {
        headerChecksum = calculateEnhancedChecksum(
            reinterpret_cast<const uint8_t*>(this),
            sizeof(EnhancedCheckpointHeader) - sizeof(uint32_t) -  // magic
                                                    sizeof(uint32_t) -  // version
                                                    sizeof(uint32_t) -  // dataChecksum
                                                    sizeof(uint32_t) -  // headerChecksum
                                                    sizeof(uint32_t));  // integritySeed
    }
};

// Enhanced section header with additional metadata
struct EnhancedSectionHeader {
    uint32_t type;
    uint64_t offset;
    uint64_t size;
    uint32_t compressedSize;
    uint32_t checksum;
    uint32_t sequenceId;  // For detecting data corruption during writes
    char reserved[8];  // Padding for future use
    
    EnhancedSectionHeader() : type(0),
                               offset(0),
                               size(0),
                               compressedSize(0),
                               checksum(0),
                               sequenceId(0) {
        std::memset(reserved, 0, sizeof(reserved));
    }
};

// Enhanced checksum calculation with error detection
uint32_t calculateEnhancedChecksum(const uint8_t* data, size_t size) {
    if (size == 0) return 0;
    
    uint32_t checksum = 0xDEADBEEF;
    
    for (size_t i = 0; i < size; ++i) {
        checksum = (checksum + static_cast<uint32_t>(data[i])) * 31;
        checksum = checksum ^ (checksum >> 13);
        checksum = checksum * 17 + 0x9E3779B9;
    }
    
    return checksum;
}

// Atomic write implementation
bool atomicWriteToFile(const std::string& filepath, const std::vector<uint8_t>& data) {
    try {
        createCheckpointBackup(filepath);
        
        std::string tempPath = filepath + ".tmp";
        std::ofstream outFile(tempPath, std::ios::binary | std::ios::trunc);
        
        if (!outFile) {
            reportCheckpointError("Atomic write", filepath, "Failed to create temporary file");
            return false;
        }
        
        outFile.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        
        if (!outFile) {
            reportCheckpointError("Atomic write", filepath, "Failed to write checkpoint data");
            std::filesystem::remove(tempPath);
            return false;
        }
        
        outFile.flush();
        
        std::ifstream verifyFile(tempPath, std::ios::binary);
        if (!verifyFile) {
            reportCheckpointError("Atomic write", filepath, "Failed to verify temporary file");
            std::filesystem::remove(tempPath);
            return false;
        }
        
        size_t verifySize = std::filesystem::file_size(tempPath);
        if (verifySize != data.size()) {
            reportCheckpointError("Atomic write", filepath, 
                                 "Size mismatch: expected " + std::to_string(data.size()) + 
                                 ", got " + std::to_string(verifySize));
            std::filesystem::remove(tempPath);
            return false;
        }
        
        std::error_code ec;
        std::filesystem::rename(tempPath, filepath, ec);
        
        if (ec) {
            reportCheckpointError("Atomic write", filepath, 
                                 "Failed to rename temporary file: " + ec.message());
            std::filesystem::remove(tempPath);
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        reportCheckpointError("Atomic write", filepath, std::string("Exception: ") + e.what());
        std::error_code ec;
        std::filesystem::remove(filepath + ".tmp", ec);
        return false;
    }
}

// Enhanced error recovery function
bool attemptCheckpointRecovery(const std::string& filepath) {
    std::string backupPath = filepath + ".backup";
    
    if (!std::filesystem::exists(backupPath)) {
        return false;
    }
    
    try {
        if (!validateCheckpointIntegrity(backupPath)) {
            reportCheckpointError("Recovery attempt", filepath, "Backup validation failed");
            return false;
        }
        
        std::string recoveryPath = filepath + ".recovered";
        
        if (!std::filesystem::exists(recoveryPath)) {
            std::filesystem::copy_file(backupPath, recoveryPath, 
                                      std::filesystem::copy_options::overwrite_existing);
        }
        
        if (validateCheckpointIntegrity(recoveryPath)) {
            std::filesystem::remove(filepath);
            std::filesystem::rename(recoveryPath, filepath);
            reportCheckpointError("Recovery", filepath, "Successfully recovered from backup");
            return true;
        }
        
        std::filesystem::remove(recoveryPath);
        
    } catch (const std::exception& e) {
        reportCheckpointError("Recovery attempt", filepath, std::string("Recovery failed: ") + e.what());
    }
    
    return false;
}

// Template helper functions for enhanced checkpoint operations
template <typename T>
void appendVec(std::vector<uint8_t>& out, const std::vector<T>& v) {
    uint64_t count = static_cast<uint64_t>(v.size());
    const uint8_t* countBytes = reinterpret_cast<const uint8_t*>(&count);
    out.insert(out.end(), countBytes, countBytes + sizeof(count));
    if (!v.empty()) {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(v.data());
        out.insert(out.end(), data, data + v.size() * sizeof(T));
    }
}

template <typename T>
bool readVec(const uint8_t*& p, const uint8_t* end, std::vector<T>& v) {
    if (static_cast<size_t>(end - p) < sizeof(uint64_t)) return false;
    uint64_t count = 0;
    std::memcpy(&count, p, sizeof(count));
    p += sizeof(uint64_t);
    size_t bytes = static_cast<size_t>(count) * sizeof(T);
    if (static_cast<size_t>(end - p) < bytes) return false;
    v.resize(static_cast<size_t>(count));
    if (bytes > 0) std::memcpy(v.data(), p, bytes);
    p += bytes;
    return true;
}

// Checkpoint parsing functions
std::vector<uint8_t> serializeNeurons(const NeuronCheckpointData& d) {
    std::vector<uint8_t> out;
    appendVec(out, d.membranePotential);
    appendVec(out, d.restingPotential);
    appendVec(out, d.threshold);
    appendVec(out, d.resetPotential);
    appendVec(out, d.leakConductance);
    appendVec(out, d.firingState);
    appendVec(out, d.refractoryRemaining);
    appendVec(out, d.refractoryPeriod);
    appendVec(out, d.lastSpikeTime);
    appendVec(out, d.firingRate);
    appendVec(out, d.neuronType);
    appendVec(out, d.regionId);
    appendVec(out, d.populationId);
    return out;
}

bool parseNeurons(const std::vector<uint8_t>& payload, NeuronCheckpointData& d) {
    const uint8_t* p = payload.data();
    const uint8_t* end = p + payload.size();
    return readVec(p, end, d.membranePotential) &&
           readVec(p, end, d.restingPotential) &&
           readVec(p, end, d.threshold) &&
           readVec(p, end, d.resetPotential) &&
           readVec(p, end, d.leakConductance) &&
           readVec(p, end, d.firingState) &&
           readVec(p, end, d.refractoryRemaining) &&
           readVec(p, end, d.refractoryPeriod) &&
           readVec(p, end, d.lastSpikeTime) &&
           readVec(p, end, d.firingRate) &&
           readVec(p, end, d.neuronType) &&
           readVec(p, end, d.regionId) &&
           readVec(p, end, d.populationId) &&
           p == end;
}

std::vector<uint8_t> serializeSynapses(const SynapseCheckpointData& d) {
    std::vector<uint8_t> out;
    appendVec(out, d.sourceNeuron);
    appendVec(out, d.destinationNeuron);
    appendVec(out, d.weight);
    appendVec(out, d.delay);
    appendVec(out, d.synapseType);
    appendVec(out, d.plasticityFlags);
    appendVec(out, d.eligibilityTrace);
    appendVec(out, d.efficacy);
    appendVec(out, d.shortTermDepression);
    appendVec(out, d.shortTermFacilitation);
    return out;
}

bool parseSynapses(const std::vector<uint8_t>& payload, SynapseCheckpointData& d) {
    const uint8_t* p = payload.data();
    const uint8_t* end = p + payload.size();
    return readVec(p, end, d.sourceNeuron) &&
           readVec(p, end, d.destinationNeuron) &&
           readVec(p, end, d.weight) &&
           readVec(p, end, d.delay) &&
           readVec(p, end, d.synapseType) &&
           readVec(p, end, d.plasticityFlags) &&
           readVec(p, end, d.eligibilityTrace) &&
           readVec(p, end, d.efficacy) &&
           readVec(p, end, d.shortTermDepression) &&
           readVec(p, end, d.shortTermFacilitation) &&
           p == end;
}

std::vector<uint8_t> serializeDelayedSpikes(const std::vector<DelayedSpikeEvent>& events) {
    std::vector<uint8_t> out;
    appendVec(out, events);
    return out;
}

bool parseDelayedSpikes(const std::vector<uint8_t>& payload,
                        std::vector<DelayedSpikeEvent>& events) {
    const uint8_t* p = payload.data();
    const uint8_t* end = p + payload.size();
    return readVec(p, end, events) && p == end;
}

// Enhanced section writing with integrity checks
bool writeSectionToStream(std::ofstream& stream, CheckpointSection type,
                   const std::vector<uint8_t>& payload, uint64_t& bytesWritten) {
    SectionHeader header;
    header.type = static_cast<uint32_t>(type);
    header.offset = static_cast<uint64_t>(stream.tellp()) + sizeof(SectionHeader);
    header.size = static_cast<uint64_t>(payload.size());
    header.compressedSize = 0;
    header.checksum = payload.empty() ? 0 : calculateEnhancedChecksum(payload.data(), payload.size());
    
    stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
    if (!payload.empty()) {
        stream.write(reinterpret_cast<const char*>(payload.data()), static_cast<std::streamsize>(payload.size()));
    }
    if (!stream) return false;
    
    bytesWritten += sizeof(header) + payload.size();
    return true;
}

}} // namespace nlm

// ---------------- Enhanced CheckpointReader ----------------

CheckpointReader::CheckpointReader() : streamPos_(0) {}

CheckpointReader::~CheckpointReader() {
    close();
}

bool CheckpointReader::open(const std::string& filepath) {
    close();
    
    // Attempt to recover from corrupted checkpoint if available
    std::error_code ec;
    if (std::filesystem::exists(filepath, ec)) {
        if (!validateCheckpointIntegrity(filepath)) {
            attemptCheckpointRecovery(filepath);
        }
    }
    
    stream_.open(filepath, std::ios::binary);
    if (!stream_) {
        error_ = "cannot open checkpoint file: " + filepath;
        return false;
    }
    
    try {
        stream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
        if (!stream_) {
            error_ = "failed to read checkpoint header";
            close();
            return false;
        }
        
        if (!header_.validate()) {
            error_ = "invalid checkpoint header (magic/version mismatch or corrupted checksum)";
            close();
            return false;
        }
        
        sections_.clear();
        while (true) {
            SectionHeader section;
            stream_.read(reinterpret_cast<char*>(&section), sizeof(section));
            if (!stream_) break;
            
            sections_.push_back(section);
            if (section.type == static_cast<uint32_t>(CheckpointSection::End)) break;
            
            stream_.seekg(static_cast<std::streamoff>(section.offset + section.size), std::ios::beg);
            if (!stream_) break;
        }
        
        stream_.clear();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("exception reading checkpoint: ") + e.what();
        close();
        return false;
    }
}

void CheckpointReader::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    stream_.clear();
    sections_.clear();
    streamPos_ = 0;
}

bool CheckpointReader::validate() const {
    if (!header_.validate()) return false;
    bool hasNeurons = false, hasSynapses = false;
    
    for (const auto& s : sections_) {
        if (s.type == static_cast<uint32_t>(CheckpointSection::Neurons)) hasNeurons = true;
        if (s.type == static_cast<uint32_t>(CheckpointSection::Synapses)) hasSynapses = true;
    }
    
    return hasNeurons && hasSynapses;
}

std::vector<uint8_t> CheckpointReader::readSection(CheckpointSection sectionType) {
    for (const auto& s : sections_) {
        if (s.type != static_cast<uint32_t>(sectionType)) continue;
        
        stream_.clear();
        stream_.seekg(static_cast<std::streamoff>(s.offset), std::ios::beg);
        if (!stream_) return {};
        
        std::vector<uint8_t> payload(static_cast<size_t>(s.size));
        if (s.size > 0) {
            stream_.read(reinterpret_cast<char*>(payload.data()), static_cast<std::streamsize>(s.size));
            if (!stream_) return {};
        }
        
        streamPos_ = static_cast<size_t>(s.offset + s.size);
        
        uint32_t actualChecksum = payload.empty() ? 0 : calculateEnhancedChecksum(payload.data(), payload.size());
        if (actualChecksum != s.checksum) {
            error_ = "section checksum mismatch";
            return {};
        }
        
        return payload;
    }
    
    return {};
}

bool CheckpointReader::readNeurons(NeuronCheckpointData& data) {
    auto payload = readSection(CheckpointSection::Neurons);
    if (payload.empty() && header_.neuronCount > 0) return false;
    
    if (!parseNeurons(payload, data)) {
        error_ = "failed to parse neuron section";
        return false;
    }
    
    if (data.membranePotential.size() != header_.neuronCount) {
        error_ = "neuron count in payload does not match header";
        return false;
    }
    
    return true;
}

bool CheckpointReader::readDelayedSpikes(std::vector<DelayedSpikeEvent>& events) {
    events.clear();
    auto payload = readSection(CheckpointSection::SpikeHistory);
    if (payload.empty()) return true;
    
    if (!parseDelayedSpikes(payload, events)) {
        error_ = "failed to parse delayed spike section";
        return false;
    }
    
    return true;
}

bool CheckpointReader::readSynapses(SynapseCheckpointData& data) {
    auto payload = readSection(CheckpointSection::Synapses);
    if (payload.empty() && header_.synapseCount > 0) return false;
    
    if (!parseSynapses(payload, data)) {
        error_ = "failed to parse synapse section";
        return false;
    }
    
    if (data.weight.size() != header_.synapseCount) {
        error_ = "synapse count in payload does not match header";
        return false;
    }
    
    return true;
}

double CheckpointReader::getProgress() const {
    if (!stream_.is_open() || header_.totalSize == 0) return 0.0;
    double p = static_cast<double>(streamPos_) / static_cast<double>(header_.totalSize);
    return std::clamp(p, 0.0, 1.0);
}

// ---------------- Enhanced CheckpointWriter ----------------

CheckpointWriter::CheckpointWriter()
    : compression_(CompressionLevel::Balanced), bytesWritten_(0) {
    header_ = CheckpointHeader();
}

CheckpointWriter::~CheckpointWriter() {
    close();
}

bool CheckpointWriter::create(const std::string& filepath, CompressionLevel compression) {
    close();
    
    filepath_ = filepath;
    compression_ = compression;
    header_ = CheckpointHeader();
    header_.compressionLevel = static_cast<uint32_t>(compression);
    sections_.clear();
    neuronPayload_.clear();
    synapsePayload_.clear();
    delayedSpikePayload_.clear();
    bytesWritten_ = 0;
    
    createCheckpointBackup(filepath_);
    
    stream_.open(filepath, std::ios::binary | std::ios::trunc);
    if (!stream_) {
        error_ = "cannot create checkpoint file: " + filepath;
        return false;
    }
    
    return true;
}

void CheckpointWriter::close() {
    if (stream_.is_open()) {
        stream_.close();
    }
    stream_.clear();
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
    if (data.membranePotential.size() != header_.neuronCount && header_.neuronCount != 0) {
        return false;
    }
    neuronPayload_ = serializeNeurons(data);
    return true;
}

bool CheckpointWriter::writeSynapses(const SynapseCheckpointData& data) {
    if (data.weight.size() != header_.synapseCount && header_.synapseCount != 0) {
        return false;
    }
    synapsePayload_ = serializeSynapses(data);
    return true;
}

bool CheckpointWriter::writeDelayedSpikes(const std::vector<DelayedSpikeEvent>& events) {
    delayedSpikePayload_ = serializeDelayedSpikes(events);
    return true;
}

bool CheckpointWriter::bufferSection(CheckpointSection sectionType,
                                     const std::vector<uint8_t>& payload) {
    for (auto& entry : extraPayloads_) {
        if (entry.first == sectionType) {
            entry.second = payload;
            return true;
        }
    }
    extraPayloads_.emplace_back(sectionType, payload);
    return true;
}

bool CheckpointWriter::writeSection(CheckpointSection sectionType, const void* data, size_t size) {
    (void)sectionType; (void)data; (void)size;
    return false;
}

bool CheckpointWriter::finalize() {
    if (!stream_.is_open()) {
        error_ = "checkpoint writer not open";
        return false;
    }
    
    try {
        stream_.seekp(0, std::ios::beg);
        CheckpointHeader placeholder;
        stream_.write(reinterpret_cast<const char*>(&placeholder), sizeof(placeholder));
        if (!stream_) {
            error_ = "failed to write placeholder header";
            return false;
        }
        
        bytesWritten_ = sizeof(placeholder);
        
        if (!writeSectionToStream(stream_, CheckpointSection::Neurons, neuronPayload_, bytesWritten_)) {
            error_ = "failed to write neurons section";
            return false;
        }
        
        if (!writeSectionToStream(stream_, CheckpointSection::Synapses, synapsePayload_, bytesWritten_)) {
            error_ = "failed to write synapses section";
            return false;
        }
        
        if (!writeSectionToStream(stream_, CheckpointSection::SpikeHistory, delayedSpikePayload_, bytesWritten_)) {
            error_ = "failed to write spike history section";
            return false;
        }
        
        for (CheckpointSection type : {CheckpointSection::SimulationState,
                                       CheckpointSection::RandomState,
                                       CheckpointSection::Plasticity,
                                       CheckpointSection::Memory}) {
            for (const auto& entry : extraPayloads_) {
                if (entry.first == type && !entry.second.empty()) {
                    if (!writeSectionToStream(stream_, type, entry.second, bytesWritten_)) {
                        error_ = "failed to write extra section " + std::to_string(static_cast<int>(type));
                        return false;
                    }
                }
            }
        }
        
        SectionHeader endHeader;
        endHeader.type = static_cast<uint32_t>(CheckpointSection::End);
        endHeader.offset = static_cast<uint64_t>(stream_.tellp()) + sizeof(SectionHeader);
        endHeader.size = 0;
        endHeader.compressedSize = 0;
        endHeader.checksum = 0;
        stream_.write(reinterpret_cast<const char*>(&endHeader), sizeof(endHeader));
        if (!stream_) {
            error_ = "failed to write end section header";
            return false;
        }
        
        bytesWritten_ += sizeof(endHeader);
        
        header_.totalSize = bytesWritten_;
        header_.checksum = calculateEnhancedChecksum(neuronPayload_.data(), neuronPayload_.size());
        
        stream_.seekp(0, std::ios::beg);
        stream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
        if (!stream_) {
            error_ = "failed to write final header";
            return false;
        }
        
        stream_.flush();
        
        if (!validateCheckpointIntegrity(filepath_)) {
            error_ = "checkpoint integrity validation failed after write";
            return false;
        }
        
        close();
        return true;
        
    } catch (const std::exception& e) {
        error_ = std::string("exception during checkpoint finalization: ") + e.what();
        close();
        return false;
    }
}

void CheckpointWriter::abort() {
    close();
    if (!filepath_.empty()) {
        std::error_code ec;
        std::filesystem::remove(filepath_, ec);
    }
}

bool CheckpointWriter::writeHeader() { return false; }
bool CheckpointWriter::writeSectionHeader(const SectionHeader&) { return false; }
bool CheckpointWriter::padToAlignment(size_t) { return true; }

// ---------------- Enhanced CheckpointManager ----------------

CheckpointManager::CheckpointManager()
    : saveIntervalSteps_(10000), maxCheckpoints_(10), compress_(true), lastSaveStep_(0) {}

CheckpointManager::~CheckpointManager() = default;

void CheckpointManager::configure(const std::string& checkpointDir,
                                  uint64_t saveIntervalSteps, size_t maxCheckpoints, bool compress) {
    checkpointDir_ = checkpointDir;
    saveIntervalSteps_ = saveIntervalSteps;
    maxCheckpoints_ = maxCheckpoints;
    compress_ = compress;
    lastSaveStep_ = 0;
    std::error_code ec;
    std::filesystem::create_directories(checkpointDir_, ec);
}

bool CheckpointManager::shouldSave(uint64_t currentStep) const {
    return saveIntervalSteps_ > 0 && currentStep > 0 &&
           (currentStep % saveIntervalSteps_) == 0 && currentStep != lastSaveStep_;
}

bool CheckpointManager::saveImmediately(const std::string& name) {
    if (!neuronProvider_ || !synapseProvider_) return false;
    
    NeuronCheckpointData neurons;
    SynapseCheckpointData synapses;
    if (!neuronProvider_(neurons) || !synapseProvider_(synapses)) return false;
    
    std::string filename = name.empty() ? "checkpoint.bin" : name;
    std::string path = (std::filesystem::path(checkpointDir_) / filename).string();
    
    CheckpointWriter writer;
    if (!writer.create(path, compress_ ? CompressionLevel::Balanced : CompressionLevel::None)) {
        reportCheckpointError("Save immediately", path, writer.getError());
        return false;
    }
    
    writer.setMetadata(neurons.membranePotential.size(), synapses.weight.size(), 0, lastSaveStep_, 0.0);
    
    if (!writer.writeNeurons(neurons) || !writer.writeSynapses(synapses)) {
        reportCheckpointError("Save immediately", path, "Failed to write checkpoint data");
        writer.abort();
        return false;
    }
    
    if (!writer.finalize()) {
        reportCheckpointError("Save immediately", path, writer.getError());
        return false;
    }
    
    lastCheckpointPath_ = path;
    pruneOldCheckpoints(maxCheckpoints_);
    return true;
}

bool CheckpointManager::update(uint64_t currentStep, double currentTime) {
    (void)currentTime;
    if (!shouldSave(currentStep)) return false;
    if (!neuronProvider_ || !synapseProvider_) return false;
    
    lastSaveStep_ = currentStep;
    return saveImmediately("checkpoint-" + std::to_string(currentStep) + ".bin");
}

bool CheckpointManager::load(const std::string& name) {
    std::string path = (std::filesystem::path(checkpointDir_) / name).string();
    CheckpointReader reader;
    if (!reader.open(path)) {
        reportCheckpointError("Load", path, reader.getError());
        return false;
    }
    
    return reader.validate();
}

std::vector<CheckpointManager::CheckpointInfo> CheckpointManager::listCheckpoints() const {
    std::vector<CheckpointInfo> infos;
    std::error_code ec;
    if (checkpointDir_.empty() || !std::filesystem::exists(checkpointDir_, ec)) return infos;
    
    for (const auto& entry : std::filesystem::directory_iterator(checkpointDir_, ec)) {
        if (entry.path().extension() != ".bin") continue;
        
        CheckpointReader reader;
        if (!reader.open(entry.path().string())) continue;
        
        CheckpointInfo info;
        info.name = entry.path().filename().string();
        info.step = reader.getHeader().step;
        info.simulationTime = reader.getHeader().simulationTime;
        info.neuronCount = reader.getHeader().neuronCount;
        info.synapseCount = reader.getHeader().synapseCount;
        info.fileSize = entry.file_size(ec);
        
        auto ftime = entry.last_write_time(ec);
        auto sysTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
        info.timestamp = std::chrono::system_clock::to_time_t(sysTime);
        
        infos.push_back(info);
    }
    
    std::sort(infos.begin(), infos.end(),
              [](const CheckpointInfo& a, const CheckpointInfo& b) { return a.step < b.step; });
    return infos;
}

void CheckpointManager::pruneOldCheckpoints(size_t keepCount) {
    auto infos = listCheckpoints();
    if (infos.size() <= keepCount) return;
    
    size_t toRemove = infos.size() - keepCount;
    for (size_t i = 0; i < toRemove; ++i) {
        std::error_code ec;
        std::filesystem::remove(std::filesystem::path(checkpointDir_) / infos[i].name, ec);
        if (ec) {
            reportCheckpointError("Prune", infos[i].name, 
                                 "Failed to remove checkpoint: " + ec.message());
        }
    }
}

std::string CheckpointManager::getLastCheckpointPath() const {
    return lastCheckpointPath_;
}

} // namespace nlm
