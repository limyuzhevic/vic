// Step 13: Apply development effects
// This module applies developmental changes over time
// Manages developmental stage progression and maturation

#include "step13_apply_development.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

namespace Step13 {

void ApplyDevelopmentEffects(Brain::Impl* impl,
                             SimulationStep currentStep,
                             Timestamp currentTime,
                             Timestamp cumulativeTime) {
    if (!impl) {
        return;
    }
    
    // Apply development every 1000 steps
    if (currentStep % 1000 == 0) {
        if (impl->developmentSystem) {
            impl->developmentSystem->update(impl, *impl->rng, cumulativeTime);
            
            // Development affects plasticity rates
            auto* sp = impl->structuralPlasticity;
            if (sp) {
                float plasticityMod = 1.0f;
                
                switch (impl->developmentalStage) {
                    case DevelopmentalStage::Initial:
                        plasticityMod = 1.0f;
                        break;
                    case DevelopmentalStage::CriticalPeriod:
                        plasticityMod = 0.8f;
                        break;
                    case DevelopmentalStage::Maturation:
                        plasticityMod = 0.5f;
                        break;
                    case DevelopmentalStage::Adult:
                        plasticityMod = 0.2f;
                        break;
                }
                
                sp->setSynaptogenesisRate(0.0001f * plasticityMod);
                sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
            }
        }
    }
}

void UpdateDevelopmentalStage(Brain::Impl* impl,
                              SimulationStep currentStep,
                              Timestamp cumulativeTime) {
    if (!impl) {
        return;
    }
    
    // Update developmental stage based on accumulated time
    if (cumulativeTime >= 10.0f && impl->developmentalStage == DevelopmentalStage::Initial) {
        impl->developmentalStage = DevelopmentalStage::CriticalPeriod;
    } else if (cumulativeTime >= 30.0f && impl->developmentalStage == DevelopmentalStage::CriticalPeriod) {
        impl->developmentalStage = DevelopmentalStage::Maturation;
    } else if (cumulativeTime >= 60.0f && impl->developmentalStage == DevelopmentalStage::Maturation) {
        impl->developmentalStage = DevelopmentalStage::Adult;
    }
}

void ApplyDevelopment(Brain::Impl* impl,
                      SimulationStep currentStep,
                      Timestamp currentTime) {
    if (!impl) {
        NLM_LOG_ERROR("ApplyDevelopment called with null impl pointer");
        return;
    }
    
    NLM_LOG_DEBUG("Applying development effects (Step " + std::to_string(currentStep) + ")");
    
    Timestamp cumulativeTime = currentTime;
    ApplyDevelopmentEffects(impl, currentStep, currentTime, cumulativeTime);
    UpdateDevelopmentalStage(impl, currentStep, cumulativeTime);
}

} // namespace Step13

} // namespace nlm