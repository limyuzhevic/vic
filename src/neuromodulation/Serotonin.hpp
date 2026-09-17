#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation - 5-HT modulates mood, punishment sensitivity, and social behavior
// Key mechanisms:
// - Responds to reward prediction errors (negative errors)
// - Modulates impulsivity and risk-taking
// - Influences social behavior and acceptance
// - regulates mood and anxiety

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Mood and social signaling
    void signalReward(float reward);
    void signalPunishment(float punishment);
    void signalSocialFeedback(float socialFeedback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm