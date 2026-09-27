#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Acetylcholine (ACh): Working memory, attention, and memory consolidation
// Biologically inspired:
// - Basal forebrain cholinergic system modulates memory encoding
// - ACh enhances signal-to-noise ratio in hippocampal and cortical circuits
// - Facilitates working memory maintenance through persistent activity
// - Promotes memory consolidation during sleep/peaceful states
// - Reduces interference between competing memory traces

class AcetylCholine {
public:
    AcetylCholine();
    ~AcetylCholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get acetylcholine level (0-1)
    float getLevel() const;
    
    // Set level with clamping
    void setLevel(float level);
    
    // Get working memory enhancement factor (0-2)
    float getWorkingMemoryEnhancement() const;
    
    // Get attention clarity factor (0-1.5)
    float getAttentionClarity() const;
    
    // Update acetylcholine dynamics
    void update(float workingMemoryLoad, float novelty, TimestepDuration dt);
    
    // Signal memory encoding event
    void signalMemoryEncoding(const std::vector<float>& pattern);
    
    // Signal memory consolidation event
    void signalMemoryConsolidation(class SimulationStep step);
    
    // Get working memory modulation strength for neuron
    float getWorkingMemoryModulation(class NeuronId neuron) const;
    
    // Get attention gating strength for neuron
    float getAttentionGating(class NeuronId neuron) const;
    
    // Set working memory focus region
    void setWorkingMemoryFocus(class RegionId region, float strength = 1.0f);
    
    // Set attentional spotlight on region
    void setAttentionalSpotlight(class RegionId region, float strength = 1.0f);
    
    // Reset acetylcholine state
    void reset();
    
    // Get memory encoding rate
    float getMemoryEncodingRate() const { return encodingRate_; }
    
    // Get consolidation strength
    float getConsolidationStrength() const { return consolidationStrength_; }
    
    // Get attentional interference reduction
    float getAttentionalInterferenceReduction() const { return interferenceReduction_; }

private:
    struct Impl;
    Impl* pImpl;
    
    Brain* brain_;
    
    // Current cholinergic state
    float currentLevel_;
    float memoryEncodingRate_;
    float consolidationStrength_;
    float interferenceReduction_;
    
    // Working memory modulation
    float workingMemoryLoad_;
    float workingMemoryStability_;
    
    // Memory traces
    std::vector<float> recentPattern_;  // Last encoded pattern
    std::vector<std::pair<SimulationStep, std::vector<float>>> memoryTraces_;
    
    // Focus mechanisms
    std::vector<std::pair<class RegionId, float>> memoryFocusRegions_;
    std::vector<std::pair<class RegionId, float>> attentionalSpotlights_;
    
    // Physiological parameters
    float encodingSensitivity_;
    float consolidationThreshold_;
    float interferenceThreshold_;
    
    // Adaptive mechanisms
    float adaptationRate_;
    float fatigueRate_;
    
    // Decay and recovery
    float encodingDecayRate_;
    float consolidationDecayRate_;
};

} // namespace nlm