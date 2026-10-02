#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../performance/MemoryPool.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <algorithm>
#include <cmath>
#include <string>
#include <chrono>
#include <thread>
#include <limits>

namespace nlm {

// Neuron and synapse state types
struct NeuronState;
struct SynapseState;

// Memory system types and configurations

// Memory type enumeration
enum class MemoryType {
    Working,
    Episodic,
    Semantic,
    Procedural,
    Associative,
    None
};

// Memory system access type
enum class AccessType {
    Read,
    Write,
    Update,
    Delete
};

// Memory operation type
enum class MemoryOperationType {
    Store,
    Retrieve,
    Update,
    Delete,
    Compress,
    Consolidate,
    Replay,
    Search,
    Cache,
    Index
};

// Compression algorithms
enum class CompressionAlgorithm {
    LZW,
    Huffman,
    PCA,
    Zlib
};

// Compression levels
enum class CompressionLevel {
    None,
    Fast,
    Balanced,
    High
};

// Neuron ID type
using NeuronId = uint64_t;
using SynapseId = uint64_t;
using NeuronIndex = uint32_t;
using SynapseIndex = uint32_t;

// Invalid neuron/synapse ID
const NeuronId INVALID_NEURON_ID = static_cast<NeuronId>(-1);
const SynapseId INVALID_SYNAPSE_ID = static_cast<SynapseId>(-1);

// Neuron and synapse state types (forward declaration)
struct NeuronState {
    float membranePotential;
    float restingPotential;
    float threshold;
    float resetPotential;
    float leakConductance;
    float synapticInput;
    float firingRate;
    uint8_t firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    uint64_t neuronType;
    uint64_t regionId;
    uint64_t populationId;
    
    NeuronState()
        : membranePotential(0.0f)
        , restingPotential(0.0f)
        , threshold(0.0f)
        , resetPotential(0.0f)
        , leakConductance(0.0f)
        , synapticInput(0.0f)
        , firingRate(0.0f)
        , firingState(0)
        , refractoryRemaining(0)
        , refractoryPeriod(0)
        , adaptationVariable(0.0f)
        , lastSpikeTime(0.0f)
        , neuronType(0)
        , regionId(0)
        , populationId(0) {}
};

// Memory system configuration structures
struct WorkingMemoryConfig {
    size_t capacity;
    float decayRate;
    bool enableCompression;
    bool enableCaching;
    bool enableReconstruction;
    bool enableIndexing;
    float interferenceThreshold;
    size_t cacheSize;
    size_t indexSize;
    
    WorkingMemoryConfig()
        : capacity(100)
        , decayRate(0.95f)
        , enableCompression(true)
        , enableCaching(true)
        , enableReconstruction(true)
        , enableIndexing(true)
        , interferenceThreshold(0.8f)
        , cacheSize(64)
        , indexSize(256) {}
};

struct EpisodeMemoryConfig {
    size_t maxEpisodes;
    bool enableReplay;
    bool enableCompression;
    float consolidationThreshold;
    float timeThreshold;
    bool enableIndexing;
    size_t cacheSize;
    bool enableDecay;
    float decayRate;
    
    EpisodeMemoryConfig()
        : maxEpisodes(1000)
        , enableReplay(true)
        , enableCompression(true)
        , consolidationThreshold(0.5f)
        , timeThreshold(100.0f)
        , enableIndexing(true)
        , cacheSize(64)
        , enableDecay(true)
        , decayRate(0.95f) {}
};

struct AssociativeMemoryConfig {
    float associationStrength;
    float interferenceThreshold;
    float plasticityInfluence;
    bool enableInterferenceManagement;
    bool enableSpreadingActivation;
    size_t cacheSize;
    
    AssociativeMemoryConfig()
        : associationStrength(1.0f)
        , interferenceThreshold(0.7f)
        , plasticityInfluence(0.5f)
        , enableInterferenceManagement(true)
        , enableSpreadingActivation(true)
        , cacheSize(64) {}
};

struct MemoryConfig {
    WorkingMemoryConfig workingMemory;
    EpisodeMemoryConfig episodicMemory;
    AssociativeMemoryConfig associativeMemory;
    MemoryPoolConfig poolConfig;
    
    MemoryConfig()
        : workingMemory()
        , episodicMemory()
        , associativeMemory()
        , poolConfig() {}
};

struct MemoryPoolConfig {
    size_t neuronPoolSize;
    size_t synapsePoolSize;
    size_t workingMemoryPoolSize;
    size_t episodicMemoryPoolSize;
    size_t associativeMemoryPoolSize;
    
    MemoryPoolConfig()
        : neuronPoolSize(1024)
        , synapsePoolSize(2048)
        , workingMemoryPoolSize(256)
        , episodicMemoryPoolSize(512)
        , associativeMemoryPoolSize(128) {}
};

struct MemoryIndexEntry {
    SimulationStep timestamp;
    float importance;
    float novelty;
    size_t sequenceNumber;
    float relevanceScore;
    
    MemoryIndexEntry(SimulationStep ts = 0, float imp = 1.0f, float nov = 0.0f, 
                     size_t seq = 0, float rel = 1.0f)
        : timestamp(ts), importance(imp), novelty(nov), sequenceNumber(seq), relevanceScore(rel) {}
};

struct MemorySearchCriteria {
    std::vector<float> sensoryPattern;
    SimulationStep startTime;
    SimulationStep endTime;
    float minImportance;
    float maxDistance;
    
    MemorySearchCriteria()
        : startTime(0), endTime(std::numeric_limits<SimulationStep>::max()),
          minImportance(0.0f), maxDistance(100.0f) {}
};

struct MemoryPoolStats {
    size_t neuronPoolSize;
    size_t synapsePoolSize;
    size_t workingMemoryPoolSize;
    size_t episodicMemoryPoolSize;
    size_t associativeMemoryPoolSize;
    
    size_t activeNeuronAllocations;
    size_t activeSynapseAllocations;
    size_t activeWorkingMemoryAllocations;
    size_t activeEpisodicMemoryAllocations;
    size_t activeAssociativeMemoryAllocations;
    
    double memoryUsagePercent;
    double allocationEfficiency;
    size_t memoryFragments;
};

struct MemoryOperation {
    MemoryOperationType type;
    MemoryType memoryType;
    size_t dataSize;
    double executionTime;
    bool success;
    std::string description;
};

struct MemoryProfile {
    SimulationStep step;
    Timestamp timestamp;
    
    struct Usage {
        size_t neuronMemory;
        size_t synapseMemory;
        size_t workingMemory;
        size_t episodicMemory;
        size_t associativeMemory;
        size_t indexMemory;
        size_t poolMemory;
        
        // Calculated fields
        size_t totalMemory;
        double memoryUtilization;
    } usage;
    
    struct Performance {
        double accessTime;
        double allocationTime;
        double compressionTime;
        double searchTime;
        size_t operationsPerStep;
        double throughput;
    } performance;
    
    struct Quality {
        float memoryIntegrity;
        float accessAccuracy;
        float compressionRatio;
        float deduplicationRate;
    } quality;
    
    struct Activity {
        size_t activeTraces;
        size_t consolidatedMemories;
        size_t replayedMemories;
        size_t cacheHits;
        size_t cacheMisses;
    } activity;
};

} // namespace nlm

