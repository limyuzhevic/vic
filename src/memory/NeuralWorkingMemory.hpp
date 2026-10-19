#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <array>
#include <deque>
#include <algorithm>
#include <random>
#include <cmath>
#include <chrono>

namespace nlm {

// Memory trace structure for working memory items
struct WorkingMemoryTrace {
    uint64_t id;                    // Unique identifier
    SimulationStep timestamp;       // When created
    SimulationStep lastAccess;      // Last access time
    float strength;                 // Memory strength (0-1)
    float confidence;               // Encoding accuracy (0-1)
    float precision;                // Information resolution (0-1)
    
    // Neural representation
    std::vector<NeuronId> neuronIds;
    std::vector<float> activations;
    std::vector<float> firingRates;
    
    // Content and metadata
    std::vector<float> contentVector;\n    std::vector<std::string> features; // Pattern features
    std::string metadata;
    
    // Position and salience
    float priority;                 // Priority (0-1)
    float salience;                 // Salience (0-1)
    float novelty;                  // Novelty (0-1)
    
    // Interference and updating
    std::vector<uint64_t> interferers; // Neurons causing interference
    float interferenceScore;         // Cumulative interference (0-1)
    
    // Temporal dynamics
    SimulationStep age;             // Age in steps
    float decayFactor;               // Per-step decay factor
    
    // Competition and selection
    float competitionScore;          // Competition fitness (0-1)
    bool isActive;                   // Currently active
    bool isConsolidated;             // Being consolidated to LTM
    
    // Pattern completion
    std::vector<float> attractorState; // Attractor dynamics state
    std::vector<float> reconstruction;  // Reconstructed pattern
    
    WorkingMemoryTrace(uint64_t traceId = 0)
        : id(traceId), timestamp(0), lastAccess(0), strength(0.5f), confidence(1.0f), 
          precision(1.0f), priority(0.5f), salience(0.5f), novelty(0.0f), 
          interferenceScore(0.0f), age(0), decayFactor(0.999f), competitionScore(0.0f),
          isActive(true), isConsolidated(false) {
        attractorState.reserve(50);
        reconstruction.reserve(50);
    }
};

// Attractor dynamics for pattern completion
struct AttractorState {
    std::vector<float> position;     // Current position in attractor space
    std::vector<float> velocity;     // Velocity
    std::vector<float> acceleration; // Acceleration
    float energy;                    // Energy level
    float temperature;               // Temperature parameter for noise
    bool isStable;                   // Whether converged
    SimulationStep stepsToStable;    // Steps to reach stability
    
    AttractorState(size_t dimension = 50) 
        : position(dimension, 0.0f), velocity(dimension, 0.0f), 
          acceleration(dimension, 0.0f), energy(0.0f), temperature(0.1f),
          isStable(false), stepsToStable(0) {}
};

// Interference management structure
struct InterferenceState {
    std::unordered_map<NeuronId, float> proactiveInterference; // Neuron's interference strength
    std::unordered_map<NeuronId, float> retroactiveInterference; // Retroactive effects
    std::vector<std::pair<uint64_t, uint64_t>> interferencePairs; // (trace1, trace2) pairs
    float interferenceThreshold; // Threshold for interference
    
    InterferenceState() : interferenceThreshold(0.3f) {}
};

// Chunking and capacity management structure
struct CapacityManager {
    std::vector<std::vector<uint64_t>> chunks;      // Chunked memory items
    std::vector<uint64_t> itemToChunk;             // Mapping item -> chunk index
    std::unordered_map<uint64_t, std::vector<uint64_t>> chunkMembers; // Chunk members
    size_t maxChunkSize;                           // Max items per chunk
    size_t maxChunks;                              // Max number of chunks
    
    CapacityManager() : maxChunkSize(4), maxChunks(7) {} // Capacity limits (4-7 items typical)
};

// Core NeuralWorkingMemory: Biological working memory implementation
// Implements active maintenance, pattern completion, interference management,
// and attention-based selection using neural dynamics
class NeuralWorkingMemory {
public:
    NeuralWorkingMemory(size_t capacity = 7, float decayRate = 0.05f);
    ~NeuralWorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Core working memory operations
    void store(const std::vector<float>& pattern, float strength = 1.0f);
    void storeToNeuron(NeuronId neuron, float activation);
    void storeTrace(const WorkingMemoryTrace& trace);
    
    // Retrieval and representation
    std::vector<float> retrieve() const;
    std::vector<float> retrievePattern(const std::vector<float>& cue) const;
    std::vector<WorkingMemoryTrace> retrieveSimilar(float threshold = 0.5f) const;
    bool contains(NeuronId neuron) const;
    float getNeuronActivation(NeuronId neuron) const;
    
    // Active maintenance and updating
    void update(TimestepDuration dt);
    void rehearse(const std::vector<NeuronId>& neurons, float factor = 1.5f);
    void refresh(const std::vector<NeuronId>& neurons);
    void updatePatterns();
    
    // Memory management
    void clear();
    void clearInactive();
    void consolidate(float threshold = 0.7f);
    void decay(float decayRate);
    void updateDecay(float deltaRate);
    
    // Pattern completion and attractor dynamics
    void patternCompletion();
    void attractorDynamics();
    void reconstructPattern(size_t traceId, std::vector<float>& reconstructed) const;
    float computePatternSimilarity(const std::vector<float>& patternA, 
                                   const std::vector<float>& patternB) const;
    
    // Interference management
    void manageInterference();
    void resolveInterference(NeuronId neuron, float interferenceStrength);
    float computeInterferenceScore(const WorkingMemoryTrace& trace) const;
    void proactiveInterferenceControl();
    void retroactiveInterferenceControl();
    
    // Competition and selection
    void runCompetition();
    void attentionSelection();
    void winnerTakeAll();
    void balancedWinnerSelection();
    std::vector<NeuronId> selectWinners(const std::vector<NeuronId>& candidates) const;
    
    // Capacity management and chunking
    void chunkInformation();
    void reorganizeMemory();
    void prioritizeItems();
    void evictItems();
    size_t getAvailableCapacity() const;
    
    // Neural coding and plasticity
    void neuralPopulationCoding(const std::vector<float>& pattern);
    void synapticModulation(float learningRate = 0.01f);
    void spikeTimingDependentPlasticity();
    void homeostaticPlasticity();
    
    // Accessors and modifiers
    size_t getCapacity() const { return capacity_; }
    void setCapacity(size_t cap) { capacity_ = cap; }
    
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
    size_t getActiveTraceCount() const { return activeTraces_.size(); }
    const std::vector<WorkingMemoryTrace>& getMemoryTraces() const { return memoryTraces_; }
    const std::vector<NeuronId>& getMemoryNeurons() const { return memoryNeurons_; }
    
    float getMemoryActivity() const;
    float getMemoryStrength() const;
    std::vector<float> getCurrentActivity() const;
    
    // Attention integration
    void setAttention(Brain* brain);
    void applyTopDownBias(NeuronId neuron, float biasStrength);
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);
    void integrateWithAttention();
    
    // State and statistics
    bool isStable() const;
    float getAverageStrength() const;
    float getMaxInterference() const;
    SimulationStep getOldestTraceAge() const;
    std::vector<float> getPatternVector() const;
    
    // Brain integration
    void integrateWithBrain(Brain* brain);
    void synchronizeWithNeurons();
    void updateNeuronActivity();
    
    // Control and configuration
    void setAttractorTemperature(float temp) { attractorTemp_ = temp; }
    void setInterferenceThreshold(float threshold) { interferenceState_.interferenceThreshold = threshold; }
    void enableChunking(bool enable) { chunkingEnabled_ = enable; }
    void setLearningRate(float rate) { learningRate_ = rate; }
    
    // Simulation and timing
    void step(SimulationStep step);
    void setCurrentTime(Timestamp time) { currentTime_ = time; }
    
    // Debugging and utilities
    void logMemoryState() const;
    void printMemoryStats() const;
    std::string getMemorySummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core state
    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    float learningRate_;
    
    // Memory content
    std::vector<WorkingMemoryTrace> memoryTraces_;
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    std::vector<size_t> activeTraces_;
    
    // Neural dynamics
    std::vector<AttractorState> attractorStates_;
    std::vector<std::vector<float>> patternMatrix_;
    std::vector<float> globalActivity_;
    std::vector<float> firingRates_;
    
    // Interference management
    InterferenceState interferenceState_;
    
    // Capacity and organization
    CapacityManager capacityManager_;
    
    // Attention and selection
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
    std::vector<NeuronId> winners_;
    std::vector<NeuronId> attendedNeurons_;
    
    // Simulation state
    SimulationStep currentStep_;
    Timestamp currentTime_;
    float attractorTemp_;
    bool chunkingEnabled_;
    bool attractorEnabled_;
    
    // Statistics
    size_t totalConsolidations_;
    size_t totalEvictions_;
    float averageStrengthHistory_;
};