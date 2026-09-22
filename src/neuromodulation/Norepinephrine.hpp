#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Noradrenergic system implementation
// Integrates arousal, vigilance, and stress response

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const { return "NE"; }
    float getLevel() const { return level_; }
    void setLevel(float level);
    
    float getPlasticityFactor() const;
    void update(float dt);
    
    // Integration with other systems
    void integrateDopamine(float daLevel);
    void integrateAcetylcholine(float achLevel);
    void integrateSerotonin(float serotoninLevel);
    
    // Arousal modulation
    float getArousalLevel() const;
    void setArousalLevel(float arousal);
    
    // Vigilance control
    float getVigilance() const;
    void updateVigilance(float dt);
    
    // Stress response
    void triggerStress(float intensity);
    bool isUnderStress() const;
    
    // State management
    void reset();
    
private:
    float level_;
    float baseline_;
    float peak_;
    float decayRate_;
    float releaseRate_;
    
    // Arousal
    float arousalLevel_;
    float targetArousal_;
    
    // Vigilance
    float vigilance_;
    float lastStimulusTime_;
    
    // Stress
    bool underStress_;
    float stressIntensity_;
    float stressDuration_;
    
    // Integration weights
    float daWeight_;
    float achWeight_;
    float serotoninWeight_;
};

} // namespace nlm
