// DevelopmentSystem implementation
#include "DevelopmentSystem.hpp"
#include <algorithm>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep timeSinceBirth;
    float maturationRate;
    float plasticityRate;
    
    Impl() : stage(DevelopmentalStage::Initial), timeSinceBirth(0), maturationRate(0.01f), plasticityRate(1.0f) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl) {}

DevelopmentSystem::~DevelopmentSystem() = default;

void DevelopmentSystem::initialize(class Brain* brain) {
    // Store brain reference if needed
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator* rng, TimestepDuration dt) {
    if (!brain || !rng) return;
    
    pImpl->timeSinceBirth += dt;
    
    // Determine developmental stage based on time
    if (pImpl->timeSinceBirth < 100.0) {
        pImpl->stage = DevelopmentalStage::Initial;
        pImpl->plasticityRate = 1.0f;  // High initial plasticity
    } else if (pImpl->timeSinceBirth < 500.0) {
        pImpl->stage = DevelopmentalStage::CriticalPeriod;
        pImpl->plasticityRate = 0.8f;
    } else if (pImpl->timeSinceBirth < 1000.0) {
        pImpl->stage = DevelopmentalStage::Maturation;
        pImpl->plasticityRate = 0.5f;
    } else {
        pImpl->stage = DevelopmentalStage::Adult;
        pImpl->plasticityRate = 0.2f;
    }
    
    // Apply developmental effects
    applyDevelopmentEffects(brain, rng, dt);
}

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
}

float DevelopmentSystem::getPlasticityModulation() const {
    return pImpl->plasticityRate;
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getMaturationProgress() const {
    if (pImpl->timeSinceBirth >= 1000.0) {
        return 1.0f;
    }
    return pImpl->timeSinceBirth / 1000.0f;
}

void DevelopmentSystem::applyDevelopmentEffects(Brain* brain, RandomGenerator* rng, TimestepDuration dt) {
    // Apply age-dependent structural changes
    // Synaptogenesis decreases with age
    // Pruning increases with age
    // Myelin development improves signal transmission
    
    float synaptogenesisRate = 0.0001f * pImpl->plasticityRate;
    float pruningRate = 0.00001f * (2.0f - pImpl->plasticityRate);
    
    // In a real implementation, these would be applied to the brain's plasticity systems
    if (brain) {
        // This would connect to the brain's structural plasticity
    }
}

} // namespace nlm
