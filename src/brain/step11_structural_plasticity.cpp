// Step 11: Apply structural plasticity
// This module applies structural plasticity rules (synaptogenesis, pruning)
// Modifies neural connectivity based on activity patterns

#include "step11_structural_plasticity.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

namespace Step11 {

void ApplyDevelopmentalPlasticityModulation(Brain::Impl* impl,
                                            DevelopmentalStage stage) {
    if (!impl) {
        return;
    }
    
    auto* sp = impl->structuralPlasticity;
    if (!sp) {
        return;
    }
    
    float plasticityMod = 1.0f;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            plasticityMod = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityMod = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityMod = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityMod = 0.2f;  // Stable
            break;
    }
    
    sp->setSynaptogenesisRate(0.0001f * plasticityMod);
    sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
}

void ApplyStructuralPlasticityUpdates(Brain::Impl* impl,
                                     SimulationStep currentStep,
                                     TimestepDuration timestep) {
    if (!impl) {
        return;
    }
    
    auto* sp = impl->structuralPlasticity;
    if (!sp) {
        return;
    }
    
    // Development affects plasticity rates
    ApplyDevelopmentalPlasticityModulation(impl, impl->developmentalStage);
}

void ApplyStructuralPlasticity(Brain::Impl* impl,
                               SimulationStep currentStep,
                               Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("ApplyStructuralPlasticity called with null impl pointer");
        return;
    }
    
    NLM_LOG_DEBUG("Applying structural plasticity (Step " + std::to_string(currentStep) + ")");
    
    // Apply structural plasticity periodically
    if (currentStep % 100 == 0) {
        ApplyStructuralPlasticityUpdates(impl, currentStep, impl->timestep);
        
        if (impl->structuralPlasticity) {
            impl->structuralPlasticity->update(impl, *impl->rng);
        }
    }
}

} // namespace Step11

} // namespace nlm