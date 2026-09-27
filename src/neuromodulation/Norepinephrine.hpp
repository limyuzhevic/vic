#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine (NE): Arousal, vigilance, and attention modulation
// Biologically inspired:
// - Locus coeruleus neurons increase firing rate during novel events
// - NE enhances signal-to-noise ratio in cortical circuits
// - Modulates working memory and attentional focus
// - Reduces response variability

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get norepinephrine level (0-1)
    float getLevel() const;
    
    // Set level with clamping
    void setLevel(float level);
    
    // Get attention enhancement factor (0-2)
    float getAttentionEnhancement() const;
    
    // Get working memory benefit factor (0-1.5)
    float getWorkingMemoryBenefit() const;
    
    // Update norepinephrine dynamics based on sensory input
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Signal important event (arousal response)
    void signalEvent(float importance);
    
    // Signal sustained attention (focus)
    void signalSustainedAttention(float focusDuration);
    
    // Get attention allocation bias for specific neurons
    float getAttentionBias(class NeuronId neuron) const;
    
    // Set attentional focus on specific region
    void setFocusRegion(class RegionId region, float strength = 1.0f);
    
    // Reset norepinephrine state
    void reset();
    
    // Get current attentional load
    float getCurrentLoad() const { return attentionalLoad_; }
    
    // Get attentional stability (resistence to distraction)
    float getAttentionalStability() const { return attentionalStability_; }

private:
    struct Impl;
    Impl* pImpl;
    
    Brain* brain_;
    
    // Current attentional state
    float currentLevel_;
    float attentionalLoad_;
    float attentionalStability_;
    float attentionAllocation_[1024];  // One per neuron
    
    // Arousal dynamics
    float arousalBuffer_;
    float baselineLevel_;
    float peakLevel_;
    float decayRate_;
    float recoveryRate_;
    
    // Focus mechanisms
    std::vector<std::pair<class RegionId, float>> focusRegions_;
    
    // Event history for sustained attention
    std::vector<float> eventHistory_;
    
    // Physiological parameters
    float tonicNE_;  // Baseline NE level
    float phasicNE_;  // Phasic NE bursts
    
    // Integration parameters
    float noveltySensitivity_;
    float predictionErrorSensitivity_;
    
    // Adaptive mechanisms
    float adaptationRate_;
    float fatigueRate_;
};

} // namespace nlm