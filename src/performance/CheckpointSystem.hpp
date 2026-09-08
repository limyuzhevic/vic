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
 * Working memory state for checkpointing
 */
struct WorkingMemoryCheckpointData {
    std::vector<uint64_t> memoryNeurons;
    std::vector<float> memoryActivations;
    std::vector<SimulationStep> memoryTimestamps;
    std::vector<size_t> activeTraces;
    std::vector<std::pair<uint64_t, uint64_t>> recurrentConnections;
    std::vector<uint64_t> winners;
    float decayRate;
    size_t capacity;
};

/**
 * Episodic memory state for checkpointing
 */
struct EpisodicMemoryCheckpointData {
    std::deque<EpisodicMemoryItem> episodes;
    std::vector<NeuronId> episodeNeurons;
    bool replayEnabled;
    size_t maxEpisodes;
};

/**
 * Associative memory state for checkpointing
 */
struct AssociativeMemoryCheckpointData {
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons;
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    float noveltyThreshold;
};

/**
 * Prediction system state for checkpointing
 */
struct PredictionSystemCheckpointData {
    float predictionError;
    float confidence;
    std::vector<float> errorHistory;
    float lastPrediction;
    float lastActual;
};

/**
 * Neural planner state for checkpointing
 */
struct NeuralPlannerCheckpointData {
    std::vector<ActionType> actionHistory;
    std::vector<float> rewardHistory;
    std::vector<float> uncertainty;
    size_t planningDepth;
    float explorationRate;
    float exploitationRate;
};

/**
 * Concept formation state for checkpointing
 */
struct ConceptFormationCheckpointData {
    std::vector<DiscoveredConcept> concepts;
    std::vector<ConceptInstance> instances;
    float noveltyThreshold;
    float stabilityThreshold;
};

/**
 * Attentional selection state for checkpointing
 */
struct AttentionalSelectionCheckpointData {
    std::vector<RegionId> attendedRegions;
    std::vector<NeuronId> winners;
    std::vector<float> neuronSalience;
    std::unordered_map<uint64_t, float> topDownBias;
    std::unordered_map<uint64_t, float> bottomUpSalience;
    float inhibitionStrength;
    float excitationStrength;
    float competitionThreshold;
};

/**
 * Development system state for checkpointing
 */
struct DevelopmentSystemCheckpointData {
    DevelopmentalStage developmentalStage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;
    double systemAge;
    float plasticityModifier;
    bool isCriticalPeriod;
};

/**
 * Neuromodulator state for checkpointing
 */
struct NeuromodulatorCheckpointData {
    float level;
    float plasticityFactor;
    float decayRate;
    std::vector<float> history;
    float lastUpdate;
};

/**
 * Dopamine state for checkpointing
 */
struct DopamineCheckpointData {
    float level;
    float plasticityFactor;
    float decayRate;
    float predictionError;
    std::vector<float> rewardHistory;
    float lastRewardTime;
};

/**
 * Curiosity state for checkpointing
 */
struct CuriosityCheckpointData {
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    std::vector<float> noveltyHistory;
    std::vector<float> predictionErrorHistory;
    float lastUpdate;
};

/**
 * Novelty state for checkpointing
 */
struct NoveltyCheckpointData {
    float level;
    float decayRate;
    float noveltyThreshold;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float lastUpdate;
};

/**
 * Prediction error state for checkpointing
 */
struct PredictionErrorCheckpointData {
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    float magnitude;
    float lastComputeTime;
};

/**
 * Plasticity system state for checkpointing
 */
struct PlasticitySystemCheckpointData {
    float stdpLtpWeight;
    float stdpLtdWeight;
    float stdpTimeConstant;
    float hebbianLearningRate;
    float structuralSynaptogenesisRate;
    float structuralPruningRate;
    float eligibilityTraceDecay;
};

/**
 * Development state for checkpointing
 */
struct DevelopmentStateCheckpointData {
    DevelopmentalStage developmentalStage;
    SimulationStep lastStageAdvance;
    double stageAge;
    double systemAge;
    float plasticityModifier;
    bool isCriticalPeriod;
    SimulationStep criticalPeriodProgress;
};

/**
 * Random generator state for checkpointing
 */
struct RandomGeneratorCheckpointData {
    uint64_t seed;
    uint64_t currentSeed;
    uint64_t currentState;  // For MT19937
};

/**
 * Simulation state for checkpointing
 */
struct SimulationStateCheckpointData {
    SimulationStep currentStep;
    Timestamp currentTime;
    TimestepDuration timestep;
    size_t totalSpikesThisStep;
    size_t totalSpikesTotal;
    Timestamp simulationStartTime;
    double simulationTime;
    bool isResting;
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
};

/**
 * Inter-region connections for checkpointing
 */
struct InterRegionConnectionsCheckpointData {
    std::vector<InterRegionConnection> connections;
    size_t connectionCount;
};

/**
 * Spike system state for checkpointing
 */
struct SpikeSystemCheckpointData {
    std::vector<SpikeEvent> pendingSpikes;
    std::vector<DelayedSpikeEvent> pendingDelayedSpikes;
    size_t pendingSpikeCount;
    size_t pendingDelayedCount;
    Timestamp lastSpikeTime;
    SimulationStep lastSpikeStep;
};

/**
 * All brain state data for checkpointing
 */
struct BrainCheckpointData {
    // Neuron and synapse data
    NeuronCheckpointData neurons;
    SynapseCheckpointData synapses;
    
    // Integration state
    InterRegionConnectionsCheckpointData interRegionConnections;
    SpikeSystemCheckpointData spikeSystem;
    
    // Memory systems
    WorkingMemoryCheckpointData workingMemory;
    EpisodicMemoryCheckpointData episodicMemory;
    AssociativeMemoryCheckpointData associativeMemory;
    
    // Cognition systems
    PredictionSystemCheckpointData predictionSystem;
    NeuralPlannerCheckpointData neuralPlanner;
    ConceptFormationCheckpointData conceptFormation;
    AttentionalSelectionCheckpointData attentionalSelection;
    
    // Development system
    DevelopmentSystemCheckpointData developmentSystem;
    
    // Neuromodulation systems
    DopamineCheckpointData dopamine;
    CuriosityCheckpointData curiosity;
    NoveltyCheckpointData novelty;
    PredictionErrorCheckpointData predictionError;
    
    // Plasticity systems
    PlasticitySystemCheckpointData plasticitySystem;
    
    // Random generator
    RandomGeneratorCheckpointData randomGenerator;
    
    // Simulation state
    SimulationStateCheckpointData simulationState;
    
    // Statistics
    size_t totalNeuronCount;
    size_t totalSynapseCount;
    size_t regionCount;
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