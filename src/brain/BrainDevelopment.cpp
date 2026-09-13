#include "BrainDevelopment.hpp"
#include "BrainCore.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Random/Random.hpp"

namespace nlm {

void BrainDevelopment::update(const class Brain* brain, RandomGenerator& rng, TimestepDuration timestep) {
    // Transition through developmental stages
    static size_t stepsInCurrentStage = 0;
    
    if (brain && brain->pImpl) {
        // Update developmental state
        stepsInCurrentStage++;
        
        // Transition to next stage based on developmental progress
        switch (developmentalStage) {
            case DevelopmentalStage::Initial:
                if (stepsInCurrentStage >= CRITICAL_PERIOD_DURATION / 1000) {
                    setStage(DevelopmentalStage::CriticalPeriod);
                    stepsInCurrentStage = 0;
                }
                break;
                
            case DevelopmentalStage::CriticalPeriod:
                if (stepsInCurrentStage >= CRITICAL_PERIOD_DURATION / 1000) {
                    setStage(DevelopmentalStage::Maturation);
                    stepsInCurrentStage = 0;
                }
                break;
                
            case DevelopmentalStage::Maturation:
                if (stepsInCurrentStage >= MATURATION_DURATION / 1000) {
                    setStage(DevelopmentalStage::Adult);
                    stepsInCurrentStage = 0;
                }
                break;
                
            case DevelopmentalStage::Adult:
                // Remain in adult stage
                break;
        }
        
        // Process activity-dependent development
        processActivityDependentDevelopment(brain);
    }
}

void BrainDevelopment::setStage(DevelopmentalStage stage) {
    developmentalStage = stage;
    stepsSinceStageChange = 0;
    
    // Reset timers for stage-specific processes
    switch (stage) {
        case DevelopmentalStage::CriticalPeriod:
            criticalPeriodStartStep = 0;
            criticalPeriodEndStep = CRITICAL_PERIOD_DURATION;
            break;
            
        case DevelopmentalStage::Maturation:
            criticalPeriodStartStep = 0;
            criticalPeriodEndStep = 0;
            break;
            
        default:
            criticalPeriodStartStep = 0;
            criticalPeriodEndStep = 0;
            break;
    }
}

DevelopmentalStage BrainDevelopment::getStage() const {
    return developmentalStage;
}

float BrainDevelopment::getPlasticityModulation() const {
    return calculatePlasticityModulation();
}

float BrainDevelopment::calculatePlasticityModulation() const {
    switch (developmentalStage) {
        case DevelopmentalStage::Initial:
            return 1.0f;  // High plasticity
            
        case DevelopmentalStage::CriticalPeriod:
            return CRITICAL_PERIOD_PLASTICITY_MULTIPLIER;
            
        case DevelopmentalStage::Maturation:
            return MATURATION_PLASTICITY_MULTIPLIER;
            
        case DevelopmentalStage::Adult:
            return ADULT_PLASTICITY_MULTIPLIER;
            
        default:
            return 1.0f;
    }
}

float BrainDevelopment::getExcitabilityModulation() const {
    // Simple model: development increases baseline excitability
    switch (developmentalStage) {
        case DevelopmentalStage::Initial:
            return 0.8f;
            
        case DevelopmentalStage::CriticalPeriod:
            return 0.9f;
            
        case DevelopmentalStage::Maturation:
            return 1.0f;
            
        case DevelopmentalStage::Adult:
            return 1.0f;
            
        default:
            return 1.0f;
    }
}

float BrainDevelopment::getMaturationModulation() const {
    // Model maturation effects on neural properties
    switch (developmentalStage) {
        case DevelopmentalStage::Initial:
            return 0.0f;
            
        case DevelopmentalStage::CriticalPeriod:
            return 0.3f;
            
        case DevelopmentalStage::Maturation:
            return 0.7f;
            
        case DevelopmentalStage::Adult:
            return 1.0f;
            
        default:
            return 0.0f;
    }
}

bool BrainDevelopment::isCriticalPeriod() const {
    return developmentalStage == DevelopmentalStage::CriticalPeriod;
}

void BrainDevelopment::enterCriticalPeriod() {
    if (developmentalStage != DevelopmentalStage::CriticalPeriod) {
        setStage(DevelopmentalStage::CriticalPeriod);
    }
}

void BrainDevelopment::exitCriticalPeriod() {
    if (developmentalStage == DevelopmentalStage::CriticalPeriod) {
        setStage(DevelopmentalStage::Maturation);
    }
}

void BrainDevelopment::processActivityDependentDevelopment(const class Brain* brain) {
    if (!brain || !brain->pImpl) return;
    
    // Activity-dependent maturation
    promoteMaturation();
    
    // Adjust plasticity based on learning history
    // This could track learning rate and adjust development accordingly
}

void BrainDevelopment::promoteMaturation() {
    maturationModulation = std::min(maturationModulation + 0.01f, 1.0f);
}

} // namespace nlm