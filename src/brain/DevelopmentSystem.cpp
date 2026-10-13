#include "DevelopmentSystem.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    std::shared_ptr<Config> config;
    Brain* brain;
    double developmentalAge;
    DevelopmentalStage currentStage;
    float plasticityModifier;
    
    Impl(std::shared_ptr<Config> cfg) 
        : config(cfg), brain(nullptr), developmentalAge(0.0), 
          currentStage(DevelopmentalStage::Initial), plasticityModifier(1.0f) {}
};

DevelopmentSystem::DevelopmentSystem(std::shared_ptr<Config> config) : pImpl(new Impl(config)) {}

DevelopmentSystem::~DevelopmentSystem() = default;

bool DevelopmentSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    NLM_LOG_INFO("DevelopmentSystem initialized");
    return true;
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, double timestep) {
    pImpl->brain = brain;
    pImpl->developmentalAge += timestep;
    
    // Simple developmental stages based on age
    if (pImpl->developmentalAge < 60.0) {  // ~1 minute
        pImpl->plasticityModifier = 1.0f;  // High plasticity
        pImpl->currentStage = DevelopmentalStage::Initial;
    } else if (pImpl->developmentalAge < 300.0) {  // ~5 minutes
        pImpl->plasticityModifier = 0.8f;
        pImpl->currentStage = DevelopmentalStage::CriticalPeriod;
    } else if (pImpl->developmentalAge < 900.0) {  // ~15 minutes
        pImpl->plasticityModifier = 0.5f;
        pImpl->currentStage = DevelopmentalStage::Maturation;
    } else {
        pImpl->plasticityModifier = 0.2f;  // Adult - more stable
        pImpl->currentStage = DevelopmentalStage::Adult;
    }
}

} // namespace nlm
