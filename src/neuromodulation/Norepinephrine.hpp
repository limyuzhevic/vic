#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Signals
    void signalArousal(float arousal);
    void signalVigilance(float vigilance);
    
    // Configuration
    void setBaseline(float baseline);
    void setTonicLevel(float tonic);
    
    // Integration modifiers
    void setArousalWeight(float weight);
    void setVigilanceWeight(float weight);
    
    // Query methods
    bool isBurstMode() const;
    float getArousalWeight() const;
    float getVigilanceWeight() const;
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm