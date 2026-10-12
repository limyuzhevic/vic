#include "BinarySerializer.hpp"
#include "SerializationSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <cstring>
#include <algorithm>
#include <zlib.h>
#include <random>

namespace nlm {

bool BinarySerializer::create(const std::string& filepath) {
    return executeSafely<bool>([&]() {
        // Check if file exists and remove if needed
        std::remove(filepath.c_str());
        
        outputStream_.open(filepath, std::ios::binary);
        if (!outputStream_.is_open()) {
            NLM_LOG_ERROR("BinarySerializer: Failed to create file: " + filepath);
            return false;
        }
        
        // Write header
        if (!writeHeader()) {
            NLM_LOG_ERROR("BinarySerializer: Failed to write header");
            return false;
        }
        
        // Write initial section headers
        sections_.clear();
        return true;
    });
}

bool BinarySerializer::write(const std::string& componentName, const std::vector<uint8_t>& data) {
    return executeSafely<bool>([&]() {
        if (!outputStream_.is_open()) {
            NLM_LOG_ERROR("BinarySerializer: File not open for writing");
            return false;
        }
        
        // Compress if requested
        std::vector<uint8_t> compressedData = data;
        if (compressionLevel_ != CompressionLevel::None) {
            if (!compress(compressedData)) {
                NLM_LOG_ERROR("BinarySerializer: Failed to compress data");
                return false;
            }
        }
        
        // Write section header
        BinarySection section;
        section.type = getSectionType(componentName);
        section.offset = static_cast<uint64_t>(outputStream_.tellp());
        section.size = compressedData.size();
        section.compressedSize = compressionLevel_ != CompressionLevel::None ? section.size : 0;
        section.checksum = calculateChecksum(compressedData.data(), compressedData.size());
        
        // Write section data
        outputStream_.write(reinterpret_cast<const char*>(compressedData.data()), compressedData.size());
        if (!outputStream_.good()) {
            NLM_LOG_ERROR("BinarySerializer: Failed to write section data");
            return false;
        }
        
        // Pad to alignment
        padToAlignment(8);
        
        // Add section to list
        sections_.push_back(section);
        bytesWritten_ += compressedData.size() + sizeof(BinarySection);
        
        return true;
    });
}

bool BinarySerializer::finalize() {
    return executeSafely<bool>([&]() {
        if (!outputStream_.is_open()) {
            NLM_LOG_ERROR("BinarySerializer: File not open for writing");
            return false;
        }
        
        // Write section headers
        uint64_t headerOffset = outputStream_.tellp();
        for (size_t i = 0; i < sections_.size(); ++i) {
            if (!writeSectionHeader(sections_[i])) {
                NLM_LOG_ERROR("BinarySerializer: Failed to write section header " + std::to_string(i));
                return false;
            }
        }
        
        // Write footer
        outputStream_.write(reinterpret_cast<const char*>(&sections_.size()), sizeof(sections_.size()));
        
        outputStream_.close();
        return true;
    });
}

bool BinarySerializer::open(const std::string& filepath) {
    return executeSafely<bool>([&]() {
        inputStream_.open(filepath, std::ios::binary);
        if (!inputStream_.is_open()) {
            NLM_LOG_ERROR("BinarySerializer: Failed to open file: " + filepath);
            return false;
        }
        
        // Read header
        if (!readHeader()) {
            NLM_LOG_ERROR("BinarySerializer: Failed to read header");
            return false;
        }
        
        // Read section headers
        uint64_t sectionCount;
        inputStream_.read(reinterpret_cast<char*>(&sectionCount), sizeof(sectionCount));
        
        sections_.resize(sectionCount);
        for (size_t i = 0; i < sectionCount; ++i) {
            if (!readSectionHeader(sections_[i])) {
                NLM_LOG_ERROR("BinarySerializer: Failed to read section header " + std::to_string(i));
                return false;
            }
        }
        
        return true;
    });
}

std::vector<uint8_t> BinarySerializer::read(const std::string& componentName) {
    return executeSafely<std::vector<uint8_t>>([&]() {
        uint32_t sectionType = getSectionType(componentName);
        
        for (const auto& section : sections_) {
            if (section.type == sectionType) {
                // Seek to section data
                inputStream_.seekg(), section.offset);
                
                // Read section data
                std::vector<uint8_t> data(section.size);
                inputStream_.read(reinterpret_cast<char*>(data.data()), data.size());
                
                // Decompress if needed
                if (section.compressedSize > 0) {
                    if (!decompress(data)) {
                        NLM_LOG_ERROR("BinarySerializer: Failed to decompress data");
                        return std::vector<uint8_t>();
                    }
                }
                
                // Verify checksum
                uint32_t checksum = calculateChecksum(data.data(), data.size());
                if (checksum != section.checksum) {
                    NLM_LOG_ERROR("BinarySerializer: Checksum mismatch for component " + componentName);
                    return std::vector<uint8_t>();
                }
                
                return data;
            }
        }
        
        NLM_LOG_ERROR("BinarySerializer: Component not found: " + componentName);
        return std::vector<uint8_t>();
    });
}

void BinarySerializer::close() {
    if (inputStream_.is_open()) {
        inputStream_.close();
    }
}

bool BinarySerializer::compress(std::vector<uint8_t>& data) {
    if (data.empty()) return true;
    
    switch (compressionLevel_) {
        case CompressionLevel::None:
            return true;
            
        case CompressionLevel::Fast:
            return compressFast(data);
            
        case CompressionLevel::Balanced:
            return compressZlib(data, Z_BEST_SPEED);
            
        case CompressionLevel::Best:
            return compressZlib(data, Z_BEST_COMPRESSION);
    }
    
    return false;
}

bool BinarySerializer::decompress(std::vector<uint8_t>& data) {
    if (data.empty()) return true;
    
    return decompressZlib(data);
}

uint32_t BinarySerializer::calculateChecksum(const void* data, size_t size) const {
    // Simple CRC32 checksum
    uint32_t crc = 0xFFFFFFFF;
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        crc ^= bytes[i];
        for (int j = 0; j < 8; ++j) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    
    return ~crc;
}

bool BinarySerializer::compressFast(std::vector<uint8_t>& data) {
    // Simple run-length encoding for fast compression
    std::vector<uint8_t> compressed;
    compressed.reserve(data.size() * 1.1);
    
    for (size_t i = 0; i < data.size();) {
        uint8_t current = data[i];
        size_t count = 1;
        
        // Count consecutive identical bytes
        while (i + count < data.size() && data[i + count] == current && count < 255) {
            ++count;
        }
        
        // Write run length and value
        compressed.push_back(static_cast<uint8_t>(count));
        compressed.push_back(current);
        
        i += count;
    }
    
    data = compressed;
    return true;
}

bool BinarySerializer::compressZlib(std::vector<uint8_t>& data, int level) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    
    if (deflateInit2(&stream, level, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        return false;
    }
    
    stream.next_in = reinterpret_cast<Bytef*>(data.data());
    stream.avail_in = data.size();
    
    const size_t CHUNK_SIZE = 8192;
    std::vector<uint8_t> compressed;
    compressed.reserve(data.size() * 0.5); // Estimate
    
    do {
        stream.next_out = reinterpret_cast<Bytef*>(compressed.data()) + compressed.size();
        stream.avail_out = CHUNK_SIZE;
        
        int ret = deflate(&stream, Z_FINISH);
        if (ret != Z_OK && ret != Z_STREAM_END) {
            deflateEnd(&stream);
            return false;
        }
        
        size_t have = CHUNK_SIZE - stream.avail_out;
        compressed.resize(compressed.size() + have);
    } while (stream.avail_out == 0);
    
    deflateEnd(&stream);
    data = compressed;
    return true;
}

bool BinarySerializer::decompressZlib(std::vector<uint8_t>& data) {
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    
    if (inflateInit2(&stream, -15) != Z_OK) {
        return false;
    }
    
    stream.next_in = reinterpret_cast<Bytef*>(data.data());
    stream.avail_in = data.size();
    
    const size_t CHUNK_SIZE = 8192;
    std::vector<uint8_t> decompressed;
    decompressed.reserve(data.size() * 2); // Estimate
    
    do {
        stream.next_out = reinterpret_cast<Bytef*>(decompressed.data()) + decompressed.size();
        stream.avail_out = CHUNK_SIZE;
        
        int ret = inflate(&stream, Z_FINISH);
        if (ret == Z_NEED_MORE) {
            decompressed.resize(decompressed.size() + CHUNK_SIZE);
        } else if (ret == Z_STREAM_END) {
            break;
        } else {
            inflateEnd(&stream);
            return false;
        }
    } while (stream.avail_out == 0);
    
    inflateEnd(&stream);
    decompressed.resize(decompressed.size() + CHUNK_SIZE - stream.avail_out);
    data = decompressed;
    return true;
}

uint32_t BinarySerializer::getSectionType(const std::string& componentName) const {
    // Simple hash function for component names
    uint32_t hash = 0;
    for (char c : componentName) {
        hash = (hash * 31) + static_cast<uint32_t>(c);
    }
    return hash;
}

bool BinarySerializer::writeHeader() {
    header_.magic = BINARY_MAGIC;
    header_.version = 1; // Version for this binary format
    header_.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    header_.checksum = 0; // Will be updated after writing
    header_.totalSize = 0; // Will be updated after writing
    header_.componentCount = 0; // Will be updated after writing sections
    header_.flags = static_cast<uint32_t>(flags_);
    
    outputStream_.write(reinterpret_cast<const char*>(&header_), sizeof(header_));
    return outputStream_.good();
}

bool BinarySerializer::readHeader() {
    inputStream_.read(reinterpret_cast<char*>(&header_), sizeof(header_));
    if (!inputStream_.good() || header_.magic != BINARY_MAGIC) {
        return false;
    }
    return true;
}

bool BinarySerializer::writeSectionHeader(const BinarySection& section) {
    outputStream_.write(reinterpret_cast<const char*>(&section), sizeof(section));
    return outputStream_.good();
}

bool BinarySerializer::readSectionHeader(BinarySection& section) {
    inputStream_.read(reinterpret_cast<char*>(&section), sizeof(section));
    return inputStream_.good();
}

bool BinarySerializer::padToAlignment(size_t alignment) {
    size_t currentPos = static_cast<size_t>(outputStream_.tellp());
    size_t remainder = currentPos % alignment;
    if (remainder != 0) {
        std::vector<uint8_t> padding(alignment - remainder, 0);
        outputStream_.write(reinterpret_cast<const char*>(padding.data()), padding.size());
    }
    return outputStream_.good();
}

std::string BinarySerializer::getName() const {
    return "BinarySerializer";
}

std::vector<std::type_index> ISerializer::getSupportedTypes() const {
    return std::vector<std::type_index>();
}

std::unique_ptr<ISerializer> SerializerFactory::createSerializer(
    SerializerType type,
    SerializationVersion version,
    CompressionLevel compression,
    uint32_t flags) {
    
    switch (type) {
        case SerializerType::Binary:
            return createBinarySerializer(version, compression, flags);
        case SerializerType::JSON:
            return createTextSerializer(version, compression, flags);
        case SerializerType::YAML:
            return createTextSerializer(version, compression, flags);
        case SerializerType::Checkpoint:
            return createCheckpointSerializer(version, compression, flags);
    }
    
    return nullptr;
}

std::unique_ptr<ISerializer> SerializerFactory::createTextSerializer(
    SerializationVersion version,
    CompressionLevel compression,
    uint32_t flags) {
    return std::make_unique<JSONSerializer>(version, compression, flags);
}

std::unique_ptr<ISerializer> SerializerFactory::createBinarySerializer(
    SerializationVersion version,
    CompressionLevel compression,
    uint32_t flags) {
    return std::make_unique<BinarySerializer>(version, compression, flags);
}

std::unique_ptr<ISerializer> SerializerFactory::createCheckpointSerializer(
    SerializationVersion version,
    CompressionLevel compression,
    uint32_t flags) {
    return std::make_unique<CheckpointSerializer>(version, compression, flags);
}

// JSON Serializer implementations
bool JSONSerializer::create(const std::string& filepath) {
    return executeSafely<bool>([&]() {
        std::remove(filepath.c_str());
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("JSONSerializer: Failed to create file: " + filepath);
            return false;
        }
        
        // Write JSON header
        json header;
        header["magic"] = "SNLM"; // NLM JSON format
        header["version"] = 1;
        header["timestamp"] = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        header["flags"] = getFlags();
        
        file << header.dump(2);
        
        data_.clear();
        currentPath_ = "";
        
        return true;
    });
}

bool JSONSerializer::write(const std::string& componentName, const json& data) {
    return executeSafely<bool>([&]() {
        // Convert component name to JSON path
        std::string path = currentPath_.empty() ? componentName : currentPath_ + "." + componentName;
        
        // Store data
        data_[path] = data;
        
        // Write to file if needed
        std::ofstream file(filepath_ + ".json", std::ios::app);
        if (file.is_open()) {
            // Append component data
            file << "\n{\"path\": \"" + path + "\", \"data\": " + data.dump() + "}";
        }
        
        return true;
    });
}

bool JSONSerializer::finalize() {
    return executeSafely<bool>([&]() {
        std::ofstream file(filepath_, std::ios::trunc);
        if (file.is_open()) {
            // Write all data to file
            file << data_.dump(2);
        }
        
        return true;
    });
}

bool JSONSerializer::open(const std::string& filepath) {
    return executeSafely<bool>([&]() {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("JSONSerializer: Failed to open file: " + filepath);
            return false;
        }
        
        // Read file content
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        // Parse JSON
        try {
            data_ = json::parse(buffer.str());
        } catch (const json::parse_error& e) {
            NLM_LOG_ERROR("JSONSerializer: Failed to parse JSON: " + std::string(e.what()));
            return false;
        }
        
        filepath_ = filepath;
        return true;
    });
}

json JSONSerializer::read(const std::string& componentName) {
    return executeSafely<json>([&]() {
        // Get component path
        std::string path = currentPath_.empty() ? componentName : currentPath_ + "." + componentName;
        
        // Find component in data
        auto it = data_.find(path);
        if (it == data_.end()) {
            NLM_LOG_ERROR("JSONSerializer: Component not found: " + componentName);
            return json();
        }
        
        return *it;
    });
}

void JSONSerializer::close() {
    data_.clear();
    filepath_.clear();
}

std::string JSONSerializer::getName() const {
    return "JSONSerializer";
}

// YAML Serializer implementations
bool YAMLSerializer::create(const std::string& filepath) {
    return executeSafely<bool>([&]() {
        std::remove(filepath.c_str());
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("YAMLSerializer: Failed to create file: " + filepath);
            return false;
        }
        
        // Write YAML header
        emitter_ << YAML::BeginMap;
        emitter_ << YAML::Key << "magic" << YAML::Value << "SNLM";
        emitter_ << YAML::Key << "version" << YAML::Value << 1;
        emitter_ << YAML::Key << "timestamp" << YAML::Value << 
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
        emitter_ << YAML::Key << "flags" << YAML::Value << getFlags();
        
        file << emitter_.c_str();
        
        data_ = YAML::Node();
        
        return true;
    });
}

bool YAMLSerializer::write(const std::string& componentName, const YAML::Node& data) {
    return executeSafely<bool>([&]() {
        // Add component to YAML data
        data_[componentName] = data;
        
        // Write to file if needed
        std::ofstream file(filepath_, std::ios::app);
        if (file.is_open()) {
            // Append component data
            file << "\n" << componentName << ": " << data;
        }
        
        return true;
    });
}

bool YAMLSerializer::finalize() {
    return executeSafely<bool>([&]() {
        std::ofstream file(filepath_);
        if (file.is_open()) {
            // Write all data to file
            file << data_;
        }
        
        return true;
    });
}

bool YAMLSerializer::open(const std::string& filepath) {
    return executeSafely<bool>([&]() {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("YAMLSerializer: Failed to open file: " + filepath);
            return false;
        }
        
        // Parse YAML
        try {
            data_ = YAML::Load(file);
        } catch (const YAML::Exception& e) {
            NLM_LOG_ERROR("YAMLSerializer: Failed to parse YAML: " + std::string(e.what()));
            return false;
        }
        
        filepath_ = filepath;
        return true;
    });
}

YAML::Node YAMLSerializer::read(const std::string& componentName) {
    return executeSafely<YAML::Node>([&]() {
        // Find component in data
        if (data_[componentName]) {
            return data_[componentName];
        }
        
        NLM_LOG_ERROR("YAMLSerializer: Component not found: " + componentName);
        return YAML::Node();
    });
}

void YAMLSerializer::close() {
    data_ = YAML::Node();
    filepath_.clear();
}

std::string YAMLSerializer::getName() const {
    return "YAMLSerializer";
}

// Checkpoint Serializer implementations
bool CheckpointSerializer::createCheckpoint(const std::string& name, const Brain& brain) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    return executeSafely<bool>([&]() {
        // Create checkpoint
        Checkpoint checkpoint;
        checkpoint.name = name;
        checkpoint.simulationStep = brain.getCurrentStep();
        checkpoint.simulationTime = brain.getCurrentTime();
        checkpoint.version = currentVersion_;
        checkpoint.creationTime = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        // Serialize brain state
        auto state = serializeBrainState(brain);
        checkpoint.components = state;
        
        // Calculate total size
        checkpoint.totalSize = 0;
        for (const auto& [name, data] : checkpoint.components) {
            checkpoint.totalSize += data.size();
        }
        
        // Calculate checksum
        checkpoint.checksum = calculateCheckpointChecksum(checkpoint.components);
        
        // Add to checkpoints
        checkpoints_.push_back(checkpoint);
        
        return true;
    });
}

bool CheckpointSerializer::saveCheckpoint(const std::string& filepath, const std::string& name, bool incremental) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    return executeSafely<bool>([&]() {
        if (!currentBrain_) {
            NLM_LOG_ERROR("CheckpointSerializer: No brain reference set");
            return false;
        }
        
        // Create checkpoint
        if (!createCheckpoint(name, *currentBrain_)) {
            NLM_LOG_ERROR("CheckpointSerializer: Failed to create checkpoint");
            return false;
        }
        
        // Save to file
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("CheckpointSerializer: Failed to create checkpoint file: " + filepath);
            return false;
        }
        
        // Write checkpoint data
        uint64_t checkpointIndex = checkpoints_.size() - 1;
        const auto& checkpoint = checkpoints_[checkpointIndex];
        
        // Write header
        uint32_t headerSize = sizeof(checkpoint.name) + sizeof(checkpoint.simulationStep) +
                            sizeof(checkpoint.simulationTime) + sizeof(checkpoint.version) +
                            sizeof(checkpoint.creationTime) + sizeof(checkpoint.totalSize) +
                            sizeof(checkpoint.checksum);
        
        file.write(reinterpret_cast<const char*>(&headerSize), sizeof(headerSize));
        file.write(checkpoint.name.c_str(), checkpoint.name.size() + 1); // +1 for null terminator
        file.write(reinterpret_cast<const char*>(&checkpoint.simulationStep), sizeof(checkpoint.simulationStep));
        file.write(reinterpret_cast<const char*>(&checkpoint.simulationTime), sizeof(checkpoint.simulationTime));
        file.write(reinterpret_cast<const char*>(&checkpoint.version), sizeof(checkpoint.version));
        file.write(reinterpret_cast<const char*>(&checkpoint.creationTime), sizeof(checkpoint.creationTime));
        file.write(reinterpret_cast<const char*>(&checkpoint.totalSize), sizeof(checkpoint.totalSize));
        file.write(reinterpret_cast<const char*>(&checkpoint.checksum), sizeof(checkpoint.checksum));
        
        // Write components
        uint32_t componentCount = checkpoint.components.size();
        file.write(reinterpret_cast<const char*>(&componentCount), sizeof(componentCount));
        
        for (const auto& [componentName, componentData] : checkpoint.components) {
            uint32_t nameLength = componentName.size();
            uint64_t dataSize = componentData.size();
            
            file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
            file.write(componentName.c_str(), nameLength);
            file.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
            file.write(reinterpret_cast<const char*>(componentData.data()), dataSize);
        }
        
        file.close();
        
        // Calculate delta if incremental
        if (incremental && !checkpoints_.empty()) {
            calculateDelta(checkpoint.components, checkpoints_[checkpointIndex - 1].components);
        }
        
        return true;
    });
}

bool CheckpointSerializer::loadCheckpoint(const std::string& filepath, Brain& brain, bool incremental) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    return executeSafely<bool>([&]() {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("CheckpointSerializer: Failed to open checkpoint file: " + filepath);
            return false;
        }
        
        // Read header
        uint32_t headerSize;
        file.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
        
        std::string name;
        char nameBuffer[256];
        file.read(nameBuffer, 255);
        nameBuffer[255] = '\0';
        name = nameBuffer;
        
        uint64_t simulationStep;
        double simulationTime;
        SerializationVersion version;
        uint64_t creationTime;
        uint64_t totalSize;
        uint32_t checksum;
        
        file.read(reinterpret_cast<char*>(&simulationStep), sizeof(simulationStep));
        file.read(reinterpret_cast<char*>(&simulationTime), sizeof(simulationTime));
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        file.read(reinterpret_cast<char*>(&creationTime), sizeof(creationTime));
        file.read(reinterpret_cast<char*>(&totalSize), sizeof(totalSize));
        file.read(reinterpret_cast<char*>(&checksum), sizeof(checksum));
        
        // Read components
        uint32_t componentCount;
        file.read(reinterpret_cast<char*>(&componentCount), sizeof(componentCount));
        
        std::map<std::string, std::vector<uint8_t>> components;
        for (uint32_t i = 0; i < componentCount; ++i) {
            uint32_t nameLength;
            file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
            
            char compNameBuffer[256];
            file.read(compNameBuffer, nameLength);
            compNameBuffer[nameLength] = '\0';
            std::string compName = compNameBuffer;
            
            uint64_t dataSize;
            file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
            
            std::vector<uint8_t> data(dataSize);
            file.read(reinterpret_cast<char*>(data.data()), dataSize);
            
            components[compName] = data;
        }
        
        file.close();
        
        // Verify checksum
        uint32_t calculatedChecksum = calculateCheckpointChecksum(components);
        if (calculatedChecksum != checksum) {
            NLM_LOG_ERROR("CheckpointSerializer: Checksum mismatch for checkpoint: " + filepath);
            return false;
        }
        
        // Deserialize into brain
        if (!deserializeBrainState(brain, components)) {
            NLM_LOG_ERROR("CheckpointSerializer: Failed to deserialize brain state");
            return false;
        }
        
        return true;
    });
}

bool CheckpointSerializer::loadCheckpoint(const std::string& name, Brain& brain, bool incremental) {
    for (const auto& checkpoint : checkpoints_) {
        if (checkpoint.name == name) {
            return deserializeBrainState(brain, checkpoint.components);
        }
    }
    
    NLM_LOG_ERROR("CheckpointSerializer: Checkpoint not found: " + name);
    return false;
}

std::vector<std::string> CheckpointSerializer::listCheckpoints() const {
    std::vector<std::string> names;
    for (const auto& checkpoint : checkpoints_) {
        names.push_back(checkpoint.name);
    }
    return names;
}

bool CheckpointSerializer::deleteCheckpoint(const std::string& name) {
    for (auto it = checkpoints_.begin(); it != checkpoints_.end(); ++it) {
        if (it->name == name) {
            checkpoints_.erase(it);
            return true;
        }
    }
    return false;
}

bool CheckpointSerializer::deleteCheckpoint(const std::string& filepath) {
    // Remove checkpoint from memory and file
    std::string name = extractCheckpointName(filepath);
    bool success = deleteCheckpoint(name);
    
    if (success) {
        std::remove(filepath.c_str());
    }
    
    return success;
}

SerializationVersion CheckpointSerializer::getCheckpointVersion(const std::string& checkpointName) const {
    for (const auto& checkpoint : checkpoints_) {
        if (checkpoint.name == checkpointName) {
            return checkpoint.version;
        }
    }
    
    return SerializationVersion(); // Default version
}

bool CheckpointSerializer::validateCheckpointVersion(const std::string& checkpointName, 
                                                    SerializationVersion minVersion) const {
    SerializationVersion version = getCheckpointVersion(checkpointName);
    return version.major >= minVersion.major && 
           version.minor >= minVersion.minor && 
           version.patch >= minVersion.patch;
}

bool CheckpointSerializer::calculateDelta(const std::map<std::string, std::vector<uint8_t>>& newState,
                                         const std::map<std::string, std::vector<uint8_t>>& oldState) {
    std::vector<uint8_t> delta;
    
    // Simple delta calculation: just store new state
    // In a real implementation, this would be more sophisticated
    for (const auto& [name, newData] : newState) {
        auto it = oldState.find(name);
        if (it == oldState.end() || it->second != newData) {
            // Component changed, include in delta
            delta.insert(delta.end(), newData.begin(), newData.end());
        }
    }
    
    // Store delta in current state
    if (!delta.empty()) {
        currentState_ = newState;
        // Store delta for later use
        // In a real implementation, this would be stored in a separate field
    }
    
    return true;
}

bool CheckpointSerializer::applyDelta(std::map<std::string, std::vector<uint8_t>>& currentState,
                                     const std::vector<uint8_t>& delta) {
    // Simple delta application: just store delta
    // In a real implementation, this would be more sophisticated
    currentState.clear();
    
    // Parse delta back into components
    // For now, just store the delta as-is
    currentState["delta"] = delta;
    
    return true;
}

uint32_t CheckpointSerializer::calculateCheckpointChecksum(const std::map<std::string, std::vector<uint8_t>>& components) const {
    // Calculate checksum of all component data
    uint32_t checksum = 0;
    for (const auto& [name, data] : components) {
        checksum = checksum ^ calculateChecksum(data.data(), data.size());
    }
    
    return checksum;
}

std::string CheckpointSerializer::extractCheckpointName(const std::string& filepath) const {
    // Extract checkpoint name from filepath (remove extension and path)
    size_t lastSlash = filepath.find_last_of('/');
    size_t lastDot = filepath.find_last_of('.');
    
    if (lastDot == std::string::npos) {
        return filepath.substr(lastSlash + 1);
    }
    
    return filepath.substr(lastSlash + 1, lastDot - lastSlash - 1);
}

// Checkpoint plugin for Integration with CheckpointSystem.hpp
class CheckpointPlugin : public ISerializer {
public:
    CheckpointPlugin(SerializationVersion version = SerializationVersion(), 
                     CompressionLevel compression = CompressionLevel::Balanced,
                     uint32_t flags = 0)
        : version_(version), compressionLevel_(compression), flags_(flags) {}
    
    SerializationVersion getVersion() const override { return version_; }
    std::string getName() const override { return "CheckpointPlugin"; }
    
    bool canSerialize(const std::type_info& type) const override {
        // Can serialize CheckpointHeader and related types
        return type == typeid(CheckpointHeader) ||
               type == typeid(SectionHeader) ||
               type == typeid(NeuronCheckpointData) ||
               type == typeid(SynapseCheckpointData);
    }
    
    std::vector<uint8_t> serialize(const void* data) override {
        return std::vector<uint8_t>(); // Implement binary serialization
    }
    
    bool deserialize(const std::vector<uint8_t>& data, void* out) override {
        return false; // Implement binary deserialization
    }
    
private:
    SerializationVersion version_;
    CompressionLevel compressionLevel_;
    uint32_t flags_;
};

// Integration with CheckpointSystem.hpp
class CheckpointSystemIntegration {
private:
    std::unique_ptr<CheckpointSerializer> checkpointSerializer_;
    CheckpointManager* checkpointManager_;
    
public:
    CheckpointSystemIntegration(CheckpointManager* manager)
        : checkpointManager_(manager) {
        checkpointSerializer_ = std::make_unique<CheckpointSerializer>(
            SerializationVersion(1, 0, 0)
        );
    }
    
    bool saveCheckpoint(const std::string& name, const Brain& brain) {
        std::string filepath = "./checkpoints/" + name + ".ckp";
        return checkpointSerializer_->saveCheckpoint(filepath, name, true);
    }
    
    bool loadCheckpoint(const std::string& name, Brain& brain) {
        return checkpointSerializer_->loadCheckpoint(name, brain, true);
    }
    
    bool saveCheckpointImmediately(const std::string& name) {
        if (!checkpointManager_) return false;
        
        std::string filepath = checkpointManager_->getLastCheckpointPath();
        if (filepath.empty()) {
            filepath = "./checkpoints/" + name + ".ckp";
        }
        
        return checkpointSerializer_->saveCheckpoint(filepath, name, false);
    }
    
    std::vector<std::string> listCheckpoints() const {
        return checkpointSerializer_->listCheckpoints();
    }
    
    bool deleteCheckpoint(const std::string& name) {
        return checkpointSerializer_->deleteCheckpoint(name);
    }
};

} // namespace nlm
