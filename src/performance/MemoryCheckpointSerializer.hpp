// MemoryCheckpointSerializer.hpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <optional>
#include <variant>
#include "BrainCheckpointSerializer.hpp"

namespace nlm {

/**
 * Advanced memory system serializer for comprehensive checkpointing
 * 
 * Features:
 * - Complete memory system serialization (working, episodic, associative, semantic, procedural)
 * - Incremental serialization for memory efficiency
 * - Differential serialization for memory checkpoint chains
 * - Compression integration
 * - Version compatibility handling
 * - Error recovery and validation
 * - Parallel serialization for large memory systems
 * - Memory-efficient serialization (zero-copy where possible)
 * - Memory system metadata and statistics
 */

struct MemoryCheckpoint {
    uint64_t magic{0x4E4C4D4D454D4F52ULL};  // "NLMMEMOR"
    uint32_t versionMajor{1};
    uint32_t versionMinor{0};
    uint32_t versionPatch{0};
    time_t timestamp{0};
    uint64_t memoryId{0};
    uint64_t step{0};
    double simulationTime{0.0};
    uint64_t workingMemoryItems{0};
    uint64_t episodicMemoryEpisodes{0};
    uint64_t semanticMemoryFacts{0};
    uint64_t proceduralMemorySkills{0};
    uint64_t associativeMemoryAssociations{0};
    uint64_t checksum{0};
    uint32_t compressionAlgorithm{0};
    uint32_t flags{0};
    
    // Serialization statistics
    uint64_t serializedSize{0};
    uint64_t compressedSize{0};
    double compressionRatio{0.0};
    double serializationTime{0.0};
    
    bool validate() const {
        return magic == 0x4E4C4D4D454D4F52ULL && versionMajor == 1;
    }
};

struct WorkingMemoryCheckpointData {
    uint64_t id{0};
    uint64_t capacity{0};
    uint64_t currentSize{0};
    uint64_t activeTraces{0};
    
    // Items
    struct Item {
        uint64_t neuronId{0};
        float value{0.0f};
        uint64_t timestamp{0};
        float decayFactor{0.0f};
        
        Item() = default;
        Item(uint64_t nid, float val, uint64_t ts) : neuronId(nid), value(val), timestamp(ts) {}
    };
    
    std::vector<Item> items;
    
    // Statistics
    uint64_t accessCount{0};
    float averageDecayRate{0.0f};
    float peakValue{0.0f};
    float averageValue{0.0f};
    
    WorkingMemoryCheckpointData() = default;
    
    size_t getSizeInBytes() const {
        size_t size = sizeof(id) + sizeof(capacity) + sizeof(currentSize) + 
                     sizeof(activeTraces) + items.size() * sizeof(Item);
        size += sizeof(accessCount) + sizeof(averageDecayRate) + 
                sizeof(peakValue) + sizeof(averageValue);
        return size;
    }
};

struct EpisodicMemoryCheckpointData {
    uint64_t id{0};
    uint64_t capacity{0};
    uint64_t currentSize{0};
    
    // Episodes
    struct Episode {
        uint64_t timestamp{0};
        uint64_t sequenceNumber{0};
        uint64_t sourceNeuronCount{0};
        uint64_t targetNeuronCount{0};
        
        // Neural pattern (compressed representation)
        struct NeuralPattern {
            std::vector<uint64_t> neuronIds;
            std::vector<float> values;
            float energy{0.0f};
            float coherence{0.0f};
            
            NeuralPattern() = default;
        };
        
        NeuralPattern pattern;
        
        // Temporal information
        double eventDuration{0.0};
        double eventIntensity{0.0f};
        float novelty{0.0f};
        std::string context;
        std::vector<std::string> tags;
        
        // Replay and consolidation
        bool isConsolidated{false};
        uint64_t consolidationStep{0};
        uint64_t replayCount{0};
        double replayStrength{0.0f};
        
        // Statistics
        float retrievalProbability{0.0f};
        uint64_t accessCount{0};
        uint64_t replayCount{0};
        float averageStrength{0.0f};
    };
    
    std::vector<Episode> episodes;
    
    // Performance metrics
    double averageEpisodeDuration{0.0};
    double averageEventIntensity{0.0f};
    float consolidationProgress{0.0f};
    uint64_t totalReplayCount{0};
    float averageReplayStrength{0.0f};
    
    EpisodicMemoryCheckpointData() = default;
    
    size_t getSizeInBytes() const {
        size_t size = sizeof(id) + sizeof(capacity) + sizeof(currentSize);
        for (const auto& episode : episodes) {
            size += sizeof(episode.timestamp) + sizeof(episode.sequenceNumber) +
                    sizeof(episode.sourceNeuronCount) + sizeof(episode.targetNeuronCount);
            size += sizeof(episode.pattern.energy) + sizeof(episode.pattern.coherence);
            size += episode.pattern.neuronIds.size() * sizeof(uint64_t);
            size += episode.pattern.values.size() * sizeof(float);
            size += sizeof(episode.eventDuration) + sizeof(episode.eventIntensity) +
                    sizeof(episode.novelty);
            size += episode.context.size() + episode.tags.size() * 16; // Estimate string sizes
            size += sizeof(episode.isConsolidated) + sizeof(episode.consolidationStep) +
                    sizeof(episode.replayCount) + sizeof(episode.replayStrength);
            size += sizeof(episode.retrievalProbability) + sizeof(episode.accessCount) +
                    sizeof(episode.replayCount) + sizeof(episode.averageStrength);
        }
        return size;
    }
};

struct SemanticMemoryCheckpointData {
    uint64_t id{0};
    
    // Facts
    struct Fact {
        std::string key;
        std::string value;
        uint64_t timestamp{0};
        float importance{0.0f};
        float confidence{0.0f};
        std::vector<std::string> relatedFacts;
        std::vector<float> confidenceScores;
        
        Fact() = default;
        Fact(const std::string& k, const std::string& v) : key(k), value(v) {}
    };
    
    std::vector<Fact> facts;
    
    // Index
    struct Index {
        std::unordered_map<std::string, std::vector<size_t>> keyIndex;
        std::unordered_map<std::string, std::vector<size_t>> valueIndex;
        std::unordered_map<std::string, std::vector<size_t>> tagIndex;
    };
    
    Index index;
    
    // Statistics
    uint64_t totalFactCount{0};
    float averageFactImportance{0.0f};
    float averageConfidence{0.0f};
    uint64_t totalAccessCount{0};
    float retrievalSuccessRate{0.0f};
    
    SemanticMemoryCheckpointData() = default;
    
    size_t getSizeInBytes() const {
        size_t size = sizeof(id);
        for (const auto& fact : facts) {
            size += fact.key.size() + fact.value.size() + sizeof(fact.timestamp) +
                    sizeof(fact.importance) + sizeof(fact.confidence);
            size += fact.relatedFacts.size() * 16; // Estimate string sizes
            size += fact.confidenceScores.size() * sizeof(float);
        }
        return size;
    }
};

struct ProceduralMemoryCheckpointData {
    uint64_t id{0};
    
    // Skills
    struct Skill {
        std::string name;
        std::vector<uint64_t> neuralPattern;
        float proficiency{0.0f};
        float masteryLevel{0.0f};
        uint64_t acquisitionStep{0};
        float usageCount{0.0f};
        float effectiveness{0.0f};
        std::vector<std::string> dependencies;
        std::vector<float> dependencyStrengths;
        
        Skill() = default;
        Skill(const std::string& n) : name(n) {}
    };
    
    std::vector<Skill> skills;
    
    // Performance metrics
    float averageProficiency{0.0f};
    float averageMastery{0.0f};
    float totalUsageCount{0.0f};
    float averageEffectiveness{0.0f};
    uint64_t totalSkillCount{0};
    uint64_t totalAcquisitionSteps{0};
    
    ProceduralMemoryCheckpointData() = default;
    
    size_t getSizeInBytes() const {
        size_t size = sizeof(id);
        for (const auto& skill : skills) {
            size += skill.name.size();
            size += skill.neuralPattern.size() * sizeof(uint64_t);
            size += sizeof(skill.proficiency) + sizeof(skill.masteryLevel) +
                    sizeof(skill.acquisitionStep) + sizeof(skill.usageCount) +
                    sizeof(skill.effectiveness);
            size += skill.dependencies.size() * 16; // Estimate string sizes
            size += skill.dependencyStrengths.size() * sizeof(float);
        }
        return size;
    }
};

struct AssociativeMemoryCheckpointData {
    uint64_t id{0};
    
    // Associations
    struct Association {
        uint64_t sourceNeuron{0};
        uint64_t targetNeuron{0};
        float strength{0.0f};
        float activation{0.0f};
        uint64_t lastUpdateStep{0};
        float temporalStability{0.0f};
        float reinforcementHistory{0.0f};
        
        Association() = default;
        Association(uint64_t src, uint64_t tgt, float str) : 
            sourceNeuron(src), targetNeuron(tgt), strength(str) {}
    };
    
    std::vector<Association> associations;
    
    // Statistics
    float averageStrength{0.0f};
    float averageActivation{0.0f};
    uint64_t totalAssociationCount{0};
    uint64_t totalActivationCount{0};
    float coherence{0.0f};
    float sparsity{0.0f};
    
    AssociativeMemoryCheckpointData() = default;
    
    size_t getSizeInBytes() const {
        size_t size = sizeof(id);
        for (const auto& assoc : associations) {
            size += sizeof(assoc.sourceNeuron) + sizeof(assoc.targetNeuron) +
                    sizeof(assoc.strength) + sizeof(assoc.activation) +
                    sizeof(assoc.lastUpdateStep) + sizeof(assoc.temporalStability) +
                    sizeof(assoc.reinforcementHistory);
        }
        return size;
    }
};

class MemoryCheckpointSerializer {
public:
    MemoryCheckpointSerializer();
    ~MemoryCheckpointSerializer();
    
    // Disable copying
    MemoryCheckpointSerializer(const MemoryCheckpointSerializer&) = delete;
    MemoryCheckpointSerializer& operator=(const MemoryCheckpointSerializer&) = delete;
    
    /**
     * Serialize memory systems to checkpoint format
     * @param workingMemory Working memory instance
     * @param episodicMemory Episodic memory instance
     * @param semanticMemory Semantic memory instance
     * @param proceduralMemory Procedural memory instance
     * @param associativeMemory Associative memory instance
     * @param checkpoint Reference to memory checkpoint metadata
     * @param compressionAlgorithm Compression algorithm to use
     * @param compressionLevel Compression level
     * @return Serialized data
     */
    std::vector<uint8_t> serialize(
        const WorkingMemory& workingMemory,
        const EpisodicMemory& episodicMemory,
        const SemanticMemory& semanticMemory,
        const ProceduralMemory& proceduralMemory,
        const AssociativeMemory& associativeMemory,
        MemoryCheckpoint& checkpoint,
        CompressionAlgorithm compressionAlgorithm = CompressionAlgorithm::Zstd,
        int compressionLevel = 3
    );
    
    /**
     * Serialize memory systems to file
     * @param workingMemory Working memory instance
     * @param episodicMemory Episodic memory instance
     * @param semanticMemory Semantic memory instance
     * @param proceduralMemory Procedural memory instance
     * @param associativeMemory Associative memory instance
     * @param filepath Output file path
     * @param compressionAlgorithm Compression algorithm to use
     * @param compressionLevel Compression level
     * @return true if successful
     */
    bool serializeToFile(
        const WorkingMemory& workingMemory,
        const EpisodicMemory& episodicMemory,
        const SemanticMemory& semanticMemory,
        const ProceduralMemory& proceduralMemory,
        const AssociativeMemory& associativeMemory,
        const std::string& filepath,
        CompressionAlgorithm compressionAlgorithm = CompressionAlgorithm::Zstd,
        int compressionLevel = 3
    );
    
    /**
     * Deserialize memory systems from checkpoint data
     * @param workingMemory Working memory instance to populate
     * @param episodicMemory Episodic memory instance to populate
     * @param semanticMemory Semantic memory instance to populate
     * @param proceduralMemory Procedural memory instance to populate
     * @param associativeMemory Associative memory instance to populate
     * @param data Checkpoint data
     * @param checkpoint Memory checkpoint metadata
     * @return true if successful
     */
    bool deserialize(
        WorkingMemory& workingMemory,
        EpisodicMemory& episodicMemory,
        SemanticMemory& semanticMemory,
        ProceduralMemory& proceduralMemory,
        AssociativeMemory& associativeMemory,
        const std::vector<uint8_t>& data,
        const MemoryCheckpoint& checkpoint
    );
    
    /**
     * Deserialize memory systems from file
     * @param workingMemory Working memory instance to populate
     * @param episodicMemory Episodic memory instance to populate
     * @param semanticMemory Semantic memory instance to populate
     * @param proceduralMemory Procedural memory instance to populate
     * @param associativeMemory Associative memory instance to populate
     * @param filepath Input file path
     * @return true if successful
     */
    bool deserializeFromFile(
        WorkingMemory& workingMemory,
        EpisodicMemory& episodicMemory,
        SemanticMemory& semanticMemory,
        ProceduralMemory& proceduralMemory,
        AssociativeMemory& associativeMemory,
        const std::string& filepath
    );
    
    /**
     * Create incremental memory checkpoint (only changed data)
     * @param workingMemory Current working memory
     * @param episodicMemory Current episodic memory
     * @param semanticMemory Current semantic memory
     * @param proceduralMemory Current procedural memory
     * @param associativeMemory Current associative memory
     * @param baseCheckpoint Base memory checkpoint
     * @param compressionAlgorithm Compression algorithm
     * @return Incremental memory checkpoint data
     */
    std::vector<uint8_t> createIncrementalMemoryCheckpoint(
        const WorkingMemory& workingMemory,
        const EpisodicMemory& episodicMemory,
        const SemanticMemory& semanticMemory,
        const ProceduralMemory& proceduralMemory,
        const AssociativeMemory& associativeMemory,
        const MemoryCheckpoint& baseCheckpoint,
        CompressionAlgorithm compressionAlgorithm = CompressionAlgorithm::Zstd
    );
    
    /**
     * Apply incremental memory checkpoint to memory systems
     * @param workingMemory Working memory instance to update
     * @param episodicMemory Episodic memory instance to update
     * @param semanticMemory Semantic memory instance to update
     * @param proceduralMemory Procedural memory instance to update
     * @param associativeMemory Associative memory instance to update
     * @param incrementalData Incremental memory checkpoint data
     * @param baseCheckpoint Base memory checkpoint metadata
     * @return true if successful
     */
    bool applyIncrementalMemoryCheckpoint(
        WorkingMemory& workingMemory,
        EpisodicMemory& episodicMemory,
        SemanticMemory& semanticMemory,
        ProceduralMemory& proceduralMemory,
        AssociativeMemory& associativeMemory,
        const std::vector<uint8_t>& incrementalData,
        const MemoryCheckpoint& baseCheckpoint
    );
    
    /**
     * Create differential memory checkpoint (differences from previous)
     * @param workingMemory Current working memory
     * @param episodicMemory Current episodic memory
     * @param semanticMemory Current semantic memory
     * @param proceduralMemory Current procedural memory
     * @param associativeMemory Current associative memory
     * @param previousCheckpoint Previous memory checkpoint
     * @param compressionAlgorithm Compression algorithm
     * @return Differential memory checkpoint data
     */
    std::vector<uint8_t> createDifferentialMemoryCheckpoint(
        const WorkingMemory& workingMemory,
        const EpisodicMemory& episodicMemory,
        const SemanticMemory& semanticMemory,
        const ProceduralMemory& proceduralMemory,
        const AssociativeMemory& associativeMemory,
        const MemoryCheckpoint& previousCheckpoint,
        CompressionAlgorithm compressionAlgorithm = CompressionAlgorithm::Zstd
    );
    
    /**
     * Apply differential memory checkpoint to memory systems
     * @param workingMemory Working memory instance to update
     * @param episodicMemory Episodic memory instance to update
     * @param semanticMemory Semantic memory instance to update
     * @param proceduralMemory Procedural memory instance to update
     * @param associativeMemory Associative memory instance to update
     * @param differentialData Differential memory checkpoint data
     * @param previousCheckpoint Previous memory checkpoint metadata
     * @return true if successful
     */
    bool applyDifferentialMemoryCheckpoint(
        WorkingMemory& workingMemory,
        EpisodicMemory& episodicMemory,
        SemanticMemory& semanticMemory,
        ProceduralMemory& proceduralMemory,
        AssociativeMemory& associativeMemory,
        const std::vector<uint8_t>& differentialData,
        const MemoryCheckpoint& previousCheckpoint
    );
    
    /**
     * Validate memory checkpoint integrity
     * @param data Memory checkpoint data
     * @param checkpoint Memory checkpoint metadata
     * @return true if valid
     */
    bool validate(const std::vector<uint8_t>& data, const MemoryCheckpoint& checkpoint) const;
    
    /**
     * Get memory checkpoint version compatibility
     * @param major Major version
     * @param minor Minor version
     * @param patch Patch version
     * @return true if compatible
     */
    bool isVersionCompatible(uint32_t major, uint32_t minor, uint32_t patch) const;
    
    /**
     * Set version compatibility range
     * @param minMajor Minimum major version
     * @param minMinor Minimum minor version
     * @param minPatch Minimum patch version
     */
    void setVersionCompatibility(uint32_t minMajor, uint32_t minMinor, uint32_t minPatch) {
        minMajorVersion_ = minMajor;
        minMinorVersion_ = minMinor;
        minPatchVersion_ = minPatch;
    }
    
    /**
     * Enable/disable parallel serialization
     */
    void setParallelEnabled(bool enabled) { parallelEnabled_ = enabled; }
    
    /**
     * Set maximum threads for parallel serialization
     */
    void setThreadCount(size_t threads) { threadCount_ = threads; }
    
    /**
     * Enable/disable memory-efficient serialization (zero-copy)
     */
    void setMemoryEfficientEnabled(bool enabled) { memoryEfficientEnabled_ = enabled; }
    
    /**
     * Get serialization statistics
     */
    struct SerializationStats {
        uint64_t workingMemoryItems{0};
        uint64_t episodicMemoryEpisodes{0};
        uint64_t semanticMemoryFacts{0};
        uint64_t proceduralMemorySkills{0};
        uint64_t associativeMemoryAssociations{0};
        uint64_t memoryBytes{0};
        uint64_t compressedBytes{0};
        double compressionRatio{0.0};
        double serializationTime{0.0};
        uint64_t parallelSpeedup{0};
    };
    
    SerializationStats getStats() const { return stats_; }
    
    /**
     * Reset serialization statistics
     */
    void resetStats() {
        std::lock_guard<std::mutex> lock(statsMutex_);
        stats_ = SerializationStats();
    }
    
    /**
     * Get compression engine instance
     */
    CompressionEngine& getCompressionEngine() { return compressionEngine_; }
    
    /**
     * Get version string
     */
    static std::string getVersion() { return "MemoryCheckpointSerializer v1.0"; }
    
private:
    // Core serialization functions
    std::vector<uint8_t> serializeWorkingMemory(const WorkingMemory& workingMemory);
    std::vector<uint8_t> serializeEpisodicMemory(const EpisodicMemory& episodicMemory);
    std::vector<uint8_t> serializeSemanticMemory(const SemanticMemory& semanticMemory);
    std::vector<uint8_t> serializeProceduralMemory(const ProceduralMemory& proceduralMemory);
    std::vector<uint8_t> serializeAssociativeMemory(const AssociativeMemory& associativeMemory);
    
    // Deserialization functions
    bool deserializeWorkingMemory(WorkingMemory& workingMemory, const std::vector<uint8_t>& data);
    bool deserializeEpisodicMemory(EpisodicMemory& episodicMemory, const std::vector<uint8_t>& data);
    bool deserializeSemanticMemory(SemanticMemory& semanticMemory, const std::vector<uint8_t>& data);
    bool deserializeProceduralMemory(ProceduralMemory& proceduralMemory, const std::vector<uint8_t>& data);
    bool deserializeAssociativeMemory(AssociativeMemory& associativeMemory, const std::vector<uint8_t>& data);
    
    // Incremental/differential serialization
    std::vector<uint8_t> serializeIncrementalMemoryChanges(
        const WorkingMemory& workingMemory,
        const EpisodicMemory& episodicMemory,
        const SemanticMemory& semanticMemory,
        const ProceduralMemory& proceduralMemory,
        const AssociativeMemory& associativeMemory,
        const MemoryCheckpoint& baseCheckpoint,
        const std::vector<uint8_t>& baseData
    );
    
    std::vector<uint8_t> serializeDifferentialMemoryChanges(
        const WorkingMemory& workingMemory,
        const EpisodicMemory& episodicMemory,
        const SemanticMemory& semanticMemory,
        const ProceduralMemory& proceduralMemory,
        const AssociativeMemory& associativeMemory,
        const MemoryCheckpoint& previousCheckpoint,
        const std::vector<uint8_t>& previousData
    );
    
    // Serialization utilities
    std::vector<uint8_t> serializeToBinary(const WorkingMemoryCheckpointData& data) const;
    std::vector<uint8_t> serializeToBinary(const EpisodicMemoryCheckpointData& data) const;
    std::vector<uint8_t> serializeToBinary(const SemanticMemoryCheckpointData& data) const;
    std::vector<uint8_t> serializeToBinary(const ProceduralMemoryCheckpointData& data) const;
    std::vector<uint8_t> serializeToBinary(const AssociativeMemoryCheckpointData& data) const;
    
    bool deserializeFromBinary(WorkingMemoryCheckpointData& data, const std::vector<uint8_t>& buffer) const;
    bool deserializeFromBinary(EpisodicMemoryCheckpointData& data, const std::vector<uint8_t>& buffer) const;
    bool deserializeFromBinary(SemanticMemoryCheckpointData& data, const std::vector<uint8_t>& buffer) const;
    bool deserializeFromBinary(ProceduralMemoryCheckpointData& data, const std::vector<uint8_t>& buffer) const;
    bool deserializeFromBinary(AssociativeMemoryCheckpointData& data, const std::vector<uint8_t>& buffer) const;
    
    // Serialization helpers
    template<typename T>
    std::vector<uint8_t> serializeVector(const std::vector<T>& data) const;
    
    template<typename T>
    bool deserializeVector(std::vector<T>& data, const std::vector<uint8_t>& buffer) const;
    
    std::vector<uint8_t> serializeString(const std::string& str) const;
    std::string deserializeString(const std::vector<uint8_t>& buffer, size_t& offset) const;
    
    uint64_t calculateChecksum(const std::vector<uint8_t>& data) const;
    
    // Version conversion
    std::vector<uint8_t> convertVersion(const std::vector<uint8_t>& data,
                                       uint32_t fromMajor, uint32_t fromMinor, uint32_t fromPatch,
                                       uint32_t toMajor, uint32_t toMinor, uint32_t toPatch);
    
    // File I/O
    bool writeCheckpointToFile(const std::vector<uint8_t>& data, const std::string& filepath);
    bool readCheckpointFromFile(std::vector<uint8_t>& data, const std::string& filepath);
    
    // Parallel processing
    template<typename Func>
    std::vector<uint8_t> parallelProcess(
        const std::vector<uint8_t>& data,
        Func processor,
        size_t chunkSize = 1024 * 1024  // 1MB chunks
    ) const;
    
    // Statistics
    mutable std::mutex statsMutex_;
    SerializationStats stats_;
    
    // Configuration
    bool parallelEnabled_{true};
    size_t threadCount_{std::thread::hardware_concurrency()};
    bool memoryEfficientEnabled_{true};
    
    // Version compatibility
    uint32_t minMajorVersion_{1};
    uint32_t minMinorVersion_{0};
    uint32_t minPatchVersion_{0};
    
    // Compression engine
    CompressionEngine compressionEngine_;
    
    // Error handling
    std::string lastError_;
    
    // Progress tracking
    std::function<void(double)> progressCallback_;
    
    // Serialization counters
    mutable uint64_t episodicMemorySize_;
    mutable uint64_t semanticMemorySize_;
    mutable uint64_t proceduralMemorySize_;
    mutable uint64_t associativeMemorySize_;
};