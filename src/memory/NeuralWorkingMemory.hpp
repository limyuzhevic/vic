#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_set>

namespace nlm {

// MemoryEntry: Individual working memory entry with advanced tracking
struct MemoryEntry {
    NeuronId neuronId;
    float activation;
    SimulationStep timestamp;     // When encoded
    SimulationStep lastAccess;    // When last accessed (for LRU)
    SimulationStep accessCount;   // How many times accessed (for LFU)
    float importance;              // Importance score (0-1)
    float strength;                // Current trace strength
    float optimalStrength;         // Target strength for this entry
    bool isCompressed;             // Whether this memory is stored in compressed form
    std::vector<float> compressedPattern; // Compressed representation
    
    MemoryEntry(NeuronId id, float act) 
        : neuronId(id), activation(act), timestamp(0), lastAccess(0)
        , accessCount(0), importance(1.0f), strength(act), optimalStrength(act)
        , isCompressed(false) {}
};

// PatternTemplate: Template for pattern completion (e.g., chunking)
struct PatternTemplate {
    std::vector<float> pattern;    // The template pattern
    std::vector<NeuronId> neurons; // Associated neurons
    float strength;                // Template strength
    float similarity;              // How similar is this to current pattern
    bool isActive;                 // Whether template is currently active
    
    PatternTemplate() : strength(0.0f), similarity(0.0f), isActive(false) {}
};

// WorkingMemoryManager: Manages capacity, decay, and advanced features
class WorkingMemoryManager {
public:
    enum class EvictionPolicy { LRU, LFU, Importance, Random };
    
    WorkingMemoryManager(size_t capacity, EvictionPolicy policy = EvictionPolicy::LRU)
        : capacity_(capacity), evictionPolicy_(policy), compressionThreshold_(0.7f)
        , consolidationThreshold_(0.5f) {}
    
    // Add memory entry
    void addEntry(const MemoryEntry& entry);
    
    // Get memory entry by neuron
    const MemoryEntry* getEntry(NeuronId neuron) const;
    
    // Update memory based on decay, activity, and consolidation
    void update(TimestepDuration dt, float baseDecayRate, 
                const std::function<float(NeuronId)>& accessImportance,
                const std::function<void(NeuronId)>& onConsolidation);
    
    // Evict memories based on capacity and policy
    void evictMemories(size_t targetSize);
    
    // Get all memory entries
    const std::vector<MemoryEntry>& getEntries() const { return entries_; }
    
    // Get current size
    size_t getSize() const { return entries_.size(); }
    
    // Get capacity
    size_t getCapacity() const { return capacity_; }
    
    // Set capacity
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    // Set eviction policy
    void setEvictionPolicy(EvictionPolicy policy) { evictionPolicy_ = policy; }
    
    // Enable/disable compression
    void setCompressionEnabled(bool enabled) { compressionEnabled_ = enabled; }
    
    // Enable/disable consolidation
    void setConsolidationEnabled(bool enabled) { consolidationEnabled_ = enabled; }
    
    // Get memory statistics
    size_t getCompressedCount() const;
    float getAverageImportance() const;
    float getCompressionRatio() const;
    
private:
    std::vector<MemoryEntry> entries_;
    size_t capacity_;
    EvictionPolicy evictionPolicy_;
    float compressionThreshold_;
    float consolidationThreshold_;
    bool compressionEnabled_;
    bool consolidationEnabled_;
    
    // Helper methods for eviction
    void evictLRU();
    void evictLFU();
    void evictByImportance();
    void evictRandom();
};

// NeuralWorkingMemory: Implements working memory using neural dynamics
// NOT a simple storage vector - uses persistent activity and recurrent connections
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention
// - Advanced capacity management with LRU/LFU eviction
// - Importance-based prioritization and consolidation
// - Compressed storage and pattern completion (chunking)
// - Multi-level storage (short-term and long-term buffers)

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store information in working memory
    // The information is encoded as neural activity pattern
    void store(const std::vector<float>& pattern, float strength = 1.0f);

    // Store specific neuron activation
    void storeToNeuron(NeuronId neuron, float activation);

    // Retrieve current working memory content as activity levels
    std::vector<float> retrieve() const;

    // Check if specific neuron is part of working memory
    bool contains(NeuronId neuron) const;

    // Get activation level of a specific neuron in working memory
    float getNeuronActivation(NeuronId neuron) const;

    // Update working memory (maintenance and decay)
    void update(TimestepDuration dt);

    // Clear working memory
    void clear();

    // Get number of active memory traces
    size_t getActiveTraces() const { return activeTraces_.size(); }

    // Get capacity
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }

    // Decay rate for memory traces
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }

    // Get neurons currently in working memory
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }

    // Strengthen working memory representation (for rehearsal)
    void strengthenMemory(float factor);

    // Competition between memory traces
    void runCompetition();

    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;

    // Get overall memory activity level
    float getMemoryActivity() const;

private:
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    // Update recurrent connections for maintenance
    void updateRecurrentConnections();

    // Decay weak memory traces
    void decayWeakTraces();

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    
    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    
    // Active memory traces
    std::vector<size_t> activeTraces_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;
};

// AttentionalSelection: Neural attention through competition
// NOT Transformer attention - uses lateral inhibition and gain modulation
//
// Mechanisms:
// - Competition between neural populations
// - Salience-based selection
// - Recurrent amplification of winners
// - Inhibitory surround suppression

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Process competing sensory inputs and select winners
    // Returns IDs of winning neural populations
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                              float globalInhibition = 0.5f);

    // Apply attention to a specific region (focus processing there)
    void focusOnRegion(RegionId region);

    // Release attention (allow all regions equal processing)
    void releaseAttention();

    // Get current attention focus
    std::vector<RegionId> getAttendedRegions() const;

    // Set attention parameters
    void setInhibitionStrength(float strength);
    void setExcitationStrength(float strength);
    void setCompetitionThreshold(float threshold);

    // Get inhibition level for a neuron
    float getInhibitionFor(NeuronId neuron) const;

    // Get excitation level for a neuron
    float getExcitationFor(NeuronId neuron) const;

    // Update attention dynamics
    void update(TimestepDuration dt);

    // Is this neuron currently attended?
    bool isAttended(NeuronId neuron) const;

    // Get attention winners
    const std::vector<NeuronId>& getWinners() const { return winners_; }

    // Top-down attention (goal-directed)
    void applyTopDownBias(NeuronId neuron, float biasStrength);

    // Bottom-up attention (stimulus-driven)
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);

    // Reset attention
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
    
    std::vector<RegionId> attendedRegions_;
    std::vector<NeuronId> winners_;
    std::vector<float> neuronSalience_;
    
    // Biases/salience stored by NeuronId for correct lookup in processCompetition
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
};

} // namespace nlm