#pragma once

#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

// BrainPlasticityManager manages plasticity rules
class BrainPlasticityManager {
public:
    BrainPlasticityManager() = default;
    ~BrainPlasticityManager() = default;
    
    // Initialize plasticity systems
    void initialize(Brain* brain);
    
    // Apply plasticity rules
    void applyPlasticity(Brain* brain, Timestamp currentTime, TimestepDuration timestep);
    
    // Update synapse states
    void updateSynapseStates(Brain* brain, Timestamp currentTime);
    
    // Apply structural plasticity
    void applyStructuralPlasticity(Brain* brain, SimulationStep currentStep);
    
    // Get plasticity statistics
    void collectPlasticityStats(Brain* brain) const;
};

} // namespace nlm
