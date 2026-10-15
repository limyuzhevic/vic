#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Implements 5-HT signaling for mood regulation, impulsivity control, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal mood and social effects
    void signalMood(float mood);
    void signalReward(float reward);
    void signalPunishment(float punishment);
    void signalSocialBonding(float bonding);
    
    // Get neuromodulation effects
    float getMoodLevel() const;
    float getImpulsivityLevel() const;
    float getSocialCohesion() const;
    float getRewardSensitivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
