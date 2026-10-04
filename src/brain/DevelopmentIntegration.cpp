#include "Integration.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void DevelopmentIntegration::initialize(DevelopmentSystem* developmentSystemPtr, Brain* brainPtr) {
    developmentSystem = developmentSystemPtr;
    brain = brainPtr;
}

void DevelopmentIntegration::update(DevelopmentSystem* devSystemPtr, StructuralPlasticity* structuralPlasticityPtr,
                                   DevelopmentalStage currentStage, SimulationStep currentStep,
                                   Timestamp timestep, RandomGenerator* rng) {
    if (!devSystemPtr) return;
    
    // Step 13: Apply development effects
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        devSystemPtr->update(brain, *rng, timestep * 1000);
        
        // Development affects plasticity rates
        if (structuralPlasticityPtr) {
            float plasticityMod = 1.0f;
            
            switch (currentStage) {
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
            
            structuralPlasticityPtr->setSynaptogenesisRate(0.0001f * plasticityMod);
            structuralPlasticityPtr->setPruningRate(0.00001f * (2.0f - plasticityMod));
        }
    }
}

} // namespace nlm