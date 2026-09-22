#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Serotonin: Mood, social behavior, and regulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Regulate mood and social behavior
    void signalRewardOutcome(float reward, float predictionError);
    void enhanceSocialLearning(float socialCohesion);
    
    // Get modulation effects
    float getMoodFactor() const;
    float getImpulsivityLevel() const;
    float getSocialBehaviorModulator() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm