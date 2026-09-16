#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Social behavior modulation
    void signalSocialInteraction(float intensity);
    void setSocialLearningBias(float bias);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm