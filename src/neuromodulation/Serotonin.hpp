#pragma once

#include "../core/Types/Types.hpp"

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
    void signalEvent(float magnitude = 1.0f) override;
    void reset() override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    
    // Mood and social modulation
    void signalSocialCohesion(float cohesionLevel);
    void signalRewardPunishment(float reward, float punishment);
    
    // Behavioral control
    void modulateImpulsivity(float controlLevel);
    void modulateAnhedonia(float anhedoniaLevel);
    
    // Get mood-related statistics
    float getMoodLevel() const;
    float getSocialCohesionLevel() const;
    float getImpulseControlLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
