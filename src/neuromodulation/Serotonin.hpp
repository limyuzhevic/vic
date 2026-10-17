#pragma once

#include "Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <string>

namespace nlm {

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Social and mood effects
    void signalPositiveSocialExperience(float reward, float socialCohesion);
    void signalSocialConflict(float isolation, float punishment);
    
    // Get current effects
    float getMoodLevel() const;
    float getSocialEngagementLevel() const;
    float getImpulsivityLevel() const;
    
    // Signal processing
    float getTotalSignal() const;
    const std::vector<float>& getSocialHistory() const;
    
    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm