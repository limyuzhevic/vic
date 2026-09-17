#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "EnhancedDevelopmentSystem.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Enhanced system
    std::unique_ptr<EnhancedDevelopmentSystem> enhancedSystem;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0), useEnhanced_(false) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    if (useEnhanced_ && pImpl->enhancedSystem) {
        return pImpl->enhancedSystem->getStage();
    }
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
}

void DevelopmentSystem::advanceStage() {
    if (useEnhanced_ && pImpl->enhancedSystem) {
        pImpl->enhancedSystem->advanceStage();
        return;
    }
    
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
    if (useEnhanced_ && pImpl->enhancedSystem) {
        return pImpl->enhancedSystem->getStageName();
    }
    
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
    if (useEnhanced_ && pImpl->enhancedSystem && brain) {
        // Use enhanced system with current timestep
        TimestepDuration dt = 0.001;  // Approximate timestep
        pImpl->enhancedSystem->update(brain, *brain->getRandomGenerator(), dt, currentStep);
        age_ = pImpl->enhancedSystem->getDevelopmentalAge();
        return;
    }
    
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
    if (useEnhanced_ && pImpl->enhancedSystem && brain) {
        // Use enhanced system
        pImpl->enhancedSystem->update(brain, rng, dt, static_cast<SimulationStep>(age_ / dt));
        age_ = pImpl->enhancedSystem->getDevelopmentalAge();
        return;
    }
    
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
    if (useEnhanced_ && pImpl->enhancedSystem) {
        return pImpl->enhancedSystem->getPlasticityModifier();
    }
    
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
    if (useEnhanced_ && pImpl->enhancedSystem) {
        return pImpl->enhancedSystem->isCriticalPeriod();
    }
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (useEnhanced_ && pImpl->enhancedSystem) {
        return pImpl->enhancedSystem->getCriticalPeriodProgress();
    }
    
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1)
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
}

EnhancedDevelopmentSystem* DevelopmentSystem::getEnhancedSystem() {
    return pImpl->enhancedSystem.get();
}

void DevelopmentSystem::initializeEnhanced(Brain* brain) {
    if (brain) {
        pImpl->enhancedSystem = std::make_unique<EnhancedDevelopmentSystem>();
        pImpl->enhancedSystem->initialize(brain);
        useEnhanced_ = true;
        NLM_LOG_INFO("DevelopmentSystem initialized with EnhancedDevelopmentSystem");
    }
}

} // namespace nlm
