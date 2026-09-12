#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

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
    void increaseVigilance(float& alertness) const;
    void enhanceResponseSelection(float& responseBias) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm