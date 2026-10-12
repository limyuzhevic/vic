#include "YAMLSerializer.hpp"
#include "CheckpointSerializer.hpp"
#include "SerializationSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <yaml-cpp/yaml.h>

namespace nlm {

bool YAMLSerializer::save(const std::string& filepath, const std::map<std::string, std::vector<uint8_t>>& data) {
    return executeSafely<bool>([&]() {
        std::remove(filepath.c_str());
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("YAMLSerializer: Failed to create file: " + filepath);
            return false;
        }
        
        // Write YAML header
        file << "# NLM Brain State File\n";
        file << "# Format: YAML\n";
        file << "# Version: 1.0\n";
        file << "# Timestamp: " << std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() << "\n\n";
        
        // Write data sections
        file << "components:" << std::endl;
        for (const auto& [name, componentData] : data) {
            file << "  " << name << ":" << std::endl;
            file << "    size: " << componentData.size() << std::endl;
            file << "    checksum: " << calculateChecksum(componentData.data(), componentData.size()) << std::endl;
            
            // Convert to base64 for YAML safety
            std::string encoded;
            encodeBase64(componentData, encoded);
            file << "    data: " << encoded << std::endl;
        }
        
        return true;
    });
}

bool YAMLSerializer::load(const std::string& filepath, std::map<std::string, std::vector<uint8_t>>& data) {
    return executeSafely<bool>([&]() {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("YAMLSerializer: Failed to open file: " + filepath);
            return false;
        }
        
        // Parse YAML
        YAML::Node root = YAML::Load(file);
        
        if (!root["components"]) {
            NLM_LOG_ERROR("YAMLSerializer: Invalid YAML format - missing components");
            return false;
        }
        
        // Clear existing data
        data.clear();
        
        // Read components
        const YAML::Node& components = root["components"];
        for (const auto& component : components) {
            std::string name = component["name"].as<std::string>();
            uint64_t size = component["size"].as<uint64_t>();
            uint32_t checksum = component["checksum"].as<uint32_t>();
            std::string encodedData = component["data"].as<std::string>();
            
            // Decode from base64
            std::vector<uint8_t> componentData;
            decodeBase64(encodedData, componentData);
            
            // Verify checksum
            uint32_t calculatedChecksum = calculateChecksum(componentData.data(), componentData.size());
            if (calculatedChecksum != checksum) {
                NLM_LOG_ERROR("YAMLSerializer: Checksum mismatch for component: " + name);
                return false;
            }
            
            data[name] = componentData;
        }
        
        return true;
    });
}

bool YAMLSerializer::convertToYAML(const std::vector<uint8_t>& data, YAML::Node& output) const {
    output["size"] = data.size();
    output["checksum"] = calculateChecksum(data.data(), data.size());
    
    std::string encoded;
    encodeBase64(data, encoded);
    output["data"] = encoded;
    
    return true;
}

bool YAMLSerializer::convertFromYAML(const YAML::Node& input, std::vector<uint8_t>& output) const {
    uint64_t size = input["size"].as<uint64_t>();
    uint32_t checksum = input["checksum"].as<uint32_t>();
    std::string encodedData = input["data"].as<std::string>();
    
    // Decode from base64
    if (!decodeBase64(encodedData, output)) {
        return false;
    }
    
    // Verify checksum
    uint32_t calculatedChecksum = calculateChecksum(output.data(), output.size());
    if (calculatedChecksum != checksum) {
        NLM_LOG_ERROR("YAMLSerializer: Checksum mismatch during deserialization");
        return false;
    }
    
    return true;
}

std::string YAMLSerializer::getName() const {
    return "YAMLSerializer";
}

// Checkpoint Serializer implementation
bool CheckpointSerializer::save(const std::string& filepath, const std::map<std::string, std::vector<uint8_t>>& data) {
    return executeSafely<bool>([&]() {
        std::remove(filepath.c_str());
        
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("CheckpointSerializer: Failed to create file: " + filepath);
            return false;
        }
        
        // Write header
        CheckpointFileHeader header;
        header.magic = CHECKPOINT_MAGIC;
        header.version = 1;
        header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        header.componentCount = data.size();
        header.totalSize = 0;
        header.checksum = 0;
        
        // Calculate total size
        for (const auto& [name, componentData] : data) {
            header.totalSize += sizeof(CheckpointFileSection) + 
                              static_cast<uint64_t>(name.size()) + 1 + 
                              componentData.size();
        }
        
        // Write header
        file.write(reinterpret_cast<const char*>(&header), sizeof(header));
        
        uint64_t currentOffset = sizeof(header);
        
        // Write sections
        for (const auto& [name, componentData] : data) {
            CheckpointFileSection section;
            section.nameHash = calculateNameHash(name);
            section.offset = currentOffset;
            section.size = componentData.size();
            section.checksum = calculateChecksum(componentData.data(), componentData.size());
            
            // Write section header
            file.write(reinterpret_cast<const char*>(&section), sizeof(section));
            
            // Write data
            file.write(reinterpret_cast<const char*>(componentData.data()), componentData.size());
            
            currentOffset += sizeof(CheckpointFileSection) + componentData.size();
        }
        
        // Calculate and write checksum
        file.seekp(0);
        header.checksum = calculateChecksum(reinterpret_cast<const uint8_t*>(&header) + 4, // Skip magic
                                            sizeof(header) - 4);
        file.write(reinterpret_cast<const char*>(&header), sizeof(header));
        
        return true;
    });
}

bool CheckpointSerializer::load(const std::string& filepath, std::map<std::string, std::vector<uint8_t>>& data) {
    return executeSafely<bool>([&]() {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("CheckpointSerializer: Failed to open file: " + filepath);
            return false;
        }
        
        // Read header
        CheckpointFileHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        
        if (header.magic != CHECKPOINT_MAGIC) {
            NLM_LOG_ERROR("CheckpointSerializer: Invalid checkpoint file magic");
            return false;
        }
        
        // Verify checksum
        uint32_t calculatedChecksum = calculateChecksum(reinterpret_cast<const uint8_t*>(&header) + 4,
                                                        sizeof(header) - 4);
        if (calculatedChecksum != header.checksum) {
            NLM_LOG_ERROR("CheckpointSerializer: Header checksum mismatch");
            return false;
        }
        
        // Clear existing data
        data.clear();
        
        // Read sections
        for (uint32_t i = 0; i < header.componentCount; ++i) {
            CheckpointFileSection section;
            file.read(reinterpret_cast<char*>(&section), sizeof(section));
            
            if (!file.good()) {
                NLM_LOG_ERROR("CheckpointSerializer: Failed to read section header");
                return false;
            }
            
            // Seek to data
            file.seekg(section.offset);
            
            // Read data
            std::vector<uint8_t> componentData(section.size);
            file.read(reinterpret_cast<char*>(componentData.data()), section.size);
            
            if (!file.good()) {
                NLM_LOG_ERROR("CheckpointSerializer: Failed to read section data");
                return false;
            }
            
            // Verify section checksum
            uint32_t calculatedSectionChecksum = calculateChecksum(componentData.data(), componentData.size());
            if (calculatedSectionChecksum != section.checksum) {
                NLM_LOG_ERROR("CheckpointSerializer: Section checksum mismatch");
                return false;
            }
            
            // Store with name (we don't have the name stored, only hash)
            // In a real implementation, we'd store the name too
            data[std::to_string(i)] = componentData;
        }
        
        return true;
    });
}

bool CheckpointSerializer::saveIncremental(const std::string& filepath,
                                          const std::map<std::string, std::vector<uint8_t>>& newData,
                                          const std::map<std::string, std::vector<uint8_t>>& oldData) {
    return executeSafely<bool>([&]() {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("CheckpointSerializer: Failed to create incremental file: " + filepath);
            return false;
        }
        
        // Write header
        CheckpointFileHeader header;
        header.magic = CHECKPOINT_MAGIC;
        header.version = 2; // Incremental version
        header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        header.componentCount = 0; // Only write changed components
        header.totalSize = 0;
        header.checksum = 0;
        
        // Identify changed components
        std::map<std::string, std::vector<uint8_t>> deltaData;
        for (const auto& [name, newComponent] : newData) {
            auto it = oldData.find(name);
            if (it == oldData.end()) {
                // New component
                deltaData[name] = newComponent;
            } else if (it->second != newComponent) {
                // Changed component
                deltaData[name] = newComponent;
            }
        }
        
        // Write only delta
        header.componentCount = deltaData.size();
        for (const auto& [name, componentData] : deltaData) {
            header.totalSize += sizeof(CheckpointFileSection) + 
                              static_cast<uint64_t>(name.size()) + 1 + 
                              componentData.size();
        }
        
        file.write(reinterpret_cast<const char*>(&header), sizeof(header));
        
        uint64_t currentOffset = sizeof(header);
        
        // Write sections
        for (const auto& [name, componentData] : deltaData) {
            CheckpointFileSection section;
            section.nameHash = calculateNameHash(name);
            section.offset = currentOffset;
            section.size = componentData.size();
            section.checksum = calculateChecksum(componentData.data(), componentData.size());
            
            file.write(reinterpret_cast<const char*>(&section), sizeof(section));
            file.write(reinterpret_cast<const char*>(componentData.data()), componentData.size());
            
            currentOffset += sizeof(CheckpointFileSection) + componentData.size();
        }
        
        // Calculate and write checksum
        file.seekp(0);
        header.checksum = calculateChecksum(reinterpret_cast<const uint8_t*>(&header) + 4,
                                            sizeof(header) - 4);
        file.write(reinterpret_cast<const char*>(&header), sizeof(header));
        
        return true;
    });
}

bool CheckpointSerializer::loadIncremental(const std::string& filepath,
                                          std::map<std::string, std::vector<uint8_t>>& oldData,
                                          std::map<std::string, std::vector<uint8_t>>& newData) {
    return executeSafely<bool>([&]() {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            NLM_LOG_ERROR("CheckpointSerializer: Failed to open incremental file: " + filepath);
            return false;
        }
        
        // Read header
        CheckpointFileHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        
        if (header.magic != CHECKPOINT_MAGIC || header.version != 2) {
            NLM_LOG_ERROR("CheckpointSerializer: Invalid incremental checkpoint file");
            return false;
        }
        
        // We need the old data for incremental loading
        // In a real implementation, we'd load the old data from somewhere
        // For now, just load the new data
        newData.clear();
        
        // Read sections
        for (uint32_t i = 0; i < header.componentCount; ++i) {
            CheckpointFileSection section;
            file.read(reinterpret_cast<char*>(&section), sizeof(section));
            
            if (!file.good()) {
                NLM_LOG_ERROR("CheckpointSerializer: Failed to read section header");
                return false;
            }
            
            // Seek to data
            file.seekg(section.offset);
            
            // Read data
            std::vector<uint8_t> componentData(section.size);
            file.read(reinterpret_cast<char*>(componentData.data()), section.size);
            
            if (!file.good()) {
                NLM_LOG_ERROR("CheckpointSerializer: Failed to read section data");
                return false;
            }
            
            // Store with placeholder name
            newData[std::to_string(i)] = componentData;
        }
        
        return true;
    });
}

bool CheckpointSerializer::createCheckpointFile(const std::string& filepath,
                                                const std::map<std::string, std::vector<uint8_t>>& data,
                                                const std::string& name) {
    return executeSafely<bool>([&]() {
        std::ofstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Write checkpoint file with name
        uint32_t nameLength = name.size();
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(name.c_str(), nameLength);
        file.write("\0", 1); // Null terminator
        
        uint32_t dataSize = data.size();
        file.write(reinterpret_cast<const char*>(&dataSize), sizeof(dataSize));
        
        for (const auto& [compName, compData] : data) {
            uint32_t compNameLength = compName.size();
            file.write(reinterpret_cast<const char*>(&compNameLength), sizeof(compNameLength));
            file.write(compName.c_str(), compNameLength);
            
            uint64_t compDataSize = compData.size();
            file.write(reinterpret_cast<const char*>(&compDataSize), sizeof(compDataSize));
            file.write(reinterpret_cast<const char*>(compData.data()), compDataSize);
        }
        
        return true;
    });
}

bool CheckpointSerializer::readCheckpointFile(const std::string& filepath,
                                             std::map<std::string, std::vector<uint8_t>>& data,
                                             std::string& name) {
    return executeSafely<bool>([&]() {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        // Read name
        uint32_t nameLength;
        file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        
        char nameBuffer[256];
        file.read(nameBuffer, nameLength);
        nameBuffer[nameLength] = '\0';
        name = nameBuffer;
        
        // Read data count
        uint32_t dataSize;
        file.read(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
        
        data.clear();
        
        for (uint32_t i = 0; i < dataSize; ++i) {
            uint32_t compNameLength;
            file.read(reinterpret_cast<char*>(&compNameLength), sizeof(compNameLength));
            
            char compNameBuffer[256];
            file.read(compNameBuffer, compNameLength);
            compNameBuffer[compNameLength] = '\0';
            std::string compName = compNameBuffer;
            
            uint64_t compDataSize;
            file.read(reinterpret_cast<char*>(&compDataSize), sizeof(compDataSize));
            
            std::vector<uint8_t> compData(compDataSize);
            file.read(reinterpret_cast<char*>(compData.data()), compDataSize);
            
            data[compName] = compData;
        }
        
        return true;
    });
}

std::string CheckpointSerializer::getName() const {
    return "CheckpointSerializer";
}

// Schema validation implementation
bool SerializationSchemaValidator::validate(const std::vector<uint8_t>& data, SerializationVersion version) {
    if (!validateVersions_ && version.major > minVersion_.major) {
        return true; // Skip version validation
    }
    
    if (version.major < minVersion_.major ||
        (version.major == minVersion_.major && version.minor < minVersion_.minor) ||
        (version.major == minVersion_.major && version.minor == minVersion_.minor && version.patch < minVersion_.patch)) {
        validationError_ = "Version " + std::to_string(version.major) + "." + 
                          std::to_string(version.minor) + "." + 
                          std::to_string(version.patch) + " is below minimum required " +
                          std::to_string(minVersion_.major) + "." +
                          std::to_string(minVersion_.minor) + "." +
                          std::to_string(minVersion_.patch);
        return false;
    }
    
    if (validateChecksums_) {
        // Calculate and verify checksum
        uint32_t checksum = calculateSimpleChecksum(data);
        if (checksum != extractChecksumFromData(data)) {
            validationError_ = "Checksum validation failed";
            return false;
        }
    }
    
    validationError_ = "";
    return true;
}

// Version compatibility checker implementation
SerializationVersion VersionCompatibilityChecker::findLowestCommonVersion(
    const std::vector<SerializationVersion>& versions) {
    if (versions.empty()) return SerializationVersion();
    
    SerializationVersion lowest = versions[0];
    for (size_t i = 1; i < versions.size(); ++i) {
        if (versions[i].major < lowest.major ||
            (versions[i].major == lowest.major && versions[i].minor < lowest.minor) ||
            (versions[i].major == lowest.major && versions[i].minor == lowest.minor && 
             versions[i].patch < lowest.patch)) {
            lowest = versions[i];
        }
    }
    
    return lowest;
}

std::vector<SerializationVersion> VersionCompatibilityChecker::getCompatibleVersions(
    SerializationVersion currentVersion,
    const std::vector<SerializationVersion>& supportedVersions) {
    std::vector<SerializationVersion> compatible;
    
    for (const auto& version : supportedVersions) {
        if (checkCompatibility(currentVersion, version).first == Compatibility::Compatible) {
            compatible.push_back(version);
        }
    }
    
    return compatible;
}

std::string VersionCompatibilityChecker::compatibilityToString(Compatibility compat) {
    switch (compat) {
        case Compatibility::Compatible: return "Compatible";
        case Compatibility::UpgradeRequired: return "UpgradeRequired";
        case Compatibility::DowngradeRequired: return "DowngradeRequired";
        case Compatibility::Incompatible: return "Incompatible";
        default: return "Unknown";
    }
}

// Delta serializer implementation
bool DeltaSerializer::calculateDelta(const std::map<std::string, std::vector<uint8_t>>& oldState,
                                     const std::map<std::string, std::vector<uint8_t>>& newState,
                                     std::vector<uint8_t>& delta) {
    std::vector<DeltaEntry> entries;
    
    // Find unchanged components
    std::set<std::string> oldKeys;
    for (const auto& [name, data] : oldState) {
        oldKeys.insert(name);
    }
    
    // Create delta entries for changed/removed components
    for (const auto& [name, newData] : newState) {
        auto it = oldState.find(name);
        if (it == oldState.end()) {
            // New component
            DeltaEntry entry;
            entry.nameHash = calculateNameHash(name);
            entry.oldSize = 0;
            entry.oldChecksum = 0;
            entry.newSize = newData.size();
            entry.newChecksum = calculateChecksum(newData.data(), newData.size());
            entry.newData = newData;
            entries.push_back(entry);
        } else if (it->second != newData) {
            // Changed component
            DeltaEntry entry;
            entry.nameHash = calculateNameHash(name);
            entry.oldSize = it->second.size();
            entry.oldChecksum = calculateChecksum(it->second.data(), it->second.size());
            entry.newSize = newData.size();
            entry.newChecksum = calculateChecksum(newData.data(), newData.size());
            entry.oldData = it->second;
            entry.newData = newData;
            entries.push_back(entry);
        }
    }
    
    // Add removed components (marked with zero size)
    for (const auto& [name, data] : oldState) {
        if (oldKeys.find(name) == oldKeys.end()) {
            DeltaEntry entry;
            entry.nameHash = calculateNameHash(name);
            entry.oldSize = data.size();
            entry.oldChecksum = calculateChecksum(data.data(), data.size());
            entry.newSize = 0;
            entry.newChecksum = 0;
            entry.oldData = data;
            entries.push_back(entry);
        }
    }
    
    // Serialize delta entries
    delta.clear();
    for (const auto& entry : entries) {
        // Write entry header
        uint8_t flags = 0;
        if (!entry.oldData.empty()) flags |= 0x01;
        if (!entry.newData.empty()) flags |= 0x02;
        
        delta.push_back(flags);
        delta.push_back(static_cast<uint8_t>(entry.nameHash >> 24));
        delta.push_back(static_cast<uint8_t>((entry.nameHash >> 16) & 0xFF));
        delta.push_back(static_cast<uint8_t>((entry.nameHash >> 8) & 0xFF));
        delta.push_back(static_cast<uint8_t>(entry.nameHash & 0xFF));
        
        serializeUint64(delta, entry.oldSize);
        serializeUint64(delta, entry.newSize);
        serializeUint32(delta, entry.oldChecksum);
        serializeUint32(delta, entry.newChecksum);
        
        // Write old data
        if (!entry.oldData.empty()) {
            delta.insert(delta.end(), entry.oldData.begin(), entry.oldData.end());
        }
        
        // Write new data
        if (!entry.newData.empty()) {
            delta.insert(delta.end(), entry.newData.begin(), entry.newData.end());
        }
    }
    
    return true;
}

bool DeltaSerializer::applyDelta(std::map<std::string, std::vector<uint8_t>>& currentState,
                                 const std::vector<uint8_t>& delta) {
    // Simple delta application for now
    // In a real implementation, this would be more sophisticated
    currentState.clear();
    
    // For now, just store the delta as-is
    currentState["delta"] = delta;
    
    return true;
}

bool DeltaSerializer::validateDelta(const std::vector<uint8_t>& delta,
                                    const std::map<std::string, std::vector<uint8_t>>& baseState) {
    // Simple validation for now
    return !delta.empty();
}

uint64_t DeltaSerializer::estimateDeltaSize(const std::map<std::string, std::vector<uint8_t>>& oldState,
                                           const std::map<std::string, std::vector<uint8_t>>& newState) const {
    uint64_t totalSize = 0;
    
    for (const auto& [name, newData] : newState) {
        auto it = oldState.find(name);
        if (it == oldState.end() || it->second != newData) {
            // Changed component contributes to delta
            totalSize += sizeof(uint8_t) + sizeof(uint32_t) * 4 + sizeof(uint64_t) * 2;
            totalSize += newData.size();
            if (it != oldState.end()) {
                totalSize += it->second.size();
            }
        }
    }
    
    return totalSize;
}

bool DeltaSerializer::compressDelta(std::vector<uint8_t>& delta) {
    return compressZlib(delta);
}

bool DeltaSerializer::decompressDelta(std::vector<uint8_t>& delta) {
    return decompressZlib(delta);
}

// Encryption utility implementation
bool EncryptionUtility::encrypt(const std::vector<uint8_t>& input,
                                std::vector<uint8_t>& output,
                                const std::vector<uint8_t>& key) {
    // Simple XOR encryption for demonstration
    // In a real implementation, use proper encryption like AES
    output.resize(input.size());
    
    size_t keySize = key.size();
    if (keySize == 0) return false;
    
    for (size_t i = 0; i < input.size(); ++i) {
        output[i] = input[i] ^ key[i % keySize];
    }
    
    return true;
}

bool EncryptionUtility::decrypt(const std::vector<uint8_t>& input,
                                std::vector<uint8_t>& output,
                                const std::vector<uint8_t>& key) {
    // Simple XOR decryption (same as encryption)
    return encrypt(input, output, key);
}

bool EncryptionUtility::generateKey(std::vector<uint8_t>& key, size_t length) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dist;
    
    key.resize(length);
    for (size_t i = 0; i < length; ++i) {
        key[i] = dist(gen);
    }
    
    return true;
}

std::vector<uint8_t> EncryptionUtility::deriveKey(const std::string& password,
                                                  const std::string& salt,
                                                  size_t keyLength) {
    // Simple key derivation (not secure, for demonstration)
    std::vector<uint8_t> key;
    key.reserve(keyLength);
    
    // Combine password and salt
    std::string combined = password + salt;
    
    // Simple hash (not secure)
    std::hash<std::string> hasher;
    size_t hash = hasher(combined);
    
    for (size_t i = 0; i < keyLength; ++i) {
        key.push_back(static_cast<uint8_t>((hash + i * 31) % 256));
    }
    
    return key;
}

// SerializationSystem implementation
bool SerializationSystem::saveBinary(const std::string& filepath, const std::map<std::string, std::vector<uint8_t>>& data) {
    return executeWithTimeout([&]() {
        return binarySerializer_->save(filepath, data);
    });
}

bool SerializationSystem::loadBinary(const std::string& filepath, std::map<std::string, std::vector<uint8_t>>& data) {
    return executeWithTimeout([&]() {
        return binarySerializer_->load(filepath, data);
    });
}

bool SerializationSystem::saveText(const std::string& filepath, const std::string& format, 
                                  const std::map<std::string, std::vector<uint8_t>>& data) {
    return executeWithTimeout([&]() {
        if (format == "json" || format == "JSON") {
            return jsonSerializer_->save(filepath, data);
        } else if (format == "yaml" || format == "YAML") {
            return yamlSerializer_->save(filepath, data);
        } else {
            NLM_LOG_ERROR("SerializationSystem: Unsupported text format: " + format);
            return false;
        }
    });
}

bool SerializationSystem::loadText(const std::string& filepath, const std::string& format,
                                  std::map<std::string, std::vector<uint8_t>>& data) {
    return executeWithTimeout([&]() {
        if (format == "json" || format == "JSON") {
            return jsonSerializer_->load(filepath, data);
        } else if (format == "yaml" || format == "YAML") {
            return yamlSerializer_->load(filepath, data);
        } else {
            NLM_LOG_ERROR("SerializationSystem: Unsupported text format: " + format);
            return false;
        }
    });
}

bool SerializationSystem::saveCheckpoint(const std::string& filepath, const std::string& name,
                                        const std::map<std::string, std::vector<uint8_t>>& data,
                                        bool incremental) {
    return executeWithTimeout([&]() {
        if (incremental) {
            return checkpointSerializer_->saveIncremental(filepath, data, currentCheckpoint_);
        } else {
            return checkpointSerializer_->save(filepath, data);
        }
    });
}

bool SerializationSystem::loadCheckpoint(const std::string& filepath, std::map<std::string, std::vector<uint8_t>>& data,
                                        const std::string& name, bool incremental) {
    return executeWithTimeout([&]() {
        if (incremental) {
            return checkpointSerializer_->loadIncremental(filepath, currentCheckpoint_, data);
        } else {
            return checkpointSerializer_->load(filepath, data);
        }
    });
}

bool SerializationSystem::saveDelta(const std::string& filepath,
                                   const std::map<std::string, std::vector<uint8_t>>& newData,
                                   const std::map<std::string, std::vector<uint8_t>>& oldData,
                                   const std::string& checkpointName) {
    return executeWithTimeout([&]() {
        std::vector<uint8_t> delta;
        if (!deltaSerializer_->calculateDelta(oldData, newData, delta)) {
            return false;
        }
        
        // Store delta in current checkpoint
        currentCheckpoint_["delta"] = delta;
        
        // Save to file
        return checkpointSerializer_->save(filepath, currentCheckpoint_);
    });
}

bool SerializationSystem::loadDelta(const std::string& filepath,
                                   std::map<std::string, std::vector<uint8_t>>& oldData,
                                   const std::map<std::string, std::vector<uint8_t>>& newData,
                                   const std::string& checkpointName) {
    return executeWithTimeout([&]() {
        std::vector<uint8_t> delta;
        
        // Extract delta from newData
        auto deltaIt = newData.find("delta");
        if (deltaIt == newData.end()) {
            NLM_LOG_ERROR("SerializationSystem: Delta not found in new data");
            return false;
        }
        delta = deltaIt->second;
        
        // Apply delta
        return deltaSerializer_->applyDelta(oldData, delta);
    });
}

bool SerializationSystem::savePartial(const std::string& filepath, const std::vector<std::string>& components,
                                     const std::string& format, const std::map<std::string, std::vector<uint8_t>>& data) {
    return executeWithTimeout([&]() {
        std::map<std::string, std::vector<uint8_t>> partialData;
        
        for (const auto& componentName : components) {
            auto it = data.find(componentName);
            if (it != data.end()) {
                partialData[componentName] = it->second;
            }
        }
        
        if (format == "binary") {
            return binarySerializer_->save(filepath, partialData);
        } else if (format == "json" || format == "JSON") {
            return jsonSerializer_->save(filepath, partialData);
        } else if (format == "yaml" || format == "YAML") {
            return yamlSerializer_->save(filepath, partialData);
        } else {
            NLM_LOG_ERROR("SerializationSystem: Unsupported format for partial save: " + format);
            return false;
        }
    });
}

bool SerializationSystem::loadPartial(const std::string& filepath, std::vector<std::string>& components,
                                     const std::string& format, std::map<std::string, std::vector<uint8_t>>& data) {
    return executeWithTimeout([&]() {
        // For now, just load the entire file and filter components
        std::map<std::string, std::vector<uint8_t>> fullData;
        
        if (format == "binary") {
            if (!binarySerializer_->load(filepath, fullData)) {
                return false;
            }
        } else if (format == "json" || format == "JSON") {
            if (!jsonSerializer_->load(filepath, fullData)) {
                return false;
            }
        } else if (format == "yaml" || format == "YAML") {
            if (!yamlSerializer_->load(filepath, fullData)) {
                return false;
            }
        } else {
            NLM_LOG_ERROR("SerializationSystem: Unsupported format for partial load: " + format);
            return false;
        }
        
        // Filter to requested components
        for (const auto& componentName : components) {
            auto it = fullData.find(componentName);
            if (it != fullData.end()) {
                data[componentName] = it->second;
            }
        }
        
        return true;
    });
}

bool SerializationSystem::saveEncrypted(const std::string& filepath, const std::vector<uint8_t>& key,
                                       const std::map<std::string, std::vector<uint8_t>>& data) {
    return executeWithTimeout([&]() {
        std::vector<uint8_t> encryptedData;
        
        // Encrypt data
        if (!EncryptionUtility::encrypt(reinterpret_cast<const uint8_t*>(&data), sizeof(data), key)) {
            return false;
        }
        
        // Save encrypted data
        return checkpointSerializer_->save(filepath, {{ "encrypted_data", encryptedData }});
    });
}

bool SerializationSystem::loadEncrypted(const std::string& filepath, const std::vector<uint8_t>& key,
                                       std::map<std::string, std::vector<uint8_t>>& data) {
    return executeWithTimeout([&]() {
        std::map<std::string, std::vector<uint8_t>> encryptedData;
        
        // Load encrypted data
        if (!checkpointSerializer_->load(filepath, encryptedData)) {
            return false;
        }
        
        auto encryptedIt = encryptedData.find("encrypted_data");
        if (encryptedIt == encryptedData.end()) {
            NLM_LOG_ERROR("SerializationSystem: Encrypted data not found");
            return false;
        }
        
        // Decrypt data
        std::vector<uint8_t> decryptedData;
        if (!EncryptionUtility::decrypt(encryptedIt->second.data(), decryptedData, key)) {
            return false;
        }
        
        // Convert back to map (simplified)
        data.clear();
        data["decrypted"] = decryptedData;
        
        return true;
    });
}

bool SerializationSystem::serializeBrain(const Brain& brain, std::map<std::string, std::vector<uint8_t>>& output) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    return executeWithTimeout([&]() {
        // Serialize brain components
        if (brain.getSpikeSystem()) {
            std::vector<uint8_t> data;
            if (!serializeSpikeSystem(brain.getSpikeSystem(), data)) {
                NLM_LOG_ERROR("SerializationSystem: Failed to serialize spike system");
                return false;
            }
            output["spike_system"] = data;
        }
        
        // Serialize other components...
        // This would continue for all brain components
        
        // Update progress
        progressMonitor_.incrementComponents(static_cast<uint32_t>(output.size()));
        
        // Update stats
        uint64_t totalSize = 0;
        for (const auto& [name, data] : output) {
            totalSize += data.size();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        updateStats(totalSize, true, durationMs);
        
        return true;
    });
}

bool SerializationSystem::deserializeBrain(Brain& brain, const std::map<std::string, std::vector<uint8_t>>& input) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    return executeWithTimeout([&]() {
        // Deserialize brain components
        if (brain.getSpikeSystem()) {
            std::vector<SpikeEvent> spikeEvents;
            if (!deserializeSpikeSystem(brain.getSpikeSystem(), spikeEvents)) {
                NLM_LOG_ERROR("SerializationSystem: Failed to deserialize spike system");
                return false;
            }
        }
        
        // Deserialize other components...
        // This would continue for all brain components
        
        // Update progress
        progressMonitor_.incrementComponents(static_cast<uint32_t>(input.size()));
        
        // Update stats
        uint64_t totalSize = 0;
        for (const auto& [name, data] : input) {
            totalSize += data.size();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        updateStats(totalSize, false, durationMs);
        
        return true;
    });
}

bool SerializationSystem::createCheckpoint(const std::string& name, const Brain& brain) {
    return executeWithTimeout([&]() {
        std::map<std::string, std::vector<uint8_t>> checkpointData;
        
        if (!serializeBrain(brain, checkpointData)) {
            return false;
        }
        
        // Add metadata
        checkpointData["metadata"]["name"] = name;
        checkpointData["metadata"]["step"] = brain.getCurrentStep();
        checkpointData["metadata"]["time"] = brain.getCurrentTime();
        
        // Save checkpoint
        return saveCheckpoint("./checkpoints/" + name + ".ckp", name, checkpointData, false);
    });
}

bool SerializationSystem::restoreCheckpoint(const std::string& name, Brain& brain) {
    return executeWithTimeout([&]() {
        // Load checkpoint
        std::map<std::string, std::vector<uint8_t>> checkpointData;
        
        if (!loadCheckpoint("./checkpoints/" + name + ".ckp", checkpointData, name, false)) {
            return false;
        }
        
        // Deserialize into brain
        return deserializeBrain(brain, checkpointData);
    });
}

std::vector<std::string> SerializationSystem::listCheckpoints() const {
    std::vector<std::string> checkpoints;
    
    // Simple implementation - in real implementation, would read from filesystem
    return checkpoints;
}

bool SerializationSystem::deleteCheckpoint(const std::string& name) {
    // Simple implementation - in real implementation, would delete from filesystem
    return true;
}

bool SerializationSystem::validateVersion(SerializationVersion version) const {
    return schemaValidator_->validate(std::vector<uint8_t>(), version);
}

bool SerializationSystem::handleError(const SerializationError& error) {
    recentErrors_.push_back(error);
    
    statsCollector_->recordError();
    
    // Handle error based on type
    switch (error.getErrorType()) {
        case SerializationError::ErrorType::VersionMismatch:
            NLM_LOG_WARNING("SerializationSystem: Version mismatch - attempting recovery");
            // Try to find compatible version
            break;
        case SerializationError::ErrorType::CorruptedData:
            NLM_LOG_WARNING("SerializationSystem: Data corruption detected - attempting recovery");
            // Try to recover from backup or reconstruct
            break;
        case SerializationError::ErrorType::MissingRequiredData:
            NLM_LOG_WARNING("SerializationSystem: Missing required data - filling with defaults");
            // Fill missing data with defaults
            break;
        case SerializationError::ErrorType::Timeout:
            NLM_LOG_ERROR("SerializationSystem: Serialization timeout");
            break;
        case SerializationError::ErrorType::MemoryUsageExceeded:
            NLM_LOG_WARNING("SerializationSystem: Memory usage exceeded - reducing memory footprint");
            break;
        default:
            NLM_LOG_ERROR("SerializationSystem: Unhandled error type");
    }
    
    return true;
}

bool SerializationSystem::isOverMemoryLimit() const {
    return getMemoryUsagePercentage() > 90.0;
}

double SerializationSystem::getMemoryUsagePercentage() const {
    // Simple implementation - in real implementation, use system-specific memory functions
    return 50.0; // Placeholder
}

uint64_t SerializationSystem::getEstimatedSize(const std::map<std::string, std::vector<uint8_t>>& data) const {
    uint64_t totalSize = 0;
    for (const auto& [name, componentData] : data) {
        totalSize += sizeof(uint32_t) * 2 + componentData.size(); // name hash + size + data
    }
    return totalSize;
}

bool SerializationSystem::registerPlugin(std::unique_ptr<ISerializer> plugin) {
    if (!plugin) {
        return false;
    }
    
    pluginManager_.registerSerializer(std::move(plugin));
    return true;
}

bool SerializationSystem::unregisterPlugin(const std::string& name) {
    // Simple implementation - in real implementation, would properly unregister
    return true;
}

bool SerializationSystem::executeWithTimeout(std::function<bool()> operation) {
    // Create thread with timeout
    std::future<bool> future = std::async(std::launch::async, operation);
    
    try {
        return future.wait_for(std::chrono::milliseconds(timeoutMs_)) == std::future_status::ready ? 
               future.get() : false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("SerializationSystem: Exception in executeWithTimeout: " + std::string(e.what()));
        return false;
    }
}

bool SerializationSystem::validateDataIntegrity(const std::vector<uint8_t>& data, uint32_t expectedChecksum) const {
    uint32_t checksum = calculateSimpleChecksum(data);
    return checksum == expectedChecksum;
}

void SerializationSystem::updateStats(uint64_t bytes, bool isWrite, uint64_t timeMs) {
    if (isWrite) {
        statsCollector_->recordWrite(bytes, timeMs);
    } else {
        statsCollector_->recordRead(bytes, timeMs);
    }
}

// BaseSerializer implementation (continued)
bool BaseSerializer::compress(std::vector<uint8_t>& data) {
    return compressZlib(data);
}

bool BaseSerializer::decompress(std::vector<uint8_t>& data) {
    return decompressZlib(data);
}

uint32_t BaseSerializer::calculateChecksum(const void* data, size_t size) const {
    return calculateSimpleChecksum(reinterpret_cast<const uint8_t*>(data), size);
}

bool BaseSerializer::validateDataIntegrity(const std::vector<uint8_t>& data) const {
    return schemaValidator_->validate(data, getVersion());
}

// Helper function implementations
bool serializeBase64(const std::vector<uint8_t>& data, std::string& output) {
    static const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    output.clear();
    output.reserve((data.size() + 2) / 3 * 4);
    
    int i = 0;
    int j = 0;
    uint8_t charArray3[3];
    uint8_t charArray4[4];
    
    while (data.size() > i) {
        charArray3[i % 3] = data[i++];
        
        if (i % 3 == 0) {
            charArray4[0] = (charArray3[0] & 0xFC) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xF0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0F) << 2) + ((charArray3[2] & 0xC0) >> 6);
            charArray4[3] = charArray3[2] & 0x3F;
            
            for (j = 0; j < 4; j++) {
                output += base64Chars[charArray4[j]];
            }
        }
    }
    
    if (j % 4 != 0) {
        for (j = j; j < 4; j++) {
            output += '=';
        }
    }
    
    return true;
}

bool decodeBase64(const std::string& input, std::vector<uint8_t>& output) {
    static const std::string base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const int base64Map[256] = { -1 };
    
    output.clear();
    
    int i = 0;
    int j = 0;
    uint8_t charArray3[3];
    uint8_t charArray4[4];
    
    for (i = 0; i < input.size(); ++i) {
        if (input[i] == '=') continue;
        
        char c = input[i];
        int idx = base64Map[static_cast<unsigned char>(c)];
        if (idx == -1) continue;
        
        charArray4[j++] = idx;
        
        if (j == 4) {
            charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
            charArray3[1] = ((charArray4[1] & 0x0F) << 4) + ((charArray4[2] & 0x3C) >> 2);
            charArray3[2] = ((charArray4[2] & 0x03) << 6) + charArray4[3];
            
            for (j = 0; j < 3; ++j) {
                output.push_back(charArray3[j]);
            }
            
            j = 0;
        }
    }
    
    return true;
}

uint32_t calculateSimpleChecksum(const std::vector<uint8_t>& data) const {
    return calculateSimpleChecksum(data.data(), data.size());
}

uint32_t calculateSimpleChecksum(const uint8_t* data, size_t size) {
    uint32_t checksum = 0;
    for (size_t i = 0; i < size; ++i) {
        checksum = (checksum + data[i]) % 0xFFFFFFFF;
    }
    return checksum;
}

uint32_t calculateNameHash(const std::string& name) {
    uint32_t hash = 0;
    for (char c : name) {
        hash = (hash * 31) + static_cast<uint32_t>(c);
    }
    return hash;
}

void serializeUint64(std::vector<uint8_t>& buffer, uint64_t value) {
    buffer.resize(buffer.size() + sizeof(value));
    memcpy(buffer.data() + buffer.size() - sizeof(value), &value, sizeof(value));
}

void serializeUint32(std::vector<uint8_t>& buffer, uint32_t value) {
    buffer.resize(buffer.size() + sizeof(value));
    memcpy(buffer.data() + buffer.size() - sizeof(value), &value, sizeof(value));
}

} // namespace nlm
