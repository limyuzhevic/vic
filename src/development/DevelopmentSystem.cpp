#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double developmentalAge;  // Overall developmental age
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0), developmentalAge(0.0) {}
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

// Get excitability modifier for current stage
float DevelopmentSystem::getExcitabilityModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.3f;  // Low initial excitability
        case DevelopmentalStage::CriticalPeriod: return 0.6f;  // Rising excitability
        case DevelopmentalStage::Maturation: return 0.8f;  // High excitability for learning
        case DevelopmentalStage::Adult: return 0.9f;  // Stable high excitability
        case DevelopmentalStage::Aging: return 0.5f;  // Reduced excitability
        default: return 0.7f;
    }
}

// Get attention stability modifier for current stage
float DevelopmentSystem::getAttentionStabilityModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.2f;  // Poor attention stability
        case DevelopmentalStage::CriticalPeriod: return 0.5f;  // Improving attention
        case DevelopmentalStage::Maturation: return 0.8f;  // Good attention stability
        case DevelopmentalStage::Adult: return 0.9f;  // Excellent attention
        case DevelopmentalStage::Aging: return 0.6f;  // Declining attention
        default: return 0.7f;
    }
}

// Get working memory capacity modifier for current stage
float DevelopmentSystem::getWorkingMemoryCapacityModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.3f;  // Low working memory
        case DevelopmentalStage::CriticalPeriod: return 0.6f;  // Increasing capacity
        case DevelopmentalStage::Maturation: return 0.8f;  // Good capacity
        case DevelopmentalStage::Adult: return 0.9f;  // Maximum capacity
        case DevelopmentalStage::Aging: return 0.7f;  // Slight decline
        default: return 0.7f;
    }
}

// Get memory consolidation strength modifier for current stage
float DevelopmentSystem::getMemoryConsolidationModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.2f;  // Poor consolidation
        case DevelopmentalStage::CriticalPeriod: return 0.5f;  // Developing consolidation
        case DevelopmentalStage::Maturation: return 0.8f;  // Strong consolidation
        case DevelopmentalStage::Adult: return 0.9f;  // Excellent consolidation
        case DevelopmentalStage::Aging: return 0.6f;  // Declining consolidation
        default: return 0.7f;
    }
}

// Get neuromodulation levels modifier for current stage
float DevelopmentSystem::getNeuromodulationModifier(NeuromodulatorType type) const {
    switch (type) {
        case NeuromodulatorType::Dopamine:
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: return 0.2f;  // Low dopamine
                case DevelopmentalStage::CriticalPeriod: return 0.5f;  // Rising dopamine
                case DevelopmentalStage::Maturation: return 0.8f;  // High dopamine for learning
                case DevelopmentalStage::Adult: return 0.7f;  // Stable dopamine
                case DevelopmentalStage::Aging: return 0.4f;  // Declining dopamine
                default: return 0.6f;
            }
        case NeuromodulatorType::Serotonin:
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: return 0.3f;  // Low serotonin
                case DevelopmentalStage::CriticalPeriod: return 0.6f;  // Rising serotonin
                case DevelopmentalStage::Maturation: return 0.8f;  // High serotonin
                case DevelopmentalStage::Adult: return 0.8f;  // Stable serotonin
                case DevelopmentalStage::Aging: return 0.6f;  // Declining serotonin
                default: return 0.7f;
            }
        case NeuromodulatorType::Norepinephrine:
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: return 0.3f;  // Low norepinephrine
                case DevelopmentalStage::CriticalPeriod: return 0.7f;  // High norepinephrine for attention
                case DevelopmentalStage::Maturation: return 0.9f;  // Peak norepinephrine
                case DevelopmentalStage::Adult: return 0.8f;  // Stable norepinephrine
                case DevelopmentalStage::Aging: return 0.6f;  // Declining norepinephrine
                default: return 0.7f;
            }
        case NeuromodulatorType::Acetylcholine:
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: return 0.2f;  // Low acetylcholine
                case DevelopmentalStage::CriticalPeriod: return 0.6f;  // Rising acetylcholine
                case DevelopmentalStage::Maturation: return 0.8f;  // High acetylcholine
                case DevelopmentalStage::Adult: return 0.9f;  // Peak acetylcholine
                case DevelopmentalStage::Aging: return 0.6f;  // Declining acetylcholine
                default: return 0.7f;
            }
        case NeuromodulatorType::Curiosity:
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: return 0.4f;  // Low curiosity
                case DevelopmentalStage::CriticalPeriod: return 0.8f;  // High curiosity
                case DevelopmentalStage::Maturation: return 1.0f;  // Peak curiosity
                case DevelopmentalStage::Adult: return 0.7f;  // Stable curiosity
                case DevelopmentalStage::Aging: return 0.5f;  // Declining curiosity
                default: return 0.7f;
            }
        case NeuromodulatorType::Novelty:
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: return 0.3f;  // Low novelty response
                case DevelopmentalStage::CriticalPeriod: return 0.7f;  // High novelty response
                case DevelopmentalStage::Maturation: return 0.9f;  // Peak novelty response
                case DevelopmentalStage::Adult: return 0.8f;  // Stable novelty response
                case DevelopmentalStage::Aging: return 0.6f;  // Declining novelty response
                default: return 0.7f;
            }
        case NeuromodulatorType::PredictionError:
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: return 0.5f;  // Moderate prediction error
                case DevelopmentalStage::CriticalPeriod: return 0.7f;  // Rising prediction error
                case DevelopmentalStage::Maturation: return 0.8f;  // High prediction error
                case DevelopmentalStage::Adult: return 0.6f;  // Stable prediction error
                case DevelopmentalStage::Aging: return 0.4f;  // Declining prediction error
                default: return 0.6f;
            }
        case NeuromodulatorType::Reward:
            switch (pImpl->stage) {
                case DevelopmentalStage::Initial: return 0.3f;  // Low reward sensitivity
                case DevelopmentalStage::CriticalPeriod: return 0.6f;  // Rising reward sensitivity
                case DevelopmentalStage::Maturation: return 0.8f;  // High reward sensitivity
                case DevelopmentalStage::Adult: return 0.7f;  // Stable reward sensitivity
                case DevelopmentalStage::Aging: return 0.5f;  // Declining reward sensitivity
                default: return 0.6f;
            }
        default: return 0.5f;
    }
}

// Get sleep/rest cycle duration modifier for current stage
float DevelopmentSystem::getSleepCycleModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.8f;  // Long sleep for development
        case DevelopmentalStage::CriticalPeriod: return 0.6f;  // Reduced sleep
        case DevelopmentalStage::Maturation: return 0.5f;  // Moderate sleep
        case DevelopmentalStage::Adult: return 0.3f;  // Short sleep for efficiency
        case DevelopmentalStage::Aging: return 0.4f;  // Increased sleep need
        default: return 0.5f;
    }
}

// Get life history memory persistence modifier for current stage
float DevelopmentSystem::getLifeHistoryPersistenceModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.1f;  // Low persistence (for flexibility)
        case DevelopmentalStage::CriticalPeriod: return 0.3f;  // Increasing persistence
        case DevelopmentalStage::Maturation: return 0.5f;  // Good persistence
        case DevelopmentalStage::Adult: return 0.8f;  // High persistence
        case DevelopmentalStage::Aging: return 0.7f;  // Stable persistence
        default: return 0.5f;
    }
}

// Get plasticity rule modifier for current stage (beyond structural)
float DevelopmentSystem::getPlasticityRuleModifier() const {
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return 0.5f;  // Basic plasticity rules
        case DevelopmentalStage::CriticalPeriod: return 0.8f;  // Enhanced STDP
        case DevelopmentalStage::Maturation: return 0.9f;  // Optimized plasticity
        case DevelopmentalStage::Adult: return 0.7f;  // Stable plasticity
        case DevelopmentalStage::Aging: return 0.6f;  // Reduced plasticity
        default: return 0.7f;
    }
}

// Update life history tracking
void DevelopmentSystem::updateLifeHistory(Brain* brain, TimestepDuration dt) {
    // Track sleep/wake cycles
    lifeHistory_.lastSleepEndTime += dt;
    lifeHistory_.totalWakeTime += dt;
    
    // Calculate sleep efficiency based on stage
    if (brain) {
        // Apply developmental modifiers to sleep efficiency
        float stageModifier = getSleepCycleModifier();
        lifeHistory_.sleepEfficiency = std::min(1.0f, lifeHistory_.sleepEfficiency + 0.01f * stageModifier);
    }
}

// Get current developmental milestone (0.0 to 1.0)
float DevelopmentSystem::getCurrentDevelopmentalMilestone() const {
    float baseProgress = std::min(1.0f, static_cast<float>(pImpl->stageAge / 1000.0));
    
    // Adjust based on stage
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: return baseProgress * 0.3f;
        case DevelopmentalStage::CriticalPeriod: return std::min(1.0f, (baseProgress * 0.7f) + 0.3f);
        case DevelopmentalStage::Maturation: return std::min(1.0f, (baseProgress * 0.5f) + 0.5f);
        case DevelopmentalStage::Adult: return std::min(1.0f, (baseProgress * 0.3f) + 0.7f);
        case DevelopmentalStage::Aging: return std::min(1.0f, (baseProgress * 0.1f) + 0.8f);
        default: return baseProgress;
    }
}

// Check if a developmental milestone has been passed
bool DevelopmentSystem::hasMilestonePassed(DevelopmentalMilestone milestone) const {
    switch (milestone) {
        case DevelopmentalMilestone::FirstSleepCycle:
            return age_ > 30.0;
        case DevelopmentalMilestone::FirstMemoryConsolidation:
            return age_ > 120.0;
        case DevelopmentalMilestone::AttentionStabilityEstablished:
            return age_ > 180.0;
        case DevelopmentalMilestone::NeuromodulationCalibration:
            return age_ > 90.0;
        case DevelopmentalMilestone::WorkingMemoryCapacityIncreased:
            return age_ > 240.0;
        case DevelopmentalMilestone::PlasticityRuleCalibration:
            return age_ > 150.0;
        case DevelopmentalMilestone::LifeHistoryEstablishment:
            return age_ > 360.0;
        case DevelopmentalMilestone::SleepWakeCycleRegulated:
            return age_ > 60.0;
        default:
            return false;
    }
}

} // namespace nlm
