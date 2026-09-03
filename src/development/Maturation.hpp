#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Neural maturation: development of adult-level properties
// PLACEHOLDER - Phase 2 will implement real maturation

class Maturation {
public:
    Maturation();
    ~Maturation();
    
    // Get maturation progress (0-1)
    float getProgress() const;
    void setProgress(float progress);
    
    // Update maturation
    void update(class Brain* brain, SimulationStep currentStep);
    
    // Get mature properties
    MembranePotential getMatureThreshold() const;
    MembranePotential getMatureRestingPotential() const;
    float getMatureTimeConstant() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
