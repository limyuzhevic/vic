#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    double age;
    float plasticityModifier;
    float criticalPeriodProgress;
    
    Impl() 
        : stage(DevelopmentalStage::Initial)
        , age(0.0)
        , plasticityModifier(1.0f)
        , criticalPeriodProgress(0.0f) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    NLM_LOG_INFO("DevelopmentSystem: Setting stage to " + std::to_string(static_cast<int>(stage)));
}

const char* DevelopmentSystem::getStageName() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        default: return "Unknown";
    }
}

void DevelopmentSystem::advanceStage() {
    DevelopmentalStage current = pImpl->stage;
    if (current == DevelopmentalStage::Initial) {
        setStage(DevelopmentalStage::CriticalPeriod);
    } else if (current == DevelopmentalStage::CriticalPeriod) {
        setStage(DevelopmentalStage::Maturation);
    } else if (current == DevelopmentalStage::Maturation) {
        setStage(DevelopmentalStage::Adult);
    }
    NLM_LOG_INFO("DevelopmentSystem: Advanced to " + std::string(getStageName()));
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    update(brain, *(brain ? brain->getRandomGenerator() : nullptr), currentStep * 0.001);
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    // Age the system
    pImpl->age += dt;
    
    // Update plasticity modifier based on stage
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->plasticityModifier = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->plasticityModifier = 0.8f;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->plasticityModifier = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            pImpl->plasticityModifier = 0.2f;
            break;
    }
    
    // Update critical period progress
    if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        pImpl->criticalPeriodProgress = std::min(1.0f, static_cast<float>(pImpl->age) / 300.0f);
    }
    
    // Apply developmental effects on plasticity
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    if (structuralPlasticity) {
        float synRate = 0.0001f * pImpl->plasticityModifier;
        float pruneRate = 0.00001f * (2.0f - pImpl->plasticityModifier);
        structuralPlasticity->setSynaptogenesisRate(synRate);
        structuralPlasticity->setPruningRate(pruneRate);
        NLM_LOG_INFO("DevelopmentSystem: Set plasticity rates - synaptogenesis: " + std::to_string(synRate) + 
                     ", pruning: " + std::to_string(pruneRate));
    }
    
    // Log developmental milestones
    if (std::abs(pImpl->age - 60.0) < dt) {
        NLM_LOG_INFO("DevelopmentSystem: Initial developmental period complete");
    } else if (std::abs(pImpl->age - 300.0) < dt) {
        NLM_LOG_INFO("DevelopmentSystem: Critical period complete");
    } else if (std::abs(pImpl->age - 900.0) < dt) {
        NLM_LOG_INFO("DevelopmentSystem: Maturation period complete");
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    return pImpl->plasticityModifier;
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    return pImpl->criticalPeriodProgress;
}

} // namespace nlm
