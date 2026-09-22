#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Norepinephrine implementation with arousal regulation and stress response
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Arousal regulation
    void enhanceSensoryProcessing(float intensity, Brain* brain);
    
    // Stress response
    void signalStress(float stressMagnitude, float duration);
    
    // Vigilance
    float getVigilance() const;
    float getArousal() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
