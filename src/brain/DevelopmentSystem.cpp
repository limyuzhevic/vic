#include "DevelopmentSystem.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

void DevelopmentSystem::initialize(Brain* brain) {
    if (brain) {
        // Development system is initialized in Brain::Impl constructor
    }
}

void DevelopmentSystem::update(Brain& brain, TimestepDuration dt) {
    updateDevelopment(brain, dt);
}

void DevelopmentSystem::updateDevelopment(Brain& brain, TimestepDuration dt) {
    // Update developmental stages and apply effects
    size_t currentStep = 0;  // This should come from brain state
    
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        // Development affects plasticity rates
        auto* sp = brain.getStructuralPlasticity();
        if (sp) {
            DevelopmentalStage stage = brain.getDevelopmentalStage();
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
}

void DevelopmentSystem::updateSynaptogenesis(Brain& brain, float rate) {
    performSynaptogenesis(brain, rate);
}

void DevelopmentSystem::updatePruning(Brain& brain, float rate) {
    performPruning(brain, rate);
}

void DevelopmentSystem::updateStructuralPlasticity(Brain& brain) {
    updateStructuralPlasticity(brain);
}

void DevelopmentSystem::setDevelopmentalStage(Brain& brain, DevelopmentalStage stage) {
    brain.setDevelopmentalStage(stage);
}

DevelopmentalStage DevelopmentSystem::getDevelopmentalStage(Brain& brain) const {
    return brain.getDevelopmentalStage();
}

void DevelopmentSystem::updateAgeDependentDynamics(Brain& brain, Timestamp age) {
    ageRelatedDecline(brain, age);
}

float DevelopmentSystem::getPlasticityFactorForStage(DevelopmentalStage stage) const {
    switch (stage) {
        case DevelopmentalStage::Initial:
            return 1.0f;
        case DevelopmentalStage::CriticalPeriod:
            return 0.8f;
        case DevelopmentalStage::Maturation:
            return 0.5f;
        case DevelopmentalStage::Adult:
            return 0.2f;
        case DevelopmentalStage::Aging:
            return 0.1f;
    }
    return 0.5f;
}

void DevelopmentSystem::applyDevelopmentEffectsToPlasticity(Brain& brain, float& synaptogenesisRate, float& pruningRate) {
    DevelopmentalStage stage = getDevelopmentalStage(brain);
    float plasticityMod = getPlasticityFactorForStage(stage);
    
    synaptogenesisRate *= plasticityMod;
    pruningRate *= (2.0f - plasticityMod);
}

void DevelopmentSystem::logDevelopmentStatus(Brain& brain) const {
    NLM_LOG_INFO("=== Development System Status ===");
    
    DevelopmentalStage stage = brain.getDevelopmentalStage();
    NLM_LOG_INFO("Developmental stage: " + std::to_string(static_cast<int>(stage)));
    
    float plasticityFactor = getPlasticityFactorForStage(stage);
    NLM_LOG_INFO("Plasticity factor: " + std::to_string(plasticityFactor));
}

void DevelopmentSystem::performSynaptogenesis(Brain& brain, float rate) {
    // Implementation would create new synapses
    // For now, just log the rate
    NLM_LOG_INFO("Performing synaptogenesis with rate: " + std::to_string(rate));
}

void DevelopmentSystem::performPruning(Brain& brain, float rate) {
    // Implementation would eliminate unused synapses
    // For now, just log the rate
    NLM_LOG_INFO("Performing pruning with rate: " + std::to_string(rate));
}

void DevelopmentSystem::activityDependentStabilization(Brain& brain) {
    // Activity-dependent stabilization of connections
    NLM_LOG_INFO("Activity-dependent stabilization performed");
}

void DevelopmentSystem::criticalPeriodPlasticity(Brain& brain, float windowStrength) {
    // Critical period plasticity effects
    NLM_LOG_INFO("Critical period plasticity with strength: " + std::to_string(windowStrength));
}

void DevelopmentSystem::maturationEffects(Brain& brain, float maturityLevel) {
    // Maturation-dependent changes
    NLM_LOG_INFO("Maturation effects with level: " + std::to_string(maturityLevel));
}

void DevelopmentSystem::ageRelatedDecline(Brain& brain, Timestamp age) {
    // Age-related decline in plasticity
    if (age > 10000) {  // After significant age
        NLM_LOG_INFO("Age-related decline detected for age: " + std::to_string(age));
    }
}

} // namespace nlm
