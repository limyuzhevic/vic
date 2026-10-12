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
struct WorkingMemoryCheckpointData {
    uint64_t neuronCount;
    uint64_t traceCount;
    
    WorkingMemoryCheckpointData() : neuronCount(0), traceCount(0) {}
};

struct EpisodicMemoryCheckpointData {
    uint64_t episodeCount;
    uint64_t maxEpisodes;
    
    EpisodicMemoryCheckpointData() : episodeCount(0), maxEpisodes(0) {}
};

struct AssociativeMemoryCheckpointData {
    uint64_t associationCount;
    
    AssociativeMemoryCheckpointData() : associationCount(0) {}
};

struct PredictionSystemCheckpointData {
    uint64_t learnedPredictions;
    uint64_t predictionErrorCount;
    
    PredictionSystemCheckpointData() : learnedPredictions(0), predictionErrorCount(0) {}
};

struct NeuralPlannerCheckpointData {
    size_t planningDepth;
    float confidence;
    
    NeuralPlannerCheckpointData() : planningDepth(0), confidence(0.0f) {}
};

struct ConceptFormationCheckpointData {
    uint64_t conceptCount;
    uint64_t activationCount;
    
    ConceptFormationCheckpointData() : conceptCount(0), activationCount(0) {}
};

struct AttentionCheckpointData {
    float inhibitionStrength;
    float excitationStrength;
    
    AttentionCheckpointData() : inhibitionStrength(0.0f), excitationStrength(0.0f) {}
};

struct NeuromodulatorCheckpointData {
    float level;
    float plasticityFactor;
    
    NeuromodulatorCheckpointData() : level(0.0f), plasticityFactor(1.0f) {}
};

struct DevelopmentSystemCheckpointData {
    DevelopmentalStage stage;
    float progress;
    
    DevelopmentSystemCheckpointData() : stage(DevelopmentalStage::Initial), progress(0.0f) {}
};

struct RandomGeneratorCheckpointData {
    uint64_t seed;
    std::vector<uint64_t> state;
    
    RandomGeneratorCheckpointData() : seed(0) {}
};

struct SimulationCheckpointData {
    uint64_t step;
    double simulationTime;
    DevelopmentalStage developmentalStage;
    
    SimulationCheckpointData() : step(0), simulationTime(0.0), developmentalStage(DevelopmentalStage::Initial) {}
};

/**
 * Checkpoint writer methods for memory systems
 */
bool writeWorkingMemory(const WorkingMemoryCheckpointData& data);
bool writeEpisodicMemory(const EpisodicMemoryCheckpointData& data);
bool writeAssociativeMemory(const AssociativeMemoryCheckpointData& data);
bool writePredictionSystem(const PredictionSystemCheckpointData& data);
bool writePlanner(const NeuralPlannerCheckpointData& data);
b bool writeConceptFormation(const ConceptFormationCheckpointData& data);
b bool writeAttention(const AttentionCheckpointData& data);
bool writeNeuromodulator(const NeuromodulatorCheckpointData& data, const std::string& type);
bool writeDevelopmentSystem(const DevelopmentSystemCheckpointData& data);
bool writeRandomState(const RandomGeneratorCheckpointData& data);
bool writeSimulationState(const SimulationCheckpointData& data);

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