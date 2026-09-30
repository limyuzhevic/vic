// AttentionalSelection.hpp
#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <map>
#include <set>
#include <string>

namespace nlm {

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Process competition between neurons for attention selection
    std::vector<NeuronId> processCompetition(const std::vector<NeuronId>& competitors,
                                           float globalInhibition);
    
    // Focus attention on a specific region
    void focusOnRegion(RegionId region);
    
    // Release attention from all regions
    void releaseAttention();
    
    // Get list of attended regions
    std::vector<RegionId> getAttendedRegions() const;
    
    // Set inhibition strength for attention
    void setInhibitionStrength(float strength);
    
    // Set excitation strength for attention
    void setExcitationStrength(float strength);
    
    // Set competition threshold
    void setCompetitionThreshold(float threshold);
    
    // Get inhibition level for a neuron
    float getInhibitionFor(NeuronId neuron) const;
    
    // Get excitation level for a neuron
    float getExcitationFor(NeuronId neuron) const;
    
    // Update attention over time
    void update(TimestepDuration dt);
    
    // Reset attention state
    void reset();
    
    // Check if neuron is being attended to
    bool isAttended(NeuronId neuron) const;
    
    // Apply top-down bias to a neuron
    void applyTopDownBias(NeuronId neuron, float biasStrength);
    
    // Apply bottom-up salience to a neuron
    void applyBottomUpSalience(NeuronId neuron, float salienceStrength);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper data for attention computation
    std::map<size_t, float> bottomUpSalience_;  // Bottom-up salience by neuron
    std::map<size_t, float> topDownBias_;      // Top-down bias by neuron
    std::set<size_t> attendedRegions_;        // Regions currently attended to
    std::vector<NeuronId> winners_;           // Neurons selected as attention winners
    
    // Attention parameters
    float inhibitionStrength_;
    float excitationStrength_;
    float competitionThreshold_;
};

} // namespace nlm