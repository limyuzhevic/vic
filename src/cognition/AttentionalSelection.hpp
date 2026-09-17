#pragma once

#include "../core/Types/Types.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

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
    
    // Integrate prediction with sensory processing
    void integratePredictionWithSensory(const PredictionSystem& predictionSystem,
                                        const SensoryInput& currentInput,
                                        std::vector<NeuronId>& attendedNeurons);
    
    // Apply concept formation to attended neurons
    void applyConceptFormation(const ConceptFormation& conceptFormation,
                               const std::vector<NeuronId>& attendedNeurons);
    
    // Store attended neurons in working memory
    void storeAttendedInWorkingMemory(NeuralWorkingMemory* workingMemory,
                                       const std::vector<NeuronId>& attendedNeurons);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<NeuronId> winners_;
    Brain* brain_;
};

} // namespace nlm