#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace nlm {

// AttentionalSelection: Focuses on relevant neural activity through competition
// NOT consciousness - simple attention mechanism for selective processing
//
// Key mechanisms:
// - Neural competition through mutual inhibition
// - Global inhibition to limit active neurons
// - Excitatory drive from important inputs
// - Dynamic attention allocation based on stimulus salience

class AttentionalSelection {
public:
    AttentionalSelection();
    ~AttentionalSelection();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Process competition among neurons
    // competitors: vector of neural activity values
    // globalInhibition: strength of global inhibition signal
    float process_competition(const std::vector<float>& competitors,
                              float globalInhibition);
    
    // Focus attention on a specific region
    void focus_on_region(const std::string& regionId);
    
    // Release current attention focus
    void release_attention();
    
    // Get currently attended regions
    std::vector<std::string> get_attended_regions() const;
    
    // Set competition parameters
    void set_inhibition_strength(float strength);
    void set_excitation_strength(float strength);
    
    // Set competition threshold
    void set_competition_threshold(float threshold);
    
    // Get inhibition for a specific neuron
    float get_inhibition_for(size_t neuronId) const;
    
    // Get excitation for a specific neuron
    float get_excitation_for(size_t neuronId) const;
    
    // Update attentional dynamics
    void update(TimestepDuration dt);
    
    // Check if a neuron is currently attended
    bool is_attended(size_t neuronId) const;
    
    // Get winning neurons from last competition
    std::vector<size_t> get_winners() const;
    
    // Apply top-down bias to a neuron
    void apply_top_down_bias(size_t neuronId, float biasStrength);
    
    // Apply bottom-up salience to a neuron
    void apply_bottom_up_salience(size_t neuronId, float salienceStrength);
    
    // Reset attentional state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm