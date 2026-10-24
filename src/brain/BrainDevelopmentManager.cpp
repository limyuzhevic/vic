#include "BrainDevelopmentManager.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Logger/Logger.hpp"

namespace nlm {

struct BrainDevelopmentManager::Impl {
    // Development system state
    DevelopmentalStage currentStage;
    
    Impl() : currentStage(DevelopmentalStage::Initial) {}
};

BrainDevelopmentManager::BrainDevelopmentManager() : pImpl(std::make_unique<Impl>()) {}

BrainDevelopmentManager::~BrainDevelopmentManager() = default;

void BrainDevelopmentManager::initialize(Brain* brain) {
    // Initialize development system through brain
    if (brain->getDevelopmentSystem()) {
        brain->getDevelopmentSystem()->initialize(brain);
    }
}

void BrainDevelopmentManager::updateDevelopment(Brain* brain, SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep) {
    if (brain->getDevelopmentSystem()) {
        brain->getDevelopmentSystem()->update(brain, brain->getRandomGenerator(), timestep * 1000);
        applyDevelopmentPlasticityEffects(brain, brain->getDevelopmentalStage());
    }
}

void BrainDevelopmentManager::applyDevelopmentPlasticityEffects(Brain* brain, DevelopmentalStage stage) {
    // Development affects plasticity rates based on stage
    auto* sp = brain->getStructuralPlasticity();
    if (sp) {
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

} // namespace nlm
