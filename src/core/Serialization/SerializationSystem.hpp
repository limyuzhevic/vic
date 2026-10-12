#pragma once

#include "SerializationBase.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <zlib.h>
#include <openssl/evp.h>
#include <chrono>
#include <filesystem>
#include <fstream>

using json = nlohmann::json;

namespace nlm {

// Binary format constants
constexpr uint32_t BINARY_MAGIC = 0x534E4C4D;  // "SNLM"
constexpr uint32_t TEXT_MAGIC = 0x544E4C4D;    // "TNLM"
constexpr uint32_t CHECKPOINT_MAGIC = 0x434B5644;  // "CKDT"

// Binary serializer implementation
class BinarySerializer : public BaseSerializer {
private:
    struct BinaryHeader {
        uint32_t magic;
        uint32_t version;
        uint64_t timestamp;
        uint64_t checksum;
        uint64_t totalSize;
        uint32_t componentCount;
        uint32_t flags;
    };
    
    struct BinarySection {
        uint32_t type;
        uint64_t offset;
        uint64_t size;
        uint64_t compressedSize;
        uint32_t checksum;
    };
    
    std::vector<BinarySection> sections_;
    std::ofstream outputStream_;
    std::ifstream inputStream_;
    
public:
    BinarySerializer(SerializationVersion version = SerializationVersion(), 
                    CompressionLevel compression = CompressionLevel::Balanced,
                    uint32_t flags = 0)
        : BaseSerializer(version, compression, flags) {}
    
    ~BinarySerializer() override {
        if (outputStream_.is_open()) outputStream_.close();
        if (inputStream_.is_open()) inputStream_.close();
    }
    
    std::string getName() const override { return "BinarySerializer"; }
    
    bool create(const std::string& filepath);
    bool write(const std::string& componentName, const std::vector<uint8_t>& data);
    bool finalize();
    
    bool open(const std::string& filepath);
    std::vector<uint8_t> read(const std::string& componentName);
    void close();
    
    bool compress(std::vector<uint8_t>& data);
    bool decompress(std::vector<uint8_t>& data);
    uint32_t calculateChecksum(const void* data, size_t size) const;
};

// JSON text serializer implementation
class JSONSerializer : public BaseSerializer {
private:
    json data_;
    std::string currentPath_;
    
public:
    JSONSerializer(SerializationVersion version = SerializationVersion(), 
                   CompressionLevel compression = CompressionLevel::Balanced,
                   uint32_t flags = 0)
        : BaseSerializer(version, compression, flags) {}
    
    std::string getName() const override { return "JSONSerializer"; }
    
    bool create(const std::string& filepath);
    bool write(const std::string& componentName, const json& data);
    bool finalize();
    
    bool open(const std::string& filepath);
    json read(const std::string& componentName);
    void close();
    
    json serializeBrainState(const Brain& brain) const;
    bool deserializeBrainState(Brain& brain, const json& data);
};

// YAML text serializer implementation
class YAMLSerializer : public BaseSerializer {
private:
    YAML::Emitter emitter_;
    YAML::Node data_;
    
public:
    YAMLSerializer(SerializationVersion version = SerializationVersion(), 
                   CompressionLevel compression = CompressionLevel::Balanced,
                   uint32_t flags = 0)
        : BaseSerializer(version, compression, flags) {}
    
    std::string getName() const override { return "YAMLSerializer"; }
    
    bool create(const std::string& filepath);
    bool write(const std::string& componentName, const YAML::Node& data);
    bool finalize();
    
    bool open(const std::string& filepath);
    YAML::Node read(const std::string& componentName);
    void close();
};

// Checkpoint serializer for complete brain state snapshots
class CheckpointSerializer {
private:
    struct Checkpoint {
        std::string name;
        uint64_t simulationStep;
        double simulationTime;
        SerializationVersion version;
        uint64_t creationTime;
        uint64_t totalSize;
        std::map<std::string, std::vector<uint8_t>> components;
        uint32_t checksum;
        std::optional<std::vector<uint8_t>> delta;  // For incremental saves
    };
    
    std::vector<Checkpoint> checkpoints_;
    std::map<std::string, std::vector<uint8_t>> currentState_;
    SerializationVersion currentVersion_;
    
public:
    CheckpointSerializer(SerializationVersion version = SerializationVersion()) 
        : currentVersion_(version) {}
    
    std::string getName() const { return "CheckpointSerializer"; }
    
    bool createCheckpoint(const std::string& name, const Brain& brain);
    bool saveCheckpoint(const std::string& filepath, bool incremental = false);
    bool loadCheckpoint(const std::string& filepath, Brain& brain, bool incremental = false);
    bool loadCheckpoint(const std::string& name, Brain& brain, bool incremental = false);
    
    std::vector<std::string> listCheckpoints() const;
    bool deleteCheckpoint(const std::string& name);
    bool deleteCheckpoint(const std::string& filepath);
    
    bool calculateDelta(const std::map<std::string, std::vector<uint8_t>>& newState,
                       std::vector<uint8_t>& delta);
    bool applyDelta(std::map<std::string, std::vector<uint8_t>>& currentState,
                   const std::vector<uint8_t>& delta);
    
    SerializationVersion getCheckpointVersion(const std::string& checkpointName) const;
    bool validateCheckpointVersion(const std::string& checkpointName, 
                                  SerializationVersion minVersion) const;
};

// Serializer factory for creating appropriate serializer instances
class SerializerFactory {
public:
    enum class SerializerType {
        Binary,
        JSON,
        YAML,
        Checkpoint
    };
    
    static std::unique_ptr<ISerializer> createSerializer(SerializerType type,
                                                         SerializationVersion version = SerializationVersion(),
                                                         CompressionLevel compression = CompressionLevel::Balanced,
                                                         uint32_t flags = 0);
    
    static std::unique_ptr<ISerializer> createTextSerializer(SerializationVersion version = SerializationVersion(),
                                                             CompressionLevel compression = CompressionLevel::Balanced,
                                                             uint32_t flags = 0);
    
    static std::unique_ptr<ISerializer> createBinarySerializer(SerializationVersion version = SerializationVersion(),
                                                              CompressionLevel compression = CompressionLevel::Balanced,
                                                              uint32_t flags = 0);
    
    static std::unique_ptr<ISerializer> createCheckpointSerializer(SerializationVersion version = SerializationVersion(),
                                                                  CompressionLevel compression = CompressionLevel::Balanced,
                                                                  uint32_t flags = 0);
};

// Advanced serialization system
class SerializationSystem {
private:
    std::unique_ptr<BinarySerializer> binarySerializer_;
    std::unique_ptr<JSONSerializer> jsonSerializer_;
    std::unique_ptr<YAMLSerializer> yamlSerializer_;
    std::unique_ptr<CheckpointSerializer> checkpointSerializer_;
    
    SerializerPluginManager pluginManager_;
    std::shared_ptr<SchemaValidator> validator_;
    SerializationStats stats_;
    
    mutable std::mutex mutex_;
    
    // Serialization hooks
    std::vector<std::function<void(const std::string&, const std::vector<uint8_t>&)>> preSerializeHooks_;
    std::vector<std::function<void(const std::string&, const std::vector<uint8_t>&)>> postSerializeHooks_;
    std::vector<std::function<bool(const std::string&, const std::vector<uint8_t>&)>> validateHooks_;
    
    // Component-specific serializers
    std::unordered_map<std::string, std::function<std::vector<uint8_t>(const Brain&)>> componentSerializers_;
    
public:
    SerializationSystem();
    ~SerializationSystem() = default;
    
    // Initialize with brain reference for component serialization
    bool initialize(Brain* brain) {
        if (!brain) return false;
        currentBrain_ = brain;
        registerDefaultComponentSerializers();
        return true;
    }
    
    // Binary serialization
    bool saveBinary(const std::string& filepath, bool compress = true);
    bool loadBinary(const std::string& filepath, Brain& brain);
    
    // Text serialization (JSON/YAML)
    bool saveText(const std::string& filepath, const std::string& format = "json", bool compress = false);
    bool loadText(const std::string& filepath, Brain& brain, const std::string& format = "json");
    
    // Checkpoint serialization
    bool saveCheckpoint(const std::string& filepath, const std::string& name = "", bool incremental = false);
    bool loadCheckpoint(const std::string& filepath, Brain& brain, bool incremental = false);
    bool loadCheckpoint(const std::string& name, Brain& brain, bool incremental = false);
    
    // Advanced features
    bool savePartial(const std::string& filepath, 
                    const std::vector<std::string>& components,
                    const std::string& format = "binary");
    bool loadPartial(const std::string& filepath,
                    Brain& brain,
                    const std::vector<std::string>& components,
                    const std::string& format = "binary");
    
    bool saveDelta(const std::string& filepath, const Brain& brain, const std::string& previousCheckpoint);
    bool loadDelta(const std::string& filepath, Brain& brain, const std::string& checkpointName);
    
    // Serialization hooks
    void addPreSerializeHook(std::function<void(const std::string&, const std::vector<uint8_t>&)> hook);
    void addPostSerializeHook(std::function<void(const std::string&, const std::vector<uint8_t>&)> hook);
    void addValidateHook(std::function<bool(const std::string&, const std::vector<uint8_t>&)> hook);
    
    // Plugin system
    void registerSerializer(std::unique_ptr<ISerializer> serializer);
    void registerValidator(std::shared_ptr<SchemaValidator> validator);
    
    // Statistics and monitoring
    SerializationStats getStats() const { return stats_; }
    void resetStats() { stats_.reset(); }
    
    // Version management
    SerializationVersion getCurrentVersion() const { return currentVersion_; }
    void setCurrentVersion(SerializationVersion version) { currentVersion_ = version; }
    
    // Configuration
    void setCompressionLevel(CompressionLevel level);
    void setEncryptionKey(const std::vector<uint8_t>& key);
    void setValidationEnabled(bool enabled);
    void setMaxFileSize(uint64_t size) { maxFileSize_ = size; }
    void setTimeoutMs(uint64_t timeout) { timeoutMs_ = timeout; }
    
    // Error handling
    bool handleError(const SerializationError& error);
    std::vector<SerializationError> getRecentErrors() const { return recentErrors_; }
    void clearErrors() { recentErrors_.clear(); }
    
    // Component registration
    void registerComponentSerializer(const std::string& name,
                                    std::function<std::vector<uint8_t>(const Brain&)> serializer);
    
    // Special component serialization
    std::vector<uint8_t> serializeNeuron(const Neuron& neuron) const;
    std::vector<uint8_t> serializeSynapse(const Synapse& synapse) const;
    std::vector<uint8_t> serializeMemory(const NeuralWorkingMemory& memory) const;
    std::vector<uint8_t> serializePredictionSystem(const PredictionSystem& prediction) const;
    std::vector<uint8_t> serializeNeuromodulation(const Dopamine& dopamine,
                                                  const Curiosity& curiosity,
                                                  const Novelty& novelty,
                                                  const PredictionError& predictionError) const;
    std::vector<uint8_t> serializeCognition(const NeuralPlanner& planner,
                                           const ConceptFormation& conceptFormation,
                                           const AttentionalSelection& attention) const;
    std::vector<uint8_t> serializeDevelopment(const DevelopmentSystem& development) const;
    std::vector<uint8_t> serializePlasticity(const STDP& stdp,
                                           const Hebbian& hebbian,
                                           const StructuralPlasticity& structural) const;
    std::vector<uint8_t> serializeSpikeSystem(const SpikeSystem& spikeSystem) const;
    
    // Deserialization helpers
    bool deserializeNeuron(Neuron& neuron, const std::vector<uint8_t>& data);
    bool deserializeSynapse(Synapse& synapse, const std::vector<uint8_t>& data);
    bool deserializeMemory(NeuralWorkingMemory& memory, const std::vector<uint8_t>& data);
    bool deserializePredictionSystem(PredictionSystem& prediction, const std::vector<uint8_t>& data);
    bool deserializeNeuromodulation(Dopamine& dopamine,
                                   Curiosity& curiosity,
                                   Novelty& novelty,
                                   PredictionError& predictionError,
                                   const std::vector<uint8_t>& data);
    bool deserializeCognition(NeuralPlanner& planner,
                             ConceptFormation& conceptFormation,
                             AttentionalSelection& attention,
                             const std::vector<uint8_t>& data);
    bool deserializeDevelopment(DevelopmentSystem& development, const std::vector<uint8_t>& data);
    bool deserializePlasticity(STDP& stdp,
                              Hebbian& hebbian,
                              StructuralPlasticity& structural,
                              const std::vector<uint8_t>& data);
    bool deserializeSpikeSystem(SpikeSystem& spikeSystem, const std::vector<uint8_t>& data);
    
    // Monitoring
    double getMemoryUsagePercentage() const;
    bool isOverMemoryLimit() const;
    uint64_t getEstimatedSize(const Brain& brain) const;
    
private:
    Brain* currentBrain_;
    SerializationVersion currentVersion_;
    CompressionLevel compressionLevel_;
    std::vector<uint8_t> encryptionKey_;
    bool validationEnabled_;
    uint64_t maxFileSize_;
    uint64_t timeoutMs_;
    std::vector<SerializationError> recentErrors_;
    
    void registerDefaultComponentSerializers();
    bool validateDataIntegrity(const std::vector<uint8_t>& data) const;
    void updateStats(uint64_t bytes, bool isRead, uint64_t timeMs);
};

} // namespace nlm
