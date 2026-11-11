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
    // CRC-64 table precomputed using polynomial 0x42F0E1EBA9EA3693
    static const uint64_t table[256] = {
        0x0000000000000000, 0x42F0E1EBA9EA3693, 0x85E1C3D753D46D26, 0xE7D1E4E2F0F3D695,
        0x4B0232A92B527E26, 0x0D12B1D8944094B5, 0xAE02AF2BEFAACD46, 0xECB2EEFC67DD9AF7,
        0x96E6FA6DD54098A4, 0xD4F69BD97C22C337, 0x77E69FC289147CA6, 0x15961F1080961B55,
        0xE93E8F2B59A8A2C6, 0x8B1ECEA9F0DAED77, 0x381E8E813136D144, 0x5A2EEFF898669A75,
        0xDCBCFECC254CEC22, 0x9E0C6F5F88DCC791, 0x3D0C7F57C4495E02, 0x5F3C5EEFD35F37F1,
        0x831CF8EC795050D2, 0xE12CD93D302A5FA3, 0x021CB995003C842C, 0x40AC5864A9E8DD5F,
        0xD20B38B359A6EAEE, 0x907B79F4F01625FD, 0x334B9E4C301F5E6E, 0x615B5F1D9723D18D,
        0xA75B0F06B75282FE, 0xE5BBE016FE427F2D, 0x867BFC18A67430AE, 0x44AB5D6F0F84467D,
        0x1A2B8D71D68E0CCE0, 0x583BB97DFEFE57B3, 0xDB6B08EA9E513934, 0x991BAD99F741B047,
        0x172B0D4230DC862D4, 0x553BD813D9FC5FC7, 0xD64D784BDBD74254, 0x94DD992CF9715B67,
        0x72872E2C893F8FD0, 0xE0779F5DEC0FA4E3, 0x53E76E8FA5F9FDC4, 0x11D72F8AECB6C627,
        0xA3C7DF69D4A8AC74, 0xE1C79ED890C897C7, 0x82878646413787556, 0x40776B76E8919E27,
        0xBA7B9B415B33CD74, 0xD8ABD1D1E2237107, 0x6B1BD1CD825C92E4, 0x99EB12DD2B6CDBF7,
        0x3F3B7250E5DACC26, 0x5D2B3320EEEDCA35, 0xFE3B433B4F4D919A, 0xAC4B2663D6BD48B9,
        0x9A7B968A72CDB5D4, 0xD8EB76AECB1D2CA7, 0x5B3B66D496410376, 0x395BA755DFE54895,
        0xEFDBB84465C5C6C2, 0x8D4DB274CE1522F1, 0x2E0DF08C29D8D9C0, 0x3D5D413C72E8EEF3,
        0x0F0D30C206B49E82, 0x4D0D507D5C0450F1, 0x8F3D0037363EFB96, 0xEDED21B3FF1D1EC5,
        0xC9CD01AF954EE836, 0xBBDD40EFEEFE573D, 0x1E9D808896EBAC8E, 0x5CBD2136DF5BC39D,
        0xD4FD8182A2D104CE, 0x96CD40724051AFDD, 0x35DD9047397C45AE, 0x77CDF2338C2E9ACD,
        0xE3DD82ECADDC417E, 0x81CD236DB1661D8D, 0x464D026481E8A2DC, 0x042D4297286178997,
        0xA03D32BB2A4E29C8, 0xE2DD93453F1E9FFB, 0x81C5E3DAFF0F2A68, 0x43D543EAC4DFF27B,
        0x0FD557B6553E7B08, 0x4DC596A69EEBE53B, 0xFE55C7B2DAA3AE88, 0xAC45E6135183C3BB,
        0x9295660821440628, 0xD005A6A4BA740B5B, 0x13355B903F489BC8, 0x51A5BA80543B04EB,
        0xBB159A47B40A2078, 0xD9D5FB370F1EB5AB, 0x6AE5BB033011EE18, 0x98F57FA38B21D6EB,
        0x3C35DF88BEEC1128, 0x5E257E8849478B5B, 0xFD3559C58FF65FC8, 0xEF254A0721AF5A5B,
        0x8125BA2AE5CCCFDC, 0x43C5FB7C4E5CE39F, 0x00D5EB852F317C08, 0x42E5FB63846958993,
        0xC605CB4B4C3AC4C0, 0xB4956A29E73E0113, 0x17052C624F66F6C4, 0x55451E81342C1BF7,
        0xD9D5EE76F7C24D86, 0x9BE57E547CE8B6B5, 0x38F57E0C2FFDFC46, 0x5A55BFC4A6CD6527c
    };
    uint64_t crc = 0xFFFFFFFFFFFFFFFFULL;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        uint8_t idx = (crc ^ bytes[i]) & 0xFF;
        crc = table[idx] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFFFFFFFFFFULL;
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