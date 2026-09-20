#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    
    // Critical period dynamics
    bool criticalPeriodClosed;          // Whether critical period has naturally closed
    float criticalPeriodSensitivity;    // Sensitivity to experience (0-1)
    float experienceThreshold;          // Minimum activity required for plasticity
    float neuralMaturation;             // Level of neural maturation (0-1)
    
    // Sensitive period tracking
    SimulationStep currentSensitiveWindow;  // Which sensitive period we're in
    float sensitivePeriodStrength;          // Current sensitivity to specific experiences
    
    // Critical period mechanisms
    float perineuronalNetDensity;         // PNN density affecting plasticity (0-1)
    float myelinMaturation;               // Myelin level affecting signal speed
    float inhibitionMaturation;           // GABAergic inhibition level
    
    // Brain reference for activity calculations
    Brain* brain;
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0),
             criticalPeriodClosed(false), criticalPeriodSensitivity(1.0f), experienceThreshold(0.1f),
             neuralMaturation(0.0f), currentSensitiveWindow(0), sensitivePeriodStrength(1.0f),
             perineuronalNetDensity(0.0f), myelinMaturation(0.0f), inhibitionMaturation(0.0f), brain(nullptr) {}
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
    
    // Update critical period mechanisms
    updateCriticalPeriod();
    updateSensitivePeriods();
    
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
    
    // Update critical period mechanisms
    updateCriticalPeriod();
    updateSensitivePeriods();
    
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

void DevelopmentSystem::updateCriticalPeriod() {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return;
    }
    
    // Critical period closure mechanisms:
    // 1. Perineuronal nets (PNNs) stabilize synapses
    pImpl->perineuronalNetDensity += 0.001f;  // Gradual PNN maturation
    
    // 2. Myelin maturation reduces plasticity
    pImpl->myelinMaturation += 0.001f;
    
    // 3. GABAergic inhibition increases
    pImpl->inhibitionMaturation += 0.001f;
    
    // 4. Experience-dependent critical period closure
    // When enough sensory input is received, critical period closes
    if (!pImpl->criticalPeriodClosed) {
        float totalActivity = calculateTotalNeuralActivity();
        if (totalActivity > pImpl->experienceThreshold) {
            // Close critical period
            pImpl->criticalPeriodClosed = true;
            pImpl->criticalPeriodSensitivity *= 0.5f;  // Reduce sensitivity
            
            // PNNs close faster with more experience
            pImpl->perineuronalNetDensity += 0.05f;
            NLM_LOG_INFO("Critical period closed - experience exceeded threshold");
        }
    }
    
    }
    
    // 5. Neural maturation affects critical period duration
    pImpl->neuralMaturation = std::min(1.0f, pImpl->stageAge / 300.0f);
}

void DevelopmentSystem::updateSensitivePeriods() {
    // Sensitive periods occur throughout development
    // Each sensitive period has different strengths and durations
    
    // Calculate current sensitive period
    pImpl->currentSensitiveWindow = static_cast<SimulationStep>(age_);
    
    // Determine sensitive period strength based on developmental stage
    if (pImpl->stage == DevelopmentalStage::Initial) {
        pImpl->sensitivePeriodStrength = 0.9f;  // Strong early sensitivity
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        pImpl->sensitivePeriodStrength = 0.7f;
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        pImpl->sensitivePeriodStrength = 0.4f;
    } else {
        pImpl->sensitivePeriodStrength = 0.2f;
    }
    
    // Experience-dependent modulation of sensitive periods
    if (pImpl->stage == DevelopmentalStage::CriticalPeriod) {
        // Language and social skills sensitive period
        float languageExperience = calculateLanguageExperience();
        float socialExperience = calculateSocialExperience();
        
        // Modulate sensitive period strength based on experience
        pImpl->sensitivePeriodStrength *= (1.0f - (languageExperience * 0.3f + socialExperience * 0.3f));
    } else if (pImpl->stage == DevelopmentalStage::Maturation) {
        // Skill acquisition sensitive period
        float skillExperience = calculateSkillExperience();
        pImpl->sensitivePeriodStrength *= (1.0f - skillExperience * 0.5f);
    }
}

float DevelopmentSystem::calculateTotalNeuralActivity() const {
    // Calculate total neural activity across all regions
    if (!pImpl->brain) return 0.0f;
    
    float totalActivity = 0.0f;
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            for (const auto* neuron : population->getNeurons()) {
                float activity = neuron->getState().firingRate;
                totalActivity += activity;
            }
        }
    }
    
    return totalActivity / 1000.0f;  // Normalize
}

float DevelopmentSystem::calculateLanguageExperience() const {
    // Calculate language exposure based on sensory input patterns
    // Placeholder - would integrate with actual sensory data
    return 0.5f;
}

float DevelopmentSystem::calculateSocialExperience() const {
    // Calculate social interaction experience
    return 0.3f;
}

float DevelopmentSystem::calculateSkillExperience() const {
    // Calculate skill training experience
    return 0.7f;
}

} // namespace nlm

