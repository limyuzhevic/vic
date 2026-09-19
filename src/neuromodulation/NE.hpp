#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Norepinephrine: Arousal and vigilance neuromodulator
// Implements noradrenergic signaling for locus coeruleus modulation,
// attention filtering, and enhanced learning responses
// Reference: Aston-Jones & Cohen (2005), Sara (2009), Yu & Dayan (2005)
// Response to novel stimuli, predictable via classical conditioning,
// tonic baseline for arousal and vigilance

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level (0.0 = baseline, 1.0 = peak phasic)
    float getLevel() const override;
    
    // Set level with clamping (0.0 to 1.0 range)
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    // NE modulates attention filtering and learning enhancement
    // Stronger NE during novel or unexpected events enhances plasticity
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state with temporal dynamics
    void update(TimestepDuration dt) override;
    
    // Signal novel stimulus detection for phasic release
    void signalNovelty(float noveltyStrength);
    
    // Signal prediction error for learning enhancement
    void signalPredictionError(float predictionError);
    
    // Signal arousal response to salient events
    void signalArousal(float arousalLevel);
    
    // Signal stress response
    void signalStress(float stressLevel);
    
    // Get attention filtering strength
    float getAttentionFilterStrength() const;
    
    // Get arousal level
    float getArousalLevel() const;
    
    // Get learning enhancement factor
    float getLearningEnhancementFactor() const;
    
    // Reset to baseline
    void reset();
    
    // Set baseline activity level
    void setBaseline(float baseline);
    
    // Set decay rate for phasic signals
    void setDecayRate(float decayRate);
    
    // Set release rate for phasic responses
    void setReleaseRate(float releaseRate);
    
    // Set novelty detection sensitivity
    void setNoveltySensitivity(float sensitivity);
    
    // Set stress response intensity
    void setStressResponseIntensity(float intensity);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm