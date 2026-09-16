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

    // Integrate with attention system
    void integrateWithAttention(const std::vector<NeuronId>& attendedNeurons);

    // Update based on prediction error
    void updateWithPredictionError(float predictionError, TimestepDuration dt);

    // Store sensory input when prediction error is high
    void storeSensoryInputWithError(const std::vector<float>& sensoryInput, float predictionError);

    // Retrieve working memory content with attention modulation
    std::vector<float> retrieveWithAttention(float attentionLevel) const;

    // Connect working memory content to neuromodulation
    void connectToNeuromodulation(float neuromodulationLevel);

// Store planned action for execution
    void storePlanningAction(ActionType action) {
        plannedAction_ = action;
        plannedActionTime_ = 0; // Use current time in implementation
    }
    
    // Get last planned action
    ActionType getPlannedAction() const { return plannedAction_; }
    
    // Clear planned action
    void clearPlanningAction() {
        plannedAction_ = ActionType::Wait;
    }
    
    // Check if has planned action
    bool hasPlannedAction() const { return plannedAction_ != ActionType::Wait; }
    // Create recurrent connection for maintenance
    void createRecurrentConnection(NeuronId from, NeuronId to, float strength);

    // Update recurrent connections for maintenance
    void updateRecurrentConnections();

    // Decay weak memory traces
    void decayWeakTraces();

    // Advanced memory management
    void prioritizeMemory();
    void compressMemory();
    void replayStrengthenMemories();
    void updateMemoryPatterns();
    void consolidateMemorySleep();
    void resolveMemoryInterference();

    // Neural mechanisms
    void updateOscillatoryPatterns();
    void bindMemoryWithTemporalCoherence();
    void contextDependentRetrieval();
    void patternCompletion();

    // Attention integration
    void applyAttentionGating(const std::vector<NeuronId>& attendedNeurons);
    void encodeFocusedMemory(const std::vector<float>& pattern, float strength);
    void decodeFocusedMemory();

    // Neuromodulation enhancements
    void enhancedNeuromodulation(float neuromodulationLevel);

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    size_t capacity_;
    float decayRate_;

    // Memory content
    std::vector<NeuronId> memoryNeurons_;
    std::vector<float> memoryActivations_;
    std::vector<SimulationStep> memoryTimestamps_;
    std::vector<float> memoryImportance_;  // 0-1 importance score
    std::vector<MemoryPriority> memoryPriorities_;  // Priority enum

    // Active memory traces
    std::vector<size_t> activeTraces_;

    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections_;
    std::vector<std::vector<float>> memoryPatternCache_;

    // Winner neurons (for competition)
    std::vector<NeuronId> winners_;

    // Prediction error threshold for memory storage
    float predictionErrorThreshold_;

    // Advanced memory features
    SimulationStep memoryReplayCounter_;
    float consolidationTimer_;
    std::vector<NeuronId> temporalBindingGroups_;
    std::vector<MemoryContext> memoryContexts_;
    std::vector<MemoryPattern> patternLibrary_;
    float thetaPhase_;
    float gammaPhase_;
    std::vector<float> memorySlots_;
};

// Enums and structs for enhanced functionality
enum class MemoryPriority {
    IMPORTANT,
    RECENT,
    WEAK
};

struct MemoryContext {
    std::vector<NeuronId> involvedNeurons;
    SimulationStep timestamp;
    std::vector<float> contextualTags;
};

struct MemoryPattern {
    std::vector<float> patternData;
    std::vector<size_t> patternIndices;
    float patternStrength;
    SimulationStep lastReplay;
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