#include "brain/BrainDevelopment.hpp"
#include "brain/Brain.hpp"
#include "brain/DevelopmentalStage.hpp"

namespace nlm {

void BrainDevelopment::updateDevelopmentSystem(DevelopmentSystem* developmentSystem,
                                             Brain* brain,
                                             RandomGenerator& rng,
                                             TimestepDuration timestep,
                                             StructuralPlasticity* structuralPlasticity,
                                             DevelopmentalStage& developmentalStage) {
    if (!developmentSystem || !brain) return;
    
    developmentSystem->update(brain, rng, timestep * 1000);
    
    // Development affects plasticity rates
    if (structuralPlasticity) {
        float plasticityMod = 1.0f;
        
        switch (developmentalStage) {
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
        
        // Update structural plasticity rates based on developmental stage
        structuralPlasticity->setSynaptogenesisRate(0.0001f * plasticityMod);
        structuralPlasticity->setPruningRate(0.00001f * (2.0f - plasticityMod));
    }
}

void BrainDevelopment::advanceDevelopmentalStage(DevelopmentSystem* developmentSystem,
                                                 size_t currentStep,
                                                 DevelopmentalStage& developmentalStage) {
    if (currentStep % 1000 == 0) {  // Update development every 1000 steps
        if (developmentSystem) {
            developmentSystem->update(nullptr, RandomGenerator(42), 1000.0f);  // Simplified update
        }
        
        // Simple developmental progression (would be more sophisticated in real system)
        if (currentStep > 5000 && developmentalStage == DevelopmentalStage::Initial) {
            developmentalStage = DevelopmentalStage::CriticalPeriod;
        } else if (currentStep > 10000 && developmentalStage == DevelopmentalStage::CriticalPeriod) {
            developmentalStage = DevelopmentalStage::Maturation;
        } else if (currentStep > 20000 && developmentalStage == DevelopmentalStage::Maturation) {
            developmentalStage = DevelopmentalStage::Adult;
        }
    }
}

} // namespace nlm
