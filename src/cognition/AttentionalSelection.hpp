#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// AttentionalSelection: Focuses processing on salient information
// NOT attention as consciousness - selective enhancement of neural activity
//
// Key mechanisms:
// - Bottom-up salience detection based on novelty
// - Top-down goal-based selection
// - Competition between neural pathways
// - Gating of sensory processing

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Apply bottom-up salience (higher = more attention)
    void applyBottomUpSalience(NeuronId neuron, float salience);

    // Apply top-down selection based on goals
    void applyTopDownSelection(NeuronId neuron, float weight);

    // Process competition between neural pathways
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors);

    // Update attention based on neuromodulation
    void update(double timestep);

    // Get current attention weights
    float getInhibitionStrength() const { return inhibitionStrength_; }
    float getExcitationStrength() const { return excitationStrength_; }

    // Set attention parameters
    void setInhibitionStrength(float strength) { inhibitionStrength_ = strength; }
    void setExcitationStrength(float strength) { excitationStrength_ = strength; }

    // Reset attention state
    void reset();

    // Has attention been applied?
    bool hasAttentionBeenApplied() const { return !attentionWeights_.empty(); }

private:
    // Compute attention score for a neuron
    float computeAttentionScore(NeuronId neuron) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
    float inhibitionStrength_;
    float excitationStrength_;
    
    // Current attention weights
    std::vector<std::pair<NeuronId, float>> attentionWeights_;
};

} // namespace nlm
