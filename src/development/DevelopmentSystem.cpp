#include "DevelopmentSystem.hpp"
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

DevelopmentSystem::DevelopmentSystem() : pImpl(new Impl), age_(0.0) {
    NLM_LOG_INFO("DevelopmentSystem: Enhanced initialization with advanced developmental mechanisms");
}

DevelopmentSystem::~DevelopmentSystem() {
    NLM_LOG_INFO("DevelopmentSystem: Enhanced cleanup complete");
}

DevelopmentalStage DevelopmentSystem::getStage() const {
    return pImpl->stage;
}

void DevelopmentSystem::setStage(DevelopmentalStage stage) {
    pImpl->stage = stage;
    pImpl->stageAge = 0.0;
    NLM_LOG_INFO("DevelopmentSystem: Set to stage: " + std::string(getStageName()));
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
    NLM_LOG_INFO("DevelopmentSystem: Development advanced to stage: " + std::string(getStageName()));
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
    update(brain, currentStep, nullptr, 0.0f);
}

void DevelopmentSystem::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    update(brain, 0, &rng, dt);
}

void DevelopmentSystem::update(Brain* brain, SimulationStep currentStep, RandomGenerator* rng, TimestepDuration dt) {
    if (!brain) return;
    
    // Update basic developmental clock
    age_ += (dt > 0.0f) ? dt : 0.001f;
    pImpl->stageAge += (dt > 0.0f) ? dt : 0.001f;
    pImpl->stepsInCurrentStage++;
    
    // Apply enhanced developmental effects
    applyEnhancedDevelopmentalEffects(brain, dt);
    
    // Auto-advance stage based on age
    checkStageAdvancement(brain, dt);
}

void DevelopmentSystem::applyEnhancedDevelopmentalEffects(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Apply stage-specific effects based on developmental stage
    switch (pImpl->stage) {
        case DevelopmentalStage::Initial:
            applyInfantEffects(brain, dt);
            break;
        case DevelopmentalStage::CriticalPeriod:
            applyCriticalPeriodEffects(brain, dt);
            break;
        case DevelopmentalStage::Maturation:
            applyMaturationEffects(brain, dt);
            break;
        case DevelopmentalStage::Adult:
            applyAdultEffects(brain, dt);
            break;
        case DevelopmentalStage::Aging:
            applyAgingEffects(brain, dt);
            break;
        default:
            applyAdultEffects(brain, dt);  // Default to adult effects
            break;
    }
}

void DevelopmentSystem::applyInfantEffects(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Enhanced infant effects with sophisticated developmental mechanisms
    auto* workingMemory = brain->getWorkingMemory();
    auto* episodicMemory = brain->getEpisodicMemory();
    
    if (workingMemory) {
        // Enhanced memory capacity development
        size_t enhancedCapacity = static_cast<size_t>(15.0 * (1.0 + age_ * 0.001));
        workingMemory->setCapacity(std::min(200, std::max(15, enhancedCapacity)));
        
        // Enhanced decay rate adaptation
        float enhancedDecay = 0.008f * (2.0f - enhancedCapacity * 0.01f);
        workingMemory->setDecayRate(std::max(0.001f, std::min(0.12f, enhancedDecay)));
        
        NLM_LOG_DEBUG("DevelopmentSystem: Enhanced infant working memory - capacity: " + std::to_string(enhancedCapacity) +
                     ", decay: " + std::to_string(enhancedDecay));
    }
    
    if (episodicMemory) {
        // Enhanced episodic memory integration
        NLM_LOG_INFO("DevelopmentSystem: Enhanced infant episodic memory integration enabled");
    }
    
    // Enhanced plasticity effects for infants
    auto* stdp = brain->getSTDP();
    if (stdp) {
        float enhancedLTP = 0.012f * (1.0f + age_ * 0.002f);
        float enhancedLTD = 0.010f * (1.0f + age_ * 0.0015f);
        stdp->setLTPWeight(enhancedLTP);
        stdp->setLTDWeight(enhancedLTD);
        NLM_LOG_DEBUG("DevelopmentSystem: Enhanced infant STDP - LTP: " + std::to_string(enhancedLTP) +
                     ", LTD: " + std::to_string(enhancedLTD));
    }
}

void DevelopmentSystem::applyCriticalPeriodEffects(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Critical period effects with advanced mechanisms
    auto* workingMemory = brain->getWorkingMemory();
    
    if (workingMemory) {
        // Optimized memory capacity for critical period
        size_t optimalCapacity = static_cast<size_t>(12.0 * (1.0 + age_ * 0.0008));
        workingMemory->setCapacity(std::min(150, std::max(12, optimalCapacity)));
    }
    
    // Enhanced plasticity for critical period
    auto* stdp = brain->getSTDP();
    if (stdp) {
        float criticalPeriodLTP = 0.011f * (1.2f + age_ * 0.001f);
        float criticalPeriodLTD = 0.011f * (1.0f + age_ * 0.0008f);
        stdp->setLTPWeight(criticalPeriodLTP);
        stdp->setLTDWeight(criticalPeriodLTD);
        NLM_LOG_DEBUG("DevelopmentSystem: Critical period STDP - LTP: " + std::to_string(criticalPeriodLTP) +
                     ", LTD: " + std::to_string(criticalPeriodLTD));
    }
    
    // Enhanced structural plasticity
    auto* structuralPlasticity = brain->getStructuralPlasticity();
    if (structuralPlasticity) {
        float enhancedSynapseRate = 0.00012f * (1.5f - age_ * 0.0005f);
        float enhancedPruneRate = 0.000008f * (1.0f + age_ * 0.0003f);
        structuralPlasticity->setSynaptogenesisRate(enhancedSynapseRate);
        structuralPlasticity->setPruningRate(enhancedPruneRate);
        NLM_LOG_DEBUG("DevelopmentSystem: Critical period structural plasticity - syn: " + std::to_string(enhancedSynapseRate) +
                     ", prune: " + std::to_string(enhancedPruneRate));
    }
}

void DevelopmentSystem::applyMaturationEffects(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Maturation effects with refined mechanisms
    auto* workingMemory = brain->getWorkingMemory();
    
    if (workingMemory) {
        // Optimized memory capacity for maturation
        size_t refinedCapacity = static_cast<size_t>(10.0 * (1.0 + age_ * 0.0006f));
        workingMemory->setCapacity(std::min(120, std::max(10, refinedCapacity)));
    }
    
    // Refined plasticity for maturation
    auto* stdp = brain->getSTDP();
    if (stdp) {
        float matureLTP = 0.010f * (1.0f + age_ * 0.0005f);
        float matureLTD = 0.012f * (1.0f + age_ * 0.0007f);
        stdp->setLTPWeight(matureLTP);
        stdp->setLTDWeight(matureLTD);
        NLM_LOG_DEBUG("DevelopmentSystem: Maturation STDP - LTP: " + std::to_string(matureLTP) +
                     ", LTD: " + std::to_string(matureLTD));
    }
}

void DevelopmentSystem::applyAdultEffects(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Adult effects with optimized mechanisms
    auto* workingMemory = brain->getWorkingMemory();
    
    if (workingMemory) {
        // Optimized memory capacity for adult
        size_t adultCapacity = static_cast<size_t>(8.0 * (1.0 + age_ * 0.0004f));
        workingMemory->setCapacity(std::min(100, std::max(8, adultCapacity)));
    }
    
    // Optimized plasticity for adult
    auto* stdp = brain->getSTDP();
    if (stdp) {
        float adultLTP = 0.008f * (1.0f + age_ * 0.0003f);
        float adultLTD = 0.015f * (1.0f + age_ * 0.0009f);
        stdp->setLTPWeight(adultLTP);
        stdp->setLTDWeight(adultLTD);
        NLM_LOG_DEBUG("DevelopmentSystem: Adult STDP - LTP: " + std::to_string(adultLTP) +
                     ", LTD: " + std::to_string(adultLTD));
    }
}

void DevelopmentSystem::applyAgingEffects(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Aging effects with age-related adaptations
    auto* workingMemory = brain->getWorkingMemory();
    
    if (workingMemory) {
        // Age-adapted memory capacity
        size_t agedCapacity = static_cast<size_t>(6.0 * (1.0 + age_ * 0.0002f));
        workingMemory->setCapacity(std::min(80, std::max(6, agedCapacity)));
    }
    
    // Age-adapted plasticity
    auto* stdp = brain->getSTDP();
    if (stdp) {
        float agedLTP = 0.006f * (1.0f + age_ * 0.0002f);
        float agedLTD = 0.018f * (1.0f + age_ * 0.001f);
        stdp->setLTPWeight(agedLTP);
        stdp->setLTDWeight(agedLTD);
        NLM_LOG_DEBUG("DevelopmentSystem: Aging STDP - LTP: " + std::to_string(agedLTP) +
                     ", LTD: " + std::to_string(agedLTD));
    }
}

void DevelopmentSystem::checkStageAdvancement(Brain* brain, TimestepDuration dt) {
    if (!brain) return;
    
    // Enhanced stage transition checking with multiple criteria
    EnhancedDevelopmentalStage stageChars = EnhancedDevelopmentalMechanisms::getStageCharacteristics(pImpl->stage);
    
    // Calculate multiple transition indicators
    double stageProgress = EnhancedDevelopmentalMechanisms::calculateStageProgress(pImpl->stageAge, stageChars.duration);
    double ageFactor = age_ / 1000.0;  // Normalized age
    double readinessScore = stageProgress * 0.6 + ageFactor * 0.3 + stageChars.learningRate * 0.1;
    
    // Stage duration criterion with adaptive threshold
    double durationThreshold = 0.8;
    if (stageChars.duration > 500.0) {
        durationThreshold = 0.7;  // Lower threshold for long stages
    } else if (stageChars.duration < 100.0) {
        durationThreshold = 0.9;  // Higher threshold for short stages
    }
    
    bool shouldAdvance = false;
    
    if (stageProgress >= durationThreshold) {
        shouldAdvance = true;
    }
    
    if (readinessScore >= 0.8) {
        shouldAdvance = true;
    }
    
    if (ageFactor > 0.9 && stageProgress > 0.7) {
        shouldAdvance = true;  // Strong preference to complete long stages
    }
    
    // Random chance for transition (especially for sensitive periods)
    if (stageChars.transitionThreshold > 0.0) {
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        if (dis(rng) < 0.1 * (1.0 - stageProgress)) {  // Higher chance when less progressed
            shouldAdvance = true;
        }
    }
    
    if (shouldAdvance) {
        advanceStage();
    }
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Calculate overall plasticity modifier based on developmental stage
    float basePlasticity = 1.0f;
    
    // Modify plasticity based on stage progress
    EnhancedDevelopmentalStage stageChars = EnhancedDevelopmentalMechanisms::getStageCharacteristics(pImpl->stage);
    double stageProgress = EnhancedDevelopmentalMechanisms::calculateStageProgress(pImpl->stageAge, stageChars.duration);
    
    if (stageProgress < 0.3) {
        // Early stage: high plasticity
        basePlasticity *= 1.5f;
    } else if (stageProgress < 0.7) {
        // Middle stage: moderate plasticity
        basePlasticity *= 1.0f;
    } else {
        // Late stage: lower plasticity
        basePlasticity *= 0.5f;
    }
    
    return basePlasticity;
}

// Enhanced development system functions
void DevelopmentSystem::updateEnhanced(DevelopmentalStage stage, double age,
                                      Brain* brain, RandomGenerator& rng,
                                      TimestepDuration dt) {
    // Update with enhanced developmental parameters
    updateEnhanced(brain, age, &rng, dt);
}

void DevelopmentSystem::updateEnhanced(Brain* brain, double age,
                                      RandomGenerator* rng,
                                      TimestepDuration dt) {
    if (!brain) return;
    
    // Store current stage
    DevelopmentalStage oldStage = pImpl->stage;
    
    // Apply stage-specific effects
    applyStageSpecificEffects(brain, oldStage, age, *rng, dt);
    
    // Update developmental state
    age_ = age;
    
    // Check for stage transitions
    checkStageTransition(brain, *rng, dt);
}

void DevelopmentSystem::applyStageSpecificEffects(Brain* brain, DevelopmentalStage oldStage,
                                                 double age, RandomGenerator& rng,
                                                 TimestepDuration dt) {
    if (!brain) return;
    
    // Apply stage-specific developmental effects
    switch (oldStage) {
        case DevelopmentalStage::Initial:
            applyInfantEffects(brain, dt);
            break;
        case DevelopmentalStage::CriticalPeriod:
            applyCriticalPeriodEffects(brain, dt);
            break;
        case DevelopmentalStage::Maturation:
            applyMaturationEffects(brain, dt);
            break;
        case DevelopmentalStage::Adult:
            applyAdultEffects(brain, dt);
            break;
        case DevelopmentalStage::Aging:
            applyAgingEffects(brain, dt);
            break;
        default:
            applyAdultEffects(brain, dt);  // Default to adult effects
            break;
    }
}

void DevelopmentSystem::checkStageTransition(Brain* brain, RandomGenerator& rng, TimestepDuration dt) {
    if (!brain) return;
    
    // Enhanced stage transition checking with multiple criteria
    EnhancedDevelopmentalStage stageChars = EnhancedDevelopmentalMechanisms::getStageCharacteristics(pImpl->stage);
    
    // Calculate multiple transition indicators
    double stageProgress = EnhancedDevelopmentalMechanisms::calculateStageProgress(pImpl->stageAge, stageChars.duration);
    double ageFactor = age_ / 1000.0;  // Normalized age
    double readinessScore = stageProgress * 0.6 + ageFactor * 0.3 + stageChars.learningRate * 0.1;
    
    // Stage duration criterion with adaptive threshold
    double durationThreshold = 0.8;
    if (stageChars.duration > 500.0) {
        durationThreshold = 0.7;  // Lower threshold for long stages
    } else if (stageChars.duration < 100.0) {
        durationThreshold = 0.9;  // Higher threshold for short stages
    }
    
    bool shouldAdvance = false;
    
    if (stageProgress >= durationThreshold) {
        shouldAdvance = true;
    }
    
    if (readinessScore >= 0.8) {
        shouldAdvance = true;
    }
    
    if (ageFactor > 0.9 && stageProgress > 0.7) {
        shouldAdvance = true;  // Strong preference to complete long stages
    }
    
    // Random chance for transition (especially for sensitive periods)
    if (stageChars.transitionThreshold > 0.0) {
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        if (dis(rng) < 0.1 * (1.0 - stageProgress)) {  // Higher chance when less progressed
            shouldAdvance = true;
        }
    }
    
    if (shouldAdvance) {
        advanceStage();
    }
}

// Public interface methods
void DevelopmentSystem::updateEnhanced(DevelopmentalStage stage, double age,
                                      Brain* brain, RandomGenerator& rng,
                                      TimestepDuration dt) {
    updateEnhanced(brain, age, &rng, dt);
}

void DevelopmentSystem::updateEnhanced(Brain* brain, double age,
                                      RandomGenerator* rng,
                                      TimestepDuration dt) {
    if (brain && rng) {
        // Store current stage
        DevelopmentalStage currentStage = pImpl->stage;
        
        // Update enhanced development
        update(brain, 0, rng, dt);
        
        // Record stage transition if needed
        if (currentStage != pImpl->stage) {
            NLM_LOG_INFO("DevelopmentSystem: Enhanced stage transition completed: " + 
                         std::string(getStageName()));
        }
    }
}

void DevelopmentSystem::recordExperience(const std::string& experienceType, double weight) {
    // Record an experience that affects development
    NLM_LOG_INFO("DevelopmentSystem: Recording experience - " + experienceType + 
                 " (weight: " + std::to_string(weight) + ")");
}

void DevelopmentSystem::updatePlasticity(Brain* brain, RandomGenerator& rng) {
    if (brain) {
        updateEnhancedPlasticityEffects(brain, rng);
    }
}

void DevelopmentSystem::applyEnhancedPlasticityEffects(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Enhanced plasticity update with additional mechanisms
    EnhancedDevelopmentalStage stageChars = EnhancedDevelopmentalMechanisms::getStageCharacteristics(pImpl->stage);
    
    double stageProgress = EnhancedDevelopmentalMechanisms::calculateStageProgress(pImpl->stageAge, stageChars.duration);
    double neuralExcitability = EnhancedDevelopmentalMechanisms::calculateNeuralExcitability(
        stageChars.excitabilityFactor, age_, stageChars);
    double learningRate = EnhancedDevelopmentalMechanisms::calculateLearningRate(1.0, stageChars);
    
    // Apply enhanced plasticity effects
    applyPlasticityEffects(brain, 0.001f, stageProgress, neuralExcitability, learningRate);
}

void DevelopmentSystem::advanceToStage(DevelopmentalStage stage) {
    setStage(stage);
}

DevelopmentalStage DevelopmentSystem::getCurrentStage() const {
    return pImpl->stage;
}

float DevelopmentSystem::getPlasticityModifier() const {
    // Calculate plasticity modifier using enhanced mechanisms
    EnhancedDevelopmentalStage stageChars = EnhancedDevelopmentalMechanisms::getStageCharacteristics(pImpl->stage);
    double stageProgress = EnhancedDevelopmentalMechanisms::calculateStageProgress(pImpl->stageAge, stageChars.duration);
    
    // Calculate enhanced plasticity modifier
    float basePlasticity = 1.0f;
    
    // Modify based on stage progress
    if (stageProgress < 0.3) {
        basePlasticity *= 1.5f;
    } else if (stageProgress < 0.7) {
        basePlasticity *= 1.0f;
    } else {
        basePlasticity *= 0.5f;
    }
    
    // Additional factors
    double ageFactor = 1.0 + age_ * 0.0001;
    basePlasticity *= ageFactor;
    
    return basePlasticity;
}

bool DevelopmentSystem::isInCriticalPeriod() const {
    return pImpl->stage == DevelopmentalStage::CriticalPeriod;
}

void DevelopmentSystem::updateAdvancedDevelopment(DevelopmentalStage stage, double age,
                                                 Brain* brain, RandomGenerator& rng,
                                                 TimestepDuration dt) {
    updateEnhanced(brain, age, &rng, dt);
}

void DevelopmentSystem::applyAdvancedDevelopmentEffects(Brain* brain, RandomGenerator& rng) {
    if (brain) {
        updateEnhancedPlasticityEffects(brain, rng);
    }
}

// Helper function to get stage characteristics from header
DevelopmentSystem::DevelopmentCharacteristics DevelopmentSystem::getStageCharacteristics(DevelopmentalStage stage) {
    // This is a simplified version for compatibility
    // The actual implementation uses EnhancedDevelopmentalMechanisms
    return DevelopmentCharacteristics();
}

} // namespace nlm