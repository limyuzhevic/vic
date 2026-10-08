#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// REAL IMPLEMENTATION - Norepinephrine regulates arousal, attention, and vigilance
// Biological roles:
// - Arousal: Increases alertness and responsiveness
// - Attention: Enhances signal-to-noise ratio in neural processing
// - Vigilance: Maintains focused attention over time
// - Stress response: Fight-or-flight activation
// - Memory modulation: Enhances consolidation of salient memories
// - Cognitive control: Balances exploration vs exploitation

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Get modulator name
    const char* getName() const { return "NE"; }
    
    // Get current concentration/level
    float getLevel() const { return pImpl->currentLevel; }
    void setLevel(float level) { pImpl->currentLevel = level; }
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    void setPlasticityFactor(float factor) { pImpl->plasticityFactor = factor; }
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Norepinephrine signaling
    void signalArousalChange(float arousalDelta);
    void signalAttentionNeed(float attentionDemand);
    void signalStressResponse(float stressLevel);
    void signalMemorySalience(float memorySalience);
    
    // State management
    void reset() {
        pImpl->currentLevel = 0.3f;
        pImpl->arousal = 0.3f;
        pImpl->vigilance = 0.5f;
        pImpl->attentionBias = 1.0f;
        pImpl->alertness = 0.5f;
        pImpl->plasticityFactor = 1.0f;
    }
    
    float getArousal() const { return pImpl->arousal; }
    float getVigilance() const { return pImpl->vigilance; }
    float getAttentionBias() const { return pImpl->attentionBias; }
    float getAlertness() const { return pImpl->alertness; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
