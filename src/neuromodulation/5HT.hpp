#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Serotonin: Mood regulation and social behavior neuromodulator
// Implements serotonergic signaling for raphe nucleus modulation,
// mood regulation, decision making, and behavioral flexibility
// Reference: Berridge & Robinson (1998), Walton & Luckhurst (2020),
// Peigneux et al. (2003). Slow, diffuse modulation across brain regions.

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level (0.0 = baseline, 1.0 = peak phasic)
    float getLevel() const override;
    
    // Set level with clamping (0.0 to 1.0 range)
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    // 5-HT modulates mood, decision making, and behavioral flexibility
    // Higher serotonin levels stabilize mood and promote flexible decision making
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state with temporal dynamics
    void update(TimestepDuration dt) override;
    
    // Signal reward processing for mood regulation
    void signalReward(float rewardOutcome);
    
    // Signal social feedback processing
    void signalSocialFeedback(float socialFeedback);
    
    // Signal impulsive decision attempt
    void signalImpulsivity(float impulsivityLevel);
    
    // Signal sleep/wake cycle modulation
    void signalSleepModulation(float sleepPressure);
    
    // Get mood regulation factor
    float getMoodRegulationFactor() const;
    
    // Get decision making flexibility factor
    float getDecisionMakingFlexibilityFactor() const;
    
    // Get impulsivity modulation factor
    float getImpulsivityModulationFactor() const;
    
    // Reset to baseline
    void reset();
    
    // Set baseline activity level
    void setBaseline(float baseline);
    
    // Set decay rate for phasic signals
    void setDecayRate(float decayRate);
    
    // Set release rate for phasic responses
    void setReleaseRate(float releaseRate);
    
    // Set mood regulation sensitivity
    void setMoodRegulationSensitivity(float sensitivity);
    
    // Set social feedback sensitivity
    void setSocialFeedbackSensitivity(float sensitivity);
    
    // Set impulsivity modulation strength
    void setImpulsivityModulationStrength(float strength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm