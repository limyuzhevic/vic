#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, punishment, and learning rate modulation
// Based on computational models of serotonin signaling (Sharp et al. 2015)
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Serotonin modulates learning rate and punishment sensitivity
    void signalPunishment(float punishment);
    void signalSocialFeedback(float feedback);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm