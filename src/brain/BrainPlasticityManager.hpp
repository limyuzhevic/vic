#pragma once

#include "Brain.hpp"
#include "../src/dynamics/SpikeSystem.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include <memory>

namespace nlm {

class BrainPlasticityManager {
public:
    BrainPlasticityManager();
    ~BrainPlasticityManager();
    
    // Disable copying
    BrainPlasticityManager(const BrainPlasticityManager&) = delete;
    BrainPlasticityManager& operator=(const BrainPlasticityManager&) = delete;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Apply STDP plasticity
    void applySTDP(Brain* brain, const std::vector<SpikeEvent>& preSpikes, 
                   const std::vector<SpikeEvent>& postSpikes, Timestamp currentTime);
    
    // Apply Hebbian plasticity
    void applyHebbian(Brain* brain, const std::vector<SpikeEvent>& preSpikes, 
                      const std::vector<SpikeEvent>& postSpikes, Timestamp currentTime);
    
    // Apply structural plasticity periodically
    void applyStructuralPlasticity(Brain* brain, SimulationStep currentStep, RandomGenerator& rng);
    
    // Get plasticity system pointers
    STDP* getSTDP() { return stdp.get(); }
    Hebbian* getHebbian() { return hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
