#pragma once

#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Norepinephrine: Arousal, vigilance, stress response, rapid eye movement
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation
    void signalArousal(float arousalSignal);
    float getArousalLevel() const;
    
    // Vigilance enhancement
    void increaseVigilance(float vigilanceIncrement);
    float getVigilanceLevel() const;
    
    // Stress response
    void signalStress(float stressLevel);
    float getStressLevel() const;
    
    // Rapid eye movement (REM) sleep regulation
    void enterREMState();
    void exitREMState();
    bool isInREMState() const;
    
    // Cognitive control enhancement
    void enhanceCognitiveControl(float controlStrength);
    float getCognitiveControlLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
