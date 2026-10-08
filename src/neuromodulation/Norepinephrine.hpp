#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getConcentration() const;
    float getBaselineConcentration() const;
    float getHomeostaticSetPoint() const;
    bool isHomeostaticallyRegulated() const;
    void resetToBaseline();
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Locus coeruleus firing dynamics
    void updateLocusCoeruleusFiring(float arousal, float novelty, float stressLevel);
    void modulateSignalToNoiseRatio(float baselineSNR, float neInfluence);
    
    // Arousal-dependent gain modulation
    void updateArousalState(float environmentalStimuli, float internalState);
    void modulateAttentionalFocus(float attentionThreshold, float neModulation);
    
    // Stress response and homeostasis
    void triggerStressResponse(float stressorIntensity, float duration);
    void updateStressRecovery(float recoveryRate);
    void setStressSetPoint(float setpoint);
    
    // NE-dependent plasticity
    void modulateSynapticPlasticity(float baselinePlasticity, float neInfluence);
    void updateReceptorSensitivity(float sensitivity, float adaptationRate);
    
    // Cross-modulation
    void modulateWithDopamine(float daLevel);
    void modulateWithAcetylcholine(float achLevel);
    void modulateWithSerotonin(float fiveHtLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm