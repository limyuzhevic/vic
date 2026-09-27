#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Configurable stage durations
    double stageDurations[5];  // Index corresponds to DevelopmentalStage enum
    double criticalPeriodDuration;
    double maturationDuration;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0),
             criticalPeriodDuration(300.0), maturationDuration(600.0) {
        // Initialize stage durations with biologically plausible defaults
        stageDurations[0] = 60.0;   // Initial
        stageDurations[1] = 360.0;  // CriticalPeriod  
        stageDurations[2] = 960.0;  // Maturation (includes CriticalPeriod)
        stageDurations[3] = 2880.0; // Adult (8 years)
        stageDurations[4] = 1e9;    // Aging (effectively infinite)
    }
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    pImpl->stageStartStep = 0; // Reset stage start
}

void DevelopmentSystem::advanceStage() {
    // Get current stage index
    int currentStageIndex = static_cast<int>(pImpl->stage);
    
    // Check if we have a next stage
    if (currentStageIndex < 4) {
        DevelopmentalStage nextStage = static_cast<DevelopmentalStage>(currentStageIndex + 1);
        pImpl->stage = nextStage;
        
        // Set next stage start step
        pImpl->stageStartStep = pImpl->stepsInCurrentStage;
        
        // Reset stage age
        pImpl->stageAge = 0.0;
        
        // Log the stage transition
        NLM_LOG_INFO("Development advanced to stage: " + std::string(getStageName()));
        
        // Notify brain of development change (if brain is available)
        // This is a placeholder for future implementation
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
    ++pImpl->stepsInCurrentStage;
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    
    // Auto-advance stage based on time in stage
    int stageIndex = static_cast<int>(pImpl->stage);
    
    if (stageIndex == 0 && pImpl->stageAge > pImpl->stageDurations[0]) {
        advanceStage();
    } else if (stageIndex == 1 && pImpl->stageAge > pImpl->stageDurations[1]) {
        advanceStage();
    } else if (stageIndex == 2 && pImpl->stageAge > pImpl->stageDurations[2]) {
        advanceStage();
    } else if (stageIndex == 3 && pImpl->stageAge > pImpl->stageDurations[3]) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Auto-advance stage based on developmental age
    int stageIndex = static_cast<int>(pImpl->stage);
    
    // Check if we should advance stage based on age
    if (stageIndex == 0 && age_ > pImpl->stageDurations[0]) {
        advanceStage();
    } else if (stageIndex == 1 && age_ > pImpl->stageDurations[1]) {
        advanceStage();
    } else if (stageIndex == 2 && age_ > pImpl->stageDurations[2]) {
        advanceStage();
    } else if (stageIndex == 3 && age_ > pImpl->stageDurations[3]) {
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
    double maxAge = pImpl->stageDurations[1];
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / maxAge));
}

// Additional configuration methods for enhanced DevelopmentSystem
void DevelopmentSystem::configureStageDuration(DevelopmentalStage stage, double duration) {
    int stageIndex = static_cast<int>(stage);
    if (stageIndex >= 0 && stageIndex < 5) {
        pImpl->stageDurations[stageIndex] = duration;
        NLM_LOG_INFO(std::string(getStageName()) + " stage duration set to: " + std::to_string(duration));
    }
}

double DevelopmentSystem::getStageDuration(DevelopmentalStage stage) const {
    int stageIndex = static_cast<int>(stage);
    if (stageIndex >= 0 && stageIndex < 5) {
        return pImpl->stageDurations[stageIndex];
    }
    return 0.0;
}

void DevelopmentSystem::configureCriticalPeriodDuration(double duration) {
    pImpl->criticalPeriodDuration = duration;
    NLM_LOG_INFO("Critical period duration set to: " + std::to_string(duration));
}

double DevelopmentSystem::getCriticalPeriodDuration() const {
    return pImpl->criticalPeriodDuration;
}

void DevelopmentSystem::configureMaturationDuration(double duration) {
    pImpl->maturationDuration = duration;
    NLM_LOG_INFO("Maturation duration set to: " + std::to_string(duration));
}

double DevelopmentSystem::getMaturationDuration() const {
    return pImpl->maturationDuration;
}

bool DevelopmentSystem::isStageComplete() const {
    int stageIndex = static_cast<int>(pImpl->stage);
    if (stageIndex < 0 || stageIndex >= 5) {
        return true; // Unknown stage or final stage
    }
    
    if (stageIndex == 0) {
        return pImpl->stageAge >= pImpl->stageDurations[0];
    } else if (stageIndex == 1) {
        return pImpl->stageAge >= pImpl->stageDurations[1];
    } else if (stageIndex == 2) {
        return pImpl->stageAge >= pImpl->stageDurations[2];
    } else if (stageIndex == 3) {
        return pImpl->stageAge >= pImpl->stageDurations[3];
    }
    
    return true; // Aging stage is never complete
}

float DevelopmentSystem::getStageProgress() const {
    int stageIndex = static_cast<int>(pImpl->stage);
    if (stageIndex < 0 || stageIndex >= 5) {
        return 1.0f; // Unknown stage or final stage
    }
    
    double maxAge = pImpl->stageDurations[stageIndex];
    if (maxAge == Double::infinity) {
        return 0.0f; // Aging stage has no progress
    }
    
    return std::min(1.0f, static_cast<float>(pImpl->stageAge / maxAge));
}

void DevelopmentSystem::enableExperienceBasedProgression(
    Brain* brain, RandomGenerator& rng, double experienceThreshold) {
    if (!brain || pImpl->stage != DevelopmentalStage::Maturation) {
        return;
    }
    
    // Check for experience-based triggers
    // This would look at brain activity, memory, or other metrics
    // For now, it's a placeholder for future implementation
    
    // Simulate experience-based progression with some randomness
    double experienceLevel = rng.uniformReal(0.0f, 1.0f);
    
    if (experienceLevel > experienceThreshold) {
        NLM_LOG_INFO("Experience-based development progression triggered");
        // Could advance stage early based on experience
        // This would depend on brain state and metrics
    }
}

} // namespace nlm
