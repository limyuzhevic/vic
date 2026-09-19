#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Neural maturation: development of adult-level properties
class Maturation {
public:
    Maturation();
    ~Maturation();
    
    // Get maturation progress (0-1)
    float getProgress() const;
    void setProgress(float progress);
    
    // Get maturation stage
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Update maturation
    void update(class Brain* brain, SimulationStep currentStep);
    
    // Get mature properties
    MembranePotential getMatureThreshold() const;
    MembranePotential getMatureRestingPotential() const;
    float getMatureTimeConstant() const;
    
    // Get mature potential state
    MaturationPotential getMaturePotential() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Mature neuron potential state
struct MaturationPotential {
    MembranePotential restingPotential;
    MembranePotential firingThreshold;
    float timeConstant;
    float plasticityFactor;
    
    MaturationPotential()
        : restingPotential(-70.0f)
        , firingThreshold(-55.0f)
        , timeConstant(20.0f)
        , plasticityFactor(1.0f) {}
};

} // namespace nlm
