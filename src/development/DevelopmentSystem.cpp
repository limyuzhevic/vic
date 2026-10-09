#include "DevelopmentSystem.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <algorithm>

namespace nlm {

DevelopmentSystem::DevelopmentSystem(Brain* brain,
                                     DevelopmentSystem* developmentSystemPtr,
                                     StructuralPlasticity* structuralPlasticity)
    : brain(brain), developmentSystemPtr(developmentSystemPtr),
      structuralPlasticity(structuralPlasticity), currentStage(DevelopmentalStage::Initial),
      developmentTime(0.0), plasticityModulation(1.0f), developmentEnabled(true) {
}

void DevelopmentSystem::updateDevelopment(double timestep, const RandomGenerator& rng) {
    developmentTime += timestep;
    
    // Update developmental stage based on accumulated time
    if (developmentEnabled) {
        // Simplified developmental stage progression
        if (developmentTime < 100.0) {
            currentStage = DevelopmentalStage::Initial;
        } else if (developmentTime < 500.0) {
            currentStage = DevelopmentalStage::CriticalPeriod;
        } else if (developmentTime < 1000.0) {
            currentStage = DevelopmentalStage::Maturation;
        } else {
            currentStage = DevelopmentalStage::Adult;
        }
        
        // Apply developmental stage effects
        applyDevelopmentalStageEffects();
        
        // Update structural plasticity
        updateStructuralPlasticity(0, rng); // Using currentStep = 0 as placeholder
    }
}

void DevelopmentSystem::applyDevelopmentalStageEffects() {
    // Set plasticity modulation based on developmental stage
    switch (currentStage) {
        case DevelopmentalStage::Initial:
            plasticityModulation = 1.0f;  // High plasticity
            break;
        case DevelopmentalStage::CriticalPeriod:
            plasticityModulation = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            plasticityModulation = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            plasticityModulation = 0.2f;  // Stable
            break;
        case DevelopmentalStage::Aging:
            plasticityModulation = 0.1f;  // Reduced plasticity
            break;
    }
    
    // Update structural plasticity rates
    if (structuralPlasticity) {
        float synaptogenesisRate = 0.0001f * plasticityModulation;
        float pruningRate = 0.00001f * (2.0f - plasticityModulation);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
    }
}

void DevelopmentSystem::updateStructuralPlasticity(SimulationStep currentStep, 
                                                   const RandomGenerator& rng) {
    if (structuralPlasticity) {
        structuralPlasticity->update(brain, rng);
    }
}

DevelopmentalStage DevelopmentSystem::getDevelopmentalStage() const {
    return currentStage;
}

float DevelopmentSystem::getPlasticityModulation() const {
    return plasticityModulation;
}

float DevelopmentSystem::getSynaptogenesisRate() const {
    return structuralPlasticity ? structuralPlasticity->getSynaptogenesisRate() : 0.0f;
}

float DevelopmentSystem::getPruningRate() const {
    return structuralPlasticity ? structuralPlasticity->getPruningRate() : 0.0f;
}

void DevelopmentSystem::setDevelopmentalStage(DevelopmentalStage stage) {
    currentStage = stage;
    applyDevelopmentalStageEffects();
}

void DevelopmentSystem::advanceDevelopment(double timeElapsed) {
    developmentTime += timeElapsed;
    if (developmentEnabled) {
        applyDevelopmentalStageEffects();
    }
}

bool DevelopmentSystem::isDevelopmentEnabled() const {
    return developmentEnabled;
}

} // namespace nlm