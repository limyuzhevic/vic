#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// AttentionalSelection: Focuses neural activity and allocates resources
// NOT awareness - selective enhancement of relevant neural activity
//
// Key mechanisms:
// - Competition between neuron populations (global inhibition)
// - Top-down bias from cognitive systems
// - Bottom-up salience from sensory input
// - Sustained attention through persistent activity
// - Resource allocation based on priority

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Process competition between candidate neurons
    // Returns neurons that win attentional competition
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                           float globalInhibition = 1.0f);

    // Apply attention to a specific region
    void focusOnRegion(RegionId region);

    // Release attention from current focus
    void releaseAttention();

    // Get currently attended regions
    std::vector<RegionId> getAttendedRegions() const;

    // Set inhibition strength (0-1, higher = more competition)
    void setInhibitionStrength(float strength) { inhibitionStrength_ = strength; }

    // Set excitation strength (0-1, higher = more facilitation)
    void setExcitationStrength(float strength) { excitationStrength_ = strength; }

    // Set competition threshold (activity level to enter competition)
    void setCompetitionThreshold(float threshold) { competitionThreshold_ = threshold; }

    // Get inhibition level for a neuron (0-1, lower = more attention)
    float getInhibitionFor(NeuronId neuron) const;

    // Get excitation level for a neuron (0-1, higher = more attention)
    float getExcitationFor(NeuronId neuron) const;

    // Update attention dynamics
    void update(TimestepDuration dt);

    // Check if a neuron is currently attended
    bool isAttended(NeuronId neuron) const;

    // Apply top-down bias (from cognitive systems)
    void applyTopDownBias(NeuronId neuron, float biasStrength);

    // Apply bottom-up salience (from sensory input)
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

    // Winners of competition
    std::vector<NeuronId> winners_;

    // Maps for top-down and bottom-up modulation
    std::unordered_map<uint64_t, float> topDownBias_;
    std::unordered_map<uint64_t, float> bottomUpSalience_;
};

} // namespace nlm