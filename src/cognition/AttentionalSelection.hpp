#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// AttentionalSelection: Selective focus and resource allocation
// Selects and prioritizes information processing based on relevance and salience
// NOT consciousness - competitive attention allocation
//
// Key mechanisms:
// - Competitive lateral inhibition among neurons
// - Salience-based priority modulation
// - Resource allocation to high-value inputs
// - Dynamic attentional shifts

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update attentional selection based on neural activity and neuromodulation
    void update(TimestepDuration dt);
    
    // Process competition among neural signals
    void processCompetition(const std::vector<NeuronId>& competitors);
    
    // Allocate attention to high-salience signals
    void allocateAttention(NeuronId neuron, float salience, float stimulusStrength);
    
    // Get currently attended neurons
    const std::vector<NeuronId>& getAttendedNeurons() const { return attendedNeurons_; }
    
    // Get attention priority for a neuron
    float getAttentionPriority(NeuronId neuron) const;
    
    // Get overall attentional focus level
    float getAttentionalFocus() const { return attentionalFocus_; }
    
    // Set attentional parameters
    void setInhibitionStrength(float strength) { inhibitionStrength_ = strength; }
    void setExcitationStrength(float strength) { excitationStrength_ = strength; }
    void setPriorityThreshold(float threshold) { priorityThreshold_ = threshold; }
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Currently attended neurons
    std::vector<NeuronId> attendedNeurons_;
    
    // Attentional state
    float attentionalFocus_;
    float inhibitionStrength_;
    float excitationStrength_;
    float priorityThreshold_;
    
    // Salience tracking
    std::vector<float> neuronSalience_;
    std::vector<float> attentionPriority_;
    
    // Dynamic attention shifts
    float currentFocusArea_;
    float attentionShiftSpeed_;
    float stimulusRelevance_;
};

} // namespace nlm