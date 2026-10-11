// BrainCheckpointSerializer.hpp
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
#include "CheckpointSystem.hpp"

namespace nlm {

/**
 * Advanced brain state serializer for comprehensive checkpointing
 * 
 * Features:
 * - Complete brain state serialization (neurons, synapses, plasticity, etc.)
 * - Incremental serialization for memory efficiency
 * - Differential serialization for checkpoint chains
 * - Compression integration
 * - Version compatibility handling
 * - Error recovery and validation
 * - Parallel serialization for large brains
 * - Memory-efficient serialization (zero-copy where possible)
 */

struct BrainCheckpoint {
    uint64_t magic{0x4E4C4D4243454E54ULL};  // "NLMBCEN"
    uint32_t versionMajor{1};
    uint32_t versionMinor{0};
    uint32_t versionPatch{0};
    time_t timestamp{0};
    uint64_t brainId{0};
    uint64_t step{0};
    double simulationTime{0.0};
    uint64_t neuronCount{0};
    uint64_t synapseCount{0};
    uint64_t regionCount{0};
    uint64_t plasticityRuleCount{0};
    uint64_t neuromodulatorCount{0};
    uint64_t memorySystemCount{0};
    uint64_t predictionSystemCount{0};
    uint64_t cognitionSystemCount{0};
    uint64_t developmentStageCount{0};
    uint64_t checksum{0};
    uint32_t compressionAlgorithm{0};
    uint32_t flags{0};
    
    // Serialization statistics
    uint64_t serializedSize{0};
    uint64_t compressedSize{0};
    double compressionRatio{0.0};
    double serializationTime{0.0};
    
    bool validate() const {
        return magic == 0x4E4C4D4243454E54ULL && versionMajor == 1;
    }
};

struct NeuronState {
    uint64_t id{0};
    NeuronType type{NeuronType::Interneuron};
    uint64_t regionId{0};
    uint64_t populationId{0};
    
    // Membrane properties
    float membranePotential{0.0f};
    float restingPotential{0.0f};
    float threshold{0.0f};
    float resetPotential{0.0f};
    float leakConductance{0.0f};
    
    // State variables
    uint8_t firingState{0};
    uint32_t refractoryRemaining{0};
    uint32_t refractoryPeriod{0};
    float lastSpikeTime{0.0f};
    
    // Dynamics
    float calciumConcentration{0.0f};
    float adaptationCurrent{0.0f};
    float potassiumCurrent{0.0f};
    float sodiumCurrent{0.0f};
    
    // Plasticity
    float eligibilityTrace{0.0f};
    float synapticEfficacy{0.0f};
    float shortTermDepression{0.0f};
    float shortTermFacilitation{0.0f};
    
    // Development
    float developmentalStage{0.0f};
    float maturationFactor{0.0f};
    float plasticityRate{0.0f};
    
    // Statistics
    uint32_t spikeCount{0};
    float averageFiringRate{0.0f};
    float lastFiringTime{0.0f};
    
    // Connectivity
    uint64_t inputSynapseCount{0};
    uint64_t outputSynapseCount{0};
    float averageInputWeight{0.0f};
    float averageOutputWeight{0.0f};
};

struct SynapseState {
    uint64_t id{0};
    uint64_t sourceNeuron{0};
    uint64_t destinationNeuron{0};
    uint64_t regionId{0};
    
    // Synaptic properties
    float weight{0.0f};
    uint32_t delay{0};
    SynapseType type{SynapseType::Excitatory};
    
    // Plasticity
    uint8_t plasticityFlags{0};
    float eligibilityTrace{0.0f};
    float efficacy{0.0f};
    float shortTermDepression{0.0f};
    float shortTermFacilitation{0.0f};
    
    // Development
    float developmentalStage{0.0f};
    float connectionStability{0.0f};
    
    // Dynamics
    float averagePostSynapticPotential{0.0f};
    float synapticCurrent{0.0f};
    float reversalPotential{0.0f};
    
    // Statistics
    uint32_t activationCount{0};
    float averageWeightChange{0.0f};
    uint32_t lastUpdateStep{0};
};

struct PlasticityRuleState {
    uint64_t id{0};
    PlasticityRuleType ruleType{PlasticityRuleType::STDP};
    std::string name;
    
    // Rule parameters
    float learningRate{0.0f};
    float traceDecay{0.0f};
    float stabilityFactor{0.0f};
    float metaPlasticityRate{0.0f};
    
    // State
    float eligibilityTrace{0.0f};
    float accumulatedWeightChange{0.0f};
    uint64_t lastUpdateStep{0};
    
    // Distribution
    uint64_t appliedToSynapseCount{0};
    uint64_t efficiency{0};
};

struct NeuromodulatorState {
    uint64_t id{0};
    NeuromodulatorType type{NeuromodulatorType::Dopamine};
    std::string name;
    
    // Concentration and dynamics
    float concentration{0.0f};
    float baseline{0.0f};
    float maxConcentration{0.0f};
    float decayRate{0.0f};
    
    // Production and modulation
    float productionRate{0.0f};
    float uptakeRate{0.0f};
    float modulationStrength{0.0f};
    float sensitivity{0.0f};
    
    // State
    float currentLevel{0.0f};
    float targetLevel{0.0f};
    float adaptationSpeed{0.0f};
    
    // Regulation
    float autoRegulator{0.0f};
    float homeostaticTarget{0.0f};
    float stressLevel{0.0f};
    
    // Statistics
    uint64_t updateCount{0};
    float averageLevel{0.0f};
    float peakLevel{0.0f};
};

struct MemorySystemState {
    uint64_t id{0};
    MemorySystemType systemType{MemorySystemType::WorkingMemory};
    std::string name;
    
    // Capacity and usage
    uint64_t capacity{0};
    uint64_t usedMemory{0};
    uint64_t activeTraces{0};
    uint64_t totalItems{0};
    
    // Performance metrics
    float accessLatency{0.0f};
    float consolidationRate{0.0f};
    float retrievalSuccessRate{0.0f};
    float encodingEfficiency{0.0f};
    
    // State
    float systemLoad{0.0f};
    float consolidationProgress{0.0f};
    float interferenceLevel{0.0f};
    float patternCompletion{0.0f};
    
    // Statistics
    uint64_t accessCount{0};
    uint64_t consolidationCount{0};
    uint64_t patternCount{0};
    float averageLoad{0.0f};
};

struct PredictionSystemState {
    uint64_t id{0};
    PredictionSystemType systemType{PredictionSystemType::TemporalSequence};
    std::string name;
    
    // Prediction performance
    float predictionAccuracy{0.0f};
    float predictionConfidence{0.0f};
    float predictionHorizon{0.0f};
    uint64_t predictionCount{0};
    
    // State
    float predictionStrength{0.0f};
    float uncertainty{0.0f};
    float novelty{0.0f};
    float errorSignal{0.0f};
    
    // Temporal dynamics
    float temporalDecay{0.0f};
    float memoryConsolidation{0.0f};
    float sequenceCompletion{0.0f};
    
    // Statistics
    uint64_t correctPredictions{0};
    uint64_t incorrectPredictions{0};
    float averageConfidence{0.0f};
};

struct CognitionSystemState {
    uint64_t id{0};
    CognitionSystemType systemType{CognitionSystemType::Attention};
    std::string name;
    
    // Cognitive performance
    float processingSpeed{0.0f};
    float attentionStrength{0.0f};
    float workingMemoryLoad{0.0f};
    float decisionConfidence{0.0f};
    
    // State
    float activation{0.0f};
    float competitionLevel{0.0f};
    float topDownBias{0.0f};
    float bottomUpInput{0.0f};
    
    // Dynamics
    float adaptationRate{0.0f};
    float plasticityModulation{0.0f};
    float interference{0.0f};
    float selectionStrength{0.0f};
    
    // Statistics
    uint64_t selectionCount{0};
    uint32_t processingTime{0};
    float averageActivation{0.0f};
};

struct DevelopmentState {
    uint64_t id{0};
    DevelopmentalStage stage{DevelopmentalStage::Initial};
    std::string name;
    
    // Development progress
    float progress{0.0f};
    float maturity{0.0f};
    float stability{0.0f};
    float plasticity{0.0f};
    
    // Timeline
    float startTime{0.0f};
    float endTime{0.0f};
    float duration{0.0f};
    float currentAge{0.0f};
    
    // Regulation
    float growthRate{0.0f};
    float pruningRate{0.0f};
    float synaptogenesisRate{0.0f};
    float myelinationRate{0.0f};
    
    // Statistics
    uint64_t eventCount{0};
    float averageProgress{0.0f};
    float developmentEfficiency{0.0f};
};

struct InterRegionConnectionState {
    uint64_t id{0};
    RegionId sourceRegion{0};
    RegionId targetRegion{0};
    
    // Connection properties
    float weight{0.0f};
    uint32_t delay{0};
    uint8_t connectionType{0};
    float connectionStrength{0.0f};
    
    // Plasticity
    uint8_t plasticityFlags{0};
    float eligibilityTrace{0.0f};
    float efficacy{0.0f};
    
    // Dynamics
    float averageConductance{0.0f};
    float temporalFilter{0.0f};
    float frequencyResponse{0.0f};
    
    // Statistics
    uint64_t activationCount{0};
    uint32_t lastUpdateStep{0};
    float averageWeightChange{0.0f};
};

class BrainCheckpointSerializer {
public:
    BrainCheckpointSerializer();
    ~BrainCheckpointSerializer();
    
    // Disable copying
    BrainCheckpointSerializer(const BrainCheckpointSerializer&) = delete;
    BrainCheckpointSerializer& operator=(const BrainCheckpointSerializer&) = delete;
    
    /**
     * Serialize brain state to checkpoint format
     * @param brain Brain instance to serialize
     * @param checkpoint Reference to checkpoint metadata
     * @param compressionAlgorithm Compression algorithm to use
     * @param compressionLevel Compression level
     * @return Serialized data
     */
    std::vector<uint8_t> serialize(
        const Brain& brain,
        BrainCheckpoint& checkpoint,
        CompressionAlgorithm compressionAlgorithm = CompressionAlgorithm::Zstd,
        int compressionLevel = 3
    );
    
    /**
     * Serialize brain state to file
     * @param brain Brain instance to serialize
     * @param filepath Output file path
     * @param compressionAlgorithm Compression algorithm to use
     * @param compressionLevel Compression level
     * @return true if successful
     */
    bool serializeToFile(
        const Brain& brain,
        const std::string& filepath,
        CompressionAlgorithm compressionAlgorithm = CompressionAlgorithm::Zstd,
        int compressionLevel = 3
    );
    
    /**
     * Deserialize brain state from checkpoint data
     * @param brain Brain instance to populate
     * @param data Checkpoint data
     * @param checkpoint Checkpoint metadata
     * @return true if successful
     */
    bool deserialize(
        Brain& brain,
        const std::vector<uint8_t>& data,
        const BrainCheckpoint& checkpoint
    );
    
    /**
     * Deserialize brain state from file
     * @param brain Brain instance to populate
     * @param filepath Input file path
     * @return true if successful
     */
    bool deserializeFromFile(Brain& brain, const std::string& filepath);
    
    /**
     * Create incremental checkpoint (only changed data)
     * @param brain Current brain state
     * @param baseCheckpoint Base checkpoint
     * @param compressionAlgorithm Compression algorithm
     * @return Incremental checkpoint data
     */
    std::vector<uint8_t> createIncrementalCheckpoint(
        const Brain& brain,
        const BrainCheckpoint& baseCheckpoint,
        CompressionAlgorithm compressionAlgorithm = CompressionAlgorithm::Zstd
    );
    
    /**
     * Apply incremental checkpoint to brain
     * @param brain Brain instance to update
     * @param incrementalData Incremental checkpoint data
     * @param baseCheckpoint Base checkpoint metadata
     * @return true if successful
     */
    bool applyIncrementalCheckpoint(
        Brain& brain,
        const std::vector<uint8_t>& incrementalData,
        const BrainCheckpoint& baseCheckpoint
    );
    
    /**
     * Create differential checkpoint (differences from previous)
     * @param brain Current brain state
     * @param previousCheckpoint Previous checkpoint
     * @param compressionAlgorithm Compression algorithm
     * @return Differential checkpoint data
     */
    std::vector<uint8_t> createDifferentialCheckpoint(
        const Brain& brain,
        const BrainCheckpoint& previousCheckpoint,
        CompressionAlgorithm compressionAlgorithm = CompressionAlgorithm::Zstd
    );
    
    /**
     * Apply differential checkpoint to brain
     * @param brain Brain instance to update
     * @param differentialData Differential checkpoint data
     * @param previousCheckpoint Previous checkpoint metadata
     * @return true if successful
     */
    bool applyDifferentialCheckpoint(
        Brain& brain,
        const std::vector<uint8_t>& differentialData,
        const BrainCheckpoint& previousCheckpoint
    );
    
    /**
     * Validate checkpoint integrity
     * @param data Checkpoint data
     * @param checkpoint Checkpoint metadata
     * @return true if valid
     */
    bool validate(const std::vector<uint8_t>& data, const BrainCheckpoint& checkpoint) const;
    
    /**
     * Get checkpoint version compatibility
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
        uint64_t neuronsSerialized{0};
        uint64_t synapsesSerialized{0};
        uint64_t regionsSerialized{0};
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
    static std::string getVersion() { return "BrainCheckpointSerializer v1.0"; }
    
private:
    // Core serialization functions
    std::vector<uint8_t> serializeNeurons(const Brain& brain);
    std::vector<uint8_t> serializeSynapses(const Brain& brain);
    std::vector<uint8_t> serializeRegions(const Brain& brain);
    std::vector<uint8_t> serializePlasticityRules(const Brain& brain);
    std::vector<uint8_t> serializeNeuromodulators(const Brain& brain);
    std::vector<uint8_t> serializeMemorySystems(const Brain& brain);
    std::vector<uint8_t> serializePredictionSystems(const Brain& brain);
    std::vector<uint8_t> serializeCognitionSystems(const Brain& brain);
    std::vector<uint8_t> serializeDevelopment(const Brain& brain);
    std::vector<uint8_t> serializeInterRegionConnections(const Brain& brain);
    
    // Deserialization functions
    bool deserializeNeurons(Brain& brain, const std::vector<uint8_t>& data);
    bool deserializeSynapses(Brain& brain, const std::vector<uint8_t>& data);
    bool deserializeRegions(Brain& brain, const std::vector<uint8_t>& data);
    bool deserializePlasticityRules(Brain& brain, const std::vector<uint8_t>& data);
    bool deserializeNeuromodulators(Brain& brain, const std::vector<uint8_t>& data);
    bool deserializeMemorySystems(Brain& brain, const std::vector<uint8_t>& data);
    bool deserializePredictionSystems(Brain& brain, const std::vector<uint8_t>& data);
    bool deserializeCognitionSystems(Brain& brain, const std::vector<uint8_t>& data);
    bool deserializeDevelopment(Brain& brain, const std::vector<uint8_t>& data);
    bool deserializeInterRegionConnections(Brain& brain, const std::vector<uint8_t>& data);
    
    // Incremental/differential serialization
    std::vector<uint8_t> serializeIncrementalChanges(
        const Brain& brain,
        const BrainCheckpoint& baseCheckpoint,
        const std::vector<uint8_t>& baseData
    );
    
    std::vector<uint8_t> serializeDifferentialChanges(
        const Brain& brain,
        const BrainCheckpoint& previousCheckpoint,
        const std::vector<uint8_t>& previousData
    );
    
    // Serialization utilities
    std::vector<uint8_t> serializeToBinary(const std::vector<NeuronState>& neurons) const;
    std::vector<uint8_t> serializeToBinary(const std::vector<SynapseState>& synapses) const;
    std::vector<uint8_t> serializeToBinary(const std::vector<RegionCheckpointData>& regions) const;
    std::vector<uint8_t> serializeToBinary(const std::vector<PlasticityRuleState>& rules) const;
    std::vector<uint8_t> serializeToBinary(const std::vector<NeuromodulatorState>& neuromodulators) const;
    
    bool deserializeFromBinary(std::vector<NeuronState>& neurons, const std::vector<uint8_t>& data) const;
    bool deserializeFromBinary(std::vector<SynapseState>& synapses, const std::vector<uint8_t>& data) const;
    bool deserializeFromBinary(std::vector<RegionCheckpointData>& regions, const std::vector<uint8_t>& data) const;
    bool deserializeFromBinary(std::vector<PlasticityRuleState>& rules, const std::vector<uint8_t>& data) const;
    bool deserializeFromBinary(std::vector<NeuromodulatorState>& neuromodulators, const std::vector<uint8_t>& data) const;
    
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
};

// Helper function to extract NeuronCheckpointData from Brain (for compatibility)
NeuronCheckpointData extractNeuronCheckpointData(const Brain& brain);

// Helper function to extract SynapseCheckpointData from Brain (for compatibility)
SynapseCheckpointData extractSynapseCheckpointData(const Brain& brain);

// Forward declaration for RegionCheckpointData
struct RegionCheckpointData {
    RegionId id;
    std::string name;
    std::vector<uint64_t> neuronIds;
    std::vector<uint64_t> synapseIds;
    float averageWeight;
    float totalActivity;
    uint32_t lastUpdateStep;
};