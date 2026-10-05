#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood, impulse control, and social behavior

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    void signalReward(float reward);
    void signalPredictionError(float error);
    void updateMood(float predictedReward, float actualReward);
    
    // Behavioral inhibition
    float getBehavioralInhibition() const;
    
    // Integration with working memory
    void signalWorkingMemoryUpdate(float activation, float stability);
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void addToHistory(float value);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm