#pragma once

#include "SerializationBase.hpp"
#include "BinarySerializer.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

// JSON Serializer implementation
class JSONSerializer : public BaseSerializer {
public:
    JSONSerializer(SerializationVersion version = SerializationVersion(), 
                   CompressionLevel compression = CompressionLevel::Balanced,
                   uint32_t flags = 0)
        : BaseSerializer(version, compression, flags) {}
    
    std::string getName() const override { return "JSONSerializer"; }
    
    bool save(const std::string& filepath, const std::map<std::string, std::vector<uint8_t>>& data);
    bool load(const std::string& filepath, std::map<std::string, std::vector<uint8_t>>& data);
    
    bool convertToJSON(const std::vector<uint8_t>& data, json& output) const;
    bool convertFromJSON(const json& input, std::vector<uint8_t>& output) const;
    
private:
    std::string filepath_;
};

// YAML Serializer implementation
class YAMLSerializer : public BaseSerializer {
public:
    YAMLSerializer(SerializationVersion version = SerializationVersion(), 
                   CompressionLevel compression = CompressionLevel::Balanced,
                   uint32_t flags = 0)
        : BaseSerializer(version, compression, flags) {}
    
    std::string getName() const override { return "YAMLSerializer"; }
    
    bool save(const std::string& filepath, const std::map<std::string, std::vector<uint8_t>>& data);
    bool load(const std::string& filepath, std::map<std::string, std::vector<uint8_t>>& data);
    
    bool convertToYAML(const std::vector<uint8_t>& data, YAML::Node& output) const;
    bool convertFromYAML(const YAML::Node& input, std::vector<uint8_t>& output) const;
    
private:
    std::string filepath_;
};

// Checkpoint Serializer implementation
class CheckpointSerializer : public BaseSerializer {
public:
    CheckpointSerializer(SerializationVersion version = SerializationVersion(), 
                         CompressionLevel compression = CompressionLevel::Balanced,
                         uint32_t flags = 0)
        : BaseSerializer(version, compression, flags) {}
    
    std::string getName() const override { return "CheckpointSerializer"; }
    
    bool saveCheckpoint(const std::string& filepath, const std::map<std::string, std::vector<uint8_t>>& data);
    bool loadCheckpoint(const std::string& filepath, std::map<std::string, std::vector<uint8_t>>& data);
    
    bool saveIncremental(const std::string& filepath, 
                         const std::map<std::string, std::vector<uint8_t>>& newData,
                         const std::map<std::string, std::vector<uint8_t>>& oldData);
    bool loadIncremental(const std::string& filepath,
                        std::map<std::string, std::vector<uint8_t>>& oldData,
                        std::map<std::string, std::vector<uint8_t>>& newData);
    
    bool createCheckpointFile(const std::string& filepath, 
                             const std::map<std::string, std::vector<uint8_t>>& data,
                             const std::string& name = "");
    bool readCheckpointFile(const std::string& filepath,
                           std::map<std::string, std::vector<uint8_t>>& data,
                           std::string& name);
    
private:
    struct CheckpointFileHeader {
        uint32_t magic;
        uint32_t version;
        uint64_t timestamp;
        uint32_t componentCount;
        uint64_t totalSize;
        uint32_t checksum;
    };
    
    struct CheckpointFileSection {
        uint32_t nameHash;
        uint64_t offset;
        uint64_t size;
        uint32_t checksum;
    };
};

// Schema validation for serialization
class SerializationSchemaValidator : public SchemaValidator {
public:
    SerializationSchemaValidator(SerializationVersion minVersion = SerializationVersion(1, 0, 0),
                                bool validateChecksums = true,
                                bool validateVersions = true)
        : minVersion_(minVersion), validateChecksums_(validateChecksums), 
          validateVersions_(validateVersions) {}
    
    bool validate(const std::vector<uint8_t>& data, SerializationVersion version) override;
    std::string getValidationError() const override { return validationError_; }
    
    SerializationVersion getMinVersion() const { return minVersion_; }
    void setMinVersion(SerializationVersion version) { minVersion_ = version; }
    
    bool getValidateChecksums() const { return validateChecksums_; }
    void setValidateChecksums(bool validate) { validateChecksums_ = validate; }
    
    bool getValidateVersions() const { return validateVersions_; }
    void setValidateVersions(bool validate) { validateVersions_ = validate; }
    
private:
    SerializationVersion minVersion_;
    bool validateChecksums_;
    bool validateVersions_;
    mutable std::string validationError_;
};

// Version compatibility checker
class VersionCompatibilityChecker {
public:
    enum class Compatibility {
        Compatible,
        UpgradeRequired,
        DowngradeRequired,
        Incompatible
    };
    
    static Compatibility checkCompatibility(SerializationVersion from, 
                                            SerializationVersion to,
                                            const std::string& componentName = "");
    
    static bool canUpgrade(SerializationVersion from, SerializationVersion to);
    static bool canDowngrade(SerializationVersion from, SerializationVersion to);
    
    static SerializationVersion findLowestCommonVersion(
        const std::vector<SerializationVersion>& versions);
    
    static std::vector<SerializationVersion> getCompatibleVersions(
        SerializationVersion currentVersion,
        const std::vector<SerializationVersion>& supportedVersions);
    
    static std::string compatibilityToString(Compatibility compat);
};

// Delta serializer for incremental updates
class DeltaSerializer {
public:
    DeltaSerializer(uint32_t maxDeltaSize = 1024 * 1024)  // 1MB default
        : maxDeltaSize_(maxDeltaSize) {}
    
    bool calculateDelta(const std::map<std::string, std::vector<uint8_t>>& oldState,
                       const std::map<std::string, std::vector<uint8_t>>& newState,
                       std::vector<uint8_t>& delta);
    
    bool applyDelta(std::map<std::string, std::vector<uint8_t>>& currentState,
                   const std::vector<uint8_t>& delta);
    
    bool validateDelta(const std::vector<uint8_t>& delta,
                      const std::map<std::string, std::vector<uint8_t>>& baseState);
    
    uint64_t estimateDeltaSize(const std::map<std::string, std::vector<uint8_t>>& oldState,
                              const std::map<std::string, std::vector<uint8_t>>& newState) const;
    
    bool compressDelta(std::vector<uint8_t>& delta);
    bool decompressDelta(std::vector<uint8_t>& delta);
    
private:
    uint32_t maxDeltaSize_;
    
    struct DeltaEntry {
        uint32_t nameHash;
        uint64_t oldOffset;
        uint64_t oldSize;
        uint64_t newOffset;
        uint64_t newSize;
        uint32_t oldChecksum;
        uint32_t newChecksum;
        std::vector<uint8_t> oldData;
        std::vector<uint8_t> newData;
    };
};

// Serialization statistics collector
class SerializationStatsCollector {
public:
    struct Stats {
        uint64_t totalBytesWritten = 0;
        uint64_t totalBytesRead = 0;
        uint64_t serializationTimeMs = 0;
        uint64_t deserializationTimeMs = 0;
        uint32_t checkpointCount = 0;
        uint32_t errorCount = 0;
        uint32_t compressionRatio = 0;
        
        void reset() {
            totalBytesWritten = 0;
            totalBytesRead = 0;
            serializationTimeMs = 0;
            deserializationTimeMs = 0;
            checkpointCount = 0;
            errorCount = 0;
            compressionRatio = 0;
        }
    };
    
    SerializationStatsCollector() : startTime_(std::chrono::high_resolution_clock::now()) {}
    
    void recordWrite(uint64_t bytes, uint64_t timeMs) {
        stats_.totalBytesWritten += bytes;
        stats_.serializationTimeMs += timeMs;
    }
    
    void recordRead(uint64_t bytes, uint64_t timeMs) {
        stats_.totalBytesRead += bytes;
        stats_.deserializationTimeMs += timeMs;
    }
    
    void recordCheckpoint() { stats_.checkpointCount++; }
    void recordError() { stats_.errorCount++; }
    void recordCompression(uint32_t ratio) { 
        stats_.compressionRatio = ratio; 
    }
    
    Stats getStats() const { return stats_; }
    void reset() { stats_.reset(); }
    
    double getCompressionEfficiency() const {
        if (stats_.totalBytesWritten == 0) return 0.0;
        return static_cast<double>(stats_.totalBytesWritten) / 
               static_cast<double>(stats_.totalBytesRead);
    }
    
    uint64_t getTotalTimeMs() const {
        return stats_.serializationTimeMs + stats_.deserializationTimeMs;
    }
    
private:
    Stats stats_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

// Encryption utility
class EncryptionUtility {
public:
    static bool encrypt(const std::vector<uint8_t>& input, 
                       std::vector<uint8_t>& output,
                       const std::vector<uint8_t>& key);
    
    static bool decrypt(const std::vector<uint8_t>& input,
                       std::vector<uint8_t>& output,
                       const std::vector<uint8_t>& key);
    
    static bool generateKey(std::vector<uint8_t>& key, size_t length = 32);
    
    static std::vector<uint8_t> deriveKey(const std::string& password,
                                         const std::string& salt,
                                         size_t keyLength = 32);
    
private:
    static const uint8_t iv_[16]; // Initialization vector
};

// Serialization progress monitor
class SerializationProgressMonitor {
public:
    SerializationProgressMonitor(uint32_t totalComponents = 0,
                                uint64_t totalBytes = 0)
        : totalComponents_(totalComponents), totalBytes_(totalBytes),
          completedComponents_(0), completedBytes_(0) {}
    
    void incrementComponents(uint32_t count = 1) {
        completedComponents_ += count;
    }
    
    void incrementBytes(uint64_t bytes) {
        completedBytes_ += bytes;
    }
    
    double getProgressPercentage() const {
        if (totalComponents_ == 0) return 0.0;
        return static_cast<double>(completedComponents_) / totalComponents_;
    }
    
    double getByteProgressPercentage() const {
        if (totalBytes_ == 0) return 0.0;
        return static_cast<double>(completedBytes_) / totalBytes_;
    }
    
    bool isComplete() const {
        return completedComponents_ >= totalComponents_ && completedBytes_ >= totalBytes_;
    }
    
    std::string getProgressString() const {
        std::stringstream ss;
        ss << "Progress: " << completedComponents_ << "/" << totalComponents_ 
           << " components, " << completedBytes_ << "/" << totalBytes_ << " bytes"
           << " (" << (isComplete() ? "Complete" : "In Progress") << ")";
        return ss.str();
    }
    
    void waitForCompletion(std::chrono::milliseconds timeout = std::chrono::milliseconds(30000)) {
        auto start = std::chrono::high_resolution_clock::now();
        while (!isComplete()) {
            if (std::chrono::high_resolution_clock::now() - start > timeout) {
                NLM_LOG_WARNING("SerializationProgressMonitor: Timeout waiting for completion");
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
private:
    uint32_t totalComponents_;
    uint64_t totalBytes_;
    uint32_t completedComponents_;
    uint64_t completedBytes_;
};

// Main serialization system with all components
class SerializationSystem : public BaseSerializer {
private:
    // Core components
    std::unique_ptr<JSONSerializer> jsonSerializer_;
    std::unique_ptr<YAMLSerializer> yamlSerializer_;
    std::unique_ptr<CheckpointSerializer> checkpointSerializer_;
    std::unique_ptr<DeltaSerializer> deltaSerializer_;
    
    // Supporting components
    std::shared_ptr<SerializationSchemaValidator> schemaValidator_;
    std::shared_ptr<SerializationStatsCollector> statsCollector_;
    
    // Integration
    std::shared_ptr<CheckpointSystemIntegration> checkpointSystemIntegration_;
    
    // State
    std::map<std::string, std::vector<uint8_t>> currentBrainState_;
    std::map<std::string, std::vector<uint8_t>> currentCheckpoint_;
    
    // Thread safety
    mutable std::mutex mutex_;
    SerializationProgressMonitor progressMonitor_;
    
    // Callbacks and hooks
    std::vector<std::function<void(const std::string&, const std::vector<uint8_t>&)>> preSerializeHooks_;
    std::vector<std::function<void(const std::string&, const std::vector<uint8_t>&)>> postSerializeHooks_;
    std::vector<std::function<bool(const std::string&, const std::vector<uint8_t>&)>> validateHooks_;
    
public:
    SerializationSystem(SerializationVersion version = SerializationVersion(1, 0, 0),
                       CompressionLevel compression = CompressionLevel::Balanced,
                       uint32_t flags = 0)
        : BaseSerializer(version, compression, flags) {
        
        jsonSerializer_ = std::make_unique<JSONSerializer>(version, compression, flags);
        yamlSerializer_ = std::make_unique<YAMLSerializer>(version, compression, flags);
        checkpointSerializer_ = std::make_unique<CheckpointSerializer>(version, compression, flags);
        deltaSerializer_ = std::make_unique<DeltaSerializer>();
        
        schemaValidator_ = std::make_shared<SerializationSchemaValidator>();
        statsCollector_ = std::make_shared<SerializationStatsCollector>();
    }
    
    std::string getName() const override { return "SerializationSystem"; }
    
    // Core serialization methods
    bool saveBinary(const std::string& filepath, const std::map<std::string, std::vector<uint8_t>>& data);
    bool loadBinary(const std::string& filepath, std::map<std::string, std::vector<uint8_t>>& data);
    
    bool saveText(const std::string& filepath, const std::string& format, 
                  const std::map<std::string, std::vector<uint8_t>>& data);
    bool loadText(const std::string& filepath, const std::string& format,
                  std::map<std::string, std::vector<uint8_t>>& data);
    
    bool saveCheckpoint(const std::string& filepath, const std::string& name,
                       const std::map<std::string, std::vector<uint8_t>>& data,
                       bool incremental = false);
    bool loadCheckpoint(const std::string& filepath, std::map<std::string, std::vector<uint8_t>>& data,
                       const std::string& name = "",
                       bool incremental = false);
    
    // Delta serialization
    bool saveDelta(const std::string& filepath, const std::map<std::string, std::vector<uint8_t>>& newData,
                  const std::map<std::string, std::vector<uint8_t>>& oldData,
                  const std::string& checkpointName = "");
    bool loadDelta(const std::string& filepath, std::map<std::string, std::vector<uint8_t>>& oldData,
                  const std::map<std::string, std::vector<uint8_t>>& newData,
                  const std::string& checkpointName = "");
    
    // Advanced features
    bool savePartial(const std::string& filepath, const std::vector<std::string>& components,
                    const std::string& format, const std::map<std::string, std::vector<uint8_t>>& data);
    bool loadPartial(const std::string& filepath, std::vector<std::string>& components,
                    const std::string& format, std::map<std::string, std::vector<uint8_t>>& data);
    
    bool saveEncrypted(const std::string& filepath, const std::vector<uint8_t>& key,
                      const std::map<std::string, std::vector<uint8_t>>& data);
    bool loadEncrypted(const std::string& filepath, const std::vector<uint8_t>& key,
                      std::map<std::string, std::vector<uint8_t>>& data);
    
    // Brain-specific serialization
    bool serializeBrain(const Brain& brain, std::map<std::string, std::vector<uint8_t>>& output);
    bool deserializeBrain(Brain& brain, const std::map<std::string, std::vector<uint8_t>>& input);
    
    // Checkpoint management
    bool createCheckpoint(const std::string& name, const Brain& brain);
    bool restoreCheckpoint(const std::string& name, Brain& brain);
    std::vector<std::string> listCheckpoints() const;
    bool deleteCheckpoint(const std::string& name);
    
    // Versioning
    SerializationVersion getCurrentVersion() const { return getVersion(); }
    void setVersion(SerializationVersion version) { setVersion(version); }
    bool validateVersion(SerializationVersion version) const;
    
    // Configuration
    void setCompressionLevel(CompressionLevel level) override {
        BaseSerializer::setCompressionLevel(level);
        jsonSerializer_->setCompressionLevel(level);
        yamlSerializer_->setCompressionLevel(level);
        checkpointSerializer_->setCompressionLevel(level);
    }
    
    void setEncryptionKey(const std::vector<uint8_t>& key) {
        encryptionKey_ = key;
        jsonSerializer_->setFlags(jsonSerializer_->getFlags() | 
                                static_cast<uint32_t>(SerializationFlags::Encrypted));
        yamlSerializer_->setFlags(yamlSerializer_->getFlags() | 
                                 static_cast<uint32_t>(SerializationFlags::Encrypted));
        checkpointSerializer_->setFlags(checkpointSerializer_->getFlags() | 
                                        static_cast<uint32_t>(SerializationFlags::Encrypted));
    }
    
    void setSchemaValidator(std::shared_ptr<SchemaValidator> validator) {
        schemaValidator_ = validator;
    }
    
    void setMaxFileSize(uint64_t size) { maxFileSize_ = size; }
    void setTimeoutMs(uint64_t timeout) { timeoutMs_ = timeout; }
    void setValidationEnabled(bool enabled) override {
        BaseSerializer::setValidationEnabled(enabled);
        schemaValidator_->setValidateChecksums(enabled);
        schemaValidator_->setValidateVersions(enabled);
    }
    
    // Statistics and monitoring
    SerializationStatsCollector::Stats getStats() const { return statsCollector_->getStats(); }
    void resetStats() { statsCollector_->reset(); }
    double getCompressionEfficiency() const { return statsCollector_->getCompressionEfficiency(); }
    
    // Error handling
    bool handleError(const SerializationError& error);
    std::vector<SerializationError> getRecentErrors() const { return recentErrors_; }
    void clearErrors() { recentErrors_.clear(); }
    
    // Callbacks and hooks
    void addPreSerializeHook(std::function<void(const std::string&, const std::vector<uint8_t>&)> hook) {
        preSerializeHooks_.push_back(std::move(hook));
    }
    
    void addPostSerializeHook(std::function<void(const std::string&, const std::vector<uint8_t>&)> hook) {
        postSerializeHooks_.push_back(std::move(hook));
    }
    
    void addValidateHook(std::function<bool(const std::string&, const std::vector<uint8_t>&)> hook) {
        validateHooks_.push_back(std::move(hook));
    }
    
    // Progress monitoring
    SerializationProgressMonitor& getProgressMonitor() { return progressMonitor_; }
    
    // Memory management
    bool isOverMemoryLimit() const;
    double getMemoryUsagePercentage() const;
    uint64_t getEstimatedSize(const std::map<std::string, std::vector<uint8_t>>& data) const;
    
    // Plugin system (to be implemented)
    bool registerPlugin(std::unique_ptr<ISerializer> plugin);
    bool unregisterPlugin(const std::string& name);
    
private:
    // Internal helper methods
    bool executeWithTimeout(std::function<bool()> operation);
    bool validateDataIntegrity(const std::vector<uint8_t>& data, uint32_t expectedChecksum) const;
    void updateStats(uint64_t bytes, bool isWrite, uint64_t timeMs);
    
    // Component-specific serializers (to be implemented)
    bool serializeNeurons(const std::vector<Neuron*>& neurons, std::vector<uint8_t>& output) const;
    bool serializeSynapses(const std::vector<Synapse*>& synapses, std::vector<uint8_t>& output) const;
    bool serializeMemory(const NeuralWorkingMemory* memory, std::vector<uint8_t>& output) const;
    bool serializePredictionSystem(const PredictionSystem* prediction, std::vector<uint8_t>& output) const;
    bool serializeNeuromodulation(const Neuromodulator* neuromod, std::vector<uint8_t>& output) const;
    bool serializeCognition(const NeuralPlanner* planner, const ConceptFormation* concepts, 
                           const AttentionalSelection* attention, std::vector<uint8_t>& output) const;
    bool serializeDevelopment(const DevelopmentSystem* development, std::vector<uint8_t>& output) const;
    bool serializePlasticity(const STDP* stdp, const Hebbian* hebbian, 
                           const StructuralPlasticity* structural, std::vector<uint8_t>& output) const;
    bool serializeSpikeSystem(const SpikeSystem* spikeSys, std::vector<uint8_t>& output) const;
    
    // Component deserialization (to be implemented)
    bool deserializeNeurons(std::vector<Neuron*>& neurons, const std::vector<uint8_t>& data);
    bool deserializeSynapses(std::vector<Synapse*>& synapses, const std::vector<uint8_t>& data);
    bool deserializeMemory(NeuralWorkingMemory* memory, const std::vector<uint8_t>& data);
    bool deserializePredictionSystem(PredictionSystem* prediction, const std::vector<uint8_t>& data);
    bool deserializeNeuromodulation(Neuromodulator* neuromod, const std::vector<uint8_t>& data);
    bool deserializeCognition(NeuralPlanner* planner, ConceptFormation* concepts,
                             AttentionalSelection* attention, const std::vector<uint8_t>& data);
    bool deserializeDevelopment(DevelopmentSystem* development, const std::vector<uint8_t>& data);
    bool deserializePlasticity(STDP* stdp, Hebbian* hebbian,
                               StructuralPlasticity* structural, const std::vector<uint8_t>& data);
    bool deserializeSpikeSystem(SpikeSystem* spikeSys, const std::vector<uint8_t>& data);
    
    // State management
    std::vector<uint8_t> encryptionKey_;
    bool validationEnabled_;
    uint64_t maxFileSize_;
    uint64_t timeoutMs_;
    
    std::vector<SerializationError> recentErrors_;
};

// Plugin system for custom serializers
class CustomSerializerPlugin : public ISerializer {
public:
    CustomSerializerPlugin(const std::string& name,
                          SerializationVersion version,
                          std::function<bool(const std::type_info&)> canSerialize,
                          std::function<std::vector<uint8_t>(const void*)> serialize,
                          std::function<bool(const std::vector<uint8_t>&, void*)> deserialize)
        : name_(name), version_(version), canSerialize_(canSerialize),
          serialize_(serialize), deserialize_(deserialize) {}
    
    SerializationVersion getVersion() const override { return version_; }
    std::string getName() const override { return name_; }
    bool canSerialize(const std::type_info& type) const override { return canSerialize_(type); }
    std::vector<uint8_t> serialize(const void* data) override { return serialize_(data); }
    bool deserialize(const std::vector<uint8_t>& data, void* out) override { return deserialize_(data, out); }
    
    std::vector<std::type_index> getSupportedTypes() const override {
        // Return types that this serializer can handle
        // This would be populated during registration
        return std::vector<std::type_index>();
    }
    
private:
    std::string name_;
    SerializationVersion version_;
    std::function<bool(const std::type_info&)> canSerialize_;
    std::function<std::vector<uint8_t>(const void*)> serialize_;
    std::function<bool(const std::vector<uint8_t>&, void*)> deserialize_;
};

// Specialized serializers for brain components
class NeuronSerializer : public ISerializer {
public:
    NeuronSerializer(SerializationVersion version = SerializationVersion()) 
        : version_(version) {}
    
    SerializationVersion getVersion() const override { return version_; }
    std::string getName() const override { return "NeuronSerializer"; }
    bool canSerialize(const std::type_info& type) const override {
        return type == typeid(Neuron);
    }
    std::vector<uint8_t> serialize(const void* data) override;
    bool deserialize(const std::vector<uint8_t>& data, void* out) override;
    
private:
    SerializationVersion version_;
};

class SynapseSerializer : public ISerializer {
public:
    SynapseSerializer(SerializationVersion version = SerializationVersion())
        : version_(version) {}
    
    SerializationVersion getVersion() const override { return version_; }
    std::string getName() const override { return "SynapseSerializer"; }
    bool canSerialize(const std::type_info& type) const override {
        return type == typeid(Synapse);
    }
    std::vector<uint8_t> serialize(const void* data) override;
    bool deserialize(const std::vector<uint8_t>& data, void* out) override;
    
private:
    SerializationVersion version_;
};

// Additional component serializers
class MemorySerializer : public ISerializer {
public:
    SerializationVersion getVersion() const override { return SerializationVersion(1, 0, 0); }
    std::string getName() const override { return "MemorySerializer"; }
    bool canSerialize(const std::type_info& type) const override {
        return type == typeid(NeuralWorkingMemory) ||
               type == typeid(NeuralEpisodicMemory) ||
               type == typeid(NeuralAssociativeMemory);
    }
    std::vector<uint8_t> serialize(const void* data) override;
    bool deserialize(const std::vector<uint8_t>& data, void* out) override;
};

class PredictionSystemSerializer : public ISerializer {
public:
    SerializationVersion getVersion() const override { return SerializationVersion(1, 0, 0); }
    std::string getName() const override { return "PredictionSystemSerializer"; }
    bool canSerialize(const std::type_info& type) const override {
        return type == typeid(PredictionSystem);
    }
    std::vector<uint8_t> serialize(const void* data) override;
    bool deserialize(const std::vector<uint8_t>& data, void* out) override;
};

class NeuromodulationSerializer : public ISerializer {
public:
    SerializationVersion getVersion() const override { return SerializationVersion(1, 0, 0); }
    std::string getName() const override { return "NeuromodulationSerializer"; }
    bool canSerialize(const std::type_info& type) const override {
        return type == typeid(Dopamine) ||
               type == typeid(Curiosity) ||
               type == typeid(Novelty) ||
               type == typeid(PredictionError);
    }
    std::vector<uint8_t> serialize(const void* data) override;
    bool deserialize(const std::vector<uint8_t>& data, void* out) override;
};

class CognitionSerializer : public ISerializer {
public:
    SerializationVersion getVersion() const override { return SerializationVersion(1, 0, 0); }
    std::string getName() const override { return "CognitionSerializer"; }
    bool canSerialize(const std::type_info& type) const override {
        return type == typeid(NeuralPlanner) ||
               type == typeid(ConceptFormation) ||
               type == typeid(AttentionalSelection);
    }
    std::vector<uint8_t> serialize(const void* data) override;
    bool deserialize(const std::vector<uint8_t>& data, void* out) override;
};

class DevelopmentSerializer : public ISerializer {
public:
    SerializationVersion getVersion() const override { return SerializationVersion(1, 0, 0); }
    std::string getName() const override { return "DevelopmentSerializer"; }
    bool canSerialize(const std::type_info& type) const override {
        return type == typeid(DevelopmentSystem);
    }
    std::vector<uint8_t> serialize(const void* data) override;
    bool deserialize(const std::vector<uint8_t>& data, void* out) override;
};

class PlasticitySerializer : public ISerializer {
public:
    SerializationVersion getVersion() const override { return SerializationVersion(1, 0, 0); }
    std::string getName() const override { return "PlasticitySerializer"; }
    bool canSerialize(const std::type_info& type) const override {
        return type == typeid(STDP) ||
               type == typeid(Hebbian) ||
               type == typeid(StructuralPlasticity);
    }
    std::vector<uint8_t> serialize(const void* data) override;
    bool deserialize(const std::vector<uint8_t>& data, void* out) override;
};

class SpikeSystemSerializer : public ISerializer {
public:
    SerializationVersion getVersion() const override { return SerializationVersion(1, 0, 0); }
    std::string getName() const override { return "SpikeSystemSerializer"; }
    bool canSerialize(const std::type_info& type) const override {
        return type == typeid(SpikeSystem);
    }
    std::vector<uint8_t> serialize(const void* data) override;
    bool deserialize(const std::vector<uint8_t>& data, void* out) override;
};

} // namespace nlm
