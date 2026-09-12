#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Attention modulation
    void enhanceSensoryProcessing(float& signalStrength) const;
    void modulateWorkingMemory(float& memoryTrace) const;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm