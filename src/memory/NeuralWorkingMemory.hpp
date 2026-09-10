#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
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

        // Store an action in working memory for planning integration
    void storeAction(ActionType action, SimulationStep step);
    
    // Retrieve planned action if available
    ActionType retrievePlannedAction() const;
    
    // Get step when action was planned
    SimulationStep getPlannedActionStep() const;
    
    // Clear planned action
    void clearPlannedAction();

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

    // Store current concept ID for other systems
    void setCurrentConcept(size_t conceptId) { currentConceptId_ = conceptId; }

    // Get current concept ID
    size_t getCurrentConceptId() const { return currentConceptId_; }

    // Check if concept formation should influence memory competition
    void applyConceptBiasToCompetition();

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

    // Planned action storage
    ActionType plannedAction_;
    SimulationStep plannedActionStep_;

    // Current concept ID for integration
    size_t currentConceptId_;
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