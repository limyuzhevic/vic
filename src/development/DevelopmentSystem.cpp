#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double developmentalAge;  // Total developmental age
    
    // Critical period parameters
    bool criticalPeriodOpen;
    double criticalPeriodOpenTime;
    double criticalPeriodClosureTime;
    double criticalPeriodExperienceThreshold;
    float criticalPeriodExperience;
    
    // Activity-dependent development
    double totalExperienceScore;
    double activityLevel;
    
    // Neuromodulation influences
    float dopamineInfluence;
    float acetylcholineInfluence;
    float norepinephrineInfluence;
    
    // Plasticity history
    float averagePlasticity;
    float plasticityTrend;
    
    // Experience-dependent learning rates
    double synaptogenesisLearningRate;
    double pruningLearningRate;
    double maturationLearningRate;
    
    Impl() : stage(DevelopmentalStage::Initial), 
             stageStartStep(0), 
             stepsInCurrentStage(0), 
             stageAge(0.0), 
             developmentalAge(0.0),
             criticalPeriodOpen(true),
             criticalPeriodOpenTime(0.0),
             criticalPeriodClosureTime(300.0),  // Critical period closes after 300 time units
             criticalPeriodExperienceThreshold(50.0f),
             criticalPeriodExperience(0.0f),
             totalExperienceScore(0.0),
             activityLevel(0.0),
             dopamineInfluence(1.0f),
             acetylcholineInfluence(1.0f),
             norepinephrineInfluence(1.0f),
             averagePlasticity(1.0f),
             plasticityTrend(0.0f),
             synaptogenesisLearningRate(0.1),
             pruningLearningRate(0.05),
             maturationLearningRate(0.03) {}
};

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {}

DevelopmentSystem::~DevelopmentSystem() = default;

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    
    // Reset stage-specific parameters
    if (stage == DevelopmentalStage::CriticalPeriod) {
        pImpl->criticalPeriodOpen = true;
        pImpl->criticalPeriodOpenTime = age_;
        pImpl->criticalPeriodExperience = 0.0f;
    }
    
    NLM_LOG_INFO("Development stage set to: " + std::string(getStageName()));
}

void DevelopmentSystem::advanceStage() {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->stage = DevelopmentalStage::CriticalPeriod;
            pImpl->criticalPeriodOpen = true;
            pImpl->criticalPeriodOpenTime = age_;
            pImpl->criticalPeriodExperienceThreshold = std::max(30.0, pImpl->criticalPeriodExperienceThreshold * 0.8f);
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->stage = DevelopmentalStage::Maturation;
            // Close critical period when advancing
            pImpl->criticalPeriodOpen = false;
            pImpl->criticalPeriodClosureTime = age_;
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
    // Update developmental age (synchronous update)
    age_ += 0.001;  // Approximate timestep
    pImpl->stageAge += 0.001;
    pImpl->stepsInCurrentStage++;
    
    // Update critical period experience if applicable
    if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->criticalPeriodOpen) {
        pImpl->criticalPeriodExperience += 1.0f;
        
        // Close critical period if experience threshold exceeded
        if (pImpl->criticalPeriodExperience >= pImpl->criticalPeriodExperienceThreshold) {
            pImpl->criticalPeriodOpen = false;
            pImpl->criticalPeriodClosureTime = age_;
            NLM_LOG_INFO("Critical period closed due to sufficient experience");
        }
    }
    
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
    // Asynchronous development update with dt
    double dtDouble = static_cast<double>(dt);
    
    age_ += dtDouble;
    pImpl->stageAge += dtDouble;
    pImpl->stepsInCurrentStage++;
    
    // Update critical period experience based on neural activity
    if (brain && pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->criticalPeriodOpen) {
        // Calculate experience score from neural activity
        float activityScore = 0.0f;
        if (brain->getWorkingMemory()) {
            activityScore += brain->getWorkingMemory()->getMemoryActivity() * 10.0f;
        }
        if (brain->getCuriosity()) {
            activityScore += brain->getCuriosity()->getLevel() * 5.0f;
        }
        if (brain->getDopamine()) {
            activityScore += brain->getDopamine()->getLevel() * 15.0f;
        }
        
        pImpl->criticalPeriodExperience += activityScore * 0.001f;
        
        // Close critical period if experience threshold exceeded
        if (pImpl->criticalPeriodExperience >= pImpl->criticalPeriodExperienceThreshold) {
            pImpl->criticalPeriodOpen = false;
            pImpl->criticalPeriodClosureTime = age_;
            NLM_LOG_INFO("Critical period closed due to sufficient experience");
        }
    }
    
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
    // Also influenced by critical period state
    float basePlasticity = 1.0f;
    
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            basePlasticity = 1.0f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            basePlasticity = 0.8f;
            // Reduced plasticity if critical period is open
            if (pImpl->criticalPeriodOpen) {
                basePlasticity *= 0.9f;  // Slightly reduced during critical period
            }
            break;
        case DevelopmentalStage::Maturation:
            basePlasticity = 0.5f;
            break;
        case DevelopmentalStage::Adult:
            basePlasticity = 0.2f;
            break;
        case DevelopmentalStage::Aging:
            basePlasticity = 0.1f;
            break;
        default:
            basePlasticity = 0.5f;
    }
    
    return basePlasticity;
}

bool DevelopmentSystem::isCriticalPeriod() const {
    // Critical period is open if:
    // 1. We're in the critical period stage
    // 2. Critical period hasn't been closed by experience
    return pImpl->stage == DevelopmentalStage::CriticalPeriod && 
           pImpl->criticalPeriodOpen;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod || !pImpl->criticalPeriodOpen) {
        return 0.0f;
    }
    
    // Progress through critical period (0 to 1)
    float timeProgress = std::min(1.0f, static_cast<float>(pImpl->stageAge / 300.0f));
    float experienceProgress = std::min(1.0f, pImpl->criticalPeriodExperience / pImpl->criticalPeriodExperienceThreshold);
    
    return (timeProgress + experienceProgress) * 0.5f;
}

} // namespace nlm
