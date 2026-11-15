#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Implemented: Enhances alertness and modulates attention
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Signal arousal or alertness events
    void signalArousal(float intensity);
    
    // Vigilance effects
    float getArousalRate() const;
    void setArousalRate(float rate);
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
