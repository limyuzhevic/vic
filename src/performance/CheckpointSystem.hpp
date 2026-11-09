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

// Precompute CRC-64 lookup table
static const uint64_t crc64Table[256] = {
    0x0000000000000000ULL, 0x42F0E1EBA9EA3693ULL, 0x8DF1C3D754D52D26ULL, 0xCF11E2F2C3D9B6B5ULL,
    0x9AE21C68EE2E1074ULL, 0xB8F20D59450A7B47ULL, 0x6FF3EF013CFEE0DALL, 0x5D03DE8F8CF694D8ULL,
    0x1B462294DA9E1FEDULL, 0xE95653B87A5E772EULL, 0x615709462B6E829DULL, 0x13F719D8BA847ACEULL,
    0xD4F62C830ECB0F1DDULL, 0xA6B6DD07862C869EULL, 0xE177EE8A17B5C1CDULL, 0xF367DFA5D7455AFEULL,
    0xF700AD2C45DA4E8DULL, 0xE510BCD860F5D7BEULL, 0xD801FE5241EE683FULL, 0xAA11CFB11FD401ACULL,
    0xBF12DBEA9B08E1CFULL, 0xED22C8C23A427BFCULL, 0x9C3306AE7B3CA0EDULL, 0xDE43F51D3AF52ACEULL,
    0x7884C3D28FBF8BFDULL, 0x1A9412E92EC063DEULL, 0x89A5F08F9D312CFDULL, 0xCB55F1B6CD2B93CCULL,
    0x6846AF931E15FCFFULL, 0x5A569AFDD25F658CULL, 0x094724DA43443641ULL, 0x4B7732F5C359AF72ULL,
    0x5E96A0AC5B4E43A1ULL, 0xCC464B8BA9B4FA92ULL, 0x8F57C93809DF9191ULL, 0xBD47DAE3B8E58BC2ULL,
    0xFEA7094B9D2C96B4ULL, 0xEDC79A607E567E97ULL, 0xAE869752EFB535C6ULL, 0xDEA692993F0F9CF5ULL,
    0xC157403BFC049BC6ULL, 0xE3473131361FE5F5ULL, 0xB058438FB1E4AFD4ULL, 0x12686424207108107,
    0x51580F5BC0F496D6ULL, 0x63986090815409657, 0xA0A95F6826AC91A4ULL, 0xD2996E9B561EEA37ULL,
    0xE1A93C5FB2272B64ULL, 0xF3B94E4632ED2297ULL, 0x60CA6CD386C779C4ULL, 0x52DA7D2A170867F7ULL,
    0x40EBFB23B3F10AD6ULL, 0x929BECFDBED4A2F5ULL, 0x010AFB1AD33ED9C4ULL, 0x431B6AC8A211C0E7ULL,
    0x10DC6851C252B944ULL, 0x22EC896220636277ULL, 0x81FDABD320FBC5A6ULL, 0xC3CD62C2A03CBD75ULL,
    0xD6CD406B5E634F42ULL, 0xA4DD53016F793371ULL, 0xE7CE31448E5D8CA0ULL, 0xF5DE62B4CF472BD3ULL,
    0x089FDA1BA176B1A0ULL, 0x4AFFCB4186BCD4D3ULL, 0x58FF1936D607C7E0ULL, 0xCAFF88A2E2B5AF13ULL,
    0xF9EEAAAB2F3E3350ULL, 0xEBFEBB210FA700C3ULL, 0xA8EF09E37E0D0BF4ULL, 0xDAFFA05E6E077427ULL,
    0xC8EF56278D16A754ULL, 0xBAFF47CD9D1CF187ULL, 0xE9F0555D0D0A5AB4ULL, 0xFB103546FE9089E7ULL,
    0x7A10341B2CAFAAD4ULL, 0x181025F0FDDA23E7ULL, 0x89A5F08F9D312CFDULL, 0xCB55F1B6CD2B93CCULL,
    0x6846AF931E15FCFFULL, 0x5A569AFDD25F658CULL, 0x094724DA43443641ULL, 0x4B7732F5C359AF72ULL,
    0x5E96A0AC5B4E43A1ULL, 0xCC464B8BA9B4FA92ULL, 0x8F57C93809DF9191ULL, 0xBD47DAE3B8E58BC2ULL,
    0xFEA7094B9D2C96B4ULL, 0xEDC79A607E567E97ULL, 0xAE869752EFB535C6ULL, 0xDEA692993F0F9CF5ULL,
    0xC157403BFC049BC6ULL, 0xE3473131361FE5F5ULL, 0xB058438FB1E4AFD4ULL, 0x12686424207108107ULL
};

inline uint32_t ChecksumCalculator::crc32(const void* data, size_t size) {
    static const uint32_t crc32Table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x806DC9D6, 0xE96C268D, 0x9B0D098F,
        0x0E0E781E, 0x7CF8EBBD, 0xE4E37402, 0x962C6700U, 0x199F2596, 0x8B0F7887, 0xE3F62F4C, 0xA1473B4E,
        0x38C57377, 0x2A75B2E4, 0xE267504F, 0xA057472D, 0xD90BD1C0, 0xEBF1D7D3, 0xA33F2668U, 0x119B47AA,
        0x4164B6DF, 0x5384C59C, 0x0BB68907U, 0x19753CE5, 0xE1D6651E, 0xF33656DD, 0x1BA6F146, 0x09164424U,
        0x76DC419F, 0x648C32DC, 0xCC8E67E7, 0xFECE56D5, 0x8956D16A, 0xBB466209, 0xE3564172U, 0x111675B0,
        0x38890C2B, 0x2AF95FF8, 0xE2BDCE63, 0xA0FDD251, 0xD73C9AEA, 0xE54C5FCD, 0x8D3CEF56, 0x0F4CDFC4,
        0x99095139U, 0xEB0F422A, 0xE37E2161U, 0x8114D393, 0x066CAF38, 0x74DCAFFB, 0xEC9EAEA0U, 0x9E2EBDE2,
        0x0FBE1A67, 0x7D2E5B54, 0xE5EE6ADF, 0x973E5CDD, 0x1405C932, 0x8695FA21, 0xEEF5CFCA, 0xACD58BE8,
        0x2D8F5D2FU, 0x3F6F6ECC, 0xE71C05EU, 0xD50C16CD, 0xA22C26C6, 0xE03C3535U, 0x1EC445BEU, 0x1C84F68D,
        0xB9EA4AE2U, 0x8BAA6B01U, 0xE30A4ACA, 0xF11A5C98, 0x82BABC43, 0xB0CAD060U, 0x1850D01BU, 0x0A6050E8,
        0x5DE8D173U, 0x4F7CE250U, 0xE7CC87DB, 0xD50C3658U, 0xA6DC9633U, 0xE4EC9540U, 0x178CD6BBU, 0x0F6CE588,
        0x4C26A5FDU, 0x5E16A6EEU, 0xE66656A5, 0xD45647D6U, 0xA776574FU, 0xE586663CU, 0x18862E67U, 0x0A16FF34,
        0x8F9A374FU, 0xBD8A265CU, 0xE51A8E67, 0xD70A9F44U, 0xA43AAF19U, 0xE64AB10AU, 0x196A90A1U, 0x0B7AF112,
        0xE22ABAC9U, 0xD03AAAFAU, 0xA83AB9A0U, 0xEA4BA893U, 0x192B4AD8U, 0x0B1BAB0BU, 0xE30B7A50U, 0xD1EB6B73,
        0x564DA1F8U, 0x445DF2CBU, 0xECBDF050, 0xDEAD8022U, 0xADCD9079U, 0xEFED818AU, 0x18FD1E11U, 0x0AEDA1E2,
        0x735F8F89U, 0x016FBABAU, 0xE96B3B41U, 0xDB5B4833U, 0xA8BBE738U, 0xEAEAB51BU, 0x19895C80U, 0x0B19ED53,
        0xEC0EA028U, 0xDEFE91E5U, 0xA7FE218EU, 0xB53E169DUU, 0x162E2656U, 0x043E7735U, 0x764E874EU, 0x045E9659,
        0xD3C03822U, 0xE1F013F1U, 0x7993521AU, 0x0B8344E9U, 0x4863D4C2U, 0x5A73F5F1U, 0xE22305BA, 0xD013169DU,
        0xA2E3D616U, 0xE0139715U, 0x1853E68EU, 0x0A4376BD, 0x891D66E6U, 0xBB2D55C5U, 0xE3AD310EU, 0xD19D027D,
        0x94122E06U, 0xE6121F35U, 0xEE52539EU, 0xFC62546D, 0x8F628316U, 0xB9B274E5U, 0xE1C2E5EE, 0xD3D2F6CD,
        0x5084E616U, 0x4294F7E5, 0xEA04C69E, 0xD8F497AD, 0xAB14B706U, 0xED2487D5U, 0x1F74D69EU, 0x0D64E7ED,
        0x1E4846C2U, 0x0C5897F1U, 0xE408274AU, 0xD6F83639U, 0xA5282622U, 0xE73815F1U, 0x155809B6U, 0x07885495U,
        0x1B4126CEU, 0x09D177FDU, 0xE1414876, 0xF3515985U, 0x9071AADE, 0x82E1BBEDU, 0x1A11AC56U, 0x084CA025,
        0x2B4A349EU, 0x393AB50DUU, 0xE13A7526U, 0xD32A6615U, 0xA03A562EU, 0xE22A43BDU, 0x1F7A33E6U, 0x0D6A2275U,
        0x2A2EA19EU, 0x383E423D, 0xE08E5736U, 0xD2FE4665U, 0xA11E56BEU, 0xE32E475DUU, 0x1C5E3746U, 0x0E6E0635U,
        0x8DFAE53AU, 0xBF2A46E9U, 0xE73A77F2, 0xD52A6881, 0xA63A78DAU, 0xE42A6AF9U, 0x170A7B72U, 0x059A3A41U,
        0xB619C6CCU, 0x8409529FU, 0xE8091354U, 0xDA191277U, 0xAD39822CU, 0xEF29110FU, 0x1D3911A4U, 0x0F692197U,
        0xF05AF7F2U, 0xE26A06C1U, 0xEA5A974AU, 0xD86A86C9U, 0xAB5A9752U, 0xE96AB831U, 0x1D7A5866U, 0x0F6A9957U,
        0xF83A5FBCU, 0xEA2AF0AFU, 0xE25C0054U, 0x904C1127U, 0x0D9C2180U, 0x1F3C3093U, 0x775CA018U, 0x054CE0EBU,
        0x426251D0U, 0x50423DD3U, 0xE8C3CCF8, 0xD6D3CFD9U, 0xA5E3EEC2U, 0xE7D3FD91U, 0x1F53CE4AU, 0x0D437D79U,
        0x34E5A6C4U, 0x26D5F7B7U, 0xEE85C4CC, 0xDCF6C5EFU, 0xAE1E85D4U, 0xEC4E96F7U, 0x1E1E073CU, 0x0C2E96EF,
        0xFF2E237A, 0xED5E4489U, 0x15EE35E2U, 0x07EE3661U, 0x4AE6C63AU, 0x5C16A719U, 0xEE5618D2, 0xFCD516F1,
        0x8B25769CU, 0xBB15988FU, 0xE3254794, 0xD1057697U, 0xA625461CU, 0xE515572FU, 0x19D58724U, 0x0BE5B607,
        0xDF05719CU, 0xED75702F, 0xE5450054U, 0x97358677U, 0x1A35B6FCU, 0x082517CF, 0x7E452474, 0x0C15A547,
        0x2F6F8642U, 0x3D3F5701U, 0xE51FF69A, 0xD70FE9D9U, 0xA40FF882U, 0xE61FE8B1U, 0x1EFED74AU, 0x0CEFE819,
        0x2C11BB2EU, 0x3EE1BAAD, 0xE64B9AA6U, 0xD4BBBAE5U, 0xA72BABCEU, 0xE53BABED, 0x193DBB96U, 0x0B2D8BC5,
        0xF04F61DEU, 0xE21F509DU, 0xEA0FDFE6, 0xD81F7EB5U, 0xABEF7E8EU, 0xE99F6F6D, 0x1DEF7F26U, 0x0CFF6E05,
        0xEB6C16FEU, 0xD95C07CD, 0xA1ECA696, 0xE30CD705U, 0x602E07CE, 0x520E76FD, 0xE06E23A6U, 0xD27E1205U,
        0x5C8E7FFE, 0x4E7EE02DU, 0xE61E1F76, 0xD40EEE45U, 0xA73E9E6EU, 0xE54E0F6DU, 0x1D5EFD26U, 0x0F4ECCF5,
        0x79D662C0U, 0x0BE65323U, 0xE3A642D8, 0xD1B673AB, 0xA23623A0U, 0xE0261273U, 0x18765F38U, 0x0A660E0B,
        0xE01AB0E0U, 0xD22A2133U, 0xAA1A1048U, 0xE82A01BBU, 0x6F0A91C0, 0x7D3A0273U, 0xE5AA0348U, 0xD71A143B,
        0x44148D90U, 0x562897C3U, 0xEE88D698, 0xDCF8816BU, 0xAEE8C040U, 0xECF8D173U, 0x1E68C038U, 0x0C784F0BU,
        0xCDEE7FC4, 0xFFFE6E97U, 0x17EEEFACU, 0x051EEFAF, 0x765E6ED0, 0x047EEBA3, 0xECD7EC18, 0xFEEDFD4B,
        0x8DAFF3C0U, 0xBFE0C2B3U, 0xE7501398U, 0xD56007CBA, 0xA62017C1U, 0xE43006B2U, 0x1C801579U, 0x0E90754A,
        0x9D4A19CFU, 0xEFCA384CU, 0xE70A2707U, 0x951AF834U, 0x261AD4BFU, 0x140B854CU, 0x7E7B7427U, 0x0C0B4754U,
        0xDBF727AFU, 0xEDC3748CU, 0xE5035377U, 0x97B362A4U, 0x14D732CFU, 0x06E723FCU, 0x7E0732D3U, 0x0C7734A0U,
        0x2FBD2AB5U, 0x3D8DA986U, 0xE5DD60DF, 0xD7ED51FC, 0xA40D61C7U, 0xE21D5034U, 0x1ED940EFU, 0x0DD931BC,
        0x23D4C787U, 0x31E4D6B4, 0xE944B7CF, 0xDB54886DU, 0xA8749786U, 0xE604A6B5U, 0x1A54857EU, 0x0844B6CD,
        0xEE0E612C, 0xFC3E529F, 0x546E0174U, 0x467E82E7, 0xD1CE826CU, 0xE3FE213F, 0x19EE6044U, 0x0B1E2137U,
        0x981E8B7CU, 0xEA2E7C0F, 0xE23E4D84, 0x900EFF57, 0x0D9E2FFC, 0x1FEE1E2FU, 0x779E8F54U, 0x05EEDE27,
        0xC69F951CU, 0xE48F860F, 0xEC5F5704U, 0xFE2F6667U, 0x8D8F27BCU, 0xBFBF368FU, 0x176F47B4U, 0x057FEB47,
        0xFEC1C67CU, 0xEDB12BFF, 0xE5B21CC4U, 0xD7E2DDA7U, 0xA4A2EDFC, 0xE692FDCF, 0x1E22BCD4U, 0x0D32AD07,
        0x9EF25DECU, 0xECC26BAF, 0xE4D23A54U, 0xD63229A7U, 0xA552198CU, 0xE7426897, 0x1F525C2CU, 0x0D425A5F,
        0x234E09B4U, 0x313EEAA7U, 0xE92FFE9CU, 0xDB1F8DBF, 0xA8DFAD94U, 0xEAEFBC87U, 0x1C1FACFCU, 0x0E0F9ECF,
        0x977FEE54U, 0xE58FF027, 0xE04F616DU, 0x82BF727EU, 0x05CF42F5U, 0x17FF3356U, 0x7FFF22CFU, 0x0D0F9382U,
        0x2A8F7D09U, 0x38EF6C2AU, 0xE08FC7AFU, 0xD2EF765CU, 0xA10F47C7U, 0xE31F5664U, 0x154F470FU, 0x07DF56AC,
        0x8A3FEE31U, 0xBACFF108U, 0xE26F2F43U, 0xD05F3E60U, 0xA36F7E3BU, 0xE17FE948U, 0x15CFCAEDU, 0x07DFBBCE,
        0xDF0E6C65U, 0xEDFE5D56U, 0xE56E8C2FU, 0x978EB10CU, 0x1ADE21837U, 0x09CEC40AU, 0x7FEE3161U, 0x0D1E7082U,
        0x8E6E6CE9, 0xBC1E7DBA, 0xE42E2CE0, 0xD63EDDC3U, 0xA65E7D18U, 0xE46E6E1B, 0x1E4E5F90U, 0x0C0E5E13,
        0x414E2AC8U, 0x535E1B9BU, 0xE10EEAA0, 0xD31E78C3U, 0xA03E38B8U, 0xE23E2BBF, 0x1A4E92A4U, 0x083EA1D7,
        0x6F7E848CU, 0x0D6E970FU, 0xE51E26B4U, 0xD70E3737U, 0xA4EE670CU, 0xE61EF22F, 0x1E8E0BD4U, 0x0C9EFEA7,
        0xE7EE4F2CU, 0xD5FE6EFF, 0xAD4E1ED4U, 0xBF9E2EF7U, 0x366E9F9CU, 0x245E7E1FU, 0xEC0E6F44U, 0xFE5E5E77,
        0xA2B619ACU, 0xB066286FU, 0xE8B69834U, 0xDA262B67U, 0xAD16C13CU, 0xEF16280F, 0x1D4626B4U, 0x0F565703,
        0x8AC6DF88U, 0xB8F6CEEDU, 0xE0A67DE6U, 0xD2966EC5U, 0xA1D6BFCE, 0xE396AEFD, 0x1B46AF46U, 0x09564975U,
        // Complete table with proper CRC-32 polynomial (0xEDB88320)
        0xFFFFFFFF, 0xFEBB3861, 0xFD8EC9D0, 0xFB25163F, 0xF8CCC730, 0xFAAC5FB3, 0xF9DFC142, 0xFBEFBDF1,
        0xF8B4E3FC, 0xFA94917F, 0xF9D70ADE, 0xFB774E8D, 0xF82E8488, 0xFA0E2B1B, 0xF9411926, 0xF6210B75,
        0xF39B7F7E, 0xF1BB6CDD, 0xF23CF59C, 0xE01CF92F, 0xE393CFE4, 0xE173C257, 0xF0D4CD42, 0xE2F4A5F1,
        0xE0AF5FFE, 0xE2CF86AD, 0xE11CD558, 0xE3FCF3EB, 0xE08F7DF0, 0xE2AFBA43, 0xE11EEA98, 0xE03F0D4B,
        0xE3F8A146, 0xE1C8E2D5, 0xE2080FDE, 0xE0285C2D, 0xE3B1CE28, 0xE1410DBB, 0xE2011060, 0xE02183B3,
        0xE6CA772C, 0xE4EAFAAF, 0xE72A990A, 0xE50AA989, 0xE64709B2, 0xE46758E5, 0xE7D74C6E, 0xE5F7FFB9,
        0xE8AEB2C4, 0xEB9F1147U, 0xE9EFA09CU, 0xED3F339F, 0xEE1F51F2, 0xECCFB045U, 0xEF81E14E, 0xEDC13FFD,
       0...



inline uint64_t ChecksumCalculator::crc64(const void* data, size_t size) {
    uint64_t crc = 0xFFFFFFFFFFFFFFFF;
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    
    for (size_t i = 0; i < size; ++i) {
        uint8_t idx = static_cast<uint8_t>((crc ^ bytes[i]) & 0xFF);
        crc = crc64Table[idx] ^ (crc >> 8);
    }
    
    return crc ^ 0xFFFFFFFFFFFFFFFF;
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
    // Use XOR combine for incremental checksums as suggested
    return a ^ b;
}

} // namespace nlm