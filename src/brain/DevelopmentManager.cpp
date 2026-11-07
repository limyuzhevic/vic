#include "DevelopmentManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

DevelopmentManager::DevelopmentManager(Brain* brain)
    : brain_(brain)
    , developmentSystem(std::make_unique<DevelopmentSystem>()) {
}

DevelopmentManager::~DevelopmentManager() = default;

bool DevelopmentManager::initialize() {
    NLM_LOG_INFO("Initializing Development Manager...");
    
    if (!brain_) return false;
    
    // Initialize development system
    developmentSystem->initialize(brain_);
    
    NLM_LOG_INFO("Development Manager initialized successfully");
    return true;
}

void DevelopmentManager::reset() {
    // Development system maintains state across episodes
}

void DevelopmentManager::step(SimulationStep currentStep, double timestep) {
    applyDevelopmentEffects(currentStep, timestep);
}

void DevelopmentManager::applyDevelopmentEffects(SimulationStep currentStep, double timestep) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        developmentSystem->update(brain_, *brain_->pImpl->rng, timestep * 1000);
        
        // Development affects plasticity rates
        auto* sp = brain_->pImpl->structuralPlasticity.get();
        if (sp) {
            DevelopmentalStage stage = getCurrentDevelopmentalStage();
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
                case DevelopmentalStage::Aging:
                    plasticityMod = 0.1f;  // Very low in aging
                    break;
            }
            
            sp->setSynaptogenesisRate(0.0001f * plasticityMod);
            sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

DevelopmentalStage DevelopmentManager::getCurrentDevelopmentalStage() const {
    return brain_->pImpl->developmentalStage;
}

} // namespace nlm
