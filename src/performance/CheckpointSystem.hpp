#pragma once

/**
 * CheckpointSystem - Brain state serialization and checkpointing
 * 
 * Provides robust checkpointing for large-scale neural simulations.
 * Supports incremental saves, version compatibility, and validation.
 * 
 * Features:
 * - Complete brain state serialization
 * - Incremental checkpointing
 * - Version compatibility checks
 * - Integrity validation
 * - Streaming for large checkpoints
 * 
 * Stored state:
 * - Neuron states (membrane potential, firing state, etc.)
 * - Synapse states (weights, plasticity, etc.)
 * - Spike history (recent)
 * - Plasticity state (eligibility traces, etc.)
 * - Developmental state
 * - Random generator state
 * - Simulation clock
 * - Environment state (optional)
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <fstream>
#include <functional>
#include <variant>

namespace nlm {

// Checkpoint version for compatibility
constexpr uint32_t CHECKPOINT_VERSION_MAJOR = 1;
constexpr uint32_t CHECKPOINT_VERSION_MINOR = 0;
constexpr uint32_t CHECKPOINT_VERSION_PATCH = 0;

// Checkpoint magic number for file validation
constexpr uint64_t CHECKPOINT_MAGIC = 0x4E4C4D434B545430ULL;  // "NLMCKT0"

// Compression options
enum class CompressionLevel {
    None,
    Fast,
    Balanced,
    Best
};

// Checkpoint section types
enum class CheckpointSection : uint32_t {
    Header = 0,
    Config = 1,
    Neurons = 2,
    Synapses = 3,
    Connectivity = 4,
    Plasticity = 5,
    Development = 6,
    Neuromodulation = 7,
    Memory = 8,
    SpikeHistory = 9,
    RandomState = 10,
    SimulationState = 11,
    Environment = 12,
    Checksum = 13,
    End = 14
};

/**
 * Checkpoint header
 */
struct CheckpointHeader {
    uint64_t magic;                    // File magic number
    uint32_t majorVersion;            // Major version
    uint32_t minorVersion;            // Minor version
    uint32_t patchVersion;            // Patch version
    uint64_t timestamp;               // Creation timestamp
    uint64_t totalSize;               // Total checkpoint size
    uint64_t neuronCount;             // Number of neurons
    uint64_t synapseCount;            // Number of synapses
    uint64_t regionCount;             // Number of regions
    uint64_t step;                   // Simulation step
    double simulationTime;            // Simulation time
    uint64_t checksum;               // Checksum of data
    uint32_t compressionLevel;       // Compression level
    uint32_t flags;                  // Reserved flags
    
    CheckpointHeader()
        : magic(CHECKPOINT_MAGIC)
        , majorVersion(CHECKPOINT_VERSION_MAJOR)
        , minorVersion(CHECKPOINT_VERSION_MINOR)
        , patchVersion(CHECKPOINT_VERSION_PATCH)
        , timestamp(0)
        , totalSize(0)
        , neuronCount(0)
        , synapseCount(0)
        , regionCount(0)
        , step(0)
        , simulationTime(0.0)
        , checksum(0)
        , compressionLevel(0)
        , flags(0) {}
    
    bool validate() const {
        return magic == CHECKPOINT_MAGIC &&
               majorVersion == CHECKPOINT_VERSION_MAJOR;
    }
};

/**
 * Checkpoint section header
 */
struct SectionHeader {
    uint32_t type;           // CheckpointSection enum
    uint64_t offset;         // Offset from start of file
    uint64_t size;           // Size of section data
    uint64_t compressedSize; // Size after compression (0 if not compressed)
    uint32_t checksum;       // Section checksum
    
    SectionHeader() : type(0), offset(0), size(0), compressedSize(0), checksum(0) {}
};

/**
 * Checksum calculator
 */
class ChecksumCalculator {
public:
    using uint128_t = unsigned __int128;
    
    static uint64_t crc64(const void* data, size_t size);
    static uint32_t crc32(const void* data, size_t size);
    static uint64_t fletcher64(const uint64_t* data, size_t count);
    
    // Combine checksums
    static uint64_t combine(uint64_t a, uint64_t b);
};

/**
 * Neuron state for checkpointing
 */
struct NeuronCheckpointData {
    std::vector<float> membranePotential;
    std::vector<float> restingPotential;
    std::vector<float> threshold;
    std::vector<float> resetPotential;
    std::vector<float> leakConductance;
    std::vector<uint8_t> firingState;
    std::vector<uint32_t> refractoryRemaining;
    std::vector<uint32_t> refractoryPeriod;
    std::vector<float> lastSpikeTime;
    std::vector<uint64_t> neuronType;
    std::vector<uint64_t> regionId;
    std::vector<uint64_t> populationId;
};

/**
 * Synapse state for checkpointing
 */
struct SynapseCheckpointData {
    std::vector<uint64_t> sourceNeuron;
    std::vector<uint64_t> destinationNeuron;
    std::vector<float> weight;
    std::vector<uint32_t> delay;
    std::vector<uint8_t> synapseType;
    std::vector<uint8_t> plasticityFlags;
    std::vector<float> eligibilityTrace;
    std::vector<float> efficacy;
    std::vector<float> shortTermDepression;
    std::vector<float> shortTermFacilitation;
};

/**
 * Memory system state for checkpointing
 */
struct MemoryCheckpointData {
    // Working memory: active neural traces
    std::vector<uint64_t> workingMemoryNeurons;
    std::vector<float> workingMemoryActivations;
    std::vector<double> workingMemoryTimestamps;
    std::vector<size_t> workingMemoryTraceIds;
    
    // Episodic memory: stored experiences
    std::vector<uint64_t> episodicMemoryIds;
    std::vector<double> episodicMemoryTimestamps;
    std::vector<float> episodicMemoryRewards;
    std::vector<float> episodicMemoryEnergies;
    std::vector<std::vector<uint64_t>> episodicMemoryNeurons;
    std::vector<std::vector<float>> episodicMemoryActivations;
    std::vector<float> episodicMemoryPositionX;
    std::vector<float> episodicMemoryPositionY;
    std::vector<float> episodicMemoryOrientation;
    std::vector<uint32_t> episodicMemoryActions;
    std::vector<std::vector<float>> episodicMemorySensoryStates;
    std::vector<std::vector<float>> episodicMemoryResultingStates;
    std::vector<std::vector<float>> episodicMemoryResultingRewards;
    std::vector<size_t> episodicMemoryAges;
    
    // Associative memory: pattern relationships
    std::vector<std::vector<uint64_t>> associativePatternNeurons;
    std::vector<std::vector<float>> associativePatternActivations;
    std::vector<std::vector<std::pair<size_t, size_t>>> associativeConnections;
    std::vector<std::vector<float>> associativeStrengths;
};

/**
 * Neuromodulation system state for checkpointing
 */
struct NeuromodulationCheckpointData {
    // Dopamine
    float dopamineLevel;
    float dopamineBaselineLevel;
    float dopamineReceptorActivation;
    float dopamineExcitabilityModulation;
    float dopamineWorkingMemoryModulation;
    float dopamineEligibilityTraceModulation;
    float dopamineAttentionModulation;
    float dopamineLearningRateModulation;
    float dopaminePlasticityFactor;
    
    // Acetylcholine
    float acetylcholineLevel;
    float acetylcholineBaselineLevel;
    float acetylcholineReceptorActivation;
    float acetylcholineExcitabilityModulation;
    float acetylcholineWorkingMemoryModulation;
    float acetylcholineEligibilityTraceModulation;
    float acetylcholineAttentionModulation;
    float acetylcholineLearningRateModulation;
    float acetylcholinePlasticityFactor;
    
    // Norepinephrine
    float norepinephrineLevel;
    float norepinephrineBaselineLevel;
    float norepinephrineReceptorActivation;
    float norepinephrineExcitabilityModulation;
    float norepinephrineWorkingMemoryModulation;
    float norepinephrineEligibilityTraceModulation;
    float norepinephrineAttentionModulation;
    float norepinephrineLearningRateModulation;
    float norepinephrinePlasticityFactor;
    
    // Serotonin
    float serotoninLevel;
    float serotoninBaselineLevel;
    float serotoninReceptorActivation;
    float serotoninExcitabilityModulation;
    float serotoninWorkingMemoryModulation;
    float serotoninEligibilityTraceModulation;
    float serotoninAttentionModulation;
    float serotoninLearningRateModulation;
    float serotoninPlasticityFactor;
    
    // Curiosity
    float curiosityLevel;
    float curiosityBaselineLevel;
    float curiosityReceptorActivation;
    float curiosityExcitabilityModulation;
    float curiosityWorkingMemoryModulation;
    float curiosityEligibilityTraceModulation;
    float curiosityAttentionModulation;
    float curiosityLearningRateModulation;
    float curiosityPlasticityFactor;
    
    // Novelty
    float noveltyLevel;
    float noveltyBaselineLevel;
    float noveltyReceptorActivation;
    float noveltyExcitabilityModulation;
    float noveltyWorkingMemoryModulation;
    float noveltyEligibilityTraceModulation;
    float noveltyAttentionModulation;
    float noveltyLearningRateModulation;
    float noveltyPlasticityFactor;
    
    // PredictionError
    float predictionErrorLevel;
    float predictionErrorBaselineLevel;
    float predictionErrorReceptorActivation;
    float predictionErrorExcitabilityModulation;
    float predictionErrorWorkingMemoryModulation;
    float predictionErrorEligibilityTraceModulation;
    float predictionErrorAttentionModulation;
    float predictionErrorLearningRateModulation;
    float predictionErrorPlasticityFactor;
};

/**
 * Development system state for checkpointing
 */
struct DevelopmentCheckpointData {
    // Developmental stage
    uint32_t developmentalStage;
    
    // Developmental age
    double developmentalAge;
    
    // Plasticity modifier
    float plasticityModifier;
    
    // Critical period info
    bool criticalPeriod;
    float criticalPeriodProgress;
    
    // Stage-specific parameters
    float synaptogenesisRate;
    float pruningRate;
    float maturationRate;
};

/**
 * Prediction system state for checkpointing
 */
struct PredictionCheckpointData {
    // Prediction system metrics
    std::vector<float> predictionAccuracy;
    std::vector<float> predictionConfidence;
    std::vector<float> predictionErrors;
    std::vector<float> predictionExpectations;
    
    // Forward model state
    std::vector<std::vector<float>> forwardModelWeights;
    std::vector<std::vector<float>> forwardModelBiases;
    std::vector<float> predictionErrorHistory;
    std::vector<float> predictionLevel;
    
    // Action prediction
    std::vector<float> predictedRewards;
    std::vector<float> predictedValues;
    std::vector<std::vector<float>> actionPredictions;
};

/**
 * Attention system state for checkpointing
 */
struct AttentionCheckpointData {
    // Attention parameters
    float inhibitionStrength;
    float excitationStrength;
    float competitionThreshold;
    
    // Attention state
    std::vector<uint32_t> attendedRegions;
    std::vector<uint64_t> attentionWinners;
    std::vector<float> neuronSalience;
    std::vector<float> topDownBias;
    std::vector<float> bottomUpSalience;
    
    // Attention dynamics
    std::vector<double> attentionTimestamps;
    std::vector<float> attentionLevels;
};

/**
 * Spike history for checkpointing
 */
struct SpikeHistoryCheckpointData {
    // Recent spike events
    std::vector<uint64_t> spikeSourceNeurons;
    std::vector<double> spikeTimestamps;
    std::vector<uint64_t> spikeSteps;
    std::vector<uint32_t> spikeRegionIds;
    std::vector<uint32_t> spikePopulationIds;
    
    // Delayed spike events
    std::vector<uint64_t> delayedSourceNeurons;
    std::vector<uint64_t> delayedDestinationNeurons;
    std::vector<uint64_t> delayedSynapseIds;
    std::vector<float> delayedWeights;
    std::vector<uint32_t> delayedDeliverySteps;
    std::vector<double> delayedDeliveryTimes;
    
    // Pre and post spike history for plasticity
    std::vector<std::vector<double>> preSpikeHistory;
    std::vector<std::vector<double>> postSpikeHistory;
};

/**
 * Random generator state for checkpointing
 */
struct RandomStateCheckpointData {
    // Random engine state
    uint64_t seed;
    
    // Underlying engine state (MT19937 state)
    std::vector<uint32_t> mtState;
    int32_t mtPosition;
    
    // Statistical properties for validation
    std::vector<double> recentValues;
    std::vector<double> statisticalMoments;
};

/**
 * Simulation state for checkpointing
 */
struct SimulationStateCheckpointData {
    // Simulation clock
    uint64_t step;
    double simulationTime;
    double timestep;
    
    // Statistics
    uint64_t totalSpikesThisStep;
    uint64_t totalSpikesTotal;
    
    // State flags
    bool isResting;
    size_t stepsSinceLastEpisode;
    
    // Integration intervals
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Environment simulation state
    std::vector<float> environmentState;
    std::vector<float> worldParameters;
};

/**
     * Checkpoint reader
     */
class CheckpointReader {
public:
    CheckpointReader();
    ~CheckpointReader();
    
    // Disable copying
    CheckpointReader(const CheckpointReader&) = delete;
    CheckpointReader& operator=(const CheckpointReader&) = delete;
    
    /**
     * Open a checkpoint file for reading
     * @return true if successful
     */
    bool open(const std::string& filepath);
    
    /**
     * Close the checkpoint file
     */
    void close();
    
    /**
     * Check if checkpoint is open
     */
    bool isOpen() const { return stream_.is_open(); }
    
    /**
     * Get header
     */
    const CheckpointHeader& getHeader() const { return header_; }
    
    /**
     * Read checkpoint data
     */
    bool readNeurons(NeuronCheckpointData& data);
    bool readSynapses(SynapseCheckpointData& data);
    bool readMemory(MemoryCheckpointData& data);
    bool readNeuromodulation(NeuromodulationCheckpointData& data);
    bool readDevelopment(DevelopmentCheckpointData& data);
    bool readPrediction(PredictionCheckpointData& data);
    bool readAttention(AttentionCheckpointData& data);
    bool readSpikeHistory(SpikeHistoryCheckpointData& data);
    bool readRandomState(RandomStateCheckpointData& data);
    bool readSimulationState(SimulationStateCheckpointData& data);
    
    /**
     * Read raw section data
     */
    std::vector<uint8_t> readSection(CheckpointSection sectionType);
    
    /**
     * Validate checkpoint integrity
     */
    bool validate() const;
    
    /**
     * Get validation error message
     */
    const std::string& getError() const { return error_; }
    
    /**
     * Get reader progress (0.0 - 1.0)
     */
    double getProgress() const;

private:
    std::ifstream stream_;
    CheckpointHeader header_;
    std::vector<SectionHeader> sections_;
    std::string error_;
    size_t streamPos_;
};

/**
 * Checkpoint writer
 */
class CheckpointWriter {
public:
    CheckpointWriter();
    ~CheckpointWriter();
    
    // Disable copying
    CheckpointWriter(const CheckpointWriter&) = delete;
    CheckpointWriter& operator=(const CheckpointWriter&) = delete;
    
    /**
     * Create a new checkpoint file
     * @param filepath Output file path
     * @param compression Compression level
     * @return true if successful
     */
    bool create(const std::string& filepath, CompressionLevel compression = CompressionLevel::Balanced);
    
    /**
     * Close the checkpoint file
     */
    void close();
    
    /**
     * Check if checkpoint is open
     */
    bool isOpen() const { return stream_.is_open(); }
    
    /**
     * Set checkpoint metadata
     */
    void setMetadata(uint64_t neuronCount, uint64_t synapseCount, 
                    uint64_t regionCount, uint64_t step, double simTime);
    
    /**
     * Write checkpoint data
     */
    bool writeNeurons(const NeuronCheckpointData& data);
    bool writeSynapses(const SynapseCheckpointData& data);
    bool writeMemory(const MemoryCheckpointData& data);
    bool writeNeuromodulation(const NeuromodulationCheckpointData& data);
    bool writeDevelopment(const DevelopmentCheckpointData& data);
    bool writePrediction(const PredictionCheckpointData& data);
    bool writeAttention(const AttentionCheckpointData& data);
    bool writeSpikeHistory(const SpikeHistoryCheckpointData& data);
    bool writeRandomState(const RandomStateCheckpointData& data);
    bool writeSimulationState(const SimulationStateCheckpointData& data);
    
    /**
     * Write raw section data
     */
    bool writeSection(CheckpointSection sectionType, const void* data, size_t size);
    
    /**
     * Finalize and write footer
     * @return true if successful
     */
    bool finalize();
    
    /**
     * Abort checkpoint (cleanup partial file)
     */
    void abort();
    
    /**
     * Get total bytes written
     */
    uint64_t getBytesWritten() const { return bytesWritten_; }

private:
    bool writeHeader();
    bool writeSectionHeader(const SectionHeader& header);
    bool padToAlignment(size_t alignment);
    
    std::ofstream stream_;
    std::string filepath_;
    CompressionLevel compression_;
    CheckpointHeader header_;
    std::vector<SectionHeader> sections_;
    uint64_t bytesWritten_;
    std::vector<uint8_t> compressBuffer_;
};

/**
 * Checkpoint manager for automated checkpointing
 */
class CheckpointManager {
public:
    CheckpointManager();
    ~CheckpointManager();
    
    /**
     * Configure checkpoint manager
     */
    void configure(const std::string& checkpointDir, 
                   uint64_t saveIntervalSteps = 10000,
                   size_t maxCheckpoints = 10,
                   bool compress = true);
    
    /**
     * Set checkpoint data providers
     */
    void setNeuronProvider(std::function<bool(NeuronCheckpointData&)> provider) {
        neuronProvider_ = provider;
    }
    
    void setSynapseProvider(std::function<bool(SynapseCheckpointData&)> provider) {
        synapseProvider_ = provider;
    }
    
    /**
     * Update checkpoint state (call each simulation step)
     */
    bool update(uint64_t currentStep, double currentTime);
    
    /**
     * Save checkpoint immediately
     */
    bool saveImmediately(const std::string& name = "");
    
    /**
     * Load checkpoint
     */
    bool load(const std::string& name);
    
    /**
     * List available checkpoints
     */
    struct CheckpointInfo {
        std::string name;
        uint64_t step;
        double simulationTime;
        uint64_t neuronCount;
        uint64_t synapseCount;
        uint64_t fileSize;
        time_t timestamp;
    };
    
    std::vector<CheckpointInfo> listCheckpoints() const;
    
    /**
     * Delete old checkpoints (keep most recent N)
     */
    void pruneOldCheckpoints(size_t keepCount);
    
    /**
     * Get checkpoint directory
     */
    const std::string& getCheckpointDir() const { return checkpointDir_; }
    
    /**
     * Get last checkpoint path
     */
    std::string getLastCheckpointPath() const;
    
    /**
     * Check if checkpoint should be saved
     */
    bool shouldSave(uint64_t currentStep) const;
    
private:
    std::string checkpointDir_;
    uint64_t saveIntervalSteps_;
    size_t maxCheckpoints_;
    bool compress_;
    uint64_t lastSaveStep_;
    std::string lastCheckpointPath_;
    
    std::function<bool(NeuronCheckpointData&)> neuronProvider_;
    std::function<bool(SynapseCheckpointData&)> synapseProvider_;
};

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

} // namespace nlm