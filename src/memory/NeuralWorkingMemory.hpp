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
// - Enhanced sensory encoding and temporal pattern storage
// - Synaptic activity traces for consolidation
// - Winner-take-all competition among memory traces
// - Working memory-based action selection
// - Exponential decay and episodic consolidation

class NeuralWorkingMemory {
public:
    NeuralWorkingMemory();
    ~NeuralWorkingMemory();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Store information in working memory
    // The information is encoded as neural activity pattern
    void store(const std::vector<float>& pattern, float strength = 1.0f);

    // Store specific neuron activation with enhanced encoding
    void storeToNeuron(NeuronId neuron, float activation, float magnitude = 0.0f, 
                      float timestamp = 0.0f, float feature = 0.0f);

    // Store temporal sensory events for sequence learning
    void storeTemporalSensoryEvent(NeuronId neuron, float magnitude, 
                                  float timestamp, float strength = 1.0f);

    // Store synaptic activity trace for plasticity and consolidation
    void storeSynapticTrace(NeuronId neuron, size_t numOutgoingSynapses,
                           float timestamp, float strength = 1.0f);

    // Store detailed spike information including rate, burst activity, and temporal context
    void storeSpikeInfo(NeuronId neuron, float spikeRate, float burstActivity,
                       float timestamp, SimulationStep step);

    // Store detailed synaptic trace with source, destination, weight, and type
    void storeSynapticTrace(NeuronId sourceNeuron, NeuronId targetNeuron,
                           float weight, SynapseType type, float timestamp,
                           SimulationStep step, float strength = 1.0f);

    // Store temporal sequence for pattern learning
    void storeTemporalSequence(NeuronId neuron, SimulationStep step,
                              float timestamp, float strength = 1.0f);

    // Get comprehensive spike information for a neuron
    bool getSpikeInfo(NeuronId neuron, float& spikeRate, float& burstActivity,
                     float& timestamp, SimulationStep& step) const;

    // Get synaptic trace information
    bool getSynapticTrace(NeuronId sourceNeuron, NeuronId targetNeuron,
                         float& weight, SynapseType& type,
                         float& timestamp, SimulationStep& step) const;

    // Get temporal sequence information
    bool getTemporalSequence(NeuronId neuron, float& strength,
                           float& timestamp, SimulationStep& step) const;

    // Update working memory (maintenance, decay, competition)
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

    // Competition between memory traces (winner-take-all)
    void runCompetition();

    // Enhanced competition with attention-based selection
    void runAttentionBasedCompetition();

    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;

    // Get memory stability score (resistance to decay)
    float getMemoryStability(NeuronId neuron) const;

    // Get overall memory activity level
    float getMemoryActivity() const;

    // Decay weak memory traces (with exponential decay)
    void decayWeakTraces();

    // Consolidate to episodic memory if memory is stable
    void consolidateToEpisodic(Brain* brain);

    // Connect working memory to motor planning and action selection
    void influenceActionSelection(Brain* brain);

private:
    // Enhanced maintenance mechanisms
    struct MemoryTrace {
        NeuronId neuron;
        float activation;
        float magnitude;
        float timestamp;
        float feature;
        float stability;
        SimulationStep lastAccessed;
        size_t accessCount;
    };
    
    // Synaptic trace information
    struct SynapticTrace {
        NeuronId sourceNeuron;
        NeuronId targetNeuron;
        float weight;
        SimulationStep timestamp;
        float eligibility;
    };
    
    // Temporal pattern information
    struct TemporalPattern {
        NeuronId neuron;
        SimulationStep step;
        float timestamp;
        float strength;
        std::vector<SimulationStep> firingSequence;
        float burstActivity;      // Added: Spike burst activity
        bool isSequence;          // Added: Flag for temporal sequences
    };
    
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    // Update recurrent connections for maintenance
    void updateRecurrentConnections();

    // Enhanced decay with exponential model
    void exponentialDecay(MemoryTrace& trace, TimestepDuration dt);

    // Calculate memory stability based on activation and rehearsal
    float calculateStability(const MemoryTrace& trace) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    float decayRate_;
    float consolidationThreshold_;
    
    // Enhanced memory storage
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<float> memoryMagnitudes_;
    std::vector<float> memoryTimestamps_;
    std::vector<float> memoryFeatures_;
    std::vector<float> memoryStabilities_;
    std::vector<SimulationStep> memoryLastAccessed_;
    std::vector<size_t> memoryAccessCounts_;
    
    // Active memory traces
    std::vector<size_t> activeTraces_;
    
    // Enhanced storage for temporal patterns and synaptic traces
    std::vector<SynapticTrace> synapticTraces_;
    std::vector<TemporalPattern> temporalPatterns_;
    std::vector<SensoryEvent> sensoryEvents_;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    
    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;
    
    // Attention-based importance scores
    std::vector<float> attentionScores_;
    
    // For tracking recent sensory events
    static constexpr size_t MAX_TEMPORAL_EVENTS = 50;
};

// Sensory event structure for temporal storage
struct SensoryEvent {
    NeuronId neuron;
    float magnitude;
    float timestamp;
    float strength;
    SimulationStep step;
};

// Working memory competition and attention system
class WorkingMemoryCompetition {
public:
    WorkingMemoryCompetition();
    ~WorkingMemoryCompetition();
    
    void initialize(Brain* brain);
    
    // Perform winner-take-all competition among memory traces
    std::vector<NeuronId> selectWinners(const std::vector<NeuronId>& competitors);
    
    // Update attention scores based on memory importance
    void updateAttentionScores(NeuralWorkingMemory* workingMemory);
    
    // Get attention-based importance for a neuron
    float getAttentionImportance(NeuronId neuron) const;
    
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    Brain* brain_;
    
    // Competition parameters
    float winnerTakeAllThreshold_;
    float globalInhibition_;
    float attentionDecayRate_;
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