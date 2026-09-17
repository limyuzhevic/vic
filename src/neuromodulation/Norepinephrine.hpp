#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Real implementation - NE modulates arousal, alertness, and working memory
// Key mechanisms:
// - Responds to salient events (threat, reward, novelty)
// - Modulates arousal levels and vigilance
// - Enhances working memory encoding for salient information
// - Reduces during relaxation/sleep

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Arousal signaling
    void signalArousal(float arousalLevel);
    void signalStress(float stressLevel);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm