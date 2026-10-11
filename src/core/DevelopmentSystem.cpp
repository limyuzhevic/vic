// DevelopmentSystem implementation - Handles developmental stages and age-related plasticity

#include "DevelopmentSystem.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

DevelopmentSystem::DevelopmentSystem(std::shared_ptr<Brain> brain)
    : brain_(brain), developmentalStage_(DevelopmentalStage::Initial), developmentalAge_(0.0) {
}

DevelopmentSystem::~DevelopmentSystem() = default;

bool DevelopmentSystem::initialize() {
    developmentSystem_ = std::make_unique<::DevelopmentSystem>();
    developmentSystem_->initialize(brain_.get());
    
    return true;
}

void DevelopmentSystem::step(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
    // Update developmental stage
    updateDevelopmentalStage(currentStep);
    
    // Apply development effects to plasticity
    applyDevelopmentEffectsToPlasticity();
}

void DevelopmentSystem::updateDevelopmentalStage(SimulationStep currentStep) {
    developmentalAge_ += static_cast<double>(timestep) * 1000.0; // Convert to ms
    
    // Simple developmental stages based on age (similar to AgentBrain)
    // This is a biologically inspired approximation
    if (developmentalAge_ < 60.0) {  // ~1 minute
        brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
    } else if (developmentalAge_ < 300.0) {  // ~5 minutes
        brain_->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
    } else if (developmentalAge_ < 900.0) {  // ~15 minutes
        brain_->setDevelopmentalStage(DevelopmentalStage::Maturation);
    } else {
        brain_->setDevelopmentalStage(DevelopmentalStage::Adult);  // ~5 minutes
    }
    
    developmentalStage_ = brain_->getDevelopmentalStage();
}

void DevelopmentSystem::applyDevelopmentEffectsToPlasticity() {
    if (!developmentSystem_ || !brain_->getStructuralPlasticity()) {
        return;
    }
    
    auto* sp = brain_->getStructuralPlasticity();
    float plasticityMod = 1.0f;
    
    switch (developmentalStage_) {
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
    
    // Apply developmental modulation to structural plasticity rates
    sp->setSynaptogenesisRate(0.0001f * plasticityMod);
    sp->setPruningRate(0.00001f * (2.0f - plasticityMod));
}

} // namespace nlm
