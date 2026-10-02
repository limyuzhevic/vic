#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// NeuralWorkingMemory: Implements working memory using neural dynamics
// NOT a simple storage vector - uses persistent activity and recurrent connections
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention

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

    // Enhanced memory capabilities for integration

    // Memory indexing and caching for O(log n) access
    void indexMemory(const std::vector<float>& pattern, SimulationStep timestamp,
                    float importance, float novelty = 0.0f);
    std::vector<size_t> searchSimilar(const std::vector<float>& pattern, 
                                     size_t maxResults = 10) const;

    // Memory compression and storage efficiency
    std::vector<float> compressMemory(const std::vector<float>& pattern) const;
    bool cacheMemory(size_t index, bool highPriority = false);

    // Memory lifecycle management
    void createMemoryTrace(const std::vector<float>& pattern, float initialImportance = 1.0f);
    void consolidate(float relevanceThreshold);
    void decay(float decayRate, float minImportance);

    // Memory interaction with neural plasticity
    void enhanceWithPlasticity(float plasticityFactor);
    void adaptToEnvironment(const std::vector<float>& environmentalFactors);

    // Memory coordination and arbitration
    void coordinateAccess(MemorySystemType type, const std::vector<float>& pattern,
                         AccessType accessType = AccessType::Read);
    void adjustPriority(const std::vector<float>& pattern, float priorityDelta);

    // Enhanced capabilities for learning
    void replay(const std::vector<float>& pattern, size_t repetitions = 5);
    void strengthenAssociations(const std::vector<float>& patternA,
                               const std::vector<float>& patternB, 
                               float strengthFactor = 1.0f);
    void detectInterference(const std::vector<float>& pattern);
    void resolveInterference(const std::vector<float>& pattern);

    // Performance and efficiency
    void optimizeAccessPatterns();
    void optimizeMemoryLayout();
    size_t getTotalMemoryUsage() const;
    double getAverageAccessTime() const;

    // Statistics and monitoring
    struct MemoryStats {
        size_t activeTraces;
        size_t cachedMemories;
        float averageImportance;
        size_t compressionRatio;
        float interferenceLevel;
        float plasticityInfluence;
    };
    
    MemoryStats getStats() const;

    // Configuration and control
    void setMemoryConfig(const MemoryConfig& config);
    MemoryConfig getMemoryConfig() const { return config_; }
    void enableCompression(bool enable) { config_.enableCompression = enable; }
    void enableCaching(bool enable) { config_.enableCaching = enable; }
    void enableReconstruction(bool enable) { config_.enableReconstruction = enable; }

    // Memory system coordination
    void registerCoordinator(MemoryCoordinator* coordinator) { coordinator_ = coordinator; }
    void setMemoryPool(MemoryPool<NeuronState>* pool) { memoryPool_ = pool; }

    // Auto-tuning
    void autoTune(float performanceThreshold);
    void analyzePerformance();
    float getPerformanceScore() const;

private:
    // Internal implementation details
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
    
    // Enhanced memory system components
    MemoryCoordinator* coordinator_;
    MemoryPool<NeuronState>* memoryPool_;
    std::vector<std::vector<float>> indexedPatterns_;
    std::vector<MemoryIndexEntry> index_;
    std::unordered_map<size_t, std::vector<float>> cache_;
    std::unordered_set<size_t> highPriorityCache_;
    
    // Performance metrics
    mutable std::atomic<size_t> accessCount_;
    mutable std::atomic<double> totalAccessTime_;
    mutable std::atomic<size_t> compressionCount_;
    mutable std::atomic<float> averageImportance_;
    
    // Configuration
    MemoryConfig config_;

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