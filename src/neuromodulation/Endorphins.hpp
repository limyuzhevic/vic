#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Endorphins: Pain modulation, stress response, reward-reward balance
// Mediates stress adaptation and social bonding
class Endorphins : public Neuromodulator {
public:
    Endorphins();
    ~Endorphins() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Pain modulation (analgesic effect)
    void reducePain(float painLevel);
    void blockPainSignals(float blockStrength);
    
    // Stress adaptation
    void signalStress(float stressLevel);
    void facilitateStressRecovery();
    
    // Social bonding and reward balance
    void facilitateSocialBonding(float socialReward);
    void modulateRewardValue(float rewardValue);
    
    // Exercise-induced analgesia
    void registerExerciseActivity(float duration, float intensity);
    
    // Opioid receptor dynamics
    void releaseEndorphins(float magnitude);
    void bindToReceptors(float receptorAffinity);
    
    // Homeostatic balance restoration
    void restoreHomeostaticBalance();
    
    // Reset to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
