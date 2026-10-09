#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double manualPlasticityModifier;  // Manual override for plasticity modifier (-1.0 = no override)
    double criticalPeriodDuration;  // Critical period duration
    double maturationRate;  // Neural maturation rate
    
    Impl() 
        : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0)
        , manualPlasticityModifier(-1.0f)
        , criticalPeriodDuration(360.0)
        , maturationRate(0.01) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

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
    // If manual override is set, use it (value != -1.0 indicates manual override)
    if (pImpl && pImpl->manualPlasticityModifier >= 0.0f) {
        return pImpl->manualPlasticityModifier;
    }
    // Otherwise, return stage-based plasticity modifier
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 1.0f;
        case DevelopmentalStage::CriticalPeriod: return 0.8f;
        case DevelopmentalStage::Maturation: return 0.5f;
        case DevelopmentalStage::Adult: return 0.2f;
        case DevelopmentalStage::Aging: return 0.1f;
        default: return 0.5f;
    }
}

void DevelopmentSystem::setPlasticityModifier(float modifier) {
    // Set manual override (-1.0 means no override, use stage-based value)
    if (pImpl) {
        if (std::abs(modifier - 0.0f) < 0.0001f) {
            pImpl->manualPlasticityModifier = -1.0f;  // Clear override
        } else {
            pImpl->manualPlasticityModifier = modifier;  // Set manual override
        }
    }
}
}

void DevelopmentSystem::setCriticalPeriodDuration(double duration) {
    if (pImpl) {
        pImpl->criticalPeriodDuration = duration;
    }
}

double DevelopmentSystem::getCriticalPeriodDuration() const {
    if (pImpl) {
        return pImpl->criticalPeriodDuration;
    }
    return 360.0;  // Default critical period duration
}

void DevelopmentSystem::triggerPhaseTransition() {
    // Force immediate stage advancement
    if (pImpl) {
        // Log the transition
        std::string currentStageName = "Unknown";
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial: currentStageName = "Initial"; break;
            case DevelopmentalStage::CriticalPeriod: currentStageName = "CriticalPeriod"; break;
            case DevelopmentalStage::Maturation: currentStageName = "Maturation"; break;
            case DevelopmentalStage::Adult: currentStageName = "Adult"; break;
            case DevelopmentalStage::Aging: currentStageName = "Aging"; break;
        }
        
        // Advance to next stage
        advanceStage();
        
        NLM_LOG_INFO("Phase transition triggered from " + currentStageName + " to " + std::string(getStageName()));
    }
}

double DevelopmentSystem::getMaturationRate() const {
    if (pImpl) {
        return pImpl->maturationRate;
    }
    return 0.01;  // Default maturation rate
}

void DevelopmentSystem::setMaturationRate(double rate) {
    if (pImpl) {
        pImpl->maturationRate = std::max(0.0, rate);
    }
}

} // namespace nlm
