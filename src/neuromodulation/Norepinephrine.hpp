#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Implemented: Modulates arousal, stress response, and attention breadth
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
    void signalArousal(float arousal);
    void signalStress(float stress);
    void signalRecovery();
    
    // Accessors
    float getArousalLevel() const;
    float getLearningRateBoost() const;
    float getAttentionBroadening() const;
    float getStressLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm