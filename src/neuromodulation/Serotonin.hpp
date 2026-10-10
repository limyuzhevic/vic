#pragma once

#include "Neuromodulator.hpp"
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
    
    // Reward and punishment signaling
    void signalReward(float reward);
    void signalPunishment(float punishment);
    void increaseSocialEngagement(float sociality);
    
    // Mood and behavioral factors
    float getImpulsivityFactor() const;
    void setImpulsivityFactor(float factor);
    
    float getSocialFactor() const;
    void setSocialFactor(float factor);
    
    float getDominantMood() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm