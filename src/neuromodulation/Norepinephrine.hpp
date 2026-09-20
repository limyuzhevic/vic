#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <cmath>

namespace nlm {

// Norepinephrine: Locus coeruleus firing modes (phasic/tonic), arousal regulation
// Implements both tonic baseline firing and phasic burst responses to salient stimuli
// Regulates arousal, attention, and stress responses

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Locus coeruleus firing modes
    void signalTonic();      // Tonic baseline firing (adenosine modulation)
    void signalPhasic();     // Phasic burst response (novelty/arousal)
    void updateArousal(float salience, float stressLevel);
    
    // Reuptake and clearance mechanisms
    void setReuptakeRate(float rate);
    void triggerVesicularRelease();
    
    // ADRB2 receptor dynamics for plasticity
    float getBetaAdrenergicEffect() const;
    float getAlphaAdrenergicEffect() const;
    
    // Arousal regulation parameters
    float getArousalLevel() const { return arousalLevel_; }
    float getStressLevel() const { return stressLevel_; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm