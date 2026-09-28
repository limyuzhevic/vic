#include "BrainDevelopment.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainDevelopment::updateDevelopmentSystem(Brain::Impl& pImpl, double timestep, SimulationStep currentStep) {
    try {
        // Validate Brain::Impl
        if (!pImpl->config) {
            NLM_LOG_WARNING("Failed to update development system: Configuration is null");
            return;
        }
        
        if (currentStep % 1000 == 0) {  // Update development every 1000 steps
            try {
                pImpl->developmentSystem->update(pImpl.get(), *pImpl->rng, timestep * 1000);
                
                // Development affects plasticity rates
                auto* sp = pImpl->structuralPlasticity.get();
                if (sp) {
                    DevelopmentalStage stage = pImpl->developmentalStage;
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
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to update development system: " + e.what());
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Exception during development system update: ") + e.what());
    }
}
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

void BrainDevelopment::logDevelopmentStats(const Brain::Impl& pImpl) const {
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(pImpl.developmentalStage)));
}

void BrainDevelopment::updateDevelopmentalStage(Brain::Impl& pImpl, double timestep) {
    pImpl->developmentalStage = pImpl->developmentSystem->update(pImpl.get(), *pImpl->rng, timestep);
}

void BrainDevelopment::applyDevelopmentToPlasticity(Brain::Impl& pImpl) {
    // Apply development effects to plasticity systems
    auto* sp = pImpl->structuralPlasticity.get();
    if (sp) {
        DevelopmentalStage stage = pImpl->developmentalStage;
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
}

void BrainDevelopment::applyDevelopmentToNeurons(Brain::Impl& pImpl) {
    // Apply development effects to neural properties
    // This would include changes in excitability, threshold, etc.
    // Based on developmental stage
    DevelopmentalStage stage = pImpl->developmentalStage;
    
    switch (stage) {
        case DevelopmentalStage::Initial:
            // Higher excitability, lower threshold
            // Development would make neurons more responsive
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Balanced excitability
            break;
        case DevelopmentalStage::Maturation:
            // More selective, lower overall excitability
            break;
        case DevelopmentalStage::Adult:
            // Stable properties
            break;
    }
}

} // namespace nlm
