#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Serotonin: Mood, learning rate, behavioral inhibition
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Learning rate modulation
    void modulateLearningRate(float rate);
    float getLearningRate() const;
    
    // Behavioral inhibition
    void modulateBehavioralInhibition(float inhibition);
    float getBehavioralInhibition() const;
    
    // Mood regulation
    void updateMood(float rewardHistory, float socialContext);
    float getMood() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

}