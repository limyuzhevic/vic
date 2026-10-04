#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm {

// AttentionalMemoryState: Current attentional context for working memory
struct AttentionalMemoryState {
    // Attentional strength (0-1)
    float attentionStrength;
    
    // Currently attended neuronal populations
    std::vector<NeuronId> attendedNeurons;
    
    // Attentional bias towards specific memory traces
    std::unordered_map<NeuronId, float> attentionalBias;
    
    // Competition level from attentional selection
    float competitionLevel;
    
    // Inhibition strength applied by attention
    float inhibitionStrength;
    
    // Excitatory enhancement from attention
    float excitationStrength;
    
    // Attentional predictions for memory updating
    std::vector<float> attentionalPredictions;
    
    // Time since attentional shift
    SimulationStep attentionalAge;
    
    // Neuromodulatory influence
    float neuromodulatoryGain;
    
    AttentionalMemoryState()
        : attentionStrength(0.0f), competitionLevel(0.5f), inhibitionStrength(0.0f),
          excitationStrength(0.0f), attentionalAge(0), neuromodulatoryGain(1.0f) {}
};

// NeuralWorkingMemory: Implements working memory using neural dynamics
// NOT a simple storage vector - uses persistent activity and recurrent connections
//
// Key mechanisms:
// - Persistent neural activity (reverberating activity)
// - Recurrent synaptic connections for maintaining information
// - Activity-dependent short-term plasticity
// - Neural competition for selective retention
//
// INTEGRATION ATTRIBUTES:
// - Attentional competition modulation for selective retention
// - Attentional focus enhancement of memory maintenance
// - Attentional-based memory strengthening
// - Neuromodulatory control of attentional influence on memory
// - Integration with concept formation for memory binding

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

    // Update working memory (maintenance and decay) with attentional modulation
    void update(TimestepDuration dt, const AttentionalMemoryState& attentionalState);

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

    // Competition between memory traces with attentional modulation
    void runCompetition(const AttentionalMemoryState& attentionalState);

    // Is this neuron part of the winning population?
    bool isWinning(NeuronId neuron) const;

    // Get overall memory activity level
    float getMemoryActivity() const;

    // ATTRIBUTION MODULATION METHODS

    // Update attentional competition modulation
    void updateWithAttentionalState(const AttentionalMemoryState& attentionalState);

    // Enhance memory maintenance based on attentional focus
    void enhanceMemoryBasedOnAttention(const AttentionalMemoryState& attentionalState);

    // Apply attentional-based memory strengthening
    void applyAttentionalStrengthening(const AttentionalMemoryState& attentionalState);

    // Get attentional modulation factors
    float getAttentionalModulation() const;

    // Set attentional parameters
    void setAttentionalParameters(float strength, float bias);

    // Get current attentional state
    AttentionalMemoryState getCurrentAttentionalState() const;

private:
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    // Update recurrent connections for maintenance with attentional modulation
    void updateRecurrentConnections(const AttentionalMemoryState& attentionalState);

    // Decay weak memory traces with attentional influence
    void decayWeakTraces(const AttentionalMemoryState& attentionalState);

    // Apply attentional enhancement to winning traces
    void applyAttentionalEnhancement(const AttentionalMemoryState& attentionalState);

    // Apply attentional bias to memory selection
    void applyAttentionalBias(const AttentionalMemoryState& attentionalState);

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
    
    // Attentional modulation
    float attentionalStrength_;
    float attentionalBias_;
};;

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