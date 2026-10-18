#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Serotonin: Mood regulation, impulsivity control, and social behavior modulation
// 5-HT system regulates mood, behavioral inhibition, social bonding, sleep-wake cycles,
// appetitive behavior, cognitive flexibility, and pain processing

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Serotonin-specific functions
    
    // Compute mood state based on reward and threat inputs
    void computeMoodState(float rewardInput, float threatInput);
    
    // Generate behavioral inhibition signal
    void generateBehavioralInhibition(float currentBehaviorStrength, 
                                       std::vector<float>& inhibitionSignals);
    
    // Modulate social behavior based on interaction history
    void modulateSocialBehavior(float socialCohesion, float statusConflict);
    
    // Regulate sleep-wake cycle through circadian rhythm integration
    void regulateSleepWakeCycle(float timeOfDay, float arousalLevel);
    
    // Modulate reward sensitivity
    void modulateRewardSensitivity(float predictionError, float predictionConfidence);
    
    // Enhance cognitive flexibility
    void enhanceCognitiveFlexibility(const std::vector<float>& currentStrategy, 
                                    std::vector<float>& strategyAlternatives);
    
    // Modulate pain sensitivity
    void modulatePainSensitivity(float nociceptiveInput, float anxietyLevel);
    
    // Generate phasic response to social and reward events
    void generatePhasicResponse(const class Reward& reward,
                               const class Novelty& novelty,
                               const class PredictionError& predictionError,
                               const std::vector<float>& socialInput);
    
    // Apply state-dependent modulation of other neuromodulators
    void applyStateDependentModulation(class Dopamine& dopamine,
                                       class Norepinephrine& norepinephrine,
                                       class Acetylcholine& acetylcholine,
                                       class Novelty& novelty);
    
    // Homeostatic regulation to maintain balance
    void homeostaticRegulation(const std::vector<float>& serotoninHistory,
                               float targetLevel);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
