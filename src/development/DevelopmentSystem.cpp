#include "DevelopmentSystem.hpp"
#include "Synaptogenesis.hpp"
#include "Pruning.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0), synaptogenesisEfficiency_(0.0f), pruningEfficiency_(0.0f) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            break;
        case DevelopmentalStage::Aging:
            // Already at final stage
            break;
    }
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
}

const char* DevelopmentSystem::getStageName() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: return "Unknown";
    }
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    
    // Auto-advance stage based on time in stage
    // Initial: 60 steps, CriticalPeriod: 300 steps, Maturation: 600 steps
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        advanceStage();
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, lower in later stages
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.0f;
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 0.5f;
        case DevelopmentalStage::Adult: return 0.2f;
        case DevelopmentalStage::Aging: return 0.1f;
        default: return 0.5f;
    }
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
}

void DevelopmentSystem::configureStructuralPlasticity(Synaptogenesis* synaptogenesis, 
                                                       Pruning* pruning) const {
    if (!synaptogenesis || !pruning) return;
    
    float plasticityMod = getPlasticityModifier();
    
    // Configure synaptogenesis based on developmental stage
    float synaptogenesisRate = 0.001f * plasticityMod;
    float synaptogenesisTarget = 0.1f * plasticityMod;
    
    synaptogenesis->setFormationRate(synaptogenesisRate);
    synaptogenesis->setTargetDensity(synaptogenesisTarget);
    synaptogenesis->setDevelopmentalInfluence(plasticityMod);
    synaptogenesis->setHomeostaticTarget(synaptogenesisTarget);
    
    NLM_LOG_DEBUG("Development: Configured synaptogenesis - rate=" + std::to_string(synaptogenesisRate) +
                 ", target=" + std::to_string(synaptogenesisTarget) +
                 ", influence=" + std::to_string(plasticityMod));
    
    // Configure pruning based on developmental stage
    float pruningRate = 0.0001f * (2.0f - plasticityMod);  // Less pruning in high plasticity stages
    float pruningThreshold = 0.05f / std::max(0.1f, plasticityMod);  // More pruning in high plasticity
    
    pruning->setPruningRate(pruningRate);
    pruning->setThreshold(pruningThreshold);
    pruning->setDevelopmentalInfluence(plasticityMod);
    pruning->setHomeostaticTarget(0.1f);
    pruning->setStrengthThreshold(0.05f);
    pruning->setActivityThreshold(0.001f);
    
    NLM_LOG_DEBUG("Development: Configured pruning - rate=" + std::to_string(pruningRate) +
                 ", threshold=" + std::to_string(pruningThreshold) +
                 ", influence=" + std::to_string(plasticityMod));
}

} // namespace nlm
