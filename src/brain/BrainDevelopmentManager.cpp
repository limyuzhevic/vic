#include "BrainDevelopmentManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm {

void BrainDevelopmentManager::initialize(Brain* brain) {
    // Development system initialization happens in BrainInitializer
}

void BrainDevelopmentManager::update(Brain* brain, SimulationStep currentStep, TimestepDuration timestep) {
    auto* pImpl = brain->pImpl.get();
    
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        if (auto* developmentSystem = brain->getDevelopmentSystem()) {
            developmentSystem->update(brain, *pImpl->rng, timestep * 1000);
        }
    }
}

void BrainDevelopmentManager::applyDevelopmentEffects(Brain* brain) {
    auto* pImpl = brain->pImpl.get();
    if (!pImpl || !pImpl->structuralPlasticity) return;
    
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
    
    pImpl->structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
    pImpl->structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
}

void BrainDevelopmentManager::collectDevelopmentStats(Brain* brain) const {
    if (!brain) return;
    
    auto* pImpl = brain->pImpl.get();
    
    // Development statistics will be collected in logStatus()
}

} // namespace nlm
