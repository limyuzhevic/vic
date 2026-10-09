#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <vector>
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Real maturation implementation with activity-dependent refinement
class Maturation {
public:
    Maturation();
    ~Maturation();
    
    // Get maturation progress (0-1)
    float getProgress() const { return progress_; }
    void setProgress(float progress) { progress_ = std::max(0.0f, std::min(progress, 1.0f)); }
    
    // Get maturation rate
    float getMaturationRate() const { return maturationRate_; }
    void setMaturationRate(float rate) { maturationRate_ = std::max(0.0f, rate); }
    
    // Get critical period duration
    double getCriticalPeriodDuration() const { return criticalPeriodDuration_; }
    void setCriticalPeriodDuration(double duration) { criticalPeriodDuration_ = std::max(0.0f, duration); }
    
    // Get activity refinement strength
    float getActivityRefinementStrength() const { return activityRefinementStrength_; }
    void setActivityRefinementStrength(float strength) { activityRefinementStrength_ = std::max(0.0f, std::min(strength, 1.0f)); }
    
    // Update maturation with biological realism
    void update(Brain* brain, SimulationStep currentStep);
    
    // Refine neuron properties based on activity
    void refineNeuronProperties(Neuron* neuron, Brain* brain, float activityLevel);
    
    // Develop mature properties from immature ones
    void developMatureProperties(Brain* brain);
    
    // Apply activity-dependent maturation
    void applyActivityDependentMaturation(Brain* brain, float globalActivityLevel);
    
    // Get mature properties
    MembranePotential getMatureThreshold() const;
    MembranePotential getMatureRestingPotential() const;
    float getMatureTimeConstant() const;
    
    // Get statistics
    size_t getNeuronsRefined() const { return neuronsRefined_; }
    float getMaturationEfficiency() const { return maturationEfficiency_; }
    
    // Reset counters
    void resetCounters() {
        neuronsRefined_ = 0;
        maturationEfficiency_ = 0.0f;
    }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float progress_;
    float maturationRate_;
    double criticalPeriodDuration_;
    float activityRefinementStrength_;
    size_t neuronsRefined_;
    float maturationEfficiency_;
};

} // namespace nlm