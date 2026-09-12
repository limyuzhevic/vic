// Brain development implementation
#include "BrainDevelopment.hpp"
#include "BrainCore.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

void BrainDevelopment::updateDevelopment(Brain* brain, SimulationStep currentStep, TimestepDuration timestep) {
    if (!brain || !brain->getDevelopmentSystem()) return;
    brain->getDevelopmentSystem()->update(brain, *brain->getRandomGenerator(), timestep * 1000);
}

void BrainDevelopment::applyDevelopmentalStageEffects(Brain* brain, DevelopmentalStage developmentalStage) {
    if (!brain) return;
    brain->setDevelopmentalStage(developmentalStage);
}

DevelopmentalStage BrainDevelopment::getDevelopmentalStage(Brain* brain) {
    if (!brain) return DevelopmentalStage::Initial;
    return brain->getDevelopmentalStage();
}

void BrainDevelopment::setDevelopmentalStage(Brain* brain, DevelopmentalStage stage) {
    if (brain) {
        brain->setDevelopmentalStage(stage);
    }
}

bool BrainDevelopment::isDevelopmentComplete(Brain* brain) {
    if (!brain) return false;
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    return stage == DevelopmentalStage::Adult || stage == DevelopmentalStage::Aging;
}

float BrainDevelopment::getPlasticityModulation(Brain* brain) {
    if (!brain) return 1.0f;
    
    DevelopmentalStage stage = brain->getDevelopmentalStage();
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
            plasticityMod = 0.1f;  // Declining plasticity
            break;
    }
    
    return plasticityMod;
}

} // namespace nlm