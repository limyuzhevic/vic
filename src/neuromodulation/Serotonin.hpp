#pragma once

#include "Neuromodulator.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced implementation with social mechanisms
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Social mechanisms
    void signalSocialReward(float socialValue);
    void signalSocialThreat(float threat);
    void setSocialThreshold(float threshold);
    
    // State access
    float getMoodLevel() const { return pImpl->moodLevel; }
    float getSocialValue() const { return pImpl->socialValue; }
    float getSocialThreat() const { return pImpl->socialThreat; }
    float getImpulsivity() const { return pImpl->impulsivity; }
    float getAnxietyLevel() const { return pImpl->anxietyLevel; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
