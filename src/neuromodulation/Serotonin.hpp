#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulator: Serotonin - mood, impulsivity, and social behavior
// Real implementation - affects mood regulation and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal mood changes
    void signalMood(float moodLevel);
    
    // Signal social status changes
    void signalSocialStatus(float socialStatus);
    
    // Get current mood level
    float getMoodLevel() const;
    
    // Get social status
    float getSocialStatus() const;
    
    // Get impulsivity level
    float getImpulsivityLevel() const;
    
    // Get reward sensitivity
    float getRewardSensitivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Update internal neuromodulation state
    void updateInternalState();
    
    // Update social processing effects
    void updateSocialProcessing(float moodLevel);
    
    // Update reward sensitivity effects
    void updateRewardSensitivity(float socialStatus);
};

} // namespace nlm