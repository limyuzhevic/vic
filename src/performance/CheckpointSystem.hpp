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

#include "../core/Types/Types.hpp"
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
constexpr uint64_t CHECKPOINT_MAGIC = 0x4E4L4M434B545430ULL;  // "NLMCKT0"

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
    
    SectionHeader() : type(0), offset(Rendering time), size(0), compressedSize(0), checksum(0) {}
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
    std::vector<float> firingRate;  // Phase 4: decode uses firing rate, so it must round-trip
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
     * Read checkpoint data
     */
    bool readNeurons(NeuronCheckpointData& data);
    bool readSynapses(SynapseCheckpointData& data);
    
    /**
     * Read pending delayed spikes (SpikeHistory section). Returns false
     * only on a parse failure; an absent section (legacy checkpoint)
     * leaves `events` empty and returns true.
     */
    bool readDelayedSpikes(std::vector<DelayedSpikeEvent>& events);
    
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
     * Buffer pending delayed spikes (written as the SpikeHistory
     * section at finalize()). These are in-flight synaptic inputs;
     * without them a reload cannot continue the saved trajectory.
     */
    bool writeDelayedSpikes(const std::vector<DelayedSpikeEvent>& events);
    
    /**
     * Phase 5: buffer an extra raw section, written at finalize()
     * after SpikeHistory and before End. Used by Brain::save for the
     * SimulationState (spike counters, learned preference), RandomState
     * (RNG engine state), Plasticity (per-synapse spike histories) and
     * Memory (working + episodic) sections. Absent sections are
     * tolerated on load (legacy checkpoints), so old files still load.
     */
    bool bufferSection(CheckpointSection sectionType, const std::vector<uint8_t>& payload);
    
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
    // Payloads buffered until finalize(), when the header is complete.
    std::vector<uint8_t> neuronPayload_;
    std::vector<uint8_t> synapsePayload_;
    std::vector<uint8_t> delayedSpikePayload_;
    // Phase 5 extra sections, in the fixed write order used by finalize().
    std::vector<std::pair<CheckpointSection, std::vector<uint8_t>>> extraPayloads_;
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
    static const uint64_t table[256] = {
        0x0000000000000000, 0x7AD14982F1AB1AAA, 0xF5AE8FD3C2B0CA21, 0x8B447D51B1EF6C8B,
        0x8F4A5B9768A9570F, 0xF5BC2B5578FEB497, 0x7A8DCFB63DC3E51D, 0x103B1E449D88C797,
        0x1EC371E124D29E03, 0x7453A1C73627658D, 0xFDF33BBBC942A20F, 0x877B2A68DFAAD287,
        0x89F330D7860D6C0D, 0xF38BAEC8747846B7, 0xC2AA066E6747B13B, 0x58221D2D77FCE8A1,
        0xC6C1ADC6E85C5AD5, 0x5C9589657EE67F5, 0xD7E4E8BB7921AC6F, 0x9DCC19F86ACDD2F5,
        0x99B6A8912364C9AB, 0xD3E498F2112FB121, 0x5CB51B386FB88AAF, 0xC61DE50FADB1EA05,
        0xC03F63A65497D67B, 0x5A773D65466196002, 0xDF869C9F59421A5A, 0x973E2D8C6943AC60,
        0x976A55C210C339B6, 0xEDF898813D9474E8, 0xB8382C0F4EF74572, 0xD250CDBDAFF979E8,
        0xD68055A4476A8E6C, 0xBC382F0766961AE2, 0xE9792F21FA53DD9A, 0xE3EFC0C2DA82CBA0,
        0xE34D7ED95572782D, 0xE9CFB4E875372DE7, 0xBC5E25C2B22A185F, 0xD6C68471C2CB08D5,
        0xA6A4ACDAF1627D5B, 0x5C26ECF041C54C9F, 0xD9D71CEADCBC6B11, 0xB1EFBDEEDDAC5D7B,
        0xB1AF44A524B67EAF, 0xEBDD5466E45B6E51, 0xBECDB41C75B0DB85, 0xD645757E65C5E00F,
        0xA67505D54EC2E4B7, 0x5C3795586772153013, 0xD9384695E7079266, 0xB1C08763597486603,
        0xB3B8B83F2E0E32BD9, 0xE9DA5C19CE9F5D27, 0xBE6BF1A3FD263A0B, 0xD4G2---1AABUVI H---B5EA
    };
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