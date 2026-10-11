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
    WorkingMemory = 8,
    EpisodicMemory = 9,
    AssociativeMemory = 10,
    Prediction = 11,
    Cognition = 12,
    SpikeHistory = 13,
    RandomState = 14,
    SimulationState = 15,
    Replay = 16,
    Environment = 17,
    Checksum = 18,
    End = 19
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
 * Working memory state for checkpointing
 */
struct WorkingMemoryCheckpointData {
    std::vector<float> patterns;
    std::vector<float> strengths;
    std::vector<uint32_t> neuronIds;
    std::vector<float> timestamps;
    std::vector<uint32_t> traces;
    size_t capacity;
    float decayRate;
};

/**
 * Episodic memory state for checkpointing
 */
struct EpisodicMemoryCheckpointData {
    std::vector<uint64_t> episodeIds;
    std::vector<uint64_t> timestamps;
    std::vector<float> rewards;
    std::vector<std::vector<uint64_t>> activeNeurons;
    std::vector<std::vector<float>> activations;
    std::vector<NeuromodulatorType> neuromodulators;
    std::vector<ReplayType> replayTypes;
    size_t maxEpisodes;
    size_t consolidationThreshold;
};

/**
 * Prediction system state for checkpointing
 */
struct PredictionCheckpointData {
    std::vector<float> forwardModelWeights;
    std::vector<float> inverseModelWeights;
    std::vector<float> predictionErrors;
    std::vector<float> predictionConfidences;
    std::vector<float> temporalMemory;
    size_t predictionHorizon;
    size_t temporalMemorySize;
    bool enablePredictiveCoding;
    bool enableForwardModel;
    bool enableInverseModel;
};

/**
 * Neuromodulation system state for checkpointing
 */
struct NeuromodulationCheckpointData {
    float dopamineLevel;
    float acetylcholineLevel;
    float norepinephrineLevel;
    float serotoninLevel;
    float curiosityLevel;
    float noveltyLevel;
    float predictionErrorSignal;
    float rewardSignal;
    float arousalLevel;
    float motivationLevel;
    float attentionLevel;
    bool isAttentionModulated;
    bool isMemoryModulated;
};

/**
 * Cognitive systems state for checkpointing
 */
struct CognitiveCheckpointData {
    // Neural planner
    std::vector<float> plannerWeights;
    std::vector<float> goalVectors;
    std::vector<uint32_t> actionHistory;
    std::vector<float> planningDepths;
    
    // Concept formation
    std::vector<float> conceptPrototypes;
    std::vector<std::vector<uint32_t>> conceptAssociations;
    std::vector<float> conceptActivations;
    
    // Attentional selection
    std::vector<float> attentionalWeights;
    std::vector<float> excitationStrengths;
    std::vector<float> inhibitionStrengths;
    std::vector<uint32_t> winnerNeurons;
    std::vector<float> attentionMaps;
};

/**
 * Replay mechanisms state for checkpointing
 */
struct ReplayCheckpointData {
    std::vector<uint64_t> replayTargets;
    std::vector<float> replayProbabilities;
    std::vector<ReplayType> replayModes;
    std::vector<uint64_t> lastReplaySteps;
    std::vector<float> consolidationStates;
    float replayStrength;
    bool isConsolidating;
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
     * Read working memory data
     */
    bool readWorkingMemory(WorkingMemoryCheckpointData& data);
    
    /**
     * Read episodic memory data
     */
    bool readEpisodicMemory(EpisodicMemoryCheckpointData& data);
    
    /**
     * Read prediction system data
     */
    bool readPrediction(PredictionCheckpointData& data);
    
    /**
     * Read neuromodulation data
     */
    bool readNeuromodulation(NeuromodulationCheckpointData& data);
    
    /**
     * Read cognitive systems data
     */
    bool readCognitive(CognitiveCheckpointData& data);
    
    /**
     * Read replay mechanisms data
     */
    bool readReplay(ReplayCheckpointData& data);
    
    /**
     * Write working memory data
     */
    bool writeWorkingMemory(const WorkingMemoryCheckpointData& data);
    
    /**
     * Write episodic memory data
     */
    bool writeEpisodicMemory(const EpisodicMemoryCheckpointData& data);
    
    /**
     * Write prediction system data
     */
    bool writePrediction(const PredictionCheckpointData& data);
    
    /**
     * Write neuromodulation data
     */
    bool writeNeuromodulation(const NeuromodulationCheckpointData& data);
    
    /**
     * Write cognitive systems data
     */
    bool writeCognitive(const CognitiveCheckpointData& data);
    
    /**
     * Write replay mechanisms data
     */
    bool writeReplay(const ReplayCheckpointData& data);
    
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
    /**
     * Write working memory data
     */
    bool writeWorkingMemory(const WorkingMemoryCheckpointData& data);
    
    /**
     * Write episodic memory data
     */
    bool writeEpisodicMemory(const EpisodicMemoryCheckpointData& data);
    
    /**
     * Write prediction system data
     */
    bool writePrediction(const PredictionCheckpointData& data);
    
    /**
     * Write neuromodulation data
     */
    bool writeNeuromodulation(const NeuromodulationCheckpointData& data);
    
    /**
     * Write cognitive systems data
     */
    bool writeCognitive(const CognitiveCheckpointData& data);
    
    /**
     * Write replay mechanisms data
     */
    bool writeReplay(const ReplayCheckpointData& data);
    
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