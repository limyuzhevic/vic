#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace nlm;

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

    // Get attention strength for a specific neuron
    float getAttentionStrength(NeuronId neuron) const {
        auto biasIt = topDownBias_.find(neuron.value);
        float bias = (biasIt != topDownBias_.end()) ? biasIt->second : 0.0f;
        auto salienceIt = bottomUpSalience_.find(neuron.value);
        float salience = (salienceIt != bottomUpSalience_.end()) ? salienceIt->second : 0.0f;
        return bias + salience;
    }

    // Integration with other brain systems
    
    // Prediction Integration
    void applyPredictionError(float error);
    void applyPredictionConfidence(float confidence);
    
    // Working Memory Integration
    void applyWorkingMemoryIntegration(const NeuralWorkingMemory* workingMemory);
    
    // Concept Formation Integration
    void applyConceptIntegration(const ConceptFormation* conceptFormation);
    
    // Neuromodulation Integration
    void applyNeuromodulationIntegration(const Dopamine* dopamine,
                                         const Acetylcholine* acetylcholine,
                                         const Norepinephrine* norepinephrine,
                                         const Serotonin* serotonin);
    
    // Replay Integration
    void applyReplayIntegration(const NeuralEpisodicMemory* episodicMemory,
                               const std::vector<NeuronId>& replayTargets);
    
    // Development Integration
    void applyDevelopmentIntegration(const DevelopmentSystem* development);
    
    // Feedback Loops
    void applyFeedbackLoops(const std::vector<NeuronId>& learningHistory);

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
