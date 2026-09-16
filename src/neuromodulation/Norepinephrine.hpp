#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Arousal modulation
    void increaseAlertness(float intensity);
    void enhanceWorkingMemory(float factor);
    void modulateSensoryProcessing(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm