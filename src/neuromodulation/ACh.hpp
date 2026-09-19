#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine: Attention and memory consolidation neuromodulator
// Implements cholinergic signaling for memory formation, attentional gain, 
// and cortical plasticity modulation
// Reference: Mesulam (1999), Hasselmo (1995), Sarrechat (2015)
// Phasic release during attention/learning events, tonic baseline for arousal

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level (0.0 = baseline, 1.0 = peak phasic)
    float getLevel() const override;
    
    // Set level with clamping (0.0 to 1.0 range)
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    // ACh enhances plasticity during attention and working memory tasks
    // Strong attention yields higher plasticity factors
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state with temporal dynamics
    void update(TimestepDuration dt) override;
    
    // Signal attention to salient stimuli for phasic release
    void signalAttention(float salience, float novelty = 0.0f);
    
    // Signal memory encoding/retrieval events
    void signalMemoryEncoding(float memoryStrength);
    void signalMemoryRetrieval(float memoryStrength);
    
    // Signal working memory activation
    void signalWorkingMemoryUpdate(float activeMemorySpan);
    
    // Get attention gain modulation factor
    float getAttentionGain() const;
    
    // Get memory consolidation factor
    float getMemoryConsolidationFactor() const;
    
    // Get cortical plasticity enhancement factor
    float getPlasticityEnhancementFactor() const;
    
    // Reset to baseline
    void reset();
    
    // Set baseline activity level
    void setBaseline(float baseline);
    
    // Set decay rate for phasic signals
    void setDecayRate(float decayRate);
    
    // Set release rate for phasic responses
    void setReleaseRate(float releaseRate);
    
    // Set attention sensitivity parameter
    void setAttentionSensitivity(float sensitivity);
    
    // Set memory modulation strength
    void setMemoryModulationStrength(float strength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm