#include "DevelopmentSystem.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"

namespace nlm;

struct DevelopmentSystem::Impl {
    DevelopmentalStage stage;
    SimulationStep stageStartStep;
    SimulationStep stepsInCurrentStage;
    double stageAge;  // Age since entering current stage
    double age_;      // Total developmental age
    
    // Experience-dependent development parameters
    double learningExperience;            // Cumulative learning experience
    double environmentalInteraction;       // Environmental interaction strength
    double predictionAccuracy;             // Prediction accuracy (0-1)
    double memoryConsolidation;            // Memory consolidation level
    
    // Neuromodulation integration
    float neuromodulationLevel;            // Combined neuromodulation signal
    float curiosityLevel;                  // Curiosity level from neuromodulation
    float dopamineLevel;                   // Dopamine level from neuromodulation
    
    // Adaptive development parameters
    double maturationRate;                 // Age/experience-dependent maturation rate
    double plasticityExponent;             // Experience-dependent plasticity scaling
    double criticalPeriodReopening;        // Critical period reopening strength
    double attentionModulation;            // Maturation-dependent attention
    
    // Developmental trajectories
    std::vector<double> developmentalTrajectory;  // Age -> plasticity curve
    std::vector<double> experienceTrajectory;     // Experience -> maturation curve
    std::vector<double> environmentTrajectory;     // Environment -> plasticity curve
    
    Impl() : stage(DevelopmentalStage::Initial), stageStartStep(0), stepsInCurrentStage(0), stageAge(0.0), age_(0.0),
             learningExperience(0.0), environmentalInteraction(0.0), predictionAccuracy(0.5),
             memoryConsolidation(0.0), neuromodulationLevel(0.0), curiosityLevel(0.0), dopamineLevel(0.0),
             maturationRate(1.0), plasticityExponent(1.0), criticalPeriodReopening(0.0), attentionModulation(1.0) {
        // Initialize developmental trajectories
        developmentalTrajectory = {0.0, 1.0, 0.8, 0.5, 0.2, 0.1};  // Initial, CP, Maturation, Adult, Aging
        experienceTrajectory = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5};     // Experience scales with age
        environmentTrajectory = {0.5, 1.0, 1.2, 1.5, 1.8, 2.0};   // Environment boosts plasticity
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
    pImpl->stageStartStep = 0;
    NLM_LOG_INFO("Development stage set to: " + std::string(getStageName()));
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
    pImpl->stageStartStep = 0;
    
    // Re-calculate adaptive parameters for new stage
    updateAdaptiveParameters();
    
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
    
    // Update developmental parameters based on current stage
    updateDevelopmentalParameters(brain);
    
    // Auto-advance stage based on developmental age with experience modifiers
    double stageDuration = getStageDuration(pImpl->stage);
    double experienceModifier = calculateExperienceModifier();
    double effectiveDuration = stageDuration * experienceModifier;
    
    if (pImpl->stage == DevelopmentalStage::Initial && pImpl->stageAge > effectiveDuration) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && pImpl->stageAge > effectiveDuration) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && pImpl->stageAge > effectiveDuration) {
        advanceStage();
    }
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    age_ += dt;
    pImpl->stageAge += dt;
    ++pImpl->stepsInCurrentStage;
    
    // Update developmental parameters
    updateDevelopmentalParameters(brain);
    
    // Auto-advance stage based on developmental age
    double stageDuration = getStageDuration(pImpl->stage);
    double experienceModifier = calculateExperienceModifier();
    double effectiveDuration = stageDuration * experienceModifier;
    
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > effectiveDuration) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > effectiveDuration) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > effectiveDuration) {
        advanceStage();
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Higher plasticity in early stages, modulated by experience and environment
    double stageIndex = static_cast<double>(pImpl->stage);
    
    // Get base plasticity for current stage
    double basePlasticity = 0.0;
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial: basePlasticity = 1.0; break;
        case DevelopmentalStage::CriticalPeriod: basePlasticity = 0.8; break;
        case DevelopmentalStage::Maturation: basePlasticity = 0.5; break;
        case DevelopmentalStage::Adult: basePlasticity = 0.2; break;
        case DevelopmentalStage::Aging: basePlasticity = 0.1; break;
        default: basePlasticity = 0.5;
    }
    
    // Apply experience and environmental modifiers
    double plasticity = basePlasticity * pImpl->plasticityExponent * (1.0 + pImpl->environmentalInteraction * 0.5);
    
    // Cap at reasonable limits
    return static_cast<float>(std::clamp(plasticity, 0.01, 1.0));
}

bool DevelopmentSystem::isCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

float DevelopmentSystem::getCriticalPeriodProgress() const {
    if (pImpl->stage != DevelopmentalStage::CriticalPeriod) {
        return 0.0f;
    }
    // Progress through critical period (0 to 1) with experience acceleration
    double progress = pImpl->stageAge / 300.0;
    progress *= pImpl->criticalPeriodReopening;
    return static_cast<float>(std::clamp(progress, 0.0, 1.0));
}

double DevelopmentSystem::getDevelopmentalAge() const {
    return age_;
}

void DevelopmentSystem::setDevelopmentalAge(double age) {
    age_ = age;
}

void DevelopmentSystem::integrateNeuromodulation(float curiosity, float dopamine) {
    pImpl->curiosityLevel = curiosity;
    pImpl->dopamineLevel = dopamine;
    pImpl->neuromodulationLevel = (curiosity + dopamine) * 0.5f;
}

void DevelopmentSystem::integratePrediction(float predictionError, float predictionAccuracy) {
    // High prediction error promotes rapid development
    if (predictionError > 0.5) {
        pImpl->learningExperience *= (1.0 + predictionError * 0.5);
    }
    
    pImpl->predictionAccuracy = predictionAccuracy;
    
    // Prediction accuracy accelerates maturation
    if (predictionAccuracy > 0.7) {
        pImpl->maturationRate *= (1.0 + (predictionAccuracy - 0.7) * 0.3);
    }
}

void DevelopmentSystem::integrateMemory(double consolidationLevel) {
    pImpl->memoryConsolidation = consolidationLevel;
    
    // Memory consolidation promotes structural stability
    pImpl->criticalPeriodReopening *= (1.0 - consolidationLevel * 0.3);
}

void DevelopmentSystem::integrateEnvironmentalInteraction(double interactionStrength) {
    pImpl->environmentalInteraction = interactionStrength;
    
    // Strong environmental interaction boosts plasticity
    if (interactionStrength > 1.0) {
        pImpl->plasticityExponent *= (1.0 + (interactionStrength - 1.0) * 0.2);
    }
}

void DevelopmentSystem::updateDevelopmentalParameters(Brain* brain) {
    // Update experience-dependent maturation rate
    if (brain) {
        // Get neuromodulation levels if available
        if (pImpl->curiosityLevel > 0.0) {
            pImpl->learningExperience *= (1.0 + pImpl->curiosityLevel * 0.2);
        }
        
        if (pImpl->dopamineLevel > 0.0) {
            pImpl->maturationRate *= (1.0 + pImpl->dopamineLevel * 0.3);
        }
        
        // Environmental interaction based on sensory activity
        if (brain->getTotalSpikeCount() > 0) {
            double spikeRate = brain->getTotalSpikeCount() / age_;
            pImpl->environmentalInteraction += spikeRate * 0.01;
            pImpl->environmentalInteraction = std::min(pImpl->environmentalInteraction, 3.0);
        }
        
        // Update attention modulation based on stage
        switch (pImpl->stage) {
            case DevelopmentalStage::Initial:
                pImpl->attentionModulation = 1.0;
                break;
            case DevelopmentalStage::CriticalPeriod:
                pImpl->attentionModulation = 1.5;  // Enhanced attention during critical period
                break;
            case DevelopmentalStage::Maturation:
                pImpl->attentionModulation = 2.0;  // Strong attention for learning
                break;
            case DevelopmentalStage::Adult:
                pImpl->attentionModulation = 1.2;
                break;
            case DevelopmentalStage::Aging:
                pImpl->attentionModulation = 0.8;
                break;
        }
    }
}

double DevelopmentSystem::calculateExperienceModifier() const {
    // Experience accelerates development progression
    double baseModifier = 1.0;
    
    // Experience accumulation (logarithmic scale to avoid runaway)
    baseModifier *= (1.0 + std::log(1.0 + pImpl->learningExperience * 0.1) * 0.3);
    
    // Environmental interaction
    baseModifier *= (1.0 + pImpl->environmentalInteraction * 0.2);
    
    // Neuromodulation boost
    baseModifier *= (1.0 + pImpl->neuromodulationLevel * 0.3);
    
    // Prediction accuracy stabilizes development
    if (pImpl->predictionAccuracy > 0.8) {
        baseModifier *= 0.9;  // Slows down when prediction is stable
    }
    
    return std::clamp(baseModifier, 0.5, 3.0);  // Keep in reasonable range
}

double DevelopmentSystem::getStageDuration(DevelopmentalStage stage) const {
    // Base stage durations in simulation seconds
    switch (stage) {
        case DevelopmentalStage::Initial: return 60.0;
        case DevelopmentalStage::CriticalPeriod: return 300.0;
        case DevelopmentalStage::Maturation: return 600.0;
        case DevelopmentalStage::Adult: return 0.0; // Never auto-advance from Adult
        case DevelopmentalStage::Aging: return 0.0; // Never auto-advance from Aging
        default: return 100.0;
    }
}

void DevelopmentSystem::updateAdaptiveParameters() {
    // Re-calculate adaptive parameters when stage changes
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            pImpl->maturationRate = 1.0;
            pImpl->criticalPeriodReopening = 1.0;
            pImpl->attentionModulation = 1.0;
            break;
        case DevelopmentalStage::CriticalPeriod:
            pImpl->maturationRate = 1.2;  // Accelerated maturation during critical period
            pImpl->criticalPeriodReopening = 0.5;  // Can reopen slightly
            pImpl->attentionModulation = 1.5;
            break;
        case DevelopmentalStage::Maturation:
            pImpl->maturationRate = 1.5;  // Strong maturation
            pImpl->criticalPeriodReopening = 0.3;
            pImpl->attentionModulation = 2.0;
            break;
        case DevelopmentalStage::Adult:
            pImpl->maturationRate = 0.8;
            pImpl->criticalPeriodReopening = 0.1;
            pImpl->attentionModulation = 1.2;
            break;
        case DevelopmentalStage::Aging:
            pImpl->maturationRate = 0.5;
            pImpl->criticalPeriodReopening = 0.0;
            pImpl->attentionModulation = 0.8;
            break;
    }
}

float DevelopmentSystem::getMaturationRate() const {
    return static_cast<float>(pImpl->maturationRate);
}

float DevelopmentSystem::getCriticalPeriodReopening() const {
    return static_cast<float>(pImpl->criticalPeriodReopening);
}

float DevelopmentSystem::getAttentionModulation() const {
    return static_cast<float>(pImpl->attentionModulation);
}

float DevelopmentSystem::getEnvironmentalInteraction() const {
    return static_cast<float>(pImpl->environmentalInteraction);
}

void DevelopmentSystem::integrateWithMemorySystem(Brain* brain) {
    if (!brain) return;
    
    // Get memory system for integration
    if (auto episodicMemory = brain->getEpisodicMemory()) {
        // Memory consolidation affects developmental stability
        double memoryRetention = episodicMemory->getRetentionStrength();
        integrateMemory(memoryRetention);
        
        // Strong memory promotes maturation stability
        pImpl->criticalPeriodReopening *= (1.0 - memoryRetention * 0.2);
        pImpl->maturationRate *= (1.0 + memoryRetention * 0.1);
    }
}

void DevelopmentSystem::integrateWithPredictionSystem(Brain* brain) {
    if (!brain) return;
    
    // Get prediction system for integration
    if (auto predictionSystem = brain->getPredictionSystem()) {
        float predictionError = predictionSystem->getPredictionError();
        float predictionAccuracy = 1.0f - predictionError;
        
        integratePrediction(predictionError, predictionAccuracy);
        
        // High prediction error encourages exploration and development
        if (predictionError > 0.5f) {
            pImpl->learningExperience *= (1.0 + predictionError * 0.5f);
            pImpl->criticalPeriodReopening *= (1.0 + predictionError * 0.3f);
        }
    }
}

void DevelopmentSystem::applyDevelopmentalEffectsToLearning(float& learningRate, float& plasticity, Brain* brain) {
    if (!brain) return;
    
    // Apply stage-dependent learning effects
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            learningRate *= 1.5f;  // Enhanced learning in initial stage
            plasticity *= 1.2f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            learningRate *= 1.2f;
            plasticity *= 1.5f;  // Peak plasticity during critical period
            break;
        case DevelopmentalStage::Maturation:
            learningRate *= 0.8f;
            plasticity *= 1.0f;
            break;
        case DevelopmentalStage::Adult:
            learningRate *= 0.5f;
            plasticity *= 0.7f;
            break;
        case DevelopmentalStage::Aging:
            learningRate *= 0.3f;
            plasticity *= 0.5f;
            break;
    }
    
    // Apply experience and environmental modifiers
    learningRate *= (1.0f + static_cast<float>(pImpl->learningExperience) * 0.1f);
    learningRate *= (1.0f + static_cast<float>(pImpl->environmentalInteraction) * 0.05f);
    
    plasticity *= static_cast<float>(pImpl->plasticityExponent);
    
    // Neuromodulatory effects
    learningRate *= (1.0f + pImpl->neuromodulationLevel * 0.3f);
    
    // Cap learning and plasticity
    learningRate = std::min(learningRate, 2.0f);
    plasticity = std::min(plasticity, 2.0f);
}

void DevelopmentSystem::updateDevelopmentState(Brain* brain) {
    if (!brain) return;
    
    // Update developmental state based on brain activity
    double spikeCount = brain->getTotalSpikeCount();
    double spikeRate = (age_ > 0.0) ? spikeCount / age_ : 0.0;
    
    // Activity-based development acceleration
    pImpl->learningExperience += spikeRate * 0.001f;
    pImpl->environmentalInteraction += spikeRate * 0.0005f;
    
    // Integrate with other systems
    integrateWithMemorySystem(brain);
    integrateWithPredictionSystem(brain);
    
    // Update developmental parameters based on experience
    pImpl->stageAge += 0.001;  // Assuming 1ms timestep
    
    // Apply developmental effects to brain systems
    if (auto curiosity = brain->getCuriosity()) {
        float curiosityLevel = curiosity->getLevel();
        integrateNeuromodulation(curiosityLevel, pImpl->dopamineLevel);
    }
    
    // Auto-advance development stage based on age
    updateDevelopmentStage();
}

void DevelopmentSystem::updateDevelopmentStage() {
    double stageDuration = getStageDuration(pImpl->stage);
    double experienceModifier = calculateExperienceModifier();
    double effectiveDuration = stageDuration * experienceModifier;
    
    // Check if we should auto-advance to next stage
    if (pImpl->stage == DevelopmentalStage::Initial && age_ > effectiveDuration) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::CriticalPeriod && age_ > effectiveDuration) {
        advanceStage();
    } else if (pImpl->stage == DevelopmentalStage::Maturation && age_ > effectiveDuration) {
        advanceStage();
    }
}

} // namespace nlm
