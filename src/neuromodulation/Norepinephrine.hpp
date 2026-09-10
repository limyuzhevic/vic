#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal and vigilance neuromodulator
// Phase 2: Arousal, vigilance, and stress response integration

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance effects
    void signalAttentionDemand(float demand);
    void signalStress(float stressLevel);
    void signalNovelty(float novelty);
    
    // Get internal states
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getStressLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm