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

// Checksum implementation - CRC tables

inline uint64_t ChecksumCalculator::crc64(const void* data, size_t size) {
    // CRC-64 (Castagnoli polynomial 0x1EDC6F41)
    static const uint64_t table[256] = {
        0x0000000000000000ULL, 0x42F0E1EBA773EA01ULL, 0x85E1C3D75647D402ULL, 0xC79104EB116C1F03ULL,
        0xE751D6FC90456C04ULL, 0x55F1D7C7A03FE605ULL, 0x76113D4830CF6F06ULL, 0xB4A15E11B5CFA707ULL,
        0xEF02D827273DF80CULL, 0x5DF2F9EE5695190DULL, 0x2E833FDF7987A40EULL, 0x8CE32F7606DD271FULL,
        0x4213078B5EF5C514ULL, 0x40F37E52EAFF5595ULL, 0xA3E3CE439CB99216ULL, 0xC1A3EFD0AC99E717ULL,
        0x8A0646E89EBD7F8CULL, 0xD8A6758F5E23C88DULL, 0xFBA64C5E5FE63B8EULL, 0x19863DB7158D1E8FULL,
        0x12D6FE49C8A17734ULL, 0xB0D6AF8078717A25ULL, 0x13E6D975AB7E1B76ULL, 0xB10638ACB9735F77ULL,
        0x3FB28AB18F5A582CULL, 0xAD3298C8CFE9532DULL, 0xDE227AA5F0C5CE7EULL, 0x9C424ED21B0D7F2FULL,
        0xD791CE1D7EF12674ULL, 0x9541208421360575ULL, 0x463116235CEA3E26ULL, 0x84814CEA0C5E9F27ULL,
        0xF5C5FA0F6A45E79CULL, 0x577529D8793C1A9DULL, 0x74767D451DC13C4EULL, 0xB6C70EC6FCAAD74FULL,
        0xC1E70E5F0883BAC0ULL, 0xF3770F962A3B1DC1ULL, 0x80972705504BE7D2ULL, 0x42C7D6C9D1754E23ULL,
        0x5B5708B2F4C05378ULL, 0xE907D9AB64A08499ULL, 0xCA6728FF8E86708AULL, 0x88672D961F1C3B8BULL,
        0x63079EDCBD7BC4E0ULL, 0xD1973F25CDADD8B1ULL, 0xA26711249C2BAC62ULL, 0x1027362DEF512063ULL,
        0xBBF77A14AC7F6928ULL, 0x1977320D9C45DA29ULL, 0x0A3743CCCE09EDDAULL, 0x783744E5EE1348DBULL,
        0x0312786D9C3E91F0ULL, 0x715265340FDD4CD1ULL, 0x02525D2730E768D2ULL, 0x10A26CFEF625DB03ULL,
        0xEB62FE02D4CFB9F8ULL, 0x5922BF9B04D5CAE9ULL, 0x7A329A893CD871EAULL, 0xB8B29B406C8D20EBULL,
        0x7302566A7C95B940ULL, 0xB1C2C7D3AC5FE371ULL, 0x6212DFE62135A9C2ULL, 0x90D2EE18E1EF7CF3ULL,
        0xD512FCD702A52A48ULL, 0x97D2BD7B522D7739ULL, 0xC402CEA66727528AULL, 0xE6A2EF5F772227B9ULL,
        0xF5C21F06E76F98E0ULL, 0x5772BE0F573BCAD1ULL, 0x2412AFFE27A8ADB2ULL, 0x86C2EE2647552A33ULL,
        0xE9F2603CA9CF8FC8ULL, 0x5B5224335D9FBCD9ULL, 0x28C22C24810E9DDAUULL, 0x8E92CFCD212C2EEBBULL,
        0x410268707AFCD710ULL, 0x83D2780BAD878721ULL, 0xD0E20A5B5E690F72ULL, 0x92124E928F838151ULL,
        0x9DDCC02B6FC9C0C6ULL, 0xDF4C41D2FDA0A2F5ULL, 0xACCBCDC6CF3B0794ULL, 0x1E8BCEE29BA1EAF5ULL,
        0x15D5EE1AF3AA28FEULL, 0xB795C013E3248ECFULL, 0x5495C70BE9BF4BCDCULL, 0xE705D6A4B9F5FEADULL,
        0xCA45A48CF3DB8796ULL, 0x8855C5D523C1CD87ULL, 0x0B45B7C6D3ADEBD4ULL, 0x1955C6AFB9883EA5ULL,
        0x642570B293EC4BFEULL, 0xD675109A2389BC2FULL, 0xA545960CCDFC6B2CULL, 0x374564D5FD56562DULL,
        0x3C857DC44F1C7B76ULL, 0xAEB57FCDA6C6ED47ULL, 0xDD456E84F68A36D4ULL, 0x9F15CFDDBC907BD5ULL,
        0x94153769A3B4C37EULL, 0xD655468022FE57DFULL, 0xF575B494B24D3CDCULL, 0xE7C515DD2386EEFDULL,
        0xECF525D49D18E776ULL, 0xFE3257AD1DD45297ULL, 0x8D22C7C4F1EEE634ULL, 0xCFB2D6FD11F80935ULL,
        0x8682972693C9E08EULL, 0x4482D6DF2354B2BFULL, 0x17520C746D6AE0ECULL, 0x05A23DD94FD9751DULL,
        0x7A422EA5D35E4D66ULL, 0xB8B2CF7CF26A6D87ULL, 0x0B82985701F6BC34ULL, 0x1912355E21C67B15ULL,
        0x5A13260D097352C4ULL, 0xE8732614297929035ULL, 0xC5A35C9D49796E46ULL, 0x8713318459C35377ULL,
        0x2C0334586F7FC3CCULL, 0xCE9333916F15D4EFULL, 0xFDA30DBCA83655ACULL, 0x9F33CE755236B0DFULL,
        0xB43E6842AB398EE4ULL, 0x16EE897B9B239AB5ULL, 0x051EA9AF6F3652B6ULL, 0x773EAD968F7C1BB7ULL,
        0xECBDEA2E7E662C0CULL, 0xFEDDDAC71F2C6E1FULL, 0x8EDC6B5FEF058CCEULL, 0xCCAC5A767F2F0CEFULL,
        0x279CA80F4E152B84ULL, 0xCBAC6917545B9EB5ULL, 0xF8FC587F1574F0C6ULL, 0xBADCCD56910DFED5ULL,
        0xB00CA01A57AAC57EULL, 0x12AC40E347408F5FULL, 0x417CA1B1C1D975ACULL, 0x832C90851DBD04DFULL,
        0xD81C04E99C6FAF34ULL, 0x9ACC0578DCF3C615ULL, 0xA9AC45B8583F1356ULL, 0x3B4C166F4842B277ULL,
        0x3E9CB2C6C8D667CCULL, 0xAC6CD18F0B58FAAFULL, 0xDF5CE085CB46E1FCULL, 0x9D7C4108FB0C43CFULL,
        0xC83C9C316252DD74ULL, 0xFA3C1D987DBDE655ULL, 0x892CC62A0D2C20A6ULL, 0x4B4CD1731D07E3D7ULL,
        0x640CA59D4DF2B55CULL, 0xD6AC860206872B7FULL, 0xA59C4637946DA0DCULL, 0x37CC7723B4A7B9DFULL,
        0x78ACB83EF6CD1294ULL, 0x0AFCB457C646E1B5ULL, 0xD98CF52C76168326ULL, 0x9B5C4485C6AD6557ULL,
        0xC01CD4BBEE623EFCULL, 0xF2AD159277E885CFULL, 0x81ED25BE6AAFE1CCULL, 0x438D96871E919DEFULL,
        0x5E0D5D2A0C564744ULL, 0xEC2D7C23FC5C6F67ULL, 0xBF1DC9080CF6D914ULL, 0xDD0DA621FC8C2017ULL,
        0xF21FC939BA560E6CULL, 0xC00F28A03A4C8D8FULL, 0x937F98C5D065862CULL, 0x512F59FCC5726F4FULL,
        0x04CF4BB5B0B52E24ULL, 0x76DF0BBC00EB7787ULL, 0xE5CFAB0315D5C534ULL, 0xB7AFAC3A25CFAC47ULL,
        0xA42AB8A217D8371CULL, 0x369AABFB07B2E5EFULL, 0x658A390C37AC461EULL, 0xD79A981521763A3DULL,
        0xD45A5A8344A46386ULL, 0x965AB54A74DD3137ULL, 0xC14AE68F4D57D084ULL, 0xF30A47D65D4F0FB3ULL,
        0xDEAA5C418A55B6B8ULL, 0xCCBA3D48BA4F14EBBB0C3DF0ULL, 0x0FEA6B05F84572FDFE7C6D39ULL, 0x6D3A180872A68CCFULL, 0x662AA50A5E73C02AULL, 0xD4FA1441FE190609ULL,
        0xA7AA54BBEFECF4BAULL, 0x35BAC462CF25B5A9ULL, 0xE87B602F4F1B04F8ULL, 0xBAEB01671360B3CBULL,
        0xD32B1348C3BAF110ULL, 0x91FB5202F0A5F6E3ULL, 0x424B207D206FAB30ULL, 0x808B01145B74BC53ULL,
        0xFFCBD08CD00045F8ULL, 0xCDEB3095C0B5A3CAULL, 0x8F2B5AEE502F0CD9ULL, 0x4D1BAA4750406CFAUULL,
        0xB6077357B85407F1ULL, 0x1497C23E9838DFB2ULL, 0x0787C6211C826CB5ULL, 0x75F7E708FCC8CDF6FULL,
        0x7EE775417EEA9884ULL, 0x1C7734380EF52EB7ULL, 0x2F67F0270DF25408ULL, 0x8D97E18EE69B878BUL,
        0xEC477EAA45B5DAF0ULL, 0x5EE77C63262DBEC3ULL, 0xAD87735DBFC2AD10ULL, 0xEFC75254DE282933ULL,
        0x9E66A0F8276C0D88ULL, 0xDC368D9117B63CB9ULL, 0xFF2658017B51742AULL, 0xCBE65E48BB2C3719ULL,
        0x84C65F01207E08C2ULL, 0x464695F830457AE1ULL, 0x17164389C02E0720ULL, 0x05943CC6F01BD403ULL,
        0x5AA126E869950608ULL, 0xE841C7C1998A08CBULL, 0xCB315E81AD377EB8ULL, 0xD9515F88EDA2E0BBULL,
        0xCE7122D5C94F4990ULL, 0xFCE11F022F0429B3ULL, 0x9F01E94F6500A2D4ULL, 0x8D916826E515EBF7ULL,
        0x4214622F36E2C1ACULL, 0x80A4E3C626F9489FULL, 0xD9B4E5A70BAD53ECCULL, 0x9BA465EE3B23A84FULL,
        0xDE64B7E74D19B6F4ULL, 0x9CE4F0AE7CFAFE27ULL, 0xCFB439BA07E152D4ULL, 0xEEE339834D2A8517ULL,
        0x5149CBE9E217D46CULL, 0xC3594A00276B72BFULL, 0x306983EFB6D0CBECULL, 0x12F9C2F673EB3E0FULL,
        0xB6B980CF1E55C15AULL, 0x549980D6FEE247597ULL, 0x47489FF73A08FD84ULL, 0x85185AEE1AD323F7ULL,
        0xE278DC017DAD7B4ACULL, 0x5018CD745AE094DFULL, 0x23084871C0FB6EDCULL, 0xB1C833682F9040FFULL,
        0x2A882720DF3512A4ULL, 0xB85816E9CF1AF5D7ULL, 0x8B4878E4CD1BE5E4ULL, 0xD9D8E90D5D46F4B7ULL,
        0xC2A8BB853D54982CULL, 0xF048FAE8616A1D1FULL, 0xF318EFD6F675D5D2ULL, 0x81D8EE1322BAA1E1ULL,
        0x44F86ECDD6520E14ULL, 0xC6784EE41A792227ULL, 0xD5087E59BB9E9494ULL, 0x97A8BF70BB651987ULL,
        0xF49851026F82A6FCULL, 0xC648700B2F15D02FULL, 0x15385571CFA7D6BCULL, 0xC248E6A8CDAC68EFULL,
        0xED689DF1DD8447D4ULL, 0x5F1829C4EDA7B5F7ULL, 0x2C082657F586E6C4ULL, 0x8E98571F46CF1833ULL,
        0x4FE3C5D6CE3B2358ULL, 0x8D4345EF6E006D6BULL, 0xBEB3E5D4441FE978ULL, 0x3CA39FDDB47C115FULL,
        0x03B1CFE51827300CULL, 0x71D13F6F286A928FULL, 0xE2E1FF857B1F5D2CULL, 0x105136AD6B14AE1FULL,
        0xAEB960F2DBFF9F24ULL, 0xD6B9815567391807, 0xA8892151C4D563D4ULL, 0x5A4D22485E5A0BD7ULL,
        0x432D80B5014E6A8CULL, 0x81CD410B913CBE6FULL, 0xF2DDAFA06A13CADCULL, 0x100DD9586A28A7FFULL,
        0x357DBE01BAC99AF4ULL, 0xA7CDB3C4BA201EC7ULL, 0xD49D7359E8BB4D10ULL, 0x963D12301C803933ULL,
        0xE0ADB6CC5C1F88E8ULL, 0x529DAB750C64F5BBULL, 0x2FDDED9FECF14E68ULL, 0x8FDDCE675E9CDB4FULL,
        0x4ADCDC202F4A9AA4ULL, 0xCD4CDD99CF5109D7ULL, 0xFECDD73B0E3CA634ULL, 0x9FDDA87620268767ULL,
        0xCEFDDA4CC7EB426CULL, 0xECDDE055C716CBBFULL, 0xBFCDEB8E39F0060CULL, 0x3F1DFA6735DA735FULULL,
        0x6E2DB34CE2CF7314ULL, 0x0CDDC23F16585D97ULL, 0xF2DCA836279BB144ULL, 0xB02C995FEA344267ULL,
        0x858CB5D7B4D9410CCULL, 0x476CAAEEC4E0EE3FFF977C29ULL, 0xB50F0CA05A14AEE228DDBE1AULL, 0x73EF8D39BAD816F9ULL,
        0x220FE74B6B3A40D6ULL, 0x901F16829B42E5D5ULL, 0x270F04AD598A8504ULL, 0xEB2F45A479C3BE17ULL,
        0xD23FF58E675AD84CULL, 0x908F24D7773E60EFULL, 0x031F659E47AF5DECULL, 0x71EF86566E2F1EFFULL,
        0xDEEF24DF922E3A64ULL, 0xACAC55B642576687ULL, 0x9D3C1529D3B5E034ULL, 0xFFBCA860067E1997ULL,
        0xD42E2A2A85BC71D2ULL, 0x860E9B4376F6D3E1ULL, 0x15FEEB6897EC0AE4ULL, 0x375E4B51423D75D7ULL,
        0x7CF65B989A270A2CULL, 0xDE9625456A5AF01FULL, 0xFD168338144CF42CULL, 0xEF26920FA8C7A6FFULL,
        0x8CF66B86F82A2D4AULL, 0x4E0639FF385CB169ULL, 0x2D16CB485163DE6CULL, 0xFF2635D71E9611DFULL,
        0x06D62536F84F1424ULL, 0x74E616FD41F4A327ULL, 0xE7A616C612BB3C74ULL, 0xB5F6772F22A8BD67ULL,
        0xD6A625D2940741CCULL, 0x941673BBA852C9EFULL, 0x1763C3A6B34B83ECULL, 0x3573C2F59B0188DFULL,
        0x2A7312E8819BF394ULL, 0xB853B2FF910CB4C7ULL, 0x8B4F525E0157ABD4ULL, 0xD94FE32B714480A7ULL,
        0xC43F51B1D1C065ACULL, 0xA61F904A85A092DFULL, 0xE51F002D4D3C7BDCULL, 0xB71F11F4CDFEE9CFULL,
        0xB01F202C52A61166ULL, 0x81CF81A56259E247ULL, 0xF2EF63A5125AA394ULL, 0x106FF398B27288C7ULL,
        0x973FF4BE05B9E31CULL, 0xD50F77F75580C13FULL, 0xA6DF35B0E59EE48CULL, 0x349F3419A5CE374FULL,
        0x319ED3C07D337FC4ULL, 0xA3FE92C72D5F3AE7ULL, 0xD01E50789DDABF54ULL, 0x924ECAB783489B77ULL,
        0xBDC3C2DC618C206CULL, 0xE363737B81B7551FULL, 0x403316AB8FA0B06CULL, 0x1213B552AF3F550FULL,
        0x27F315B05E3733BAULL, 0xB5D38671FE784289ULL, 0x86935F0668A20E6AULL, 0x4483CEFD72E65359ULL,
        0x579369D70B172C02ULL, 0xE5F6B8A06C8D2E31ULL, 0xB2762A3D3C9C13C2ULL, 0x40B63E14041B8AB5ULL,
        0x3356632C9C1B4C0AULL, 0xA12602032EC8CFB9ULL, 0xE2D6D458EE92294AULL, 0x1026B560EEB09D79ULL,
        0x2F6645088CB66E1EULL, 0xBD06D401AD70ECADOUT, 0xDE16D6E5EDB6819CULL, 0x9C7615C84ADFA69FULL,
        0x4D1626E8255550D4ULL, 0xFF26A1510580F9E7ULL, 0x7C266D843D75AA24ULL, 0xEE163FBB2DB63357ULL,
        0x53FF6EDB7A55FACCULL, 0xC1CFCE823AB233EFULL, 0x92DFBC680D09CCF2ULL, 0x104FEBE5DFE4C025ULL,
        0xEB1F23C61CA08E7EULL, 0xB9DF9214AD9A4EDDULL, 0xFAEF74DABB6902CEULL, 0x888F5543FBF2DF3DULL,
        0xDDEF8767A6EBB196ULL, 0x9F0FB6AE9644B7C5ULL, 0xAC0F147F867CF9C6ULL, 0xFEEFC7B6B6F36DD5ULL,
        0xCAEF37D6980AD47EULL, 0xE84E96200B40F1ADULL, 0xBB4E74E50CDCA8DEULL, 0xD9FE35808EC3F4ADULL,
        0xCEEE9B04FC6BCD26ULL, 0xECBEEA2D0C313555ULL, 0xBFCE2C286DE25746ULL, 0x3D8EDD857EAED175ULL,
        0xBEDE4F2B1E70AACEULL, 0xDCFEEE12FE6ADDEFULL, 0xEFCECE087F901CDEULL, 0xADDE8F1F4F6337EFULL,
        0xC28EBEBECFDA9AC4ULL, 0x109EDEF7EE242F37ULL, 0x438EDACDF4FEAF34ULL, 0x81DE3AC645C73267ULL,
        0x046EA8E45D0DBBCCULL, 0x76AE699C4D10C6EFULL, 0xE59E7BC6061B6FECULL, 0xB7DEDADE16C8091FULL,
        0x240E9C686E2C90C4ULL, 0x96DE4D5FDF5E9907ULL, 0xD5EE0F46CF9A0404ULL, 0x979EB2AFDFBE3B27ULL,
        0x768EC6C87E346428ULL, 0x043E37540702652FULL, 0x178E154C57CACE2CULL, 0x35EE5648B79C0A1FULL,
        0xBED9984CA4A13374ULL, 0xCE1988B974872F47ULL, 0xFE29CB10421A9B34ULL, 0x9C792B03E24CC307ULL,
        0xDD99133F36AF4ABCULL, 0x9F09F236267EF2CFULL, 0xBC1971A115E56DCCULL, 0xDEC75F280743864FULL,
        0xE2676C638F84AD24ULL, 0xB067ADD3BFB46F57ULL, 0x8307DF5FDC833C44ULL, 0x41C7C0767C28B177ULL,
        0x4C773D4AEE0D980CULL, 0xEE771A237FE92C9FULL, 0xCD6712A5CFD57F8CULL, 0x9F7736F8D07AD42FULL,
        0xFA47A8B78F9F8924ULL, 0xB8F769AFF2C41A17ULL, .cpp: src/performance/CheckpointSystem.hpp:444:30: warning: 'static const' initializer is missing in member initializers - consider adding an out-of-line definition
    return crc ^ 0xFFFFFFFFFFFFFFFF;
  static const uint64_t table[256] = { // (precomputed)
        0x0000000000000000, 0x42F0E1EBA773EA01, ...


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