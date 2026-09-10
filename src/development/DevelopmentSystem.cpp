#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {
    NLM_LOG_DEBUG("DevelopmentSystem::DevelopmentSystem(): Created development system");
}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    NLM_LOG_DEBUG("DevelopmentSystem::setStage(): Setting stage to " + std::to_string(static_cast<int>(stage)));
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
}

void DevelopmentSystem::advanceStage() {
    DevelopmentalStage oldStage = pImpl->stage;
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
        default:
            NLM_LOG_ERROR("DevelopmentSystem::advanceStage(): Unknown developmental stage " + std::to_string(static_cast<int>(pImpl->stage)));
            break;
    }
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    
    if (pImpl->stage != oldStage) {
        NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
    }
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
    NLM_LOG_DEBUG("DevelopmentSystem::update(Brain*, SimulationStep): Starting development update at step " + std::to_string(currentStep));
    
    // Validate brain pointer
    if (!brain) {
        NLM_LOG_ERROR("DevelopmentSystem::update(Brain*, SimulationStep): Brain pointer is null");
        return;
    }
    
    // Validate currentStep
    if (currentStep > static_cast<SimulationStep>(-1) / 2) {
        NLM_LOG_ERROR("DevelopmentSystem::update(Brain*, SimulationStep): Invalid simulation step: " + std::to_string(currentStep));
        return;
    }
    
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    
    // Validate age values
    if (std::isnan(age_) || std::isinf(age_)) {
        NLM_LOG_ERROR("DevelopmentSystem::update(Brain*, SimulationStep): Invalid developmental age: " + std::to_string(age_));
        age_ = 0.0;
    }
    
    if (std::isnan(pImpl->stageAge) || std::isinf(pImpl->stageAge)) {
        NLM_LOG_ERROR("DevelopmentSystem::update(Brain*, SimulationStep): Invalid stage age: " + std::to_string(pImpl->stageAge));
        pImpl->stageAge = 0.0;
    }
    
    // Auto-advance stage based on time in stage
    // Initial: 60 steps, CriticalPeriod: 300 steps, Maturation: 600 steps
    bool stageAdvanced = false;
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > 60.0) {
        advanceStage();
        stageAdvanced = true;
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > 300.0) {
        advanceStage();
        stageAdvanced = true;
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > 600.0) {
        advanceStage();
        stageAdvanced = true;
    }
    
    if (stageAdvanced) {
        NLM_LOG_INFO("DevelopmentSystem::update(): Stage automatically advanced to " + std::string(getStageName()) + 
                     " at step " + std::to_string(currentStep));
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    NLM_LOG_DEBUG("DevelopmentSystem::update(Brain*, RandomGenerator&, TimestepDuration): Starting development update with dt " + std::to_string(dt));
    
    // Validate brain pointer
    if (!brain) {
        NLM_LOG_ERROR("DevelopmentSystem::update(Brain*, RandomGenerator&, TimestepDuration): Brain pointer is null");
        return;
    }
    
    // Validate random generator
    if (!&rng) {
        NLM_LOG_ERROR("DevelopmentSystem::update(Brain*, RandomGenerator&, TimestepDuration): Random generator is null");
        return;
    }
    
    // Validate timestep
    if (dt <= 0.0) {
        NLM_LOG_ERROR("DevelopmentSystem::update(Brain*, RandomGenerator&, TimestepDuration): Invalid timestep " + std::to_string(dt));
        return;
    }
    
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Validate age values
    if (std::isnan(age_) || std::isinf(age_)) {
        NLM_LOG_ERROR("DevelopmentSystem::update(Brain*, RandomGenerator&, TimestepDuration): Invalid developmental age: " + std::to_string(age_));
        age_ = 0.0;
    }
    
    if (std::isnan(pImpl->stageAge) || std::isinf(pImpl->stageAge)) {
        NLM_LOG_ERROR("DevelopmentSystem::update(Brain*, RandomGenerator&, TimestepDuration): Invalid stage age: " + std::to_string(pImpl->stageAge));
        pImpl->stageAge = 0.0;
    }
    
    // Auto-advance stage based on developmental age
    // These thresholds are in simulation seconds
    bool stageAdvanced = false;
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        advanceStage();
        stageAdvanced = true;
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        advanceStage();
        stageAdvanced = true;
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        advanceStage();
        stageAdvanced = true;
    }
    
    if (stageAdvanced) {
        NLM_LOG_INFO("DevelopmentSystem::update(): Stage automatically advanced to " + std::string(getStageName()) + 
                     " at age " + std::to_string(age_));
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    float modifier = 0.5f;  // Default value
    
    // Higher plasticity in early stages, lower in later stages
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: modifier = 1.0f; break;
        case DevelopmentalStage::CriticalPeriod: modifier = 0.8f; break;
        case DevelopmentalStage::Maturation: modifier = 0.5f; break;
        case DevelopmentalStage::Adult: modifier = 0.2f; break;
        case DevelopmentalStage::Aging: modifier = 0.1f; break;
        default: 
            NLM_LOG_WARNING("DevelopmentSystem::getPlasticityModifier(): Unknown stage " + std::to_string(static_cast<int>(pImpl->stage)) +
                           ", using default modifier: " + std::to_string(modifier));
            break;
    }
    
    NLM_LOG_DEBUG("DevelopmentSystem::getPlasticityModifier(): Returning plasticity modifier " + std::to_string(modifier) +
                 " for stage " + std::string(getStageName()));
    return modifier;
}

bool DevelopmentSystem::isCriticalPeriod() const {
    bool result = (pImpl->stage == DevelopmentalStage::CriticalPeriod);
    NLM_LOG_DEBUG("DevelopmentSystem::isCriticalPeriod(): Critical period " + std::string(result ? "enabled" : "disabled"));
    return result;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    float progress = 0.0f;
    
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        NLM_LOG_DEBUG("DevelopmentSystem::getCriticalPeriodProgress(): Not in critical period");
        return progress;
    }
    
    // Progress through critical period (0 to 1)
    progress = std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
    
    NLM_LOG_DEBUG("DevelopmentSystem::getCriticalPeriodProgress(): Critical period progress: " + std::to_string(progress));
    return progress;
}

} // namespace nlm
