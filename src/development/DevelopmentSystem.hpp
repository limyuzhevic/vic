#pragma once

#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

class DevelopmentSystem {
public:
    DevelopmentSystem(Brain* brain,
                     DevelopmentSystem* developmentSystemPtr,
                     StructuralPlasticity* structuralPlasticity);
    
    // Update developmental effects with current simulation state
    void updateDevelopment(double timestep, const RandomGenerator& rng);
    
    // Apply developmental stage effects on plasticity rates
    void applyDevelopmentalStageEffects();
    
    // Update structural plasticity based on developmental stage
    void updateStructuralPlasticity(SimulationStep currentStep, 
                                   const RandomGenerator& rng);
    
    // Get current developmental stage and effects
    DevelopmentalStage getDevelopmentalStage() const;
    float getPlasticityModulation() const;
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    
    // Development control
    void setDevelopmentalStage(DevelopmentalStage stage);
    void advanceDevelopment(double timeElapsed);
    bool isDevelopmentEnabled() const;
    
private:
    Brain* brain;
    DevelopmentSystem* developmentSystemPtr;
    StructuralPlasticity* structuralPlasticity;
    
    // State tracking
    DevelopmentalStage currentStage;
    double developmentTime;
    float plasticityModulation;
    bool developmentEnabled;
};
