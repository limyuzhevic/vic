#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Signals
    void signalMood(float mood);
    void signalSocial(float social);
    
    // Configuration
    void setBaseline(float baseline);
    void setTonicLevel(float tonic);
    
    // Integration modifiers
    void setMoodWeight(float weight);
    void setImpulsivityWeight(float weight);
    
    // Query methods
    bool isBurstMode() const;
    float getMoodWeight() const;
    float getImpulsivityWeight() const;
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm