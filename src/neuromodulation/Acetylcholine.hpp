#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// REAL IMPLEMENTATION - Acetylcholine regulates attention and memory consolidation
// Biological roles:
// - Attention: Enhances signal-to-noise ratio, focuses attention
// - Memory consolidation: Promotes long-term potentiation
// - Cortical activation: Increases cortical arousal
// - Learning: Facilitates acquisition of new information
// - REM sleep: Regulates sleep cycles
// - Cognitive flexibility: Enables shifting between tasks/behaviors

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Get modulator name
    const char* getName() const { return "ACh"; }
    
    // Get current concentration/level
    float getLevel() const { return pImpl->currentLevel; }
    void setLevel(float level) { pImpl->currentLevel = level; }
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const;
    void setPlasticityFactor(float factor) { pImpl->plasticityFactor = factor; }
    
    // Update neuromodulator state
    void update(TimestepDuration dt);
    
    // Acetylcholine signaling
    void signalAttentionFocus(float focusStrength);
    void signalMemoryConsolidation(float memoryPriority);
    void signalLearningReadiness(float learningReadiness);
    void signalCorticalActivation(float activationLevel);
    
    // State management
    void reset() {
        pImpl->currentLevel = 0.6f;
        pImpl->attentionStrength = 0.7f;
        pImpl->memoryConsolidationRate = 0.8f;
        pImpl->learningReadiness = 0.5f;
        pImpl->corticalActivation = 0.6f;
        pImpl->plasticityFactor = 1.0f;
    }
    
    float getAttentionStrength() const { return pImpl->attentionStrength; }
    float getMemoryConsolidationRate() const { return pImpl->memoryConsolidationRate; }
    float getLearningReadiness() const { return pImpl->learningReadiness; }
    float getCorticalActivation() const { return pImpl->corticalActivation; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
