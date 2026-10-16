#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <stdexcept>
#include <limits>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Initialize developmental system with default initial stage
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0) {
        NLM_LOG_DEBUG("DevelopmentSystem: Initialized in stage " + std::string(getStageName()));
    }
};

DevelopmentSystem::DevelopmentSystem() : pImpl(std::make_unique<Impl>()), age_(0.0) {
    NLM_LOG_INFO("DevelopmentSystem: Creating developmental system with default parameters");
}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    // Validate stage transition and log state change
    NLM_LOG_DEBUG("DevelopmentSystem::setStage: Transitioning from " + 
                 std::string(getStageName()) + " to " + std::string(getStageNameFromEnum(stage)));
    
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;  // Reset stage age when changing stages
    pImpl->stageStartStep = 0;  // Reset stage start step
    
    NLM_LOG_INFO("DevelopmentSystem: Set to stage " + std::string(getStageName()));
}

void DevelopmentSystem::advanceStage() {
    // Transition to next developmental stage with validation
    NLM_LOG_DEBUG("DevelopmentSystem::advanceStage: Current stage = " + std::string(getStageName()));
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            NLM_LOG_INFO("DevelopmentSystem: Transitioning from Initial to CriticalPeriod");
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            NLM_LOG_INFO("DevelopmentSystem: Transitioning from CriticalPeriod to Maturation");
            break;
        case DevelopmentalStage::Maturation:
            pImpl->stage = DevelopmentalStage::Adult;
            NLM_LOG_INFO("DevelopmentSystem: Transitioning from Maturation to Adult");
            break;
        case DevelopmentalStage::Adult:
            pImpl->stage = DevelopmentalStage::Aging;
            NLM_LOG_INFO("DevelopmentSystem: Transitioning from Adult to Aging");
            break;
        case DevelopmentalStage::Aging:
            // Already at final stage
            NLM_LOG_INFO("DevelopmentSystem: Already at final Aging stage - no further advancement");
            break;
        default:
            NLM_LOG_WARNING("DevelopmentSystem::advanceStage: Unknown developmental stage - remaining in current");
            break;
    }
    
    // Reset stage-specific counters
    pImpl->stepsInCurrentStage = 0;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
}

const char* DevelopmentSystem::getStageName() const {
    // Return human-readable stage name
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: 
            NLM_LOG_WARNING("DevelopmentSystem::getStageName: Unknown stage enum value");
            return "Unknown";
    }
}

const char* DevelopmentSystem::getStageNameFromEnum(DevelopmentalStage stage) const {
    // Helper function to get stage name from enum value
    switch (stage) {
        case DevelopmentalStage::Initial: return "Initial";
        case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
        case DevelopmentalStage::Maturation: return "Maturation";
        case DevelopmentalStage::Adult: return "Adult";
        case DevelopmentalStage::Aging: return "Aging";
        default: 
            NLM_LOG_WARNING("DevelopmentSystem::getStageNameFromEnum: Unknown stage enum value");
            return "Unknown";
    }
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep) {
    // Update developmental system based on simulation steps
    if (!brain) {
        NLM_LOG_WARNING("DevelopmentSystem::update(Brain*): Null brain pointer - developmental updates skipped");
    }
    
    // Update internal timers
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep for step-based updates
    pImpl->stageAge += 0.001;
    
    NLM_LOG_DEBUG("DevelopmentSystem::update(Brain*): age=" + std::to_string(age_) +
                 ", stageAge=" + std::to_string(pImpl->stageAge) +
                 ", stepsInStage=" + std::to_string(pImpl->stepsInCurrentStage));
    
    // Auto-advance stage based on time in stage (step-based thresholds)
    checkAndAdvanceStage(brain, "step-based");
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    // Update developmental system based on simulation time
    if (!brain) {
        NLM_LOG_WARNING("DevelopmentSystem::update(Brain*, dt): Null brain pointer - developmental updates skipped");
    }
    
    // Update internal timers with actual timestep
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    NLM_LOG_DEBUG("DevelopmentSystem::update(Brain*, dt): age=" + std::to_string(age_) +
                 ", stageAge=" + std::to_string(pImpl->stageAge) +
                 ", stepsInStage=" + std::to_string(pImpl->stepsInCurrentStage) +
                 ", dt=" + std::to_string(dt) + "s");
    
    // Auto-advance stage based on developmental age (time-based thresholds)
    checkAndAdvanceStage(brain, "time-based");
}

void DevelopmentSystem::checkAndAdvanceStage(Brain* brain, const std::string& updateType) {
    // Check if current stage duration exceeds threshold and advance if needed
    if (!brain) return;
    
    double threshold = 0.0;
    const char* thresholdType = "";
    
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > 60.0) {
        threshold = 60.0;
        thresholdType = "60s (Initial stage)";
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > 360.0) {
        threshold = 360.0;
        thresholdType = "360s (CriticalPeriod stage)";
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > 960.0) {
        threshold = 960.0;
        thresholdType = "960s (Maturation stage)";
    } else {
        return;  // No advancement needed
    }
    
    // Log advancement attempt
    NLM_LOG_INFO("DevelopmentSystem::checkAndAdvanceStage: Reached " + thresholdType + 
                 " (current age = " + std::to_string(age_) + "s)");
    
    // Advance to next stage
    advanceStage();
    
    // Apply developmental effects to brain based on new stage
    applyDevelopmentalEffects(brain);
}

void DevelopmentSystem::applyDevelopmentalEffects(Brain* brain) {
    // Apply stage-specific developmental effects to the brain
    if (!brain) {
        NLM_LOG_WARNING("DevelopmentSystem::applyDevelopmentalEffects: Null brain - cannot apply effects");
        return;
    }
    
    // Set developmental stage in brain system
    brain->setDevelopmentalStage(pImpl->stage);
    
    // Log developmental effects application
    NLM_LOG_INFO("DevelopmentSystem::applyDevelopmentalEffects: Applied effects for " + 
                 std::string(getStageName()));
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Get plasticity modifier based on current developmental stage
    float modifier = getPlasticityModifierForStage(pImpl->stage);
    
    // Log plasticity modifier for debugging
    NLM_LOG_DEBUG("DevelopmentSystem::getPlasticityModifier: Returning " + std::to_string(modifier) +
                 " for stage " + std::string(getStageName()));
    
    return modifier;
}

float DevelopmentSystem::getPlasticityModifierForStage(DevelopmentalStage stage) const {
    // Return plasticity modifier for specific stage
    switch (stage) {
        case DevelopmentalStage::Initial: 
            NLM_LOG_DEBUG("DevelopmentSystem::getPlasticityModifierForStage: Initial stage - high plasticity (1.0)");
            return 1.0f;
        case DevelopmentalStage::CriticalPeriod:
            NLM_LOG_DEBUG("DevelopmentSystem::getPlasticityModifierForStage: CriticalPeriod stage - high but decreasing plasticity (0.8)");
            return 0.8f;
        case DevelopmentalStage::Maturation:
            NLM_LOG_DEBUG("DevelopmentSystem::getPlasticityModifierForStage: Maturation stage - moderate plasticity (0.5)");
            return 0.5f;
        case DevelopmentalStage::Adult:
            NLM_LOG_DEBUG("DevelopmentSystem::getPlasticityModifierForStage: Adult stage - low plasticity (0.2)");
            return 0.2f;
        case DevelopmentalStage::Aging:
            NLM_LOG_DEBUG("DevelopmentSystem::getPlasticityModifierForStage: Aging stage - very low plasticity (0.1)");
            return 0.1f;
        default:
            NLM_LOG_WARNING("DevelopmentSystem::getPlasticityModifierForStage: Unknown stage - returning default plasticity (0.5)");
            return 0.5f;
    }
}

bool DevelopmentSystem::isCriticalPeriod() const {
    // Check if current developmental stage is critical period
    bool isCritical = (pImpl->stage == DevelopmentalStage::CriticalPeriod);
    
    if (isCritical) {
        NLM_LOG_DEBUG("DevelopmentSystem::isCriticalPeriod: Current stage IS CriticalPeriod");
    } else {
        NLM_LOG_DEBUG("DevelopmentSystem::isCriticalPeriod: Current stage is NOT CriticalPeriod");
    }
    
    return isCritical;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    // Calculate progress through critical period (0.0 to 1.0)
    if (!isCriticalPeriod()) {
        NLM_LOG_DEBUG("DevelopmentSystem::getCriticalPeriodProgress: Not in critical period - returning 0.0");
        return 0.0f;
    }
    
    // Calculate progress through critical period (300 simulation seconds total)
    float progress = std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0));
    
    NLM_LOG_DEBUG("DevelopmentSystem::getCriticalPeriodProgress: Critical period progress = " + 
                 std::to_string(progress) + " (stageAge = " + std::to_string(pImpl->stageAge) + "s)");
    
    return progress;
}

} // namespace nlm
